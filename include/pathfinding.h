#pragma once
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "states.h"

void findDijkstraPath(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window, const shared_ptr<Node>& startNode, const shared_ptr<Node>& endNode);

void bellmanFord(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window, const shared_ptr<Node>& startNode, const shared_ptr<Node>& endNode);