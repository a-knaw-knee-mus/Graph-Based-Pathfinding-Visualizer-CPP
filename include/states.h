#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
using namespace sf;
using namespace std;

struct Connection {
    shared_ptr<CircleShape> start, end;
    int weight;

    Connection(const shared_ptr<CircleShape> start, const shared_ptr<CircleShape> end, int weight=1) : start(start), end(end), weight(weight) {}
};