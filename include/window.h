#pragma once
#include <vector>
#include <memory>
#include "states.h"
#include <SFML/Graphics.hpp>

void drawNodes(vector<shared_ptr<Node>> nodes, RenderWindow& window);

RectangleShape getShapeForEdge(const shared_ptr<Node>& startNode, const shared_ptr<Node>& endNode, int weight, int thickness);

void drawArrowheads(const unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window);

Color getEdgeColor(int weight);

void refreshScreen(const vector<shared_ptr<Node>>& nodes, const unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window);