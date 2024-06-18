// any functions that draw to the RenderWindow or modify SFML Shapes

#include "window.h"
#include "states.h"
#include <cmath>
#include <SFML/Graphics.hpp>
using namespace std;

void drawNodes(vector<shared_ptr<Node>> nodes, RenderWindow& window) {
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
            case Visited:
                n->node.setFillColor(Color(175, 238, 238));
            break;
            case Path:
                n->node.setFillColor(Color(204, 153, 255));
            break;
            case InQueue:
                n->node.setFillColor(Color(152, 251, 152));
            break;
            case CurrentNode:
                n->node.setFillColor(Color(178, 102, 255));
            break;
            case VisitedNoPath:
                n->node.setFillColor(Color(192, 192, 192));
            break;
            default:
                n->node.setFillColor(Color::White);
            break;
        }
        window.draw(n->node);
    }
}


Color getEdgeColor(int weight) {
    switch(weight) {
        case 1:
            return Color::Red;
        case 2:
            return Color::Blue;
        case 3:
            return Color::Green;
        case 4:
            return Color::Magenta;
        case 5:
            return Color::Cyan;
        default:
            return Color::Red;
    }
}

// draw arrowheads to show edge direction
void drawArrowheads(const unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window) {
    vector<ConvexShape> arrowheads{};

    for (auto& [node, currNodeEdges] : edgeData) {
        int i = 0;
        for (auto& e: currNodeEdges) {
            Vector2f point1 = get<0>(e)->node.getPosition();
            Vector2f point2 = node->node.getPosition();
            const int nodeRadius = get<0>(e)->node.getRadius() + 4; // +4 for border

            // Calculate the angle between point1 and point2
            float dx = point2.x - point1.x;
            float dy = point2.y - point1.y;
            float angle = atan2(dy, dx) * 180 / M_PI; // Convert to degrees

            // Create a convex shape (triangle)
            ConvexShape arrowhead;
            arrowhead.setPointCount(3);

            // set arrowhead size relative to node size
            float startY = -24.f+nodeRadius;
            float height = nodeRadius*0.65;
            if (height < 5) height = 5; // set min arrowhead height
            float width = nodeRadius*0.5;
            if (width < 3.85*get<2>(e)) width = 3.85*get<2>(e); // set min arrowhead width

            arrowhead.setPoint(0, Vector2f(0.f, startY)); // Top point of the triangle
            arrowhead.setPoint(1, Vector2f(-(width/2), startY+height)); // Bottom left point
            arrowhead.setPoint(2, Vector2f((width/2), startY+height)); // Bottom right point
            arrowhead.setOrigin(0.f, -20.f); // Set origin to top point
            arrowhead.setPosition(point1); // Set position to point 1
            arrowhead.setRotation(angle-90); // Set rotation towards point 2
            arrowhead.setFillColor(getEdgeColor(get<1>(e)));

            window.draw(arrowhead);
        }
    }
}

RectangleShape getShapeForEdge(const shared_ptr<Node>& startNode, const shared_ptr<Node>& endNode, const int weight, const int thickness) {
    Vector2f startPos = startNode->node.getPosition();
    Vector2f endPos = endNode->node.getPosition();
    float distance = sqrt(pow(endPos.x - startPos.x, 2) + pow(endPos.y - startPos.y, 2));
    RectangleShape line(Vector2f(distance, thickness));
    line.setPosition(startPos);
    line.setFillColor(getEdgeColor(weight));
    float angle = atan2(endPos.y - startPos.y, endPos.x - startPos.x);
    line.setRotation(angle * 180 / M_PI);
    return line;
}

void refreshScreen(const vector<shared_ptr<Node>>& nodes, const unordered_map<shared_ptr<Node>, vector<tuple<shared_ptr<Node>, int, int>>, NodePtrHash, NodePtrEqual>& edgeData, RenderWindow& window) {
    window.clear(Color::White);

    // draw edges
    for (auto& [node, currNodeEdges] : edgeData) {
        for (auto& e: currNodeEdges) {
            RectangleShape edgeShape = getShapeForEdge(node, get<0>(e), get<1>(e), get<2>(e));
            window.draw(edgeShape);
        }
    }
    drawArrowheads(edgeData, window);
    drawNodes(nodes, window);

    window.display();
}