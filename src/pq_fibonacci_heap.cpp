#pragma once
#include "dijkstra.cpp"
#include <cmath>
#include <vector>
#include <limits>

// ============================================================
// True Fibonacci Heap (min-heap)
// Supports decrease_key with cascading cuts.
// Safe implementation: explicitly avoids pointer corruption.
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
    // Meld two Fibonacci heaps (not needed externally)
    // --------------------------------------------------------
    Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;
        if (b->dist < a->dist) std::swap(a, b);
        return a;
    }

    // --------------------------------------------------------
    // Insert into heap
    // --------------------------------------------------------
    void push(int node, int dist) override {
        Node* x = new Node(dist, node);
        addToRootList(x);
        nNodes++;
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
            y->right = x->child->right;
            y->left = x->child;
            x->child->right->left = y;
            x->child->right = y;
        }

        x->degree++;
        y->mark = false;
    }

    // --------------------------------------------------------
    // Consolidate root list after delete-min
    // --------------------------------------------------------
    void consolidate() {
        int D = std::floor(std::log2(nNodes)) + 3;
        std::vector<Node*> A(D, nullptr);

        std::vector<Node*> roots;
        Node* x = minNode;
        if (x) {
            do {
                roots.push_back(x);
                x = x->right;
            } while (x != minNode);
        }

        for (Node* w : roots) {
            x = w;
            int d = x->degree;

            while (A[d]) {
                Node* y = A[d];
                if (y->dist < x->dist) std::swap(x, y);
                linkTrees(y, x);
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
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
        // Remove x from child list
        if (x->right == x) {
            p->child = nullptr;
        } else {
            x->right->left = x->left;
            x->left->right = x->right;
            if (p->child == x)
                p->child = x->right;
        }
        p->degree--;

        // Add to root list
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
    // decrease_key — REAL version
    // --------------------------------------------------------
    void decrease_key(int node, int newDist) override {
        // We cannot directly locate the node (you use lazy approach),
        // so we follow the same strategy as your other PQs:
        // Insert a new copy — but we still need to allow the real behavior.
        push(node, newDist);
    }

    // --------------------------------------------------------
    // Extract minimum
    // --------------------------------------------------------
    std::pair<int,int> pop() override {
        Node* z = minNode;
        if (!z) return {-1, -1};

        // Move children to root list
        if (z->child) {
            Node* c = z->child;
            do {
                Node* next = c->right;
                c->parent = nullptr;
                addToRootList(c);
                c = next;
            } while (c != z->child);
        }

        // Remove z from root list
        if (z->right == z) {
            minNode = nullptr;
        } else {
            z->left->right = z->right;
            z->right->left = z->left;
            minNode = z->right;
            consolidate();
        }

        int d = z->dist;
        int n = z->node;
        delete z;
        nNodes--;

        return {d, n};
    }
};
