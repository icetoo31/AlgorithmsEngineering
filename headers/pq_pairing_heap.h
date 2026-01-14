#include <vector>
#include <utility>
#include <unordered_map>
#include "pq.h" // Assuming pq.h defines the base class PQ

// ------------------------------------------------------------
// Pairing Heap Implementation (min-heap)
// Non-lazy decrease-key (requires parent pointers for O(1) cut)
// ------------------------------------------------------------

struct PairingHeapPQ : PQ {

    // --------------------------------------------------------
    // Node structure for pairing heap
    // Added 'parent' pointer to support O(1) cut operation
    // --------------------------------------------------------
    struct Node {
        int dist, node;
        Node* child;    // leftmost child
        Node* sibling;  // next sibling
        Node* parent;   // Pointer to the parent node
        Node* prev_sibling;  // Previous sibling for O(1) detach
        
        Node(int d, int n) : dist(d), node(n), child(nullptr), sibling(nullptr), parent(nullptr), prev_sibling(nullptr) {}
        
        // Helper to detach a node from its parent/siblings for cut operation
        void detach() {
            if (parent) {
                // Remove from sibling list using doubly-linked structure
                if (parent->child == this) {
                    parent->child = sibling;
                    if (sibling) {
                        sibling->prev_sibling = nullptr;
                    }
                } else {
                    if (prev_sibling) {
                        prev_sibling->sibling = sibling;
                    }
                    if (sibling) {
                        sibling->prev_sibling = prev_sibling;
                    }
                }
                parent = nullptr;
                sibling = nullptr;
                prev_sibling = nullptr;
            }
        }
    };

    Node* root = nullptr;
    std::unordered_map<int, Node*> nodeMap; // Map node id to its corresponding heap node

    // --------------------------------------------------------
    // Merge two pairing heap trees
    // Updated to handle parent and prev_sibling pointers
    // --------------------------------------------------------
    Node* merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;

        if (b->dist < a->dist)
            std::swap(a, b);

        // b becomes the first child of a
        b->parent = a;
        b->sibling = a->child;
        b->prev_sibling = nullptr;
        
        if (a->child) {
            a->child->prev_sibling = b;
        }
        a->child = b;
        
        return a;
    }

    // --------------------------------------------------------
    // Two-pass merge (recursive pairing)
    // Reset parent and prev_sibling pointers during merge
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

            // Reset connections for nodes being merged
            a->sibling = nullptr;
            a->parent = nullptr;
            a->prev_sibling = nullptr;
            if (b) {
                b->sibling = nullptr;
                b->parent = nullptr;
                b->prev_sibling = nullptr;
            }

            arr.push_back(merge(a, b));
        }

        // Second pass: merge from right to left
        for (int i = arr.size() - 2; i >= 0; --i) {
            arr[i] = merge(arr[i], arr[i + 1]);
        }

        return arr[0];
    }
    
    // --------------------------------------------------------
    // Cut a node from the heap structure and merge it with the root.
    // This is the O(1) amortized operation for non-lazy decrease_key.
    // --------------------------------------------------------
    void cut_and_merge(Node* n) {
        // If n is the root, there is nothing to cut.
        if (n == root) return;
        
        // 1. Cut the node from its parent/sibling list
        n->detach(); // Uses the helper function to fix parent/sibling pointers

        // 2. Merge the resulting tree (which is just 'n') with the root.
        root = merge(root, n);
    }


    // --------------------------------------------------------
    // Insert new node (only called on first visit in some Dijkstra setups)
    // --------------------------------------------------------
    void push(int node, int dist) override {
        Node* newNode = new Node(dist, node);
        root = merge(root, newNode);
        nodeMap[node] = newNode;
    }

    bool empty() override {
        return root == nullptr;
    }

    // --------------------------------------------------------
    // Extract-min
    // --------------------------------------------------------
    std::pair<int, int> pop() override {
        if (!root) return {-1, -1}; // Handle empty case safely

        Node* old = root;
        int d = old->dist;
        int n = old->node;

        // The children of the old root become a forest.
        // We need to reset the parent pointers for all children 
        // before two_pass_merge is called on them.
        Node* child = old->child;
        while(child) {
            child->parent = nullptr;
            child = child->sibling;
        }

        root = two_pass_merge(old->child);
        nodeMap.erase(n); // Remove the node from the map

        delete old;
        return {d, n};
    }

    // --------------------------------------------------------
    // Non-lazy decrease-key: directly update the node's value, cut, and merge.
    // This correctly maintains the heap structure.
    // --------------------------------------------------------
    void decrease_key(int node, int dist) override {
        auto it = nodeMap.find(node);
        
        if (it != nodeMap.end()) {
            Node* n = it->second;
            
            // The node is already in the heap
            if (dist < n->dist) {
                n->dist = dist; // Update the distance
                cut_and_merge(n); // Cut it from its current position and merge with the root
            }
        } else {
            // Node is NOT in the map (first time reaching this node), so we push it.
            // This handles the 'handles push OR decrease-key' comment in dijkstra.cpp.
            push(node, dist);
        }
    }

    // Destructor to clean up memory
    ~PairingHeapPQ() override {
        std::vector<Node*> nodes_to_delete;
        if (root) nodes_to_delete.push_back(root);

        // Simple BFS/DFS to traverse and delete all nodes
        for (size_t i = 0; i < nodes_to_delete.size(); ++i) {
            Node* current = nodes_to_delete[i];
            
            // Add all children
            Node* child = current->child;
            while(child) {
                nodes_to_delete.push_back(child);
                child = child->sibling;
            }
            
            // The sibling link is already covered by the children traversal, 
            // as children become root-level nodes during pop.
        }

        for (Node* n : nodes_to_delete) {
            delete n;
        }
    }
};