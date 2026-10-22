#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>  //TCP服务端
#include <QTcpSocket>  //TCP套接字
#include <QMessageBox> //消息盒子
#include <QVector>     //动态数组
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QPixmap>
#include <QMenu>
#include <QBuffer>
#include <QFileDialog>
#include <QFile>
#include <QListWidgetItem>
#include "emoji_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class Server;
}
QT_END_NAMESPACE

class Server : public QMainWindow
{
    Q_OBJECT

  public:
    Server(QWidget *parent = nullptr);
    Server(QWidget *parent, QString friendName, QString friendIp, QString friendIco);
    Server(QWidget *parent, QString currentUsername);
    ~Server();

    void save_client(QTcpSocket *msocket);
    void setSocket(QTcpSocket *socket);
    void appendMessage(QString msg);
    void loadOnlineFriends(QString currentUsername);
  private slots:
    void new_client();

    void dis_client();

    void on_btnsend_clicked();

    void on_listWidget_itemDoubleClicked(int row);

    void on_btnclear_clicked();

    void on_label_clicked();
    void on_label_17_clicked();
    void insertEmoji(QString emojiPath);
    void sendFile(QString filePath);
    bool eventFilter(QObject *obj, QEvent *event);

  private:
    Ui::Server *ui;
    QTcpServer *mserver;
    QVector<QTcpSocket *> sockets;
    QString friendIp;
    QString friendName;
    QString currentUsername;
    QStringList pendingMessages;

public:
    static QVector<Server *> chatWindows;
    QString getFriendIp() const { return friendIp; }
    void receiveMessage(QString friendName, QString msgStr);
    bool hasSocket(QTcpSocket *socket) const { return sockets.contains(socket); }
    void addSocket(QTcpSocket *socket);
};
#endif // SERVER_H
