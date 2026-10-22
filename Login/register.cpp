#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Register)
{
    ui->setupUi(this);

    QRegularExpression regExp("^[a-zA-Z0-9]*$");
    QRegularExpressionValidator *Validator = new QRegularExpressionValidator(regExp);
    ui->linepassword->setValidator(Validator);
    ui->linepasswordok->setValidator(Validator);
}

Register::~Register() { delete ui; }

QString Register::Pass_MD5(QString password)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(password.toUtf8());
    QByteArray array = hash.result();
    QString passok = array.toHex();
    qDebug() << passok;
    return passok;
}

void Register::on_btnReg_clicked()
{
    QString name = ui->linename->text().trimmed();
    QString password = ui->linepassword->text();
    QString passwordok = ui->linepasswordok->text();

    if (name.isEmpty())
    {
        QMessageBox::warning(this, "注册提示", "请输入用户昵称");
        return;
    }

    if (password.isEmpty())
    {
        QMessageBox::warning(this, "注册提示", "请输入密码");
        return;
    }

    if (password != passwordok)
    {
        QMessageBox::warning(this, "注册提示", "两次输入密码不一致");
        ui->linepasswordok->clear();
        return;
    }

    QSqlQuery query;
    QString checkSql = QString("select name from user where name='%1';").arg(name);
    if (query.exec(checkSql) && query.next())
    {
        QMessageBox::warning(this, "注册提示", "该昵称已被注册");
        ui->linename->clear();
        return;
    }

    QString sql = QString("insert into user values('%1','%2','%3','%4','');")
                      .arg(name)
                      .arg(name)
                      .arg(QString::number(ui->cBico->currentIndex() + 11) + ".png")
                      .arg(Pass_MD5(password));

    bool flag = query.exec(sql);
    if (flag)
    {
        QMessageBox::information(this, "注册提示", "注册成功");
        this->parentWidget()->show();
        this->close();
    }
    else
    {
        QMessageBox::warning(this, "注册提示", "注册失败: " + query.lastError().text());
        ui->linename->clear();
    }
}