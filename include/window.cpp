#include "window.h"
#include "states.h"

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