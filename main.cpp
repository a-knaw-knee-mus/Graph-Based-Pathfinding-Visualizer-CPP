#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "include/states.h"
using namespace sf;
using namespace std;

// get the line shapes that represent connections between nodes to be drawn to the window
vector<VertexArray> getConnectionsBetweenNodes(vector<Connection> connectionData) {
    vector<VertexArray> connections;

    for (auto& c: connectionData) {
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
        VertexArray line(Lines, 2);
        line[0].position = c.start->getPosition();
        line[0].color = lineColorStart;
        line[1].position = c.end->getPosition();
        line[1].color = lineColorEnd;
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
bool doesConnectionExist(vector<Connection> connectionData, shared_ptr<CircleShape> start, shared_ptr<CircleShape> end) {
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
    const int circleRadius = 30;
    vector<shared_ptr<CircleShape>> nodes;
    vector<Connection> connectionData;

    CircleShape node1(circleRadius-4);
    node1.setPosition(100, 100);
    node1.setOutlineColor(Color::Black);
    node1.setOutlineThickness(2);
    node1.setOrigin({ node1.getRadius(), node1.getRadius() });
    nodes.push_back(make_shared<CircleShape>(node1));

    CircleShape node2(circleRadius-4);
    node2.setPosition(300, 300);
    node2.setOutlineColor(Color::Black);
    node2.setOutlineThickness(2);
    node2.setOrigin({ node2.getRadius(), node2.getRadius() });
    nodes.push_back(make_shared<CircleShape>(node2));

    connectionData.emplace_back(nodes[0], nodes[1]);

    int currCircle = -1;
    Vector2f offset; // Offset for dragging
    int lineStartIdx=-1;
    bool isShiftPressed = false;
    bool isCtrlPressed = false;
    shared_ptr<CircleShape> startNode;
    shared_ptr<CircleShape> endNode;

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
                    if (nodes[i]->getGlobalBounds().contains(mousePos)) {
                        startNode = nodes[i];
                        if (startNode == endNode) { // override endnode
                            endNode = nullptr;
                        }
                        break;
                    }
                }
            }
            else if (isShiftPressed && Mouse::isButtonPressed(Mouse::Right)) { // set end node
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->getGlobalBounds().contains(mousePos)) {
                        endNode = nodes[i];
                        if (endNode == startNode) { // override startnode
                            startNode = nullptr;
                        }
                        break;
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
                    if (nodes[i]->getGlobalBounds().contains(mousePos)) {
                        shared_ptr<CircleShape> removedNode = nodes[i];
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
                vector<VertexArray> lines = getConnectionsBetweenNodes(connectionData);
                for (int i=0; i<connectionData.size(); i++) {
                    VertexArray line = lines[i];
                    float dist = distance(line[0].position, line[1].position, mousePos);
                    bool isMouseOver = dist <= 5.0f;
                    if (isMouseOver) {
                        connectionData.erase(connectionData.begin()+i);
                    }
                }
            }

            // add connection
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Right) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->getGlobalBounds().contains(mousePos)) {
                        lineStartIdx = i;
                        break;
                    }
                }
            }
            else if (lineStartIdx!=-1 && event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Right) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                for (int lineEndIdx = 0; lineEndIdx < nodes.size(); lineEndIdx++) {
                    if (nodes[lineEndIdx]->getGlobalBounds().contains(mousePos)) {
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
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->getGlobalBounds().contains(mousePos)) {
                        offset = mousePos - nodes[i]->getPosition();
                        currCircle = i;
                    }
                }
            }
            else if (currCircle != -1 && event.type == Event::MouseButtonReleased) {
                currCircle = -1;
            }

            // add new node
            else if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::A) {
                    // Generate a new circle
                    CircleShape newCircle(circleRadius-4);
                    Vector2f newPos;
                    bool positionFound = false;
                    for (float y = circleRadius+10; y < window.getSize().y-circleRadius-10; y += circleRadius * 2) {
                        for (float x = circleRadius+10; x < window.getSize().x-circleRadius-10; x += circleRadius * 2) {
                            newPos = Vector2f(x, y);
                            bool validPosition = true;
                            for (const auto& existingNode : nodes) {
                                float combinedRadius = newCircle.getRadius() + existingNode->getRadius();
                                Vector2f centerDiff = newPos - existingNode->getPosition();
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
                        nodes.push_back(make_shared<CircleShape>(newCircle));
                    } else {
                        cout << "No possible spot found" << endl;
                    }
                }
            }
        }

        // Dragging logic
        if (currCircle != -1 && Mouse::isButtonPressed(Mouse::Left)) {
            Vector2f mousePos = Vector2f(Mouse::getPosition(window));

            Vector2f newPos = mousePos - offset; // Calculate potential new position

            // Check collision before updating position
            bool collisionDetected = false;
            for (int i = 0; i < nodes.size(); i++) {
                if (i == currCircle) continue;
                Vector2f currCirclePoints = newPos;
                Vector2f otherCirclePoints = nodes[i]->getPosition();
                float distance = sqrt(pow(currCirclePoints.x - otherCirclePoints.x, 2) + pow(currCirclePoints.y - otherCirclePoints.y, 2));
                if (distance < nodes[currCircle]->getRadius() + nodes[i]->getRadius() + (circleRadius*0.7)) {
                    collisionDetected = true;
                    break;
                }
            }

            if (!collisionDetected) {
                nodes[currCircle]->setPosition(newPos); // Update position if no collision detected
            }
        }

        window.clear(Color::White);
        for (const auto& connection: getConnectionsBetweenNodes(connectionData)) {
            window.draw(connection);
        }
        // cout << nodes[0].getPosition().x << " " << nodes[0].getPosition().y << endl;
        for (auto& circle : nodes) {
            if (circle == startNode) {
                circle->setFillColor(Color::Blue);
            } else if (circle == endNode) {
                circle->setFillColor(Color::Green);
            } else {
                circle->setFillColor(Color::White);
            }
            window.draw(*circle);
        }
        window.display();
    }

    return 0;
}
