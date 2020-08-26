#ifndef FRUIT_H
#define FRUIT_H

#include <QRandomGenerator>
#include <QJsonObject>
#include <QPair>
typedef QPair<int,int> Point;

class Fruit {
private:
    Point pos;
public:
    Fruit();
    Fruit(const QJsonObject& obj);
    bool checkEat(Point _pos) const;
    Point generateNewFruit(int height, int width);
    void setNewFruit(Point _pos);
    Point getPos() const;
    bool validPos(Point _pos) const;
    QJsonObject toJson() const;
};

#endif // FRUIT_H
