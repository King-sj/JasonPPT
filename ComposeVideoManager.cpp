#include "ComposeVideoManager.h"
#include<QDir>
#include<QPainter>
//#include<opencv2/opencv.hpp>
#include<opencv2\imgproc\types_c.h>
//#if defined(_MSC_VER) && (_MSC_VER >= 1600)
//# pragma execution_character_set("utf-8")
//#endif
//#include<QtCore5Compat/QTextCodec>
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

//extern "C" {
//    #include <libavcodec/avcodec.h>
//    #include<libavformat/avformat.h>
//    #include<libavcodec/packet.h>
//    #include <libswscale/swscale.h>
//    #include <libavdevice/avdevice.h>
//    #include <libavformat/version.h>
//    #include <libavutil/time.h>
//    #include <libavutil/frame.h>
//    #include <libavutil/mathematics.h>
//    #include <libswresample/swresample.h>

//    #include "libavfilter/avfilter.h"
//    #include "libavutil/imgutils.h"
////    #include "libavutil/ffversion.h"
//    #include "libpostproc/postprocess.h"
//}

ComposeVideoManager::ComposeVideoManager(QObject *parent)
    : QObject{parent}
{
//    #if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
//    #if _MSC_VER
//    QTextCodec *codec = QTextCodec::codecForName("gbk");
//    #else
//    QTextCodec *codec = QTextCodec::codecForName("utf-8");
//    #endif
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForCStrings(codec);
//    QTextCodec::setCodecForTr(codec);
//    #else
//    QTextCodec *codec = QTextCodec::codecForName("utf-8");
//    QTextCodec::setCodecForLocale(codec);
//    #endif

}

bool ComposeVideoManager::composeNewVideo(QVector<VMItem *> imgForms, Dialog_VideoJParameterSetting::Parameters parameters)
{
    setlocale(LC_ALL,"utf-8");
    QString text;
    _parameters = parameters;
     perFrameCnt = _parameters.fps;
     w = _parameters.w, h = _parameters.h;
    fileAllName = QDir(_parameters.export_file_path+"/"+"result2.mp4").absolutePath();//note:it's a video without audio
    if(fileAllName.toStdString() == "" || fileAllName.toStdString().size() == 0 || !QDir(QDir(_parameters.export_file_path).absolutePath()).exists())
    {
        text = "合成视频路径:"+fileAllName+" 无效";
        emit signalProcessInformationText(text);
        return false;
    }

    text = "  总任务数量:"+QString::number(imgForms.size()-1)+" \n";
    emit signalProcessInformationText(text);
//    std::cout<<("begin composeVideo \n"+text).toStdString();
    emit signalNextStage("开始合成视频：");

    if(!initFfmpegWriter(fileAllName))
    {
        closeFfmpeg();
        emit signalProcessInformationText("写入视频失败");
        return false;
    }

//    CV_FOURCC()
//    vw = new VideoWriter();
//    cout<<fileAllName<<endl;


//    //自定义编码格式
//    int fourcc2 = cv::VideoWriter::fourcc('X','2','6','4');
//    qDebug()<<"自定义编码格式："<<fourcc2;

//    //实例视频保存对象
//    VideoWriter videoWriter("c:/opencv/cup2.mp4",fourcc2,_parameters.fps,Size(_parameters.w,_parameters.h),true);
//    if(!videoWriter.isOpened()){
//        cout<<"false";
//        return false;
//    }



//    vw->open(fileAllName.toStdString(),CAP_OPENCV_MJPEG
//              ,_parameters.fps,Size(_parameters.w,_parameters.h));
////    vw->open("D:/01temp.mp4",CAP_OPENCV_MJPEG
////              ,_parameters.fps,Size(_parameters.w,_parameters.h));
//    puts(fileAllName.toStdString().c_str());
//    cout<<fileAllName.toStdString().size();
//    if(!vw->isOpened())
//    {
//        text = "imgs2video:创建视频: "+fileAllName+"失败\n";
//        vw->release();
//        emit signalProcessInformationText(text);
////        std::cout<<text.toStdString();

//        return false;
//    }

//    int imgNum = 0;
    double proc = 1;//进度

//    int time = 0;
    for(const auto& file : imgForms)
    {
        emit signalProcess(proc/imgForms.size());

       if(file->getVMItem()->getType() == ImgForm::TYPE::IMG)
        {

            if(!writerImg(file))
            {
                text = "第"+QString::number(proc)+"项解码失败";
                emit signalProcessInformationText(text);
                //information::个别失败不影响总体的
//                return;
            }
        }else if(file->getVMItem()->getType() == ImgForm::TYPE::VIDEO){
            writerVideo(file);
        }
        proc++;
    }


    //    vw->release();
    //note:其他地方也需要关闭
    closeFfmpeg();
    text = "已合成无声视频！\n";
    emit signalProcessInformationText(text);
    // 处理完之后会在得到一个视频文件。

    emit signalNextStage("正在替换BGM\n");
//    std::cout<<std::string("正在替换BGM\n");

    //note:添加音频
//    if(musicFileName != "")
//    {
//        this->mergeVideoAndBgm(parameters.export_file_path+"/temp.mp4" , musicFileName);
//    }
//    else
//    {
//        QMessageBox::information(NULL,"","未添加音频");
//        return;
//    }
//    auto msb2 = QMessageBox();
    emit signalNextStage("替换BGM结束\n");
//    cout<<"替换BGM结束\n";

    return true;
}

void ComposeVideoManager::init()
{

}

