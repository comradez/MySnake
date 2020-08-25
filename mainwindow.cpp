#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , special(Special::Ordinary)
    , status(Status::NotBegin)
    , direction(Direction::Right)
    , headcoord(10, 11)
    , tailcoord(10, 10)
    , globalTimer(new QTimer(this))
    , tick_count(0)
    , score(0) {
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("MySnake"));
    //Set the size of the grid
    ui->lineEdit->setVisible(false);
    ui->lineEdit->setEnabled(false);
    ui->label_2->setVisible(false);
    ui->touchButton_1->setVisible(false);
    ui->touchButton_1->setEnabled(false);
    ui->touchButton_2->setVisible(false);
    ui->touchButton_2->setEnabled(false);
    ui->touchButton_3->setVisible(false);
    ui->touchButton_3->setEnabled(false);
    ui->spinBox->setMaximum(5);
    ui->spinBox->setMinimum(1);
    grid.resize(Height);
    grid_b.resize(Height);
    for (int i = 0; i < Height; i++) {
        grid[i].resize(Width);
        grid_b[i].resize(Width);
    }
    grid[10][10] = Block::Snake;
    grid_b[10][10] = Direction::Right;
    grid[10][11] = Block::Head;
    grid_b[10][11] = Direction::Nothing;
    setNotBegin();
    generateFruit();
    globalTimer->start(50);
    connect(ui->startButton, &QPushButton::clicked, [=]() {
        setGaming();
    });
    connect(ui->pauseButton, &QPushButton::clicked, [=]() {
        setPaused() ;
    });
    connect(ui->continueButton, &QPushButton::clicked, [=]() {
        setGaming();
    });
    connect(ui->restartButton, &QPushButton::clicked, [=]() {
        reset();
    });
    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        save();
    });
    connect(ui->loadButton, &QPushButton::clicked, [=]() {
        load();
    });
    connect(ui->exitButton, &QPushButton::clicked, [=]() {
        close();
    });
    connect(globalTimer, &QTimer::timeout, [=]() {
        if (status == Status::Gaming)
            runSingleStep();
        repaint();
    });
    repaint();
}

MainWindow::~MainWindow() {
    delete ui;
}

QPoint MainWindow::Pix2Map(QPoint p) {
    int px = p.x(), py = p.y();
    int cx = (px - leftpos) / Length;
    int cy = (py - toppos) / Length;
    return QPoint(cx, cy);
}

QPoint MainWindow::Map2Pix(QPoint c) {
    int cx = c.x(), cy = c.y();
    int px = cx * Length + leftpos;
    int py = cy * Length + toppos;
    return QPoint(px, py);
}

void MainWindow::paintEvent(QPaintEvent*) {
    int HorizentalLength = (frameGeometry().width() - Margin - leftpos) / Width;
    int VerticalLength = (frameGeometry().height() - Margin - toppos) / Height;
    Length = qMin(HorizentalLength, VerticalLength);

    QPainter painter(this);
    painter.translate(leftpos, toppos);
    QPen pen(QColor("#000000"));
    QBrush brush(QColor("#273540"));

    QPoint cursorpos = mapFromGlobal(QCursor::pos());
    if (cursorpos.x() >= 170 || cursorpos.x() <= 770 || cursorpos.y() >= 40 || cursorpos.y() <= 640) {
        QPoint cursorcoord = Pix2Map(cursorpos);
        mousex = cursorcoord.y();
        mousey = cursorcoord.x();
    } else {
        mousex = mousey = -1;
    }
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            if (mousex == i && mousey == j) {
                brush.setColor(bri_colors[int(grid[i][j])]);
            } else {
                brush.setColor(colors[int(grid[i][j])]);
            }
            painter.setPen(Qt::NoPen);
            painter.setBrush(brush);
            QRectF currRect(j * Length, i * Length, Length, Length);
            painter.drawRect(currRect);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (status == Status::NotBegin) {
        QPoint pos = event->pos();
        if (pos.x() < 170 || pos.x() > 770 || pos.y() < 40 || pos.y() > 640) {
            return;
        }
        QPoint temp = Pix2Map(event->pos());
        if (grid[temp.y()][temp.x()] == Block::Nothing) {
            grid[temp.y()][temp.x()] = Block::Obstacle;
        } else if (grid[temp.y()][temp.x()] == Block::Obstacle) {
            grid[temp.y()][temp.x()] = Block::Nothing;
        }
    } repaint();
}

