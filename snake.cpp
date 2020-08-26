#include "snake.h"

typedef QPair<int,int> Point;

Snake::Snake() : Snake(Direction::Right,
                       "#B0B0B0", "#C0C0C0",
                       "#C0C0C0", "#D0D0D0",
                       qMakePair(10, 11), qMakePair(10, 10)) {}

Snake::Snake(Direction _direction,
             QString _headColor, QString _bodyColor,
             QString _headColorBright, QString _bodyColorBright,
             Point _head, Point _body)
            : headColor(_headColor), bodyColor(_bodyColor)
            , headColorBright((_headColorBright)), bodyColorBright(_bodyColorBright)
            , direction(_direction), interval(0) {
    body.push_back(_body);
    body.push_back(_head);
}

Snake::Snake(const QJsonObject& obj) {
    direction = Direction(obj.value("direction").toInt());
    interval = obj.value("interval").toInt();
    score = obj.value("score").toInt();
    headColor = obj.value("headColor").toString();
    bodyColor = obj.value("bodyColor").toString();
    QString posXString = obj.value("posX").toString();
    QString posYString = obj.value("posY").toString();
    QStringList posXList = posXString.split(",");
    QStringList posYList = posYString.split(",");
    for (int i = 0; i < posXList.size(); i++) {
        body.push_back(qMakePair(posXList[i].toInt(), posYList[i].toInt()));
    }
}

Point Snake::getNewHead() const {
    Point head = *body.rbegin();
    return qMakePair(head.first + positions[int(direction) - 1].first,
                     head.second + positions[int(direction) - 1].second);
}

void Snake::eat() {
    interval += 3;
    score += 5;
}

void Snake::setNewHead(Point _head) {
    body.push_back(_head);
}

void Snake::setNewTail() {
    if (interval > 0) {
        interval -= 1;
        return;
    } else {
        body.pop_front();
    }
}

bool Snake::setDirection(Direction _direction) {
    if (int(direction) + int(_direction) != 5 && direction != _direction) {
        direction = _direction;
        return true;
    } return false;
} //如果设置direction成功,返回true;否则返回false

const QQueue<Point>& Snake::expose() const {
    return body;
}

QColor Snake::getHeadColor() const {
    return QColor(headColor);
}

QColor Snake::getBodyColor() const {
    return QColor(bodyColor);
}

QColor Snake::getHeadColorBright() const {
    return QColor(headColorBright);
}

QColor Snake::getBodyColorBright() const {
    return QColor(bodyColorBright);
}

bool Snake::validPos(Point pos) const {
    return body.contains(pos) == false;
}

int Snake::getScore() const {
    return score;
}

QJsonObject Snake::toJson() const {
    QJsonObject tempObj;
    tempObj.insert("direction", int(direction));
    tempObj.insert("headColor", headColor);
    tempObj.insert("bodyColor", bodyColor);
    tempObj.insert("interval", interval);
    tempObj.insert("score", score);
    QString posXString, posYString;
    for (const auto& each : body) {
        posXString += QString::number(each.first);
        posYString += QString::number(each.second);
        if (each != *body.rbegin()) {
            posXString += ",";
            posYString += ",";
        }
    }
    tempObj.insert("posX", posXString);
    tempObj.insert("posY", posYString);
    return tempObj;
}
