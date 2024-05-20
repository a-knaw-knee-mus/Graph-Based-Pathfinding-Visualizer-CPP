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

// bool isStartOrEnd(const shared_ptr<Node>& node, const shared_ptr<Node>& start, const shared_ptr<Node>& end) {
//     if (node == start || node == end) return true;
//     return false;
// }

void kruskal(vector<shared_ptr<Node>>& nodes, const unordered_map<shared_ptr<Node>, vector<pair<shared_ptr<Node>, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window) {
    vector<Edge> edges;

    for (const auto& [node, neighbors]: edgeData) {
        for (const auto& [neighborNode, weight]: neighbors) {
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

    // Kruskal's algorithm
    for (const auto& edge : edges) {
        int u = nodeIndex[edge.u];
        int v = nodeIndex[edge.v];

        if (uf.find(u) != uf.find(v)) {
            uf.unionSets(u, v);
            mst.push_back(edge);
            edge.u->state = Path;
            edge.v->state = Path;
            mstWeight += edge.weight;
        }
    }

    // Output the MST edges and total weight
    cout << "Minimum Spanning Tree Weight: " << mstWeight << endl;
    cout << "Edges in the MST:" << endl;
    // for (const auto& edge : mst) {
    //     cout << "Node " << edge.u->id << " - Node " << edge.v->id << " with weight " << edge.weight << endl;
    // }
}
