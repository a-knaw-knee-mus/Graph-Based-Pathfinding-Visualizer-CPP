#include <vector>
#include <iostream>
#include <memory>
#include <limits>
#include <queue>
#include <SFML/Graphics.hpp>
#include "states.h"
#include "window.h"
#include <chrono>
#include <thread>
#include <numeric>
using namespace std;
using namespace sf;

struct Edge {
    shared_ptr<Node> u;
    shared_ptr<Node> v;
    int weight;

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

// Union-Find (Disjoint Set) data structure
class UnionFind {
public:
    UnionFind(int n) : parent(n), rank(n, 0) {
        iota(parent.begin(), parent.end(), 0); // Initialize parent to be itself
    }

    int find(int u) {
        if (u != parent[u]) {
            parent[u] = find(parent[u]);
        }
        return parent[u];
    }

    void unionSets(int u, int v) {
        int rootU = find(u);
        int rootV = find(v);
        if (rootU != rootV) {
            if (rank[rootU] > rank[rootV]) {
                parent[rootV] = rootU;
            } else if (rank[rootU] < rank[rootV]) {
                parent[rootU] = rootV;
            } else {
                parent[rootV] = rootU;
                rank[rootU]++;
            }
        }
    }

private:
    vector<int> parent;
    vector<int> rank;
};

void kruskal(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window) {
    vector<Edge> edges;

    for (const auto& [node, neighbors]: edgeData) {
        for (const auto& [neighborNode, weight, width]: neighbors) {
            edges.push_back({node, neighborNode, weight});
        }
    }

    sort(edges.begin(), edges.end());

    // Initialize Union-Find for all nodes
    unordered_map<shared_ptr<Node>, int, NodePtrHash, NodePtrEqual> nodeIndex;
    int index = 0;
    for (const auto& pair : edgeData) {
        nodeIndex[pair.first] = index++;
    }
    UnionFind uf(index);

    vector<Edge> mst;
    int mstWeight = 0;
    chrono::milliseconds duration(10);

    // Kruskal's algorithm
    for (const auto& edge : edges) {
        int u = nodeIndex[edge.u];
        int v = nodeIndex[edge.v];

        if (uf.find(u) != uf.find(v)) {
            uf.unionSets(u, v);
            mst.push_back(edge);
            edge.u->state = Path;
            for (auto& e: edgeData[edge.u]) {
                if (get<0>(e) == edge.v) {
                    get<2>(e) = 3; // modify edge size to indicate this edge was taken
                }
            }
            edge.v->state = Path;
            this_thread::sleep_for(duration);
            refreshScreen(nodes, edgeData, window);
            mstWeight += edge.weight;
        }
    }

    // Output the MST edges and total weight
    cout << "Minimum Spanning Tree Weight: " << mstWeight << endl;
}

void prim(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window) {
    unordered_map<shared_ptr<Node>, bool, NodePtrHash, NodePtrEqual> inMST;
    priority_queue<Edge> pq;

    // Start from any arbitrary node, here we start from the first node in edgeData
    auto startNode = edgeData.begin()->first;
    inMST[startNode] = true;

    // Add all edges from the start node to the priority queue
    for (const auto& [neighborNode, weight, width] : edgeData[startNode]) {
        pq.push({startNode, neighborNode, weight});
    }

    vector<Edge> mst;
    int mstWeight = 0;
    chrono::milliseconds duration(10);

    while (!pq.empty()) {
        Edge edge = pq.top();
        pq.pop();

        // If the destination node is already in the MST, skip this edge
        if (inMST[edge.v]) {
            continue;
        }

        // Add the edge to the MST
        mst.push_back(edge);
        mstWeight += edge.weight;
        edge.u->state = Path;
        for (auto& e: edgeData[edge.u]) {
            if (get<0>(e) == edge.v) {
                get<2>(e) = 3; // modify edge size to indicate this edge was taken
            }
        }
        edge.v->state = Path;
        this_thread::sleep_for(duration);
        refreshScreen(nodes, edgeData, window);

        // Mark the new node as included in the MST
        inMST[edge.v] = true;

        // Add all edges from the new node to the priority queue
        for (const auto& [neighborNode, weight, width] : edgeData[edge.v]) {
            if (!inMST[neighborNode]) {
                pq.push({edge.v, neighborNode, weight});
            }
        }
    }

    // Output the MST edges and total weight
    cout << "Minimum Spanning Tree Weight: " << mstWeight << endl;
}