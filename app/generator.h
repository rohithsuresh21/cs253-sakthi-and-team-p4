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
