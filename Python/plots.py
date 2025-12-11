import re
import sys
from pathlib import Path
from collections import defaultdict
import matplotlib.pyplot as plt

# --- Configuration ---
LABELS = {
    "b": "Binary heap",
    "e": "8-ary heap",
    "p": "Pairing heap",
    "f": "Fibonacci heap"
}

COLORS = {
    "b": "#925E78",
    "e": "#8B9556",s
    "p": "#DD7230",
    "f": "#537D8D",
}

# Regular Expression to capture Heap, N, M, and Time
# Format: Heap: b, N: 100, M: 500, Time: 0.0000330000
LINE_RE = re.compile(r"Heap:\s*([bepf]),\s*N:\s*(\d+),\s*M:\s*(\d+),\s*Time:\s*([0-9.eE+-]+)"
                     r"(?:,\s*Pushes:\s*(\d+),\s*DecreaseKey:\s*(\d+),\s*Pops:\s*(\d+))?")

def parse_and_average(path: Path):
    # time_data[heap][n] = list of runtimes
    time_data = {h: defaultdict(list) for h in LABELS.keys()}
s
    # op_data[metric][heap][n] = list of counts
    metrics = ["pushes", "decrease", "pops"]
    op_data = {
        metric: {h: defaultdict(list) for h in LABELS.keys()}
        for metric in metrics
    }

    try:
        with path.open("r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                m = LINE_RE.match(line.strip())
                if not m:
                    continue

                heap = m.group(1)
                n = int(m.group(2))
                t = float(m.group(4))

                # Always store times
                time_data[heap][n].append(t)

                # Optional operation counts
                pushes_str = m.group(5)
                dec_str    = m.group(6)
                pops_str   = m.group(7)

                if pushes_str is not None:
                    pushes = int(pushes_str)
                    dec    = int(dec_str)
                    pops   = int(pops_str)

                    op_data["pushes"][heap][n].append(pushes)
                    op_data["decrease"][heap][n].append(dec)
                    op_data["pops"][heap][n].append(pops)
    except FileNotFoundError:
        return None, None, None, None

    # Average runtime: avg_time[heap][n] = avg_time
    avg_time = {}
    for heap in LABELS.keys():
        avg_time[heap] = {}
        for n, times in time_data[heap].items():
            avg_time[heap][n] = sum(times) / len(times)

    # Average operations: avg_ops[metric][heap][n] = avg_count
    avg_ops = {metric: {} for metric in metrics}
    for metric in metrics:
        for heap in LABELS.keys():
            avg_ops[metric][heap] = {}
            for n, vals in op_data[metric][heap].items():
                if vals:
                    avg_ops[metric][heap][n] = sum(vals) / len(vals)

    return avg_time, time_data, avg_ops, op_data


def plot_overall_average(avg_data):
    # avg_data[heap] = { n: avg_time }
    
    out_dir = Path("outputs")
    out_dir.mkdir(exist_ok=True)
    
    plt.figure()

    for heap, label in LABELS.items():
        results = avg_data.get(heap)
        if not results:
            continue
            
        # Convert dictionary to list of (N, Avg_Time) tuples
        points = [(n, avg_time) for n, avg_time in results.items()]
            
        # Sort by N for clean plotting
        points.sort(key=lambda x: x[0])
        ns = [p[0] for p in points]
        avgs = [p[1] for p in points]
            
        # Plot as a line, as the N values are scaling test cases
        plt.plot(ns, avgs, marker='o', linestyle='-', markersize=5, 
                 label=label, color=COLORS.get(heap))

    # --- CHANGES APPLIED HERE ---
    # Both x-axis and y-axis are now linear (default)
    plt.xlabel("Number of Nodes (N)")
    plt.ylabel("Average Runtime (seconds)")
    plt.title("Dijkstra Runtime Scaling (Overall Average Across Densities)")
        
    # Removed: plt.xscale("log") 
    # Removed: plt.yscale("log") 
    # ---------------------------
        
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)

    out_path = out_dir / f"avg_runtime_overall_linear.png"
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)
    print(f"Saved plot for Overall Average to: {out_path}")

def plot_operation_metric(avg_for_metric, title, ylabel, filename_suffix):
    """
    avg_for_metric: dict[heap][n] -> avg count (for one metric, e.g. pushes)
    """
    out_dir = Path("plots")
    out_dir.mkdir(exist_ok=True)

    plt.figure()

    for heap, label in LABELS.items():
        ns = sorted(avg_for_metric[heap].keys())
        if not ns:
            continue
        ys = [avg_for_metric[heap][n] for n in ns]
        plt.plot(ns, ys, marker="o", label=label)

    plt.xlabel("Number of nodes N")
    plt.ylabel(ylabel)
    plt.title(title)

    # Linear scales (no log)
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)

    out_path = out_dir / f"{filename_suffix}_overall_linear.png"
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)
    print(f"Saved plot for {title} to: {out_path}")


def plot_operation_metric(avg_for_metric, title, ylabel, filename_suffix):
    """
    avg_for_metric: dict[heap][n] -> avg count (for one metric, e.g. pushes)
    """
    out_dir = Path("plots")
    out_dir.mkdir(exist_ok=True)

    plt.figure()

    for heap, label in LABELS.items():
        ns = sorted(avg_for_metric[heap].keys())
        if not ns:
            continue
        ys = [avg_for_metric[heap][n] for n in ns]
        plt.plot(ns, ys, marker="o", label=label)

    plt.xlabel("Number of nodes N")
    plt.ylabel(ylabel)
    plt.title(title)

    # Linear scales (no log)
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)

    out_path = out_dir / f"{filename_suffix}_overall_linear.png"
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)
    print(f"Saved plot for {title} to: {out_path}")


if __name__ == "__main__":
    main()