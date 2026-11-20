#include "adversarial_cases.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "../src/common/zones.h"
#include "../src/structA/kruskal.h"
#include "../src/structB/prim.h"

struct OracleResult {
    bool connected = false;
    long long total = 0;
    long long spacing = 0;
};

long long resultSink = 0;

struct DisjointSet {
    std::vector<int> parent;
    std::vector<int> size;

    explicit DisjointSet(int n)
        : parent(static_cast<std::size_t>(n + 1)),
          size(static_cast<std::size_t>(n + 1), 1) {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }

    bool join(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (size[a] < size[b]) std::swap(a, b);
        parent[b] = a;
        size[a] += size[b];
        return true;
    }
};

OracleResult referenceAnswer(const mstbench::Graph& graph) {
    std::vector<mstbench::Edge> edges = graph.edges;
    std::sort(edges.begin(), edges.end(), [](const auto& a, const auto& b) {
        if (a.w != b.w) return a.w < b.w;
        if (a.u != b.u) return a.u < b.u;
        return a.v < b.v;
    });

    DisjointSet mstSets(graph.n);
    int accepted = 0;
    long long total = 0;
    for (const auto& edge : edges) {
        if (mstSets.join(edge.u, edge.v)) {
            total += edge.w;
            if (++accepted == graph.n - 1) break;
        }
    }
    if (accepted != graph.n - 1) return {};

    DisjointSet zones(graph.n);
    int components = graph.n;
    for (const auto& edge : edges) {
        if (zones.find(edge.u) == zones.find(edge.v)) continue;
        if (components == graph.k) return {true, total, edge.w};
        zones.join(edge.u, edge.v);
        --components;
    }
    throw std::runtime_error("Could not calculate zone spacing");
}

Graph convertGraph(const mstbench::Graph& source) {
    Graph graph;
    graph.n = source.n;
    graph.k = source.k;
    graph.edges.reserve(source.edges.size());
    for (const auto& edge : source.edges) {
        graph.edges.push_back({edge.u - 1, edge.v - 1, edge.w});
    }
    return graph;
}

void verify(const MSTResult& result, const OracleResult& expected,
            int n, int k) {
    if (result.connected != expected.connected) {
        throw std::runtime_error("Connectivity result mismatch");
    }
    if (result.connected &&
        (result.total != expected.total ||
         zoneSpacing(result, n, k) != expected.spacing)) {
        throw std::runtime_error("MST result mismatch");
    }
}

double average(const std::vector<double>& values) {
    double sum = 0.0;
    for (double value : values) sum += value;
    return values.empty() ? 0.0 : sum / values.size();
}

template <typename Function>
double measure(Function function) {
    const auto start = std::chrono::steady_clock::now();
    const MSTResult result = function();
    const auto stop = std::chrono::steady_clock::now();
    resultSink += result.total;
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

void benchmark(const mstbench::Graph& source, const std::string& family,
               int repetitions, std::uint64_t seed, std::ofstream& csv) {
    if (!mstbench::validateGraph(source)) {
        throw std::runtime_error("Invalid generated graph");
    }

    const Graph graph = convertGraph(source);
    const OracleResult expected = referenceAnswer(source);
    std::vector<double> primTimes;
    std::vector<double> kruskalTimes;
    primTimes.reserve(static_cast<std::size_t>(repetitions));
    kruskalTimes.reserve(static_cast<std::size_t>(repetitions));

    for (int repeat = 0; repeat < repetitions; ++repeat) {
        MSTResult primResult;
        MSTResult kruskalResult;
        double primMs = 0.0;
        double kruskalMs = 0.0;

        if (repeat % 2 == 0) {
            primMs = measure([&] { primResult = prim(graph); return primResult; });
            kruskalMs = measure([&] { kruskalResult = kruskal(graph); return kruskalResult; });
        } else {
            kruskalMs = measure([&] { kruskalResult = kruskal(graph); return kruskalResult; });
            primMs = measure([&] { primResult = prim(graph); return primResult; });
        }

        verify(primResult, expected, graph.n, graph.k);
        verify(kruskalResult, expected, graph.n, graph.k);
        primTimes.push_back(primMs);
        kruskalTimes.push_back(kruskalMs);
    }

    const double primAverage = average(primTimes);
    const double kruskalAverage = average(kruskalTimes);
    const double ratio = primAverage / std::max(kruskalAverage, 1e-12);
    const long long m = static_cast<long long>(source.edges.size());
    const double density = 2.0 * m / (1.0 * source.n * (source.n - 1));

    csv << family << ',' << source.n << ',' << m << ','
        << std::setprecision(12) << density << ',' << seed << ','
        << repetitions << ',' << primAverage << ',' << kruskalAverage << ','
        << ratio << ','
        << (expected.connected ? std::to_string(expected.total) : "DISCONNECTED")
        << ",PASS\n";
    csv.flush();

    std::cout << family << " V=" << source.n << " E=" << m
              << " Prim=" << primAverage << " ms"
              << " Kruskal=" << kruskalAverage << " ms"
              << " Ratio=" << ratio << '\n';
}

int main(int argc, char* argv[]) {
    if (argc < 1 || argc > 3) {
        std::cerr << "Usage: crossover_analysis [repetitions] [output_csv]\n";
        return 1;
    }

    try {
        const int repetitions = argc >= 2 ? std::stoi(argv[1]) : 5;
        if (repetitions < 1) throw std::invalid_argument("Invalid repetition count");

        const std::filesystem::path csvPath = argc >= 3
            ? std::filesystem::path(argv[2])
            : std::filesystem::path("results/crossover.csv");
        if (!csvPath.parent_path().empty()) {
            std::filesystem::create_directories(csvPath.parent_path());
        }

        std::ofstream csv(csvPath);
        if (!csv) throw std::runtime_error("Cannot open crossover CSV");
        csv << "family,n,m,density,seed,repetitions,prim_ms,kruskal_ms,ratio,mst_cost,status\n";

        constexpr std::uint64_t seed = 42;
        for (int n : {500, 1000, 2000, 5000}) {
            const long long maximum = 1LL * n * (n - 1) / 2;
            for (int factor : {3, 5, 10, 25, 50}) {
                const long long m = std::min(maximum, 1LL * factor * n);
                const auto graph = mstbench::generateGraph(
                    mstbench::GraphType::Random, n, m, 2, seed
                );
                benchmark(graph, "random", repetitions, seed, csv);
            }
        }

        for (int n : {100, 500, 1000}) {
            for (const auto type : {
                     mstbench::GraphType::Chain,
                     mstbench::GraphType::Star,
                     mstbench::GraphType::EqualWeight,
                     mstbench::GraphType::Bridge,
                     mstbench::GraphType::Disconnected}) {
                long long m = n - 1;
                if (type == mstbench::GraphType::EqualWeight ||
                    type == mstbench::GraphType::Bridge) m = 5LL * n;
                if (type == mstbench::GraphType::Disconnected) m = n - 2;
                const auto graph = mstbench::generateGraph(type, n, m, 2, seed);
                benchmark(graph, mstbench::graphTypeName(type), repetitions, seed, csv);
            }
        }

        for (int n : {100, 300, 500}) {
            const long long m = 1LL * n * (n - 1) / 2;
            const auto graph = mstbench::generateGraph(
                mstbench::GraphType::Complete, n, m, 2, seed
            );
            benchmark(graph, "complete", repetitions, seed, csv);
        }

        std::cout << "Results saved to " << csvPath.string() << '\n';
        std::cout << "Result check: " << resultSink << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Benchmark error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
