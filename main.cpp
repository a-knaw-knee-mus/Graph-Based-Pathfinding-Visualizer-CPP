#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>
#include "include/states.h"
using namespace sf;
using namespace std;

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

void genRandomGraph(vector<shared_ptr<Node>>& nodes, vector<Connection>& connectionData, const int circleRadius, RenderWindow& window) {
    nodes.clear();
    connectionData.clear();

    // generate nodes
    CircleShape newCircle(circleRadius-4);
    const int ratio = 4; // used to space nodes; increase/decrease num nodes; min=2
    int numRows = window.getSize().x / ((circleRadius*ratio)+(circleRadius*0.7));
    int numCols = window.getSize().y / ((circleRadius*ratio)+(circleRadius*0.7));
    vector<vector<shared_ptr<Node>>> nodeGrid; // keep track of added nodes in a grid to add random connections
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

    // generate connections between all sibling nodes (including diagonal siblings)

    // Define possible movement directions (right, down, bottomleft, bottomright)
    constexpr int dx[] = {1, 0, 1};
    constexpr  int dy[] = {0, 1, 1};
    for (int row=0; row<nodeGrid.size(); row++) {
        for (int col=0; col<nodeGrid[0].size(); col++) {
            for (int neighborId=0; neighborId<3; neighborId++) { // make connection with right, down, and bottom right node from current
                if (row+dx[neighborId] >= nodeGrid.size()) continue;
                if (col+dy[neighborId] >= nodeGrid.size()) continue;
                int direction = rand() % 2; // 0 or 1; from current node or to current node
                if (direction == 0) {
                    connectionData.push_back({nodeGrid[row][col], nodeGrid[row+dx[neighborId]][col+dy[neighborId]]});
                } else {
                    connectionData.push_back({nodeGrid[row+dx[neighborId]][col+dy[neighborId]], nodeGrid[row][col]});
                }
            }
        }
    }
}

// get the line shapes that represent connections between nodes to be drawn to the window
vector<RectangleShape> getConnectionsBetweenNodes(vector<Connection> connectionData) {
    vector<RectangleShape> connections;

    for (auto& c : connectionData) {
        Color lineColorStart{}, lineColorEnd{};
        switch(c.weight) {
            case 1:
                lineColorStart = Color::Red;
                lineColorEnd = Color::Blue;
                break;
            case 2:
                lineColorStart = Color::Green;
                lineColorEnd = Color::Yellow;
                break;
            default:
                lineColorStart = Color::Red;
                lineColorEnd = Color::Blue;
                break;
        }
        Vector2f startPos = c.start->node.getPosition();
        Vector2f endPos = c.end->node.getPosition();
        float distance = sqrt(pow(endPos.x - startPos.x, 2) + pow(endPos.y - startPos.y, 2));
        RectangleShape line(Vector2f(distance, 1));
        line.setPosition(startPos);
        line.setFillColor(Color::Red);
        float angle = atan2(endPos.y - startPos.y, endPos.x - startPos.x);
        line.setRotation(angle * 180 / M_PI);
        connections.push_back(line);
    }
    return connections;
}

// calc distance between a line and another point
float distance(Vector2f v1, Vector2f v2, Vector2f p) {
    float a = v2.y - v1.y;
    float b = v1.x - v2.x;
    float c = (v2.x * v1.y) - (v1.x * v2.y);

    return abs((a * p.x) + (b * p.y) + c) / sqrt((a * a) + (b * b));
}

// check if connection between 2 nodes exists in either direction
bool doesConnectionExist(vector<Connection> connectionData, shared_ptr<Node> start, shared_ptr<Node> end) {
    for (const auto& connection : connectionData) {
        if ((connection.start == start && connection.end == end) || (connection.start == end && connection.end == start)) {
            return true;
        }
    }

    return false;
}

