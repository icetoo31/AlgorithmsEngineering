#ifndef PQ_H
#define PQ_H

#include <utility>  // for std::pair

struct PQ {
    long long push_count        = 0;
    long long pop_count         = 0;
    long long decrease_key_count = 0;

    // --- Interface ---
    virtual void push(int node, int dist) = 0;
    virtual bool empty() = 0;
    virtual std::pair<int, int> pop() = 0;
    virtual void decrease_key(int node, int dist) = 0;
    virtual ~PQ() = default;

    // --- Helper to reset counters before each run ---
    void reset_counters() {
        push_count = 0;
        pop_count = 0;
        decrease_key_count = 0;
};

#endif // PQ_H
