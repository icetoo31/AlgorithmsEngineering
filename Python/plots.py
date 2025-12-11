import sys
from pathlib import Path
from collections import defaultdict
import matplotlib.pyplot as plt

# --- Configuration ----------------------------------------------------------

LABELS = {
    "b": "Binary heap",
    "e": "8-ary heap",
    "p": "Pairing heap",
    "f": "Fibonacci heap",
}

COLORS = {
    "b": "#925E78",
    "e": "#8B9556",
    "p": "#DD7230",
    "f": "#537D8D",
}

TIME_FIELD      = "Time"
PUSH_AVG_FIELD  = "PushTimeAvg"
DEC_AVG_FIELD   = "DecreaseKeyTimeAvg"
POP_AVG_FIELD   = "PopTimeAvg"


# --- Parsing ----------------------------------------------------------------

def parse_file(path: Path):
    """
    Parse one output file produced by main.cpp.

    Returns
    -------
    avg_time : dict[heap][N] -> avg total Dijkstra runtime
    avg_ops  : dict[metric][heap][N] -> avg per-operation time
               metric in {"push", "decrease", "pop"}
    """
    raw_time = {h: defaultdict(list) for h in LABELS.keys()}
    metrics = ("push", "decrease", "pop")
    raw_ops = {
        m: {h: defaultdict(list) for h in LABELS.keys()}
        for m in metrics
    }

    def safe_float(s):
        try:
            return float(s)
        except (TypeError, ValueError):
            return None

    with path.open("r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            line = line.strip()
            if not line.startswith("Heap:"):
                continue

            # split "Key: value" pairs by commas
            fields = {}
            for chunk in line.split(","):
                chunk = chunk.strip()
                if ":" not in chunk:
                    continue
                key, val = chunk.split(":", 1)
                fields[key.strip()] = val.strip()

            heap = fields.get("Heap")
            if heap not in LABELS:
                continue

            try:
                n = int(fields.get("N", ""))
            except ValueError:
                continue

            # total runtime
            t = safe_float(fields.get(TIME_FIELD))
            if t is not None:
                raw_time[heap][n].append(t)

            # per-op avg times
            push_avg = safe_float(fields.get(PUSH_AVG_FIELD))
            dec_avg  = safe_float(fields.get(DEC_AVG_FIELD))
            pop_avg  = safe_float(fields.get(POP_AVG_FIELD))

            if push_avg is not None:
                raw_ops["push"][heap][n].append(push_avg)
            if dec_avg is not None:
                raw_ops["decrease"][heap][n].append(dec_avg)
            if pop_avg is not None:
                raw_ops["pop"][heap][n].append(pop_avg)

    # averages
    avg_time = {h: {} for h in LABELS.keys()}
    for heap, by_n in raw_time.items():
        for n, vals in by_n.items():
            if vals:
                avg_time[heap][n] = sum(vals) / len(vals)

    avg_ops = {m: {h: { } for h in LABELS.keys()} for m in metrics}
    for metric in metrics:
        for heap, by_n in raw_ops[metric].items():
            for n, vals in by_n.items():
                if vals:
                    avg_ops[metric][heap][n] = sum(vals) / len(vals)

    return avg_time, avg_ops


# --- Plotting helpers -------------------------------------------------------

def _plot_metric(avg_data, ylabel, title, filename):
    """
    avg_data: dict[heap][N] -> value
    """
    out_dir = Path("outputs")
    out_dir.mkdir(exist_ok=True)

    plt.figure()

    for heap, label in LABELS.items():
        results = avg_data.get(heap)
        if not results:
            continue
        points = sorted(results.items(), key=lambda x: x[0])
        ns   = [p[0] for p in points]
        vals = [p[1] for p in points]

        plt.plot(
            ns,
            vals,
            marker="o",
            linestyle="-",
            markersize=3,
            label=label,
            color=COLORS.get(heap),
        )

    plt.xlabel("Number of nodes N")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True, linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()

    out_path = out_dir / filename
    plt.savefig(out_path, dpi=200)
    plt.close()
    print(f"Saved {out_path}")


def make_plots_for_file(path: Path):
    """
    For a single file (sparse or dense), produce:
      - runtime plot
      - avg push time
      - avg decrease-key time
      - avg pop time
    with filenames prefixed by the file stem ('sparse', 'dense', etc.).
    """
    print(f"\n=== Processing {path} ===")
    avg_time, avg_ops = parse_file(path)

    # derive prefix from filename (e.g. 'sparse_output.txt' -> 'sparse')
    stem = path.stem
    if "sparse" in stem.lower():
        prefix = "sparse_"
    elif "dense" in stem.lower():
        prefix = "dense_"
    else:
        prefix = stem + "_"

    # 1) overall runtime
    _plot_metric(
        avg_time,
        ylabel="Average Dijkstra runtime (s)",
        title=f"Runtime vs N ({prefix.rstrip('_')})",
        filename=f"{prefix}avg_runtime.png",
    )

    # 2) per-op averages
    metric_info = {
        "push": (
            "Average time per Insert-key (s)",
            f"{prefix}avg_push_time.png",
        ),
        "decrease": (
            "Average time per Decrease key (s)",
            f"{prefix}avg_decrease_key_time.png",
        ),
        "pop": (
            "Average time per Extract-min (s)",
            f"{prefix}avg_pop_time.png",
        ),
    }

    for metric, (ylabel, fname) in metric_info.items():
        data = avg_ops.get(metric, {})
        has_any = any(len(by_n) > 0 for by_n in data.values())
        if not has_any:
            print(f"[WARN] no data for metric '{metric}' in {path.name}, skipping.")
            continue

        _plot_metric(
            data,
            ylabel=ylabel,
            title=f"{ylabel} vs N ({prefix.rstrip('_')})",
            filename=fname,
        )


# --- Main -------------------------------------------------------------------

def main():
    # If no args: default to sparse_output.txt and dense_output.txt next to IO/
    if len(sys.argv) <= 1:
        candidates = [
            Path("IO/sparse_output.txt"),
            Path("IO/dense_output.txt"),
        ]
        paths = [p for p in candidates if p.exists()]
        if not paths:
            print("Usage: python plots.py <output1> [<output2> ...]")
            print("Or place IO/sparse_output.txt and IO/dense_output.txt and run without arguments.")
            sys.exit(1)
    else:
        paths = [Path(arg) for arg in sys.argv[1:]]

    for p in paths:
        if not p.exists():
            print(f"WARNING: {p} does not exist, skipping.")
            continue
        make_plots_for_file(p)


if __name__ == "__main__":
    main()
