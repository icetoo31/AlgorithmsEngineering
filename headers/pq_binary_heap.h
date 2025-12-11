#include <vector>
#include <utility>
#include <limits>
#include <algorithm>
#include "pq.h" // Assuming pq.h defines the base class PQ

// Define a constant for an "invalid" index to mark nodes not in the heap
inline constexpr int NOT_IN_HEAP = -1;

struct BinaryHeapPQ : PQ {

    // --------------------------------------------------------
    // Entry structure: stores dist, node ID, and its current index in the heap array
    // --------------------------------------------------------
    struct Entry {
        int dist;
        int node;
        int index_in_heap; // Points to its own position in the 'heap' vector
    };

    // The actual heap storage (min-heap)
    std::vector<Entry*> heap;

    // The mapping from graph node ID to its entry's current position in the 'heap' vector
    std::vector<int> node_to_index;

    // Max number of nodes in the graph (needed for vector size)
    int max_nodes = 0;

    // --------------------------------------------------------
    // Heap Helper Functions
    // --------------------------------------------------------

    void swap_entries(int i, int j) {
        Entry* ei = heap[i];
        Entry* ej = heap[j];
        
        std::swap(heap[i], heap[j]);

        ei->index_in_heap = j;
        ej->index_in_heap = i;

        node_to_index[ei->node] = j;
        node_to_index[ej->node] = i;
    }
    
    void bubble_up(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[i]->dist < heap[parent]->dist) {
                swap_entries(i, parent);
                i = parent;
            } else {
                break;
            }
        }
    }

    void bubble_down(int i) {
        int n = heap.size();
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;

            if (left < n && heap[left]->dist < heap[smallest]->dist) {
                smallest = left;
            }
            if (right < n && heap[right]->dist < heap[smallest]->dist) {
                smallest = right;
            }

            if (smallest != i) {
                swap_entries(i, smallest);
                i = smallest;
            } else {
                break;
            }
        }
    }

    // --------------------------------------------------------
    // PQ Interface Implementations
    // --------------------------------------------------------

    void push(int node, int dist) override {
        ++push_count;

        if (node >= max_nodes) {
            max_nodes = node + 1;
            node_to_index.resize(max_nodes, NOT_IN_HEAP);
        } else if (node_to_index.empty()) {
            max_nodes = (node + 1) > max_nodes ? (node + 1) : max_nodes;
            node_to_index.resize(max_nodes, NOT_IN_HEAP);
        }

        int index = heap.size();
        Entry* newEntry = new Entry{dist, node, index};

        heap.push_back(newEntry);
        node_to_index[node] = index;

        bubble_up(index);
    }

    bool empty() override {
        return heap.empty();
    }

    std::pair<int, int> pop() override {
        ++pop_count;

        Entry* root_entry = heap[0];
        int d = root_entry->dist;
        int n = root_entry->node;

        node_to_index[n] = NOT_IN_HEAP;

        int last_index = heap.size() - 1;
        if (last_index > 0) {
            Entry* last_entry = heap.back();
            heap[0] = last_entry;
            last_entry->index_in_heap = 0;
            node_to_index[last_entry->node] = 0;
            heap.pop_back();
            bubble_down(0);
        } else {
            heap.pop_back();
        }

        delete root_entry;
        return {d, n};
    }

    // --------------------------------------------------------
    // Non-lazy decrease-key: Find the entry, update its value, and bubble it up
    // FIX: Cast 'node' to std::size_t to resolve the signed/unsigned warning
    // --------------------------------------------------------
    void decrease_key(int node, int dist) override {
        ++decrease_key_count;

        // FIX APPLIED: Cast 'node' to std::size_t for comparison with vector::size()
        if ((std::size_t)node >= node_to_index.size() || node_to_index[node] == NOT_IN_HEAP) {
            push(node, dist);
            return;
        }

        int index = node_to_index[node];
        Entry* entry = heap[index];

        if (dist < entry->dist) {
            entry->dist = dist;
            bubble_up(index);
        }
    }

    ~BinaryHeapPQ() override {
        for (Entry* e : heap) {
            delete e;
        }
    }
};