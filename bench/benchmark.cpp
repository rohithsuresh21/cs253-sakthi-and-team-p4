#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "../src/structA/kruskal.h"
#include "../src/structB/prim.h"
#include "../tests/randomgraph.h"

namespace {

long long resultSink = 0;

double timeKruskal(const Graph& graph, bool compress) {
    const auto start = std::chrono::steady_clock::now();
    const MSTResult result = kruskal(graph, compress);
    const auto stop = std::chrono::steady_clock::now();
    resultSink += result.total;
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

double timePrim(const Graph& graph) {
    const auto start = std::chrono::steady_clock::now();
    const MSTResult result = prim(graph);
    const auto stop = std::chrono::steady_clock::now();
    resultSink += result.total;
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

double medianOfFive(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    return values[2];
}

}

int main() {
    constexpr int repetitions = 5;
    constexpr std::uint64_t seed = 42;
    FILE* raw = std::fopen("results/timings_raw.csv", "w");
    FILE* summary = std::fopen("results/timings_summary.csv", "w");
    FILE* unionFind = std::fopen("results/union_find.csv", "w");
    FILE* unionFindRaw = std::fopen("results/union_find_raw.csv", "w");
    FILE* heap = std::fopen("results/prim_heap.csv", "w");

    if (!raw || !summary || !unionFind || !unionFindRaw || !heap) {
        std::cerr << "Could not open result files; run from repository root\n";
        if (raw) std::fclose(raw);
        if (summary) std::fclose(summary);
        if (unionFind) std::fclose(unionFind);
        if (unionFindRaw) std::fclose(unionFindRaw);
        if (heap) std::fclose(heap);
        return 1;
    }

    std::fprintf(raw, "seed,n,m,edges_per_vertex,repeat,algorithm,ms\n");
    std::fprintf(summary, "seed,n,m,edges_per_vertex,repetitions,kruskal_median_ms,prim_median_ms,faster\n");
    std::fprintf(unionFind, "seed,n,m,repetitions,with_compression_median_ms,without_compression_median_ms\n");
    std::fprintf(unionFindRaw, "seed,n,m,repeat,compression,ms\n");
    std::fprintf(heap, "seed,n,m,edges_per_vertex,max_heap_size,pushes,pops,stale_pops\n");

    std::mt19937_64 rng(seed);
    const int sizes[] = {200, 500, 1000, 2000};
    const int multipliers[] = {3, 5, 10, 25, 50, 0};

    for (int n : sizes) {
        const long long maximum = 1LL * n * (n - 1) / 2;
        long long previous = -1;
        for (int multiplier : multipliers) {
            const long long m = multiplier == 0
                ? maximum
                : std::min(maximum, 1LL * multiplier * n);
            if (m == previous) continue;
            previous = m;

            const Graph graph = randomGraph(n, m, 1000000, true, rng, 2);
            std::vector<double> kruskalTimes;
            std::vector<double> primTimes;
            (void)timeKruskal(graph, true);
            (void)timePrim(graph);

            for (int repeat = 0; repeat < repetitions; ++repeat) {
                const double kTime = timeKruskal(graph, true);
                const double pTime = timePrim(graph);
                kruskalTimes.push_back(kTime);
                primTimes.push_back(pTime);
                const double density = static_cast<double>(graph.edges.size()) / n;
                std::fprintf(raw, "%llu,%d,%zu,%.4f,%d,kruskal,%.6f\n",
                             static_cast<unsigned long long>(seed), n,
                             graph.edges.size(), density, repeat, kTime);
                std::fprintf(raw, "%llu,%d,%zu,%.4f,%d,prim,%.6f\n",
                             static_cast<unsigned long long>(seed), n,
                             graph.edges.size(), density, repeat, pTime);
            }

            const double kMedian = medianOfFive(kruskalTimes);
            const double pMedian = medianOfFive(primTimes);
            const char* faster = pMedian < kMedian ? "prim" : kMedian < pMedian ? "kruskal" : "tie";
            std::fprintf(summary, "%llu,%d,%zu,%.4f,%d,%.6f,%.6f,%s\n",
                         static_cast<unsigned long long>(seed), n,
                         graph.edges.size(),
                         static_cast<double>(graph.edges.size()) / n,
                         repetitions, kMedian, pMedian, faster);

            const MSTResult primResult = prim(graph);
            std::fprintf(heap, "%llu,%d,%zu,%.4f,%lld,%lld,%lld,%lld\n",
                         static_cast<unsigned long long>(seed), n,
                         graph.edges.size(),
                         static_cast<double>(graph.edges.size()) / n,
                         primResult.maxHeap, primResult.pushes,
                         primResult.pops, primResult.stalePops);
        }
    }

    const int cases[][2] = {{2000, 10}, {20000, 3}, {20000, 10}, {50000, 5}};
    for (const auto& item : cases) {
        const int n = item[0];
        const Graph graph = randomGraph(n, 1LL * item[1] * n, 1000000, true, rng, 2);
        std::vector<double> withCompression;
        std::vector<double> withoutCompression;
        for (int repeat = 0; repeat < repetitions; ++repeat) {
            const double withTime = timeKruskal(graph, true);
            const double withoutTime = timeKruskal(graph, false);
            withCompression.push_back(withTime);
            withoutCompression.push_back(withoutTime);
            std::fprintf(unionFindRaw, "%llu,%d,%zu,%d,on,%.6f\n",
                         static_cast<unsigned long long>(seed), n, graph.edges.size(), repeat, withTime);
            std::fprintf(unionFindRaw, "%llu,%d,%zu,%d,off,%.6f\n",
                         static_cast<unsigned long long>(seed), n, graph.edges.size(), repeat, withoutTime);
        }
        std::fprintf(unionFind, "%llu,%d,%zu,%d,%.6f,%.6f\n",
                     static_cast<unsigned long long>(seed), n,
                     graph.edges.size(), repetitions,
                     medianOfFive(withCompression), medianOfFive(withoutCompression));
    }

    std::fclose(raw);
    std::fclose(summary);
    std::fclose(unionFind);
    std::fclose(unionFindRaw);
    std::fclose(heap);
    std::cout << "Benchmark complete: " << resultSink << '\n';
    return 0;
}
