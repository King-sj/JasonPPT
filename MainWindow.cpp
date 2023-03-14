#pragma execution_character_set("utf-8")

#include "MainWindow.h"
#include<QDir>
#include<stdio.h>
#include<QMessageBox>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<Share.h>
//#include<QTest>
#include<QFileDialog>
#include<QMimeType>
#include<QMimeDatabase>

#include<QStringList>
#include<QFile>
#include<QTextStream>



using namespace std;
using namespace cv;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setupUi(this);
//    initFFMPEG();

//    av_register_all();//已弃用
//    avformat_network_init();//not need

    /*
     * 放setupUI后创建，否则无反应
    */
    this->showMaximized();

    setFocus();//获取焦点
    vector_ImgForm.clear();
    vedioParameEdit = nullptr;
    playVideo = NULL;
    init();
    rebuiltFolder();
//    cout<<"mainWindow:"<<QThread::currentThreadId()<<endl;

    /*
//     * 多线程播放视频
//    */
//    playVideoThread = new QThread();
//    playVideo = new PlayVideo();
//    playVideo->moveToThread(playVideoThread);
//    playVideoThread->start();
//    //线程结束事件
//    connect(playVideoThread,&QThread::finished,playVideo,&QObject::deleteLater);//销毁对象
//    connect(playVideoThread,&QThread::finished,playVideoThread,&QThread::deleteLater);//多线程自销毁
//    //启动线程类的任何方法都应该通过信号-槽

//    this->progressBar->hide();
    this->progressBar->setVisible(false);
    this->progressBar->setTextVisible(false);
    this->progressBar->setMinimum(0);

    process = nullptr;
}

MainWindow::~MainWindow()
{
    if(vedioParameEdit)
        delete vedioParameEdit;
    vedioParameEdit = nullptr;

//    /*
//     * 结束线程
//    */
//    if(playVideoThread)
//    {
//        playVideoThread->quit();
//    }
//    playVideoThread->wait();
    //不应该手动删除线程    QString targetBatFileName = QDir("./temp.bat").absolutePath();
//    addPauseToBat();
}

void MainWindow::init()
{
    QPointer<ImgForm> temp = getNewImgForm();
    vector_ImgForm.append(temp);
//    connect(vector_ImgForm[0] , &ImgForm::loadDone , this , &MainWindow::dealImgFormSignal_loadDone);

//    connect(vector_ImgForm[0] , &ImgForm::signal_delete , this , &MainWindow::dealImgFormSignal_loadDone);

}

inline QPoint MainWindow::getCenterPointForRectRegion(int width, int height)
{
    return QPoint( this->width()/2.0 - width/2.0,this->height()/2.0 - height/2.0);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    auto centerP = getCenterPointForRectRegion(imgWidth,imgHeight);
    this->label->move(getCenterPointForRectRegion(this->label->width(),0).x(),0);
    //右边的
    for(auto& img : vector_ImgForm)
    {
        img->hide();
    }
    for(int i = pos ; i < vector_ImgForm.size() ; i++)
    {
        vector_ImgForm[i]->move(centerP.x() + (i - pos)*(imgWidth+interval) + border, centerP .y());
        if(vector_ImgForm[i]->geometry().x() > this->width())
            break;
        vector_ImgForm[i]->show();
    }
    //左边的
    for(int i = pos - 1 ; i >= 0 ; i--)
    {
        vector_ImgForm[i]->move(centerP.x() + (i - pos)*(imgWidth+interval) + border , centerP .y());
        if(vector_ImgForm[i]->geometry().y() < -imgWidth)
            break;
        vector_ImgForm[i]->show();
    }
    e->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        rightKeyMenuePos = e->pos();
        initRightKeyMenu();

    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Left)
    {
        if(pos > 0)
            pos--;
    }
    if(e->key() == Qt::Key_Right)
    {
        if(pos < vector_ImgForm.size() - 1)
            pos++;
    }
    //cout<<"pos:"<<pos<<" imgNum: "<<imgNum<<"\n";
    this->label->setText(QString::number(pos+1)+"/"+QString::number(vector_ImgForm.size()-1));
    e->accept();
}

void MainWindow::videoInfo(const char *fileName)
{
    int ret = 0;
//    av_register_all();
    AVFormatContext *format_ctx = avformat_alloc_context();

    ret = avformat_open_input(&format_ctx,fileName,NULL,NULL);
    if (ret){
        cout << "open failed : " << ret << endl;
        return;
    }
    const AVInputFormat *input_fmt = format_ctx->iformat;
    //获取视频的格式名字
    cout<< "name:" << input_fmt->name << endl;
    cout<< "long name:" << input_fmt->long_name << endl;
    AVStream **stream = (format_ctx->streams);
    //获取视频的时长
    cout<< "shipin shichang :" << format_ctx->duration << "us == " <<
                 format_ctx->duration / 1000000 << "s" << endl;
    int den = (*stream)->avg_frame_rate.den;
    int num = (*stream)->avg_frame_rate.num;
    cout<< "shipin zhensulv den:" <<  den << endl;
    cout<< "shipin zhensulv num:" <<  num << endl;
    //获取视频的帧率
    float rate = (float)num/(float)den;
    cout<< "shipin zhensulv    :" <<  rate << endl;

    int msnum = (format_ctx->duration % 1000000) / 1000;
    cout<< "shipin msnum  :" <<  msnum << endl;
    int yuzhen =  (int)(msnum * 1/rate);
    cout<< "shipin yuzhensu  :" <<  yuzhen << endl;
    //获取视频的总帧数
    cout<< "shipin zongzhensu  :" <<  (format_ctx->duration / 1000000) * (int)rate + yuzhen << endl;

    avformat_close_input(&format_ctx);
    return;
}

