#include "login.h"

#include <QApplication>

//数据库连接
void sqlite_init(QSqlDatabase &DB)
{
    //设置数据库的驱动内容 链接sqlite
    DB = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库保存路径
    DB.setDatabaseName("./data.db");
    //打开数据库
    DB.open();
    if (!DB.isOpen()) //打开失败
    {
        exit(0);
    }
    else //打开成功
    {
        QSqlQuery query;

        QString sql = QString("create table if not exists user(username text primary key"
                              ",name text not null,ico text not null,password text not null,ip text);");
        bool flag = query.exec(sql);
        if (flag)
            qDebug() << "用户表创建成功";
        else
            qDebug() << "用户表创建失败";

        QString alterSql = QString("alter table user add column ip text;");
        bool alterFlag = query.exec(alterSql);
        if (alterFlag)
            qDebug() << "用户表添加IP列成功";
        else
            qDebug() << "用户表添加IP列:" << query.lastError().text();

        QString sql_friend = QString("create table if not exists friend(username text not null,"
                                     "friendusername text not null,"
                                     "friendip text,"
                                     "primary key(username, friendusername));");
        bool flag_friend = query.exec(sql_friend);
        if (flag_friend)
            qDebug() << "好友表创建成功";
        else
            qDebug() << "好友表创建失败";

        QString alterFriendSql = QString("alter table friend add column friendip text;");
        bool alterFriendFlag = query.exec(alterFriendSql);
        if (alterFriendFlag)
            qDebug() << "好友表添加friendip列成功";
        else
            qDebug() << "好友表添加friendip列:" << query.lastError().text();

        QString updateIco1 = QString("update user set ico='11.png' where name='飞';");
        query.exec(updateIco1);
        QString updateIco2 = QString("update user set ico='12.png' where name='哈';");
        query.exec(updateIco2);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSqlDatabase DB; //数据库对象
    sqlite_init(DB); //数据库初始化
    Login w;
    w.show();
    return a.exec();
}
