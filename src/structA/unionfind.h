#pragma once

#include <stdexcept>
#include <vector>

struct UnionFind {
    std::vector<int> parent;
    std::vector<int> rank;
    bool compress;

    explicit UnionFind(int n, bool compressPaths = true)
        : parent(static_cast<std::size_t>(n)),
          rank(static_cast<std::size_t>(n), 0),
          compress(compressPaths) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int find(int x) {
        if (x < 0 || static_cast<std::size_t>(x) >= parent.size()) {
            throw std::out_of_range("UnionFind index out of range");
        }
        int root = x;
        while (parent[root] != root) root = parent[root];
        if (compress) {
            while (x != root) {
                const int next = parent[x];
                parent[x] = root;
                x = next;
            }
        }
        return root;
    }

    bool join(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);
        if (rootA == rootB) return false;
        if (rank[rootA] < rank[rootB]) {
            parent[rootA] = rootB;
        } else if (rank[rootA] > rank[rootB]) {
            parent[rootB] = rootA;
        } else {
            parent[rootB] = rootA;
            ++rank[rootA];
        }
        return true;
    }
};