bool ComposeVideoManager::writerImg(VMItem *img)
{
    int time;
    int fps = _parameters.fps;
    if(img->getVMItem()->getIsSpecial())
    {
        time = img->getVMItem()->deaultTime;

    }else{
        time = _parameters.img_time_default;

    }
    QString text;
    auto imgFileName = QDir(img->getVMItem()->getFileName()).absolutePath();
    if(! QFileInfo(imgFileName).isFile() )
    {
        emit signalProcessInformationText(imgFileName+" 不存在");
        return false;
    }
//    Mat image = imread(imgFileName);
//    QImage resultImg;//输出
//    if(image.empty())
//    {
//        text = "imgs2video："+QString::fromStdString(imgFileName)+"为空";
//        emit signalProcessInformationText(text);
//        return false;
//    }
//    //note:mat 不可以直接resize
//    resultImg = MatToQImage(image);
//    QImage resultImg(imgFileName);
////    emit signalProcessInformationText("\nwidth"+QString::number(resultImg.width()));
////    return false;
//    if(resultImg.width() == 0 || resultImg.height() == 0)
//    {
//        text = "imgs2video：帧为空\n";//error
////        cout<<text;
//        emit signalProcessInformationText(text);
//        return false;
//    }
    QImage resultImg(QDir(img->getVMItem()->getFileName()).absolutePath());
    if(resultImg.width() != _parameters.w || resultImg.height() != _parameters.h)
    {
        resultImg = getSuitableImage(resultImg);
    }


    Mat matImg = QImage2cvMat(resultImg);
//        char imgPath[] = "img/p0.jpg";
    //note:it's different
//        int index = 0;


//    matImg = imread(imgFileName.toStdString());
//    matImg = imread("C:\\Users\\SJ\\Pictures\\81925163_p0_master1200.jpg");

//    imshow("img", matImg);
    //note:it's different
//            imshow("img", img);
//            waitKey(0);
    memcpy(buffer, matImg.data, size);
//    memcpy()
    sws_scale(imgCtx,
        rgbFrame->data,
        rgbFrame->linesize,
        0,
        codecCtx->height,
        yuvFrame->data,
        yuvFrame->linesize);

    for (int j = 0; j < time*fps; j++) {
        text = "process:pos "+QString::number( CapPos)+"\n";
        emit signalProcessInformationText(text);
        yuvFrame->pts = CapPos++;
        //将解码的流程抽离成一个方法
        rgb2mp4Encode(codecCtx, yuvFrame, pkt, vStream, fmtCtx);
    }




//    int frameNum = time*fps;
//    for (int i = 0; i < frameNum; i++)
//    {
//        text = "process:"+QString::number(i)+QString::number(frameNum)+"\n";
////        cout<<text;
//        emit signalProcessInformationText(text);
//        if(!writeOneQImage(resultImg))
//        {
//            text = "process:"+QString::number(i)+QString::number(frameNum)+" generates failed\n";
//            emit signalProcessInformationText(text);
//        }
//        emit signalProcessInformationText(text);

////        *vw << QImage2cvMat(resultImg);

//    }
    return true;
}

bool ComposeVideoManager::writerVideo(VMItem *video)
{


//    for (size_t i = 0; i < imgNum; i++)
//    {
//        puts((folderName.toStdString() + QString::number(i).toStdString()+ String(".png")).c_str());

//        Mat image = imread(folderName.toStdString() + QString::number(i).toStdString()+ String(".png") );

//        if(image.empty())
//        {
//            QMessageBox::warning(NULL,"","imgs2video：帧为空");
//        }
//        //note:mat 不可以直接resize
//        if(image.size().width != _parameters.w || image.size().height != _parameters.h)
//        {
//            QMessageBox::warning(NULL,"","imgs2video：分辨率转换失败");//error
//        }
//        if(image.size().width == 0 || image.size().height == 0)
//        {
//            QMessageBox::warning(NULL,"","imgs2video：帧转换后为空");//error
//        }
//        vw << image;
//        try {
//            //防止最大值达不到
//            this->progressBar->setValue(static_cast<int>(i));
//            update();
//        } catch (...) {
//        }

//    }

    return false;
}

//bool ComposeVideoManager::writeOneQImage(QImage img)
//{
//    int ret = -1;
//    //8、写入每一帧数据
////    for(int i = 0; i<527; i++)
////    {
////        if(av_compare_ts(next_pts, kAVCodecContext->time_base,  10.0, time_1) > 0)      /*这里只生成10s的视频*/
////        {
////            qDebug()<<"暂时不需要生成新的帧"; break;
////        }
//    if (av_frame_make_writable(frame) < 0)  {return false;}  /*当我们传递一个帧给编码器时，它可能会在内部保留一个对它的引用;确保我们没有在这里覆盖它*/
////    if (av_frame_make_writable(tmp_frame) < 0)  {return false;}  /*当我们传递一个帧给编码器时，它可能会在内部保留一个对它的引用;确保我们没有在这里覆盖它*/
//    //note i could use
////    QImage img( QObject::tr("%1/%2.png").arg(folder_path).arg(i));
//    if(img.isNull()){
//        qDebug()<<"打开图片失败";
//        return false;
//    }
//    img = img.convertToFormat(QImage::Format_RGB888);

//    kSwsContext = sws_getContext(kAVCodecContext->width,kAVCodecContext->height,
//                                 AV_PIX_FMT_RGB24,kAVCodecContext->width,kAVCodecContext->height,
//                                 AV_PIX_FMT_YUV420P,SWS_LANCZOS | SWS_ACCURATE_RND,NULL,NULL,NULL);
//    if(!kSwsContext){emit signalProcessInformationText("无法初始化图片转换器");    return false;}


//    intBuffer = (uint8_t*)malloc(sizeof(uint8_t)*img.bytesPerLine()*img.height());
//    memcpy(intBuffer,img.constBits(),sizeof(uint8_t)*img.bytesPerLine()*img.height());

