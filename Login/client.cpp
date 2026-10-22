#include "client.h"
#include "ui_client.h"
//1. 先初始化父类 `QMainWindow(parent)`
//2. 再初始化成员 `ui`
//3. 最后才进入大括号 `{}` 里面的代码（`ui->setupUi(this);`、创建 msocket 等）
Client::Client(QWidget *parent)
    : QMainWindow(parent)    //:初始化列表的开始
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    //初始化    把 ui 文件上拖的按钮、输入框、textBrowser 全部创建出来，挂载到 this（当前 Client 窗口）上
    msocket = new QTcpSocket(this);  //把当前 Client 窗口设为 msocket 的父对象
    //若需要读取服务端的消息，则通过套接字读取readyRead()绑定槽函数
    connect(msocket, &QTcpSocket::readyRead, [this] {   //connect是Qt信号槽绑定函数    第三个槽函数Lambda 匿名槽函数
        ui->textBrowser->setAlignment(Qt::AlignLeft); //左对齐,展示服务器发来的消息
        //读取客户端发送消息内容
        QByteArray msg = msocket->readAll();
        //显示客户端消息
        ui->textBrowser->append(msg);
    });
}

Client::~Client() { delete ui; }
//链接服务端 准备通信
void Client::on_btnconnect_clicked()
{
    //获取连接的ip地址和端口号
    QString ip = ui->lineIP->text();
    QString port = ui->linePort->text();
    //建立客户端与服务端之间的连接，使用成员函数connectToHost()
    msocket->connectToHost(ip, port.toUShort());
    //通过connected()信号判断套接字链接成功
    connect(msocket, &QTcpSocket::connected,
            [this] { QMessageBox::warning(this, "连接服务端提示", "连接服务端成功"); });
}
//获取本机IP地址
QString Client::GetHostAddress()
{
    //获取本机所有网络地址
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    //遍历所有信息接口
    foreach (QNetworkInterface interface, interfaces)
    {
        //检查接口是否处于活跃状态和运行状态以及不为回环状态
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            //解析ip地址身份
            QList<QNetworkAddressEntry> entrys = interface.addressEntries();
            //遍历ip地址对应信息
            foreach (QNetworkAddressEntry entry, entrys)
            {
                //获取可以操作ipv4地址
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                {
                    return entry.ip().toString();
                }
            }
        }
    }
}
//发送消息
void Client::on_btnsend_clicked()
{
    //拼接消息内容 时间+地址+内容
    QString dt = QDateTime::currentDateTime().toString("yyyy/MM/dd dddd hh:mm:ss");
    QString msg = ui->textEdit->toPlainText();
    QString data = QString("%1\n\t[%2]:%3").arg(dt).arg(GetHostAddress()).arg(msg);
    //发送给客户端
    msocket->write(data.toUtf8());
    //将data消息显示在texttextBrowser右侧
    ui->textBrowser->setAlignment(Qt::AlignRight); //右对齐
    ui->textBrowser->append(data);

    ui->textEdit->clear();
}
//清空
void Client::on_btnclear_clicked() { ui->textBrowser->clear(); }
