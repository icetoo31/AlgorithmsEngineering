#include <vector>
#include <utility>
#include "dijkstra.cpp" 

// ------------------------------------------------------------
// Pairing Heap Implementation (min-heap)
// ------------------------------------------------------------

struct PairingHeapPQ : PQ {

    // --------------------------------------------------------
    // Node structure for pairing heap
    // --------------------------------------------------------
    struct Node {
        int dist, node;
        Node* child;   // leftmost child
        Node* sibling; // next sibling
        Node(int d, int n) : dist(d), node(n), child(nullptr), sibling(nullptr) {}
    };

    Node* root = nullptr;

    // --------------------------------------------------------
    // Merge two pairing heap trees
    // --------------------------------------------------------
    Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;

        if (b->dist < a->dist)
            std::swap(a, b);

        // b becomes the first child of a
        b->sibling = a->child;
        a->child = b;
        return a;
    }

    // --------------------------------------------------------
    // Two-pass merge (recursive pairing)
    // --------------------------------------------------------
    Node* two_pass_merge(Node* first) {
        if (!first || !first->sibling)
            return first;

        std::vector<Node*> arr;

        // First pass: merge siblings in pairs
        while (first) {
            Node* a = first;
            Node* b = first->sibling;

            first = (b ? b->sibling : nullptr);

            a->sibling = nullptr;
            if (b) b->sibling = nullptr;

            arr.push_back(merge(a, b));
        }

        // Second pass: merge from right to left
        for (int i = arr.size() - 2; i >= 0; --i) {
            arr[i] = merge(arr[i], arr[i + 1]);
        }

        return arr[0];
    }

    // --------------------------------------------------------
    // Insert new node
    // --------------------------------------------------------
    void push(int node, int dist) override {
        root = merge(root, new Node(dist, node));
    }

    bool empty() override {
        return root == nullptr;
    }

    // --------------------------------------------------------
    // Extract-min
    // --------------------------------------------------------
    std::pair<int,int> pop() override {
        Node* old = root;
        int d = old->dist;
        int n = old->node;

        root = two_pass_merge(old->child);

        delete old;
        return {d, n};
    }

    // --------------------------------------------------------
    // Lazy decrease-key: simply insert a new copy
    // --------------------------------------------------------
    void decrease_key(int node, int dist) override {
        push(node, dist);
    }
};
