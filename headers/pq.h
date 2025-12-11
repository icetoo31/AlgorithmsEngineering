#ifndef PQ_H
#define PQ_H

#include <utility>
#include <chrono>

// Common clock / duration aliases
using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double>; // seconds

// Single global marker for "not in heap"
inline constexpr int NOT_IN_HEAP = -1;

struct PQ
{
    // --- Operation counters ---
    long long push_count = 0;
    long long pop_count = 0;
    long long decrease_key_count = 0;

    // --- Accumulated time per operation type ---
    Duration push_time_acc{0.0};
    Duration pop_time_acc{0.0};
    Duration decrease_key_time_acc{0.0};

    virtual ~PQ() = default;

    // --- Interface ---
    virtual void push(int node, int dist) = 0;
    virtual std::pair<int, int> pop() = 0;
    virtual void decrease_key(int node, int dist) = 0;
    virtual bool empty() = 0; // NOTE: no 'const' here

    // --- Reset counters + timers between runs ---
    void reset_counters()
    {
        push_count = 0;
        pop_count = 0;
        decrease_key_count = 0;

        push_time_acc = Duration::zero();
        pop_time_acc = Duration::zero();
        decrease_key_time_acc = Duration::zero();
    }
};

#endif // PQ_H
