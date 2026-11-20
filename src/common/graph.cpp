#pragma once
#include <istream>
#include <vector>

struct Edge {
    int u;
    int v;
    long long w;
};

struct Graph {
    int n;
    int k;
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

inline bool readGraph(std::istream& in, Graph& g) {
    int m;
    if (!(in >> g.n >> m >> g.k)) return false;
    if (g.n < 2 || g.k < 2 || g.k > g.n || m < 0) return false;
    g.edges.resize(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        if (!(in >> u >> v >> w)) return false;
        if (u < 1 || u > g.n || v < 1 || v > g.n) return false;
        g.edges[i].u = u - 1;
        g.edges[i].v = v - 1;
        g.edges[i].w = w;
    }
    return true;
}
