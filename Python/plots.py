import re
import sys
from pathlib import Path
import matplotlib.pyplot as plt


LABELS = {
    "b": "Binary heap",
    "e": "8-ary heap",
    "p": "Pairing heap",
    "f": "Fibonacci heap",
}

COLORS = {
    "b": "tab:blue",
    "e": "tab:orange",
    "p": "tab:green",
    "f": "tab:pink",
}

LINE_RE = re.compile(r"^\s*([bepf])\s+Time:\s*([0-9.eE+-]+)\s*$")


def parse_output_file(path: Path):
    # times[heap] = [(test_idx, time), ...]
    times = {k: [] for k in LABELS.keys()}
    test_idx = 0

    with path.open("r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            m = LINE_RE.match(line)
            if not m:
                continue
            heap = m.group(1)
            t = float(m.group(2))

            if heap == "b":
                test_idx += 1

            times[heap].append((test_idx, t))

    return times, test_idx


def main():
    in_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("IO") / "output.txt"
    if not in_path.exists():
        print(f"ERROR: cannot find {in_path}")
        sys.exit(1)

    times, num_tests = parse_output_file(in_path)
    if num_tests == 0:
        print("ERROR: no matching timing lines found in output.txt")
        sys.exit(1)

    plt.figure()
    for heap, label in LABELS.items():
        pts = times[heap]
        if not pts:
            continue
        xs = [x for x, _ in pts]
        ys = [y for _, y in pts]
        plt.plot(xs, ys, marker="o", label=label, color=COLORS.get(heap, None))

    plt.xlabel("Input graphs")
    plt.ylabel("Runtime (seconds)")
    plt.title(f"Dijkstra runtime")
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)

    out_dir = Path("outputs")
    out_dir.mkdir(exist_ok=True)
    out_path = out_dir / "runtime_comparison.png"
    plt.tight_layout()
    plt.savefig(out_path, dpi=200)

    print(f"Saved plot to: {out_path}")


if __name__ == "__main__":
    main()
