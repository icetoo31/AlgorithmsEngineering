import re
import sys
from pathlib import Path
from collections import defaultdict
import matplotlib.pyplot as plt
import numpy as np

# --- Configuration ---
LABELS = {
    "b": "Binary heap",
    "e": "8-ary heap",
    "p": "Pairing heap",
    "f": "Fibonacci heap"
}

COLORS = {
    "b": "#0072B2",  # Blue (Okabe-Ito palette - color-blind safe)
    "e": "#E69F00",  # Orange
    "p": "#009E73",  # Green
    "f": "#D55E00",  # Red-Orange
}

# Regular Expression to capture Heap, N, M, and Time
# Format: Heap: b, N: 100, M: 500, Median Time: 0.0000330000
LINE_RE = re.compile(r"Heap:\s*([bepf]),\s*N:\s*(\d+),\s*M:\s*(\d+),\s*(?:Median )?Time:\s*([0-9.eE+-]+)")

def parse_and_average(path: Path):
    # data[heap][n] = list of times (from all trials for this N)
    data = {h: defaultdict(list) for h in LABELS.keys()}

    try:
        with path.open("r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                m = LINE_RE.match(line.strip())
                if not m:
                    continue
                
                # Capture the groups: (1: heap, 2: N, 3: M, 4: Time)
                heap = m.group(1)
                n = int(m.group(2))
                # m_val is ignored, as requested (averaging over M for a given N)
                t = float(m.group(4))
                
                # Store time keyed only by N
                data[heap][n].append(t)
    except FileNotFoundError:
        return None, None

    # Calculate median and IQR for each heap and N
    stats = {}
    for heap in LABELS.keys():
        stats[heap] = {}
        for n, times in data[heap].items():
            times_arr = np.array(times)
            median = np.median(times_arr)
            q1 = np.percentile(times_arr, 25)
            q3 = np.percentile(times_arr, 75)
            iqr_lower = median - q1  # Distance from median to Q1
            iqr_upper = q3 - median  # Distance from median to Q3
            stats[heap][n] = (median, iqr_lower, iqr_upper)

    return stats, data

def plot_overall_average(stats_data):
    # stats_data[heap] = { n: (median, iqr_lower, iqr_upper) }
    
    out_dir = Path("outputs")
    out_dir.mkdir(exist_ok=True)
    
    plt.figure(figsize=(10, 6))

    for heap, label in LABELS.items():
        results = stats_data.get(heap)
        if not results:
            continue
            
        # Convert dictionary to list of (N, median, iqr_lower, iqr_upper) tuples
        points = [(n, median, iqr_lower, iqr_upper) 
                  for n, (median, iqr_lower, iqr_upper) in results.items()]
            
        # Sort by N for clean plotting
        points.sort(key=lambda x: x[0])
        ns = [p[0] for p in points]
        medians = [p[1] for p in points]
        iqr_lower = [p[2] for p in points]
        iqr_upper = [p[3] for p in points]
            
        # Plot with error bars showing IQR (interquartile range)
        plt.errorbar(ns, medians, yerr=[iqr_lower, iqr_upper], fmt="o", 
                     markersize=4, capsize=5, capthick=2,
                     label=label, color=COLORS.get(heap), alpha=0.8)

    plt.xlabel("Number of Nodes (N)", fontsize=12)
    plt.ylabel("Runtime (seconds)", fontsize=12)
    plt.title("Dijkstra Runtime Scaling (Median ± IQR)", fontsize=14)
        
    plt.legend(fontsize=11)
    plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.7)

    out_path = out_dir / f"dense.png"
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)
    print(f"Saved plot with deviation to: {out_path}")



def main(input_filename="IO/dense_output.txt"):
    # Allow passing the filename via command line arguments
    in_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(input_filename)
    
    if not in_path.exists():
        print(f"ERROR: cannot find {in_path}")
        sys.exit(1)

    # stats[heap][n] = (median, iqr_lower, iqr_upper)
    stats, raw = parse_and_average(in_path) 
    
    if stats is None:
        print(f"ERROR: Error reading or parsing {in_path}")
        sys.exit(1)

    # Sanity check: did we parse anything?
    total_points = sum(len(raw[h]) for h in raw)
    if total_points == 0:
        print("ERROR: no matching lines found. Expected format: 'Heap: b, N: 100, M: 500, Time: 0.0000330000'")
        sys.exit(1)

    # Generate the plot with deviation (IQR)
    plot_overall_average(stats)

if __name__ == "__main__":
    main()