#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDateTime>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QVector>
#include <QTimer>
#include "weather.h"
#include "datetime.h"
#include "server.h"
#include "client.h"
#include "login.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr, QString username = "");
    ~MainWindow();

    void loadFriends();
    void addFriendToList(QString friendUsername, QString friendName, QString friendIco);

  private slots:
    void on_btnAddFriend_clicked();
    void on_btnDeleteFriend_clicked();
    void on_btnSearch_clicked();
    void on_lineSearch_textChanged(const QString &arg1);
    void on_listWidget_friends_itemDoubleClicked(QListWidgetItem *item);
    void refreshFriendStatus();

    void on_btn_msg_clicked();
    void on_btn_contacts_clicked();
    void on_btn_groups_clicked();
    void on_btn_httppix_clicked();
    void on_btn_settings_clicked();

  public:
    void registerSocket(QString friendIp, QTcpSocket *socket);
    void openChatWindow(QString friendUsername, QString friendName, QString friendIp, QString friendIco);

  private slots:
    void newClientConnected();
    void clientDisconnected();
    void readClientData();
    void connectToFriend(QString friendIp);
    void connectToAllFriends();
    void updateFriendOnlineStatus(QString friendIp, bool online);

  protected:
    void closeEvent(QCloseEvent *event) override;

  private:
    Ui::MainWindow *ui;
    QString currentUsername;
    QString currentUserIco;
    QString currentUserName;
    QTcpServer *tcpServer;
    QVector<QTcpSocket *> clientSockets;
    QMap<QString, QTcpSocket *> ipToSocketMap;
    QSet<QString> pendingConnections;
};

#endif // MAINWINDOW_H
