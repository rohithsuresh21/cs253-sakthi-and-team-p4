
#include "adversarial_cases.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

using namespace mstbench;

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr
            << "Usage: generate_case "
            << "<type> <n> <m> <k> <seed> <output>\n";
        return 1;
    }

    try {
        GraphType type = parseGraphType(argv[1]);

        int n = std::stoi(argv[2]);
        long long m = std::stoll(argv[3]);
        int k = std::stoi(argv[4]);
        std::uint64_t seed = std::stoull(argv[5]);

        Graph graph = generateGraph(
            type, n, m, k, seed
        );

        if (!validateGraph(graph)) {
            std::cerr << "Generated graph is invalid\n";
            return 1;
        }

        writeGraph(graph, argv[6]);

        std::cout
            << "Type: " << graphTypeName(type) << '\n'
            << "Vertices: " << graph.n << '\n'
            << "Edges: " << graph.edges.size() << '\n'
            << "Connected: "
            << (isConnected(graph) ? "yes" : "no")
            << '\n'
            << "Output: " << argv[6] << '\n';

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
