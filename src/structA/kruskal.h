#pragma once
#include <vector>
#include "../common/graph.h"
#include "heapsort.h"
#include "unionfind.h"

inline MSTResult kruskal(const Graph& g, bool compress = true) {
    std::vector<Edge> sorted = g.edges;
    heapSort(sorted);
    UnionFind groups(g.n, compress);
    MSTResult result;
    for (const Edge& e : sorted) {
        if (groups.join(e.u, e.v)) {
            result.edges.push_back(e);
            result.total += e.w;
            if ((int)result.edges.size() == g.n - 1) break;
        }
    }
    result.connected = ((int)result.edges.size() == g.n - 1);
    return result;
}
