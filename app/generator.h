#pragma once

#include <cmath>
#include <cstdint>
#include <ostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../src/common/graph.h"

inline void generateCampus(int n, double radius, int k,
                           std::uint64_t seed, std::ostream& out) {
    if (n < 2 || n > 20000 || k < 2 || k > n ||
        !std::isfinite(radius) || radius < 0.0) {
        throw std::invalid_argument("invalid campus generation parameters");
    }
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> coordinate(0.0, 1000.0);
    std::vector<double> x(static_cast<std::size_t>(n));
    std::vector<double> y(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) {
        x[i] = coordinate(rng);
        y[i] = coordinate(rng);
    }

    std::vector<Edge> edges;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            const double dx = x[i] - x[j];
            const double dy = y[i] - y[j];
            const double distanceSquared = dx * dx + dy * dy;
            if (distanceSquared <= radius * radius) {
                long long weight = std::llround(std::sqrt(distanceSquared) * 1000.0);
                if (weight < 1) weight = 1;
                edges.push_back({i, j, weight});
            }
        }
    }

    out << n << ' ' << edges.size() << ' ' << k << '\n';
    for (const Edge& edge : edges) {
        out << edge.u + 1 << ' ' << edge.v + 1 << ' ' << edge.w << '\n';
    }
}
