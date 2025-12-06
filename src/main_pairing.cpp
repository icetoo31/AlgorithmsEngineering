#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <filesystem>

#include "pq_pairing_heap.cpp"
#include "load_graph_new.cpp"   // stream-based loader: load_graph(in)

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./run_pairing <inputfile>\n";
        return 1;
    }

    fs::path inputPath = argv[1];
    std::ifstream in(inputPath);
    if (!in.is_open()) {
        std::cerr << "ERROR: Cannot open input file: " << inputPath << "\n";
        return 1;
    }

    fs::create_directories("outputs");
    fs::path outPath = fs::path("outputs") / (inputPath.stem().string() + "_pairing.txt");

    std::ofstream out(outPath);
    if (!out.is_open()) {
        std::cerr << "ERROR: Cannot open output file: " << outPath << "\n";
        return 1;
    }

    int T;
    in >> T;

    out << T << "\n";
    out << std::fixed << std::setprecision(10);

    for (int t = 0; t < T; ++t) {
        auto adj = load_graph(in);
        int n = (int)adj.size();

        PairingHeapPQ pq;

        auto start = std::chrono::high_resolution_clock::now();
        auto dist  = dijkstra(n, adj, pq);
        auto end   = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;

        out << duration.count() << "\n";
        std::cout << "Test " << (t + 1) << ": " << duration.count() << " seconds\n";
    }

    std::cerr << "Wrote timings to: " << outPath << "\n";
    
    return 0;
}
