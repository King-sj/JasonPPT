


#include "RectText.h"
#include"Share.h"
#include<QEvent>
#include<QMouseEvent>
#include <QTextDocument>
#include<QWheelEvent>
#include<QApplication>
#include<QFont>
RectText::RectText(QRect rect, QString text, QWidget *parent)
    :QTextEdit{text,parent}
{
    this->setAcceptDrops(true);//可拖入
    init();
    this->move(rect.x(),rect.y());
    this->resize(rect.width(),rect.height());
//    this->resize(rect.width()*10,rect.height()*10);
//    this->show();
//    this->hide();
//    cout<<"create_fun_pos:"<<this->x()<<","<<this->y()<<endl;
//    this->installEventFilter(this);
//    this->setFont(QFont("Microsoft YaHei"));
//    this->setCurrentCharFormat(QTextCh)
}

RectText::RectText(const RectText &h)
{
    this->setParent(h.parentWidget());
    this->move(h.pos());
    this->resize(h.size());
    this->setText(h.toPlainText());
//    cout<<"copy_fun_pos:"<<this->x()<<","<<this->y()<<endl;
    init();
}

RectText &RectText::operator=(const RectText &h)
{
    this->setParent(h.parentWidget());
    this->move(h.pos());
    this->resize(h.size());
    this->setText(h.toPlainText());
    init();
//    cout<<"equal_fun_pos:"<<this->x()<<","<<this->y()<<endl;
    return *this;
}
void RectText::init()
{
//    this->setAttribute( Qt::WA_NoSystemBackground, true );
//        this->setFrameStyle(QFrame::NoFrame);
    this->setStyleSheet("RectText{color: red; background-color: transparent;border:2px dashed #00f;}");

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(this,&QTextEdit::textChanged,this,&RectText::slot_textChange);
}

bool RectText::eventFilter(QObject *watched, QEvent *event)
{
//    cout<<"event"<<event->type()<<endl;
//    cout<<watched->objectName();
    static QPoint lastPoint;
    static bool isPressed = false;

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        if (this->rect().contains(e->pos()) && (e->button() == Qt::LeftButton)) {
            lastPoint = e->pos();
            isPressed = true;
            cout<<"Left Button pressed"<<endl;
        }
    } else if (event->type() == QEvent::MouseMove && isPressed) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        int dx = e->pos().x() - lastPoint.x();
        int dy = e->pos().y() - lastPoint.y();

        this->move(this->x() + dx, this->y() + dy);
        cout<<"Mouse move"<<endl;
        return true;
    } else if (event->type() == QEvent::MouseButtonRelease && isPressed) {
        isPressed = false;
        cout<<"mouse release"<<endl;
    }


    if (event->type() == QEvent::MouseButtonPress) {

//        emit clicked();
    } else if (event->type() == QEvent::MouseButtonDblClick) {
//        emit doubleClicked();
    }

//    return QWidget::eventFilter(watched, event);
    return false;
}

void RectText::wheelEvent(QWheelEvent *e)
{

    //ctrl + 滚动 resize
    if(QApplication::keyboardModifiers() == Qt::ControlModifier)//ctrl键的判断
    {
        int s = this->width() + e->angleDelta().y();
        if(s > 0)
        {
            this->resize(s,this->height());
        }
    }
    e->accept();
}



void RectText::slot_textChange()
{
    QTextDocument *document = this->document();
    this->setFixedHeight(document->size().height()+2);
    qDebug()<<document->size().height();
}



