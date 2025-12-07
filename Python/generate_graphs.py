import random
import os

# --------------------- CONFIGURATION ---------------------

OUTPUT_DIR = "IO/PythonGraphs"
SEED = 42

# Graph sizes & repetitions per file
N = 20
TESTS_PER_TYPE = 10

# Weight range
W_MIN = 1
W_MAX = 100

random.seed(SEED)

# -------------------------------------------------------


def write_test_file(filename, test_cases):
    with open(filename, "w") as f:
        f.write(f"{len(test_cases)}\n")
        for n, edges in test_cases:
            f.write(f"{n} {len(edges)}\n")
            for u, v, w in edges:
                f.write(f"{u} {v} {w}\n")


# =======================================================
# GRAPH GENERATORS
# =======================================================

def random_graph(n, m):
    edges = set()
    while len(edges) < m:
        u = random.randrange(n)
        v = random.randrange(n)
        if u != v:
            edges.add((u, v, random.randint(W_MIN, W_MAX)))
    return n, list(edges)


def directed_tree(n):
    edges = []
    for v in range(1, n):
        parent = random.randint(0, v - 1)
        edges.append((parent, v, random.randint(W_MIN, W_MAX)))
    return n, edges


def dag_graph(n, m):
    edges = set()
    while len(edges) < m:
        u = random.randrange(n)
        v = random.randrange(n)
        if u < v:
            edges.add((u, v, random.randint(W_MIN, W_MAX)))
    return n, list(edges)


def reverse_edge_graph(n, m):
    edges = set()
    while len(edges) < m:
        u = random.randrange(n)
        v = random.randrange(n)
        if u != v:
            w = random.randint(W_MIN, W_MAX)
            edges.add((u, v, w))
            edges.add((v, u, w))
    return n, list(edges)


def multi_edge_graph(n, m):
    edges = []
    for _ in range(m):
        u = random.randrange(n)
        v = random.randrange(n)
        if u != v:
            edges.append((u, v, random.randint(W_MIN, W_MAX)))
    return n, edges


def disconnected_graph(n, components=4):
    edges = []
    size = n // components

    for c in range(components):
        start = c * size
        end = start + size if c < components - 1 else n

        for v in range(start + 1, end):
            parent = random.randint(start, v - 1)
            edges.append((parent, v, random.randint(W_MIN, W_MAX)))
    return n, edges


def chain_with_shortcuts(n, shortcuts_factor=3):
    edges = []

    # Main chain
    for i in range(n - 1):
        edges.append((i, i + 1, random.randint(W_MIN, W_MAX)))

    # Shortcuts (forces decrease-key)
    for i in range(n):
        for _ in range(shortcuts_factor):
            j = random.randint(i + 1, n - 1) if i + 1 < n else None
            if j:
                edges.append((i, j, random.randint(W_MIN, W_MAX)))

    return n, edges


def star_graph(n):
    edges = []
    center = 0
    for v in range(1, n):
        edges.append((center, v, random.randint(W_MIN, W_MAX)))
    return n, edges


# =======================================================
# FILE GENERATION
# =======================================================

def generate_all():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    graph_sets = {
        "random_sparse.txt": lambda: random_graph(N, N * 2),
        "random_dense.txt": lambda: random_graph(N, N * 10),
        "tree.txt": lambda: directed_tree(N),
        "dag.txt": lambda: dag_graph(N, N * 3),
        "reverse_edges.txt": lambda: reverse_edge_graph(N, N * 3),
        "multi_edges.txt": lambda: multi_edge_graph(N, N * 5),
        "disconnected.txt": lambda: disconnected_graph(N, 5),
        "chain_shortcuts.txt": lambda: chain_with_shortcuts(N),
        "star.txt": lambda: star_graph(N),
    }

    for filename, generator in graph_sets.items():
        tests = [generator() for _ in range(TESTS_PER_TYPE)]
        write_test_file(os.path.join(OUTPUT_DIR, filename), tests)
        print(f"Generated {filename} ({TESTS_PER_TYPE} tests)")


if __name__ == "__main__":
    generate_all()