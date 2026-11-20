# Tools and Team Contributions

## Tools Used

| Tool | Purpose | What we checked or changed |
|---|---|---|
| Claude (AI assistant) | Assisted with initial code, tests, benchmark programs and plotting script. | Reviewed and modified the suggestions; each member tested their assigned task. |
| C++ / g++ | Implementing, compiling and running the two planners. | Fixed compilation/runtime issues and checked program output. |
| Python / Matplotlib | Processing benchmark results and making charts. | Checked that chart data matched the recorded measurements. |

## Individual Contributions

Replace each placeholder with the member's name and the work they actually completed.

### Member 1: Rohit MS — Kruskal Planner
- **Work:** Implemented the edge sorting and Kruskal planner; selected a cable only when it joins two previously separate groups.
- **Tested:** Checked edge order, cycle prevention and the resulting MST on sample graphs.
- **Understood:** How Kruskal builds an MST from the cheapest available cables.

### Member 2: Rehan Sharief — Prim Planner
- **Work:** Implemented the Prim planner; started from one building and repeatedly selected the cheapest cable reaching an unconnected building.
- **Tested:** Checked the selected cables and MST cost on sample connected graphs.
- **Understood:** How Prim grows one connected network and tracks candidate cables.

### Member 3: Sakthi — Program Integration
- **Work:** Combined both planners into one program, added the planner choice and campus-layout input/generation, and handled disconnected graphs and maintenance-zone output.
- **Tested:** Ran both planner options and checked the program's messages and output for connected and disconnected layouts.
- **Understood:** How the graph input, planners and final output work together.

### Member 4: Parthasarathy — Correctness Checks
- **Work:** Prepared at least 1,000 random campus layouts and compared the two planners' results. For small graphs, checked whether swapping an unused cable could reduce the selected network cost.
- **Tested:** Compared total MST costs and checked small cases for possible cheaper alternatives.
- **Understood:** How to validate MST results beyond simply checking that both planners agree.

### Member 5: Bhavan Jeeth — Benchmarking and Charts
- **Work:** Created repeatable speed tests for graphs with few and many possible cables; measured the shortcut used by Kruskal and how many cable candidates Prim tracks at once; prepared charts.
- **Tested:** Repeated measurements on the same graph inputs and checked that the plotted values matched the results.
- **Understood:** How graph size and density affect execution time and resource use.

### Member 6: Kavin Krishnan — Difficult Cases and Crossover Analysis
- **Work:** Created difficult campus layouts and investigated whether Prim becomes faster than Kruskal as the number of possible cables increases.
- **Tested:** Ran both planners on the same difficult graphs and recorded where a speed crossover appeared, or noted that none appeared in the tested cases.
- **Understood:** How graph structure and density can affect the relative performance of the two planners.
