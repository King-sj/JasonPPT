#pragma once

#include <QTextEdit>
#include <QObject>
class RectText : public QTextEdit
{
    Q_OBJECT
public:
    RectText(QRect rect,QString text ="",QWidget* parent = nullptr);
    RectText(const RectText& h);
    RectText& operator=(const RectText& h);
    void dealTextChanged();
private:
    void init();
private:
    bool eventFilter(QObject *watched, QEvent *event)override;

    void wheelEvent(QWheelEvent *e)override;

public slots:
//    void append(const QString& text)override;
private slots:
    void slot_textChange();
};

