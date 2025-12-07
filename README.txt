Since we are only supposed to be testing the different priority queues, we will try to minimize the effect of external factors 
such as the cache and the heap on the performance of the priority queues.

CACHE: The cache is optimizing the dijkstra runs when they are run in sequence. This can be observed by running the same graph 
multiple times. It is not clear how we can prevent the cache from interfering as C++ does not provide much control over it. One
thing that can be done is to do a few warm-up runs to prepare the cache to make the individual runs more consistent.

MACHINE HEAP: The pq implementations are supposed to be ran independently, which is why the dynamically allocated 
memory for the pqs is manually garbage collected after being used. Since no other memory is allocated in our code we can conclude 
that the machine heap does not introduce any unfairness between the data structure heaps. In this context it is reasonable to
garbage collect between runs because if the heap gets fuller that would impact the performance of the other pqs that remain.

CLOCK: Each priority queue performance is measured by using a high resolution nanosecond clock. The clock only measures the
dijkstra call and excludes the graph generation, the reading of the input, the writing of the output and the garbage collection.
This is reasonable for these tests because we are only interested in the performance of the priority queues and the setup and
context for each one should be the same.

