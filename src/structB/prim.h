#pragma once
#include <vector>
#include "../common/graph.h"
#include "minheap.h"

struct Arc {
    int to;
    long long w;
};

inline MSTResult prim(const Graph& g) {
    std::vector<std::vector<Arc>> next(g.n);
    for (const Edge& e : g.edges) {
        next[e.u].push_back({e.v, e.w});
        next[e.v].push_back({e.u, e.w});
    }
    std::vector<bool> visited(g.n, false);
    MinHeap heap;
    MSTResult result;

    visited[0] = true;
    for (const Arc& a : next[0]) {
        heap.push({a.w, 0, a.to});
        result.pushes++;
        if (heap.size() > result.maxHeap) result.maxHeap = heap.size();
    }

    while (!heap.empty() && (int)result.edges.size() < g.n - 1) {
        HeapItem top = heap.pop();
        result.pops++;
        if (visited[top.to]) {
            result.stalePops++;
            continue;
        }
        visited[top.to] = true;
        result.edges.push_back({top.from, top.to, top.w});
        result.total += top.w;
        for (const Arc& a : next[top.to]) {
            if (!visited[a.to]) {
                heap.push({a.w, top.to, a.to});
                result.pushes++;
                if (heap.size() > result.maxHeap) result.maxHeap = heap.size();
            }
        }
    }
    result.connected = ((int)result.edges.size() == g.n - 1);
    return result;
}
