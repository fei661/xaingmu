#ifndef ROLLPIX_H
#define ROLLPIX_H

#include <QWidget>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QPainter>
#include <QDir>

class rollpix : public QWidget
{
    Q_OBJECT
  public:
    explicit rollpix(QWidget *parent = nullptr);
    virtual void paintEvent(QPaintEvent *event); //绘制事件
    virtual void timerEvent(QTimerEvent *event); //定时事件
    void setBackgroundDir(const QString &path);  //读取背景目录
  signals:
  private:
    QPixmap pix;           //图像类对象
    QStringList filepaths; //保存目录中的路径
    QDir dir;              //实例化目录
};

#endif // ROLLPIX_H
