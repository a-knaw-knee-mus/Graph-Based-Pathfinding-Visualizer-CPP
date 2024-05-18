#include "pathfinding.h"
#include <vector>
#include <iostream>
#include <memory>
#include <limits>
#include <queue>
#include <SFML/Graphics.hpp>
#include "states.h"
using namespace std;
using namespace sf;

void findDijkstraPath(vector<shared_ptr<Node>>& nodes, const unordered_map<shared_ptr<Node>, vector<pair<shared_ptr<Node>, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window, const shared_ptr<Node>& startNode, const shared_ptr<Node>& endNode) {
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

    while (!pq.empty()) {
        shared_ptr<Node> currentNode = pq.top();
        pq.pop();

        if (currentNode == endNode) {
            pathFound = true;
            break;
        }
        for (const auto& neighbor: edgeData.at(currentNode)) {
            shared_ptr<Node> neighborNode = neighbor.first;
            int weight = neighbor.second;
            int newDist = distances[currentNode] + weight;

            if (newDist < distances[neighborNode]) {
                distances[neighborNode] = newDist;
                previous[neighborNode] = currentNode;
                pq.push(neighborNode);
            }
        }
    }

    vector<shared_ptr<Node>> path;
    for (shared_ptr<Node> at = endNode; at != nullptr; at = previous[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    for (const auto& n: path) {
        if (n == startNode) continue;
        if (n == endNode) continue;
        n->state = Path;
    }

    if (pathFound) {
        cout << "min dist: " << distances[endNode] << endl;
    } else {
        cout << "no path found" << endl;
    }
}