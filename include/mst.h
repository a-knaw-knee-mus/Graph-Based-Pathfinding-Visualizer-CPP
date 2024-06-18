#pragma once

void kruskal(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window);

void prim(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window);
