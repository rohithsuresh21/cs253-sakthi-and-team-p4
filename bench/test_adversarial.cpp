
#include "adversarial_cases.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace mstbench;

void require(bool condition, const std::string& message) {
    if (!condition)
        throw std::runtime_error(message);
}

void testChain() {
    auto g = generateGraph(GraphType::Chain, 100, 99);

    require(validateGraph(g), "Invalid chain");
    require(isConnected(g), "Chain disconnected");
    require(g.edges.size() == 99, "Wrong chain size");
}

void testStar() {
    auto g = generateGraph(GraphType::Star, 100, 99);

    require(validateGraph(g), "Invalid star");
    require(isConnected(g), "Star disconnected");

    for (const auto& e : g.edges)
        require(e.u == 1, "Invalid star center");
}

void testRandom() {
    auto g = generateGraph(
        GraphType::Random, 200, 1000
    );

    require(validateGraph(g), "Invalid random graph");
    require(isConnected(g), "Random graph disconnected");
    require(g.edges.size() == 1000, "Wrong edge count");
}

void testEqualWeight() {
    auto g = generateGraph(
        GraphType::EqualWeight, 100, 300
    );

    require(validateGraph(g), "Invalid equal-weight graph");
    require(isConnected(g), "Equal-weight graph disconnected");

    for (const auto& e : g.edges)
        require(e.w == 1, "Weights are not equal");
}

void testBridge() {
    auto g = generateGraph(
        GraphType::Bridge, 100, 400
    );

    require(validateGraph(g), "Invalid bridge graph");
    require(isConnected(g), "Bridge graph disconnected");

    int crossing = 0;

    for (const auto& e : g.edges) {
        if (e.u <= 50 && e.v > 50)
            ++crossing;
    }

    require(crossing == 1, "Expected exactly one bridge");
}

void testDisconnected() {
    auto g = generateGraph(
        GraphType::Disconnected, 100, 200
    );

    require(validateGraph(g), "Invalid disconnected graph");
    require(!isConnected(g), "Graph unexpectedly connected");
}

void testComplete() {
    auto g = generateGraph(
        GraphType::Complete, 30, 435
    );

    require(validateGraph(g), "Invalid complete graph");
    require(isConnected(g), "Complete graph disconnected");
    require(g.edges.size() == 435, "Wrong complete size");
}

void testDeterminism() {
    auto a = generateGraph(
        GraphType::Random, 100, 400, 2, 123
    );

    auto b = generateGraph(
        GraphType::Random, 100, 400, 2, 123
    );

    require(
        graphToString(a) == graphToString(b),
        "Generation is not deterministic"
    );
}

void testLargeGraph() {
    auto g = generateGraph(
        GraphType::Random, 20000, 60000
    );

    require(validateGraph(g), "Invalid large graph");
    require(isConnected(g), "Large graph disconnected");
    require(g.edges.size() == 60000, "Wrong large size");
}

void testInvalidParameters() {
    bool rejected = false;

    try {
        generateGraph(GraphType::Random, 10, 2);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }

    require(rejected, "Invalid graph was accepted");
}

int main() {
    try {
        testChain();
        testStar();
        testRandom();
        testEqualWeight();
        testBridge();
        testDisconnected();
        testComplete();
        testDeterminism();
        testLargeGraph();
        testInvalidParameters();

        std::cout
            << "All adversarial graph tests passed.\n";

    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
