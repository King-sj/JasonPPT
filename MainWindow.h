#pragma once

#include "ui_MainWindow.h"
#include <QMainWindow>
//#include"ImgForm.h"
#include"VMItem.h"
#include"Dialog_VideoJParameterSetting.h"
#include<QVector>
#include<QPointer>
#include<QPoint>
#include<QPaintEvent>
#include<QMouseEvent>
#include<QKeyEvent>
#include<QThread>
#include<QSet>
#include<QProcess>
#include"ComposeVideoManager.h"
// ffmpeg 是纯 C 语言的代码，在 C++ 当中不能直接进行 include
extern "C" {
    #include <libavcodec/avcodec.h>
    #include<libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libavformat/version.h>
    #include <libavutil/time.h>
    #include <libavutil/mathematics.h>
    #include <libswresample/swresample.h>
}
class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
//    QThread* playVideoThread;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    enum RIGHT_KEY_MENU{INSERT_NEW_IMGFORM = 0};
private slots:
    void on_actionexport_triggered();

    void on_action_play_vedio_triggered();

    void on_action_export_triggered();

private:
    void init();
    inline QPoint getCenterPointForRectRegion(int width,int height);
    void initRightKeyMenu();
    void onTaskBoxContextMenuEvent();
    void composeVideo();
    bool imgs2video(QString folderName);
    bool video2imgs(QString fileName,int pos = 0);
    bool saveImg(QString fileName,const QImage& img);
    bool saveVedio(QString toFileName);
    void rebuiltFolder();
    VMItem* getNewVMItem();
    void mergeVideoAndBgm(QString videoFileName ,QString musicFileName);
    QString getFormatTime(int seconds);
    void addPauseToBat();
    void setupCMD();//启动新的cmd
    void doCommand(QString command);//执行命令
    QString changeVideoFPS(QString fileName , int fps);
private slots:
    void dealImgFormSignal_loadDone();
    void do_signal_delete(ImgForm* p);
    void do_triggered_action_INSERT_NEW_IMGFORM();
    void on_actionImportMusic_triggered();

    void on_action_import_triggered();
    //process_cmd signal_slot
    void slot_readdata();
    void slot_cmderror();
    void slot_cmdfinished();

//    void pngTomp4_2();
    void do_signalProcessInformationText(QString processInformationText);
    void do_signalProcess(double pro);
    void do_signalNextStage(QString stage);
private:
    virtual void paintEvent(QPaintEvent* e)override;
    virtual void mousePressEvent(QMouseEvent* e)override;
    virtual void keyPressEvent(QKeyEvent* e)override;
private:
    int imgWidth = 320;
    int imgHeight = 240;//在界面上显示的大小
    int videoWidth = 1280;
    int videoHeight = 720;//输出的大小
    int pos = 0;
    int interval = 5;
    int border = imgWidth / 2;
    int imgNum = 0;
    QPoint rightKeyMenuePos;
    QString temp_imgs_folder = "./temp/imgs/";
    Dialog_VideoJParameterSetting* vedioParameEdit;
    Dialog_VideoJParameterSetting::Parameters _parameters;
    PlayVideo* playVideo;
    QString musicFileName ="";
    QString musicFileNames_txt ="./temp/targetBgm.txt";
    QPointer<QProcess> process;

    QThread* composeVideoManagerThread;
    ComposeVideoManager* composeVideoManager;
    QString _stage="";
private:
    QVector< VMItem* >vector_VMItem;
    QSet<int>fpsList;

//ffmpeg:
private:
    SwrContext* swrContext;
    AVCodecContext* audioCodecContext;
    void videoInfo(const char *);
    int getVideoFPS(const char* fileName);
private:
    void initFFMPEG();
signals:
    void sendcontext(AVFormatContext*,AVCodecContext*,AVCodecContext*,SwrContext*);
    void signalComposeVideo(QVector<VMItem*> imgForms,Dialog_VideoJParameterSetting::Parameters parameters);
};
