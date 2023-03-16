#include "VMItem.h"
#include"Share.h"
VMItem::VMItem(QWidget *parent, int width, int height, int x, int y)
    : QObject{parent},timeLabel(parent)
{
    cout<<"VMItem initialed"<<endl;
    this->imgForm = new ImgForm(parent,width,height,x,y);
    initTimeLabel();
}

ImgForm *VMItem::getVMItem()
{
    return this->imgForm;
}

bool VMItem::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == this->imgForm)
    {
        if(e->type() == QEvent::Paint)
        {
//            cout<<"imgForm repaint"<<endl;
            this->timeLabel.move(this->imgForm->pos().x(),this->imgForm->pos().y() - 1.5*timeLabelHeight);
            this->timeLabel.setText("时长为:"+QString::number(this->imgForm->deaultTime)+"s");
        }
    }
    return false;
}

void VMItem::initTimeLabel()
{
    this->timeLabel.resize(this->imgForm->width,timeLabelHeight);
    this->imgForm->installEventFilter(this);
    this->timeLabel.setText("时长为:2s");
    this->timeLabel.setStyleSheet("QLabel{background-color:rgb(0,250,154)}");
    this->timeLabel.setAlignment(Qt::AlignHCenter);//水平居中
    this->timeLabel.show();
}
