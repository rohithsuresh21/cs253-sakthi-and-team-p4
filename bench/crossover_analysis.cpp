
#include "adversarial_cases.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace mstbench;
namespace fs = std::filesystem;

struct DSU {
    std::vector<int> parent, sz;

    explicit DSU(int n)
        : parent(n + 1), sz(n + 1, 1) {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }

    bool unite(int u, int v) {
        u = find(u);
        v = find(v);

        if (u == v) return false;

        if (sz[u] < sz[v])
            std::swap(u, v);

        parent[v] = u;
        sz[u] += sz[v];
        return true;
    }
};

struct Answer {
    bool connected = false;
    long long mst = 0;
    long long spacing = 0;
};

Answer referenceAnswer(const Graph& graph) {
    std::vector<Edge> edges = graph.edges;

    std::sort(
        edges.begin(), edges.end(),
        [](const Edge& a, const Edge& b) {
            if (a.w != b.w) return a.w < b.w;
            if (a.u != b.u) return a.u < b.u;
            return a.v < b.v;
        }
    );

    DSU mstDSU(graph.n);
    long long mstCost = 0;
    int accepted = 0;

    for (const Edge& e : edges) {
        if (mstDSU.unite(e.u, e.v)) {
            mstCost += e.w;
            ++accepted;

            if (accepted == graph.n - 1)
                break;
        }
    }

    if (accepted != graph.n - 1)
        return {false, 0, 0};

    // The project must define k=1 spacing separately.
    // This benchmark uses k=2 throughout.
    if (graph.k < 2)
        throw std::invalid_argument(
            "Benchmark requires k >= 2"
        );

    DSU clusterDSU(graph.n);
    int components = graph.n;
    long long spacing = 0;
    bool found = false;

    for (const Edge& e : edges) {
        if (clusterDSU.find(e.u) ==
            clusterDSU.find(e.v))
            continue;

        if (components == graph.k) {
            spacing = e.w;
            found = true;
            break;
        }

        clusterDSU.unite(e.u, e.v);
        --components;
    }

    if (!found)
        throw std::runtime_error(
            "Unable to determine k-spacing"
        );

    return {true, mstCost, spacing};
}

std::string readFile(const fs::path& path) {
    std::ifstream in(path);

    if (!in)
        throw std::runtime_error("Cannot read output");

    return std::string(
        std::istreambuf_iterator<char>(in),
        std::istreambuf_iterator<char>()
    );
}

Answer parseAnswer(const std::string& output) {
    std::istringstream in(output);
    std::string first;

    if (!(in >> first))
        throw std::runtime_error("Empty output");

    if (first == "DISCONNECTED") {
        std::string extra;

        if (in >> extra)
            throw std::runtime_error(
                "Unexpected disconnected output"
            );

        return {false, 0, 0};
    }

    long long mst = 0;

    try {
        size_t consumed = 0;
        mst = std::stoll(first, &consumed);

        if (consumed != first.size())
            throw std::invalid_argument("Invalid integer");
    } catch (...) {
        throw std::runtime_error("Invalid MST cost");
    }

    long long spacing;
    std::string extra;

    if (!(in >> spacing) || (in >> extra))
        throw std::runtime_error(
            "Expected exactly two numeric outputs"
        );

    return {true, mst, spacing};
}

void verify(
    const Answer& actual,
    const Answer& expected
) {
    if (actual.connected != expected.connected)
        throw std::runtime_error(
            "Connectivity result mismatch"
        );

    if (actual.connected &&
        (actual.mst != expected.mst ||
         actual.spacing != expected.spacing)) {
        throw std::runtime_error(
            "MST cost or spacing mismatch"
        );
    }
}

std::string quoted(const std::string& path) {
    return "\"" + path + "\"";
}

double runProgram(
    const std::string& executable,
    const fs::path& input,
    const fs::path& output
) {
    std::string command =
        quoted(executable) +
        " < " + quoted(input.string()) +
        " > " + quoted(output.string());

    auto begin = std::chrono::steady_clock::now();

    int status = std::system(command.c_str());

    auto end = std::chrono::steady_clock::now();

    if (status != 0)
        throw std::runtime_error(
            "Algorithm executable failed"
        );

    return std::chrono::duration<double, std::milli>(
        end - begin
    ).count();
}

double median(std::vector<double> values) {
    std::sort(values.begin(), values.end());

    size_t n = values.size();

    if (n % 2 == 1)
        return values[n / 2];

    return (values[n / 2 - 1] + values[n / 2]) / 2;
}

