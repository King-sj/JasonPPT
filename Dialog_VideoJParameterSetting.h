﻿#pragma once

#include "ui_Dialog_VideoJParameterSetting.h"
#include<QCloseEvent>
class Dialog_VideoJParameterSetting : public QDialog, private Ui::Dialog_VideoJParameterSetting
{
    Q_OBJECT

public:
    explicit Dialog_VideoJParameterSetting(QWidget *parent = nullptr);
    struct Parameters{
        int img_time_default;
        QString export_file_path;
        QString export_filename;
        int fps;
        int w;
        int h;
        int crf;//视频质量
    }_parameters;
private:
signals:
    void signal_videoParameters(Parameters parameters);

public slots:
    virtual void accept() override;
private slots:
    void on_pushButton_setOutPutFile_clicked();
};