void MainWindow::keyPressEvent(QKeyEvent *event) { //WASD and Vim-Style HJKL supported:)
    switch (event->key()) {
        case Qt::Key_G:
            if (status == Status::NotBegin) {
                setGaming();
            } break;
        case Qt::Key_F:
            if (status == Status::Gaming) {
                setPaused();
            } else if (status == Status::Paused) {
                setGaming();
            } break;
        case Qt::Key_R:
            if (status == Status::Ended) {
                reset();
                //setGaming();
            } break;
        case Qt::Key_Q:
            if (status == Status::NotBegin || status == Status::Ended) {
                close();
            } break;
    }
    if (status != Status::Gaming) {
        return;
    }
    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_A:
        case Qt::Key_H:
            operations.push_back(Direction::Left);
            break;
        case Qt::Key_Up:
        case Qt::Key_W:
        case Qt::Key_K:
            operations.push_back(Direction::Up);
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
        case Qt::Key_L:
            operations.push_back(Direction::Right);
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
        case Qt::Key_J:
            operations.push_back(Direction::Down);
            break;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent*) {
    repaint();
}

QPoint MainWindow::getNewTail(QPoint tail) { //Search for new Tail Position
    int x_offset = dx[int(grid_b[tail.x()][tail.y()]) - 1];
    int y_offset = dy[int(grid_b[tail.x()][tail.y()]) - 1];
    return QPoint(tail.x() + x_offset, tail.y() + y_offset);
}

QPoint MainWindow::getNewHead(QPoint head, Direction direction) { //Generate new Head position
    int x_offset = dx[int(direction) - 1], y_offset = dy[int(direction) - 1];
    return QPoint(head.x() + x_offset, head.y() + y_offset);
}

void MainWindow::runSingleStep() {
    if (operations.empty() == false) {
        Direction neodirection = operations.dequeue();
        while ((neodirection == direction || int(neodirection) + int(direction) == 5) && operations.empty() == false) {
            neodirection = operations.dequeue();
        }
        if (neodirection != direction && int(neodirection) + int(direction) != 5) {
            direction = neodirection;
        }
    } //else there are no valid operations, just keep the current direction.
    tick_count  += 1;
    if (latency > 0) {
        latency -= 1;
    }
    QPoint neoHeadcoord = getNewHead(headcoord, direction);
    QPoint neoTailcoord = getNewTail(tailcoord);
    if (check(neoHeadcoord) == false) {
        setEnded();
        auto box = QMessageBox::critical(this, "Game Over!", "Game over!", QMessageBox::Ok);
        if (box == QMessageBox::Ok) {
            repaint();
            return;
        }
    }
    if (eat(neoHeadcoord)) {
        latency += 3;
    }
    if (latency == 0) {
        grid[tailcoord.x()][tailcoord.y()] = Block::Nothing;
        //grid_f[tailcoord.x()][tailcoord.y()] = Direction::Nothing;
        grid_b[tailcoord.x()][tailcoord.y()] = Direction::Nothing;
        tailcoord = neoTailcoord;
    }

    grid[headcoord.x()][headcoord.y()] = Block::Snake;
    grid_b[headcoord.x()][headcoord.y()] = direction;
    headcoord = neoHeadcoord;
    grid[headcoord.x()][headcoord.y()] = Block::Head;
    QString stepnum = QString("%1 step").arg(tick_count);
    if (tick_count != 1) {
        stepnum += "s";
    }
    ui->TimeLabel->setText(stepnum);
    QString scoretext = QString("%1 score").arg(score);
    if (score > 1) {
        scoretext += "s";
    }
    ui->scoreLabel->setText(scoretext);
}

