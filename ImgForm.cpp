#include "ImgForm.h"
#include<QMessageBox>
#include<QFileDialog>
#include <QPainter>
#include<QImage>
#include<QMimeType>
#include<QMimeDatabase>
#include<QTime>
#include<QMenu>
#include<QThread>
#include"Share.h"
#include"Dialog_SingleImgFormConfig.h"
#include"EditImgForm.h"
#include<stdio.h>
using namespace std;
using namespace cv;
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


/************************note:debug***************************************************************/
#define cout qDebug()<<"["<<__LINE__<<":"<<__FILE__<<":"<<__FUNCTION__<<"]"
using Qt::endl;
/***************************************************************************************/
ImgForm::ImgForm(QWidget *p,int w,int h,int x,int y)
{
    this->setParent(p);

    this->setStyleSheet("ImgForm{background-color:rgb(0,250,154)}");
    this->move(x,y);
    width = w;
    height = h;
    this->resize(width,height);
    this->hide();
    cout<<"ImgForm:"<<QThread::currentThreadId()<<endl;
    playVideo = nullptr;
    //只输出错误日志
    //utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);
}

ImgForm::~ImgForm()
{
    if(playVideo)
        delete playVideo;
    playVideo = NULL;
}



void ImgForm::moveToLeft()
{
    this->move(this->x() - width - interval, height);
}

void ImgForm::moveTORight()
{
    this->move(this->x() + width - interval, height);
}

void ImgForm::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    if(type == TYPE::NONE)
    {
        p.drawImage(QPoint(this->width/2.0 - addImgSize/2.0,this->height/2.0 - addImgSize/2.0),QImage(":/img/res/add.png").scaled(addImgSize,addImgSize));

    }else if(type == TYPE::IMG){
        p.drawImage(QPoint(0,0),img.scaled(width,height));
    }else{

        p.drawImage(QPoint(0,0),img.scaled(width,height));
        p.drawImage(QPoint(this->width/2.0 - addImgSize/2.0,this->height/2.0 - addImgSize/2.0),QImage(":/img/res/begin_2.jfif").scaled(addImgSize,addImgSize));
    }

    e->accept();
}

void ImgForm::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
//        if(type == TYPE::VIDEO)
//        {
//          //单击为播放
//            if(playVideo)
//            {
//                delete playVideo;
//            }
//            playVideo = new PlayVideo(fileName);
//            e->accept();
//        }
        fileName = QFileDialog::getOpenFileName(nullptr,
            "Open Image/Vedio", "/home", "Image/video Files (*.png *.jpg *.bmp *.mp4 *.avi *)");
        //cout<<e->pos()<<endl;

        if(fileName != "")
        {
            //type = TYPE::IMG;//default type
            load();
            update(this->pos().x(),this->pos().y(),this->width,this->height);
        }
        else if(type == TYPE::NONE){
//            type = TYPE::NONE;
            fileName = ":/img/res/add.png";
        }
        //否则不做任何改变

        e->accept();
    }
    if(e->button() == Qt::RightButton)
    {
        initRightKeyMenu();
        e->accept();
    }
}

void ImgForm::doEditDone(QString newFileName)
{
//    cout<<newFileName<<endl;
    this->fileName = QDir(newFileName).absolutePath();
    img = QImage(fileName);
//    QMessageBox::information(NULL,"","更改后路径"+this->fileName);
    update();
}

ImgForm::TYPE ImgForm::getType() const
{
    return type;
}

QString ImgForm::getFileName() const
{
    return fileName;
}

void ImgForm::SetFileName(QString file)
{
    fileName = file;

    load();
    update();
}

bool ImgForm::getIsSpecial()
{
    return isSpecial;
}

