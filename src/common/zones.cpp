#pragma once
#include <vector>
#include "graph.cpp"
#include "../structA/heapsort.cpp"

inline long long zoneSpacing(const MSTResult& r, int n, int k) {
    std::vector<Edge> sorted = r.edges;
    heapSort(sorted);
    return sorted[n - k].w;
}
