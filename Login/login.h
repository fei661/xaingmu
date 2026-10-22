#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include "register.h"
#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class Login;
}
QT_END_NAMESPACE

class Login : public QMainWindow
{
    Q_OBJECT

  public:
    Login(QWidget *parent = nullptr);
    ~Login();

  private slots:
    void on_btnreg_clicked();

    void on_lineusername_textChanged(const QString &arg1);

    void on_btnok_clicked();

    void on_checkBox_stateChanged(int arg1);

  private:
    Ui::Login *ui;
};
#endif // LOGIN_H