void ImgForm::edit()
{
//    QMessageBox::information(NULL,"",this->fileName);
    auto pMainEditWindow = new EditImgForm(fileName,this->img,this);

    pMainEditWindow->setWindowModality(Qt::WindowModal);//模态
    //关闭后自动释放窗口,不需要自己释放pMyMainWindow
    pMainEditWindow->setAttribute(Qt::WA_DeleteOnClose);


    connect(pMainEditWindow,&EditImgForm::EditDone,this,&ImgForm::doEditDone);

    pMainEditWindow->show();

    //delete pMainEditWindow;
}
void ImgForm::load()
{
    cout<<fileName<<endl;


    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(fileName);//获取文件类型
    cout<<"type:"<<mime.name()<<endl;

    if(mime.name().startsWith("image/") && fileName != ":/img/res/add.png")
    {
        cout<<"image\n";
        if(type == TYPE::NONE)
            emit loadDone();
        type = TYPE::IMG;
        img = QImage(fileName);
    }else if(mime.name().startsWith("video/")){
        cout<<"video\n";
        if(createPreviewWidthFile(fileName.toLocal8Bit().constData())){
            if(type == TYPE::NONE)
                emit loadDone();
            type = TYPE::VIDEO;
        }else{
            cout<<"open video unsuccessfully.";
//            type = TYPE::NONE;
        }
//        String cStr(fileName.toStdString());
////        cout<<QString::fromStdString(cStr);
//        VideoCapture cap;
//        cap.open(cStr);
//        if(cap.isOpened())
//        {
//            cout<<"open video successfully.";
//            cap >>mat;
//            //非播放
////            cap.set(CAP_PROP_POS_FRAMES,0);
//            img = MatToQImage(mat);
//            if(type == TYPE::NONE)
//                emit loadDone();
//            type = TYPE::VIDEO;
//        }else
//        {
//            cout<<"open video unsuccessfully.";
//            type = TYPE::NONE;
//        }
//        cap.release();
    }else{
        cout<<"other\n";
        type = TYPE::NONE;
    }
}
void ImgForm::initRightKeyMenu()
{

    //创建菜单对象
    QMenu *pMenu = new QMenu(this);

    QAction *pDELETE_IMG_FORM = new QAction(tr("删除"), this);
    QAction *pCHANGE_DEFAULT_CONFIG = new QAction(tr("改变图片默认参数"), this);
    QAction *pEDIT = new QAction(tr("编辑"), this);


    pDELETE_IMG_FORM->setData(RIGHT_KEY_MENU::DELETE_IMG_FORM);
    pCHANGE_DEFAULT_CONFIG->setData(RIGHT_KEY_MENU::CHANGE_DEFAULT_CONFIG);
    pEDIT->setData(RIGHT_KEY_MENU::EDIT);

    //把QAction对象添加到菜单上
    pMenu->addAction(pDELETE_IMG_FORM);
    pMenu->addAction(pCHANGE_DEFAULT_CONFIG);
    pMenu->addAction(pEDIT);


    //连接鼠标右键点击信号
    connect(pDELETE_IMG_FORM, &QAction::triggered, this,&ImgForm::onTaskBoxContextMenuEvent);
    connect(pCHANGE_DEFAULT_CONFIG, &QAction::triggered, this,&ImgForm::onTaskBoxContextMenuEvent);
    connect(pEDIT, &QAction::triggered, this,&ImgForm::onTaskBoxContextMenuEvent);


    //在鼠标右键点击的地方显示菜单
    pMenu->exec(cursor().pos());

    //释放内存
    QList<QAction*> list = pMenu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete pMenu;
}

void ImgForm::onTaskBoxContextMenuEvent()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
    if(pEven == nullptr)return;


    int iType = pEven->data().toInt();
