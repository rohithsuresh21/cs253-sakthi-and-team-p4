#include <algorithm>
#include <cstdio>
#include <random>
#include <vector>
#include "../src/common/zones.h"
#include "../src/structB/prim.h"

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
    std::mt19937_64 rng(2);

    MinHeap empty;
    check(empty.empty(), "new heap is empty");

    MinHeap single;
    single.push({5, 0, 1});
    check(single.size() == 1, "one item in heap");
    check(single.pop().w == 5, "pop returns the only item");
    check(single.empty(), "heap empty after pop");

    for (int t = 0; t < 200; t++) {
        int m = 1 + (int)(rng() % 60);
        MinHeap heap;
        std::vector<long long> weights;
        for (int i = 0; i < m; i++) {
            long long w = (long long)(rng() % 15);
            weights.push_back(w);
            heap.push({w, 0, 1});
        }
        std::sort(weights.begin(), weights.end());
        bool same = true;
        for (int i = 0; i < m; i++) {
            if (heap.pop().w != weights[i]) same = false;
        }
        check(same, "heap pops in sorted order");
    }

    Graph g;
    g.n = 5;
    g.k = 2;
    g.edges = {{0, 1, 2}, {0, 2, 6}, {1, 2, 3}, {1, 3, 5}, {2, 3, 4}, {2, 4, 8}, {3, 4, 7}};
    MSTResult r = prim(g);
    check(r.connected, "sample is connected");
    check(r.total == 16, "sample total is 16");
    check(r.edges.size() == 4, "sample has 4 tree edges");
    check(zoneSpacing(r, g.n, g.k) == 7, "sample spacing is 7");
    check(r.maxHeap > 0, "max heap size is recorded");
    check(r.pops <= r.pushes, "pops never exceed pushes");

    Graph d;
    d.n = 4;
    d.k = 2;
    d.edges = {{0, 1, 5}, {2, 3, 7}};
    check(!prim(d).connected, "disconnected detected");

    printf("structB tests: passed %d, failed %d\n", passed, failed);
    return failed == 0 ? 0 : 1;
}
