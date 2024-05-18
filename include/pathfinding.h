#pragma once
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "states.h"

void findDijkstraPath(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<pair<shared_ptr<Node>, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window, shared_ptr<Node>& startNode, shared_ptr<Node>& endNode);