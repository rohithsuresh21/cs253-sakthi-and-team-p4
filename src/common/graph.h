#pragma once

#include <istream>
#include <utility>
#include <vector>

struct Edge {
    int u = 0;
    int v = 0;
    long long w = 0;
};

struct Graph {
    int n = 0;
    int k = 0;
    std::vector<Edge> edges;
};

struct MSTResult {
    bool connected = false;
    long long total = 0;
    std::vector<Edge> edges;
    long long maxHeap = 0;
    long long pushes = 0;
    long long pops = 0;
    long long stalePops = 0;
};

inline bool readGraph(std::istream& in, Graph& graph) {
    int n = 0;
    int m = 0;
    int k = 0;
    if (!(in >> n >> m >> k) || n < 2 || m < 0 || k < 2 || k > n) {
        return false;
    }

    Graph candidate;
    candidate.n = n;
    candidate.k = k;
    candidate.edges.reserve(static_cast<std::size_t>(m));
    for (int i = 0; i < m; ++i) {
        int u = 0;
        int v = 0;
        long long w = 0;
        if (!(in >> u >> v >> w) || u < 1 || u > n || v < 1 || v > n) {
            return false;
        }
        candidate.edges.push_back({u - 1, v - 1, w});
    }
    graph = std::move(candidate);
    return true;
}
