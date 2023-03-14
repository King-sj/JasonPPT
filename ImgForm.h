#pragma once

#include <QLabel>
#include <QObject>
#include<QPaintEvent>
#include<QMouseEvent>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"PlayVideo.h"
using namespace std;
using namespace cv;
//class PlayVideo;//前置声明
class ImgForm : public QLabel
{
    Q_OBJECT
public:
    enum class TYPE{NONE,IMG,VIDEO};
    enum RIGHT_KEY_MENU{DELETE_IMG_FORM = 0,CHANGE_DEFAULT_CONFIG,EDIT};
public:
    ImgForm(QWidget* p = nullptr,int width = 640,int height = 480,int x = 0,int y = 0);
    ~ImgForm();
public:
    void moveToLeft();
    void moveTORight();
    static QImage MatToQImage(const cv::Mat &inMat);
//    void img_move(int x,int y);
//    void img_move(QPoint p);
    TYPE getType()const;
    QString getFileName()const;
    void SetFileName(QString file);
    bool getIsSpecial();
public:
    int interval  = 10;
    int width = 400;
    int height = 300;
    int deaultTime = 2;
    int fps = 30;
private:
    int addImgSize = 60;
    bool isSpecial = false;//是否特殊配置
    TYPE type = TYPE::NONE;
    QString fileName = ":/img/res/add.png";
    QImage img;//
    Mat mat;
    PlayVideo* playVideo;
private:
    void edit();
    virtual void load();
    virtual void onTaskBoxContextMenuEvent();
    virtual void initRightKeyMenu();
    virtual void do_CHANGE_DEFAULT_CONFIG();


    bool createPreviewWidthFile(const char *file);
private slots:
    virtual void paintEvent(QPaintEvent* e)override;
    virtual void mousePressEvent(QMouseEvent* e)override;

private slots:
    void doEditDone(QString newFileName);
signals:
    void loadDone();
    void signal_delete(QPointer<ImgForm> p);
};

