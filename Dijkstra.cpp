#include <iostream>
#include <vector>
#include <climits>

using namespace std;

class Graph {
public:
    int V; // Number of vertices
    vector<vector<int>> adjMatrix; // Adjacency matrix

    Graph(int V) {
        this->V = V;
        adjMatrix.resize(V, vector<int>(V, INT_MAX));
    }

    void addEdge(int u, int v, int weight) {
        adjMatrix[u][v] = weight;
        adjMatrix[v][u] = weight; // For undirected graph
    }

    void dijkstra(int source) {
        vector<int> dist(V, INT_MAX); // Distance array
        vector<bool> sptSet(V, false); // Shortest path tree set
        dist[source] = 0;

        for (int count = 0; count < V - 1; ++count) {
            // Find the vertex with the minimum distance value
            int u = minDistance(dist, sptSet);
            sptSet[u] = true;

            // Update distance value of the adjacent vertices of the selected vertex
            for (int v = 0; v < V; ++v) {
                if (!sptSet[v] && adjMatrix[u][v] != INT_MAX && dist[u] != INT_MAX && dist[u] + adjMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + adjMatrix[u][v];
                }
            }
        }

        printSolution(dist);
    }

private:
    int minDistance(const vector<int>& dist, const vector<bool>& sptSet) {
        int min = INT_MAX, min_index;
        for (int v = 0; v < V; ++v) {
            if (!sptSet[v] && dist[v] <= min) {
                min = dist[v];
                min_index = v;
            }
        }
        return min_index;
    }

    void printSolution(const vector<int>& dist) {
        cout << "Vertex \t Distance from Source" << endl;
        for (int i = 0; i < V; ++i) {
            if (dist[i] == INT_MAX)
                cout << i << " \t " << "INF" << endl;
            else
                cout << i << " \t " << dist[i] << endl;
        }
    }
};

int main() {
    Graph g(9); // Create a graph with 9 vertices

    // Adding edges (u, v, weight)
    g.addEdge(0, 1, 4);
    g.addEdge(0, 7, 8);
    g.addEdge(1, 2, 8);
    g.addEdge(1, 7, 11);
    g.addEdge(2, 3, 7);
    g.addEdge(2, 5, 4);
    g.addEdge(2, 8, 2);
    g.addEdge(3, 4, 9);
    g.addEdge(3, 5, 14);
    g.addEdge(4, 5, 10);
    g.addEdge(5, 6, 2);
    g.addEdge(6, 7, 1);
    g.addEdge(6, 8, 6);
    g.addEdge(7, 8, 7);

    int source = 0; // Starting vertex
    g.dijkstra(source); // Run Dijkstra's algorithm from source vertex

    return 0;
}
