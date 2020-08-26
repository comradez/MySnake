#ifndef SNAKE_H
#define SNAKE_H

#include "base.h"
#include <QQueue>
#include <QString>
#include <QPair>
#include <QColor>
#include <QJsonObject>

class Snake {
private:
    typedef Assist::Direction Direction;
    typedef QPair<int,int> Point;
    const QPair<int,int> positions[4] = {
        qMakePair(0, -1),
        qMakePair(-1, 0),
        qMakePair(1, 0),
        qMakePair(0, 1)
    };
    QString headColor, bodyColor, headColorBright, bodyColorBright;
    QQueue<Point> body;
    Direction direction;
    int interval, score = 0;

public:
    Snake();
    Snake(Direction _direction,
          QString _headColor, QString _bodyColor,
          QString _headColorBright, QString _bodyColorBright,
          Point _head, Point _body);
    Snake(const QJsonObject& _obj);
    void eat();
    Point getNewHead() const;
    void setNewHead(Point _head);
    void setNewTail();
    bool setDirection(Direction _direction);
    const QQueue<Point>& expose() const;
    QColor getHeadColor() const;
    QColor getBodyColor() const;
    QColor getHeadColorBright() const;
    QColor getBodyColorBright() const;
    bool validPos(Point pos) const;
    int getScore() const;
    QJsonObject toJson() const;
};

#endif // SNAKE_H
