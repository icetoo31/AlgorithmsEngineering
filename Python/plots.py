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
    "e": "#8B9556",
    "p": "#DD7230",
    "f": "#537D8D",
}

# Regular Expression to capture Heap, N, M, and Time
# Format: Heap: b, N: 100, M: 500, Time: 0.0000330000
LINE_RE = re.compile(r"Heap:\s*([bepf]),\s*N:\s*(\d+),\s*M:\s*(\d+),\s*Time:\s*([0-9.eE+-]+)")

def parse_and_average(path: Path):
    # data[heap][n] = list of times (from all M values for this N)
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

    # avg[heap] = { n: avg_time }
    avg = {}
    for heap in LABELS.keys():
        avg[heap] = {}
        # Iterate over all N values for this heap
        for n, times in data[heap].items():
            avg[heap][n] = sum(times) / len(times)

    return avg, data

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
            
        # Plot as scatter (no lines)
        plt.scatter(ns, avgs, s=5, label=label, color=COLORS.get(heap))
        # plt.plot(ns, avgs, label=label, color=COLORS.get(heap))

    plt.xlabel("Number of Nodes (N)")
    plt.ylabel("Average Runtime (seconds)")
    plt.title("Dijkstra Runtime Scaling (Overall Average Across Densities)")
        
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)

    out_path = out_dir / f"dense_measurements.png"
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)
    print(f"Saved scatter plot for Overall Average to: {out_path}")



def main(input_filename="IO/dense_output_2.txt"):
    # Allow passing the filename via command line arguments
    in_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(input_filename)
    
    if not in_path.exists():
        print(f"ERROR: cannot find {in_path}")
        sys.exit(1)

    # avg is now data[heap][n] = avg_time
    avg, raw = parse_and_average(in_path) 
    
    if avg is None:
        print(f"ERROR: Error reading or parsing {in_path}")
        sys.exit(1)

    # Sanity check: did we parse anything?
    total_points = sum(len(raw[h]) for h in raw)
    if total_points == 0:
        print("ERROR: no matching lines found. Expected format: 'Heap: b, N: 100, M: 500, Time: 0.0000330000'")
        sys.exit(1)

    # Generate the single plot for overall average
    plot_overall_average(avg)

if __name__ == "__main__":
    main()