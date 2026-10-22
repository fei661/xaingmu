#include "datetime.h"
#include "ui_datetime.h"

DateTime::DateTime(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DateTime)
    , mtimer(new QTimer)
{
    ui->setupUi(this);
    //绑定timeout信号和定时器
    connect(mtimer, &QTimer::timeout, this, &DateTime::showDT);
    //启动定时器
    mtimer->start(1000);
}

DateTime::~DateTime() { delete ui; }
//自定义槽函数 完成日期和时间获取
void DateTime::showDT()
{
    QString d = QDate::currentDate().toString("yyyy-MM-dd");
    QString t = QTime::currentTime().toString("hh:mm:ss");
    QString dateFull = QDate::currentDate().toString("yyyy年MM月dd日");
    QString week = QDate::currentDate().toString("dddd");
    QString dayOfYear = QString("今天是第 %1 天").arg(QDate::currentDate().dayOfYear());
    QString leapYear = QDate::currentDate().isLeapYear(QDate::currentDate().year()) ? "闰年" : "平年";
    QString quarter = QString("第 %1 季度").arg((QDate::currentDate().month() + 2) / 3);

    ui->lcdDate->display(d);
    ui->lcdTime->display(t);
    ui->label_date->setText(dateFull);
    ui->label_week->setText(week);
    ui->label_other->setText(dayOfYear + " | " + leapYear + " | " + quarter);
}
//启动定时器
void DateTime::on_btnstart_clicked() { mtimer->start(1000); }
//关闭定时器
void DateTime::on_btnstop_clicked() { mtimer->stop(); }
