#include "PlayVideo.h"
#include"Share.h"
#include<QThread>
#include"ImgForm.h"
//#include<Windows.h>
#include <QMediaPlayer>
#include <QVideoWidget>
#include<QTime>

//#include<QtMultimedia>
//#include <QtMultimediaWidgets>
//多媒体类
PlayVideo::PlayVideo(QString videoPath, QWidget *parent):QMainWindow(parent)
{
    setupUi(this);


    timer = new QTimer();
    vc.open(videoPath.toStdString());
    this->label_playVideo->resize(vc.get(CAP_PROP_FRAME_WIDTH),vc.get(CAP_PROP_FRAME_HEIGHT));

    connect(timer,&QTimer::timeout,this,&PlayVideo::playFrame);
    fps = vc.get(CAP_PROP_FPS);
    frame_time = int(1000.0/fps);
    timer->start(frame_time);

    this->progressBar->setRange(0,vc.get(CAP_PROP_FRAME_COUNT));
    this->progressBar->setValue(0);
    this->hide();
}

void PlayVideo::playFrame()
{
    if(vc.isOpened())
    {
        try {
            vc>>mat;
            this->label_playVideo->setPixmap(QPixmap::fromImage(ImgForm::MatToQImage(mat)));
            this->progressBar->setValue(vc.get(CAP_PROP_POS_FRAMES));
        } catch (...) {
            cout<<"play video erorr"<<endl;
        }

    }
}
