#include "pathfinding.h"
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
using namespace std;
using namespace sf;

bool isStartOrEnd(const shared_ptr<Node>& node, const shared_ptr<Node>& start, const shared_ptr<Node>& end) {
    if (node == start || node == end) return true;
    return false;
}

void bellmanFord(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window, const shared_ptr<Node>& startNode, const shared_ptr<Node>& endNode) {
    // Initialize the distance and previous node maps
    unordered_map<shared_ptr<Node>, int, NodePtrHash, NodePtrEqual> distances;
    unordered_map<shared_ptr<Node>, shared_ptr<Node>, NodePtrHash, NodePtrEqual> previous;
    for (const auto& pair : edgeData) {
        distances[pair.first] = numeric_limits<int>::max();
        previous[pair.first] = nullptr;
    }
    distances[startNode] = 0;
    chrono::milliseconds duration(10);

    // Relax all edges |V| - 1 times
    for (size_t i = 0; i < edgeData.size() - 1; ++i) {
        for (const auto& pair : edgeData) {
            auto u = pair.first;
            for (const auto& neighborPair : pair.second) {
                auto v = get<0>(neighborPair);
                int weight = get<1>(neighborPair);
                if (distances[u] != numeric_limits<int>::max() && distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    previous[v] = u;
                    if (!isStartOrEnd(v, startNode, endNode)) {
                        v->state = Visited;
                        this_thread::sleep_for(duration);
                        refreshScreen(nodes, edgeData, window);
                    }
                }
            }
        }
    }

    // Check for negative-weight cycles
    for (const auto& pair : edgeData) {
        auto u = pair.first;
        for (const auto& neighborPair : pair.second) {
            auto v = get<0>(neighborPair);
            int weight = get<1>(neighborPair);
            if (distances[u] != numeric_limits<int>::max() && distances[u] + weight < distances[v]) {
                cout << "Graph contains a negative-weight cycle" << endl;
                return;
            }
        }
    }

    // Reconstruct the path from endNode to startNode
    vector<shared_ptr<Node>> path;
    for (shared_ptr<Node> at = endNode; at != nullptr; at = previous[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    for (int i=0; i<path.size()-1; i++) {
        for (auto& e: edgeData[path[i]]) {
            if (get<0>(e) == path[i+1]) {
                get<2>(e) = 3; // modify edge size to indicate this edge was taken
            }
        }
        if (!isStartOrEnd(path[i], startNode, endNode)) {
            path[i]->state = Path;
            chrono::milliseconds duration(50);
            this_thread::sleep_for(duration);
            refreshScreen(nodes, edgeData, window);
        }
    }

    if (distances[endNode] != numeric_limits<int>::max()) {
        cout << "min dist: " << distances[endNode] << endl;
    } else {
        cout << "no path found" << endl;
        for (const auto& n: nodes) {
            if (n->state == Visited) {
                n->state = VisitedNoPath;
            }
        }
        refreshScreen(nodes, edgeData, window);
    }
}

void findDijkstraPath(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window, const shared_ptr<Node>& startNode, const shared_ptr<Node>& endNode) {
    unordered_map<shared_ptr<Node>, int, NodePtrHash, NodePtrEqual> distances;
    unordered_map<shared_ptr<Node>, shared_ptr<Node>, NodePtrHash, NodePtrEqual> previous;
    for (const auto& [node, neighbors]: edgeData) {
        distances[node] = numeric_limits<int>::max();
    }
    distances[startNode] = 0;

    auto compare = [&distances](shared_ptr<Node> a, shared_ptr<Node> b) {
        return distances[a] > distances[b];
    };
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, decltype(compare)> pq(compare);
    pq.push(startNode);

    bool pathFound = false;
    chrono::milliseconds duration(10);

    while (!pq.empty()) {
        shared_ptr<Node> currentNode = pq.top();
        pq.pop();
        if (!isStartOrEnd(currentNode, startNode, endNode)) {
            currentNode->state = CurrentNode;
            this_thread::sleep_for(duration);
            refreshScreen(nodes, edgeData, window);
        }

        for (const auto& neighbor: edgeData.at(currentNode)) {
            shared_ptr<Node> neighborNode = get<0>(neighbor);
            int weight = get<1>(neighbor);
            int newDist = distances[currentNode] + weight;

            if (newDist < distances[neighborNode]) {
                distances[neighborNode] = newDist;
                previous[neighborNode] = currentNode;
                if (!isStartOrEnd(neighborNode, startNode, endNode)) {
                    neighborNode->state = InQueue;
                    this_thread::sleep_for(duration);
                    refreshScreen(nodes, edgeData, window);
                }
                pq.push(neighborNode);
            }
        }

        if (!isStartOrEnd(currentNode, startNode, endNode)) {
            currentNode->state = Visited;
            this_thread::sleep_for(duration);
            refreshScreen(nodes, edgeData, window);
        }

        if (currentNode == endNode) {
            pathFound = true;
            break;
        }
    }

    vector<shared_ptr<Node>> path;
    for (shared_ptr<Node> at = endNode; at != nullptr; at = previous[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    for (int i=0; i<path.size()-1; i++) {
        for (auto& e: edgeData[path[i]]) {
            if (get<0>(e) == path[i+1]) {
                get<2>(e) = 3; // modify edge size to indicate this edge was taken
            }
        }
        if (!isStartOrEnd(path[i], startNode, endNode)) {
            path[i]->state = Path;
            chrono::milliseconds duration(50);
            this_thread::sleep_for(duration);
            refreshScreen(nodes, edgeData, window);
        }
    }

    if (pathFound) {
        cout << "min dist: " << distances[endNode] << endl;
    } else {
        cout << "no path found" << endl;
        for (const auto& n: nodes) {
            if (n->state == Visited) {
                n->state = VisitedNoPath;
            }
        }
        refreshScreen(nodes, edgeData, window);
    }
}