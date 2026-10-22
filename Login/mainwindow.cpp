#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "httppix.h"

MainWindow::MainWindow(QWidget *parent, QString username)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentUsername(username)
{
    ui->setupUi(this);

    QSqlQuery query;
    QString sql = QString("select name, ico from user where username='%1';").arg(username);
    if (query.exec(sql) && query.next())
    {
        currentUserName = query.value(0).toString();
        currentUserIco = query.value(1).toString();
        ui->label_username->setText(currentUserName);
        QString styles = QString("border-image: url(:/res/ico/%1);").arg(currentUserIco);
        ui->label_avatar->setStyleSheet(styles);
    }

    loadFriends();

    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::newClientConnected);

    QString localIp = "127.0.0.1";

    quint16 basePort = 8888;
    for (int i = 0; i < 100; ++i)
    {
        quint16 port = basePort + i;
        if (tcpServer->listen(QHostAddress::Any, port))
        {
            qDebug() << "TCP Server started on port" << port;

            QSqlQuery query;
            QString updateSql = QString("update user set ip='%1:%2' where username='%3';")
                                    .arg(localIp)
                                    .arg(port)
                                    .arg(currentUsername);
            query.exec(updateSql);

            connectToAllFriends();

            QTimer *timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, &MainWindow::refreshFriendStatus);
            timer->start(3000);
            break;
        }
    }

    if (!tcpServer->isListening())
    {
        qDebug() << "TCP Server start failed:" << tcpServer->errorString();
        QMessageBox::warning(this, "服务启动失败", "无法启动服务，请检查网络设置");
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSqlQuery query;
    QString updateSql = QString("update user set ip='' where username='%1';").arg(currentUsername);
    query.exec(updateSql);

    for (QTcpSocket *socket : clientSockets)
    {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    clientSockets.clear();
    ipToSocketMap.clear();
    pendingConnections.clear();

    if (tcpServer)
    {
        tcpServer->close();
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::loadFriends()
{
    ui->listWidget_friends->clear();

    QSqlQuery query;
    QString sql = QString("select f.friendusername, u.name, u.ico from friend f "
                          "join user u on f.friendusername = u.username "
                          "where f.username='%1';")
                      .arg(currentUsername);
    if (query.exec(sql))
    {
        while (query.next())
        {
            QString friendUsername = query.value(0).toString();
            QString friendName = query.value(1).toString();
            QString friendIco = query.value(2).toString();
            addFriendToList(friendUsername, friendName, friendIco);
        }
    }
}

void MainWindow::addFriendToList(QString friendUsername, QString friendName, QString friendIco)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(friendName + " [离线]");
    item->setForeground(QColor(128, 128, 128));
    item->setData(Qt::UserRole, friendUsername);

    QString iconPath;
    if (friendIco == "15.png" || friendIco == "16.png")
        iconPath = QString(":/res/%1").arg(friendIco);
    else
        iconPath = QString(":/res/ico/%1").arg(friendIco);
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull())
    {
        item->setIcon(QIcon(pixmap.scaled(48, 48, Qt::KeepAspectRatio)));
    }

    ui->listWidget_friends->addItem(item);
}

void MainWindow::on_btnAddFriend_clicked()
{
    QString searchInput = ui->lineSearch->text().trimmed();
    if (searchInput.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请输入要添加的好友昵称或IP地址");
        return;
    }

    QSqlQuery query;
    QString findUserSql = QString("select * from user where username='%1' or name='%2';")
                              .arg(searchInput)
                              .arg(searchInput);
    if (!query.exec(findUserSql) || !query.next())
    {
        QMessageBox::warning(this, "提示", "该用户不存在");
        return;
    }

    QString friendUsername = query.value(0).toString();
    QString friendName = query.value(1).toString();
    QString friendIco = query.value(2).toString();
    QString friendIp = query.value(4).toString();

    if (friendUsername == currentUsername)
    {
        QMessageBox::warning(this, "提示", "不能添加自己为好友");
        return;
    }

    QString checkFriendSql = QString("select * from friend where username='%1' and friendusername='%2';")
                                 .arg(currentUsername)
                                 .arg(friendUsername);
    if (query.exec(checkFriendSql) && query.next())
    {
        QMessageBox::warning(this, "提示", "该用户已经是你的好友");
        return;
    }

    QString insertSql = QString("insert into friend values('%1', '%2', '%3');")
                            .arg(currentUsername)
                            .arg(friendUsername)
                            .arg(friendIp);
    if (query.exec(insertSql))
    {
        addFriendToList(friendUsername, friendName, friendIco);
        QMessageBox::information(this, "提示", "添加好友成功");
        ui->lineSearch->clear();
    }
    else
    {
        QMessageBox::warning(this, "提示", "添加好友失败: " + query.lastError().text());
    }
}

void MainWindow::on_btnDeleteFriend_clicked()
{
    QListWidgetItem *selectedItem = ui->listWidget_friends->currentItem();
    if (!selectedItem)
    {
        QMessageBox::warning(this, "提示", "请选择要删除的好友");
        return;
    }

    QString friendUsername = selectedItem->data(Qt::UserRole).toString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", "确定要删除好友 " + selectedItem->text() + " 吗？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QSqlQuery query;
        QString deleteSql = QString("delete from friend where username='%1' and friendusername='%2';")
                                .arg(currentUsername)
                                .arg(friendUsername);
        if (query.exec(deleteSql))
        {
            delete selectedItem;
            QMessageBox::information(this, "提示", "删除好友成功");
        }
        else
        {
            QMessageBox::warning(this, "提示", "删除好友失败");
        }
    }
}

