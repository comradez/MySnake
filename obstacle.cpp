#include "obstacle.h"

Obstacle::Obstacle() {}

Obstacle::Obstacle(int _posx, int _posy) : pos(qMakePair(_posx, _posy)) {}

Obstacle::Obstacle(Point _pos) : pos(_pos) {}

Point Obstacle::getPos() const {
    return pos;
}

bool Obstacle::validPos(Point _pos) {
    return pos != _pos;
}

uint qHash(const Obstacle& key) {
    return key.getPos().first * 100 + key.getPos().second;
}
