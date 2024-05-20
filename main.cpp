#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <utility>
#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>
#include "include/states.h"
#include "include/pathfinding.h"
#include "include/window.h"
using namespace sf;
using namespace std;

// Remove Visited, InQueue and Path Cells
void resetPathfinding(vector<shared_ptr<Node>> nodes) {
    for (const auto& n: nodes) {
        if (n->state == Visited || n->state == Path || n->state == InQueue || n->state == VisitedNoPath) {
            n->state = Clear;
        }
    }
}

void addNode(vector<shared_ptr<Node>>& nodes, const int circleRadius, RenderWindow& window) {
    CircleShape newCircle(circleRadius-4);
    Vector2f newPos;
    bool positionFound = false;
    for (float y = circleRadius+10; y < window.getSize().y-circleRadius-10; y += circleRadius * 2) {
        for (float x = circleRadius+10; x < window.getSize().x-circleRadius-10; x += circleRadius * 2) {
            newPos = Vector2f(x, y);
            bool validPosition = true;
            for (const auto& existingNode : nodes) {
                float combinedRadius = newCircle.getRadius() + existingNode->node.getRadius();
                Vector2f centerDiff = newPos - existingNode->node.getPosition();
                float centerDist = sqrt(centerDiff.x * centerDiff.x + centerDiff.y * centerDiff.y);
                if (centerDist < (combinedRadius + (circleRadius*0.7))) { // Ensure new circle doesn't overlap or come too close
                    validPosition = false;
                    break;
                }
            }
            if (validPosition) {
                positionFound = true;
                break;
            }
        }
        if (positionFound) break;
    }
    if (positionFound) {
        newCircle.setPosition(newPos);
        newCircle.setOutlineColor(Color::Black);
        newCircle.setOutlineThickness(2);
        newCircle.setOrigin({ newCircle.getRadius(), newCircle.getRadius() });
        nodes.push_back(make_shared<Node>(Node(newCircle)));
    } else {
        cout << "No possible spot found" << endl;
    }
}

void genRandomGraph(vector<shared_ptr<Node>>& nodes, unordered_map<shared_ptr<Node>, vector<pair<shared_ptr<Node>, int>>, NodePtrHash, NodePtrEqual>& edgeData, const int circleRadius, RenderWindow& window) {
    nodes.clear();
    edgeData.clear();

    // generate nodes
    CircleShape newCircle(circleRadius-4);
    const int ratio = 4; // used to space nodes; increase/decrease num nodes; min=2
    const int numRows = window.getSize().x / ((circleRadius*ratio)+(circleRadius*0.7));
    const int numCols = window.getSize().y / ((circleRadius*ratio)+(circleRadius*0.7));
    vector<vector<shared_ptr<Node>>> nodeGrid; // keep track of added nodes in a grid to add random edges
    for (int i=0; i<numRows; i++) {
        nodeGrid.push_back(vector<shared_ptr<Node>>{});
        for (int j=0; j<numCols; j++) {
            float x=i*((circleRadius*ratio)+(circleRadius*0.7)) + (circleRadius+10);
            float y=j*((circleRadius*ratio)+(circleRadius*0.7)) + (circleRadius+10);
            newCircle.setPosition({x, y});
            newCircle.setOutlineColor(Color::Black);
            newCircle.setOutlineThickness(2);
            newCircle.setOrigin({ newCircle.getRadius(), newCircle.getRadius() });
            nodes.push_back(make_shared<Node>(Node(newCircle)));
            nodeGrid[i].push_back(nodes[nodes.size()-1]);
        }
    }

    // Define possible movement directions (right, down, bottomleft, bottomright)
    constexpr int dx[] = {1, 0, 1};
    constexpr int dy[] = {0, 1, 1};
    for (int row=0; row<nodeGrid.size(); row++) {
        for (int col=0; col<nodeGrid[0].size(); col++) {
            for (int neighborId=0; neighborId<3; neighborId++) { // make edge with right, down, and bottom right node from current
                if (row+dx[neighborId] >= nodeGrid.size()) continue;
                if (col+dy[neighborId] >= nodeGrid.size()) continue;
                int direction = rand() % 2; // 0 or 1; from current node or to current node
                int weight = rand() % 5 + 1;
                if (direction == 0) {
                    edgeData[nodeGrid[row][col]].emplace_back(nodeGrid[row+dx[neighborId]][col+dy[neighborId]], weight);
                } else {
                    edgeData[nodeGrid[row+dx[neighborId]][col+dy[neighborId]]].emplace_back(nodeGrid[row][col], weight);
                }
            }
        }
    }
}

