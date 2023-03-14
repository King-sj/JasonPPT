#pragma once

#include "ui_PlayVideo.h"
#include<QMainWindow>
//#include<QMutex>
#include<QTimer>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include<ImgForm.h>
using namespace std;
using namespace cv;
class PlayVideo : public QMainWindow, private Ui::PlayVideo
{
    Q_OBJECT

public:
    explicit PlayVideo(QWidget *parent = nullptr) = delete;
    explicit PlayVideo(QString videoPath,QWidget *parent = nullptr);

private:
    void playFrame();
private:
    VideoCapture vc;
    Mat mat;
    QTimer* timer;
    int fps;
    int frame_time;

};