int MainWindow::getVideoFPS(const char *fileName)
{
    int ret = 0;
    AVFormatContext* fmctx = avformat_alloc_context();
    ret = avformat_open_input(&fmctx,fileName,NULL,NULL);
    if(ret)
    {
        QMessageBox::warning(NULL,"","failed to open video :"+QString::number(ret));
        return -1;
    }
    AVStream **stream = (fmctx->streams);
    int den = (*stream)->avg_frame_rate.den;
    int num = (*stream)->avg_frame_rate.num;
    //获取视频的帧率
    float rate = (float)num/(float)den;
    return static_cast<int>(rate);
}

void MainWindow::initFFMPEG()
{
    cout << "av_version_info:"<< av_version_info();
    unsigned version = avcodec_version();
//    QString ch = QString::number(version,10);
    cout<<"version: "<<version<<"\n配置信息："<<avcodec_configuration();

    int videoStreamIndex=-1;
    int audioStreamIndex=-1;
    uint i;

    AVFormatContext* avFormatContext = avformat_alloc_context();//申請一個AVFormatContext結構的内存，並進行簡單初始化
    //打開視頻流
    //note:just test
    int ret=avformat_open_input(&avFormatContext,"C:\\Users\\SJ\\Videos\\23a.mp4",NULL,NULL);
    if(ret!=0){
        QMessageBox::warning(NULL,"","打开视频失败");
         avformat_free_context(avFormatContext);
    }
//    cout<<ret<<endl;
    //讀取流數據包並獲取相關信息
     if(avformat_find_stream_info(avFormatContext,NULL)<0){
          QMessageBox::warning(NULL,"","获取视频信息失败");;
          avformat_close_input(&avFormatContext);
         }

    cout<<"avFormatContext->nb_streams"<<avFormatContext->nb_streams<<endl;
     for(i=0;i<avFormatContext->nb_streams;i++){
         //確定流格式是否為視頻
         if(avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
             videoStreamIndex=i;
         }
         if(videoStreamIndex==-1){
             avformat_close_input(&avFormatContext);
             QMessageBox::warning(NULL,"","获取视频流索引失败");;
         }
         //note:don't need audio part until
//         //確定流格式是否為音頻
//         if(avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
//             audioStreamIndex=i;
//         }
     }
     //视频部分处理
     //根據編碼器ID獲取視頻劉解碼器
     const AVCodec* videoCodec = avcodec_find_decoder(avFormatContext->streams[videoStreamIndex]->codecpar->codec_id);
     if(videoCodec==NULL){
         QMessageBox::warning(NULL,"","尋找视频解码器失败");
     }

     //获取视频编解码器上下文信息
     AVCodecContext* videoCodecContext = avcodec_alloc_context3(videoCodec);
     if(videoCodecContext==NULL){
         avformat_close_input(&avFormatContext);
         QMessageBox::warning(NULL,"","获取上下文信息失败");
     }
     //拷贝视频上下文信息
     int ret1=avcodec_parameters_to_context(videoCodecContext,avFormatContext->streams[videoStreamIndex]->codecpar);
     if(ret1==0){
         cout<<"拷贝视频流成功";
     }
     if(ret1<0){
         QMessageBox::warning(NULL,"","拷贝视频流失败！");
         avformat_close_input(&avFormatContext);
     }

     //打開對應视频解碼器
     if(avcodec_open2(videoCodecContext,videoCodec,NULL)<0){
        QMessageBox::warning(NULL,"","打開视频解码器失败");
        avformat_close_input(&avFormatContext);
        avcodec_free_context(&videoCodecContext);
     }

//     //音频部分处理
//     //根據編碼器ID獲取音频劉解碼器
//     if(audioStreamIndex==1){
//         const AVCodec* audioCodec;
//         audioCodec= avcodec_find_decoder(avFormatContext->streams[audioStreamIndex]->codecpar->codec_id);
//         if(audioCodec!=NULL){
//             //获取音频编解码器上下文信息
//             /*AVCodecContext**/audioCodecContext = avcodec_alloc_context3(audioCodec);
//             if(audioCodecContext==NULL){
//                 avformat_close_input(&avFormatContext);
//                 qDebug()<<"获取上下文信息失败";
//             }

//             int ret2=avcodec_parameters_to_context(audioCodecContext,avFormatContext->streams[audioStreamIndex]->codecpar);
//             if(ret2==0){
//                 qDebug()<<"拷贝音频流成功";
//             }
//             if(ret2<0){
//                 qDebug()<<"拷贝音频流失败！";
//                 avformat_close_input(&avFormatContext);
//             }

//             //打開對應音频解碼器
//             if(avcodec_open2(audioCodecContext,audioCodec,NULL)<0){
//                qDebug()<<"打開音频解碼器失敗";
//                avformat_close_input(&avFormatContext);
//                avcodec_free_context(&audioCodecContext);
//             }

//             //音频流特殊处理部分
//             //音频转码配置
//             //ffmpeg中刚刚解码出的数据因为排列方式的原因，不能直接播放，必须要转换，首先根据音频解码上下文设置并初始化转换上下文：
//             /*SwrContext**/swrContext = swr_alloc_set_opts(
//                         nullptr,
//                         AV_CH_LAYOUT_STEREO,
//                         AV_SAMPLE_FMT_S16,
//                         44100,
//                         audioCodecContext->channel_layout,
//                         audioCodecContext->sample_fmt,
//                         audioCodecContext->sample_rate,
//                         0, nullptr);
//             if(swr_init(swrContext) < 0){
//                 avformat_close_input(&avFormatContext);
//                 avcodec_free_context(&videoCodecContext);
//                 avcodec_free_context(&audioCodecContext);
//             }
//         }}else{
//         qDebug()<<"尋找音频解碼器失敗";}

     qDebug()<<"視頻流初始化成功";
//     emit sendcontext(avFormatContext,videoCodecContext,audioCodecContext,swrContext);
}

