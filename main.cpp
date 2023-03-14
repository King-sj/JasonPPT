#pragma execution_character_set("utf-8")
#include "MainWindow.h"
#include <QApplication>
#include<QThread>
#include"Share.h"
#include<QtCore5Compat/QTextCodec>
int main(int argc, char *argv[])
{
    //解决汉字乱码问题
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    QApplication a(argc, argv);
    cout<<"main:"<<QThread::currentThreadId()<<endl;
    MainWindow w;
    w.show();
    return a.exec();

}
