#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QPair>

typedef QPair<int,int> Point;

class Obstacle {
private:
    Point pos;
public:
    Obstacle();
    Obstacle(int _posx, int _posy);
    Obstacle(Point _pos);
    Point getPos() const;
    bool operator==(const Obstacle& b) const {
        return pos == b.pos;
    }
    bool operator!=(const Obstacle& b) const {
        return pos != b.pos;
    }
    bool validPos(Point _pos);
};

uint qHash(const Obstacle& key);

#endif // OBSTACLE_H