void MainWindow::setupCMD()
{
//    try {
//        if(process != nullptr || process->state() != QProcess::ProcessState::NotRunning)
//        {
//            if(QMessageBox::warning(NULL,"","脚本已经运行,是否强行重新开始",QMessageBox::Ok,QMessageBox::No) == QMessageBox::No)
//            {
//                return;
//            }
//            delete process;
//        }
//    } catch (...) {
//        cout<<"error"<<endl;
//    }

    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);

    connect(process,&QProcess::finished,this,&MainWindow::slot_cmdfinished);
    //读取通道上有可用数据时
    connect(process,&QProcess::readyRead,this,&MainWindow::slot_readdata);
    //有新的标准输出数据时
    connect(process,&QProcess::readyReadStandardOutput,this,&MainWindow::slot_readdata);
    //有error
    //note:混合使用qt4 qt5 signal-slot
    connect(process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(slot_cmderror()));

    process->start("cmd.exe");
}

void MainWindow::doCommand(QString command)
{

    // 注意：需要回车才会执行
    command.append("\r\n");
    cout << "will write Cmd :" << command << endl;
    process->write(command.toLocal8Bit());
    cout << "Suceesee to write local8bit Cmd :" << command.toLocal8Bit() << endl;
    process->waitForFinished(5);
    cout << "Suceesee to write local8bit Cmd :" << command.toLocal8Bit() << endl;
}

QString MainWindow::changeVideoFPS(QString _fileName, int fps)
{
    QString ffmpeg = QDir("./ffmpeg/bin/ffmpeg.exe").absolutePath();
    QString resultFile =  QDir("./temp/"+QFileInfo(_fileName).fileName()).absolutePath();
    QString cmd = ffmpeg + " -i "+QDir(_fileName).absolutePath()+" -qscale 0 -r "+QString::number(_parameters.fps)+" -y "+resultFile;
    cout<<cmd;
    QProcess pro(this);
    pro.setProgram("cmd");
    QStringList arg;
    arg<<"/c"<<cmd;
    pro.setArguments(arg);
    pro.start();
    pro.waitForStarted();
    pro.waitForFinished(-1);
    QString temp=QString::fromLocal8Bit(pro.readAllStandardOutput()); //程序输出信息
    cout<<temp<<endl;

    return resultFile;
}

void MainWindow::dealImgFormSignal_loadDone()
{
    QPointer<ImgForm> temp = getNewImgForm();
    vector_ImgForm.append(temp);
//    connect(vector_ImgForm[vector_ImgForm.size() - 1] , &ImgForm::loadDone , this , &MainWindow::dealImgFormSignal_loadDone);
    if(vector_ImgForm.size() > 2)
        pos++;
    this->label->setText(QString::number(pos+1)+"/"+QString::number(vector_ImgForm.size()-1));
}

void MainWindow::do_signal_delete(QPointer<ImgForm> p)
{
    //note:Three are some bugs when delete a imgform in multiple imgforms but it done well in just one imgform
    for(auto i = vector_ImgForm.begin() ; i != vector_ImgForm.end() ; i++)
    {
        if(i->data() == p)
        {
            cout<<"delete "<<i->data()->getFileName()<<endl;
            vector_ImgForm.erase(i);
            break;
        }
    }
    update();
}


void MainWindow::initRightKeyMenu()
{
    //创建菜单对象
    QMenu *pMenu = new QMenu(this);

    QAction *pInsertImgformMenue = new QAction(tr("插入"), this);

    pInsertImgformMenue->setData(RIGHT_KEY_MENU::INSERT_NEW_IMGFORM);

    //把QAction对象添加到菜单上
    pMenu->addAction(pInsertImgformMenue);


    //连接鼠标右键点击信号
    connect(pInsertImgformMenue,&QAction::triggered,this,&MainWindow::onTaskBoxContextMenuEvent);

    //在鼠标右键点击的地方显示菜单
    pMenu->exec(cursor().pos());

    //释放内存
    QList<QAction*> list = pMenu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete pMenu;
}

void MainWindow::onTaskBoxContextMenuEvent()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
    if(pEven == nullptr){
        cout<<"invalid triggered\n";
        return;
    }

    //获取发送信息类型
    int iType = pEven->data().toInt();
    cout<<"iType:"<<iType<<endl;
    switch (iType)
    {
    case RIGHT_KEY_MENU::INSERT_NEW_IMGFORM:
        cout<<"INSERT_NEW_IMGFORM action was trigled\n";
        do_triggered_action_INSERT_NEW_IMGFORM();
        break;
    default:
        break;
    }
}

