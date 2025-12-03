#include <queue>
#include <vector>
#include <utility>
#include "dijkstra.cpp" 

// ------------------------------------------------------------
// Binary Heap (min-heap) priority queue
// using std::priority_queue (lazy decrease-key)
// ------------------------------------------------------------

struct BinaryHeapPQ : PQ {

    struct Entry {
        int dist, node;
    };

    struct Compare {
        bool operator()(const Entry &a, const Entry &b) const {
            return a.dist > b.dist;   // min-heap
        }
    };

    std::priority_queue<Entry, std::vector<Entry>, Compare> heap;

    void push(int node, int dist) override {
        heap.push({dist, node});
    }

    bool empty() override {
        return heap.empty();
    }

    std::pair<int,int> pop() override {
        Entry e = heap.top();
        heap.pop();
        return {e.dist, e.node};
    }

    // Lazy decrease-key: just push a new (node,dist) entry
    // Old entries become outdated and are ignored in Dijkstra
    void decrease_key(int node, int dist) override {
        heap.push({dist, node});
    }
};
