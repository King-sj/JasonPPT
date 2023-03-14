#include "EditImgForm.h"
#include <QMimeDatabase>
#include<QMimeType>
#include<QMessageBox>
#include<QPainter>
#include<QPixmap>
#include<QFileInfo>
#include<QDir>
#include<QApplication>
EditImgForm::EditImgForm(QString fileName, QWidget *parent):
    QMainWindow(parent)
{
    setupUi(this);
//    this->hide();
    QMessageBox::information(NULL,"","原始文件路径为:" + QDir(fileName).absolutePath());
//    this->installEventFilter(this);

    this->fileName = fileName;
    timer = new QTimer(this);
    timer->setInterval(100);//10fps
    //note:奇怪的bug,这样做才能正常刷新
    connect(timer,&QTimer::timeout,[=](){repaint();});
    timer->start();
    init();

//    img = QImage(fileName);
//    if(img.isNull())
//    {
//        QMessageBox::information(NULL,"","获取图片失败:"+QDir(fileName).absolutePath());
//        this->close();
//    }
//    imgWidth = img.width();
//    imgHeight = img.height();
//    QMessageBox::information(NULL,"","图片大小为:"+QString::number(imgWidth)+"X"+QString::number(imgHeight));

}

EditImgForm::~EditImgForm()
{
    QMessageBox msb;
    msb.setText("确定更改？");
    msb.addButton(QMessageBox::StandardButton::Ok);
    msb.addButton(QMessageBox::StandardButton::Cancel);

    if(msb.exec() == QMessageBox::StandardButton::Ok)
    {
        getNewImg();
    }

    //error:sometimes 关闭后崩溃，可能是内存问题
//    delete vc;
//    vc = NULL;

}

void EditImgForm::init()
{
    QDir dir("./");
    dir.mkdir("EditedImg");

    rectTexts.clear();
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(fileName);//获取文件类型

    if(mime.name().startsWith("image/"))
    {
        cout<<"image\n";
        type = TYPE::IMG;
        //drawImg();
        //update();
    }else if(mime.name().startsWith("video/")){
        QMessageBox msb;
        msb.setText("暂不支持视频编辑");
        msb.exec();
        this->close();

//        vc->open(fileName.toStdString());
//        if(!vc->isOpened())
//        {
//            QMessageBox msb;
//            msb.setText("打开文件失败");
//            msb.exec();
//            this->close();
//        }

    }else{
        cout<<"other\n";
        type = TYPE::NONE;
    }


}

void EditImgForm::drawImg()
{
//    img = QImage(fileName);
    QFile file(fileName);
    if(!img.load(fileName))
    {
        QFile file(fileName);
        if(!file.isOpen())
        {
            QMessageBox::information(NULL,"",fileName+"打开失败");
            this->close();
            return;
        }
        img.loadFromData(file.readAll());
    }
    if(img.isNull() || img.width() == 0)
    {
        QMessageBox::information(NULL,"","获取图片失败:"+QDir(fileName).absolutePath());
        this->close();
    }
    auto w = this->screen()->geometry().width();
    cout<<"screen width"<<w<<endl;
    imgWidth = img.width();
    imgHeight = img.height();
//    QMessageBox::information(NULL,"","图片大小为:"+QString::number(imgWidth)+"X"+QString::number(imgHeight));
//    imgHeight = vc->get(CAP_PROP_FRAME_HEIGHT);
//    imgWidth = vc->get(CAP_PROP_FRAME_WIDTH);

    QPainter painter(this);
//    QRect rect((this->width() - imgWidth)/2.0,topY,imgWidth,imgHeight);
    QRect rect(30.0,topY,imgWidth,imgHeight);
//    cout<<"TopY"<<topY<<endl;
    if(imgWidth > w)
        painter.drawImage(rect,img.scaled(QSize(w,w),Qt::KeepAspectRatio));
    else painter.drawImage(rect,img);
    //update();
}

QImage EditImgForm::MatToQImage(const Mat &inMat)
{
    switch (inMat.type())
    {
        case CV_8UC4:		// 8-bit, 4 channel
        {
            QImage image(inMat.data,
                inMat.cols, inMat.rows,
                static_cast<int>(inMat.step),
                QImage::Format_ARGB32);

            return image;
        }

        case CV_8UC3:		// 8-bit, 3 channel
        {
            QImage image(inMat.data,
                inMat.cols, inMat.rows,
                static_cast<int>(inMat.step),
                QImage::Format_RGB888);

            return image.rgbSwapped();
        }


        case CV_8UC1:// 8-bit, 1 channel
        {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QImage image(inMat.data,
                inMat.cols, inMat.rows,
                static_cast<int>(inMat.step),
                QImage::Format_Grayscale8); //Format_Alpha8 and Format_Grayscale8 were added in Qt 5.5
#else
            static QVector<QRgb>  sColorTable;

            // only create our color table the first time
            if (sColorTable.isEmpty())
            {
                sColorTable.resize(256);

                for (int i = 0; i < 256; ++i)
                {
                    sColorTable[i] = qRgb(i, i, i);
                }
            }

            QImage image(inMat.data,
                inMat.cols, inMat.rows,
                static_cast<int>(inMat.step),
                QImage::Format_Indexed8);

            image.setColorTable(sColorTable);
#endif
            return image;
        }

        default:
            qWarning() << "CVS::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            throw "CVS::cvMatToQImage() - cv::Mat image type not handled in switch:";
            break;
    }

    return QImage();

}