void MainWindow::composeVideo()
{
    //添加进度
    this->progressBar->setMaximum(vector_ImgForm.size()+1);
    this->progressBar->setValue(1);
//    this->progressBar->setTextVisible(true);
    this->progressBar->setFormat("解码中");
    this->progressBar->setVisible(true);
    this->progressBar->setTextVisible(true);
//    QTest::qSleep(100);
//    this->progressBar->show();


    update();
//    auto cap = new VideoCapture();
    imgNum = 0;
    int proc = 1;//进度

    int time = 0 ,fps = 0;
    for(const auto& file : vector_ImgForm)
    {

        this->progressBar->setValue(proc);
//        update();//it's invalid
        repaint(QRect(this->progressBar->pos(),this->progressBar->size()));
        if(file->getType() == ImgForm::TYPE::IMG)
        {
            if(file->getIsSpecial())
            {
                time = file->deaultTime;
                fps = file->fps;
            }else{
                time = _parameters.img_time_default;
                fps = _parameters.fps;
            }

            for(int j = time*fps ; j > 0 ; j--)
                if(saveImg(temp_imgs_folder+ QString::number(imgNum)+".png",QImage(file->getFileName()) ) )
                {
                    imgNum++;
                }else{
                    QMessageBox::warning(NULL,"img","解码失败");
                }
        }else if(file->getType() == ImgForm::TYPE::VIDEO){
            video2imgs(file->getFileName() , imgNum);
        }
        proc++;
    }
    this->progressBar->setFormat("合成视频中");
    this->progressBar->setValue(0);
    this->progressBar->setMaximum(imgNum);
    if(!imgs2video(temp_imgs_folder))
    {//生成视频失败
        return;
    }

    this->progressBar->setVisible(false);
    update();
    auto msb = QMessageBox();
    msb.setText("合成视频结束");
    msb.exec();

    auto msb1 = QMessageBox();
    msb1.setText("正在替换BGM");
    msb1.exec();
    //添加音频
    if(musicFileName != "")
    {
        this->mergeVideoAndBgm(_parameters.export_file_path+"/temp.mp4" , musicFileName);
    }
    else
    {
        QMessageBox::information(NULL,"","未添加音频");
        return;
    }
    auto msb2 = QMessageBox();
    msb2.setText("替换BGM结束");
    msb2.exec();
}

bool MainWindow::imgs2video(QString folderName)
{

    auto tempDir = QDir(_parameters.export_file_path+"/temp.mp4").absolutePath();
    if(tempDir.toStdString() == "" || tempDir.toStdString().size() == 0)
    {
        QMessageBox::warning(NULL,"","合成视频路径:"+tempDir+" 无效");
    }
    VideoWriter vw(tempDir.toStdString(),CAP_OPENCV_MJPEG
                               ,_parameters.fps,Size(_parameters.w,_parameters.h));
//    cout<<tempDir<<_parameters.fps<<_parameters.w<<_parameters.h;
//    cout<<tempDir<<endl;
//    puts(tempDir.toStdString().c_str());
    if(!vw.isOpened())
    {
        QMessageBox::warning(NULL,"","imgs2video:创建视频"+tempDir+"失败");
        cout<<tempDir<<endl;
        puts(tempDir.toStdString().c_str());
        return false;
    }

    for (size_t i = 0; i < imgNum; i++)
    {
        puts((folderName.toStdString() + QString::number(i).toStdString()+ String(".png")).c_str());

        Mat image = imread(folderName.toStdString() + QString::number(i).toStdString()+ String(".png") );

        if(image.empty())
        {
            QMessageBox::warning(NULL,"","imgs2video：帧为空");
        }
        //note:mat 不可以直接resize
        if(image.size().width != _parameters.w || image.size().height != _parameters.h)
        {
            QMessageBox::warning(NULL,"","imgs2video：分辨率转换失败");//error
        }
        if(image.size().width == 0 || image.size().height == 0)
        {
            QMessageBox::warning(NULL,"","imgs2video：帧转换后为空");//error
        }
        vw << image;
        try {
            //防止最大值达不到
            this->progressBar->setValue(i);
            update();
        } catch (...) {
        }

    }
    vw.release();
    cout << "处理完毕！" << endl;
    // 处理完之后会在得到一个视频文件。
    return true;
}

bool MainWindow::video2imgs(QString fileName,int pos)
{
    fileName  = QDir(fileName).absolutePath();
//    String cStr(fileName.toStdString());
//    auto cap = new VideoCapture(cStr);
    Mat mat;
    QImage img;/*
    if(!cap->isOpened())
    {
        QMessageBox::information(NULL,"","素材视频"+QDir(fileName).absolutePath()+"打开失败");
    }*/
    int fps = getVideoFPS(fileName.toLocal8Bit().constData());
    cout<<fps<<endl;
    if(fps == -1){
        return false;
    }
    if(_parameters.fps != fps)
    {   //帧率切换
        QMessageBox::information(NULL,"","开始更改素材视频帧率");
        fileName = changeVideoFPS(fileName , _parameters.fps);
        cout<<fileName<<endl;
//        cap->release();
//        cap->open(fileName.toStdString());
//        if(!cap->isOpened())
//        {
//            QMessageBox::warning(NULL,"","帧率改变后视频打开失败");
//        }else{
//            fileName = QDir(resultFile).absolutePath();
//        }
    }
    QFileInfo fileInfro(fileName);
    if(!fileInfro.isFile())
    {
        QMessageBox::warning(NULL,"",fileName+"不存在");
        return false;
    }
    auto cap = new VideoCapture;
    cap->open(fileName.toStdString());
    if(!cap->isOpened()){
        QMessageBox::warning(NULL,"","帧率改变后视频打开失败");
        return false;
    }
    for(int i = pos ; cap->isOpened() ;)
    {
        *cap >> mat;
        img = ImgForm::MatToQImage(mat).scaled(_parameters.w,_parameters.h);
        if(img.isNull())
        {
            QMessageBox::information(NULL,"",fileName+"/视频解码第"+QString::number(i)+"帧失败");
        }
//        imwrite(QString(temp_imgs_folder + QString::number(i)+".png").toStdString() ,mat);
        if(saveImg(QDir(temp_imgs_folder + QString::number(i)+".png").absolutePath(),img))
        {
            i++;
            imgNum++;
            try{

                this->progressBar->setValue(imgNum);
                update();
            }catch(...)
            {

            }
        }else{
            cout<<"video save img errr\n";
            return false;
        }
    }

    cap->release();
    delete cap;
    cap = nullptr;
    if(pos == imgNum)
        return false;
    return true;
}

