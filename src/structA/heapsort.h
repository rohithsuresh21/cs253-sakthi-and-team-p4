#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "../common/graph.h"

inline void siftDown(std::vector<Edge>& edges, std::size_t root, std::size_t end) {
    while (true) {
        const std::size_t left = 2 * root + 1;
        if (left >= end) return;
        const std::size_t right = left + 1;
        std::size_t largest = root;
        if (edges[left].w > edges[largest].w) largest = left;
        if (right < end && edges[right].w > edges[largest].w) largest = right;
        if (largest == root) return;
        std::swap(edges[root], edges[largest]);
        root = largest;
    }
}

inline void heapSort(std::vector<Edge>& edges) {
    const std::size_t n = edges.size();
    for (std::size_t start = n / 2; start > 0; --start) {
        siftDown(edges, start - 1, n);
    }
    for (std::size_t end = n; end > 1; --end) {
        std::swap(edges[0], edges[end - 1]);
        siftDown(edges, 0, end - 1);
    }
}
