#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "paintwidget.h"
#include <QMainWindow>
#include <QRandomGenerator>
#include <QMessageBox>
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
    //bool eventFilter(QObject* obj, QEvent* ev) override;

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
    enum class Special {
        Ordinary = 0, EatGlass = 1
    } special;
    enum class Block {
        Nothing = 0, Obstacle = 1, Snake = 2, Head = 3, Fruit = 4
    };
    enum class Status {
        NotBegin = 0, Gaming = 1, Paused = 2, Ended = 3
    } status;
    enum class Direction {
        Nothing = 0, Left = 1, Up = 2, Down = 3, Right = 4
    } direction;
    const int dx[4] = {0, -1, 1, 0}, dy[4] = {-1, 0, 0, 1};
    const QColor colors[5] = {QColor("#273540"), QColor("#648F6C"), QColor("#BBC2C0"), QColor("#E2E5DE"), QColor("#FDB106")};
    int leftpos = 170, toppos = 40, latency = 0, Length = 15;
    QPoint headcoord, tailcoord;
    const static int Height = 40, Width = 40, StatusBarMargin = 20, Margin = 20;
    vector<vector<Block>> grid;
    vector<vector<Direction>> grid_b;
    QTimer* globalTimer;
    int tick_count;
    QQueue<Direction> operations;

    QPoint Pix2Map(QPoint p);
    QPoint Map2Pix(QPoint c);
    QPoint getNewTail(QPoint tail);
    QPoint getNewHead(QPoint head, Direction direction);
    void setNotBegin();
    void setGaming();
    void setPaused();
    void setEnded();
    void runSingleStep();
    void generateFruit();
    bool eat(QPoint head);
    bool check(QPoint head);
    void reset();
};
#endif // MAINWINDOW_H