bool MainWindow::saveImg(QString toFileName,const QImage& img)
{
    QImage temp =  img.scaled(_parameters.w,_parameters.h);
    if(temp.width() != _parameters.w || temp.height()!=_parameters.h)
    {
        QMessageBox::information(NULL,"","saveImg:分辨率转换失败");
    }
    if(!temp.save(toFileName,"PNG"))
    {
        QMessageBox::information(NULL,"","save "+toFileName+" erorr\n");
        return false;
    }else{
        cout<<"save "<<toFileName<<" successfully\n";
    }
    return true;
}

bool MainWindow::saveVedio(QString fileName)
{
    return true;
}

void MainWindow::rebuiltFolder()
{
    //清空上次缓存
    try {
        QDir rm("./temp");
        rm.removeRecursively();//note:don't remove

    } catch (...) {
        cout<<"delete folder erorr\n";
    }
    QDir dir("./");
    dir.mkpath("temp/imgs");
    dir.mkpath("temp/result");
}

QPointer<ImgForm> MainWindow::getNewImgForm()
{
    /*
     * 不应该单独创建对象，而要自定义函数创建对象(尤其是需要多次创建的对象),这样可以方便绑定信号和槽
    */
    QPointer<ImgForm> temp = new ImgForm(this,imgWidth,imgHeight);
    connect(temp , &ImgForm::loadDone , this , &MainWindow::dealImgFormSignal_loadDone);

    connect(temp , &ImgForm::signal_delete , this , &MainWindow::do_signal_delete);
    return temp;
}

void MainWindow::mergeVideoAndBgm(QString _videoFileName, QString _musicFileName)
{
    _videoFileName = QDir(_videoFileName).absolutePath();
    _musicFileName = QDir(_musicFileName).absolutePath();

//    cout<<_videoFileName<<" "<<_musicFileName<<endl;
    VideoCapture cap;
    //error:中文路径\空格路径失败
    cap.open(_videoFileName.toStdString());
    if(!cap.isOpened()){
        QMessageBox::warning(this,"","mergeVideoAndBgm:打开视频:"+_videoFileName+"失败");//error
        return;
    }

    int iTime = cap.get(CAP_PROP_FRAME_COUNT) / cap.get(CAP_PROP_FPS);
    QString sTime(getFormatTime(iTime));


    //暂不处理音乐时长不够的问题
    QString targetMusic = "file " + QString("'")+QDir(_musicFileName).absolutePath()+QString("'");
    //
    QFile targetMusicFile(musicFileNames_txt);
//    cout<<targetMusic<<" "<<musicFileNames_txt<<endl;
    if(!targetMusicFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))  //删除之前的，不会覆盖之前的文件
    {
        QMessageBox::warning(this,"错误","mergeVideoAndBgm:背景音乐配置文件:"+_musicFileName+"写入失败，信息没有保存！","确定");
        return;

    }
    QTextStream inStream(&targetMusicFile);//文字流
    inStream<<targetMusic;
    inStream.flush();
    targetMusicFile.close();

    QString cmd = "";


    QString ffmpeg = QDir("./ffmpeg/bin/ffmpeg.exe").absolutePath();//note:应该优化
    QString bat = "";
    auto mergeMusics =  ffmpeg+" -f concat -safe 0 -i " + QDir(musicFileNames_txt).absolutePath() + " -c copy "+QDir("./temp/temp.mp3").absolutePath();//合成一个音频
    cmd += mergeMusics;

    bat += mergeMusics + "\n";
    cmd += " && ";//成功后执行
    auto spilitMusitToSameLengthOfVideo =  ffmpeg + " -i "+QDir("./temp/temp.mp3").absolutePath()+" -ss 00:00:00.000 -t " + sTime +" -acodec copy "+QDir("./temp/result.mp3").absolutePath();//截取
    cmd += spilitMusitToSameLengthOfVideo;

    bat += spilitMusitToSameLengthOfVideo +"\n";
    cmd += " && ";
    auto mergeMusicAndVideo =  ffmpeg + " -i " + QDir("./temp/result.mp3").absolutePath() +" -i " + _videoFileName + " -y " + QDir(_parameters.export_file_path+"/result.mp4").absolutePath();//合并音视频
    cmd += mergeMusicAndVideo;
    bat += mergeMusicAndVideo + "\n" ;//+ "pause";

    cout<<cmd<<endl;
    //just test
    QString targetBatFileName = QDir("./temp.bat").absolutePath();
    QFile cmdFile(targetBatFileName);
    if(!cmdFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QMessageBox::warning(NULL,"","批处理脚本打开失败");
    }
    QTextStream cmdStream(&cmdFile);
    cmdStream<<bat;//<<"pause";
    cmdStream.flush();
    cmdFile.close();

//    setupCMD();
//    doCommand(mergeMusics);
//    doCommand(spilitMusitToSameLengthOfVideo);
//    doCommand(mergeMusicAndVideo);

