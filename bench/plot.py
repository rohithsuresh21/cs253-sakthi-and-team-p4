"""Plot the random-graph density sweep from crossover_analysis."""

import argparse
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv", type=Path, nargs="?",
                        default=Path("results/crossover/results.csv"))
    parser.add_argument("figure", type=Path, nargs="?",
                        default=Path("results/figures/crossover.png"))
    args = parser.parse_args()

    with args.csv.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        required = {"family", "n", "density", "prim_median_ms", "kruskal_median_ms", "status"}
        if not required.issubset(reader.fieldnames or []):
            parser.error("expected a crossover CSV with median runtimes")
        results = list(reader)
        if any(row["status"] != "PASS" for row in results):
            parser.error("unverified benchmark record")
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
    args.figure.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(args.figure, dpi=120, bbox_inches="tight")
    plt.close()


if __name__ == "__main__":
    main()