void MainWindow::generateFruit() {
    int randval = QRandomGenerator::global()->bounded(0, 1601), cx, cy;
    do {
       cx = randval / 40, cy = randval % 40;
       randval = QRandomGenerator::global()->bounded(0, 1601);
    } while (grid[cx][cy] != Block::Nothing);
    grid[cx][cy] = Block::Fruit;
    repaint();
}

bool MainWindow::eat(QPoint head) {
    if (grid[head.x()][head.y()] == Block::Fruit) {
        score += ui->spinBox->value();
        generateFruit();
        return true;
    } else return false;
}

bool MainWindow::check(QPoint head) {
    if (head.x() < 0 || head.y() < 0 || head.x() >= Height || head.y() >= Width) {
        return false;
    }
    switch (special) {
        case Special::EatGlass:
            if (grid[head.x()][head.y()] == Block::Nothing ||
                grid[head.x()][head.y()] == Block::Fruit ||
                grid[head.x()][head.y()] == Block::Obstacle) {
                return true;
            } else return false;
            break;
        default:
            if (grid[head.x()][head.y()] == Block::Nothing ||
                grid[head.x()][head.y()] == Block::Fruit) {
                return true;
            } else return false;
    }
}

void MainWindow::reset() {
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            grid[i][j] = Block::Nothing;
            grid_b[i][j] = Direction::Nothing;
        }
    }
    grid[10][10] = Block::Snake;
    grid_b[10][10] = Direction::Right;
    grid[10][11] = Block::Head;
    tailcoord = QPoint(10, 10);
    headcoord = QPoint(10, 11);
    direction = Direction::Right;
    tick_count = 0;
    ui->TimeLabel->setText(QString("0 step"));
    score = 0;
    ui->scoreLabel->setText(QString("0 score"));
    setNotBegin();
    generateFruit();
}

void MainWindow::save() {
    if (savefile.exists() == false) {
        QString filename = QFileDialog::getOpenFileName(this, tr("Select Save File"), "*.json");
        savefile.setFileName(filename);
    }
    if (!savefile.open(QIODevice::WriteOnly)) {
        return;
    }
    QTextStream outstream(&savefile);
    QJsonObject obj;
    obj.insert("status", int(status));
    obj.insert("direction", int(direction));
    obj.insert("tick_count", tick_count);
    obj.insert("score", score);
    obj.insert("headcoord_x", headcoord.x());
    obj.insert("headcoord_y", headcoord.y());
    obj.insert("tailcoord_x", tailcoord.x());
    obj.insert("tailcoord_y", tailcoord.y());
    QString grid_str, grid_b_str;
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            grid_str += QString::number(int(grid[i][j]));
        } grid_str += "@";
    }
    obj.insert("grid", grid_str);
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            grid_b_str += QString::number(int(grid_b[i][j]));
        } grid_b_str += "@";
    }
    obj.insert("grid_b", grid_b_str);
    QJsonDocument doc = QJsonDocument(obj);
    QByteArray savedoc = doc.toJson();
    outstream << savedoc;
}

void MainWindow::load() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select Load File"), ".");
    loadfile.setFileName(filename);
    if (!loadfile.open(QIODevice::ReadOnly)) {
        qDebug() << "Load Failed";
        return;
    }
    QByteArray array = loadfile.readAll();
    QJsonParseError parseError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(array, &parseError));
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Parse Failed";
        return;
    }
    QJsonObject obj = jsonDoc.object();
    status = Status(obj.value("status").toInt());
    direction = Direction(obj.value("direction").toInt());
    tick_count = obj.value("tick_count").toInt();
    score = obj.value("score").toInt();
    headcoord.setX(obj.value("headcoord_x").toInt());
    headcoord.setY(obj.value("headcoord_y").toInt());
    tailcoord.setX(obj.value("tailcoord_x").toInt());
    tailcoord.setY(obj.value("tailcoord_y").toInt());
    QString grid_str = obj.value("grid").toString();
    QString grid_b_str = obj.value("grid_b").toString();
    QStringList grid_list = grid_str.split("@");
    QStringList grid_b_list = grid_b_str.split("@");
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            grid[i][j] = Block(grid_list[i][j].unicode() - '0');
            grid_b[i][j] = Direction(grid_b_list[i][j].unicode() - '0');
        }
    }
    setPaused();
}