int main() {
    RenderWindow window(VideoMode(600, 600), "Graph Pathfinding");

    // Define two circles
    const int circleRadius = 20;
    vector<shared_ptr<Node>> nodes;
    vector<Connection> connectionData;

    CircleShape node1(circleRadius-4);
    node1.setPosition(100, 100);
    node1.setOutlineColor(Color::Black);
    node1.setOutlineThickness(2);
    node1.setOrigin({ node1.getRadius(), node1.getRadius() });
    nodes.push_back(make_shared<Node>(Node(node1)));

    CircleShape node2(circleRadius-4);
    node2.setPosition(300, 300);
    node2.setOutlineColor(Color::Black);
    node2.setOutlineThickness(2);
    node2.setOrigin({ node2.getRadius(), node2.getRadius() });
    nodes.push_back(make_shared<Node>(Node(node2)));

    connectionData.emplace_back(nodes[0], nodes[1]);

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
                    if (nodes[i]->state == Start) {
                        nodes[i]->state = Clear;
                    }
                    if (nodes[i]->node.getGlobalBounds().contains(mousePos)) {
                        startNode = nodes[i];
                        nodes[i]->state = Start;
                        if (startNode == endNode) { // override endnode
                            endNode = nullptr;
                        }
                    }
                }
            }
            else if (isShiftPressed && Mouse::isButtonPressed(Mouse::Right)) { // set end node
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->state == End) {
                        nodes[i]->state = Clear;
                    }
                    if (nodes[i]->node.getGlobalBounds().contains(mousePos)) {
                        endNode = nodes[i];
                        nodes[i]->state = End;
                        if (endNode == startNode) { // override startnode
                            startNode = nullptr;
                        }
                    }
                }
            }

            // delete node/connection on shift+left click
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
                        nodes.erase(nodes.begin() + i);

                        // Use a lambda function to check if a connection should be deleted
                        auto shouldDeleteConnection = [&](const Connection& connection) {
                            return connection.start == removedNode || connection.end == removedNode;
                        };

                        connectionData.erase(remove_if(connectionData.begin(), connectionData.end(), shouldDeleteConnection), connectionData.end());
                        break;  // Exit loop after deleting the node and its connections
                    }
                }

                // delete connection
                vector<RectangleShape> lines = getConnectionsBetweenNodes(connectionData);
                for (int i=0; i<connectionData.size(); i++) {
                    if (lines[i].getGlobalBounds().contains(mousePos)) {
                        connectionData.erase(connectionData.begin()+i);
                    }
                }
            }

            // add connection
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
                    if (nodes[lineEndIdx]->node.getGlobalBounds().contains(mousePos)) {
                        if (!doesConnectionExist(connectionData, nodes[lineStartIdx], nodes[lineEndIdx])) {
                            connectionData.emplace_back(nodes[lineStartIdx], nodes[lineEndIdx]);
                        }

                        lineStartIdx = -1;
                        break;
                    }
                }
            }

            // get current held node
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                if (mousePos.x < (circleRadius) || mousePos.x > window.getSize().x-circleRadius || mousePos.y < circleRadius || mousePos.y > window.getSize().y-circleRadius) continue;
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
                    genRandomGraph(nodes, connectionData, circleRadius, window);
                }
            }

            // add new node
            else if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::A) {
                    // Generate a new circle
                    addNode(nodes, circleRadius, window);
                }
            }
        }

        // Dragging logic
        if (currCircle != -1 && Mouse::isButtonPressed(Mouse::Left)) {
            Vector2f mousePos = Vector2f(Mouse::getPosition(window));
            if (mousePos.x < (circleRadius) || mousePos.x > window.getSize().x-circleRadius || mousePos.y < circleRadius || mousePos.y > window.getSize().y-circleRadius) continue;

            Vector2f newPos = mousePos; // Calculate potential new position

            // Check collision before updating position
            bool collisionDetected = false;
            for (int i = 0; i < nodes.size(); i++) {
                if (i == currCircle) continue;
                Vector2f currCirclePoints = newPos;
                Vector2f otherCirclePoints = nodes[i]->node.getPosition();
                float distance = sqrt(pow(currCirclePoints.x - otherCirclePoints.x, 2) + pow(currCirclePoints.y - otherCirclePoints.y, 2));
                if (distance < nodes[currCircle]->node.getRadius() + nodes[i]->node.getRadius() + (circleRadius*0.7)) {
                    collisionDetected = true;
                    break;
                }
            }

            if (!collisionDetected) {
                nodes[currCircle]->node.setPosition(newPos); // Update position if no collision detected
            }
        }

        window.clear(Color::White);
        for (const auto& connection: getConnectionsBetweenNodes(connectionData)) {
            window.draw(connection);
        }
        // cout << nodes[0].getPosition().x << " " << nodes[0].getPosition().y << endl;
        for (auto& n : nodes) {
            switch (n->state) {
                case Clear:
                    n->node.setFillColor(Color::White);
                    break;
                case Start:
                    n->node.setFillColor(Color(255, 0, 0));
                    break;
                case End:
                    n->node.setFillColor(Color(255, 128, 0));
                    break;
                default:
                    n->node.setFillColor(Color::White);
                    break;
            }
            window.draw(n->node);
        }
        window.display();
    }

    return 0;
}
