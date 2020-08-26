#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , special(Special::Ordinary)
    , status(Status::NotBegin)
    , direction(Direction::Right)
    , snake1(new Snake())
    , canvasColor(QColor("#ECE5DB"))
    , obstacleColor(QColor("#E08863"))
    , fruitColor(QColor("#DBBE3F"))
    , canvasColorBright(QColor("#F6EEE4"))
    , obstacleColorBright(QColor("#F0916A"))
    , fruitColorBright(QColor("#EDCE44"))
    , fruit(new Fruit())
    , tickCount(0)
    , globalTimer(new QTimer(this)) {
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle(tr("MySnake"));
    setMinimumSize(size());

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

    setNotBegin();
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
    delete fruit;
    delete snake1;
    delete ui;
}

Point MainWindow::Pix2Map(QPoint p) {
    int px = p.x(), py = p.y();
    int cx = (px - leftpos) / Length;
    int cy = (py - toppos) / Length;
    return qMakePair(cy, cx);
}

QPoint MainWindow::Map2Pix(Point c) {
    int cx = c.first, cy = c.second;
    int px = cx * Length + leftpos;
    int py = cy * Length + toppos;
    return QPoint(px, py);
}

void MainWindow::paintEvent(QPaintEvent*) {
    Width = (geometry().width() - leftpos - Margin) / Length;
    Height = (geometry().height() - toppos - Margin) / Length;

    QPainter painter(this);
    QBrush brush(QColor("#ECE5DB"));

    painter.translate(leftpos, toppos);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawRect(0, 0, Length * Width, Length * Height);

    for (auto it = snake1->expose().rbegin(); it != snake1->expose().rend(); it++) {
        int hori = it->second, vert = it->first;
        QBrush snakebrush("#C0C0C0");
        if (it == snake1->expose().rbegin()) {
            snakebrush.setColor(snake1->getHeadColor());
        } else {
            snakebrush.setColor(snake1->getBodyColor());
        }
        painter.setBrush(snakebrush);
        painter.drawRect(hori * Length, vert * Length, Length, Length);
    }

    for (const auto& obstacle : obstacles) {
        int hori = obstacle.getPos().second, vert = obstacle.getPos().first;
        QBrush obstaclebrush(obstacleColor);
        painter.setBrush(obstaclebrush);
        painter.drawRect(hori * Length, vert * Length, Length, Length);
    }
    {
        int hori = fruit->getPos().second, vert = fruit->getPos().first;
        QBrush fruitbrush(fruitColor);
        painter.setBrush(fruitbrush);
        painter.drawRect(hori * Length, vert * Length, Length, Length);
    }
    QPoint cursorPos = mapFromGlobal(QCursor::pos());
    if (inTheMap(cursorPos)) {
        Point cursorCoord = Pix2Map(cursorPos);
        QBrush cursorBrush(QColor("#000000"));
        //qDebug() << int(getCoordType(cursorCoord));
        switch (getCoordType(cursorCoord)) {
            case (Block::Nothing):
                cursorBrush.setColor(QColor("#F6EEE4"));
                break;
            case (Block::Snake):
                cursorBrush.setColor(snake1->getBodyColorBright());
                break;
            case (Block::Head):
                cursorBrush.setColor(snake1->getHeadColorBright());
                break;
            case (Block::Obstacle):
                cursorBrush.setColor(QColor("#F0916A"));
                break;
            case (Block::Fruit):
                cursorBrush.setColor(fruitColorBright);
                break;
        }
        painter.setBrush(cursorBrush);
        int hori = cursorCoord.second, vert = cursorCoord.first;
        painter.drawRect(hori * Length, vert * Length, Length, Length);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (status == Status::NotBegin) {
        QPoint pos = event->pos();
        if (inTheMap(pos) == false) {
            return;
        }
        Point temp = Pix2Map(event->pos());
        temp = qMakePair(temp.first, temp.second);
        if (getCoordType(temp) == Block::Nothing) {
            obstacles.insert(Obstacle(temp));
        } else if (getCoordType(temp) == Block::Obstacle) {
            obstacles.remove(Obstacle(temp));
        } repaint();
    }
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

void MainWindow::runSingleStep() {  
    bool everyThingFine = true;
    if (operations.empty() == false) {
        Direction neodirection = operations.dequeue();
        while (!snake1->setDirection(neodirection) && !operations.empty()) {
            neodirection = operations.dequeue();
        }
    } //否则就没有合法操作了,保持当前方向即可.
    Point newHead = snake1->getNewHead();
    if (checkOverFlow(newHead)) { //撞墙了
        QMessageBox::critical(this, "Game over!", "Game over!", QMessageBox::Ok);
        everyThingFine = false;
        setEnded();
    }
    if (getCoordType(newHead) == Block::Obstacle) { //撞障碍物了
        switch (special) {
            case Special::Ordinary:
            case Special::Transparent:
                QMessageBox::critical(this, "Game over!", "Game over!", QMessageBox::Ok);
                setEnded();
                everyThingFine = false;
                break;
            case Special::EatGlass:
                obstacles.remove(Obstacle(newHead));
                break;
        }
    }
    if (getCoordType(newHead) == Block::Snake) { //撞到身体了
        switch (special) {
            case Special::Ordinary:
            case Special::EatGlass:
                QMessageBox::critical(this, "Game over!", "Game over!", QMessageBox::Ok);
                setEnded();
                everyThingFine = false;
                break;
            //case Special::Transparent:
            //  do nothing
        }
    }
    if (getCoordType(newHead) == Block::Fruit) { //吃到水果了
        snake1->eat(); //长度+3
        generateNewFruit(newHead);
    }
    if (everyThingFine) {
        snake1->setNewHead(newHead);
        snake1->setNewTail();
    }
    tickCount += 1;
    QString stepNum = QString("%1 step").arg(tickCount);
    if (tickCount > 1) {
        stepNum += "s";
    }
    ui->TimeLabel->setText(stepNum);
    QString scoreText = QString("%1 score").arg(snake1->getScore());
    if (snake1->getScore() > 1) {
        scoreText += "s";
    }
    ui->scoreLabel->setText(scoreText);
}

void MainWindow::reset() {
    snake1 = new Snake();
    generateNewFruit(qMakePair(-1, 1));
    tickCount = 0;
    ui->TimeLabel->setText(QString("0 step"));
    ui->scoreLabel->setText(QString("0 score"));
    setNotBegin();
}

QJsonObject MainWindow::obstaclesToJson() const {
    QString obstacleXString, obstacleYString;
    for (const auto& each : obstacles) {
        obstacleXString += QString::number(each.getPos().first);
        obstacleYString += QString::number(each.getPos().second);
        if (each != *obstacles.rbegin()) {
            obstacleXString += ",";
            obstacleYString += ",";
        }
    }
    QJsonObject obj;
    obj.insert("posX", obstacleXString);
    obj.insert("posY", obstacleYString);
    return obj;
}

void MainWindow::save() {
    if (saveFile.exists() == false) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select Save File"), "$HOME/Documents", "*.json");
        saveFile.setFileName(fileName);
    }
    if (!saveFile.open(QIODevice::WriteOnly)) {
        return;
    }
    QTextStream outstream(&saveFile);
    QJsonObject obj;
    obj.insert("windowWidth", width());
    obj.insert("windowHeight", height());
    obj.insert("Width", Width);
    obj.insert("Height", Height);
    obj.insert("snake1", snake1->toJson());
    obj.insert("obstacles", obstaclesToJson());
    obj.insert("fruit", fruit->toJson());
    obj.insert("tickCount", tickCount);
    QJsonDocument doc = QJsonDocument(obj);
    QByteArray savedoc = doc.toJson();
    outstream << savedoc;
}

void MainWindow::jsonToObstacles(QJsonObject obj) {
    obstacles.clear();
    QString posXString = obj.value("posX").toString();
    QString posYString = obj.value("posY").toString();
    if (posXString == tr("")) {
        return;
    }
    QStringList posXList = posXString.split(",");
    QStringList posYList = posYString.split(",");
    for (int i = 0; i < posXList.size(); i++) {
        obstacles.insert(Obstacle(qMakePair(posXList[i].toInt(), posYList[i].toInt())));
    }
}

void MainWindow::load() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Load File"), "$HOME/Documents", "*.json");
    loadFile.setFileName(fileName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Load Failed";
        return;
    }
    QByteArray array = loadFile.readAll();
    QJsonParseError parseError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(array, &parseError));
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Parse Failed";
        return;
    }
    QJsonObject obj = jsonDoc.object();
    tickCount = obj.value("tickCount").toInt();
    Height = obj.value("Height").toInt();
    Width = obj.value("Width").toInt();
    setFixedSize(obj.value("windowWidth").toInt(), obj.value("windowHeight").toInt());
    delete snake1;
    delete fruit;
    snake1 = new Snake(obj.value("snake1").toObject());
    fruit = new Fruit(obj.value("fruit").toObject());
    jsonToObstacles(obj.value("obstacles").toObject());
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
    } else if (code == tr("TRANSPARENT")){
        setWindowTitle(tr("Hey! I feel a bit weird!"));
        special = Special::Transparent;
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
    ui->startButton->setDisabled(true);
    ui->pauseButton->setDisabled(true);
    ui->continueButton->setDisabled(false);
    ui->restartButton->setDisabled(false);
    ui->saveButton->setDisabled(false);
    ui->loadButton->setDisabled(true);
    ui->spinBox->setDisabled(true);
    setFocus(Qt::MouseFocusReason);
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
    setMinimumSize(width(), height());
    setMaximumSize(114514, 114514);
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

bool MainWindow::inTheMap(QPoint _point) {
    int px = _point.x(), py = _point.y();
    return (px >= leftpos && px <= leftpos + Width * Length && py >= toppos && py <= toppos + Height * Length);
}

bool MainWindow::checkOverFlow(Point pos) {
    return (pos.first < 0 || pos.second < 0 || pos.first >= Height || pos.second >= Width);
}

void MainWindow::generateNewFruit(Point newHead) {
    fruit->setNewFruit(qMakePair(0, 0));
    Point tryFruit = fruit->generateNewFruit(Height, Width);
    while (getCoordType(tryFruit) != Block::Nothing || tryFruit == newHead) {
        tryFruit = fruit->generateNewFruit(Height, Width);
    } fruit->setNewFruit(tryFruit); //生成新水果
}

Assist::Block MainWindow::getCoordType(Point pos) {
    if (snake1->validPos(pos) == false) {
        if (pos == *snake1->expose().rbegin()) {
            return Block::Head;
        } else {
            return Block::Snake;
        }
    }
    if (fruit->validPos(pos) == false) {
        return Block::Fruit;
    }
    if (obstacles.contains(Obstacle(pos))) {
        return Block::Obstacle;
    }
    return Block::Nothing;
}
