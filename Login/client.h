#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>  //TCP套接字
#include <QMessageBox> //消息盒子
#include <QDateTime>
#include <QNetworkAddressEntry> //获取网络地址信息
#include <QNetworkInterface>    //网络信息
#include <QHostAddress>         //网络地址

QT_BEGIN_NAMESPACE
namespace Ui
{
class Client;
}
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT         //开启信号槽功能

//控制client窗口的创建和销毁
  public:
    Client(QWidget *parent = nullptr);
    ~Client();

//获取本机地址
    QString GetHostAddress();  //Qstring是Qt专门处理字符串的类，代替c语言中的char*,c++标准std：：string
  private slots:   //槽函数
    void on_btnconnect_clicked(); //连接

    void on_btnsend_clicked();    //发送

    void on_btnclear_clicked();   //清空

  private:
    Ui::Client *ui;
    //创建QTcpSocket客户端套接字
    QTcpSocket *msocket;  //套接字指针
};
#endif // CLIENT_H
