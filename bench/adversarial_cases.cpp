
#include "adversarial_cases.hpp"

#include <algorithm>
#include <fstream>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace mstbench {
namespace {

using U64 = std::uint64_t;

long long capacity(int n) {
    return 1LL * n * (n - 1) / 2;
}

U64 edgeKey(int u, int v) {
    if (u > v) std::swap(u, v);

    return (U64(static_cast<unsigned>(u)) << 32)
         | U64(static_cast<unsigned>(v));
}

struct Builder {
    Graph graph;
    std::mt19937_64 rng;
    std::unordered_set<U64> seen;
    bool equalWeights;

    Builder(int n, int k, U64 seed, bool equal)
        : graph{n, k, {}},
          rng(seed),
          equalWeights(equal) {}

    long long weight() {
        if (equalWeights) return 1;

        return std::uniform_int_distribution<long long>(
            1, 1000000
        )(rng);
    }

    bool add(int u, int v) {
        if (u == v) return false;
        if (u > v) std::swap(u, v);

        if (!seen.insert(edgeKey(u, v)).second)
            return false;

        graph.edges.push_back({u, v, weight()});
        return true;
    }

    // Add exactly 'extra' new edges from the
    // specified vertex subset.
    void addExtra(
        const std::vector<int>& vertices,
        long long extra
    ) {
        if (extra == 0) return;

        long long available = 0;

        for (size_t i = 0; i < vertices.size(); ++i) {
            for (size_t j = i + 1;
                 j < vertices.size(); ++j) {
                if (!seen.count(edgeKey(
                    vertices[i], vertices[j]
                ))) {
                    ++available;
                }
            }
        }

        if (extra > available)
            throw std::invalid_argument(
                "Requested edge count exceeds capacity"
            );

        // Dense cases: enumerate missing edges.
        if (available <= 2000000 &&
            extra * 3 >= available) {

            std::vector<std::pair<int, int>> candidates;

            for (size_t i = 0; i < vertices.size(); ++i) {
                for (size_t j = i + 1;
                     j < vertices.size(); ++j) {

                    int u = vertices[i];
                    int v = vertices[j];

                    if (!seen.count(edgeKey(u, v)))
                        candidates.push_back({u, v});
                }
            }

            std::shuffle(
                candidates.begin(),
                candidates.end(),
                rng
            );

            for (long long i = 0; i < extra; ++i)
                add(candidates[i].first,
                    candidates[i].second);

            return;
        }

        std::uniform_int_distribution<size_t> pick(
            0, vertices.size() - 1
        );

        long long added = 0;
        long long attempts = 0;
        long long limit = std::max(10000LL, extra * 50);

        while (added < extra && attempts++ < limit) {
            int u = vertices[pick(rng)];
            int v = vertices[pick(rng)];

            if (add(u, v)) ++added;
        }

        if (added < extra) {
            // Deterministic fallback.
            for (size_t i = 0;
                 i < vertices.size() && added < extra;
                 ++i) {

                for (size_t j = i + 1;
                     j < vertices.size() && added < extra;
                     ++j) {

                    if (add(vertices[i], vertices[j]))
                        ++added;
                }
            }
        }

        if (added != extra)
            throw std::runtime_error(
                "Unable to generate requested edges"
            );
    }
};

} // namespace

