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