#include "Dialog_SingleImgFormConfig.h"

Dialog_SingleImgFormConfig::Dialog_SingleImgFormConfig(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

void Dialog_SingleImgFormConfig::accept()
{
     Parameters _parameters;
     _parameters.default_time = this->lineEdit_defaultTime->text().toInt();
    emit signal_videoParameters(_parameters);
    //调用父类
    QDialog::accept();
}