void MainWindow::setNotBegin() {
    status = Status::NotBegin;
    ui->startButton->setDisabled(false);
    ui->pauseButton->setDisabled(true);
    ui->continueButton->setDisabled(true);
    ui->restartButton->setDisabled(true);
    ui->saveButton->setDisabled(true);
    ui->loadButton->setDisabled(false);
    ui->spinBox->setDisabled(false);
    setFocus(Qt::MouseFocusReason);
}

void MainWindow::setGaming() {
    QString code = ui->lineEdit->text();
    if (code == tr("EATGLASS")) {
        setWindowTitle(tr("I can eat glass. It doesn't hurt me."));
        special = Special::EatGlass;
    } else {
        setWindowTitle(tr("MySnake"));
        special = Special::Ordinary;
    }
    globalTimer->stop();
    globalTimer->start(speeds[ui->spinBox->value() - 1]);
    status = Status::Gaming;
    ui->startButton->setDisabled(true);
    ui->pauseButton->setDisabled(false);
    ui->continueButton->setDisabled(true);
    ui->restartButton->setDisabled(true);
    ui->saveButton->setDisabled(true);
    ui->loadButton->setDisabled(true);
    ui->spinBox->setDisabled(true);
    setFocus(Qt::MouseFocusReason);
    setFixedSize(width(), height());
}

void MainWindow::setPaused() {
    status = Status::Paused;
    //globalTimer->stop();
    ui->startButton->setDisabled(true);
    ui->pauseButton->setDisabled(true);
    ui->continueButton->setDisabled(false);
    ui->restartButton->setDisabled(false);
    ui->saveButton->setDisabled(false);
    ui->loadButton->setDisabled(true);
    ui->spinBox->setDisabled(true);
    setFocus(Qt::MouseFocusReason);
    setFixedSize(width(), height());
}

void MainWindow::setEnded() {
    status = Status::Ended;
    ui->startButton->setDisabled(true);
    ui->pauseButton->setDisabled(true);
    ui->continueButton->setDisabled(true);
    ui->restartButton->setDisabled(false);
    ui->saveButton->setDisabled(true);
    ui->loadButton->setDisabled(true);
    ui->spinBox->setDisabled(true);
    setFocus(Qt::MouseFocusReason);
    setFixedSize(width(), height());
}

void MainWindow::on_actionDon_t_touch_triggered() {
    QMessageBox::critical(this, "Stop Dreaming!", "Only hard work brings good score.", QMessageBox::Ok);
}

void MainWindow::on_actionnew_npy_triggered() {
    QMessageBox::critical(this, "Stop Dreaming!", "You can't. You just can't :(", QMessageBox::Ok);
}

void MainWindow::on_actionbecome_rich_triggered() {
    QMessageBox::critical(this, "Stop Dreaming!", "No way!", QMessageBox::Ok);
}

void MainWindow::on_actionDon_t_touch_2_triggered() {
    ui->touchButton_1->setVisible(true);
    ui->touchButton_1->setEnabled(true);
}

void MainWindow::on_touchButton_1_clicked() {
    ui->touchButton_2->setVisible(true);
    ui->touchButton_2->setEnabled(true);
}

void MainWindow::on_touchButton_2_clicked() {
    ui->touchButton_3->setVisible(true);
    ui->touchButton_3->setEnabled(true);
}

void MainWindow::on_touchButton_3_clicked() {
    ui->label_2->setVisible(true);
    ui->lineEdit->setVisible(true);
    ui->lineEdit->setEnabled(true);
    ui->touchButton_1->setVisible(false);
    ui->touchButton_2->setVisible(false);
    ui->touchButton_3->setVisible(false);
}
