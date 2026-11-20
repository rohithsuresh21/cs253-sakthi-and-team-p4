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
for (int n : sizes) {
        long long totalPairs = (long long)n * (n - 1) / 2;
        long long lastM = -1;
        for (int mult : multipliers) {
            long long m = (mult == 0) ? totalPairs : (long long)mult * n;
            if (m > totalPairs) m = totalPairs;
            if (m == lastM) continue;
            lastM = m;

            Graph g = randomGraph(n, m, 1000000, true, rng, 2);
            std::vector<double> kr, pr;
for (int r = 0; r < repeats; r++) {
                double a = timeKruskal(g, true);
                double b = timePrim(g);
                kr.push_back(a);
                pr.push_back(b);
                fprintf(raw, "%d,%zu,%.2f,%d,kruskal,%.4f\n", n, g.edges.size(), (double)g.edges.size() / n, r, a);
                fprintf(raw, "%d,%zu,%.2f,%d,prim,%.4f\n", n, g.edges.size(), (double)g.edges.size() / n, r, b);
            }
double km = median(kr);
            double pm = median(pr);
            fprintf(summary, "%d,%zu,%.2f,%.4f,%.4f,%s\n", n, g.edges.size(), (double)g.edges.size() / n, km, pm,
                    pm < km ? "prim" : "kruskal");
            MSTResult pr1 = prim(g);
            fprintf(heap, "%d,%zu,%.2f,%lld,%lld,%lld,%lld\n", n, g.edges.size(), (double)g.edges.size() / n, pr1.maxHeap,
                    pr1.pushes, pr1.pops, pr1.stalePops);
            printf("n=%d m=%zu kruskal=%.3f ms prim=%.3f ms\n", n, g.edges.size(), km, pm);
        }
    }
 int ufSizes[][2] = {{2000, 10}, {20000, 3}, {20000, 10}, {50000, 5}};
    for (auto& s : ufSizes) {
        int n = s[0];
        long long m = (long long)s[1] * n;
        Graph g = randomGraph(n, m, 1000000, true, rng, 2);
        std::vector<double> with, without;
        for (int r = 0; r < repeats; r++) {
            with.push_back(timeKruskal(g, true));
            without.push_back(timeKruskal(g, false));
        }
        fprintf(uf, "%d,%zu,%.4f,%.4f\n", n, g.edges.size(), median(with), median(without));
        printf("union-find n=%d m=%zu with=%.3f ms without=%.3f ms\n", n, g.edges.size(), median(with), median(without));
    }
fclose(raw);
    fclose(summary);
    fclose(uf);
    fclose(heap);
    printf("done (%lld)\n", sink % 10);
    return 0;
}
