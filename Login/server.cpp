#include "server.h"
#include "ui_server.h"
#include "mainwindow.h"
#include <QListWidgetItem>

QVector<Server *> Server::chatWindows;

Server::Server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    mserver = new QTcpServer(this);
    connect(mserver, &QTcpServer::newConnection, this, &Server::new_client);

    ui->label->setCursor(Qt::PointingHandCursor);
    ui->label->installEventFilter(this);

    ui->label_17->setCursor(Qt::PointingHandCursor);
    ui->label_17->installEventFilter(this);

    chatWindows.push_back(this);
}

Server::Server(QWidget *parent, QString currentUsername)
    : QMainWindow(parent)
    , ui(new Ui::Server)
    , currentUsername(currentUsername)
{
    ui->setupUi(this);
    this->setWindowTitle("QQ聊天");
    mserver = new QTcpServer(this);
    connect(mserver, &QTcpServer::newConnection, this, &Server::new_client);

    ui->groupBox_2->hide();

    loadOnlineFriends(currentUsername);

    chatWindows.push_back(this);
}

Server::Server(QWidget *parent, QString name, QString ip, QString ico)
    : QMainWindow(parent)
    , ui(new Ui::Server)
    , friendIp(ip)
    , friendName(name)
{
    ui->setupUi(this);
    this->setWindowTitle("与 " + name + " 聊天");

    QString iconPath;
    if (ico == "15.png" || ico == "16.png")
        iconPath = QString(":/res/%1").arg(ico);
    else
        iconPath = QString(":/res/ico/%1").arg(ico);
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull())
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(name + " (" + ip + ")");
        item->setIcon(QIcon(pixmap.scaled(48, 48, Qt::KeepAspectRatio)));
        ui->listWidget->addItem(item);
    }
    else
    {
        ui->listWidget->addItem(name + " (" + ip + ")");
    }

    ui->label->setCursor(Qt::PointingHandCursor);
    ui->label->installEventFilter(this);

    ui->label_17->setCursor(Qt::PointingHandCursor);
    ui->label_17->installEventFilter(this);

    chatWindows.push_back(this);
}

void Server::addSocket(QTcpSocket *socket)
{
    if (socket && !sockets.contains(socket))
    {
        sockets.push_back(socket);
        connect(socket, &QTcpSocket::disconnected, this, &Server::dis_client);
    }
}

void Server::setSocket(QTcpSocket *socket)
{
    if (socket && !sockets.contains(socket))
    {
        if (socket->state() == QAbstractSocket::ConnectedState)
        {
            addSocket(socket);
        }
        else
        {
            connect(socket, &QTcpSocket::connected, [this, socket]() {
                addSocket(socket);
            });
        }
    }
}

void Server::appendMessage(QString msg)
{
    ui->textBrowser->append(msg);
}

