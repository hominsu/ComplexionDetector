#pragma once

#ifndef FILETHREADCONTROLLER_H
#define FILETHREADCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <vector>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QAction>
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <imgproc/imgproc.hpp>


class FileThreadController: public QObject
{
    Q_OBJECT
public:
    FileThreadController(QObject *parent= nullptr);
    ~FileThreadController();

private:
    QThread fileThread;

signals:
    void fileThreadStartSignal(const QString fileName);
    void sendImage(const QImage qimage);

public slots:
    void recvFrameSlot(const QImage qImage);
    void fileThreadStartSlot(const QString fileName);

};



class FileThread: public QObject
{
    Q_OBJECT
public:
    void fileRead(const QString fileName);

private:
    cv::Mat frameResize(const cv::Mat srcFrame);
    void startVideoRead(const std::string fileName);
    void startImageRead(const std::string fileName);
    QImage MatImageToQt(const cv::Mat &src);

private:
    const int maxWidth  = 640;
    const int maxHeight = 480;
    const double scale  = 0.5;
    cv::VideoCapture cap;
    cv::Mat     srcImage;
    QImage      dstImage;
    cv::Mat     srcFrame;
    QImage      dstFrame;
    cv::Mat     tmpFrame;
    cv::Mat     midFrame;

signals:
    void sendFileFrame(const QImage Frame);

public slots:

};

#endif // FILETHREADCONTROLLER_H
