#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
using namespace sf;
using namespace std;

enum cellState {
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
    cellState state;

    Node(const CircleShape node, const cellState state=Clear) : node(node), state(state) {}
};

struct Edge {
    shared_ptr<Node> start, end;
    int weight;

    Edge(const shared_ptr<Node> start, const shared_ptr<Node> end, const int weight=1) : start(start), end(end), weight(weight) {}
};

