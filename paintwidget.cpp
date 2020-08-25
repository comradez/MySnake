#include "paintwidget.h"
#include "ui_paintwidget.h"

paintWidget::paintWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::paintWidget)
{
    ui->setupUi(this);
}

paintWidget::~paintWidget()
{
    delete ui;
}
