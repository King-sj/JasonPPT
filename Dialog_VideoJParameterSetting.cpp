#include "Dialog_VideoJParameterSetting.h"
//#include"Share.h"
#include<QThread>
Dialog_VideoJParameterSetting::Dialog_VideoJParameterSetting(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
//    cout<<"Dialog_VideoJParameterSetting:"<<QThread::currentThreadId()<<endl;
}

void Dialog_VideoJParameterSetting::accept()
{
    qDebug()<<"Dialog_VideoJParameterSetting: accept\n";

    _parameters.export_file_path = this->lineEdit_file_path->text();
    _parameters.fps = this->lineEdit_fps->text().toInt();
    _parameters.h = lineEdit_height->text().toInt();
    _parameters.w = lineEdit_width->text().toInt();
    _parameters.img_time_default = lineEdit_img_time->text().toInt();
    _parameters.export_filename = lineEdit_filename->text();
    emit signal_videoParameters(_parameters);
    //调用父类
    QDialog::accept();
}
