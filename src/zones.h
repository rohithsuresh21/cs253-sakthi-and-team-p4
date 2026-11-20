#pragma once
#include <vector>
#include "graph.h"
#include "../structA/heapsort.h"

inline long long zoneSpacing(const MSTResult& r, int n, int k) {
    std::vector<Edge> sorted = r.edges;
    heapSort(sorted);
    return sorted[n - k].w;
}
