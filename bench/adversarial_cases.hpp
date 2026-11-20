
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace mstbench {

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

enum class GraphType {
    Chain,
    Star,
    Random,
    EqualWeight,
    Bridge,
    Disconnected,
    Complete
};

Graph generateGraph(
    GraphType type,
    int n,
    long long m,
    int k = 2,
    std::uint64_t seed = 42
);

bool validateGraph(const Graph& graph);
bool isConnected(const Graph& graph);

std::string graphToString(const Graph& graph);

void writeGraph(
    const Graph& graph,
    const std::string& filename
);

GraphType parseGraphType(const std::string& name);
std::string graphTypeName(GraphType type);

} 
