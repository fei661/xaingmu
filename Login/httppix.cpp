#include "httppix.h"
#include "ui_httppix.h"

HTTPpix::HTTPpix(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HTTPpix)
{
    ui->setupUi(this);
    //实例化对象
    manager = new QNetworkAccessManager(this);
    //绑定finished信号与自定义槽函数，网络请求结束后自动触发槽函数
    connect(manager, &QNetworkAccessManager::finished, this, &HTTPpix::Reply);
}

HTTPpix::~HTTPpix() { delete ui; }
//提供网址访问
void HTTPpix::on_pushButton_clicked() { Request(ui->lineEdit->text()); }
//网络请求
void HTTPpix::Request(QString http)
{
    //建立访问资源
    QUrl url(http);
    //建立网络请求
    QNetworkRequest request(url);
    //通过get发送QNetworkRequest网络请求
    manager->get(request);
}
//网络应答
void HTTPpix::Reply(QNetworkReply *reply)
{
    //读取应答结果
    QByteArray array = reply->readAll();
    //解析数据
    show_pix(array);
}
//显示网页的图片
void HTTPpix::show_pix(QByteArray &array)
{
    //实例化类对象
    QPixmap pix;
    //将网络字节像素导入到pix对象
    pix.loadFromData(array);
    //设置尺寸大小
    pix = pix.scaled(ui->label->size());
    //显示图像
    ui->label->setPixmap(pix);
}