////        avpicture_fill((AVPicture *)tmp_frame,intBuffer,AV_PIX_FMT_RGB24,kAVCodecContext->width,kAVCodecContext->height); //pAVFrame32的data指针指向了outBuffer
////        avpicture_fill((AVFrame *)tmp_frame,intBuffer,AV_PIX_FMT_RGB24,kAVCodecContext->width,kAVCodecContext->height); //pAVFrame32的data指针指向了outBuffer
//    //note: that's I do，maybe have some error
//    av_image_fill_arrays(tmp_frame->data, tmp_frame->linesize, intBuffer, AV_PIX_FMT_RGB32,
//                         kAVCodecContext->width, kAVCodecContext->height, 1);
////note:don't scale
////    sws_scale(kSwsContext,(const uint8_t * const *)tmp_frame->data,
////              tmp_frame->linesize,0,kAVCodecContext->height,frame->data,frame->linesize);   //图片信息转换
//    sws_freeContext(kSwsContext);
//    kSwsContext = 0;

//    frame->pts = next_pts++;
////    tmp_frame->pts = frame->pts;
//    //frame->pts = i*(kAVStream->time_base.den)/((kAVStream->time_base.num)*24);

////        got_packer = 0;//now it's ret
////        ret = avcodec_encode_video2(kAVCodecContext,&kAVPacket,frame,&got_packer);  //8、编码、把一帧数据编码成AVPacket
//    ret = avcodec_send_frame(kAVCodecContext,frame);//note:it should be tmp_frame but frame
//    if(ret < 0) {emit signalProcessInformationText("发送视频帧失败:"+QString::number(ret)); return false;}
//    ret = 0;
//    ret = avcodec_receive_packet(kAVCodecContext, kAVPacket);
//    cout<<"ret:"<<ret<<endl;
//    if(ret)
//    {
//        emit signalProcessInformationText("done++");
//        cout<<"done";
//        av_packet_rescale_ts(kAVPacket, kAVCodecContext->time_base, kAVStream->time_base);
//        kAVPacket->stream_index = kAVStream->index;
//        ret = av_write_frame(kAVFormatContext, kAVPacket); /*将压缩帧写入媒体文件。 */
////            av_free_packet(kAVPacket);
//        av_packet_unref(kAVPacket);
//    }else
//    {ret = 0;}
//    free(intBuffer);intBuffer = 0;
//    if(ret<0){ qDebug()<<"写入video文件失败"<<ret<<kAVPacket->stream_index; return false; }
////    }
//    return true;
//}

QImage ComposeVideoManager::getSuitableImage(const QImage &img)
{
//    image.size().width != _parameters.w || image.size().height != _parameters.h
    QPixmap pix(_parameters.w,_parameters.h);
    QImage temp = img.scaled(QSize(_parameters.w,_parameters.h),Qt::KeepAspectRatio);
    QPainter painter(&pix);
    pix.fill(Qt::white);
    QRect  rect((_parameters.w - temp.width())/2.0,(_parameters.h - temp.height()/2.0),temp.width(),temp.height());
    painter.drawImage(rect,temp);
    return pix.toImage();
}

// Mat 转换成 QImage
QImage ComposeVideoManager::MatToQImage(const cv::Mat &inMat)
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
//            qWarning() << "CVS::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            break;
    }

    return QImage();
}

