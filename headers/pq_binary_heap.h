#ifndef PQ_BINARY_HEAP_H
#define PQ_BINARY_HEAP_H

#include <vector>
#include <algorithm>
#include "pq.h"

// ------------------------------------------------------------
// Binary heap implementation (min-heap) with explicit index map
// ------------------------------------------------------------
struct BinaryHeapPQ : PQ {

    struct Entry {
        int dist;
        int node;
        int index_in_heap;
    };

    std::vector<Entry*> heap;       // heap array
    std::vector<int>    node_to_index; // node -> index in heap, or NOT_IN_HEAP

    BinaryHeapPQ() = default;

    ~BinaryHeapPQ() override {
        for (Entry* e : heap) {
            delete e;
        }
    }

    // This overrides PQ::empty()  (same signature, no const)
    bool empty() override {
        return heap.empty();
    }

private:
    void ensure_node_capacity(int node) {
        if (node < 0) return;
        if (node >= static_cast<int>(node_to_index.size())) {
            node_to_index.resize(node + 1, NOT_IN_HEAP);
        }
    }

    void swap_entries(int i, int j) {
        if (i == j) return;
        Entry* ei = heap[i];
        Entry* ej = heap[j];
        heap[i] = ej;
        heap[j] = ei;
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
        int n = static_cast<int>(heap.size());
        while (true) {
            int left  = 2 * i + 1;
            int right = 2 * i + 2;
            int best  = i;

            if (left < n && heap[left]->dist < heap[best]->dist)
                best = left;
            if (right < n && heap[right]->dist < heap[best]->dist)
                best = right;

            if (best == i) break;

            swap_entries(i, best);
            i = best;
        }
    }

public:
    void push(int node, int dist) override {
        auto t0 = Clock::now();
        ++push_count;

        ensure_node_capacity(node);

        int index = static_cast<int>(heap.size());
        Entry* e = new Entry{dist, node, index};
        heap.push_back(e);
        node_to_index[node] = index;

        bubble_up(index);

        auto t1 = Clock::now();
        push_time_acc += std::chrono::duration_cast<Duration>(t1 - t0);
    }

    std::pair<int,int> pop() override {
        auto t0 = Clock::now();
        ++pop_count;

        if (heap.empty()) {
            auto t1 = Clock::now();
            pop_time_acc += std::chrono::duration_cast<Duration>(t1 - t0);
            return {-1, -1};
        }

        Entry* root = heap[0];
        int d = root->dist;
        int n = root->node;

        node_to_index[n] = NOT_IN_HEAP;

        int last_index = static_cast<int>(heap.size()) - 1;
        if (last_index > 0) {
            Entry* last = heap[last_index];
            heap[0] = last;
            last->index_in_heap = 0;
            node_to_index[last->node] = 0;
            heap.pop_back();
            bubble_down(0);
        } else {
            heap.pop_back();
        }

        delete root;

        auto t1 = Clock::now();
        pop_time_acc += std::chrono::duration_cast<Duration>(t1 - t0);

        return {d, n};
    }

    void decrease_key(int node, int dist) override {
        auto t0 = Clock::now();
        ++decrease_key_count;

        ensure_node_capacity(node);

        if (node < 0 || node >= static_cast<int>(node_to_index.size()) ||
            node_to_index[node] == NOT_IN_HEAP) {
            // Not in heap yet -> treat as push
            auto t1 = Clock::now();
            decrease_key_time_acc += std::chrono::duration_cast<Duration>(t1 - t0);
            push(node, dist);
            return;
        }

        int index = node_to_index[node];
        Entry* e = heap[index];

        if (dist < e->dist) {
            e->dist = dist;
            bubble_up(index);
        }

        auto t1 = Clock::now();
        decrease_key_time_acc += std::chrono::duration_cast<Duration>(t1 - t0);
    }
};

#endif // PQ_BINARY_HEAP_H
