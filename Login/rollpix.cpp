#include "rollpix.h"

rollpix::rollpix(QWidget *parent)
    : QWidget(parent)
{
    startTimer(5000); // 5秒钟更新一次界面
}
//绘制事件
void rollpix::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);        //将窗口设置为画板
    painter.drawPixmap(0, 0, pix); //将绘制对象进行重绘显示
}
//更新显示
void rollpix::timerEvent(QTimerEvent *event)
{
    static int number = 0;
    if (number >= filepaths.count()) //超过总数量
        number = 0;
    //从目录列表的每个下标位置读取
    QString filename = filepaths.at(number++);
    //显示对象 目录中完整的路径
    pix.load(dir.filePath(filename));
    //设置显示大小
    pix = pix.scaled(this->size());
    //设置完成更新数据
    update();
}
//读取目录中的内容
void rollpix::setBackgroundDir(const QString &path)
{
    //设置目录的读取路径
    dir.setPath(path);
    //设置读取内容过滤格式和后缀
    dir.setFilter(QDir::Files);
    dir.setNameFilters({".bmp", "*.jpg", "*.png"});
    //读取目录中所有文件名
    filepaths = dir.entryList();
}
