#include <algorithm>
#include <chrono>
#include <cstdio>
#include <random>
#include <vector>
#include "../tests/randomgraph.cpp"
#include "../src/structA/kruskal.cpp"
#include "../src/structB/prim.cpp"

long long sink = 0;

double timeKruskal(const Graph& g, bool compress) {
    auto start = std::chrono::steady_clock::now();
    MSTResult r = kruskal(g, compress);
    auto stop = std::chrono::steady_clock::now();
    sink += r.total;
    return std::chrono::duration<double, std::milli>(stop - start).count();
}
