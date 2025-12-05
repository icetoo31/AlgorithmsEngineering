#include <iostream>
#include <string>

#include "pq_eightary_heap.cpp"
#include "load_graph.cpp"
#include <chrono>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./run_eightary <inputfile>\n";
        return 1;
    }

    std::string filename = argv[1];

    auto adj = load_graph(filename);
    int n = adj.size();

    EightAryHeapPQ pq;

    auto start = std::chrono::high_resolution_clock::now();
    auto dist = dijkstra(n, adj, pq);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    std::cout << "Dijkstra algorithm took " << duration.count() << " seconds.\n";

    for (int i = 0; i < n; i++)
        std::cout << "dist[" << i << "] = " << dist[i] << "\n";

    return 0;
}
