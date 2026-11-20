#pragma once

#include <stdexcept>
#include <vector>

#include "../structA/heapsort.h"

inline long long zoneSpacing(const MSTResult& mst, int n, int k) {
    if (n < 2 || k < 2 || k > n || !mst.connected ||
        static_cast<int>(mst.edges.size()) != n - 1) {
        throw std::invalid_argument("zoneSpacing requires a connected MST");
    }
    std::vector<Edge> sorted = mst.edges;
    heapSort(sorted);
    return sorted[static_cast<std::size_t>(n - k)].w;
}
