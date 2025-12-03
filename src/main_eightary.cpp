#include <iostream>
#include <string>

#include "pq_eightary_heap.cpp"
#include "load_graph.cpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./run_eightary <inputfile>\n";
        return 1;
    }

    std::string filename = argv[1];

    auto adj = load_graph(filename);
    int n = adj.size();

    EightAryHeapPQ pq;
    auto dist = dijkstra(n, adj, pq);

    for (int i = 0; i < n; i++)
        std::cout << "dist[" << i << "] = " << dist[i] << "\n";

    return 0;
}
