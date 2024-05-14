#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
using namespace sf;
using namespace std;

int main() {
    RenderWindow window(VideoMode(600, 600), "Graph Pathfinding");

    // Define two circles
    const int circleRadius = 30;
    vector<CircleShape> circles;

    CircleShape circle1(circleRadius-4);
    circle1.setPosition(100, 100);
    circle1.setOutlineColor(Color::Black);
    circle1.setOutlineThickness(2);
    circle1.setOrigin({ circle1.getRadius(), circle1.getRadius() });
    circles.push_back(circle1);

    CircleShape circle2(circleRadius-4);
    circle2.setPosition(300, 300);
    circle2.setOutlineColor(Color::Black);
    circle2.setOutlineThickness(2);
    circle2.setOrigin({ circle2.getRadius(), circle2.getRadius() });
    circles.push_back(circle2);

    int currCircle = -1;
    Vector2f offset; // Offset for dragging

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            else if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = Vector2f(Mouse::getPosition(window));
                if (!(mousePos.x >= 0 && mousePos.x <= window.getSize().x && mousePos.y >= 0 && mousePos.y <= window.getSize().y)) continue;
                for (int i = 0; i < circles.size(); i++) {
                    if (circles[i].getGlobalBounds().contains(mousePos)) {
                        offset = mousePos - circles[i].getPosition();
                        currCircle = i;
                    }
                }
            }
            else if (event.type == Event::MouseButtonReleased) {
                currCircle = -1;
            }
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
                            for (const auto& existingCircle : circles) {
                                float combinedRadius = newCircle.getRadius() + existingCircle.getRadius();
                                Vector2f centerDiff = newPos - existingCircle.getPosition();
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
                        circles.push_back(newCircle);
                    } else {
                        cout << "No possible spot found" << endl;
                    }
                }
            }
        }

        // Dragging logic
        if (Mouse::isButtonPressed(Mouse::Left)) {
            if (currCircle == -1) continue;
            Vector2f mousePos = Vector2f(Mouse::getPosition(window));

            Vector2f newPos = mousePos - offset; // Calculate potential new position

            // Check collision before updating position
            bool collisionDetected = false;
            for (int i = 0; i < circles.size(); i++) {
                if (i == currCircle) continue;
                Vector2f currCirclePoints = newPos;
                Vector2f otherCirclePoints = circles[i].getPosition();
                float distance = sqrt(pow(currCirclePoints.x - otherCirclePoints.x, 2) + pow(currCirclePoints.y - otherCirclePoints.y, 2));
                if (distance < circles[currCircle].getRadius() + circles[i].getRadius() + (circleRadius*0.7)) {
                    collisionDetected = true;
                    break;
                }
            }

            if (!collisionDetected) {
                circles[currCircle].setPosition(newPos); // Update position if no collision detected
            }
        }

        window.clear(Color::White);
        for (auto& circle : circles) {
            window.draw(circle);
        }
        window.display();
    }

    return 0;
}
