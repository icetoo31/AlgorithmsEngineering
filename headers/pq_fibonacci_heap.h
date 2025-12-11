#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include "pq.h"

// NOTE: Ensure NOT_IN_HEAP (or a similar constant for the other heaps) is defined
// using 'inline constexpr' in a single common header to avoid redefinition errors.

// ============================================================
// True Fibonacci Heap (min-heap)
// Non-Lazy implementation: O(1) amortized decrease_key
// ============================================================
struct FibonacciHeapPQ : PQ {
    // --------------------------------------------------------
    // Node structure
    // --------------------------------------------------------
    struct Node {
        int dist, node;
        int degree;
        bool mark;
        Node* parent;
        Node* child;
        Node* left;
        Node* right;

        Node(int d, int n)
            : dist(d), node(n), degree(0), mark(false),
              parent(nullptr), child(nullptr), left(this), right(this) {}
    };

    Node* minNode = nullptr;
    int nNodes = 0;

    // --------------------------------------------------------
    // Map Graph Node ID to its Heap Node Pointer
    // --------------------------------------------------------
    std::vector<Node*> nodeMap;
    int max_nodes = 0;

    // Helper function to resize the map if a new node ID is pushed
    void ensureNodeMapSize(int node) {
        if (node >= max_nodes) {
            max_nodes = node + 1;
            nodeMap.resize(max_nodes, nullptr);
        }
    }

    // --------------------------------------------------------
    // Insert node into root list (circular doubly linked)
    // --------------------------------------------------------
    void addToRootList(Node* x) {
        if (!minNode) {
            minNode = x;
            x->left = x->right = x;
            return;
        }
        // Insert right of minNode
        x->right = minNode->right;
        x->left = minNode;
        minNode->right->left = x;
        minNode->right = x;
        if (x->dist < minNode->dist)
            minNode = x;
    }

    // --------------------------------------------------------
    // Insert into heap
    // --------------------------------------------------------
    void push(int node, int dist) override {
        auto t0 = Clock::now();
        ++push_count;

        ensureNodeMapSize(node); // Ensure map can hold this node ID

        Node* x = new Node(dist, node);
        addToRootList(x);
        nNodes++;

        // Store the pointer in the map
        nodeMap[node] = x;

        auto t1 = Clock::now();
        push_time_acc += std::chrono::duration_cast<Duration>(t1 - t0);
    }

    bool empty() override {
        return minNode == nullptr;
    }

    // --------------------------------------------------------
    // Link y under x (child link)
    // --------------------------------------------------------
    void linkTrees(Node* y, Node* x) {
        // Remove y from root list
        y->left->right = y->right;
        y->right->left = y->left;

        // Add y as a child of x
        y->parent = x;
        y->left = y->right = y;

        if (!x->child) {
            x->child = y;
        } else {
            // Insert into child list
            Node* current_child = x->child;
            y->right = current_child->right;
            y->left = current_child;
            current_child->right->left = y;
            current_child->right = y;
        }

        x->degree++;
        y->mark = false;
    }

    // --------------------------------------------------------
    // Consolidate root list after delete-min
    // --------------------------------------------------------
    void consolidate() {
        if (nNodes == 0) return;

        int D = (int)std::floor(std::log2(nNodes)) + 2;
        std::vector<Node*> A(D, nullptr);

        std::vector<Node*> roots;
        Node* x = minNode;
        do {
            roots.push_back(x);
            x = x->right;
        } while (x != minNode);

        for (Node* w : roots) {
            x = w;
            int d = x->degree;

            while (d < D && A[d]) {
                Node* y = A[d];
                if (y->dist < x->dist) std::swap(x, y);
                linkTrees(y, x);
                A[d] = nullptr;
                d++;
            }
            if (d < D) {
                A[d] = x;
            }
        }

        minNode = nullptr;

        for (Node* n : A) {
            if (n) {
                n->left = n->right = n;
                n->parent = nullptr;
                addToRootList(n);
            }
        }
    }

    // --------------------------------------------------------
    // Cut child from parent and add to root list
    // --------------------------------------------------------
    void cut(Node* x, Node* p) {
        if (x->right == x) {
            p->child = nullptr;
        } else {
            x->right->left = x->left;
            x->left->right = x->right;
            if (p->child == x)
                p->child = x->right;
        }
        p->degree--;

        x->parent = nullptr;
        x->left = x->right = x;
        addToRootList(x);
        x->mark = false;
    }

    // --------------------------------------------------------
    // Cascading cut
    // --------------------------------------------------------
    void cascadingCut(Node* y) {
        Node* p = y->parent;
        if (!p) return;

        if (!y->mark) {
            y->mark = true;
        } else {
            cut(y, p);
            cascadingCut(p);
        }
    }

    // --------------------------------------------------------
    // decrease_key — TRUE NON-LAZY IMPLEMENTATION (Fixed check)
    // --------------------------------------------------------
    void decrease_key(int node, int newDist) override {
        auto t0 = Clock::now();
        ++decrease_key_count;

        // 1. Check if node ID is out of bounds or not in the map
        // FIX: Use size check, ensure node ID is valid before access
        if ((std::size_t)node >= nodeMap.size()) {
            // Treat as an initial push
            push(node, newDist);
            return;
        }

        Node* x = nodeMap[node];

        if (!x) {
            // Node pointer is null (never pushed or already popped)
            push(node, newDist);
            return;
        }

        // 2. Perform the decrease
        if (newDist >= x->dist) {
             return;
        }
        x->dist = newDist;

        // 3. Check and cut (if heap property is violated)
        Node* p = x->parent;
        if (p && x->dist < p->dist) {
            cut(x, p);
            cascadingCut(p);
        }

        // 4. Update min pointer if necessary
        if (x->dist < minNode->dist) {
            minNode = x;
        }

        auto t1 = Clock::now();
        decrease_key_time_acc += std::chrono::duration_cast<Duration>(t1 - t0);
    }

    // --------------------------------------------------------
    // Extract minimum
    // --------------------------------------------------------
    std::pair<int,int> pop() override {
        auto t0 = Clock::now();
        ++pop_count;

        Node* z = minNode;
        if (!z) return {-1, -1};

        if (z->child) {
            Node* c = z->child;
            do {
                Node* next = c->right;
                c->parent = nullptr;
                addToRootList(c);
                c = next;
            } while (c != z->child);
        }

        z->left->right = z->right;
        z->right->left = z->left;

        if (z == z->right) {
            minNode = nullptr;
        } else {
            minNode = z->right;
            consolidate();
        }

        nodeMap[z->node] = nullptr;
        int d = z->dist;
        int n = z->node;
        delete z;
        nNodes--;

        auto t1 = Clock::now();
        pop_time_acc += std::chrono::duration_cast<Duration>(t1 - t0);

        return {d, n};
    }

    // Destructor to clean up all nodes
    ~FibonacciHeapPQ() override {
        while (minNode) {
            pop();
        }
    }
};
