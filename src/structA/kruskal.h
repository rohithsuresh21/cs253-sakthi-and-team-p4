#pragma once

#include <vector>

#include "../common/graph.h"
#include "heapsort.h"
#include "unionfind.h"

inline MSTResult kruskal(const Graph& graph, bool compress = true) {
    MSTResult result;
    if (graph.n < 2) return result;
    std::vector<Edge> sorted = graph.edges;
    heapSort(sorted);
    UnionFind groups(graph.n, compress);
    for (const Edge& edge : sorted) {
        if (edge.u < 0 || edge.u >= graph.n || edge.v < 0 || edge.v >= graph.n) {
            return MSTResult{};
        }
        if (groups.join(edge.u, edge.v)) {
            result.edges.push_back(edge);
            result.total += edge.w;
            if (static_cast<int>(result.edges.size()) == graph.n - 1) break;
        }
    }
    result.connected = static_cast<int>(result.edges.size()) == graph.n - 1;
    return result;
}
