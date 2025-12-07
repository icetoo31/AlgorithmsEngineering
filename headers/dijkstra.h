#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include <limits>
#include "pq.h"  // Include the priority queue interface

std::vector<int> dijkstra(
    int n,
    const std::vector<std::vector<std::pair<int, int>>>& adj,
    PQ& pq
);

#endif // DIJKSTRA_H
