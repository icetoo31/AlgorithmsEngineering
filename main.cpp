#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <iomanip>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <random> 

// Assuming these are defined in your headers/source files
#include "headers/dijkstra.h"
#include "headers/pq_binary_heap.h"
#include "headers/pq_eightary_heap.h"
#include "headers/pq_pairing_heap.h"
#include "headers/pq_fibonacci_heap.h"

// Define the graph structure (Adjacency List)
using Graph = std::vector<std::vector<std::pair<int, int>>>;

// --- Function to Generate Graph In-Memory (Unchanged) ---
Graph generate_random_graph(int num_nodes, int num_edges, int max_weight) {
    Graph adj(num_nodes);
    
    for (int i = 0; i < num_edges; ++i) {
        int u = std::rand() % num_nodes; 
        int v = std::rand() % num_nodes; 
        int weight = 1 + (std::rand() % max_weight); 
        adj[u].push_back({v, weight});
    }
    return adj;
}

// --- Benchmarking/Running Function (Unchanged) ---
std::vector<int> runDijkstra(PQ& pq, const Graph& adj, int nodes, double& duration) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> dist = dijkstra(nodes, adj, pq); 
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> chrono_duration = end - start;
    duration = chrono_duration.count();
    return dist;
}

// ------------------------------------------------------------------
// --- Automated Test Case Generator (Unchanged) ---
// ------------------------------------------------------------------

/**
 * @brief Generates a list of (Nodes, Edges) pairs based on scaling parameters using a linear step size for N.
 */
std::vector<std::pair<int, int>> generate_test_cases(
    int start_N, 
    int end_N, 
    int step_N,
    double edge_ratio_multiplier,
    double edge_ratio_exponent
) {
    std::vector<std::pair<int, int>> testCases;
    
    for (int current_N = start_N; current_N <= end_N; current_N += step_N) {
        
        double edges_float = edge_ratio_multiplier * std::pow(current_N, edge_ratio_exponent);
        int current_M = static_cast<int>(std::round(edges_float));
        
        if (current_M < 1) current_M = 1;
        
        testCases.push_back({current_N, current_M});
    }

    return testCases;
}


// --- Main Benchmarking Loop ---