void MainWindow::on_btnSearch_clicked()
{
    QString keyword = ui->lineSearch->text().trimmed();
    if (keyword.isEmpty())
    {
        loadFriends();
        return;
    }

    ui->listWidget_friends->clear();

    QSqlQuery query;
    QString sql = QString("select f.friendusername, u.name, u.ico from friend f "
                          "join user u on f.friendusername = u.username "
                          "where f.username='%1' and u.name like '%%2%';")
                      .arg(currentUsername)
                      .arg(keyword);
    if (query.exec(sql))
    {
        while (query.next())
        {
            QString friendUsername = query.value(0).toString();
            QString friendName = query.value(1).toString();
            QString friendIco = query.value(2).toString();
            addFriendToList(friendUsername, friendName, friendIco);
        }
    }
}

void MainWindow::on_lineSearch_textChanged(const QString &arg1)
{
    if (arg1.isEmpty())
    {
        loadFriends();
    }
}
void MainWindow::on_btn_msg_clicked()
{
    Weather *weather = new Weather();
    weather->show();
}

void MainWindow::on_btn_contacts_clicked()
{
    DateTime *dateTime = new DateTime();
    dateTime->show();
}

void MainWindow::on_btn_groups_clicked()
{
    Server *server = new Server(this, currentUsername);
    server->show();
}

void MainWindow::on_btn_httppix_clicked()
{
    HTTPpix *pix = new HTTPpix();
    pix->show();
}

void MainWindow::on_btn_settings_clicked()
{
    Login *login = new Login();
    login->show();
    this->close();
}

void MainWindow::on_listWidget_friends_itemDoubleClicked(QListWidgetItem *item)
{
    QString friendUsername = item->data(Qt::UserRole).toString();

    QSqlQuery query;
    QString sql = QString("select name, ip, ico from user where username='%1';").arg(friendUsername);
    if (query.exec(sql) && query.next())
    {
        QString friendName = query.value(0).toString();
        QString friendIp = query.value(1).toString();
        QString friendIco = query.value(2).toString();

        Server *chatWindow = new Server(this, friendName, friendIp, friendIco);

        if (!friendIp.isEmpty() && friendIp.contains(":"))
        {
            if (ipToSocketMap.contains(friendIp) && ipToSocketMap[friendIp]->state() == QAbstractSocket::ConnectedState)
            {
                chatWindow->setSocket(ipToSocketMap[friendIp]);
                connect(ipToSocketMap[friendIp], &QTcpSocket::readyRead, this, &MainWindow::readClientData);
            }
            else
            {
                QString ip = friendIp;
                quint16 port = 8888;
                if (friendIp.contains(":"))
                {
                    QStringList parts = friendIp.split(":");
                    ip = parts[0];
                    port = parts[1].toUShort();
                }

                QTcpSocket *socket = new QTcpSocket();
                connect(socket, &QTcpSocket::connected, [this, socket, friendIp, chatWindow]() {
                    ipToSocketMap[friendIp] = socket;
                    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnected);
                    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readClientData);
                    chatWindow->addSocket(socket);
                });
                socket->connectToHost(ip, port);
            }
        }
        chatWindow->show();
    }
    else
    {
        QMessageBox::warning(this, "提示", "无法获取好友信息");
    }
}

