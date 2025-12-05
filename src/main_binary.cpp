#include <iostream>
#include <string>
#include "pq_binary_heap.cpp"
#include "load_graph.cpp"
#include <chrono>  // For timing

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./run_binary <inputfile>\n";
        return 1;
    }

    std::string filename = argv[1];
    
    auto adj = load_graph(filename);
    int n = adj.size();

    BinaryHeapPQ pq;

    auto start = std::chrono::high_resolution_clock::now();
    auto dist = dijkstra(n, adj, pq);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    std::cout << "Dijkstra algorithm took " << duration.count() << " seconds.\n";

    for (int i = 0; i < n; i++)
        std::cout << "dist[" << i << "] = " << dist[i] << "\n";



    std::ofstream outFile("output.txt");  // You can change the filename here

    if (!outFile) {
        std::cerr << "Error opening output file.\n";
        return 1;
    }

    outFile << "Dijkstra algorithm took " << duration.count() << " seconds.\n";

    for (int i = 0; i < n; i++) {
        outFile << "dist[" << i << "] = " << dist[i] << "\n";
    }

    // Close the file (optional, but a good practice)
    outFile.close();
}
