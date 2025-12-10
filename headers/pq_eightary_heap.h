#include <vector>
#include <utility>
#include <algorithm>
#include <limits>
#include "pq.h" 

// NOTE: The definition of NOT_IN_HEAP MUST be removed from here
// and defined only once (e.g., using 'inline constexpr int NOT_IN_HEAP = -1;'
// in pq_binary_heap.h or a common utility header).
// We assume it is defined elsewhere and accessible here.

// ------------------------------------------------------------
// 8-ary heap implementation (min-heap)
// Non-lazy with O(log_8 N) decrease-key
// ------------------------------------------------------------

struct EightAryHeapPQ : PQ {

    static const int D = 8;    // branching factor

    struct Entry {
        int dist, node;
        int index_in_heap; 
    };

    std::vector<Entry*> heap;
    std::vector<int> node_to_index;
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

    void sift_up(int i) {
        while (i > 0) {
            int parent = (i - 1) / D;
            if (heap[i]->dist >= heap[parent]->dist)
                break;
            swap_entries(i, parent);
            i = parent;
        }
    }

    void sift_down(int i) {
        const std::size_t n = heap.size(); 
        while (true) {
            int best = i;

            for (int k = 1; k <= D; k++) {
                std::size_t child_idx = (std::size_t)i * D + k;
                
                if (child_idx < n && heap[child_idx]->dist < heap[best]->dist)
                    best = (int)child_idx;
            }

            if (best == i)
                break;

            swap_entries(i, best);
            i = best;
        }
    }

    // --------------------------------------------------------
    // PQ Interface Implementations
    // --------------------------------------------------------
    
    bool empty() override {
        return heap.empty();
    }

    void push(int node, int dist) override {
        // NOTE: We assume NOT_IN_HEAP is defined and accessible here.
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

        sift_up(index);
    }

    std::pair<int,int> pop() override {
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
            sift_down(0);
        } else {
            heap.pop_back();
        }

        delete root_entry;
        return {d, n};
    }

    void decrease_key(int node, int dist) override {
        // NOTE: We assume NOT_IN_HEAP is defined and accessible here.
        if ((std::size_t)node >= node_to_index.size() || node_to_index[node] == NOT_IN_HEAP) {
            push(node, dist);
            return;
        }

        int index = node_to_index[node];
        Entry* entry = heap[index];

        if (dist < entry->dist) {
            entry->dist = dist;
            sift_up(index);
        }
    }
    
    ~EightAryHeapPQ() override {
        for (Entry* e : heap) {
            delete e;
        }
    }
};