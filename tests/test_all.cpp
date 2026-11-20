#include <algorithm>
#include <climits>
#include <cstdio>
#include <numeric>
#include <random>
#include <vector>
#include "randomgraph.h"
#include "../src/common/bfs.h"
#include "../src/common/zones.h"
#include "../src/structA/kruskal.h"
#include "../src/structB/prim.h"

bool certificate(const Graph& g, const MSTResult& r) {
    if ((int)r.edges.size() != g.n - 1) return false;
    long long sum = 0;
    for (const Edge& e : r.edges) sum += e.w;
    if (sum != r.total) return false;

    std::vector<std::vector<std::pair<int, long long>>> next(g.n);
    for (const Edge& e : r.edges) {
        next[e.u].push_back({e.v, e.w});
        next[e.v].push_back({e.u, e.w});
    }
    for (const Edge& e : g.edges) {
        if (e.u == e.v) continue;
        std::vector<char> seen(g.n, 0);
        std::vector<long long> best(g.n, 0);
        std::vector<int> stack;
        stack.push_back(e.u);
        seen[e.u] = 1;
        while (!stack.empty()) {
            int x = stack.back();
            stack.pop_back();
            for (auto& p : next[x]) {
                if (!seen[p.first]) {
                    seen[p.first] = 1;
                    best[p.first] = std::max(best[x], p.second);
                    stack.push_back(p.first);
                }
            }
        }
        if (!seen[e.v]) return false;
        if (e.w < best[e.v]) return false;
    }
    return true;
}

long long independentSpacing(const Graph& g, const MSTResult& r) {
    std::vector<Edge> tree = r.edges;
    std::sort(tree.begin(), tree.end(), [](const Edge& a, const Edge& b) { return a.w < b.w; });
    std::vector<int> label(g.n);
    std::iota(label.begin(), label.end(), 0);
    int keep = g.n - g.k;
    for (int i = 0; i < keep; i++) {
        int a = label[tree[i].u];
        int b = label[tree[i].v];
        if (a != b) {
            for (int x = 0; x < g.n; x++) {
                if (label[x] == b) label[x] = a;
            }
        }
    }
    long long best = LLONG_MAX;
    for (const Edge& e : g.edges) {
        if (label[e.u] != label[e.v]) best = std::min(best, e.w);
    }
    return best;
}

int total = 0;
int failed = 0;
int certified = 0;
int disconnectedCount = 0;
FILE* csv = nullptr;

void runCase(int id, const char* category, const Graph& g, bool useCertificate) {
    total++;
    bool connected = isConnected(g);
    MSTResult a = kruskal(g);
    MSTResult b = prim(g);
    bool ok = true;
    bool cert = false;
    long long spacingA = -1;
    long long spacingB = -1;

    if (a.connected != connected || b.connected != connected) ok = false;
    if (!connected) disconnectedCount++;

    if (connected && ok) {
        if (a.total != b.total) ok = false;
        spacingA = zoneSpacing(a, g.n, g.k);
        spacingB = zoneSpacing(b, g.n, g.k);
        if (spacingA != spacingB) ok = false;
        if (useCertificate) {
            cert = certificate(g, a) && certificate(g, b);
            certified++;
            if (!cert) ok = false;
            if (independentSpacing(g, a) != spacingA) ok = false;
        }
    }
    if (!ok) {
        failed++;
        printf("FAIL: case %d (%s) n=%d m=%zu k=%d\n", id, category, g.n, g.edges.size(), g.k);
    }
    if (csv) {
        fprintf(csv, "%d,%s,%d,%zu,%d,%d,%lld,%lld,%lld,%lld,%d,%d\n", id, category, g.n, g.edges.size(), g.k,
                connected ? 1 : 0, a.total, b.total, spacingA, spacingB, cert ? 1 : 0, ok ? 1 : 0);
    }
}

int main() {
    std::mt19937_64 rng(42);
    csv = fopen("results/correctness.csv", "w");
    if (csv) {
        fprintf(csv, "case,category,n,m,k,connected,kruskal_total,prim_total,kruskal_spacing,prim_spacing,certificate,pass\n");
    } else {
        printf("warning: run from the project folder so results/correctness.csv can be written\n");
    }

    int id = 0;

    for (int i = 0; i < 400; i++) {
        int n = 2 + (int)(rng() % 29);
        long long total_pairs = (long long)n * (n - 1) / 2;
        long long m = n - 1 + (long long)(rng() % (2 * n + 1));
        if (m > total_pairs) m = total_pairs;
        int k = 2 + (int)(rng() % (n - 1));
        runCase(++id, "small_connected", randomGraph(n, m, 100, true, rng, k), true);
    }
    for (int i = 0; i < 200; i++) {
        int n = 31 + (int)(rng() % 170);
        long long m = n - 1 + (long long)(rng() % (2 * n + 1));
        int k = 2 + (int)(rng() % (n - 1));
        runCase(++id, "medium_connected", randomGraph(n, m, 1000, true, rng, k), true);
    }
    for (int i = 0; i < 150; i++) {
        int n = 2 + (int)(rng() % 199);
        long long m = (long long)(rng() % (3 * n));
        int k = 2 + (int)(rng() % (n - 1));
        runCase(++id, "disconnected", randomDisconnected(n, m, 100, rng, k), true);
    }
    for (int i = 0; i < 150; i++) {
        int n = 2 + (int)(rng() % 99);
        long long m = n - 1 + (long long)(rng() % (3 * n));
        int k = 2 + (int)(rng() % (n - 1));
        runCase(++id, "equal_weights", randomGraph(n, m, 1 + (long long)(rng() % 3), true, rng, k), true);
    }
    for (int i = 0; i < 100; i++) {
        int n = 2 + (int)(rng() % 199);
        int k = 2 + (int)(rng() % (n - 1));
        runCase(++id, "tree", randomGraph(n, n - 1, 1000, true, rng, k), true);
    }
    for (int i = 0; i < 100; i++) {
        int n = 5 + (int)(rng() % 56);
        long long total_pairs = (long long)n * (n - 1) / 2;
        int k = 2 + (int)(rng() % (n - 1));
        runCase(++id, "dense", randomGraph(n, total_pairs - (long long)(rng() % 5), 1000, true, rng, k), true);
    }
    for (int i = 0; i < 100; i++) {
        int n = 3 + (int)(rng() % 98);
        int choice = i % 3;
        int k = (choice == 0) ? 2 : (choice == 1 ? n : n - 1);
        runCase(++id, "special_k", randomGraph(n, 3 * n, 1000, true, rng, k), true);
    }

    for (int i = 0; i < 5; i++) {
        int n = 20000;
        int k = 2 + (int)(rng() % 50);
        runCase(++id, "large_connected", randomGraph(n, 3 * n, 1000000, true, rng, k), false);
    }
    for (int i = 0; i < 2; i++) {
        int n = 20000;
        runCase(++id, "large_disconnected", randomDisconnected(n, 3 * n, 1000000, rng, 5), false);
    }

    if (csv) fclose(csv);
    printf("cases: %d, certified small graphs: %d, disconnected: %d, failed: %d\n", total, certified, disconnectedCount, failed);
    return failed == 0 ? 0 : 1;
}
