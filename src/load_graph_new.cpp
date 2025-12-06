#include <vector>
#include <istream>
#include <utility>
using Graph = std::vector<std::vector<std::pair<int,int>>>;

// Reads a graph in the format:
// n m
// u v w
// u v w
// ...
// Reads ONE graph from the stream
Graph load_graph(std::istream &in) {
    int n, m;
    in >> n >> m;

    Graph adj(n);

    for (int i = 0; i < m; i++) {
        int u, v, w;
        in >> u >> v >> w;
        adj[u].push_back({v, w});
    }

    return adj;
}
