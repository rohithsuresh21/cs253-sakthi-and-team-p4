# Measurements: how fast is each algorithm?

**Files** [`benchmark.cpp`](benchmark.cpp), [`bench.ps1`](bench.ps1), and [`plot.py`](plot.py) · **Data** [`results/`](../results/)

## How we measured
* Random connected graphs with n = 200, 500, 1,000 and 2,000 buildings.
* For each n, 6 densities from 3 cables per building up to a complete graph.
* After one warm-up per planner, each setting has **five timed runs** per planner. We report the **median**.
* Both algorithms receive the same graph.

## Result

![Runtime versus density](../results/figures/runtime_vs_density.png)

| n | m | Kruskal (median ms) | Prim (median ms) | Faster |
|---:|---:|---:|---:|---|
| 1,000 | 3,000 | 0.217 | 0.795 | Kruskal 3.67x |
| 1,000 | 50,000 | 7.238 | 8.349 | Kruskal 1.15x |
| 1,000 | 499,500 | 225.130 | 62.950 | Prim 3.58x |
| 2,000 | 6,000 | 0.604 | 2.551 | Kruskal 4.22x |
| 2,000 | 1,999,000 | 1714.276 | 350.875 | Prim 4.89x |

**Kruskal wins 22 of 24 settings, Prim wins 2, and 0 tie.**

## Why
* Kruskal sorts **every** cable first. That is cheap for few cables and expensive for millions.
* Prim only pops what it needs, so it grows more slowly with density.

## Extra measurements
* `prim_heap.csv`: heap size, pushes, pops and stale pops. About 75% of pops on the densest graph are stale.
* `union_find.csv` reports medians from the fresh path-compression run. `union_find_raw.csv` contains all five trials per mode and setting.

## Run it

Run these commands from the repository root. The benchmark commands overwrite
the corresponding CSVs in `results/`. Plotting requires Python 3 and
Matplotlib. To redraw from existing data, run `python3 bench/plot.py` on
Linux/macOS or `python bench/plot.py` in Windows PowerShell.

**Windows PowerShell**

```powershell
.\bench\bench.ps1
```

**Linux/macOS**

```sh
g++ -std=c++17 -O2 bench/benchmark.cpp -o /tmp/mst-benchmark
/tmp/mst-benchmark
g++ -std=c++17 -O2 bench/crossover_analysis.cpp bench/adversarial_cases.cpp -o /tmp/crossover_analysis
/tmp/crossover_analysis 5 results/crossover.csv
python3 bench/plot.py
```

See the [crossover notes](../results/README.md) for the median CSV schema, the
text report, and commands for redrawing only the crossover figure.
