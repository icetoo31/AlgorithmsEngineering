#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <random>
#include <filesystem>

// Assuming these are defined in your headers/source files
#include "headers/pq_binary_heap.h"
#include "headers/pq_eightary_heap.h"
#include "headers/pq_pairing_heap.h"
#include "headers/pq_fibonacci_heap.h"

// ============================================================
// Microbenchmark: Test operations with graph-realistic patterns
// ============================================================

// Helper: Measure time for insert operation (N inserts - one per node)
double benchmark_insert(PQ& pq, int n, int max_dist) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        int dist = 1 + (std::rand() % max_dist);
        pq.push(i, dist);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

// Helper: Measure time for extract-min operation (N extracts)
double benchmark_extract_min(PQ& pq, int n) {
    auto start = std::chrono::high_resolution_clock::now();
    while (!pq.empty()) {
        pq.pop();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

// Helper: Measure time for decrease-key operation (M decrease-keys, like edge relaxations)
double benchmark_decrease_key(PQ& pq, int n, int m, int max_dist) {
    // Pre-populate the heap
    for (int i = 0; i < n; ++i) {
        int dist = 500 + (std::rand() % max_dist);
        pq.push(i, dist);
    }
    
    // Now measure M decrease-key operations (simulating edge relaxations)
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < m; ++i) {
        int node = std::rand() % n;
        int new_dist = std::rand() % 500;  // Smaller value to ensure actual decrease
        pq.decrease_key(node, new_dist);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

int main() {
    std::srand(std::time(0));
    std::mt19937 rng(std::time(0));
    
    const int NUM_TRIALS = 10;
    const int MAX_WEIGHT = 1000;
    
    // Clean up IO folder
    try {
        for (const auto& entry : std::filesystem::directory_iterator("IO")) {
            if (std::filesystem::is_regular_file(entry.path())) {
                std::filesystem::remove(entry.path());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error cleaning IO folder: " << e.what() << std::endl;
    }
    
    // Open output files for each operation (separate for sparse and dense)
    std::ofstream insert_sparse("IO/microbench_insert_sparse.txt", std::ios::app);
    std::ofstream extract_sparse("IO/microbench_extract_min_sparse.txt", std::ios::app);
    std::ofstream decrease_sparse("IO/microbench_decrease_key_sparse.txt", std::ios::app);
    
    std::ofstream insert_dense("IO/microbench_insert_dense.txt", std::ios::app);
    std::ofstream extract_dense("IO/microbench_extract_min_dense.txt", std::ios::app);
    std::ofstream decrease_dense("IO/microbench_decrease_key_dense.txt", std::ios::app);
    
    if (!insert_sparse.is_open() || !extract_sparse.is_open() || !decrease_sparse.is_open() ||
        !insert_dense.is_open() || !extract_dense.is_open() || !decrease_dense.is_open()) {
        std::cerr << "Failed to open output files." << std::endl;
        return 1;
    }
    
    insert_sparse << std::fixed << std::setprecision(10);
    extract_sparse << std::fixed << std::setprecision(10);
    decrease_sparse << std::fixed << std::setprecision(10);
    insert_dense << std::fixed << std::setprecision(10);
    extract_dense << std::fixed << std::setprecision(10);
    decrease_dense << std::fixed << std::setprecision(10);
    
    // Test parameters
    std::vector<int> test_sizes = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000};
    
    std::cout << "Starting microbenchmarks..." << std::endl;
    
    for (int n : test_sizes) {
        std::cout << "Testing N = " << n << std::endl;
        
        // Calculate sparse and dense edge counts
        int m_sparse = 3 * n;                          // M = 3N
        int m_dense = static_cast<int>(0.5 * n * n);   // M = N²/2
        
        for (int trial = 0; trial < NUM_TRIALS; ++trial) {
            // Define heap factories
            auto heaps = std::vector<std::pair<char, PQ*>> {
                {'b', new BinaryHeapPQ()},
                {'e', new EightAryHeapPQ()},
                {'p', new PairingHeapPQ()},
                {'f', new FibonacciHeapPQ()}
            };
            
            // Shuffle order to avoid bias
            std::shuffle(heaps.begin(), heaps.end(), rng);
            
            for (auto& [heap_char, pq] : heaps) {
                // ==================== SPARSE GRAPHS ====================
                // Test INSERT (N inserts for sparse)
                PQ* pq_insert_sparse = nullptr;
                if (heap_char == 'b') pq_insert_sparse = new BinaryHeapPQ();
                else if (heap_char == 'e') pq_insert_sparse = new EightAryHeapPQ();
                else if (heap_char == 'p') pq_insert_sparse = new PairingHeapPQ();
                else if (heap_char == 'f') pq_insert_sparse = new FibonacciHeapPQ();
                
                double insert_time_sparse = benchmark_insert(*pq_insert_sparse, n, MAX_WEIGHT);
                insert_sparse << "Heap: " << heap_char << ", N: " << n << ", M: " << m_sparse << ", Time: " << insert_time_sparse << "\n";
                delete pq_insert_sparse;
                
                // Test EXTRACT-MIN (N extracts for sparse)
                PQ* pq_extract_sparse = nullptr;
                if (heap_char == 'b') pq_extract_sparse = new BinaryHeapPQ();
                else if (heap_char == 'e') pq_extract_sparse = new EightAryHeapPQ();
                else if (heap_char == 'p') pq_extract_sparse = new PairingHeapPQ();
                else if (heap_char == 'f') pq_extract_sparse = new FibonacciHeapPQ();
                
                // Populate heap first
                for (int i = 0; i < n; ++i) {
                    int dist = 1 + (std::rand() % MAX_WEIGHT);
                    pq_extract_sparse->push(i, dist);
                }
                
                double extract_time_sparse = benchmark_extract_min(*pq_extract_sparse, n);
                extract_sparse << "Heap: " << heap_char << ", N: " << n << ", M: " << m_sparse << ", Time: " << extract_time_sparse << "\n";
                delete pq_extract_sparse;
                
                // Test DECREASE-KEY (M = 3N decrease-keys for sparse)
                PQ* pq_decrease_sparse = nullptr;
                if (heap_char == 'b') pq_decrease_sparse = new BinaryHeapPQ();
                else if (heap_char == 'e') pq_decrease_sparse = new EightAryHeapPQ();
                else if (heap_char == 'p') pq_decrease_sparse = new PairingHeapPQ();
                else if (heap_char == 'f') pq_decrease_sparse = new FibonacciHeapPQ();
                
                double decrease_time_sparse = benchmark_decrease_key(*pq_decrease_sparse, n, m_sparse, MAX_WEIGHT);
                decrease_sparse << "Heap: " << heap_char << ", N: " << n << ", M: " << m_sparse << ", Time: " << decrease_time_sparse << "\n";
                delete pq_decrease_sparse;
                
                // ==================== DENSE GRAPHS ====================
                // Test INSERT (N inserts for dense)
                PQ* pq_insert_dense = nullptr;
                if (heap_char == 'b') pq_insert_dense = new BinaryHeapPQ();
                else if (heap_char == 'e') pq_insert_dense = new EightAryHeapPQ();
                else if (heap_char == 'p') pq_insert_dense = new PairingHeapPQ();
                else if (heap_char == 'f') pq_insert_dense = new FibonacciHeapPQ();
                
                double insert_time_dense = benchmark_insert(*pq_insert_dense, n, MAX_WEIGHT);
                insert_dense << "Heap: " << heap_char << ", N: " << n << ", M: " << m_dense << ", Time: " << insert_time_dense << "\n";
                delete pq_insert_dense;
                
                // Test EXTRACT-MIN (N extracts for dense)
                PQ* pq_extract_dense = nullptr;
                if (heap_char == 'b') pq_extract_dense = new BinaryHeapPQ();
                else if (heap_char == 'e') pq_extract_dense = new EightAryHeapPQ();
                else if (heap_char == 'p') pq_extract_dense = new PairingHeapPQ();
                else if (heap_char == 'f') pq_extract_dense = new FibonacciHeapPQ();
                
                // Populate heap first
                for (int i = 0; i < n; ++i) {
                    int dist = 1 + (std::rand() % MAX_WEIGHT);
                    pq_extract_dense->push(i, dist);
                }
                
                double extract_time_dense = benchmark_extract_min(*pq_extract_dense, n);
                extract_dense << "Heap: " << heap_char << ", N: " << n << ", M: " << m_dense << ", Time: " << extract_time_dense << "\n";
                delete pq_extract_dense;
                
                // Test DECREASE-KEY (M = N²/2 decrease-keys for dense)
                PQ* pq_decrease_dense = nullptr;
                if (heap_char == 'b') pq_decrease_dense = new BinaryHeapPQ();
                else if (heap_char == 'e') pq_decrease_dense = new EightAryHeapPQ();
                else if (heap_char == 'p') pq_decrease_dense = new PairingHeapPQ();
                else if (heap_char == 'f') pq_decrease_dense = new FibonacciHeapPQ();
                
                double decrease_time_dense = benchmark_decrease_key(*pq_decrease_dense, n, m_dense, MAX_WEIGHT);
                decrease_dense << "Heap: " << heap_char << ", N: " << n << ", M: " << m_dense << ", Time: " << decrease_time_dense << "\n";
                delete pq_decrease_dense;
                
                delete pq;
            }
        }
    }
    
    insert_sparse.close();
    extract_sparse.close();
    decrease_sparse.close();
    insert_dense.close();
    extract_dense.close();
    decrease_dense.close();
    
    std::cout << "Microbenchmarks complete." << std::endl;
    std::cout << "Output files:" << std::endl;
    std::cout << "  Sparse: IO/microbench_*_sparse.txt" << std::endl;
    std::cout << "  Dense: IO/microbench_*_dense.txt" << std::endl;
    
    return 0;
}
