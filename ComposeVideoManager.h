#pragma once
#include <QObject>
#include<QVector>
#include<QMutex>
#include"VMItem.h"
#include"Dialog_VideoJParameterSetting.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
extern "C" {
    #include <libavcodec/avcodec.h>
    #include<libavformat/avformat.h>
    #include<libavcodec/packet.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libavformat/version.h>
    #include <libavutil/time.h>
    #include <libavutil/frame.h>
    #include <libavutil/mathematics.h>
    #include <libswresample/swresample.h>

    #include "libavfilter/avfilter.h"
    #include "libavutil/imgutils.h"
//    #include "libavutil/ffversion.h"
    #include "libpostproc/postprocess.h"
}
class ComposeVideoManager : public QObject
{
    Q_OBJECT
public:
    explicit ComposeVideoManager(QObject *parent = nullptr);
    bool composeNewVideo(QVector<VMItem*> imgForms,Dialog_VideoJParameterSetting::Parameters parameters,QVector<QString> musicFileNames);
    struct Audio{
        QString audioFileName;
        int beginTime;//sec
        int time;//long
    public:
        Audio(QString audioFileName,int beginTime,int time):audioFileName(audioFileName),beginTime(beginTime),time(time){};
    };

private:
    Dialog_VideoJParameterSetting::Parameters _parameters;
    QMutex isStopMutex;
    bool isStop = true;
private:
    void init();
    bool writerImg(VMItem* img);
    bool writerVideo(VMItem* video);
//    bool writeOneQImage(QImage img);
    QImage getSuitableImage(const QImage& img);
    QImage MatToQImage(const cv::Mat &inMat);
    cv::Mat QImage2cvMat(QImage image);

    bool initFfmpegWriter(QString outFilename);
    void closeFfmpeg();

    int getVideoFPS(QString fileName);
    QString changeVideoFPS(QString _fileName,int fps);
    QSize getVideoSize(QString fileName);

    bool rgb2mp4();
    int rgb2mp4Encode(AVCodecContext* codecCtx, AVFrame* yuvFrame, AVPacket* pkt, AVStream* vStream, AVFormatContext* fmtCtx);
    void mergeVideoAndBgm(QString _videoFileName,const QVector<QString>& _musicFileNames);
    QString getFormatTime(int seconds);
    QString getAudio(QString videoFileName);
    void recoverVideoAudio();
    void addSilent(QString srcfileName);
private:
    QString fileAllName;
    VideoWriter* vw;

//    AVFormatContext *kAVFormatContext;
//    AVOutputFormat *kAVOutputFormat;
//    AVCodecContext *kAVCodecContext;
//    AVDictionary *kAVDictionary = NULL;
//    AVDictionary *opt = NULL;
//    AVCodec *video_codec;
//    AVStream *kAVStream;
//    AVFrame *frame;
//    AVFrame *tmp_frame;
//    AVPacket* kAVPacket ;//= { 0 };
//    struct SwsContext *kSwsContext;
//    AVRational time{1,24};  /*1s25帧*/
//    AVRational time_1{1,1};  /*1s25帧*/
//    uint8_t *intBuffer = 0;                           // 图片数据缓冲区

//    int64_t next_pts = 0;                               //下一帧位置

private:
    int ret = -1;
    AVFormatContext* fmtCtx = NULL;
    AVCodecContext* codecCtx = NULL;
    AVCodec* codec = NULL;
    AVStream* vStream = NULL;
    AVPacket* pkt = av_packet_alloc();

    AVFrame* rgbFrame = NULL, * yuvFrame = NULL;

    int w;
    int h;
    int perFrameCnt;

    SwsContext* imgCtx;
    uint8_t* yuvBuffer;
    uint8_t* buffer;
    int size;
    int yuvSize;

    int64_t CapPos = 0;

    QVector<Audio>audios;
signals:
    void signalProcessInformationText(QString processInformationText);
    void signalProcess(double pro);
    void signalNextStage(QString stage);
    void signalNewImg(QImage img);
};

