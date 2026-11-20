# Campus Network Cabling using Minimum Spanning Trees

## Project overview

This project models a campus cabling layout as an **undirected, weighted graph**:

- **Vertex:** a campus building or network location.
- **Edge:** a possible cable connection between two locations.
- **Weight:** the estimated cost (or other chosen measure) of installing that cable.

The goal is to connect all buildings with the minimum total cable cost. The project implements and compares two Minimum Spanning Tree (MST) approaches: **Kruskal's algorithm** and **Prim's algorithm**. It also considers maintenance zones, correctness testing and performance evaluation.

## Main project tasks

1. **Kruskal planner:** considers cable connections in increasing weight order and adds an edge only when it joins two previously separate components. Heapsort can be used to sort the edge list, and Disjoint Set Union (DSU) is used for cycle detection.
2. **Prim planner:** starts from one building and repeatedly adds the cheapest available edge that reaches an unvisited building. The implementation uses a heap to manage candidate edges.
3. **Program integration:** provides a common program flow for choosing a planner, loading or generating a campus graph, checking connectivity and displaying the network cost and requested maintenance-zone information.
4. **Correctness and testing:** runs both planners on the same test graphs, checks their results and verifies MST optimality independently.
5. **Performance evaluation:** measures planner runtimes and relevant implementation statistics on graphs of different sizes and densities, then prepares charts.
6. **Difficult cases and crossover study:** tests challenging graph structures and investigates whether and under what conditions Prim becomes faster than Kruskal.

## Maintenance zones

The requested value `k` is the number of maintenance zones. For a connected graph, the MST can be divided into `k` components by removing the `k - 1` most expensive edges of the MST. The **zone spacing** is the minimum-weight original graph edge that could connect two different resulting zones. Zone grouping is a separate planning result; it does not change the fact that the global MST connects all vertices.

## Correctness testing (Task 4)

The Task 4 test program is `tests/test_all.cpp`, and its graph-generation helpers are in `tests/randomgraph.h`.

For each generated graph, the test program:

- runs Kruskal and Prim on the same graph;
- checks whether each planner's connectivity result agrees with an independent graph-connectivity check;
- compares the total MST costs when the graph is connected;
- compares the maintenance-zone spacing returned by the two planners;
- applies an independent MST certificate to eligible connected cases; and
- writes one row per test case to `results/correctness.csv`.

### Test coverage

The completed run contains **1,207 cases** across these categories:

- small connected random graphs;
- medium connected random graphs;
- disconnected graphs;
- equal-weight graphs;
- tree graphs;
- dense graphs;
- special maintenance-zone values, including `k = 2`, `k = n - 1` and `k = n`;
- large connected and disconnected graphs with 20,000 vertices.

The test program uses a fixed random seed (`42`) so the generated test sequence can be reproduced.

### Recorded correctness result

The completed run on the member's laptop produced:

```text
cases: 1207, certified small graphs: 1050, disconnected: 152, failed: 0
```

Interpretation:

- **1,207** graph cases were processed.
- **1,050** eligible connected cases received the MST certificate check.
- **152** disconnected cases were detected.
- **0** test cases failed.
- The five 20,000-vertex connected cases were tested without the certificate check, as configured in the test program.

The generated detailed results are stored in `results/correctness.csv`. Commit this CSV as test evidence when submitting the Task 4 files. If the source code or algorithms are changed after this run, rerun the tests and regenerate the CSV.

## How to run the correctness tests

Run these commands from the project root (or from the scratch folder containing the copied project files). Ensure the `results` directory exists first.

### Windows PowerShell

```powershell
New-Item -ItemType Directory -Force results

g++ -std=c++17 -O2 -o test_all tests/test_all.cpp
.\test_all.exe
```

### Linux / macOS

```bash
mkdir -p results

g++ -std=c++17 -O2 -o test_all tests/test_all.cpp
./test_all
```

The test summary is printed in the terminal, and the detailed CSV is written to `results/correctness.csv`.

## MST certificate idea

Agreement between Prim and Kruskal is a useful check, but agreement alone does not independently prove optimality. The test program also uses the MST cycle-property certificate:

> For every original graph edge `(u, v)`, its weight must be at least the maximum edge weight on the unique path between `u` and `v` in the proposed spanning tree.

If an original edge were cheaper than the maximum-weight edge on that tree path, replacing the heavier tree edge with the cheaper edge would produce a lower-cost spanning tree. The certificate checks this condition for the eligible connected test cases.

## Q4: Prim's heap complexity

In the heap-based Prim implementation described for this project, each undirected cable can be inserted into the candidate heap at most twice: once while examining each endpoint. Thus, the heap can contain at most `2E` entries, and there are `O(E)` pushes and pops overall. Each operation costs `O(log E)`. For a simple graph, `E <= V(V - 1)/2`, so `log E = O(log V)`. Therefore, the stated running time is:

```text
O(E log V)
```

This bound applies to the described lazy-heap implementation and its edge-insertion strategy.

## Repository files relevant to Task 4

```text
.
├── src/
│   ├── common/          # Shared graph, connectivity and zone utilities
│   ├── structA/         # Kruskal implementation
│   └── structB/         # Prim implementation
├── tests/
│   ├── randomgraph.h    # Random graph generators
│   ├── test_all.cpp     # Correctness test driver and CSV writer
│   ├── sample1.in       # Sample test input
│   ├── sample1.out      # Expected sample output
│   ├── sample2.in       # Sample test input
│   └── sample2.out      # Expected sample output
├── results/
│   └── correctness.csv # Generated correctness results
├── test.ps1             # PowerShell test helper
├── TOOLS.md             # Tools used and team contributions
└── README.md
```

The exact repository may contain additional source, benchmark and plotting files for Tasks 1–6.

## Tools

- **C++17 / g++:** graph representation, MST planners and correctness tests.
- **Python / Matplotlib:** intended for processing benchmark data and producing performance charts (Task 5).
- **Git:** version control and collaboration through branches and pull requests.

## Notes

- The correctness CSV is generated by running `tests/test_all.cpp`; it is not generated by `randomgraph.h` alone.
- `randomgraph.h` supplies graph-generation functions, while `test_all.cpp` controls the test cases, runs the planners, checks the results and writes the CSV.
- Do not commit compiled executables or temporary scratch folders.
- Keep the README's status and recorded results updated whenever the implementation or test suite changes.
