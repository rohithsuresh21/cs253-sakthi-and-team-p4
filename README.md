# cs253-sakthi-and-team-p4
## Prim and Kruskal crossover benchmark

For each graph, the benchmark times each executable seven times by default and
alternates which algorithm runs first. It checks connectivity, MST cost, and
cluster spacing against a reference result on every run. Timings are wall-clock
milliseconds and include process startup and input/output.

The CSV records `prim_median_ms` and `kruskal_median_ms`. With an odd repetition
count, the median is the middle sorted runtime. With an even count, it is the
midpoint of the two middle sorted runtimes. The CSV ratio divides Prim's median
by Kruskal's median. The figure shows the percent reduction in median runtime
with Prim relative to Kruskal on the random-graph density sweep. Negative
values indicate that Kruskal is faster.

From the repository root, build the benchmark and report with a C++17 compiler.
Supply executable paths that read the graph from standard input and write the
MST cost and spacing, or `DISCONNECTED`, to standard output.

```sh
g++ -std=c++17 -O2 bench/crossover_analysis.cpp bench/adversarial_cases.cpp -o /tmp/crossover_analysis
g++ -std=c++17 -O2 bench/crossover_report.cpp -o /tmp/crossover_report
/tmp/crossover_analysis /path/to/prim /path/to/kruskal 7
/tmp/crossover_report results/crossover/results.csv
python3 bench/plot.py
```

The plotter requires Matplotlib. It accepts optional input CSV and output PNG
paths. Both the report and plotter require CSV columns that explicitly identify
the median runtimes.

The checked-in CSV and [crossover figure](results/figures/crossover.png) use
seven fresh trials per algorithm for each of the 38 graph cases. The executables
were built with GCC and `-O2 -std=c++17` from the project application at
`3311645c587639cca4f9354386dc223063810fc1`, invoked with `--structure B` for Prim
and `--structure A` for Kruskal. Timings vary by machine and workload.
