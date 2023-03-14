#pragma once

#include "ui_EditImgForm.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<QImage>
#include<QPaintEvent>
#include<QWheelEvent>
#include"RectText.h"
#include<QMouseEvent>
#include<QTimer>
using namespace std;
using namespace cv;
class EditImgForm : public QMainWindow, private Ui::EditImgForm
{
    Q_OBJECT


public:
    EditImgForm(QWidget *parent = nullptr) = delete;
    EditImgForm(QString fileName,QWidget *parent = nullptr) ;
    ~EditImgForm();
    enum class TYPE{NONE,IMG,VIDEO};
    enum RIGHT_KEY_MENU{IMPORT_TEXT};
public:
private:
    void init();
    void drawImg();

    QImage MatToQImage(const cv::Mat &inMat);
    void initRightKeyMenu();
    void onTaskBoxContextMenuEvent();
    void InsertRectText(QPoint p,QString s= "");


    void getNewImg();
private:
    void paintEvent(QPaintEvent* e)override;
    void wheelEvent(QWheelEvent* e)override;
    void mousePressEvent(QMouseEvent* e)override;
//    bool eventFilter(QObject *watched, QEvent *event)override;
private:
    QString fileName = "";
//    VideoCapture* vc;
    TYPE type = TYPE::NONE;
    QImage img;
    int imgWidth = 0;
    int imgHeight = 0;
    int topY = 10;
    QPoint rightButtonPoint{0,0};
    QVector<RectText>rectTexts;
    QTimer* timer;
signals:
    void EditDone(QString newFileName);
private slots:
};