//    if(process != nullptr || process->state() != QProcess::ProcessState::NotRunning)
//    {
//        if(QMessageBox::warning(NULL,"","脚本已经运行,是否强行重新开始",QMessageBox::Ok,QMessageBox::No) == QMessageBox::No)
//        {
//            return;
//        }
//        delete process;
//        process = new QProcess();
//    }
//    process->start(targetBatFileName);
//    process->waitForStarted();
//    process->waitForFinished();//wait end
//    QString temp=process->readAllStandardOutput(); //程序输出信息
//    cout<<temp<<endl;
    //test end
//    QStringList args;
//    args<<"/c"<<mergeMusics<<spilitMusitToSameLengthOfVideo<<mergeMusicAndVideo;
//    process->start("cmd.exe",args);
//    if(!process->waitForStarted())
//    {
//        QMessageBox::warning(NULL,"","脚本启动失败");
//    }
//    process->waitForStarted();
//    process->waitForFinished();//wait end
//    QString temp=process->readAllStandardOutput(); //程序输出信息
//    cout<<temp<<endl;
    QProcess pro(this);
    pro.setProgram("cmd");
    QStringList arg;
    arg<<"/c"<<cmd;
    pro.setArguments(arg);

    pro.start();
    pro.waitForStarted();
    pro.waitForFinished();//wait end
    QString temp2=pro.readAllStandardOutput(); //程序输出信息
    cout<<temp2<<endl;//note:temp always be void , why?
//    addPauseToBat();
}

QString MainWindow::getFormatTime(int seconds)
{
    //don't consider ms
    int hh = seconds/3600;
    seconds -= (hh*3600);
    int mm = seconds/60;
    seconds -= (mm*60);
    int ss = seconds;
    QString ans = "";
    if(hh > 9){
        ans += QString::number(hh);
    }else if(hh > 0){
        ans += ("0" + QString::number(hh));
    }else{
        ans += "00";
    }
    ans+=":";

    if(mm > 9){
        ans += QString::number(mm);
    }else if(mm > 0){
        ans += ("0" + QString::number(mm));
    }else{
        ans += "00";
    }
    ans+=":";

    if(ss > 9){
        ans += QString::number(ss);
    }else if(ss > 0){
        ans += ("0" + QString::number(ss));
    }else{
        ans += "00";
    }
    ans+=".000";
    return ans;//such as 00:00:00.000
}

void MainWindow::addPauseToBat()
{
    QString targetBatFileName = QDir("./temp.bat").absolutePath();
    QFile cmdFile(targetBatFileName);
    if(!cmdFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QMessageBox::warning(NULL,"","批处理脚本打开失败");
    }
    QTextStream cmdStream(&cmdFile);
    cmdStream<<"pause";
    cmdStream.flush();
    cmdFile.close();
}

void MainWindow::do_triggered_action_INSERT_NEW_IMGFORM()
{
    int i = 0;
    for(auto imgIter = vector_ImgForm.begin() ; imgIter != vector_ImgForm.end() ; ++imgIter)
    {

        if(imgIter->data()->pos().x() > rightKeyMenuePos.x()\
                && imgIter->data()->pos().x() - rightKeyMenuePos.x() < 3*interval)
        {
            QPointer<ImgForm> temp = getNewImgForm();
//            vector_ImgForm.append(temp);
//            cout<<"IMGNAME:"<<imgIter->data()->getFileName()<<endl;
//            cout<<"size:"<<vector_ImgForm.size()<<endl;
            vector_ImgForm.insert(imgIter,temp);
//            cout<<"size:"<<vector_ImgForm.size()<<endl;
//          cout<<"IMGNAME:"<<imgIter->data()->getFileName()<<endl;
//            cout<<"IMGNAME:"<<vector_ImgForm[i].data()->getFileName()<<endl;

//      connect(vector_ImgForm[i] , &ImgForm::loadDone , this , &MainWindow::dealImgFormSignal_loadDone);
            if(vector_ImgForm.size() > 2)
                pos++;
            this->label->setText(QString::number(pos+1)+"/"+QString::number(vector_ImgForm.size()-1));
            return;
        }
        i++;
    }
}

void MainWindow::on_action_play_vedio_triggered()
{
//    return;//error:窗口莫名嵌套了
    if(playVideo != NULL)
        delete playVideo;
    playVideo = new PlayVideo(_parameters.export_file_path+"/result.mp4",this);

    playVideo->setWindowModality(Qt::WindowModal);
    playVideo->setAttribute(Qt::WA_DeleteOnClose);

    playVideo->show();
}
//note:奇怪的bug:这两个一模一样的都不能删除(改过名字)
void MainWindow::on_actionexport_triggered()
{
    if(vedioParameEdit == nullptr)
        vedioParameEdit = new Dialog_VideoJParameterSetting();
    connect(vedioParameEdit,&Dialog_VideoJParameterSetting::signal_videoParameters,
            [&](Dialog_VideoJParameterSetting::Parameters parameters){
            _parameters = parameters;
//            cout<<"收到参数\n";
    });
    if(vedioParameEdit->exec() == QDialog::Accepted)
    {
        cout<<"开始制作\n";
        composeVideo();
    }
}


void MainWindow::on_action_export_triggered()
{
    if(vedioParameEdit == nullptr)
        vedioParameEdit = new Dialog_VideoJParameterSetting();
    connect(vedioParameEdit,&Dialog_VideoJParameterSetting::signal_videoParameters,
            [&](Dialog_VideoJParameterSetting::Parameters parameters){
            _parameters = parameters;
            cout<<"收到参数\n";
    });
    if(vedioParameEdit->exec() == QDialog::Accepted)
    {
        cout<<"开始制作\n";
        composeVideo();

    }
}