Server::~Server()
{
    chatWindows.removeOne(this);
    delete ui;
}
void Server::new_client()
{
    QTcpSocket *msocket = mserver->nextPendingConnection();
    save_client(msocket);
    connect(msocket, &QTcpSocket::disconnected, this, &Server::dis_client);

    MainWindow *mainWindow = dynamic_cast<MainWindow *>(parent());
    if (mainWindow)
    {
        QString ip = msocket->peerAddress().toString().remove("::ffff:");
        QString friendIp = ip;
        QSqlQuery q;
        QString s = QString("select ip from user where ip like '%1:%';").arg(ip);
        if (q.exec(s) && q.next())
            friendIp = q.value(0).toString();
        mainWindow->registerSocket(friendIp, msocket);
    }
}
void Server::save_client(QTcpSocket *msocket)
{
    msocket->write("connect success");
    sockets.push_back(msocket);
    QString ip = msocket->peerAddress().toString().remove("::ffff:");

    QSqlQuery query;
    QString sql = QString("select name, ico from user where ip='%1';").arg(ip);
    if (query.exec(sql) && query.next())
    {
        QString name = query.value(0).toString();
        QString ico = query.value(1).toString();

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(name + " (" + ip + ")");
        item->setData(Qt::UserRole, ip);

        QString iconPath;
        if (ico == "15.png" || ico == "16.png")
            iconPath = QString(":/res/%1").arg(ico);
        else
            iconPath = QString(":/res/ico/%1").arg(ico);
        QPixmap pixmap(iconPath);
        if (!pixmap.isNull())
        {
            item->setIcon(QIcon(pixmap.scaled(48, 48, Qt::KeepAspectRatio)));
        }

        ui->listWidget->addItem(item);
    }
    else
    {
        ui->listWidget->addItem(ip);
    }
}
void Server::dis_client()
{
    QTcpSocket *msocket = dynamic_cast<QTcpSocket *>(sender());
    int row = sockets.indexOf(msocket);
    if (row >= 0)
    {
        sockets.removeAt(row);
        QListWidgetItem *item = ui->listWidget->takeItem(row);
        delete item;
        item = nullptr;
    }
}
void Server::on_btnsend_clicked()
{
    QString msgHtml = ui->textEdit->toHtml();
    QString msgText = ui->textEdit->toPlainText();
    if (msgText.isEmpty() && msgHtml.isEmpty())
    {
        return;
    }

    QString dt = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    QString data = QString("[%1] 我: ").arg(dt);
    ui->textBrowser->append(data);
    ui->textBrowser->insertHtml(msgHtml);
    ui->textBrowser->append("");
    ui->textEdit->clear();

    QString sendHtml;
    if (msgHtml.contains("<img"))
    {
        QStringList imgParts = msgHtml.split("<img");
        QString imgTag;
        for (int i = 1; i < imgParts.size(); i++)
        {
            QString part = imgParts[i];
            int endPos = part.indexOf(">");
            if (endPos != -1)
            {
                imgTag += "<img" + part.left(endPos + 1);
            }
        }
        if (!imgTag.isEmpty())
        {
            int bodyStart = msgHtml.indexOf("<body");
            if (bodyStart != -1)
            {
                bodyStart = msgHtml.indexOf(">", bodyStart);
                if (bodyStart != -1)
                {
                    int bodyEnd = msgHtml.lastIndexOf("</body>");
                    if (bodyEnd != -1)
                    {
                        QString bodyContent = msgHtml.mid(bodyStart + 1, bodyEnd - bodyStart - 1);
                        if (!bodyContent.isEmpty())
                            sendHtml = bodyContent;
                    }
                }
            }
            if (sendHtml.isEmpty())
                sendHtml = imgTag;
        }
        else
        {
            sendHtml = msgText;
        }
    }
    else
    {
        sendHtml = msgText;
    }

    QTcpSocket *msocket = nullptr;
    for (QTcpSocket *socket : sockets)
    {
        if (socket->state() == QAbstractSocket::ConnectedState)
        {
            msocket = socket;
            break;
        }
    }

    if (msocket)
    {
        msocket->write((sendHtml + "\nMSG_END\n").toUtf8());
        msocket->flush();
        return;
    }

    if (friendIp.isEmpty())
    {
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

    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, [this, socket, sendHtml]() {
        sockets.push_back(socket);
        connect(socket, &QTcpSocket::disconnected, this, &Server::dis_client);
        socket->write((sendHtml + "\nMSG_END\n").toUtf8());
            socket->flush();

            for (const QString &msg : pendingMessages)
            {
                socket->write((msg + "\nMSG_END\n").toUtf8());
                socket->flush();
            }
        pendingMessages.clear();

        MainWindow *mainWindow = dynamic_cast<MainWindow *>(parent());
        if (mainWindow)
        {
            mainWindow->registerSocket(friendIp, socket);
        }
    });
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            [this, socket](QAbstractSocket::SocketError) {
                socket->deleteLater();
            });
    socket->connectToHost(ip, port);
}
void Server::on_btnclear_clicked() { ui->textBrowser->clear(); }

bool Server::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->label && event->type() == QEvent::MouseButtonPress)
    {
        on_label_clicked();
        return true;
    }
    if (obj == ui->label_17 && event->type() == QEvent::MouseButtonPress)
    {
        on_label_17_clicked();
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void Server::on_label_clicked()
{
    EmojiDialog *dialog = new EmojiDialog(this);
    connect(dialog, &EmojiDialog::emojiSelected, this, &Server::insertEmoji);
    dialog->exec();
    dialog->deleteLater();
}

void Server::insertEmoji(QString emojiPath)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QPixmap pixmap(emojiPath);
    if (!pixmap.isNull())
    {
        QImage image = pixmap.toImage();
        QByteArray ba;
        QBuffer buffer(&ba);
        image.save(&buffer, "PNG");
        QString base64 = ba.toBase64();

        QString html = QString("<img src=\"data:image/png;base64,%1\" width=\"32\" height=\"32\">").arg(base64);
        cursor.insertHtml(html);
    }
}

