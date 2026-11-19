#include <cstdio>
#include <cstdlib>
#include <string>
#include "../src/common/graph.h"
#include "../src/common/bfs.h"
#include "../src/common/zones.h"
#include "../src/structA/kruskal.h"
#include "../src/structB/prim.h"
#include "generator.h"

int main(int argc, char** argv) {
    char structure = 'A';
    bool compress = true;
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--structure" && i + 1 < argc) {
            structure = argv[++i][0];
        } else if (a == "--no-compress") {
            compress = false;
        } else if (a == "--generate" && i + 4 < argc) {
            int n = atoi(argv[i + 1]);
            double radius = atof(argv[i + 2]);
            int k = atoi(argv[i + 3]);
            unsigned long long seed = strtoull(argv[i + 4], nullptr, 10);
            generateCampus(n, radius, k, seed);
            return 0;
        }
    }
    if (structure != 'A' && structure != 'B') {
        fprintf(stderr, "use: run --structure A|B\n");
        return 1;
    }

    Graph g;
    if (!readGraph(stdin, g)) {
        fprintf(stderr, "bad input\n");
        return 1;