void MainWindow::openChatWindow(QString friendUsername, QString friendName, QString friendIp, QString friendIco)
{
    Q_UNUSED(friendUsername);
    for (Server *existingWindow : Server::chatWindows)
    {
        if (existingWindow->getFriendIp() == friendIp)
        {
            existingWindow->show();
            existingWindow->raise();
            return;
        }
    }

    Server *chatWindow = new Server(this, friendName, friendIp, friendIco);

    if (!friendIp.isEmpty() && friendIp.contains(":"))
    {
        if (ipToSocketMap.contains(friendIp) && ipToSocketMap[friendIp]->state() == QAbstractSocket::ConnectedState)
        {
            chatWindow->setSocket(ipToSocketMap[friendIp]);
            connect(ipToSocketMap[friendIp], &QTcpSocket::readyRead, this, &MainWindow::readClientData);
        }
        else
        {
            QString ip = friendIp;
            quint16 port = 8888;
            if (friendIp.contains(":"))
            {
                QStringList parts = friendIp.split(":");
                ip = parts[0];
                port = parts[1].toUShort();
            }

            QTcpSocket *socket = new QTcpSocket();
            connect(socket, &QTcpSocket::connected, [this, socket, friendIp, chatWindow]() {
                ipToSocketMap[friendIp] = socket;
                connect(socket, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnected);
                connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readClientData);
                chatWindow->addSocket(socket);
            });
            socket->connectToHost(ip, port);
        }
    }
    chatWindow->show();
}

void MainWindow::connectToAllFriends()
{
    QSqlQuery query;
    QString sql = QString("select friendusername from friend where username='%1';").arg(currentUsername);
    if (query.exec(sql))
    {
        while (query.next())
        {
            QString friendUsername = query.value(0).toString();

            QSqlQuery ipQuery;
            QString ipSql = QString("select ip from user where username='%1';").arg(friendUsername);
            if (ipQuery.exec(ipSql) && ipQuery.next())
            {
                QString friendIp = ipQuery.value(0).toString();
                if (!friendIp.isEmpty() && friendIp.contains(":"))
                {
                    connectToFriend(friendIp);
                }
            }
        }
    }
}

void MainWindow::refreshFriendStatus()
{
    for (int i = 0; i < ui->listWidget_friends->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidget_friends->item(i);
        QString friendUsername = item->data(Qt::UserRole).toString();

        QSqlQuery query;
        QString sql = QString("select ip from user where username='%1';").arg(friendUsername);
        if (query.exec(sql) && query.next())
        {
            QString friendIp = query.value(0).toString();
            bool hasValidConnection = ipToSocketMap.contains(friendIp) &&
                                      ipToSocketMap[friendIp]->state() == QAbstractSocket::ConnectedState;

            if (!friendIp.isEmpty() && friendIp.contains(":"))
            {
                if (!hasValidConnection)
                {
                    connectToFriend(friendIp);
                }
            }

            updateFriendOnlineStatus(friendIp, hasValidConnection);
        }
        else
        {
            updateFriendOnlineStatus("", false);
        }
    }
}

void MainWindow::updateFriendOnlineStatus(QString friendIp, bool online)
{
    for (int i = 0; i < ui->listWidget_friends->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidget_friends->item(i);
        QString friendUsername = item->data(Qt::UserRole).toString();

        QSqlQuery query;
        QString sql = QString("select ip from user where username='%1';").arg(friendUsername);
        if (query.exec(sql) && query.next())
        {
            QString ip = query.value(0).toString();
            if (ip == friendIp)
            {
                QString text = item->text();
                if (online)
                {
                    text.remove(" [离线]");
                    if (!text.contains(" [在线]"))
                    {
                        item->setText(text + " [在线]");
                    }
                    item->setForeground(QColor(0, 180, 0));
                }
                else
                {
                    text.remove(" [在线]");
                    if (!text.contains(" [离线]"))
                    {
                        item->setText(text + " [离线]");
                    }
                    item->setForeground(QColor(128, 128, 128));
                }
                break;
            }
        }
    }
}

void MainWindow::newClientConnected()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    clientSockets.push_back(socket);

    QString ip = socket->peerAddress().toString().remove("::ffff:");

    QSqlQuery query;
    QString sql = QString("select ip, name from user where ip like '%1:%';").arg(ip);
    QString friendIp = ip;
    QString friendName = ip;
    if (query.exec(sql) && query.next())
    {
        friendIp = query.value(0).toString();
        friendName = query.value(1).toString();
    }

    ipToSocketMap[friendIp] = socket;

    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readClientData);

    updateFriendOnlineStatus(friendIp, true);
}

void MainWindow::clientDisconnected()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    QString ip = socket->peerAddress().toString().remove("::ffff:");
    QString fullIp = ip + ":" + QString::number(socket->peerPort());

    QString friendIpToRemove;
    for (auto it = ipToSocketMap.begin(); it != ipToSocketMap.end(); ++it)
    {
        if (it.value() == socket)
        {
            friendIpToRemove = it.key();
            break;
        }
    }
    ipToSocketMap.remove(friendIpToRemove);

    int index = clientSockets.indexOf(socket);
    if (index >= 0)
    {
        clientSockets.removeAt(index);
    }

    QSqlQuery query;
    QString sql = QString("select name from user where ip like '%1:%';").arg(ip);
    QString friendName = ip;
    if (query.exec(sql) && query.next())
    {
        friendName = query.value(0).toString();
    }

    updateFriendOnlineStatus(friendIpToRemove, false);
    socket->deleteLater();
}

