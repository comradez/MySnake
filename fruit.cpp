#include "fruit.h"

Fruit::Fruit() {
    pos = qMakePair(20, 20);
}

Fruit::Fruit(const QJsonObject& obj) {
    pos = qMakePair(obj.value("posX").toInt(), obj.value("posY").toInt());
}

bool Fruit::checkEat(Point _pos) const {
    return pos == _pos;
}

Point Fruit::generateNewFruit(int height, int width) {
    int randomval = QRandomGenerator::global()->bounded(0, height * width + 1);
    return qMakePair(randomval / width, randomval % width);
}

void Fruit::setNewFruit(Point _pos) {
    pos = _pos;
}

Point Fruit::getPos() const {
    return pos;
}

bool Fruit::validPos(Point _pos) const {
    return pos != _pos;
}

QJsonObject Fruit::toJson() const {
    QJsonObject obj;
    obj.insert("posX", pos.first);
    obj.insert("posY", pos.second);
    return obj;
}
