import csv
import os
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

os.makedirs("results/figures", exist_ok=True)


def read(path):
    with open(path) as f:
        return list(csv.DictReader(f))
      summary = read("results/timings_summary.csv")
sizes = sorted(set(int(r["n"]) for r in summary))
plt.figure(figsize=(8, 5))
for n in sizes:
    rows = [r for r in summary if int(r["n"]) == n]
    x = [float(r["edges_per_vertex"]) for r in rows]
    plt.plot(x, [float(r["kruskal_median_ms"]) for r in rows], marker="o", label="Kruskal n=" + str(n))
    plt.plot(x, [float(r["prim_median_ms"]) for r in rows], marker="s", linestyle="--", label="Prim n=" + str(n))
