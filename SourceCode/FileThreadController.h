#pragma once

#ifndef FILETHREADCONTROLLER_H
#define FILETHREADCONTROLLER_H

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <imgproc/imgproc.hpp>
#include "Detected/detected.h"
#include "PPTControl/PPtController.h"


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
    void sendImage(const QImage qimage, int action);
    void stopReadFrameSignal();

public slots:
    void recvFrameSlot(const QImage qImage, int action);
    void fileThreadStartSlot(const QString fileName);
    void stopReadFrameSlot();

};



class FileThread: public QObject
{
    Q_OBJECT
public:
    void fileRead(const QString fileName);
    int action = -1;

private:
    cv::Mat frameResize(const cv::Mat srcFrame);
    void startVideoRead(const std::string fileName);
    void startImageRead(const std::string fileName);
    QImage MatImageToQt(const cv::Mat &src);
    std::string selectFile();


private:
    bool isCap = false;
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

    Detected    detected;
    PPtController* ppt = nullptr;

signals:
    void sendFileFrame(const QImage Frame, int action);

public slots:
    void stopReadFrameSlot();
    //void setMainwindowPtr(QObject* ptr);

};

#endif // FILETHREADCONTROLLER_H
