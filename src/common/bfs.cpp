#pragma once
#include <vector>
#include "graph.cpp"

inline bool isConnected(const Graph& g) {
    std::vector<std::vector<int>> next(g.n);
    for (const Edge& e : g.edges) {
        next[e.u].push_back(e.v);
        next[e.v].push_back(e.u);
    }
    std::vector<bool> seen(g.n, false);
    std::vector<int> queue;
    queue.push_back(0);
    seen[0] = true;
    for (size_t i = 0; i < queue.size(); i++) {
        int x = queue[i];
        for (int y : next[x]) {
            if (!seen[y]) {
                seen[y] = true;
                queue.push_back(y);
            }
        }
    }
    return static_cast<int>(queue.size()) == g.n;
}
