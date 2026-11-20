#pragma once
#include <vector>

struct UnionFind {
    std::vector<int> parent;
    std::vector<int> rnk;
    bool compress;


    UnionFind(int n, bool compressPaths = true) : parent(n), rnk(n, 0), compress(compressPaths) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }
