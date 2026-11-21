# Adversarial graphs and the crossover benchmark

`bench/test_adversarial.cpp` checks the graph generator on chains, stars,
random connected graphs, equal-weight graphs, bridge graphs, disconnected
graphs, and complete graphs. It also checks determinism, large graphs, and
invalid inputs.

`bench/crossover_analysis.cpp` runs both algorithms on the same graph and
alternates which algorithm runs first. Every trial checks connectivity, MST
cost, and cluster spacing against a reference result. The benchmark measures
in-process runtime in milliseconds.

## Crossover measurements

![Crossover](figures/crossover.png)

The figure uses random connected graphs with 500, 1,000, and 2,000 vertices at
densities from about 1% to 100% of all possible edges.
[`crossover.csv`](crossover.csv) also includes the structural adversarial cases
and a separate complete-graph series. Each setting in the checked-in crossover CSV summarizes five timed trials
per algorithm.

The CSV columns `prim_median_ms` and `kruskal_median_ms` give the middle sorted
runtime for an odd trial count and the midpoint of the two middle sorted
runtimes for an even count. The ratio divides Prim's median by Kruskal's median.
The figure shows the percentage reduction in median runtime with Prim relative
to Kruskal. Negative values favor Kruskal. Timing varies by machine and workload.

[`crossover/results.csv`](crossover/results.csv) records the earlier seven-trial
`Kavin` sweep. It uses a different set of graph sizes and includes executable
startup and input/output in the measured time.

## Rerun the crossover benchmark

Run from the project root on Linux/macOS with a C++17 compiler and Python with
Matplotlib. This overwrites the crossover CSV and figure. On Windows, the
[benchmark script](../bench/bench.ps1) runs the full benchmark pipeline.

```sh
g++ -std=c++17 -O2 bench/crossover_analysis.cpp bench/adversarial_cases.cpp -o /tmp/crossover_analysis
g++ -std=c++17 -O2 bench/crossover_report.cpp -o /tmp/crossover_report
/tmp/crossover_analysis 5 results/crossover.csv
/tmp/crossover_report results/crossover.csv
python3 bench/plot.py results/crossover.csv results/figures/crossover.png
```

To redraw just the crossover from the checked-in CSV, run the last command
above. Run `python3 bench/plot.py` to redraw all figures from existing CSVs.
The crossover report and plot require explicit median runtime columns.
