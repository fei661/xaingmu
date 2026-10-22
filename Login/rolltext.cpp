#include "rolltext.h"
//构造函数
rolltext::rolltext(QWidget *parent)
    : QWidget(parent)
{
    //获取显示起点位置
    this->x = this->width(); //获取窗口载体宽度
    startTimer(50);          //启动定时器
}
//重绘事件
void rolltext::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);                                    //实例化绘制对象
    painter.drawText(this->x, this->height() / 2, this->text); //指定位置重绘文本
}
//定时器事件
void rolltext::timerEvent(QTimerEvent *event)
{
    if (x < -20)
    {
        this->x = this->width(); //重置位置
    }
    x--;      //向左更新位置
    update(); //更新一次位置
}
//设置需要显示内容
void rolltext::setText(const QString &value) { this->text = value; }