int main() {
    // Initialization
    std::srand(std::time(0));
    std::mt19937 rng(std::time(0));  // Random number generator for shuffling heap order
    const int NUM_TRIALS = 3; 
    const int MAX_WEIGHT = 1000;
    
    // Clean up IO folder - delete all files before starting
    try {
        for (const auto& entry : std::filesystem::directory_iterator("IO")) {
            if (std::filesystem::is_regular_file(entry.path())) {
                std::filesystem::remove(entry.path());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error cleaning IO folder: " << e.what() << std::endl;
    }
    
    // File Streams
    // Note: ios::trunc is added to verificationFile to ensure it only contains the first run's output.
    std::ofstream sparseOutFile("IO/sparse_output.txt", std::ios::app);
    std::ofstream denseOutFile("IO/dense_output.txt", std::ios::app);
    std::ofstream verificationFile("IO/verification.txt", std::ios::trunc); 

    if (!sparseOutFile.is_open() || !denseOutFile.is_open() || !verificationFile.is_open()) {
        std::cerr << "Failed to open one or more output files." << std::endl;
        return 1;
    }
    sparseOutFile << std::fixed << std::setprecision(10); 
    denseOutFile << std::fixed << std::setprecision(10); 

    // ----------------------------------------------------
    // --- BENCHMARK CONFIGURATION ---
    // ----------------------------------------------------

    // 1. O(N) Sparsity: M is proportional to N (e.g., M = 5N)
    std::vector<std::pair<int, int>> sparseCases = generate_test_cases(
        200000,            // start_N
        50001,          // end_N
        2000,            // step_N
        3.0,            // multiplier (c=2)
        1.0             // exponent (k=1.0 for O(N) -> M = 2*N)
    );

    // 2. O(N^2) Density: M is proportional to N^2 (e.g., M = 0.5% of N^2)
    std::vector<std::pair<int, int>> denseCases = generate_test_cases(
        2000,            // start_N
        30001,           // end_N
        2000,            // step_N
        0.5,          
        2.0             // exponent (k=2.0 for O(N^2) -> M = N^2)
    );
    
    // ----------------------------------------------------
    // --- RUN BENCHMARK ---
    // ----------------------------------------------------
    
    // This flag ensures the verification output only happens for the first test run, 
    // regardless of whether it's the sparse or dense set.
    bool is_first_trial_ever = true; 

    // Helper function to run a set of test cases
    auto run_benchmark_set = [&](const std::vector<std::pair<int, int>>& cases, std::ofstream& output_file) {
        
        for (const auto& testCase : cases) {
            int nodes = testCase.first;
            int edges = testCase.second;

            if (nodes <= 0 || edges <= 0) continue;
            
            std::cout << "Testing N = " << nodes << std::endl;

            // Storage for times across all trials for each heap type
            std::vector<double> binaryTimes, eightaryTimes, pairingTimes, fibonacciTimes;
            std::vector<int> first_distances; // For verification

            // --- Trials for a single (N, M) configuration ---
            for (int trial = 1; trial <= NUM_TRIALS; ++trial) {
                
                // 1. GENERATE A NEW RANDOM GRAPH FOR THIS TRIAL
                Graph adj = generate_random_graph(nodes, edges, MAX_WEIGHT);
                
                // 2. Re-create PQs for the official timing run
                std::vector<std::pair<char, PQ*>> runVariants = {
                    {'b', new BinaryHeapPQ()},
                    {'e', new EightAryHeapPQ()},
                    {'p', new PairingHeapPQ()},
                    {'f', new FibonacciHeapPQ()}
                };
                
                // Shuffle the heap order to avoid sequential execution bias
                std::shuffle(runVariants.begin(), runVariants.end(), rng);

                // 3. FOR EACH HEAP TYPE, RUN THE BENCHMARK
                for (const auto& heapVariant : runVariants) {
                    char heapType = heapVariant.first;
                    PQ* pq = heapVariant.second;
                    double duration = 0.0;

                    std::vector<int> distances = runDijkstra(*pq, adj, nodes, duration);

                    // Store times for median calculation
                    if (heapType == 'b') binaryTimes.push_back(duration);
                    else if (heapType == 'e') eightaryTimes.push_back(duration);
                    else if (heapType == 'p') pairingTimes.push_back(duration);
                    else if (heapType == 'f') fibonacciTimes.push_back(duration);

                    // --- DISTANCE VERIFICATION OUTPUT (Only first trial EVER) ---
                    if (is_first_trial_ever) {
                        // The verification file will be overwritten for the first 
                        // run of each heap type, ensuring all four outputs are captured
                        // for the very first generated graph.
                        verificationFile << "--- Heap Type: " << heapType << " (N=" << nodes << ", M=" << edges << ") ---\n";
                        verificationFile << "Trial 1 Distances from Source (Node 0):\n";
                        for (int j = 0; j < nodes; ++j) {
                            verificationFile << "Node " << j << ": " << distances[j] << "\n";
                        }
                        verificationFile << "\n";
                    }
                    
                    delete pq;
                }
                
                // --- Set the flag to false after the first trial of the first case is complete ---
                if (is_first_trial_ever) {
                    is_first_trial_ever = false; 
                }
                
            } // End of NUM_TRIALS loop

            // Output all trial times for each heap type (for deviation calculation in plotting)
            for (double t : binaryTimes) {
                output_file << "Heap: b, N: " << nodes << ", M: " << edges 
                           << ", Time: " << t << "\n";
            }
            for (double t : eightaryTimes) {
                output_file << "Heap: e, N: " << nodes << ", M: " << edges 
                           << ", Time: " << t << "\n";
            }
            for (double t : pairingTimes) {
                output_file << "Heap: p, N: " << nodes << ", M: " << edges 
                           << ", Time: " << t << "\n";
            }
            for (double t : fibonacciTimes) {
                output_file << "Heap: f, N: " << nodes << ", M: " << edges 
                           << ", Time: " << t << "\n";
            }

            output_file << "\n"; // Separator between different graph sizes (N, M)
        } // End of cases loop
    };

    // Run Sparse Cases (Assumed to be the first set run)
    run_benchmark_set(sparseCases, sparseOutFile);

    // Run Dense Cases
    run_benchmark_set(denseCases, denseOutFile);


    // Close all files
    sparseOutFile.close();
    denseOutFile.close();
    verificationFile.close();
    
    // All terminal prints have been removed.

    return 0;
}