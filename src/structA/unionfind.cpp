#pragma once
#include <vector>

struct UnionFind {
    std::vector<int> parent;
    std::vector<int> rnk;
    bool compress;


    UnionFind(int n, bool compressPaths = true) : parent(n), rnk(n, 0), compress(compressPaths) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }


    int find(int x) {
        int root = x;
        while (parent[root] != root) root = parent[root];
        if (compress) {
            while (x != root) {
                int next = parent[x];
                parent[x] = root;
                x = next;
            }
        }
        return root;
    }
