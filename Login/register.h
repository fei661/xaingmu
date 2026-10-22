#ifndef REGISTER_H
#define REGISTER_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QtDebug>
#include <QRegularExpressionValidator> //验证器
#include <QCryptographicHash>          //MD5类加密

namespace Ui
{
class Register;
}

class Register : public QMainWindow
{
    Q_OBJECT

  public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

    static QString Pass_MD5(QString password);

  private slots:
    void on_btnReg_clicked();

  private:
    Ui::Register *ui;
};

#endif // REGISTER_H
