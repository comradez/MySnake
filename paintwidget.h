#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>

namespace Ui {
class paintWidget;
}

class paintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit paintWidget(QWidget *parent = nullptr);
    ~paintWidget();
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::paintWidget *ui;
};

#endif // PAINTWIDGET_H