// calc distance between a line and another point
float distance(Vector2f v1, Vector2f v2, Vector2f p) {
    float a = v2.y - v1.y;
    float b = v1.x - v2.x;
    float c = (v2.x * v1.y) - (v1.x * v2.y);

    return abs((a * p.x) + (b * p.y) + c) / sqrt((a * a) + (b * b));
}

// check if edge between start and end node exists
bool doesConnectionExist(unordered_map<shared_ptr<Node>, vector<pair<shared_ptr<Node>, int>>, NodePtrHash, NodePtrEqual>& edgeData, shared_ptr<Node>& start, shared_ptr<Node>& end) {
    for (const auto& edge : edgeData[start]) {
        if (edge.first == end) return true;
    }
    return false;
}

int main() {
    RenderWindow window(VideoMode(600, 600), "Graph Pathfinding");

    // Define two circles
    int nodeRadius = 10;
    if (nodeRadius < 5) nodeRadius = 5; // anything smaller wont render
    vector<shared_ptr<Node>> nodes;
    //vector<Edge> edgeData;
    unordered_map<shared_ptr<Node>, vector<pair<shared_ptr<Node>, int>>, NodePtrHash, NodePtrEqual> edgeData;

    // set 2 default nodes with 1 edge
    CircleShape node1(nodeRadius-4);
    node1.setPosition(100, 100);
    node1.setOutlineColor(Color::Black);
    node1.setOutlineThickness(2);
    node1.setOrigin({ node1.getRadius(), node1.getRadius() });
    nodes.push_back(make_shared<Node>(Node(node1)));

    CircleShape node2(nodeRadius-4);
    node2.setPosition(300, 300);
    node2.setOutlineColor(Color::Black);
    node2.setOutlineThickness(2);
    node2.setOrigin({ node2.getRadius(), node2.getRadius() });
    nodes.push_back(make_shared<Node>(Node(node2)));

    edgeData[nodes[0]].emplace_back(nodes[1], 1); // edge from node1 to node2

    int currCircle = -1;
    int lineStartIdx=-1;
    bool isShiftPressed = false;
    bool isCtrlPressed = false;
    shared_ptr<Node> startNode;
    shared_ptr<Node> endNode;

    while (window.isOpen()) {
        Event event{};
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            // begin search
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::Enter) {
                if (startNode == nullptr) continue;
                if (endNode == nullptr) continue;
                resetPathfinding(nodes);
                //findDijkstraPath(nodes, edgeData, window, startNode, endNode);
                bellmanFord(nodes, edgeData, window, startNode, endNode);
            }

            // add start/end node
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::LShift) {
                isShiftPressed = true;
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::LShift) {
                isShiftPressed = false;
            }
            else if (isShiftPressed && Mouse::isButtonPressed(Mouse::Left)) { // set start node
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->node.getGlobalBounds().contains(mousePos)) {
                        for (auto& n: nodes) {
                            if (n->state == Start) {
                                n->state = Clear;
                            }
                        }
                        startNode = nodes[i];
                        nodes[i]->state = Start;
                        if (startNode == endNode) { // override endnode
                            endNode = nullptr;
                        }
                        resetPathfinding(nodes);
                    }
                }
            }
            else if (isShiftPressed && Mouse::isButtonPressed(Mouse::Right)) { // set end node
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->node.getGlobalBounds().contains(mousePos)) {
                        for (auto& n: nodes) {
                            if (n->state == End) {
                                n->state = Clear;
                            }
                        }
                        endNode = nodes[i];
                        nodes[i]->state = End;
                        if (endNode == startNode) { // override startnode
                            startNode = nullptr;
                        }
                        resetPathfinding(nodes);
                    }
                }
            }

            // delete node/edge on shift+left click
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::LControl) {
                isCtrlPressed = true;
            }
            else if (event.type == Event::KeyReleased && event.key.code == Keyboard::LControl) {
                isCtrlPressed = false;
            }
            else if (isCtrlPressed && Mouse::isButtonPressed(Mouse::Left)) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                if (!(mousePos.x >= 0 && mousePos.x <= window.getSize().x && mousePos.y >= 0 && mousePos.y <= window.getSize().y)) continue;

                // delete node
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->node.getGlobalBounds().contains(mousePos)) {
                        shared_ptr<Node> removedNode = nodes[i];
                        if (removedNode->state == Start) {
                            startNode = nullptr;
                        } else if (removedNode->state == End) {
                            endNode = nullptr;
                        }

                        // delete edges going to this node
                        for (auto& [node, currNodeEdges] : edgeData) {
                            int j = 0;
                            for (auto& e: currNodeEdges) {
                                if (e.first == removedNode) {
                                    edgeData[node].erase(edgeData[node].begin()+j);
                                }
                                j++;
                            }
                        }
                        edgeData.erase(nodes[i]); // delete edges coming from this node
                        nodes.erase(nodes.begin() + i);
                        resetPathfinding(nodes);
                        break;  // Exit loop after deleting the node and its edges
                    }
                }

                // delete edge
                for (auto& [node, currNodeEdges] : edgeData) {
                    for (auto it = currNodeEdges.begin(); it != currNodeEdges.end(); ) {
                        auto& e = *it;

                        RectangleShape edgeShape = getShapeForEdge(node, e.first, e.second);
                        std::array<Vector2f, 4> vertices;
                        for (int vertexId = 0; vertexId < 4; ++vertexId) {
                            vertices[vertexId] = edgeShape.getTransform().transformPoint(edgeShape.getPoint(vertexId));
                        }
                        int n = vertices.size();
                        int a, b;
                        bool cursorInEdge = false;
                        for (a = 0, b = n - 1; a < n; b = a++) {
                            if (((vertices[a].y > mousePos.y) != (vertices[b].y > mousePos.y)) &&
                                (mousePos.x < (vertices[b].x - vertices[a].x) * (mousePos.y - vertices[a].y) / (vertices[b].y - vertices[a].y) + vertices[a].x)) {
                                cursorInEdge = !cursorInEdge;
                                }
                        }
                        if (cursorInEdge) {
                            it = currNodeEdges.erase(it); // Erase returns the next iterator
                            resetPathfinding(nodes);
                        } else {
                            ++it; // Increment iterator if not erasing
                        }
                    }
                }
            }

            // add edge
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Right) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->node.getGlobalBounds().contains(mousePos)) {
                        lineStartIdx = i;
                        break;
                    }
                }
            }
            else if (lineStartIdx!=-1 && event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Right) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                for (int lineEndIdx = 0; lineEndIdx < nodes.size(); lineEndIdx++) {
                    if (lineStartIdx == lineEndIdx) continue; // cant make a self edge
                    if (nodes[lineEndIdx]->node.getGlobalBounds().contains(mousePos)) {
                        if (!doesConnectionExist(edgeData, nodes[lineStartIdx], nodes[lineEndIdx])) {
                            edgeData[nodes[lineStartIdx]].emplace_back(nodes[lineEndIdx], 2);
                            resetPathfinding(nodes);
                        }

                        lineStartIdx = -1;
                        break;
                    }
                }
            }

            // get current held node
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                if (mousePos.x < (nodeRadius) || mousePos.x > window.getSize().x-nodeRadius || mousePos.y < nodeRadius || mousePos.y > window.getSize().y-nodeRadius) continue;
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->node.getGlobalBounds().contains(mousePos)) {
                        currCircle = i;
                    }
                }
            }
            else if (currCircle != -1 && event.type == Event::MouseButtonReleased) {
                currCircle = -1;
            }

            // generate random graph
            else if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Space) {
                    startNode = nullptr;
                    endNode = nullptr;
                    genRandomGraph(nodes, edgeData, nodeRadius, window);
                }
            }

            // add new node
            else if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::A) {
                    addNode(nodes, nodeRadius, window);
                }
            }
        }

        // Dragging logic
        if (currCircle != -1 && Mouse::isButtonPressed(Mouse::Left)) {
            Vector2f mousePos = Vector2f(Mouse::getPosition(window));
            if (mousePos.x < (nodeRadius) || mousePos.x > window.getSize().x-nodeRadius || mousePos.y < nodeRadius || mousePos.y > window.getSize().y-nodeRadius) continue;

            Vector2f newPos = mousePos; // Calculate potential new position

            // Check collision before updating position
            bool collisionDetected = false;
            for (int i = 0; i < nodes.size(); i++) {
                if (i == currCircle) continue;
                Vector2f currCirclePoints = newPos;
                Vector2f otherCirclePoints = nodes[i]->node.getPosition();
                float distance = sqrt(pow(currCirclePoints.x - otherCirclePoints.x, 2) + pow(currCirclePoints.y - otherCirclePoints.y, 2));
                if (distance < nodes[currCircle]->node.getRadius() + nodes[i]->node.getRadius() + (nodeRadius*0.7)) {
                    collisionDetected = true;
                    break;
                }
            }

            if (!collisionDetected) {
                nodes[currCircle]->node.setPosition(newPos); // Update position if no collision detected
            }
        }

        refreshScreen(nodes, edgeData, window);
    }

    return 0;
}
