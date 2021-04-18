#pragma once

#ifndef CAMERATHREADCONTROLLER_H
#define CAMERATHREADCONTROLLER_H

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <imgproc/imgproc.hpp>
#include "Detected/detected.h"
#include "PPTControl/PPtController.h"


class CameraThreadController: public QObject   //controller用于启动线程和处理线程执行结果
{
    Q_OBJECT
public:
    QThread cameraThread;

public:
    CameraThreadController(QObject *parent= nullptr);
    ~CameraThreadController();

signals:
    void cameraThreadStart();
    void statusCameraThreadCatchFrame(const QString fileName, const bool isRecoding);
    void sendImage(const QImage qimage, int action);
    void cameraEnableSignal(const bool isEnable);
    void noneCameraSignal();
    void stopReadFrameSignal();
    void setClientActionSignal(const int action);

public slots:
    void recvImageSlot(const QImage qImage, int action);    //处理线程执行的结果
    void statusVedioRecordingSlot(const QString fileName, const bool isRecoding);
    void cameraStatusSlot(const bool isEnable);
    void recvNoneCameraSlot();
    void stopReadFrameSlot();
    void addClientActionSlot(const int action);
};



class CameraThread: public QObject
{
    Q_OBJECT
public:
    ~CameraThread();

private:
    void readframe();
    std::string selectFile();
    cv::Mat frameResize(const cv::Mat srcFrame);
    QImage MatImageToQt(const cv::Mat &src);

public:
    QImage image;

private:
    bool    isCap = false;
    bool    isStop;
    bool    isRecording;
    int     timerDelay  = 50;
    double  fps         = 1000.0/timerDelay;
    const int maxWidth  = 640;
    const int maxHeight = 480;
    const double scale  = 0.5;
    cv::Mat     srcImage;
    cv::Mat     dstImage;
    cv::Mat     tmpFrame;
    cv::Mat     midFrame;
    cv::VideoCapture    cap;
    cv::VideoWriter     vedio;
    std::string         vedioRecordingFileName;

    Detected detected;
    PPtController* ppt = nullptr;
    int action;

signals:
    void noneCamera();
    void sendImage(const QImage image, int action);
    void cameraEnable(const bool IsEnable);

public slots:
    void startCamera();
    void statusCatchFrame(const QString fileName, const bool isRecording);
    void stopReadFrameSlot();
    void addClientActionSlot(const int action);

};

#endif // CAMERATHREADCONTROLLER_H