//    cout<<iType<<" / "<<RIGHT_KEY_MENU::CHANGE_DEFAULT_CONFIG<<endl;
    switch (iType)
    {
    case RIGHT_KEY_MENU::DELETE_IMG_FORM:
        emit signal_delete(this);
        break;
    case RIGHT_KEY_MENU::CHANGE_DEFAULT_CONFIG:
        if(type != TYPE::IMG)
            break;
        do_CHANGE_DEFAULT_CONFIG();
        break;
    case RIGHT_KEY_MENU::EDIT:
        edit();
        break;
    default:
        break;
    }
}
void ImgForm::do_CHANGE_DEFAULT_CONFIG()
{
    auto dialog = new Dialog_SingleImgFormConfig();
    int time = deaultTime;
    connect(dialog,&Dialog_SingleImgFormConfig::signal_videoParameters,[&](Dialog_SingleImgFormConfig::Parameters parameters){
        deaultTime = parameters.default_time;
    });
    if(dialog->exec() == QDialog::Rejected)
    {
    //recover, but it seems to be unuseful
        deaultTime = time;
        isSpecial = false;
    }else{
        isSpecial = true;
//        this->update();
    }
    delete dialog;
    dialog = NULL;
}
// Mat 转换成 QImage
QImage ImgForm::MatToQImage(const cv::Mat &inMat)
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
            break;
    }

    return QImage();
}
bool ImgForm::createPreviewWidthFile(const char *file){
    AVFormatContext* fmt_ctx_ = nullptr;

    //打开视频文件
    int errCode = avformat_open_input(&fmt_ctx_, file, nullptr, nullptr);
    if(errCode != 0){
        QMessageBox::warning(NULL,"","avformat_open_input fail" + QString::number(errCode));
        return false;
    }

    //读取音视频流信息
    errCode = avformat_find_stream_info(fmt_ctx_, nullptr);
    if(errCode != 0){
        QMessageBox::warning(NULL,"","avformat_find_stream_info fail" + QString::number(errCode));
        avformat_close_input(&fmt_ctx_);
        return false;
    }
    //打印输出视频相关信息
    av_dump_format(fmt_ctx_, 0, file, 0);

    AVPacket* pkt = av_packet_alloc();
    AVFrame* temp_frame = av_frame_alloc();
    SwsContext* sws_ctx = nullptr;
    int ret = 0;
    QImage preview;
    bool preview_done = false;

    for (int i=0; i<int(fmt_ctx_->nb_streams) && !preview_done; i++){
        //只处理视频信息
        if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //查找视频解码器
            const AVCodec* codec = avcodec_find_decoder(fmt_ctx_->streams[i]->codecpar->codec_id);
            AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
            //根据提供的编解码器参数的值填充编解码器上下文
            avcodec_parameters_to_context(codec_ctx, fmt_ctx_->streams[i]->codecpar);
            //打开解码器
            avcodec_open2(codec_ctx, codec, nullptr);

            //读取帧数据
            while (av_read_frame(fmt_ctx_, pkt) >= 0){
                av_frame_unref(temp_frame);

                //对视频帧数据进行解码
                while ((ret = avcodec_receive_frame(codec_ctx, temp_frame)) == AVERROR(EAGAIN)){
                    ret = avcodec_send_packet(codec_ctx, pkt);
                    if (ret < 0) {
                        cout<<"Failed to send packet to decoder."  + QString::number(ret);
                        break;
                    }
                }

                if(ret < 0 && ret != AVERROR_EOF){
                    cout<<"Failed to receive packet from decoder."  + QString::number(ret);
                    continue;
                }

                //等比例缩放
                int dstH = this->width;
                int dstW = qRound(dstH * (float(temp_frame->width)/float(temp_frame->height)));
//                int dstW = this->height;
                //消除可能的告警
                dstH = (dstH >> 4) << 4;
                dstW = (dstW >> 4) << 4;

                sws_ctx = sws_getContext(
                                        temp_frame->width,
                                        temp_frame->height,
                                        static_cast<AVPixelFormat>(temp_frame->format),
                                        dstW,
                                        dstH,
                                        static_cast<AVPixelFormat>(AV_PIX_FMT_RGBA),
                                        SWS_FAST_BILINEAR,
                                        nullptr,
                                        nullptr,
                                        nullptr
                                        );
                int linesize[AV_NUM_DATA_POINTERS];
                linesize[0] = dstW*4;

                //生成图片
                preview = QImage(dstW, dstH, QImage::Format_RGBA8888);
                uint8_t* data = preview.bits();//note:key code
                sws_scale(sws_ctx,
                          temp_frame->data,
                          temp_frame->linesize,
                          0,
                          temp_frame->height,
                          &data,
                          linesize);
                sws_freeContext(sws_ctx);

                avcodec_close(codec_ctx);
                avcodec_free_context(&codec_ctx);
                preview_done = true;
                break;
            }
        }
    }
    //释放资源
    av_frame_free(&temp_frame);
    av_packet_free(&pkt);
    avformat_close_input(&fmt_ctx_);

    //使用该图片，贴图到textlabel
    if(preview_done){
//        ui->label->setPixmap(QPixmap::fromImage(preview));
        img  = preview;
        return true;
    }else{
        QMessageBox::warning(NULL,"","打开视频失败封面");
        this->setText("视频路径："+fileName);
        return false;
    }
    return false;
}
