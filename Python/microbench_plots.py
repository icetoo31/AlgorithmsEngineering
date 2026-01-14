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
    "b": "#0072B2",  # Blue
    "e": "#E69F00",  # Orange
    "p": "#009E73",  # Green
    "f": "#D55E00",  # Red-Orange
}

# Regular Expression to capture Heap, N, M, and Time
# Format: Heap: b, N: 2000, M: 6000, Time: 0.001234
LINE_RE = re.compile(r"Heap:\s*([bepf]),\s*N:\s*(\d+),\s*M:\s*(\d+),\s*Time:\s*([0-9.eE+-]+)")

def parse_microbench(path: Path, operation_type: str):
    """Parse microbenchmark output and calculate per-operation stats
    
    operation_type: 'insert', 'extract_min', or 'decrease_key'
    """
    data = {h: defaultdict(list) for h in LABELS.keys()}

    try:
        with path.open("r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                m = LINE_RE.match(line.strip())
                if not m:
                    continue
                
                heap = m.group(1)
                n = int(m.group(2))
                edges = int(m.group(3))  # M value
                total_time = float(m.group(4))
                
                # Calculate per-operation time based on operation type
                if operation_type == 'insert':
                    per_op_time = (total_time / n) * 1e6  # N inserts, convert to microseconds
                elif operation_type == 'extract_min':
                    per_op_time = (total_time / n) * 1e6  # N extracts, convert to microseconds
                elif operation_type == 'decrease_key':
                    per_op_time = (total_time / edges) * 1e6  # M decrease-keys, convert to microseconds
                else:
                    per_op_time = total_time * 1e6
                
                data[heap][n].append(per_op_time)
    except FileNotFoundError:
        return None

    # Calculate median and IQR for each heap and N
    stats = {}
    for heap in LABELS.keys():
        stats[heap] = {}
        for n, times in data[heap].items():
            times_arr = np.array(times)
            median = np.median(times_arr)
            q1 = np.percentile(times_arr, 25)
            q3 = np.percentile(times_arr, 75)
            iqr_lower = median - q1
            iqr_upper = q3 - median
            stats[heap][n] = (median, iqr_lower, iqr_upper)

    return stats

def plot_microbench(stats_data, operation_name, output_filename):
    """Plot microbenchmark results"""
    out_dir = Path("outputs")
    out_dir.mkdir(exist_ok=True)
    
    plt.figure(figsize=(10, 6))

    for heap, label in LABELS.items():
        results = stats_data.get(heap)
        if not results:
            continue
        
        # Convert to sorted list
        points = [(n, median, iqr_lower, iqr_upper) 
                  for n, (median, iqr_lower, iqr_upper) in results.items()]
        points.sort(key=lambda x: x[0])
        
        ns = [p[0] for p in points]
        medians = [p[1] for p in points]
        iqr_lower = [p[2] for p in points]
        iqr_upper = [p[3] for p in points]
        
        # Plot with error bars
        plt.errorbar(ns, medians, yerr=[iqr_lower, iqr_upper], fmt="o", 
                     markersize=4, capsize=5, capthick=2,
                     label=label, color=COLORS.get(heap), alpha=0.8)

    plt.xlabel("Number of Nodes (N)", fontsize=12)
    plt.ylabel("Time per Operation (μs)", fontsize=12)
    plt.title(f"{operation_name} (Median ± IQR)", fontsize=14)
    
    plt.legend(fontsize=11)
    plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.7)

    out_path = out_dir / output_filename
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)
    print(f"Saved plot to: {out_path}")
    plt.close()

def main():
    graph_types = ["sparse", "dense"]
    
    for graph_type in graph_types:
        print(f"\nProcessing {graph_type} graphs...")
        
        operations = [
            (f"IO/microbench_insert_{graph_type}.txt", "insert", "Insert", f"microbench_insert_{graph_type}.png"),
            (f"IO/microbench_extract_min_{graph_type}.txt", "extract_min", "Extract-Min", f"microbench_extract_min_{graph_type}.png"),
            (f"IO/microbench_decrease_key_{graph_type}.txt", "decrease_key", "Decrease-Key", f"microbench_decrease_key_{graph_type}.png"),
        ]
        
        for input_file, op_type, op_name, output_file in operations:
            in_path = Path(input_file)
            
            if not in_path.exists():
                print(f"WARNING: {in_path} not found. Run microbench first.")
                continue
            
            stats = parse_microbench(in_path, op_type)
            
            if stats is None or not any(stats.values()):
                print(f"WARNING: No data found in {in_path}")
                continue
            
            plot_microbench(stats, f"{op_name} ({graph_type.capitalize()} Graphs)", output_file)

if __name__ == "__main__":
    main()