cv::Mat ComposeVideoManager::QImage2cvMat(QImage image)
{



//    Mat mat;
//	switch(image.format()){
//		case QImage::Format_RGB888:
//			mat = Mat(image.height(), image.width(),
//				CV_8UC3,(void*)image.constBits(),image.bytesPerLine());
//			break;
//		case QImage::Format_ARGB32_Premultiplied:
//			mat = Mat(image.height(), image.width(),
//				CV_8UC4,(void*)image.constBits(),image.bytesPerLine());
//			break;
//	}



    cv::Mat mat;
//    qDebug()<< image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
//        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
//        cv::cvtColor(mat, mat, CV_BGR2RGB);
        cout<<"no cv::cvtColor(mat, mat, CV_BGR2RGB);\n";
        mat = Mat(image.height(), image.width(),
        CV_8UC3,(void*)image.constBits(),image.bytesPerLine());

        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default:
        QString text = "未支持的格式" + QString::number(image.format());
        emit signalProcessInformationText(text);
        break;
    }
    return mat;
}
bool ComposeVideoManager::initFfmpegWriter(QString outFilename)
{
    if(!rgb2mp4())
        return false;

////    QString folder_path = QFileDialog::getExistingDirectory(this,"选择文件目录","",QFileDialog::ShowDirsOnly
////                                                            | QFileDialog::DontResolveSymlinks);
////    if(folder_path.isEmpty())   return;


////    QString outFilename = QFileDialog::getSaveFileName(this,"选择保存路径",folder_path+"/视频文件","*.mp4;; *.avi");
////    if(outFilename.isEmpty())   return;

////    QStringList filters;
////    filters<<QString("*.png");

////    定义迭代器并设置过滤器
////    QDirIterator dir_iterator(folder_path,filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
////    QList<QString > pnglist;
////    while(dir_iterator.hasNext())       //获取目录下的png文件
////    {
////        dir_iterator.next();
////        QFileInfo file_info = dir_iterator.fileInfo();
////        pnglist.append(QString(tr("%1").arg(file_info.absoluteFilePath())));
////    }


////    AVFormatContext *kAVFormatContext;
////    AVOutputFormat *kAVOutputFormat;
////    AVCodecContext *kAVCodecContext;
////    AVDictionary *kAVDictionary = NULL;
////    AVDictionary *opt = NULL;
////    AVCodec *video_codec;
////    AVStream *kAVStream;
////    AVFrame *frame;
////    AVFrame *tmp_frame;
////    AVPacket* kAVPacket ;//= { 0 };
////    struct SwsContext *kSwsContext;
////    AVRational time{1,24};  /*1s25帧*/
////    AVRational time_1{1,1};  /*1s25帧*/
////    uint8_t *intBuffer = 0;                           // 图片数据缓冲区

//    int ret;
////    int got_packer = 0;


//    tmp_frame = av_frame_alloc();
////    av_init_packet(&kAVPacket);
//    kAVPacket = av_packet_alloc();
////    av_packet_ref(kAVPacket,)

////    av_register_all();     //1、注册所有容易和编解码器/**/


//    //note:auto get codec type
////    const AVOutputFormat* output_format =
////              av_guess_format(nullptr, outFilename.toLocal8Bit().data() , nullptr);
////          if (!output_format) {
////            printf("output format not found\n");
////            emit signalProcessInformationText("output format not found\n");
////            return false;
////          }
//    /* 2、分配输出媒体上下文 */
//    avformat_alloc_output_context2(&kAVFormatContext, NULL, NULL, outFilename.toLocal8Bit().data());      //上下文结构体、输出结构体、文件类型、路径

//    if(!kAVFormatContext)        //如果根据文件名没有找到对应的格式则默认mpeg格式
//    {
//        ret = avformat_alloc_output_context2(&kAVFormatContext, NULL, "mpeg",  outFilename.toLocal8Bit().data());    //没有找到文件类型默认mpeg（MP4）
//    }
//    if(!kAVOutputFormat) {return false;}

//     kAVOutputFormat = const_cast<AVOutputFormat*>(kAVFormatContext->oformat);    //把输出操作结构体赋值出来

//    if (!(kAVOutputFormat->flags & AVFMT_NOFILE))   /* 如果需要，打开输出文件*/
//    {
//        ret = avio_open(&kAVFormatContext->pb, outFilename.toLocal8Bit().data(), AVIO_FLAG_WRITE);  //3、打开输出文件
//        if (ret < 0) {qDebug()<<"打开输出文件失败。"<<ret; }
//    }

//    /* 使用默认格式的编解码器添加音频和视频流，并初始化编解码器。*/
//    if (kAVOutputFormat->video_codec != AV_CODEC_ID_NONE)
//    {
//        video_codec = const_cast<AVCodec*>(avcodec_find_encoder(kAVOutputFormat->video_codec));   //4、找到编码器
//        if(!video_codec)
//        {
//            emit signalProcessInformationText("创建编码器失败");
//            return false;
//        }

//        kAVStream = avformat_new_stream(kAVFormatContext,NULL);         //5、新建一个输出流
//        if(!kAVStream) {emit signalProcessInformationText("创建流AVStream失败。");return false;}

//        kAVCodecContext = avcodec_alloc_context3(video_codec);      //初始化一个AVCodecContext
//        if(!kAVCodecContext)    {emit signalProcessInformationText("用编码器video_codec初始化的AVCodecContext默认参数失败");return false;}

//        switch(video_codec->type)
//        {
//        case AVMEDIA_TYPE_VIDEO:
//            emit signalProcessInformationText("开始处理 video");
//            kAVCodecContext->codec_id = video_codec->id;
//            kAVCodecContext->bit_rate = 800000;
//            kAVCodecContext->width = _parameters.w;
//            kAVCodecContext->height = _parameters.h;
//            kAVStream->time_base = time;
//            kAVCodecContext->time_base = time;
//            kAVCodecContext->gop_size = 12; /*最多每十二帧发射一个内帧 */
//            kAVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

////             kAVCodecContext->time_base.num = 1;
////             kAVCodecContext->time_base.den = 25;
////             kAVCodecContext->framerate.den = 1;
////             kAVCodecContext->keyint_min = 100;
////             kAVCodecContext->qmin = 10;
////             kAVCodecContext->qmax = 36;
//////             kAVCodecContext->has_b_frames = 0;

////             av_opt_set(kAVCodecContext->priv_data, "profile", "high", 0);
////             av_opt_set(kAVCodecContext->priv_data, "preset", "slow", 0);
////             av_opt_set(kAVCodecContext->priv_data, "level", "5", 0);
////             av_opt_set(kAVCodecContext->priv_data, "tune", "zerolatency", 0);


////            if (kAVCodecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO)
////            {
////                kAVCodecContext->max_b_frames = 2;  /*为了测试，我们还添加了b帧*/
////            }
//            if (kAVCodecContext->codec_id == AV_CODEC_ID_MPEG1VIDEO)
//            {
//                /* 需要避免使用一些coeffs溢出的宏块。这在正常的视频中不会发生，
//                 * 只是在色度平面的运动与luma平面不匹配时才会发生。 */
//                kAVCodecContext->mb_decision = 2;
//            }

//            break;
//        case AVMEDIA_TYPE_AUDIO:
//            break;

//        default:
//            break;
//        }

//        if(kAVOutputFormat->flags & AVFMT_GLOBALHEADER)/*有些格式希望流标头是单独的*/
//        {
//            kAVCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//        }
//    }

//    av_dict_copy(&opt,kAVDictionary, 0);//bug:两个参数均为空
//    ret = avcodec_open2(kAVCodecContext,video_codec,&opt);      //6、打开编码器
//    if(ret<0){emit signalProcessInformationText("打开视频编解码器失败"+QString::number(ret)); return false;}
//    av_dict_free(&opt);

//    frame = av_frame_alloc();
//    if(!frame)  {emit signalProcessInformationText("分配帧失败.");return false;}
//    frame->format = kAVCodecContext->pix_fmt;
//    frame->width = kAVCodecContext->width;
//    frame->height = kAVCodecContext->height;

//    ret = av_frame_get_buffer(frame, 32);   //分配内存空间frame必须要有上三条件，32也就是4字节对齐
//    if(ret < 0){emit signalProcessInformationText("frame分配内存失败");return false;}

//    ret = avcodec_parameters_from_context(kAVStream->codecpar,kAVCodecContext);/*将流参数复制到muxer */
//    if (ret < 0) {emit signalProcessInformationText("无法复制流参数");return false; }

//    av_dump_format(kAVFormatContext, 0, outFilename.toLocal8Bit().data(), 1);     //打印输出文件的详细信息



//    ret = avformat_write_header(kAVFormatContext,&kAVDictionary);/* 7、写流标头(如果有的话)*/
//    if(ret < 0){emit signalProcessInformationText("写流标题失败");return false;}


////    //8、写入每一帧数据
////    for(int i = 0; i<527; i++)
////    {
//////        if(av_compare_ts(next_pts, kAVCodecContext->time_base,  10.0, time_1) > 0)      /*这里只生成10s的视频*/
//////        {
//////            qDebug()<<"暂时不需要生成新的帧"; break;
//////        }
////        if (av_frame_make_writable(frame) < 0)  {goto end;}  /*当我们传递一个帧给编码器时，它可能会在内部保留一个对它的引用;确保我们没有在这里覆盖它*/
////        //note i could use
////        QImage img( QObject::tr("%1/%2.png").arg(folder_path).arg(i));
////        if(img.isNull()){qDebug()<<"打开图片失败";break;}
////        img = img.convertToFormat(QImage::Format_RGB888);

////        kSwsContext = sws_getContext(kAVCodecContext->width,kAVCodecContext->height,
////                                     AV_PIX_FMT_RGB24,kAVCodecContext->width,kAVCodecContext->height,
////                                     AV_PIX_FMT_YUV420P,SWS_LANCZOS | SWS_ACCURATE_RND,NULL,NULL,NULL);
////        if(!kSwsContext){qDebug()<<"无法初始化图片转换器";    goto end;}


////        intBuffer = (uint8_t*)malloc(sizeof(uint8_t)*img.bytesPerLine()*img.height());
////        memcpy(intBuffer,img.constBits(),sizeof(uint8_t)*img.bytesPerLine()*img.height());

//////        avpicture_fill((AVPicture *)tmp_frame,intBuffer,AV_PIX_FMT_RGB24,kAVCodecContext->width,kAVCodecContext->height); //pAVFrame32的data指针指向了outBuffer
//////        avpicture_fill((AVFrame *)tmp_frame,intBuffer,AV_PIX_FMT_RGB24,kAVCodecContext->width,kAVCodecContext->height); //pAVFrame32的data指针指向了outBuffer
////        //note: that's I do，maybe have some error
////        av_image_fill_arrays(tmp_frame->data, tmp_frame->linesize, intBuffer, AV_PIX_FMT_RGB32,
////                             kAVCodecContext->width, kAVCodecContext->height, 1);
////        sws_scale(kSwsContext,(const uint8_t * const *)tmp_frame->data,
////                  tmp_frame->linesize,0,kAVCodecContext->height,frame->data,frame->linesize);   //图片信息转换
////        sws_freeContext(kSwsContext);
////        kSwsContext = 0;

////        frame->pts = next_pts++;
////        //frame->pts = i*(kAVStream->time_base.den)/((kAVStream->time_base.num)*24);

//////        got_packer = 0;//now it's ret
//////        ret = avcodec_encode_video2(kAVCodecContext,&kAVPacket,frame,&got_packer);  //8、编码、把一帧数据编码成AVPacket
////        ret = avcodec_send_frame(kAVCodecContext,frame);
////        if(ret < 0) {qDebug()<<"发送视频帧失败"<<ret; goto end;}
////        ret = 0;
////        ret = avcodec_receive_packet(kAVCodecContext, kAVPacket);
//////        if(ret < 0) {qDebug()<<"编码视频帧失败"<<ret; goto end;}//note:need rewrite
//////        if(ret < 0) {qDebug()<<"错误编码视频帧"<<ret; goto end;}

////        if(ret != 0)
////        {
////            av_packet_rescale_ts(kAVPacket, kAVCodecContext->time_base, kAVStream->time_base);
////            kAVPacket->stream_index = kAVStream->index;
////            ret = av_write_frame(kAVFormatContext, kAVPacket); /*将压缩帧写入媒体文件。 */
//////            av_free_packet(kAVPacket);
////            av_packet_unref(kAVPacket);
////        }else
////        {ret = 0;}
////        free(intBuffer);intBuffer = 0;
////        if(ret<0){ qDebug()<<"写入video文件失败"<<ret<<kAVPacket->stream_index; break; }
////    }

////    /*  选择写预告片，如果有的话。预告片必须在你之前写好 当你写标题时关闭CodecContexts打开;否则
////        av_write_trailer()可以尝试使用被释放的内存  av_codec_close()。要编码的流 */
////    av_write_trailer(kAVFormatContext);
//    emit signalProcessInformationText("create outFile successfully");
//    return true;
}

