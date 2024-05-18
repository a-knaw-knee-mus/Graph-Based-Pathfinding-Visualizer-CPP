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

class UniqueIDGenerator {
public:
    static int generateID() {
        static int counter = 0;
        return ++counter;
    }
};

struct Node {
    CircleShape node;
    nodeState state;
    int id;

    Node(const CircleShape node, const nodeState state=Clear) : node(node), state(state) {
        id = UniqueIDGenerator::generateID();
    }

    bool operator==(const Node& other) const {
        return id == other.id;
    }

    struct Hash {
        size_t operator()(const Node& node) const {
            return hash<int>()(node.id);
        }
    };
};


struct NodePtrHash {
    size_t operator()(const shared_ptr<Node>& nodePtr) const {
        return hash<int>()(nodePtr->id);
    }
};

struct NodePtrEqual {
    bool operator()(const shared_ptr<Node>& lhs, const shared_ptr<Node>& rhs) const {
        return lhs->id == rhs->id;
    }
};

struct Edge {
    shared_ptr<Node> start, end;
    int weight;

    Edge(const shared_ptr<Node> start, const shared_ptr<Node> end, const int weight=1) : start(start), end(end), weight(weight) {}
};

