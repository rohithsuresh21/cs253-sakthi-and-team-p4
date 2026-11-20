#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>
#include "../src/common/graph.h"

using namespace std;

void generateCampus(int n, double radius, int k, unsigned long long seed) {
    mt19937_64 rng(seed);
    uniform_real_distribution<double> pick(0.0, 1000.0);
    vector<double> x(n), y(n);
    for (int i = 0; i < n; i++) {
        x[i] = pick(rng);
        y[i] = pick(rng);
    }
    vector<Edge> edges;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = x[i] - x[j];
            double dy = y[i] - y[j];
            double d2 = dx * dx + dy * dy;
            if (d2 <= radius * radius) {
                long long w = llround(sqrt(d2) * 1000.0);
                if (w < 1) w = 1;
                edges.push_back({i, j, w});
            }
        }
    }
    printf("%d %zu %d\n", n, edges.size(), k);
    for (const Edge& e : edges) {
        printf("%d %d %lld\n", e.u + 1, e.v + 1, e.w);
    }
}

// Usage: ./generator n radius k seed > input.txt
int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s n radius k seed\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    double radius = atof(argv[2]);
    int k = atoi(argv[3]);
    unsigned long long seed = strtoull(argv[4], nullptr, 10);

    generateCampus(n, radius, k, seed);
    return 0;
}
