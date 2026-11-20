#pragma once

#include <vector>

#include "../common/graph.h"
#include "minheap.h"

struct Arc {
    int to = 0;
    long long w = 0;
};

inline MSTResult prim(const Graph& graph) {
    MSTResult result;
    if (graph.n < 2) return result;
    std::vector<std::vector<Arc>> adjacency(static_cast<std::size_t>(graph.n));
    for (const Edge& edge : graph.edges) {
        if (edge.u < 0 || edge.u >= graph.n || edge.v < 0 || edge.v >= graph.n) {
            return MSTResult{};
        }
        adjacency[edge.u].push_back({edge.v, edge.w});
        adjacency[edge.v].push_back({edge.u, edge.w});
    }

    std::vector<char> visited(static_cast<std::size_t>(graph.n), 0);
    MinHeap heap;
    visited[0] = 1;
    for (const Arc& arc : adjacency[0]) {
        if (!visited[arc.to]) {
            heap.push({arc.w, 0, arc.to});
            ++result.pushes;
        }
    }
    result.maxHeap = static_cast<long long>(heap.size());

    while (!heap.empty() && static_cast<int>(result.edges.size()) < graph.n - 1) {
        const HeapItem item = heap.pop();
        ++result.pops;
        if (visited[item.to]) {
            ++result.stalePops;
            continue;
        }
        visited[item.to] = 1;
        result.edges.push_back({item.from, item.to, item.w});
        result.total += item.w;
        for (const Arc& arc : adjacency[item.to]) {
            if (!visited[arc.to]) {
                heap.push({arc.w, item.to, arc.to});
                ++result.pushes;
                if (static_cast<long long>(heap.size()) > result.maxHeap) {
                    result.maxHeap = static_cast<long long>(heap.size());
                }
            }
        }
    }
    result.connected = static_cast<int>(result.edges.size()) == graph.n - 1;
    return result;
}
