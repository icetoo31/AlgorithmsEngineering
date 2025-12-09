#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <chrono>
#include "headers/dijkstra.h"
#include "headers/pq_binary_heap.h"
#include "headers/pq_eightary_heap.h"
#include "headers/pq_pairing_heap.h"
#include "headers/pq_fibonacci_heap.h"

// Function to run Dijkstra's algorithm INDEPENDENTLY and return the time taken!
double runDijkstra(PQ& pq, const std::vector<std::vector<std::pair<int, int>>>& adj, int nodes) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> dist = dijkstra(nodes, adj, pq);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

int main() {
    std::ifstream inFile("IO/input.txt");
    std::ofstream outFile("IO/output.txt", std::ios::app);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open the input file." << std::endl;
        return 1;
    }
    
    if (!outFile.is_open()) {
        std::cerr << "Failed to open the output file." << std::endl;
        return 1;
    }

    std::string line;
    
    while (std::getline(inFile, line)) {
        if (line.empty()) continue; // Skip empty lines

        // Read number of nodes and edges
        std::istringstream firstLineStream(line);
        int nodes, edges;
        firstLineStream >> nodes >> edges;

        if (nodes <= 0 || edges < 0) {
            std::cerr << "Invalid graph parameters: nodes=" << nodes << ", edges=" << edges << std::endl;
            continue; // Skip this graph if parameters are invalid
        }

        std::vector<std::vector<std::pair<int, int>>> adj(nodes);

        // Read the edges
        for (int i = 0; i < edges; ++i) {
            if (!std::getline(inFile, line)) {
                std::cerr << "Edge format error or missing edge at line " << i + 2 << std::endl;
                break;  // Exit if we can't read all edges
            }
            std::istringstream edgeStream(line);
            int u, v, w;
            edgeStream >> u >> v >> w;

            if (u < 0 || v < 0 || u >= nodes || v >= nodes) {
                std::cerr << "Invalid edge: " << u << " -> " << v << " for graph with " << nodes << " nodes." << std::endl;
                continue; // Skip invalid edges
            }

            adj[u].push_back({v, w});
            //adj[v].push_back({u, w});  // For undirected graph
        }

        //Skip the comma separating graphs
        std::getline(inFile, line);


        //ACTUALLY IMPORTANT CODE:

        // ALL HEAP TYPES TO BE TESTED
        std::vector<std::pair<char, PQ*>> heapVariants = {
            {'b', new BinaryHeapPQ()},
            {'e', new EightAryHeapPQ()},
            {'p', new PairingHeapPQ()},
            {'f', new FibonacciHeapPQ()}
        };

        //WARM-UP THE CACHE
        for (const auto& heapVariant : heapVariants) {
            PQ* pq = heapVariant.second;
            runDijkstra(*pq, adj, nodes);  // Run once to warm up the cache
        }

        //FOR EACH HYPE TYPE
        for (const auto& heapVariant : heapVariants) {
            char heapType = heapVariant.first;
            PQ* pq = heapVariant.second;

            //RUN
            double duration = runDijkstra(*pq, adj, nodes);

            //OUTPUT
            outFile << heapType << " Nodes: " << nodes << " Time: " << std::scientific << duration << "\n";

            /*outFile << heapType << " ";
            for (int d : dijkstra(nodes, adj, *pq)) {
                outFile << d << " ";
            }
            outFile << "Time: " << std::scientific << duration << "\n";*/

            // GARBAGE COLLECT EACH PRIORITY QUEUE
            delete pq;
        }
    }

    // Close the files
    inFile.close();
    outFile.close();

    return 0;
}