void ComposeVideoManager::closeFfmpeg()
{



    try {
        if(codecCtx)
        {
            rgb2mp4Encode(codecCtx, NULL, pkt, vStream, fmtCtx);
            av_write_trailer(fmtCtx);
            av_free(buffer);
            av_free(yuvBuffer);
            sws_freeContext(imgCtx);
        }
    } catch (...) {

    }

    try {
        av_packet_free(&pkt);

        if (fmtCtx)
            avformat_free_context(fmtCtx);

        if (codecCtx)
            avcodec_free_context(&codecCtx);

        if (rgbFrame)
            av_frame_free(&rgbFrame);
        if (yuvFrame)
            av_frame_free(&yuvFrame);
    } catch (...) {
    }

//    /* 关闭每个编解码器。 */
//    avcodec_free_context(&kAVCodecContext);
//    av_frame_free(&frame);

//    if (!(kAVOutputFormat->flags & AVFMT_NOFILE))
//        avio_closep(&kAVFormatContext->pb);/*关闭输出文件*/


//    avformat_free_context(kAVFormatContext);   /*释放上下文*/
}

//--------------------------------------------------------------------------------------------------//
//刷新解码缓冲区，包数据的处理方式一致
/*
int flush_encoder(AVFormatContext* fmtCtx, AVCodecContext* codecCtx, int vStreamIndex) {
    int ret;
    AVPacket* pkt = av_packet_alloc();
    pkt->data = NULL;
    pkt->size = 0;

    if (!(codecCtx->codec->capabilities & AV_CODEC_CAP_DELAY)) {
        av_packet_free(&pkt);
        return 0;
    }

    cout << "Flushing stream " << vStreamIndex << " encoder" << endl;

    if ((ret = avcodec_send_frame(codecCtx, 0)) >= 0) {
        while (avcodec_receive_packet(codecCtx, pkt) >= 0) {
            cout << "encoder success:" << pkt->size << endl;

            pkt->stream_index = vStreamIndex;
            av_packet_rescale_ts(pkt, codecCtx->time_base,
                fmtCtx->streams[vStreamIndex]->time_base);
            ret = av_interleaved_write_frame(fmtCtx, pkt);
            if (ret < 0) {
                break;
            }
        }
    }

    av_packet_free(&pkt);
    return ret;
}*/

