#pragma once
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include "../src/common/graph.cpp"

using namespace std;

inline void generateCampus(int n, double radius, int k, unsigned long long seed) {
    mt19937_64 rng(seed);
    uniform_real_distribution<double> pick(0.0, 1000.0);
    vector<double> x(n), y(n);
    for (int i = 0; i < n; i++) {
        x[i] = pick(rng);
        y[i] = pick(rng);
    }
    vector<Edge> edges;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = x[i] - x[j];
            double dy = y[i] - y[j];
            double d2 = dx * dx + dy * dy;
            if (d2 <= radius * radius) {
                long long w = llround(sqrt(d2) * 1000.0);
                if (w < 1) w = 1;
                edges.push_back({i, j, w});
            }
        }
    }
    cout << n << ' ' << edges.size() << ' ' << k << '\n';
    for (const Edge& e : edges) {
        cout << e.u + 1 << ' ' << e.v + 1 << ' ' << e.w << '\n';
    }
}