Graph generateGraph(
    GraphType type,
    int n,
    long long m,
    int k,
    std::uint64_t seed
) {
    if (n < 2 || n > 20000)
        throw std::invalid_argument("Invalid vertex count");

    if (k < 1 || k > n)
        throw std::invalid_argument("Invalid zone count");

    if (m < 0 || m > capacity(n))
        throw std::invalid_argument("Invalid edge count");

    Builder b(
        n, k, seed,
        type == GraphType::EqualWeight
    );

    std::vector<int> vertices(n);
    std::iota(vertices.begin(), vertices.end(), 1);

    switch (type) {

        case GraphType::Chain:
        case GraphType::Star: {
            if (m != n - 1)
                throw std::invalid_argument(
                    "Chain and star require n-1 edges"
                );

            for (int v = 2; v <= n; ++v) {
                int u = type == GraphType::Star
                    ? 1 : v - 1;
                b.add(u, v);
            }
            break;
        }

        case GraphType::Complete: {
            if (m != capacity(n))
                throw std::invalid_argument(
                    "Complete graph requires all edges"
                );

            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    b.add(u, v);

            break;
        }

        case GraphType::Random:
        case GraphType::EqualWeight: {
            if (m < n - 1)
                throw std::invalid_argument(
                    "Connected graph requires n-1 edges"
                );

            // Random spanning tree.
            for (int v = 2; v <= n; ++v) {
                int u = std::uniform_int_distribution<int>(
                    1, v - 1
                )(b.rng);

                b.add(u, v);
            }

            b.addExtra(vertices, m - (n - 1));
            break;
        }

        case GraphType::Bridge: {
            if (m < n - 1)
                throw std::invalid_argument(
                    "Bridge graph requires n-1 edges"
                );

            int mid = n / 2;

            std::vector<int> left, right;

            for (int v = 1; v <= mid; ++v)
                left.push_back(v);

            for (int v = mid + 1; v <= n; ++v)
                right.push_back(v);

            long long leftCap = capacity(
                static_cast<int>(left.size())
            );

            long long rightCap = capacity(
                static_cast<int>(right.size())
            );

            if (m > leftCap + rightCap + 1)
                throw std::invalid_argument(
                    "Too many edges for one-bridge graph"
                );

            // Connect vertices inside each cluster.
            for (int v = 2; v <= mid; ++v)
                b.add(v - 1, v);

            for (int v = mid + 2; v <= n; ++v)
                b.add(v - 1, v);

            // The only cross-cluster edge.
            b.add(mid, mid + 1);

            long long extra = m - (n - 1);

            long long freeLeft =
                leftCap - (static_cast<int>(left.size()) - 1);

            long long freeRight =
                rightCap - (static_cast<int>(right.size()) - 1);

            long long leftExtra = std::min(
                extra, freeLeft
            );

            long long rightExtra = extra - leftExtra;

            if (rightExtra > freeRight)
                throw std::invalid_argument(
                    "Insufficient bridge graph capacity"
                );

            b.addExtra(left, leftExtra);
            b.addExtra(right, rightExtra);

            break;
        }

        case GraphType::Disconnected: {
            // Vertex n remains isolated.
            if (m > capacity(n - 1))
                throw std::invalid_argument(
                    "Too many edges for disconnected graph"
                );

            std::vector<int> subset(
                vertices.begin(),
                vertices.end() - 1
            );

            b.addExtra(subset, m);
            break;
        }
    }

    if (static_cast<long long>(
        b.graph.edges.size()) != m)
        throw std::runtime_error(
            "Generated edge count mismatch"
        );

    return std::move(b.graph);
}

bool validateGraph(const Graph& g) {
    if (g.n < 2 || g.n > 20000)
        return false;

    if (g.k < 1 || g.k > g.n)
        return false;

    if (static_cast<long long>(
        g.edges.size()) > capacity(g.n))
        return false;

    std::unordered_set<U64> seen;

    for (const Edge& e : g.edges) {
        if (e.u < 1 || e.v > g.n ||
            e.u >= e.v || e.w <= 0)
            return false;

        if (!seen.insert(edgeKey(e.u, e.v)).second)
            return false;
    }

    return true;
}

bool isConnected(const Graph& g) {
    if (!validateGraph(g))
        return false;

    std::vector<std::vector<int>> adj(g.n + 1);

    for (const Edge& e : g.edges) {
        adj[e.u].push_back(e.v);
        adj[e.v].push_back(e.u);
    }

    std::vector<char> visited(g.n + 1, false);
    std::vector<int> stack = {1};

    visited[1] = true;
    int count = 0;

    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        ++count;

        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                stack.push_back(v);
            }
        }
    }

    return count == g.n;
}

std::string graphToString(const Graph& g) {
    std::ostringstream out;

    out << g.n << ' ' << g.edges.size()
        << ' ' << g.k << '\n';

    for (const Edge& e : g.edges) {
        out << e.u << ' '
            << e.v << ' '
            << e.w << '\n';
    }

    return out.str();
}

void writeGraph(
    const Graph& g,
    const std::string& filename
) {
    std::ofstream out(filename);

    if (!out)
        throw std::runtime_error(
            "Cannot open output file"
        );

    out << graphToString(g);
}

GraphType parseGraphType(const std::string& s) {
    if (s == "chain") return GraphType::Chain;
    if (s == "star") return GraphType::Star;
    if (s == "random") return GraphType::Random;
    if (s == "equal") return GraphType::EqualWeight;
    if (s == "bridge") return GraphType::Bridge;
    if (s == "disconnected")
        return GraphType::Disconnected;
    if (s == "complete") return GraphType::Complete;

    throw std::invalid_argument("Unknown graph type");
}

std::string graphTypeName(GraphType type) {
    switch (type) {
        case GraphType::Chain: return "chain";
        case GraphType::Star: return "star";
        case GraphType::Random: return "random";
        case GraphType::EqualWeight: return "equal";
        case GraphType::Bridge: return "bridge";
        case GraphType::Disconnected: return "disconnected";
        case GraphType::Complete: return "complete";
    }

    return "unknown";
}

} // namespace mstbench