void EditImgForm::paintEvent(QPaintEvent *e)
{
    drawImg();
//    qDebug()<<"textNum:"<<rectTexts.size()<<"\n";
    for(auto& t:rectTexts)
    {
//        if(t.parent() == this)
        t.show();
//        cout<<"show pos:"<<this->x()<<","<<this->y()<<endl;
    }

//    e->ignore();

}

void EditImgForm::wheelEvent(QWheelEvent *e)
{
    //添加图片滚动
    //if(std::abs(topY) <= imgHeight)
        topY += e->angleDelta().y();
        for(auto& t:rectTexts )
        {
            t.move(t.x(),t.y() + e->angleDelta().y());
        }
//        update();
//        update();
        repaint();
//        showNormal();
//        adjustSize();
}

void EditImgForm::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
//        cout<<"event:"<<e->pos()<<endl;
        rightButtonPoint = e->pos();
        initRightKeyMenu();
    }
}

//bool EditImgForm::eventFilter(QObject *watched, QEvent *event)
//{
//    cout<<"event"<<event->type()<<endl;
//    cout<<watched->objectName();
//    static QPoint lastPoint;
//    static bool isPressed = false;

//    if (event->type() == QEvent::MouseButtonPress) {
//        QMouseEvent *e = static_cast<QMouseEvent *>(event);
//        if (this->rect().contains(e->pos()) && (e->button() == Qt::LeftButton)) {
//            lastPoint = e->pos();
//            isPressed = true;
//            cout<<"Left Button pressed"<<endl;
//        }
//    } else if (event->type() == QEvent::MouseMove && isPressed) {
//        QMouseEvent *e = static_cast<QMouseEvent *>(event);
//        int dx = e->pos().x() - lastPoint.x();
//        int dy = e->pos().y() - lastPoint.y();

//        this->move(this->x() + dx, this->y() + dy);
//        cout<<"Mouse move"<<endl;
//        return true;
//    } else if (event->type() == QEvent::MouseButtonRelease && isPressed) {
//        isPressed = false;
//        cout<<"mouse release"<<endl;
//    }


//    if (event->type() == QEvent::MouseButtonPress) {

////        emit clicked();
//    } else if (event->type() == QEvent::MouseButtonDblClick) {
////        emit doubleClicked();
//    }

//    return QWidget::eventFilter(watched, event);

//}


void EditImgForm::initRightKeyMenu()
{

    //创建菜单对象
    QMenu *pMenu = new QMenu(this);

    QAction *pIMPORT_TEXT = new QAction(tr("导入文案"), this);



    pIMPORT_TEXT->setData(RIGHT_KEY_MENU::IMPORT_TEXT);


    //把QAction对象添加到菜单上
    pMenu->addAction(pIMPORT_TEXT);



    //连接鼠标右键点击信号
    connect(pIMPORT_TEXT, &QAction::triggered, this,&EditImgForm::onTaskBoxContextMenuEvent);



    //在鼠标右键点击的地方显示菜单

//    cout<<"rightButtonPoint:"<<rightButtonPoint<<endl;
    pMenu->exec(cursor().pos());

    //释放内存
    QList<QAction*> list = pMenu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete pMenu;
}

void EditImgForm::onTaskBoxContextMenuEvent()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
    if(pEven == nullptr)return;


    int iType = pEven->data().toInt();

    switch (iType)
    {
    case RIGHT_KEY_MENU::IMPORT_TEXT:
        InsertRectText(rightButtonPoint);
        break;
    default:
        break;
    }
}

void EditImgForm::InsertRectText(QPoint p, QString s)
{

    auto rt =  QRect(p,QSize(100,100));
    auto temp = RectText(rt,s,this);


    rectTexts.append(temp);
    rectTexts[rectTexts.size()-1].installEventFilter(this);
    QMessageBox msb;
    msb.setText("添加成功");
    msb.exec();

}

void EditImgForm::getNewImg()
{
//    QPixmap temp(fileName);
    QPixmap temp = QPixmap::fromImage(img);
    QPainter p(&temp);
    for(auto& t:rectTexts)
    {
        QPixmap textPix(t.grab(QRect(QPoint(2,2),t.viewport()->size()-QSize(2,2))));
        QRect rt(t.x() - 30,t.y() - topY,textPix.width(),textPix.height());
        p.drawPixmap(rt,textPix);
    }
//    cout<<rectTexts.size()<<endl;
    if(rectTexts.size() > 0)
    {
        QFileInfo fileInfo(fileName);
        QString fileNewName = QDir("./EditedImg/"+fileInfo.fileName()).absolutePath();
        temp.save(fileNewName);
        QMessageBox::information(NULL,"","修改后的文件保存到了：" + fileName);
        emit EditDone(fileNewName);
    }
}
