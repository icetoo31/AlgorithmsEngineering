import re
import sys
from pathlib import Path
from collections import defaultdict
import matplotlib.pyplot as plt

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

LINE_RE = re.compile(r"^\s*([bepf])\s+Nodes:\s*(\d+)\s+Time:\s*([0-9.eE+-]+)\s*$")

def parse_and_average(path: Path):
    # data[heap][n] = list of times
    data = {h: defaultdict(list) for h in LABELS.keys()}

    with path.open("r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            m = LINE_RE.match(line.strip())
            if not m:
                continue
            heap = m.group(1)
            n = int(m.group(2))
            t = float(m.group(3))
            data[heap][n].append(t)

    # avg[heap] = (sorted_ns, avg_times)
    avg = {}
    for heap in LABELS.keys():
        ns = sorted(data[heap].keys())
        avgs = []
        for n in ns:
            times = data[heap][n]
            avgs.append(sum(times) / len(times))
        avg[heap] = (ns, avgs)

    return avg, data

def main():
    in_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("IO") / "output.txt"
    if not in_path.exists():
        print(f"ERROR: cannot find {in_path}")
        sys.exit(1)

    avg, raw = parse_and_average(in_path)

    # sanity check: did we parse anything?
    total_points = sum(len(raw[h]) for h in raw)
    if total_points == 0:
        print("ERROR: no matching lines found. Expected format: 'b Nodes: 100 Time: 1.23e-06'")
        sys.exit(1)

    plt.figure()
    for heap, label in LABELS.items():
        ns, avgs = avg[heap]
        if not ns:
            continue
        plt.scatter(ns, avgs, s=12, label=label, color=COLORS.get(heap))

    plt.xlabel("Number of nodes (n)")
    plt.ylabel("Average runtime (seconds)")
    plt.title("Dijkstra average runtime per number of nodes")
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)

    out_dir = Path("outputs")
    out_dir.mkdir(exist_ok=True)
    out_path = out_dir / "avg_runtime_by_nodes.png"
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)

    print(f"Saved plot to: {out_path}")

if __name__ == "__main__":
    main()
