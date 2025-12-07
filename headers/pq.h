#ifndef PQ_H
#define PQ_H

#include <utility>  // for std::pair

struct PQ {
    virtual void push(int node, int dist) = 0;
    virtual bool empty() = 0;
    virtual std::pair<int, int> pop() = 0;
    virtual void decrease_key(int node, int dist) = 0;
    virtual ~PQ() = default;
};

#endif // PQ_H