void MainWindow::on_actionImportMusic_triggered()
{
    auto tempfileName = QFileDialog::getOpenFileName(nullptr,
        "Open BGM", "/home", "Music Files (*.mp3 *.avi *)");
    QMimeDatabase db;
    QMimeType mmtype = db.mimeTypeForFile(tempfileName);
    if(mmtype.name().startsWith("audio/"))
    {
        this->musicFileName = tempfileName;
        QMessageBox msb;
        msb.setText("BGM导入完毕");
        msb.exec();
    }else{
//        musicFileName="";
        QMessageBox msb;
        msb.setText("BGM无效");
        msb.exec();
    }

}

void MainWindow::on_action_import_triggered()
{
    //批量导入
    auto tempfileNames = QFileDialog::getOpenFileNames(nullptr,
        "Open BGM", "/home", "Music Files (*.mp3 *.avi *)");
    for(size_t i = 0 ; i < tempfileNames.size() ; i++)
    {
        auto end = vector_ImgForm.end();
        (end-1)->data()->SetFileName(tempfileNames[i]);
    }
}

void MainWindow::slot_readdata()
{
    QByteArray mreaddata =  process->readAll();
//    ui->textBrowser->append(QString::fromLocal8Bit(mreaddata));
//    ui->textBrowser->update();

    cout << "Success to read:" << QString::fromLocal8Bit(mreaddata) << endl;
}

void MainWindow::slot_cmderror()
{
    int errorcode = process->exitCode();

     QString error = process->errorString();

    QMessageBox::warning(NULL,"",QString("脚本运行错误,Process error coed:%1  " + error).arg(errorcode));
     cout << "Success to read cmderror:" << error << endl;
}

void MainWindow::slot_cmdfinished()
{
    int flag = process->exitCode();

    /* 信息输出 */
    cout << "Cmd finish:" << flag << endl;
}


//ffmpeg:
///*
//void MainWindow::pngTomp4_2()         //png图集转mp4
//{
//    QString folder_path = QFileDialog::getExistingDirectory(this,"选择文件目录","",QFileDialog::ShowDirsOnly
//                                                            | QFileDialog::DontResolveSymlinks);
//    if(folder_path.isEmpty())   return;


//    QString outFilename = QFileDialog::getSaveFileName(this,"选择保存路径",folder_path+"/视频文件","*.mp4;; *.avi");
//    if(outFilename.isEmpty())   return;

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


//    AVFormatContext *kAVFormatContext;
//    AVOutputFormat *kAVOutputFormat;
//    AVCodecContext *kAVCodecContext;
//    AVDictionary *kAVDictionary = NULL;
//    AVDictionary *opt = NULL;
//    AVCodec *video_codec;
//    AVStream *kAVStream;
//    AVFrame *frame;
//    AVFrame *tmp_frame;
//    AVPacket kAVPacket = { 0 };
//    struct SwsContext *kSwsContext;
//    AVRational time{1,24};  /*1s25帧*/
//    AVRational time_1{1,1};  /*1s25帧*/
//    uint8_t *intBuffer = 0;                           // 图片数据缓冲区

//    int ret;
//    int got_packer = 0;
//    int64_t next_pts = 0;

//    tmp_frame = av_frame_alloc();
//    av_init_packet(&kAVPacket);

//    av_register_all();     //1、注册所有容易和编解码器

//    /* 2、分配输出媒体上下文 */
//    avformat_alloc_output_context2(&kAVFormatContext, NULL, NULL, outFilename.toLocal8Bit().data());      //上下文结构体、输出结构体、文件类型、路径

//    if(!kAVFormatContext)        //如果根据文件名没有找到对应的格式则默认mpeg格式
//    {
//        ret = avformat_alloc_output_context2(&kAVFormatContext, NULL, "mpeg",  outFilename.toLocal8Bit().data());    //没有找到文件类型默认mpeg（MP4）
//    }
//    if(!kAVOutputFormat) { goto end;}

//    kAVOutputFormat = static_cast<AVOutputFormat*>(kAVFormatContext->oformat);    //把输出操作结构体赋值出来

//    if (!(kAVOutputFormat->flags & AVFMT_NOFILE))   /* 如果需要，打开输出文件*/
//    {
//        ret = avio_open(&kAVFormatContext->pb, outFilename.toLocal8Bit().data(), AVIO_FLAG_WRITE);  //3、打开输出文件
//        if (ret < 0) {qDebug()<<"打开输出文件失败。"<<ret; }
//    }

//    /* 使用默认格式的编解码器添加音频和视频流，并初始化编解码器。*/
//    if (kAVOutputFormat->video_codec != AV_CODEC_ID_NONE)
//    {
//        video_codec = avcodec_find_encoder(kAVOutputFormat->video_codec);   //4、找到编码器
//        if(!video_codec)    goto end;

//        kAVStream = avformat_new_stream(kAVFormatContext,NULL);         //5、新建一个输出流
//        if(!kAVStream) {qDebug()<<"创建流kAVStream失败。";goto end;}

//        kAVCodecContext = avcodec_alloc_context3(video_codec);      //初始化一个AVCodecContext
//        if(!kAVCodecContext)    {qDebug()<<"用编码器video_codec初始化的kAVCodecContext默认参数失败";goto end;}

//        switch(video_codec->type)
//        {
//        case AVMEDIA_TYPE_VIDEO:

