# Performance Testing Considerations

This section outlines the external factors that may influence the performance of the priority queues and how we aim to minimize their impact during testing.

## 1. **Cache Optimization**
The cache can improve the performance of Dijkstra's algorithm when runs are executed sequentially. This effect can be observed when running the same graph multiple times.

### Key Points:
- **Cache Effect**: Sequential runs benefit from cache optimization, which can skew performance results.
- **Mitigation Strategy**: We do not have fine-grained control over the cache in C++, but we can mitigate its effects by performing **warm-up runs** to prepare the cache and stabilize results for individual tests.

## 2. **Heap**
To ensure independent execution of priority queue (pq) implementations, we manually garbage collect memory after each test run.

### Key Points:
- **Independent Memory**: Each priority queue operates on its own allocated memory, which is garbage collected after use to avoid interference.
- **Heap Impact**: Since no other memory is allocated during our tests, we conclude that the machine heap does not introduce unfairness between the pq data structures. 
- **Garbage Collection**: Memory is collected between runs to prevent the heap from becoming slower for subsequent tests. (Essentially each pq is tested on an empty heap)

## 3. **Clock Measurements**
Performance is measured using a high-resolution **nanosecond clock**, which captures only the time taken for the Dijkstra call.

### Key Points:
- **Exclusions**: The clock measures only the time taken for the algorithm’s core operation and excludes:
  - Graph generation
  - Input reading
  - Output writing
  - Garbage collection
- **Reasoning**: This approach isolates the performance of the priority queues which ensures that the external factors do not influence the results.
