#pragma once
#include <vector>
#include <memory>
#include "states.h"
#include <SFML/Graphics.hpp>

void drawNodes(vector<shared_ptr<Node>> nodes, RenderWindow& window);