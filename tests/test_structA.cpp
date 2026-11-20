#include <algorithm>
#include <cstdio>
#include <random>
#include <vector>
#include "../src/common/zones.h"
#include "../src/structA/kruskal.h"

int passed = 0;
int failed = 0;

void check(bool ok, const char* name) {
    if (ok) {
        passed++;
    } else {
        failed++;
        printf("FAIL: %s\n", name);
    }
}

int main() {
    std::mt19937_64 rng(1);

    for (int t = 0; t < 200; t++) {
        int m = (int)(rng() % 50);
        std::vector<Edge> a(m);
        for (Edge& e : a) {
            e.u = 0;
            e.v = 1;
            e.w = (long long)(rng() % 20);
        }
        std::vector<Edge> b = a;
        heapSort(a);
        std::sort(b.begin(), b.end(), [](const Edge& x, const Edge& y) { return x.w < y.w; });
        bool same = true;
        for (int i = 0; i < m; i++) {
            if (a[i].w != b[i].w) same = false;
        }
        check(same, "heapsort gives sorted order");
    }

    UnionFind uf(5);
    check(uf.find(0) != uf.find(1), "start in different groups");
    check(uf.join(0, 1), "join different groups");
    check(!uf.join(1, 0), "join same group returns false");
    uf.join(1, 2);
    check(uf.find(0) == uf.find(2), "joined groups share root");
    check(uf.find(3) != uf.find(0), "other group stays separate");

    Graph g;
    g.n = 5;
    g.k = 2;
    g.edges = {{0, 1, 2}, {0, 2, 6}, {1, 2, 3}, {1, 3, 5}, {2, 3, 4}, {2, 4, 8}, {3, 4, 7}};
    MSTResult r = kruskal(g);
    check(r.connected, "sample is connected");
    check(r.total == 16, "sample total is 16");
    check(r.edges.size() == 4, "sample has 4 tree edges");
    check(zoneSpacing(r, g.n, g.k) == 7, "sample spacing is 7");

    MSTResult noCompress = kruskal(g, false);
    check(noCompress.total == 16, "no compression gives same total");

    Graph d;
    d.n = 4;
    d.k = 2;
    d.edges = {{0, 1, 5}, {2, 3, 7}};
    check(!kruskal(d).connected, "disconnected detected");

    Graph ties;
    ties.n = 4;
    ties.k = 2;
    ties.edges = {{0, 1, 1}, {1, 2, 1}, {2, 3, 1}, {0, 3, 1}, {0, 2, 1}};
    MSTResult tr = kruskal(ties);
    check(tr.connected && tr.total == 3, "all equal weights gives total 3");

    printf("structA tests: passed %d, failed %d\n", passed, failed);
    return failed == 0 ? 0 : 1;
}
