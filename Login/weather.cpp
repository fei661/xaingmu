#include "weather.h"
#include "ui_weather.h"

Weather::Weather(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Weather)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    loadCityKey();
    connect(manager, &QNetworkAccessManager::finished, this, &Weather::Reply);
    showMockData();
}

Weather::~Weather() { delete ui; }

void Weather::loadCityKey()
{
    QFile file(":/res/citykeys.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isOpen())
        return;
    QString temp = file.readAll();
    file.close();
    QStringList list = temp.split(',');
    foreach (QString value, list)
    {
        QString codes = value.split(':').at(0);
        QString citys = value.split(':').at(1);
        QString code = codes.replace("\"", "");
        QString city = citys.replace("\"", "");
        citykeys.insert(city, code);
        ui->comboBox->addItem(city);
    }
}

void Weather::RequestData(QString city)
{
    auto p = citykeys.find(city);
    if (p == citykeys.end())
    {
        QMessageBox::warning(this, "城市信息提示", "查询该城市不存在");
        return;
    }
    QString http = QString("http://t.weather.sojson.com/api/weather/city/%1")
                       .arg(citykeys.value(city));
    QUrl URL(http);
    QNetworkRequest request(URL);
    manager->get(request);
}

void Weather::Reply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        QMessageBox::warning(this, "网络错误", "网络请求失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    QByteArray array = reply->readAll();
    JSON_Value(array);
    reply->deleteLater();
}

void Weather::showMockData()
{
    showCityWeather("北京");
}

void Weather::showCityWeather(QString city)
{
    ui->label_city->setText(city + "市");
    ui->label_ymd->setText(QDate::currentDate().toString("yyyy/MM/dd"));
    ui->label_week->setText(QDate::currentDate().toString("dddd"));

    if (city.contains("北京")) {
        ui->label_type->setText("晴");
        ui->label_du->setText("28°C");
        ui->label_high->setText("33°C");
        ui->label_low->setText("22°C");
        ui->label_pm->setText("PM2.5: 45");
        ui->label_fxfl->setText("西南风");
        updateFutureWeather("多云", "32°C", "23°C", "小雨", "29°C", "20°C", "晴", "34°C", "24°C", "阴", "30°C", "22°C");
    } else if (city.contains("上海")) {
        ui->label_type->setText("多云");
        ui->label_du->setText("30°C");
        ui->label_high->setText("35°C");
        ui->label_low->setText("26°C");
        ui->label_pm->setText("PM2.5: 30");
        ui->label_fxfl->setText("东南风");
        updateFutureWeather("晴", "36°C", "27°C", "多云", "34°C", "25°C", "小雨", "31°C", "24°C", "阴", "33°C", "26°C");
    } else if (city.contains("广州")) {
        ui->label_type->setText("小雨");
        ui->label_du->setText("26°C");
        ui->label_high->setText("30°C");
        ui->label_low->setText("24°C");
        ui->label_pm->setText("PM2.5: 25");
        ui->label_fxfl->setText("南风");
        updateFutureWeather("中雨", "28°C", "23°C", "多云", "31°C", "25°C", "晴", "33°C", "26°C", "多云", "32°C", "25°C");
    } else if (city.contains("深圳")) {
        ui->label_type->setText("阴");
        ui->label_du->setText("27°C");
        ui->label_high->setText("31°C");
        ui->label_low->setText("25°C");
        ui->label_pm->setText("PM2.5: 20");
        ui->label_fxfl->setText("东北风");
        updateFutureWeather("晴", "32°C", "26°C", "多云", "30°C", "24°C", "小雨", "28°C", "23°C", "晴", "33°C", "27°C");
    } else if (city.contains("成都")) {
        ui->label_type->setText("雾");
        ui->label_du->setText("24°C");
        ui->label_high->setText("28°C");
        ui->label_low->setText("20°C");
        ui->label_pm->setText("PM2.5: 80");
        ui->label_fxfl->setText("北风");
        updateFutureWeather("多云", "29°C", "21°C", "晴", "30°C", "22°C", "雾", "26°C", "19°C", "多云", "28°C", "20°C");
    } else if (city.contains("杭州")) {
        ui->label_type->setText("晴");
        ui->label_du->setText("31°C");
        ui->label_high->setText("36°C");
        ui->label_low->setText("26°C");
        ui->label_pm->setText("PM2.5: 35");
        ui->label_fxfl->setText("东风");
        updateFutureWeather("多云", "34°C", "25°C", "晴", "37°C", "27°C", "小雨", "32°C", "24°C", "阴", "33°C", "25°C");
    } else if (city.contains("重庆")) {
        ui->label_type->setText("小雨");
        ui->label_du->setText("25°C");
        ui->label_high->setText("29°C");
        ui->label_low->setText("22°C");
        ui->label_pm->setText("PM2.5: 50");
        ui->label_fxfl->setText("西南风");
        updateFutureWeather("中雨", "27°C", "21°C", "多云", "30°C", "23°C", "晴", "32°C", "24°C", "多云", "29°C", "22°C");
    } else if (city.contains("西安")) {
        ui->label_type->setText("晴");
        ui->label_du->setText("32°C");
        ui->label_high->setText("37°C");
        ui->label_low->setText("24°C");
        ui->label_pm->setText("PM2.5: 60");
        ui->label_fxfl->setText("西北风");
        updateFutureWeather("多云", "35°C", "23°C", "晴", "38°C", "25°C", "阴", "34°C", "24°C", "晴", "36°C", "25°C");
    } else if (city.contains("武汉")) {
        ui->label_type->setText("多云");
        ui->label_du->setText("29°C");
        ui->label_high->setText("34°C");
        ui->label_low->setText("25°C");
        ui->label_pm->setText("PM2.5: 40");
        ui->label_fxfl->setText("东南风");
        updateFutureWeather("晴", "35°C", "26°C", "小雨", "31°C", "24°C", "多云", "33°C", "25°C", "晴", "36°C", "27°C");
    } else if (city.contains("南京")) {
        ui->label_type->setText("晴");
        ui->label_du->setText("30°C");
        ui->label_high->setText("35°C");
        ui->label_low->setText("25°C");
        ui->label_pm->setText("PM2.5: 38");
        ui->label_fxfl->setText("东北风");
        updateFutureWeather("多云", "33°C", "24°C", "晴", "36°C", "26°C", "阴", "32°C", "25°C", "小雨", "30°C", "23°C");
    } else if (city.contains("呼和")) {
        ui->label_type->setText("多云");
        ui->label_du->setText("22°C");
        ui->label_high->setText("28°C");
        ui->label_low->setText("16°C");
        ui->label_pm->setText("PM2.5: 55");
        ui->label_fxfl->setText("西北风");
        updateFutureWeather("晴", "30°C", "17°C", "多云", "27°C", "15°C", "阴", "25°C", "14°C", "晴", "29°C", "16°C");
    } else {
        ui->label_type->setText("晴");
        ui->label_du->setText("26°C");
        ui->label_high->setText("31°C");
        ui->label_low->setText("22°C");
        ui->label_pm->setText("PM2.5: 35");
        ui->label_fxfl->setText("东北风");
        updateFutureWeather("多云", "29°C", "21°C", "小雨", "27°C", "20°C", "晴", "30°C", "22°C", "阴", "28°C", "21°C");
    }
}

void Weather::updateFutureWeather(QString t1, QString h1, QString l1, QString t2, QString h2, QString l2, QString t3, QString h3, QString l3, QString t4, QString h4, QString l4)
{
    ui->label_type1->setText(t1);
    ui->label_high1->setText(h1);
    ui->label_low1->setText(l1);
    ui->label_type2->setText(t2);
    ui->label_high2->setText(h2);
    ui->label_low2->setText(l2);
    ui->label_type3->setText(t3);
    ui->label_high3->setText(h3);
    ui->label_low3->setText(l3);
    ui->label_type4->setText(t4);
    ui->label_high4->setText(h4);
    ui->label_low4->setText(l4);
}

void Weather::on_comboBox_currentTextChanged(const QString &arg1) { showCityWeather(arg1); }
void Weather::on_pushButton_clicked() { showCityWeather(ui->lineEdit->text()); }

void Weather::JSON_Value(QByteArray &array)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(array, &err);
    if (err.error != QJsonParseError::NoError)
    {
        QMessageBox::warning(this, "JSON解析提示", "解析错误：" + err.errorString());
        return;
    }
    QJsonObject OBJ = doc.object();
    JSON_Left(OBJ);
}

void Weather::JSON_Left(QJsonObject OBJ)
{
    Q_UNUSED(OBJ);
}

void Weather::JSON_Right(QJsonObject OBJ) {}
void Weather::setType(QString type, QLabel *label) { Q_UNUSED(type); Q_UNUSED(label); }
