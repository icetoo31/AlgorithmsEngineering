#include <vector>
#include <utility>
#include <algorithm>
#include "dijkstra.cpp" 

// ------------------------------------------------------------
// 8-ary heap implementation (min-heap)
// ------------------------------------------------------------

struct EightAryHeapPQ : PQ {

    static const int D = 8;   // branching factor

    struct Entry {
        int dist, node;
    };

    std::vector<Entry> heap;

    bool empty() override {
        return heap.empty();
    }

    // --------------------------------------------------------
    // Push new entry
    // --------------------------------------------------------
    void push(int node, int dist) override {
        heap.push_back({dist, node});
        sift_up(heap.size() - 1);
    }

    // --------------------------------------------------------
    // Sift-up (bubble-up)
    // --------------------------------------------------------
    void sift_up(int i) {
        while (i > 0) {
            int parent = (i - 1) / D;
            if (heap[i].dist >= heap[parent].dist)
                break;
            std::swap(heap[i], heap[parent]);
            i = parent;
        }
    }

    // --------------------------------------------------------
    // Pop minimum element
    // --------------------------------------------------------
    std::pair<int,int> pop() override {
        Entry root = heap[0];

        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty())
            sift_down(0);

        return {root.dist, root.node};
    }

    // --------------------------------------------------------
    // Sift-down
    // --------------------------------------------------------
    void sift_down(int i) {
        int n = heap.size();
        while (true) {
            int best = i;

            // Check all 8 children
            for (int k = 1; k <= D; k++) {
                int child = i * D + k;
                if (child < n && heap[child].dist < heap[best].dist)
                    best = child;
            }

            if (best == i)
                break;

            std::swap(heap[i], heap[best]);
            i = best;
        }
    }

    // --------------------------------------------------------
    // Lazy decrease-key: just insert a new entry
    // --------------------------------------------------------
    void decrease_key(int node, int dist) override {
        push(node, dist);
    }
};