int ComposeVideoManager::rgb2mp4Encode(AVCodecContext* codecCtx, AVFrame* yuvFrame, AVPacket* pkt, AVStream* vStream, AVFormatContext* fmtCtx){
    int ret = 0;
    if (avcodec_send_frame(codecCtx, yuvFrame) >= 0) {
        while (avcodec_receive_packet(codecCtx, pkt) >= 0) {
//            cout << "encoder success:" << pkt->size << endl;
            //note:it's different
            pkt->stream_index = vStream->index;
            pkt->pos = -1;

            av_packet_rescale_ts(pkt, codecCtx->time_base, vStream->time_base);
            cout << "encoder success:" << pkt->size << endl;

            ret = av_interleaved_write_frame(fmtCtx, pkt);
            if (ret < 0) {
                char errStr[256];
                av_strerror(ret, errStr, 256);
                cout << "error is:" << errStr << endl;
            }
        }
    }
    return ret;
}

bool ComposeVideoManager::rgb2mp4(){
//    int ret = -1;
//    AVFormatContext* fmtCtx = NULL;
//    AVCodecContext* codecCtx = NULL;
//    AVCodec* codec = NULL;
//    AVStream* vStream = NULL;
//    AVPacket* pkt = av_packet_alloc();

//    AVFrame* rgbFrame = NULL, * yuvFrame = NULL;

//    int w = _parameters.w, h = _parameters.h, perFrameCnt = _parameters.fps;

    do {
        //输出文件名 从.h264后缀改为mp4
//        const char* filePath = "result2.mp4";
//        string str(fileAllName.toStdString());
//        const char* filePath = str.c_str();
        string filePath = fileAllName.toStdString();
//        cout<<QString::fromLocal8Bit(filePath)<endl;
        ret = avformat_alloc_output_context2(&fmtCtx, NULL, NULL, filePath.c_str());
        if (ret < 0) {
            cout << "Cannot alloc output file context" << endl;
            char errStr[256];
            av_strerror(ret, errStr, 256);
            cout << "error is:" << errStr << endl;
            break;
        }

        ret = avio_open(&fmtCtx->pb, filePath.c_str(), AVIO_FLAG_READ_WRITE);
        if (ret < 0) {
            cout << "output file open failed" << endl;
            break;
        }

        codec = const_cast<AVCodec*>(avcodec_find_encoder(AV_CODEC_ID_H264));

        if (codec == NULL) {
            cout << "Cannot find any endcoder" << endl;
            break;
        }

        codecCtx = avcodec_alloc_context3(codec);
        if (codecCtx == NULL) {
            cout << "Cannot alloc AVCodecContext" << endl;
            break;
        }

        vStream = avformat_new_stream(fmtCtx, codec);
        if (vStream == NULL) {
            cout << "failed create new video stream" << endl;
            break;
        }

        vStream->time_base = AVRational{ 1,perFrameCnt };
        //note:it's different
        AVCodecParameters* param = vStream->codecpar;
        param->width = w;
        param->height = h;
        //note:it's different
        param->codec_type = AVMEDIA_TYPE_VIDEO;

        ret = avcodec_parameters_to_context(codecCtx, param);
        if (ret < 0) {
            cout << "Cannot copy codec para" << endl;
            break;
        }

        codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
        codecCtx->time_base = AVRational{ 1,perFrameCnt };
        codecCtx->bit_rate = 400000;
        codecCtx->gop_size = 12;

        // 某些封装格式必须要设置该标志，否则会造成封装后文件中信息的缺失,如：mp4
        if (fmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
            codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (codec->id == AV_CODEC_ID_H264) {
            codecCtx->qmin = 10;
            codecCtx->qmax = 51;
            codecCtx->qcompress = (float)0.6;
        }
        //note:it's different

        ret = avcodec_open2(codecCtx, codec, NULL);
        if (ret < 0) {
            cout << "Open encoder failed" << endl;
            char errStr[256];
            av_strerror(ret, errStr, 256);
            cout << "error is:" << errStr << endl;
            break;
        }
//        //note:it's me add
//         av_dump_format(fmtCtx, 0, outFile, 1);

        //note:it's different
        //再将codecCtx设置的参数传给param，用于写入头文件信息
        avcodec_parameters_from_context(param, codecCtx);

        rgbFrame = av_frame_alloc();
        yuvFrame = av_frame_alloc();

        yuvFrame->width = w;
        yuvFrame->height = h;
        yuvFrame->format = codecCtx->pix_fmt;

        rgbFrame->width = w;
        rgbFrame->height = h;
        rgbFrame->format = AV_PIX_FMT_BGR24;

        size = av_image_get_buffer_size((AVPixelFormat)rgbFrame->format, w, h, 1);
//        uint8_t*
        buffer = (uint8_t*)av_malloc(size);
        ret = av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize,
                                   buffer, (AVPixelFormat)rgbFrame->format,
                                   w, h, 1);
        if (ret < 0) {
            cout << "Cannot filled rgbFrame" << endl;
            break;
        }

        yuvSize = av_image_get_buffer_size((AVPixelFormat)yuvFrame->format, w, h, 1);
        //note: it's me change ,size->yuvSize
//        uint8_t*
        yuvBuffer = (uint8_t*)av_malloc(yuvSize);
        ret = av_image_fill_arrays(yuvFrame->data, yuvFrame->linesize, yuvBuffer, (AVPixelFormat)yuvFrame->format, w, h, 1);
        if (ret < 0) {
            cout << "Cannot filled yuvFrame" << endl;
            break;
        }
        //note:it's different
//        SwsContext* imgCtx = sws_getContext(w, h, AV_PIX_FMT_BGR24,
//                                            w, h, codecCtx->pix_fmt,
//                                            0, NULL, NULL, NULL);
        imgCtx = sws_getContext(w, h, AV_PIX_FMT_BGR24,
                                            w, h, codecCtx->pix_fmt,
                                            0, NULL, NULL, NULL);

        ret = avformat_write_header(fmtCtx, NULL);
        if (ret != AVSTREAM_INIT_IN_WRITE_HEADER) {
            cout << "Write file header fail" << endl;
            break;
        }

        av_new_packet(pkt, size);
//note:以下需要修改
//        Mat img;
//        char imgPath[] = "img/p0.jpg";
//        //note:it's different
////        int index = 0;

//        for (int i = 0; i < 7; i++) {
//            imgPath[5] = '0' + i;
//            img = imread(imgPath);
//            //note:it's different
////            imshow("img", img);
////            waitKey(0);
//            memcpy(buffer, img.data, size);

//            sws_scale(imgCtx,
//                rgbFrame->data,
//                rgbFrame->linesize,
//                0,
//                codecCtx->height,
//                yuvFrame->data,
//                yuvFrame->linesize);

//            for (int j = 0; j < perFrameCnt; j++) {
//                yuvFrame->pts = i * perFrameCnt + j;//note:should use ++ from 0
//                //将解码的流程抽离成一个方法
//                rgb2mp4Encode(codecCtx, yuvFrame, pkt, vStream, fmtCtx);
//            }
//        }

//        rgb2mp4Encode(codecCtx, NULL, pkt, vStream, fmtCtx);

//        av_write_trailer(fmtCtx);

//        av_free(buffer);
//        av_free(yuvBuffer);
//        sws_freeContext(imgCtx);
        return true;
    } while (0);
    return false;

//    av_packet_free(&pkt);

//    if (fmtCtx)
//        avformat_free_context(fmtCtx);

//    if (codecCtx)
//        avcodec_free_context(&codecCtx);

//    if (rgbFrame)
//        av_frame_free(&rgbFrame);
//    if (yuvFrame)
//        av_frame_free(&yuvFrame);

}
//
//void rgb2h264() {
//    int ret = -1;

