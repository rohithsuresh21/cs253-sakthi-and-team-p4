#pragma once
#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../src/common/graph.h"

inline long long pairKey(int a, int b, int n) {
    if (a > b) std::swap(a, b);
    return (long long)a * n + b;
}

inline long long randomWeight(std::mt19937_64& rng, long long maxW) {
    return 1 + (long long)(rng() % (unsigned long long)maxW);
}

inline Graph randomGraph(int n, long long m, long long maxW, bool connected, std::mt19937_64& rng, int k) {
    Graph g;
    g.n = n;
    g.k = k;
    long long total = (long long)n * (n - 1) / 2;
    if (connected && m < n - 1) m = n - 1;
    if (m > total) m = total;
    std::unordered_set<long long> used;

    if (connected) {
        std::vector<int> order(n);
        std::iota(order.begin(), order.end(), 0);
        std::shuffle(order.begin(), order.end(), rng);
        for (int i = 1; i < n; i++) {
            int j = (int)(rng() % i);
            used.insert(pairKey(order[i], order[j], n));
            g.edges.push_back({order[i], order[j], randomWeight(rng, maxW)});
        }
    }

    long long need = m - (long long)g.edges.size();
    if (need > 0 && total <= 3 * m) {
        std::vector<std::pair<int, int>> all;
        for (int a = 0; a < n; a++) {
            for (int b = a + 1; b < n; b++) {
                if (used.count(pairKey(a, b, n)) == 0) all.push_back({a, b});
            }
        }
        std::shuffle(all.begin(), all.end(), rng);
        for (long long i = 0; i < need; i++) {
            g.edges.push_back({all[i].first, all[i].second, randomWeight(rng, maxW)});
        }
    } else {
        while (need > 0) {
            int a = (int)(rng() % n);
            int b = (int)(rng() % n);
            if (a == b) continue;
            if (used.count(pairKey(a, b, n))) continue;
            used.insert(pairKey(a, b, n));
            g.edges.push_back({a, b, randomWeight(rng, maxW)});
            need--;
        }
    }
    return g;
}

inline Graph randomDisconnected(int n, long long m, long long maxW, std::mt19937_64& rng, int k) {
    Graph g;
    g.n = n;
    g.k = k;
    int half = n / 2;
    long long sameHalf = (long long)half * (half - 1) / 2 + (long long)(n - half) * (n - half - 1) / 2;
    if (m > sameHalf) m = sameHalf;
    if (sameHalf <= 3 * m) {
        std::vector<std::pair<int, int>> all;
        for (int a = 0; a < n; a++) {
            for (int b = a + 1; b < n; b++) {
                if ((a < half) == (b < half)) all.push_back({a, b});
            }
        }
        std::shuffle(all.begin(), all.end(), rng);
        for (long long i = 0; i < m; i++) {
            g.edges.push_back({all[i].first, all[i].second, randomWeight(rng, maxW)});
        }
    } else {
        std::unordered_set<long long> used;
        long long need = m;
        while (need > 0) {
            int a = (int)(rng() % n);
            int b = (int)(rng() % n);
            if (a == b) continue;
            if ((a < half) != (b < half)) continue;
            if (used.count(pairKey(a, b, n))) continue;
            used.insert(pairKey(a, b, n));
            g.edges.push_back({a, b, randomWeight(rng, maxW)});
            need--;
        }
    }
    return g;
}

inline Graph chainGraph(int n, long long maxW, std::mt19937_64& rng, int k) {
    Graph g;
    g.n = n;
    g.k = k;
    for (int i = 0; i + 1 < n; i++) g.edges.push_back({i, i + 1, randomWeight(rng, maxW)});
    return g;
}

inline Graph starGraph(int n, long long maxW, std::mt19937_64& rng, int k) {
    Graph g;
    g.n = n;
    g.k = k;
    for (int i = 1; i < n; i++) g.edges.push_back({0, i, randomWeight(rng, maxW)});
    return g;
}

inline Graph completeEqualGraph(int n, long long w, int k) {
    Graph g;
    g.n = n;
    g.k = k;
    for (int a = 0; a < n; a++) {
        for (int b = a + 1; b < n; b++) g.edges.push_back({a, b, w});
    }
    return g;
}
