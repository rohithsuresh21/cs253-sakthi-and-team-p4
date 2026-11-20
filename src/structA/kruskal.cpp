#pragma once
#include <vector>
#include "../common/graph.cpp"
#include "heapsort.cpp"
#include "unionfind.cpp"


inline MSTResult kruskal(const Graph& g, bool compress = true) {
    std::vector<Edge> sorted = g.edges;
    heapSort(sorted);
    UnionFind groups(g.n, compress);
    MSTResult result;