void benchmark(
    const Graph& graph,
    const std::string& family,
    const std::string& prim,
    const std::string& kruskal,
    int repetitions,
    std::uint64_t seed,
    std::ofstream& csv
) {
    if (!validateGraph(graph))
        throw std::runtime_error("Invalid benchmark graph");

    fs::path input = "crossover_input.tmp";
    fs::path output = "crossover_output.tmp";

    writeGraph(graph, input.string());

    Answer expected = referenceAnswer(graph);

    std::vector<double> primTimes;
    std::vector<double> kruskalTimes;

    for (int i = 0; i < repetitions; ++i) {
        bool primFirst = (i % 2 == 0);

        for (int j = 0; j < 2; ++j) {
            bool usePrim = (j == 0) == primFirst;

            const std::string& executable =
                usePrim ? prim : kruskal;

            double elapsed = runProgram(
                executable, input, output
            );

            Answer actual = parseAnswer(
                readFile(output)
            );

            verify(actual, expected);

            if (usePrim)
                primTimes.push_back(elapsed);
            else
                kruskalTimes.push_back(elapsed);
        }
    }

    double primMedianMs = median(primTimes);
    double kruskalMedianMs = median(kruskalTimes);
    double ratio = primMedianMs / kruskalMedianMs;

    long long m = graph.edges.size();

    double density =
        2.0 * m / (1.0 * graph.n * (graph.n - 1));

    csv << family << ','
        << graph.n << ','
        << m << ','
        << std::setprecision(12) << density << ','
        << seed << ','
        << repetitions << ','
        << primMedianMs << ','
        << kruskalMedianMs << ','
        << ratio << ','
        << (expected.connected
            ? std::to_string(expected.mst)
            : "DISCONNECTED")
        << ",PASS\n";

    csv.flush();

    std::cout
        << family
        << " V=" << graph.n
        << " E=" << m
        << " Prim median=" << primMedianMs << " ms"
        << " Kruskal median=" << kruskalMedianMs << " ms"
        << " Ratio=" << ratio << '\n';

    std::error_code ec;
    fs::remove(input, ec);
    fs::remove(output, ec);
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 5) {
        std::cerr
            << "Usage: crossover_analysis "
            << "<prim_executable> <kruskal_executable> "
            << "[repetitions] [output_csv]\n";
        return 1;
    }

    try {
        std::string prim =
            fs::absolute(argv[1]).string();

        std::string kruskal =
            fs::absolute(argv[2]).string();

        int repetitions =
            argc >= 4 ? std::stoi(argv[3]) : 7;

        if (repetitions < 1)
            throw std::invalid_argument(
                "Invalid repetition count"
            );

        fs::path csvPath =
            argc >= 5
            ? fs::path(argv[4])
            : fs::path("results/crossover/results.csv");

        if (!csvPath.parent_path().empty())
            fs::create_directories(csvPath.parent_path());

        std::ofstream csv(csvPath);

        if (!csv)
            throw std::runtime_error("Cannot open CSV");

        csv << "family,n,m,density,seed,repetitions,"
               "prim_median_ms,kruskal_median_ms,ratio,mst_cost,status\n";

        constexpr std::uint64_t seed = 42;

        // Fixed vertex counts, increasing edge density.
        for (int n : {500, 1000, 2000, 5000}) {
            long long maximum =
                1LL * n * (n - 1) / 2;

            for (int factor : {3, 5, 10, 25, 50}) {
                long long m = std::min(
                    maximum, 1LL * factor * n
                );

                Graph graph = generateGraph(
                    GraphType::Random,
                    n, m, 2, seed
                );

                benchmark(
                    graph, "random",
                    prim, kruskal,
                    repetitions, seed, csv
                );
            }
        }

        // Structural adversarial cases.
        for (int n : {100, 500, 1000}) {
            for (GraphType type : {
                GraphType::Chain,
                GraphType::Star,
                GraphType::EqualWeight,
                GraphType::Bridge,
                GraphType::Disconnected
            }) {
                long long m = n - 1;

                if (type == GraphType::EqualWeight ||
                    type == GraphType::Bridge)
                    m = 5LL * n;

                if (type == GraphType::Disconnected)
                    m = n - 2;

                Graph graph = generateGraph(
                    type, n, m, 2, seed
                );

                benchmark(
                    graph, graphTypeName(type),
                    prim, kruskal,
                    repetitions, seed, csv
                );
            }
        }

        // Complete graphs at manageable sizes.
        for (int n : {100, 300, 500}) {
            long long m = 1LL * n * (n - 1) / 2;

            Graph graph = generateGraph(
                GraphType::Complete,
                n, m, 2, seed
            );

            benchmark(
                graph, "complete",
                prim, kruskal,
                repetitions, seed, csv
            );
        }

        std::cout
            << "Results saved to "
            << csvPath.string() << '\n';

    } catch (const std::exception& e) {
        std::cerr
            << "Benchmark error: "
            << e.what() << '\n';
        return 1;
    }

    return 0;
}