//            kAVCodecContext->codec_id = video_codec->id;
//            kAVCodecContext->bit_rate = 800000;
//            kAVCodecContext->width = 720;
//            kAVCodecContext->height = 404;
//            kAVStream->time_base = time;
//            kAVCodecContext->time_base = time;
//            kAVCodecContext->gop_size = 12; /*最多每十二帧发射一个内帧 */
//            kAVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
//            if (kAVCodecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO)
//            {
//                kAVCodecContext->max_b_frames = 2;  /*为了测试，我们还添加了b帧*/
//            }
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

//    av_dict_copy(&opt,kAVDictionary, 0);
//    ret = avcodec_open2(kAVCodecContext,video_codec,&opt);      //6、打开编码器
//    if(ret<0){qDebug()<<"打开视频编解码器失败"<<ret; goto end;}
//    av_dict_free(&opt);

//    frame = av_frame_alloc();
//    if(!frame)  {qDebug()<<"分配帧失败."; goto end;}
//    frame->format = kAVCodecContext->pix_fmt;
//    frame->width = kAVCodecContext->width;
//    frame->height = kAVCodecContext->height;

//    ret = av_frame_get_buffer(frame, 32);   //分配内存空间frame必须要有上三条件，32也就是4字节对齐
//    if(ret < 0){qDebug()<<"frame分配内存失败"; goto end;}

//    ret = avcodec_parameters_from_context(kAVStream->codecpar,kAVCodecContext);/*将流参数复制到muxer */
//    if (ret < 0) {qDebug()<<"无法复制流参数";goto end; }

//    av_dump_format(kAVFormatContext, 0, outFilename.toLocal8Bit().data(), 1);     //打印输出文件的详细信息



//    ret = avformat_write_header(kAVFormatContext,&kAVDictionary);/* 7、写流标头(如果有的话)*/
//    if(ret < 0){qDebug()<<"写流标题失败"; goto end;}


//    //8、写入每一帧数据
//    for(int i = 0; i<527; i++)
//    {
////        if(av_compare_ts(next_pts, kAVCodecContext->time_base,  10.0, time_1) > 0)      /*这里只生成10s的视频*/
////        {
////            qDebug()<<"暂时不需要生成新的帧"; break;
////        }
//        if (av_frame_make_writable(frame) < 0)  {goto end;}  /*当我们传递一个帧给编码器时，它可能会在内部保留一个对它的引用;确保我们没有在这里覆盖它*/

//        QImage img(tr("%1/%2.png").arg(folder_path).arg(i));
//        if(img.isNull()){qDebug()<<"打开图片失败";break;}
//        img = img.convertToFormat(QImage::Format_RGB888);

//        kSwsContext = sws_getContext(kAVCodecContext->width,kAVCodecContext->height,
//                                     AV_PIX_FMT_RGB24,kAVCodecContext->width,kAVCodecContext->height,
//                                     AV_PIX_FMT_YUV420P,SWS_LANCZOS | SWS_ACCURATE_RND,NULL,NULL,NULL);
//        if(!kSwsContext){qDebug()<<"无法初始化图片转换器";    goto end;}


//        intBuffer = (uint8_t*)malloc(sizeof(uint8_t)*img.bytesPerLine()*img.height());
//        memcpy(intBuffer,img.constBits(),sizeof(uint8_t)*img.bytesPerLine()*img.height());
//        avpicture_fill((AVPicture *)tmp_frame,intBuffer,AV_PIX_FMT_RGB24,kAVCodecContext->width,kAVCodecContext->height); //pAVFrame32的data指针指向了outBuffer

//        sws_scale(kSwsContext,(const uint8_t * const *)tmp_frame->data,
//                  tmp_frame->linesize,0,kAVCodecContext->height,frame->data,frame->linesize);   //图片信息转换
//        sws_freeContext(kSwsContext);
//        kSwsContext = 0;

//        frame->pts = next_pts++;
//        //frame->pts = i*(kAVStream->time_base.den)/((kAVStream->time_base.num)*24);

//        got_packer = 0;
//        ret = avcodec_encode_video2(kAVCodecContext,&kAVPacket,frame,&got_packer);  //8、编码、把一帧数据编码成AVPacket
//        if(ret < 0) {qDebug()<<"错误编码视频帧"<<ret; goto end;}

//        if(got_packer)
//        {
//            av_packet_rescale_ts(&kAVPacket, kAVCodecContext->time_base, kAVStream->time_base);
//            kAVPacket.stream_index = kAVStream->index;
//            ret = av_write_frame(kAVFormatContext, &kAVPacket); /*将压缩帧写入媒体文件。 */
//            av_free_packet(&kAVPacket);
//        }else
//        {ret = 0;}
//        free(intBuffer);intBuffer = 0;
//        if(ret<0){ qDebug()<<"写入video文件失败"<<ret<<kAVPacket.stream_index; break; }
//    }

//    /*  选择写预告片，如果有的话。预告片必须在你之前写好 当你写标题时关闭CodecContexts打开;否则
//        av_write_trailer()可以尝试使用被释放的内存  av_codec_close()。要编码的流 */
//    av_write_trailer(kAVFormatContext);

//end:
//    /* 关闭每个编解码器。 */
//    avcodec_free_context(&kAVCodecContext);
//    av_frame_free(&frame);

//    if (!(kAVOutputFormat->flags & AVFMT_NOFILE))
//        avio_closep(&kAVFormatContext->pb);/*关闭输出文件*/


//    avformat_free_context(kAVFormatContext);   /*释放上下文*/
//}

