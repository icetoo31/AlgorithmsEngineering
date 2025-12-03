#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>

using Graph = std::vector<std::vector<std::pair<int,int>>>;

// Reads a graph in the format:
// n m
// u v w
// u v w
// ...
Graph load_graph(const std::string &filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "ERROR: Cannot open file " << filename << std::endl;
        exit(1);
    }

    int n, m;
    in >> n >> m;

    Graph adj(n);

    for (int i = 0; i < m; ++i) {
        int u, v, w;
        in >> u >> v >> w;
        adj[u].push_back({v, w});
    }

    return adj;
}
