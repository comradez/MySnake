#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base.h"
#include "snake.h"
#include "obstacle.h"
#include "fruit.h"
#include <QMainWindow>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QSet>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPoint>
#include <QColor>
#include <QEvent>
#include <QPainter>
#include <QTimer>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <vector>
#include <QQueue>
#include <QVector>
#include <QFile>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <cassert>
using std::vector;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void on_actionDon_t_touch_triggered();

    void on_actionnew_npy_triggered();

    void on_actionbecome_rich_triggered();

    void on_actionDon_t_touch_2_triggered();

    void on_touchButton_1_clicked();

    void on_touchButton_2_clicked();

    void on_touchButton_3_clicked();

private:
    Ui::MainWindow *ui;
    typedef Assist::Special Special;
    typedef Assist::Block Block;
    typedef Assist::Status Status;
    typedef Assist::Direction Direction;
    typedef QPair<int,int> Point;
    Special special;
    Status status;
    Direction direction;
    Snake* snake1;
    QSet<Obstacle> obstacles;
    QColor canvasColor, obstacleColor, fruitColor;
    QColor canvasColorBright, obstacleColorBright, fruitColorBright;
    Fruit* fruit;
    QAction* actionStart;
    QAction* actionPause;
    QAction* actionContinue;
    QAction* actionRestart;
    QAction* actionSave;
    QAction* actionLoad;
    QAction* actionExit;

    const int speeds[5] = {150, 125, 100, 75, 50};
    int leftpos = 170, toppos = 60, Height = 40, Width = 40, tickCount = 0;
    const static int Length = 15, Margin = 20;
    QTimer* globalTimer;
    QQueue<Direction> operations;
    QFile saveFile, loadFile;

    Point Pix2Map(QPoint p);
    QPoint Map2Pix(Point c);
    bool inTheMap(QPoint _point);
    bool checkOverFlow(Point pos);
    void setNotBegin();
    void setGaming();
    void setPaused();
    void setEnded();
    void runSingleStep();
    void generateNewFruit(Point newHead);
    void save();
    void load();
    void reset();
    void setState(QString code);
    Block getCoordType(Point pos);
    QJsonObject obstaclesToJson() const;
    void jsonToObstacles(QJsonObject obj);
};
#endif // MAINWINDOW_H
