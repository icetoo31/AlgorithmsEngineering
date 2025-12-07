import random
import os

# ---------------- CONFIGURATION ----------------

OUTPUT_DIR = "IO"
OUTPUT_FILE = "input.txt"

SEED = 42

W_MIN = 1
W_MAX = 100

EDGE_FACTOR = 3      # sparse: m = 3n
GRAPHS_PER_SIZE = 100

random.seed(SEED)

# -----------------------------------------------

def sparse_graph(n):
    m = min(EDGE_FACTOR * n, n * (n - 1))
    edges = set()

    while len(edges) < m:
        u = random.randrange(n)
        v = random.randrange(n)
        if u != v:
            edges.add((u, v, random.randint(W_MIN, W_MAX)))

    return n, list(edges)


def generate():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    path = os.path.join(OUTPUT_DIR, OUTPUT_FILE)

    with open(path, "w") as f:
        for n in range(10, 1001, 10):
            for _ in range(GRAPHS_PER_SIZE):
                n_nodes, edges = sparse_graph(n)

                f.write(f"{n_nodes} {len(edges)}\n")

                for i, (u, v, w) in enumerate(edges):
                    if i == len(edges) - 1:
                        f.write(f"{u} {v} {w},\n")
                    else:
                        f.write(f"{u} {v} {w}\n")

                f.write("\n")

    print(f"Generated {path}")


if __name__ == "__main__":
    generate()
