# Qt大作业：贪吃蛇

计97 张楚炎 2019013271

##　编译和运行环境

+ 操作系统：Ubuntu 18.04.5 LTS
+ Qt版本：5.12.9
+ 编译器版本：GCC 7.5.0

## 游戏介绍

基于Qt实现的贪吃蛇游戏。

允许使用上下左右、WASD或者HJKL控制蛇的方向。

地图为米色，障碍物为橙红色，蛇头为灰色，蛇身为银色，食物为黄色。

鼠标移动到地图上时，对应的方格会高亮。

游戏未开始时，点击地图上的空白处可以设置障碍，再次点击障碍可以将其消除。

游戏未开始时，可以调节蛇运行的速度，不同速度下吃掉食物获得的分数不同。

除菜单栏、工具栏和按钮外，游戏还提供了快捷键：

+ 空格键切换游戏状态与暂停状态；如果游戏还未开始，可以用空格键开始游戏
+ R键在游戏结束后复位，回到初始状态。
+ Q键在游戏结束后退出。
+ Ctrl+S保存
+ Ctrl+L加载
+ Ctrl+C继续
+ Ctrl+P暂停

游戏有内置彩蛋。

## 界面展示

<img src="/home/chriszhang/.config/Typora/typora-user-images/image-20200827111017142.png" alt="image-20200827111017142" style="zoom:67%;" />

<img src="/home/chriszhang/.config/Typora/typora-user-images/image-20200827112735659.png" alt="image-20200827112735659" style="zoom:67%;" />

## 设计思路

工程用到的文件如下：

![image-20200827112808124](/home/chriszhang/.config/Typora/typora-user-images/image-20200827112808124.png)

`base.h`中实现了若干枚举类型，方便其他文件使用：

+ `Status`类用于游戏状态的指示

+ `Block`类用于标示某个位置方格的类型
+ `Direction`类用于指示方向
+ `Special`类用于彩蛋的处理

`snake.cpp`和`snake.h`表示蛇，内部用`QQueue`存储

`obstacle.cpp`和`obstacle.h`表示障碍物

`fruit.cpp`和`fruit.h`表示果实

`mainwindow.cpp`和`mainwindow.h`是主窗口类

`icons`文件夹中存放了工具栏中按钮的图标

`MainWindow`类负责控制和显示，具体的步进由`Snake`类、`Obstacle`类和`Fruit`类实现。

绘制直接在`MainWindow`上进行，`MainWindow`中由成员`globalTimer`设定计时间隔（取决于`spinBox`中的速度值），最快每秒钟20次。计时器发出信号时，如果检测到游戏状态是`Status::Gaming`则调用`runSingleStep()`让蛇前进一步，否则不动；同时调用一次`repaint()`重新绘制。

在`runSingleStep()`函数中，程序首先会尝试操作队列里取出第一个合法操作（与当前方向同向和反向的操作都被认为是非法的），如果能成功取出则更新蛇头的方向；然后从蛇处获取它下一步的蛇头坐标，然后检查是否撞墙、撞障碍物或者撞到自己身体，如果是则直接游戏结束；再检查是否吃到果子，如果是则蛇变长、更新分值、生成新的食物；最后更新蛇头蛇尾的位置。

`MainWindow`中重载了`paintEvent`（用于绘制）、`mousePressEvent`（用于设置和消除障碍物）和`keyPressEvent`（用于快捷键和蛇头方向的控制）。为了保证键盘事件的有效性，窗口焦点将一直被设置在`MainWindow`上。

## 未来可能的改进方向

单独设置一个`Contorller`类，将控制部分从`MainWindow`类中去除，实现MVC分离。

增加第二只蛇和双人对战模式。

添加移动、吃食物和死亡时的音效。