//    //声明所需的变量名
//    AVFormatContext* fmtCtx = NULL;
//    AVCodecContext* codecCtx = NULL;
//    AVStream* vStream = NULL;
//    AVCodec* codec = NULL;

//    AVPacket* pkt = av_packet_alloc();

//    AVFrame* rgbFrame = NULL;
//    AVFrame* yuvFrame = NULL;

//    //需要编码的视频宽高、每幅图像所占帧数
//    int w = 600, h = 900, perFrameCnt = 25;

//    do {
//        //输出文件名
//        const char* outFile = "result.h264";

//        //----------------- 打开输出文件 -------------------
//        //创建输出结构上下文 AVFormatContext,会根据文件后缀创建相应的初始化参数
//        if (avformat_alloc_output_context2(&fmtCtx, NULL, NULL, outFile) < 0) {
//            cout << "Cannot alloc output file context" << endl;
//            break;
//        }

//        //打开文件
//        if (avio_open(&fmtCtx->pb, outFile, AVIO_FLAG_READ_WRITE) < 0) {
//            cout << "output file open failed" << endl;
//            break;
//        }

//        //----------------- 查找编码器 -------------------
//        //查找codec有三种方法
//        /*1.AVFormatContext的oformat中存放了对应的编码器类型
//        AVOutputFormat* outFmt = fmtCtx->oformat;
//        codec = avcodec_find_encoder(outFmt->video_codec);
//        */
//        /*2.根据编码器名称去查找
//        codec = avcodec_find_encoder_by_name("libx264");
//        */
//        //3.根据编码器ID查找
//        codec = const_cast<AVCodec*>(avcodec_find_encoder(AV_CODEC_ID_H264));

//        if (codec == NULL) {
//            cout << "Cannot find any endcoder" << endl;
//            break;
//        }

//        //----------------- 申请编码器上下文结构体 -------------------
//        codecCtx = avcodec_alloc_context3(codec);
//        if (codecCtx == NULL) {
//            cout << "Cannot alloc context" << endl;
//            break;
//        }

//        //----------------- 创建视频流，并设置参数 -------------------
//        vStream = avformat_new_stream(fmtCtx, codec);
//        if (vStream == NULL) {
//            cout << "failed create new video stream" << endl;
//            break;
//        }

//        //设置时间基，25为分母，1为分子，表示以1/25秒时间间隔播放一帧图像
//        vStream->time_base = AVRational{ 1,25 };
//        /*两种设置方法等价
//        vStream->time_base.den = 25;
//        vStream->time_base.num = 1;
//        */

//        //设置编码所需的参数
//        AVCodecParameters* param = fmtCtx->streams[vStream->index]->codecpar;
//        param->codec_type = AVMEDIA_TYPE_VIDEO;
//        param->width = w;
//        param->height = h;

