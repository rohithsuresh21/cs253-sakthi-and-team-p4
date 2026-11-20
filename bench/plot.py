import csv
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


os.makedirs("results", exist_ok=True)


def read_csv(path):
    with open(path, newline="", encoding="utf-8") as stream:
        return list(csv.DictReader(stream))


summary = read_csv("results/timings_summary.csv")
for n in sorted({int(row["n"]) for row in summary}):
    rows = sorted(
        (row for row in summary if int(row["n"]) == n),
        key=lambda row: float(row["edges_per_vertex"]),
    )
    x = [float(row["edges_per_vertex"]) for row in rows]
    plt.plot(x, [float(row["kruskal_avg_ms"]) for row in rows], marker="o", label=f"Kruskal n={n}")
    plt.plot(x, [float(row["prim_avg_ms"]) for row in rows], marker="s", linestyle="--", label=f"Prim n={n}")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("edges per vertex")
plt.ylabel("average time (ms)")
plt.title("Average runtime by graph density")
plt.legend(fontsize=7)
plt.grid(True, alpha=0.3)
plt.savefig("results/runtime_vs_density.png", dpi=120, bbox_inches="tight")
plt.close()

union_find = read_csv("results/union_find.csv")
labels = [f"{row['n']}\nm={row['m']}" for row in union_find]
positions = list(range(len(union_find)))
plt.bar([i - 0.2 for i in positions], [float(row["with_compression_avg_ms"]) for row in union_find], width=0.4, label="with compression")
plt.bar([i + 0.2 for i in positions], [float(row["without_compression_avg_ms"]) for row in union_find], width=0.4, label="without compression")
plt.xticks(positions, labels)
plt.xlabel("graph (n, m)")
plt.ylabel("average Kruskal time (ms)")
plt.title("Union-find path compression")
plt.legend()
plt.grid(True, axis="y", alpha=0.3)
plt.savefig("results/union_find_comparison.png", dpi=120, bbox_inches="tight")
plt.close()

heap = read_csv("results/prim_heap.csv")
for n in sorted({int(row["n"]) for row in heap}):
    rows = sorted((row for row in heap if int(row["n"]) == n), key=lambda row: int(row["m"]))
    plt.plot([int(row["m"]) for row in rows], [int(row["max_heap_size"]) for row in rows], marker="o", label=f"n={n}")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("number of edges")
plt.ylabel("maximum heap size")
plt.title("Prim lazy deletion heap size")
plt.legend()
plt.grid(True, alpha=0.3)
plt.savefig("results/prim_heap_size.png", dpi=120, bbox_inches="tight")
plt.close()

crossover_path = "results/crossover.csv"
if os.path.exists(crossover_path):
    crossover = [row for row in read_csv(crossover_path) if row["family"] == "random"]
    for n in sorted({int(row["n"]) for row in crossover}):
        rows = sorted((row for row in crossover if int(row["n"]) == n), key=lambda row: float(row["density"]))
        x = [100 * float(row["density"]) for row in rows]
        y = [100 * (float(row["kruskal_ms"]) - float(row["prim_ms"])) / max(float(row["kruskal_ms"]), 1e-12) for row in rows]
        plt.plot(x, y, marker="o", label=f"n={n}")
    plt.axhline(0, color="black", linewidth=0.8)
    plt.xlabel("density (% of possible edges)")
    plt.ylabel("Prim faster by (%)")
    plt.title("Prim/Kruskal crossover")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig("results/crossover.png", dpi=120, bbox_inches="tight")
    plt.close()

print("Figures saved in results")
