#pragma once

#include <QObject>
#include"ImgForm.h"
#include<QEvent>
class VMItem : public QObject
{
    Q_OBJECT
public:
    VMItem(QWidget* p = nullptr,int width = 640,int height = 480,int x = 0,int y = 0);
    ~VMItem();
//    ~VMItem();
    ImgForm* getVMItem();
    QLabel* getimeLabel();
private:
    ImgForm* imgForm;
    QLabel timeLabel;
    const int timeLabelHeight = 20;
private:
    bool eventFilter(QObject* watched,QEvent* e)override;
    void initTimeLabel();
    QString getFormatTime(int seconds);
signals:

};

