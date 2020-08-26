#ifndef BASE_H
#define BASE_H
class Assist {
private:
    friend class MainWindow;
    friend class Snake;
    enum class Special {
        Ordinary = 0, EatGlass, Transparent
    };
    enum class Block {
        Nothing = 0, Obstacle, Snake, Head, Fruit
    };
    enum class Status {
        NotBegin = 0, Gaming, Paused, Ended
    };
    enum class Direction {
        Nothing = 0, Left, Up, Down, Right
    };
};
#endif
