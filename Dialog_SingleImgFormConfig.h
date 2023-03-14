#pragma once

#include "ui_Dialog_SingleImgFormConfig.h"

class Dialog_SingleImgFormConfig : public QDialog, private Ui::Dialog_SingleImgFormConfig
{
    Q_OBJECT

public:
    explicit Dialog_SingleImgFormConfig(QWidget *parent = nullptr);

public:
    struct Parameters{
        int default_time = 2;
    }_parameters;
private:

signals:
    void signal_videoParameters(Parameters parameters);

public slots:
    virtual void accept() override;
};

