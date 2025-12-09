import random
import os

# ---------------- CONFIGURATION ----------------

OUTPUT_DIR = "IO"
OUTPUT_FILE = "input.txt"

SEED = 42

W_MIN = 1
W_MAX = 100

GRAPHS_PER_SIZE = 20

random.seed(SEED)

# -----------------------------------------------

def directed_tree(n):
    edges = []

    for v in range(1, n):
        parent = random.randint(0, v - 1)   # ensures acyclic & connected
        edges.append((parent, v, random.randint(W_MIN, W_MAX)))

    return n, edges


def generate():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    path = os.path.join(OUTPUT_DIR, OUTPUT_FILE)

    with open(path, "w") as f:
        for n in range(10, 10001, 10):
            for _ in range(GRAPHS_PER_SIZE):
                n_nodes, edges = directed_tree(n)

                f.write(f"{n_nodes} {len(edges)}\n")

                for i, (u, v, w) in enumerate(edges):
                    if i == len(edges) - 1:
                        f.write(f"{u} {v} {w},\n")
                    else:
                        f.write(f"{u} {v} {w}\n")

                f.write("\n")  # blank line between graphs

    print(f"Generated {path}")


if __name__ == "__main__":
    generate()