void Server::receiveMessage(QString senderName, QString msgStr)
{
    QString dt = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");

    if (msgStr.contains("<img"))
    {
        ui->textBrowser->append("[" + dt + "] " + senderName + ": ");
        ui->textBrowser->insertHtml(msgStr);
        ui->textBrowser->append("");
    }
    else if (msgStr.startsWith("FILE_START:"))
    {
        QStringList parts = msgStr.split("\n");
        QString header = parts.first();
        QStringList headerParts = header.split(":");
        if (headerParts.size() >= 3)
        {
            QString fileName = headerParts[1];
            QString savePath = QFileDialog::getSaveFileName(this, "保存文件", fileName, "所有文件 (*.*)");
            if (!savePath.isEmpty())
            {
                QString base64Data;
                for (int i = 1; i < parts.size(); ++i)
                {
                    base64Data += parts[i];
                }
                QByteArray fileData = QByteArray::fromBase64(base64Data.toUtf8());
                QFile saveFile(savePath);
                if (saveFile.open(QIODevice::WriteOnly))
                {
                    saveFile.write(fileData);
                    saveFile.close();
                    ui->textBrowser->append(QString("[%1] %2: 发送文件 - %3 已保存").arg(dt).arg(senderName).arg(fileName));
                }
                else
                {
                    ui->textBrowser->append(QString("[%1] %2: 发送文件 - %3 保存失败").arg(dt).arg(senderName).arg(fileName));
                }
            }
        }
    }
    else
    {
        QString msg = QString("[%1] %2: %3").arg(dt).arg(senderName).arg(msgStr);
        ui->textBrowser->append(msg);
    }
}

void Server::on_label_17_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择要发送的文件", "", "所有文件 (*.*)");
    if (!filePath.isEmpty())
    {
        sendFile(filePath);
    }
}

void Server::sendFile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "发送失败", "无法打开文件");
        return;
    }

    QByteArray fileData = file.readAll();
    QString fileName = QFileInfo(filePath).fileName();
    qint64 fileSize = fileData.size();
    file.close();

    QString dt = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    QString msg = QString("[%1] 我: 发送文件 - %2 (%3 bytes)\n").arg(dt).arg(fileName).arg(fileSize);
    ui->textBrowser->append(msg);

    QString fileHeader = QString("FILE_START:%1:%2\n").arg(fileName).arg(fileSize);
    QString fileFooter = "\nFILE_END\n";

    QString base64Data = QString(fileData.toBase64());
    QByteArray sendData = fileHeader.toUtf8() + base64Data.toUtf8() + fileFooter.toUtf8();

    if (!sockets.isEmpty())
    {
        QTcpSocket *msocket = sockets.first();
        if (msocket->state() == QAbstractSocket::ConnectedState)
        {
            msocket->write(sendData);
            msocket->flush();
            return;
        }
        else
        {
            sockets.removeFirst();
        }
    }

    if (friendIp.isEmpty())
    {
        return;
    }

    QString ip = friendIp;
    quint16 port = 8888;
    if (friendIp.contains(":"))
    {
        QStringList parts = friendIp.split(":");
        ip = parts.first();
        port = parts.last().toUShort();
    }

    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, [this, socket, sendData]() {
        sockets.push_back(socket);
        connect(socket, &QTcpSocket::disconnected, this, &Server::dis_client);
        socket->write(sendData);
        socket->flush();

        MainWindow *mainWindow = dynamic_cast<MainWindow *>(parent());
        if (mainWindow)
        {
            mainWindow->registerSocket(friendIp, socket);
        }
    });
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            [this, socket](QAbstractSocket::SocketError) {
                socket->deleteLater();
            });
    socket->connectToHost(ip, port);
}

void Server::loadOnlineFriends(QString currentUsername)
{
    ui->listWidget->clear();

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

            QSqlQuery ipQuery;
            QString ipSql = QString("select ip from user where username='%1';").arg(friendUsername);
            QString friendIp;
            if (ipQuery.exec(ipSql) && ipQuery.next())
            {
                friendIp = ipQuery.value(0).toString();
            }

            QListWidgetItem *item = new QListWidgetItem();
            if (!friendIp.isEmpty() && friendIp.contains(":"))
            {
                item->setText(friendName + " [在线]");
                item->setForeground(QColor(0, 128, 0));
            }
            else
            {
                item->setText(friendName + " [离线]");
                item->setForeground(QColor(128, 128, 128));
                continue;
            }

            item->setData(Qt::UserRole, friendUsername);
            item->setData(Qt::UserRole + 1, friendIp);
            item->setData(Qt::UserRole + 2, friendIco);

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

            ui->listWidget->addItem(item);
        }
    }
}

void Server::on_listWidget_itemDoubleClicked(int row)
{
    QListWidgetItem *item = ui->listWidget->item(row);
    if (!item)
        return;

    QString friendUsername = item->data(Qt::UserRole).toString();
    QString friendIp = item->data(Qt::UserRole + 1).toString();
    QString friendIco = item->data(Qt::UserRole + 2).toString();
    QString friendName = item->text().split(" ").first();

    MainWindow *mainWindow = dynamic_cast<MainWindow *>(parent());
    if (mainWindow)
    {
        mainWindow->openChatWindow(friendUsername, friendName, friendIp, friendIco);
    }
}
