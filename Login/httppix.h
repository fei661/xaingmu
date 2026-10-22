#ifndef HTTPPIX_H
#define HTTPPIX_H

#include <QMainWindow>
#include <QNetworkAccessManager> //网络管理者
#include <QNetworkRequest>       //网络请求
#include <QNetworkReply>         //网络应答
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui
{
class HTTPpix;
}
QT_END_NAMESPACE

class HTTPpix : public QMainWindow
{
    Q_OBJECT

  public:
    HTTPpix(QWidget *parent = nullptr);
    ~HTTPpix();

    void Request(QString http);
    void show_pix(QByteArray &array);
  private slots:
    void on_pushButton_clicked();

    void Reply(QNetworkReply *reply);

  private:
    Ui::HTTPpix *ui;
    //实例化QNetworkAccessManager网络管理对象
    QNetworkAccessManager *manager;
};
#endif // HTTPPIX_H
