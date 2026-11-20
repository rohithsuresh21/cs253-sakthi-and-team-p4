#pragma once

#include <vector>

#include "graph.h"

inline bool isConnected(const Graph& graph) {
    if (graph.n < 1) return false;
    std::vector<std::vector<int>> adjacency(static_cast<std::size_t>(graph.n));
    for (const Edge& edge : graph.edges) {
        if (edge.u < 0 || edge.u >= graph.n || edge.v < 0 || edge.v >= graph.n) {
            return false;
        }
        adjacency[edge.u].push_back(edge.v);
        adjacency[edge.v].push_back(edge.u);
    }

    std::vector<char> seen(static_cast<std::size_t>(graph.n), 0);
    std::vector<int> queue;
    queue.reserve(static_cast<std::size_t>(graph.n));
    seen[0] = 1;
    queue.push_back(0);
    for (std::size_t head = 0; head < queue.size(); ++head) {
        for (const int v : adjacency[queue[head]]) {
            if (!seen[v]) {
                seen[v] = 1;
                queue.push_back(v);
            }
        }
    }
    return static_cast<int>(queue.size()) == graph.n;
}
