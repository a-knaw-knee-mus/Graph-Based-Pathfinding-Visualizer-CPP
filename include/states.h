#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
using namespace sf;
using namespace std;

enum nodeState {
    Clear, // empty node
    Start, // start node
    End, // end node
    Path, // node part of final path
    Visited, // visited node
    InQueue, // node set to be visited in the future
    CurrentNode, // current node selected from InQueue to be processed
    VisitedNoPath, // visited nodes where a path wasn't found
};

struct Node {
    CircleShape node;
    nodeState state;

    Node(const CircleShape node, const nodeState state=Clear) : node(node), state(state) {}
};

struct Edge {
    shared_ptr<Node> start, end;
    int weight;

    Edge(const shared_ptr<Node> start, const shared_ptr<Node> end, const int weight=2) : start(start), end(end), weight(weight) {}
};

