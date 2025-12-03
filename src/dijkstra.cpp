#include <vector>
#include <utility>
#include <limits>

// ------------------------------------------------------------
// Priority Queue Interface (no header file needed)
// ------------------------------------------------------------

struct PQ {
    virtual void push(int node, int dist) = 0;
    virtual std::pair<int,int> pop() = 0;    // returns (dist, node)
    virtual bool empty() = 0;
    virtual void decrease_key(int node, int newDist) = 0;
    virtual ~PQ() {}
};

// ------------------------------------------------------------
// Universal Dijkstra using the PQ interface
// ------------------------------------------------------------

std::vector<int> dijkstra(
    int n,
    const std::vector<std::vector<std::pair<int,int>>>& adj,
    PQ &pq
) {
    const int INF = std::numeric_limits<int>::max();

    std::vector<int> dist(n, INF);
    dist[0] = 0;

    pq.push(0, 0);

    while (!pq.empty()) {

        auto [d, u] = pq.pop();

        // Outdated entry — skip it
        if (d != dist[u])
            continue;

        // Relax edges
        for (const auto &edge : adj[u]) {
            int v = edge.first;
            int w = edge.second;

            if (dist[v] > d + w) {
                dist[v] = d + w;
                pq.decrease_key(v, dist[v]);   // handles push OR decrease-key
            }
        }
    }

    return dist;
}
