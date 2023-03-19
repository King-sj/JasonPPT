#include "VMItem.h"
#include"Share.h"
VMItem::VMItem(QWidget *parent, int width, int height, int x, int y)
    : QObject{parent},timeLabel(parent)
{
    cout<<"VMItem initialed"<<endl;
    this->imgForm = new ImgForm(parent,width,height,x,y);
    initTimeLabel();
}

VMItem::~VMItem()
{
    delete imgForm;
    imgForm = nullptr;
}

ImgForm *VMItem::getVMItem()
{
    return this->imgForm;
}

QLabel* VMItem::getimeLabel()
{
    return &timeLabel;
}

bool VMItem::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == this->imgForm)
    {
        if(e->type() == QEvent::Paint)
        {
//            cout<<"imgForm repaint"<<endl;
            this->timeLabel.move(this->imgForm->pos().x(),this->imgForm->pos().y() - 1.5*timeLabelHeight);
            if(imgForm->getType() == ImgForm::TYPE::IMG)
                this->timeLabel.setText("时长为:"+QString::number(this->imgForm->deaultTime)+"s");
            else if(imgForm->getType() == ImgForm::TYPE::VIDEO)
                this->timeLabel.setText("时长为:"+getFormatTime(this->imgForm->deaultTime));
        }
    }
    return false;
}

QString VMItem::getFormatTime(int seconds)
{
    //don't consider ms
    int hh = seconds/3600;
    seconds -= (hh*3600);
    int mm = seconds/60;
    seconds -= (mm*60);
    int ss = seconds;
    QString ans = "";
    if(hh > 9){
        ans += QString::number(hh);
    }else if(hh > 0){
        ans += ("0" + QString::number(hh));
    }else{
        ans += "00";
    }
    ans+=":";

    if(mm > 9){
        ans += QString::number(mm);
    }else if(mm > 0){
        ans += ("0" + QString::number(mm));
    }else{
        ans += "00";
    }
    ans+=":";

    if(ss > 9){
        ans += QString::number(ss);
    }else if(ss > 0){
        ans += ("0" + QString::number(ss));
    }else{
        ans += "00";
    }
    return ans;//such as 00:00:00.000
}
void VMItem::initTimeLabel()
{
    this->timeLabel.resize(this->imgForm->width,timeLabelHeight);
    this->imgForm->installEventFilter(this);
    if(imgForm->getType() == ImgForm::TYPE::VIDEO)
    {
        this->timeLabel.setText("时长为:"+getFormatTime(imgForm->deaultTime));
    }
    this->timeLabel.setText("时长为:2s");
    this->timeLabel.setStyleSheet("QLabel{background-color:rgb(0,250,154)}");
    this->timeLabel.setAlignment(Qt::AlignHCenter);//水平居中
    this->timeLabel.show();
}
