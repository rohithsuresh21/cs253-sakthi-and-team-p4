#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

#include "../src/common/bfs.h"
#include "../src/common/graph.h"
#include "../src/common/zones.h"
#include "../src/structA/kruskal.h"
#include "../src/structB/prim.h"
#include "generator.h"

namespace {

void printUsage(const char* program) {
    std::cerr << "Usage:\n"
              << "  " << program << " --structure A [--no-compress] < graph.in\n"
              << "  " << program << " --structure B < graph.in\n"
              << "  " << program << " --generate <n> <radius> <k> <seed>\n";
}

}

int main(int argc, char** argv) {
    char structure = '\0';
    bool compress = true;

    try {
        for (int i = 1; i < argc; ++i) {
            const std::string argument = argv[i];
            if (argument == "--structure") {
                if (i + 1 >= argc) {
                    printUsage(argv[0]);
                    return 2;
                }
                const std::string choice = argv[++i];
                if (choice.size() != 1 || (choice[0] != 'A' && choice[0] != 'B')) {
                    printUsage(argv[0]);
                    return 2;
                }
                structure = choice[0];
            } else if (argument == "--no-compress") {
                compress = false;
            } else if (argument == "--generate") {
                if (i + 4 >= argc) {
                    printUsage(argv[0]);
                    return 2;
                }
                const int n = std::stoi(argv[++i]);
                const double radius = std::stod(argv[++i]);
                const int k = std::stoi(argv[++i]);
                const std::uint64_t seed = std::stoull(argv[++i]);
                generateCampus(n, radius, k, seed, std::cout);
                return 0;
            } else {
                printUsage(argv[0]);
                return 2;
            }
        }

        if (structure != 'A' && structure != 'B') {
            printUsage(argv[0]);
            return 2;
        }

        Graph graph;
        if (!readGraph(std::cin, graph)) {
            std::cerr << "Invalid graph input\n";
            return 2;
        }
        if (!isConnected(graph)) {
            std::cout << "DISCONNECTED\n";
            return 0;
        }

        const MSTResult mst = structure == 'A'
            ? kruskal(graph, compress)
            : prim(graph);
        if (!mst.connected) {
            std::cout << "DISCONNECTED\n";
            return 0;
        }

        std::cout << mst.total << '\n';
        std::cout << zoneSpacing(mst, graph.n, graph.k) << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 2;
    }
}
