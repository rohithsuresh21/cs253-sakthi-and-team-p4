#pragma once
#include <cmath>
#include <cstdio>
#include <random>
#include <vector>
#include "../src/common/graph.h"

inline void generateCampus(int n, double radius, int k, unsigned long long seed) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> pick(0.0, 1000.0);
    std::vector<double> x(n), y(n);
    for (int i = 0; i < n; i++) {
        x[i] = pick(rng);
        y[i] = pick(rng);
    }
    std::vector<Edge> edges;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = x[i] - x[j];
            double dy = y[i] - y[j];
            double d2 = dx * dx + dy * dy;
            if (d2 <= radius * radius) {
                long long w = std::llround(std::sqrt(d2) * 1000.0);
                if (w < 1) w = 1;
                edges.push_back({i, j, w});
            }
        }
    }
    printf("%d %zu %d\n", n, edges.size(), k);
    for (const Edge& e : edges) {
        printf("%d %d %lld\n", e.u + 1, e.v + 1, e.w);
    }
}
