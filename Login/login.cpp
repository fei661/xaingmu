#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login() { delete ui; }
//注册
void Login::on_btnreg_clicked()
{
    Register *w = new Register(this); //实例化对象
    w->show();                        //显示
    this->hide();                     //隐藏
}
void Login::on_lineusername_textChanged(const QString &arg1)
{
    QString sql = QString("select * from user where username='%1' or name='%1';").arg(arg1);
    QSqlQuery query(sql);
    if (query.next())
    {
        QString ico = query.value(2).toString();
        QString styles;
        if (ico == "15.png" || ico == "16.png")
            styles = QString("border-image: url(:/res/%1);").arg(ico);
        else
            styles = QString("border-image: url(:/res/ico/%1);").arg(ico);
        ui->label_ico->setStyleSheet(styles);
    }
    else
    {
        QString styles = QString("border-image: url(:/res/6.png);");
        ui->label_ico->setStyleSheet(styles);
    }
}
void Login::on_btnok_clicked()
{
    if (!ui->checkBox->isChecked())
    {
        ui->label_warning->setVisible(true);
        return;
    }
    ui->label_warning->setVisible(false);

    QString input = ui->lineusername->text().trimmed();
    QString password = ui->linepassword->text();

    QSqlQuery query;
    QString sql = QString("select * from user where username='%1' or name='%2';")
                      .arg(input)
                      .arg(input);
    if (query.exec(sql) && query.next())
    {
        if (query.value(3).toString() == Register::Pass_MD5(password))
        {
            QMessageBox::information(this, "登录提示", "登录成功");
            MainWindow *w = new MainWindow(nullptr, query.value(0).toString());
            w->show();
            this->close();
        }
        else
        {
            QMessageBox::warning(this, "登录提示", "登录失败，密码输入错误");
            ui->linepassword->clear();
        }
    }
    else
    {
        QMessageBox::warning(this, "登录提示", "登录失败，该用户未注册");
        ui->lineusername->clear();
    }
}

void Login::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->label_warning->setVisible(false);
    }
}
