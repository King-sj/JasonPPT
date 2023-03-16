//#pragma execution_character_set("utf-8")
#include "MainWindow.h"
#include <QApplication>
#include<QThread>
#include"Share.h"
#include<QtCore5Compat/QTextCodec>
//#include<QtCore5Compat/QTextCodec>
int main(int argc, char *argv[])
{
        //解决汉字乱码问题

//    #if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
//    #if _MSC_VER
//    QTextCodec *codec = QTextCodec::codecForName("gbk");
//    #else
//    QTextCodec *codec = QTextCodec::codecForName("utf-8");
//    #endif
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForCStrings(codec);
//    QTextCodec::setCodecForTr(codec);
//    #else
//    QTextCodec *codec = QTextCodec::codecForName("utf-8");
//    QTextCodec::setCodecForLocale(codec);
//    #endif

    QApplication a(argc, argv);
    cout<<"main:"<<QThread::currentThreadId()<<endl;
    MainWindow w;
    w.show();
    return a.exec();

}
