#include <iostream>
#include <string>
#include "../src/common/graph.cpp"
#include "../src/common/bfs.cpp"
#include "../src/common/zones.cpp"
#include "../src/structA/kruskal.cpp"
#include "../src/structB/prim.cpp"
#include "generator.cpp"

using namespace std;

int main(int argc, char** argv) {
    char structure = '\0';
    bool compress = true;
    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        if (a == "--structure" && i + 1 < argc) {
            structure = argv[++i][0];
        } else if (a == "--no-compress") {
            compress = false;
        } else if (a == "--generate" && i + 4 < argc) {
            int n = stoi(argv[i + 1]);
            double radius = stod(argv[i + 2]);
            int k = stoi(argv[i + 3]);
            unsigned long long seed = stoull(argv[i + 4]);
            generateCampus(n, radius, k, seed);
            return 0;
        }
    }
    if (structure == '\0') {
    cerr << "Choose A (Kruskal) or B (Prim): ";
    cin >> structure;
    }

    Graph g;
    if (!readGraph(cin, g)) {
        cerr << "bad input\n";
        return 1;
    }
    if (!isConnected(g)) {
        cout << "DISCONNECTED\n";
        return 0;
    }

    MSTResult r = (structure == 'A') ? kruskal(g, compress) : prim(g);
    cout << r.total << '\n';
    cout << zoneSpacing(r, g.n, g.k) << '\n';
    return 0;
}
