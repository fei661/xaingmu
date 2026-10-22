#ifndef ROLLTEXT_H
#define ROLLTEXT_H

#include <QWidget>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QPainter>

class rolltext : public QWidget
{
    Q_OBJECT
  public:
    explicit rolltext(QWidget *parent = nullptr);
    virtual void paintEvent(QPaintEvent *event); //绘制事件
    virtual void timerEvent(QTimerEvent *event); //定时事件
    void setText(const QString &value);          //设置文本内容
  signals:
  private:
    QString text; //显示文本
    int x;        //显示位置
};

#endif // ROLLTEXT_H
