#include <algorithm>
#include <chrono>
#include <cstdio>
#include <random>
#include <vector>
#include "../tests/randomgraph.cpp"
#include "../src/structA/kruskal.cpp"
#include "../src/structB/prim.cpp"

long long sink = 0;

double timeKruskal(const Graph& g, bool compress) {
    auto start = std::chrono::steady_clock::now();
    MSTResult r = kruskal(g, compress);
    auto stop = std::chrono::steady_clock::now();
    sink += r.total;
    return std::chrono::duration<double, std::milli>(stop - start).count();
}
double timePrim(const Graph& g) {
    auto start = std::chrono::steady_clock::now();
    MSTResult r = prim(g);
    auto stop = std::chrono::steady_clock::now();
    sink += r.total;
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

double median(std::vector<double> v) {
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

int main() {
    const int repeats = 5;
    std::mt19937_64 rng(42);

    FILE* raw = fopen("results/timings_raw.csv", "w");
    FILE* summary = fopen("results/timings_summary.csv", "w");
    FILE* uf = fopen("results/union_find.csv", "w");
    FILE* heap = fopen("results/prim_heap.csv", "w");
    if (!raw || !summary || !uf || !heap) {
        printf("run this from the project folder (results/ must exist)\n");
        return 1;
    }
    fprintf(raw, "n,m,edges_per_vertex,repeat,algorithm,ms\n");
    fprintf(summary, "n,m,edges_per_vertex,kruskal_median_ms,prim_median_ms,faster\n");
    fprintf(uf, "n,m,with_compression_median_ms,without_compression_median_ms\n");
    fprintf(heap, "n,m,edges_per_vertex,max_heap_size,pushes,pops,stale_pops\n");

    int sizes[] = {200, 500, 1000, 2000};
    int multipliers[] = {3, 5, 10, 25, 50, 0};
