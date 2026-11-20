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
plt.xscale("log")
plt.yscale("log")
plt.xlabel("edges per vertex (m / n)")
plt.ylabel("median running time (ms)")
plt.title("Running time vs density")
plt.legend(fontsize=7)
plt.grid(True, alpha=0.3)
plt.savefig("results/figures/runtime_vs_density.png", dpi=150, bbox_inches="tight")
plt.close()
uf = read("results/union_find.csv")
labels = [r["n"] + "\nm=" + r["m"] for r in uf]
x = range(len(uf))
plt.figure(figsize=(8, 5))
plt.bar([i - 0.2 for i in x], [float(r["with_compression_median_ms"]) for r in uf], width=0.4, label="with path compression")
plt.bar([i + 0.2 for i in x], [float(r["without_compression_median_ms"]) for r in uf], width=0.4, label="without path compression")
plt.xticks(list(x), labels)
plt.xlabel("graph (n, m)")
plt.ylabel("median Kruskal time (ms)")
plt.title("Union-find: path compression on and off")
plt.legend()
plt.savefig("results/figures/union_find_comparison.png", dpi=150, bbox_inches="tight")
plt.close()