void MainWindow::readClientData()
{
    QTcpSocket *socket = dynamic_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    static QMap<QTcpSocket *, QString> pendingDataMap;

    QByteArray data = socket->readAll();
    QString pendingData = pendingDataMap[socket];
    pendingData += QString(data);

    while (true)
    {
        int msgEndIndex = pendingData.indexOf("\nMSG_END\n");
        int fileEndIndex = pendingData.indexOf("\nFILE_END\n");

        int endIndex = -1;
        int endLength = 0;

        if (msgEndIndex != -1 && fileEndIndex != -1)
        {
            if (msgEndIndex < fileEndIndex)
            {
                endIndex = msgEndIndex;
                endLength = 9;
            }
            else
            {
                endIndex = fileEndIndex;
                endLength = 10;
            }
        }
        else if (msgEndIndex != -1)
        {
            endIndex = msgEndIndex;
            endLength = 9;
        }
        else if (fileEndIndex != -1)
        {
            endIndex = fileEndIndex;
            endLength = 10;
        }

        if (endIndex == -1)
            break;

        QString msgStr = pendingData.left(endIndex);
        pendingData = pendingData.mid(endIndex + endLength);

        QString ip = socket->peerAddress().toString().remove("::ffff:");

        QString friendIp;
        QString friendName = ip;

        bool found = false;
        for (auto it = ipToSocketMap.begin(); it != ipToSocketMap.end(); ++it)
        {
            if (it.value() == socket)
            {
                friendIp = it.key();
                found = true;
                break;
            }
        }

        QSqlQuery q;
        if (found && !friendIp.isEmpty())
        {
            QString s = QString("select name from user where ip='%1';").arg(friendIp);
            if (q.exec(s) && q.next())
                friendName = q.value(0).toString();
        }
        else
        {
            QString s = QString("select name from user where ip like '%1:%';").arg(ip);
            if (q.exec(s) && q.next())
                friendName = q.value(0).toString();
        }

        for (Server *chatWindow : Server::chatWindows)
        {
            QString chatWindowIp = chatWindow->getFriendIp();
            if (chatWindow->hasSocket(socket) || chatWindowIp == friendIp || chatWindowIp.startsWith(ip + ":"))
            {
                chatWindow->receiveMessage(friendName, msgStr);
                break;
            }
        }
    }

    pendingDataMap[socket] = pendingData;
}

void MainWindow::registerSocket(QString friendIp, QTcpSocket *socket)
{
    if (socket)
    {
        if (ipToSocketMap.contains(friendIp))
        {
            QTcpSocket *oldSocket = ipToSocketMap[friendIp];
            clientSockets.removeOne(oldSocket);
            oldSocket->deleteLater();
        }
        ipToSocketMap[friendIp] = socket;
        clientSockets.push_back(socket);
        connect(socket, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readClientData);
    }
}

void MainWindow::connectToFriend(QString friendIp)
{
    if (ipToSocketMap.contains(friendIp))
    {
        QTcpSocket *socket = ipToSocketMap[friendIp];
        if (socket->state() == QAbstractSocket::ConnectedState)
        {
            qDebug() << "Already connected to" << friendIp;
            return;
        }
        else
        {
            socket->deleteLater();
            ipToSocketMap.remove(friendIp);
        }
    }

    if (pendingConnections.contains(friendIp))
    {
        qDebug() << "Already connecting to" << friendIp;
        return;
    }

    QString ip = friendIp;
    quint16 port = 8888;

    if (friendIp.contains(":"))
    {
        QStringList parts = friendIp.split(":");
        ip = parts[0];
        port = parts[1].toUShort();
    }

    qDebug() << "Connecting to friend:" << ip << ":" << port;
    pendingConnections.insert(friendIp);

    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(ip, port);

    connect(socket, &QTcpSocket::connected, [this, socket, friendIp]() {
        qDebug() << "Connected to" << friendIp;
        pendingConnections.remove(friendIp);
        clientSockets.push_back(socket);
        ipToSocketMap[friendIp] = socket;
        connect(socket, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readClientData);

        updateFriendOnlineStatus(friendIp, true);
    });

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            [this, socket, friendIp](QAbstractSocket::SocketError err) {
                qDebug() << "Connection failed to" << friendIp << ":" << socket->errorString();
                pendingConnections.remove(friendIp);
                socket->deleteLater();
                updateFriendOnlineStatus(friendIp, false);
            });
}
