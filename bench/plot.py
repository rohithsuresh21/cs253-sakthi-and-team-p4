"""Render the benchmark figures or plot a single crossover CSV."""

import argparse
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def read_csv(path):
    with open(path, newline="", encoding="utf-8") as stream:
        return list(csv.DictReader(stream))


def plot_crossover(csv_path, figure_path):
    with csv_path.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        required = {"family", "n", "density", "prim_median_ms", "kruskal_median_ms", "status"}
        if not required.issubset(reader.fieldnames or []):
            raise ValueError("expected a crossover CSV with median runtimes")
        results = list(reader)
        if any(row["status"] != "PASS" for row in results):
            raise ValueError("unverified benchmark record")
        crossover = [row for row in results if row["family"] == "random"]

    for n in sorted({int(row["n"]) for row in crossover}):
        rows = sorted(
            (row for row in crossover if int(row["n"]) == n),
            key=lambda row: float(row["density"]),
        )
        density_percent = [100 * float(row["density"]) for row in rows]
        prim_medians = [float(row["prim_median_ms"]) for row in rows]
        kruskal_medians = [float(row["kruskal_median_ms"]) for row in rows]
        margin_percent = [
            100 * (kruskal - prim) / max(kruskal, 1e-12)
            for prim, kruskal in zip(prim_medians, kruskal_medians)
        ]
        plt.plot(density_percent, margin_percent, marker="o", label=f"n={n}")

    plt.axhline(0, color="black", linewidth=0.8)
    plt.xlabel("Density (% of possible edges)")
    plt.ylabel("Median runtime reduction with Prim (%)")
    plt.title("Prim/Kruskal crossover by median runtime")
    plt.legend()
    plt.grid(True, alpha=0.3)
    figure_path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(figure_path, dpi=120, bbox_inches="tight")
    plt.close()


def plot_runtime_figures():
    Path("results/figures").mkdir(parents=True, exist_ok=True)
    summary = read_csv("results/timings_summary.csv")
    for n in sorted({int(row["n"]) for row in summary}):
        rows = sorted(
            (row for row in summary if int(row["n"]) == n),
            key=lambda row: float(row["edges_per_vertex"]),
        )
        x = [float(row["edges_per_vertex"]) for row in rows]
        plt.plot(x, [float(row["kruskal_median_ms"]) for row in rows], marker="o", label=f"Kruskal n={n}")
        plt.plot(x, [float(row["prim_median_ms"]) for row in rows], marker="s", linestyle="--", label=f"Prim n={n}")
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("edges per vertex")
    plt.ylabel("median time (ms)")
    plt.title("Median runtime by graph density")
    plt.legend(fontsize=7)
    plt.grid(True, alpha=0.3)
    plt.savefig("results/figures/runtime_vs_density.png", dpi=120, bbox_inches="tight")
    plt.close()

    union_find = read_csv("results/union_find.csv")
    labels = [f"{row['n']}\nm={row['m']}" for row in union_find]
    positions = list(range(len(union_find)))
    plt.bar([i - 0.2 for i in positions], [float(row["with_compression_median_ms"]) for row in union_find], width=0.4, label="with compression")
    plt.bar([i + 0.2 for i in positions], [float(row["without_compression_median_ms"]) for row in union_find], width=0.4, label="without compression")
    plt.xticks(positions, labels)
    plt.xlabel("graph (n, m)")
    plt.ylabel("median Kruskal time (ms)")
    plt.title("Union-find path compression")
    plt.legend()
    plt.grid(True, axis="y", alpha=0.3)
    plt.savefig("results/figures/union_find_comparison.png", dpi=120, bbox_inches="tight")
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
    plt.savefig("results/figures/prim_heap_size.png", dpi=120, bbox_inches="tight")
    plt.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv", type=Path, nargs="?")
    parser.add_argument("figure", type=Path, nargs="?",
                        default=Path("results/figures/crossover.png"))
    args = parser.parse_args()

    if args.csv is None:
        plot_runtime_figures()
        crossover_path = Path("results/crossover.csv")
        if not crossover_path.exists():
            return
    else:
        crossover_path = args.csv

    try:
        plot_crossover(crossover_path, args.figure)
    except ValueError as error:
        parser.error(str(error))


if __name__ == "__main__":
    main()
