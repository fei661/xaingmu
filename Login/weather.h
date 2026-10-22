#ifndef WEATHER_H
#define WEATHER_H

#include <QMainWindow>
#include <QNetworkAccessManager> //网络管理者
#include <QNetworkRequest>       //网络请求
#include <QNetworkReply>         //网络应答
#include <QMessageBox>           //消息盒子对话框
#include <QJsonObject>           //json对象
#include <QJsonArray>            //json数组
#include <QJsonDocument>         //json文档
#include <QJsonParseError>       //json错误
#include <QJsonValue>            //Json数据转化
#include <QFile>                 //文件类
#include <QMap>
#include <QDate>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Weather;
}
QT_END_NAMESPACE

class Weather : public QMainWindow
{
    Q_OBJECT

  public:
    Weather(QWidget *parent = nullptr);
    ~Weather();

    void loadCityKey();

    void RequestData(QString city);
    void JSON_Value(QByteArray &array);
    void JSON_Left(QJsonObject OBJ);
    void JSON_Right(QJsonObject OBJ);
    void setType(QString type, QLabel *label);
    void showMockData();
    void showCityWeather(QString city);
    void updateFutureWeather(QString t1, QString h1, QString l1, QString t2, QString h2, QString l2, QString t3, QString h3, QString l3, QString t4, QString h4, QString l4);
  private slots:
    void Reply(QNetworkReply *reply);
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_pushButton_clicked();

  private:
    Ui::Weather *ui;
    QMap<QString, QString> citykeys;
    QNetworkAccessManager *manager;
};
#endif // WEATHER_H
