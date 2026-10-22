#ifndef DATETIME_H
#define DATETIME_H

#include <QMainWindow>
#include <QTimer>    //定时器
#include <QDateTime> //日期时间类

QT_BEGIN_NAMESPACE
namespace Ui
{
class DateTime;
}
QT_END_NAMESPACE

class DateTime : public QMainWindow
{
    Q_OBJECT

  public:
    DateTime(QWidget *parent = nullptr);
    ~DateTime();

  private slots:
    void on_btnstart_clicked();

    void on_btnstop_clicked();

    void showDT();

  private:
    Ui::DateTime *ui;
    QTimer *mtimer; //实例化定时器
};
#endif // DATETIME_H