//        //----------------- 将参数传给解码器上下文 -------------------
//        avcodec_parameters_to_context(codecCtx, param);

//        //视频帧类型，使用YUV420P格式
//        codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
//        codecCtx->time_base = AVRational{ 1,25 };
//        codecCtx->bit_rate = 400000;
//        //gop表示多少个帧中存在一个关键帧
//        codecCtx->gop_size = 12;

//        //H264-设置量化步长范围
//        if (codecCtx->codec_id == AV_CODEC_ID_H264) {
//            codecCtx->qmin = 10;
//            codecCtx->qmax = 51;
//            //(0~1.0),0=>CBR 1->恒定QP
//            codecCtx->qcompress = (float)0.6;
//        }

//        if (codecCtx->codec_id == AV_CODEC_ID_MPEG2VIDEO)
//            codecCtx->max_b_frames = 2;
//        if (codecCtx->codec_id == AV_CODEC_ID_MPEG1VIDEO)
//            codecCtx->mb_decision = 2;

//        //----------------- 打开解码器 -------------------
//        if (avcodec_open2(codecCtx, codec, NULL) < 0) {
//            cout << "Open encoder failed" << endl;
//            break;
//        }

//        av_dump_format(fmtCtx, 0, outFile, 1);

//        //设置视频帧参数
//        rgbFrame = av_frame_alloc();
//        yuvFrame = av_frame_alloc();
//        rgbFrame->width = codecCtx->width;
//        yuvFrame->width = codecCtx->width;
//        rgbFrame->height = codecCtx->height;
//        yuvFrame->height = codecCtx->height;
//        rgbFrame->format = AV_PIX_FMT_BGR24;
//        yuvFrame->format = codecCtx->pix_fmt;

//        int size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, codecCtx->width, codecCtx->height, 1);
//        int yuvSize = av_image_get_buffer_size(codecCtx->pix_fmt, codecCtx->width, codecCtx->height, 1);

//        uint8_t* pictureBuf = (uint8_t*)av_malloc(size);
//        uint8_t* yuvBuf = (uint8_t*)av_malloc(yuvSize);

//        av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize,
//            pictureBuf, AV_PIX_FMT_BGR24,
//            codecCtx->width, codecCtx->height, 1);

//        av_image_fill_arrays(yuvFrame->data, yuvFrame->linesize,
//            yuvBuf, codecCtx->pix_fmt,
//            codecCtx->width, codecCtx->height, 1);

//        //设置BGR数据转换为YUV的SwsContext
//        struct SwsContext* imgCtx = sws_getContext(
//            codecCtx->width, codecCtx->height, AV_PIX_FMT_BGR24,
//            codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
//            SWS_BILINEAR, NULL, NULL, NULL);

//        //写入文件头信息
//        ret = avformat_write_header(fmtCtx, NULL);
//        if (ret != AVSTREAM_INIT_IN_WRITE_HEADER) {
//            cout << "Write file header fail" << endl;
//            break;
//        }

//        av_new_packet(pkt, size);

//        //这里使用OpenCV读取图像的数据，当然FFmpeg也可以读取图像数据，会略微麻烦一些之后会举例
//        Mat img;
//        char imgPath[] = "img/p0.jpg";
//        for (int i = 0; i < 6; i++) {
//            imgPath[5] = '0' + i;
//            img = imread(imgPath);
//            //imshow("img", img);
//            //waitKey(0);
//            //----------------- BGR数据填充至图像帧 -------------------
//            memcpy(pictureBuf, img.data, size);

//            //进行图像格式转换
//            sws_scale(imgCtx,
//                rgbFrame->data,
//                rgbFrame->linesize,
//                0,
//                codecCtx->height,
//                yuvFrame->data,
//                yuvFrame->linesize);

//            for (int j = 0; j < perFrameCnt; j++) {
//                //设置 pts 值，用于度量解码后视频帧位置
//                yuvFrame->pts = i * perFrameCnt + j;
//                //解码时为 avcodec_send_packet ，编码时为 avcodec_send_frame
//                if (avcodec_send_frame(codecCtx, yuvFrame) >= 0) {
//                    //解码时为 avcodec_receive_frame ，编码时为 avcodec_receive_packet
//                    while (avcodec_receive_packet(codecCtx, pkt) >= 0) {
//                        cout << "encoder success:" << pkt->size << endl;
//                        pkt->stream_index = vStream->index;
//                        //将解码上下文中的时间基同等转换为流中的时间基
//                        av_packet_rescale_ts(pkt, codecCtx->time_base, vStream->time_base);
//                        //pos为-1表示未知，编码器编码时进行设置
//                        pkt->pos = -1;
//                        //将包数据写入文件中
//                        ret = av_interleaved_write_frame(fmtCtx, pkt);
//                        if (ret < 0) {
//                            cout << "error is:" << ret << endl;
//                        }
//                    }
//                }
//            }
//        }

//        //刷新解码缓冲区
//        ret = flush_encoder(fmtCtx, codecCtx, vStream->index);
//        if (ret < 0) {
//            printf("flushing encoder failed!\n");
//            break;
//        }

//        //向文件中写入文件尾部标识，并释放该文件
//        av_write_trailer(fmtCtx);

//        av_free(pictureBuf);
//        av_free(yuvBuf);
//        sws_freeContext(imgCtx);
//    } while (0);

//    //释放资源
//    av_packet_free(&pkt);
//    avcodec_close(codecCtx);

//    if (rgbFrame)
//        av_frame_free(&rgbFrame);
//    if (yuvFrame)
//        av_frame_free(&yuvFrame);

//    if (fmtCtx) {
//        avio_close(fmtCtx->pb);
//        avformat_free_context(fmtCtx);
//    }
//}
