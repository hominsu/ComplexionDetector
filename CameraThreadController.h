#pragma once

#ifndef CAMERATHREADCONTROLLER_H
#define CAMERATHREADCONTROLLER_H

#include <QDebug>
#include <algorithm>
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
#include <stack>


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
    void sendImage(const QImage qimage);
    void cameraEnableSignal(const bool isEnable);
    void noneCameraSignal();

public slots:
    void recvImageSlot(const QImage qImage);    //处理线程执行的结果
    void statusVedioRecordingSlot(const QString fileName, const bool isRecoding);
    void cameraStatusSlot(const bool isEnable);
    void recvNoneCameraSlot();
};



class CameraThread: public QObject
{
    Q_OBJECT
public:
    ~CameraThread();
    void setFlag(bool flag = true);

private:
    void readframe();
    cv::Mat frameResize(const cv::Mat srcFrame);
    QImage MatImageToQt(const cv::Mat &src);
    cv::Mat HCbCr_detect(const cv::Mat& src);
    cv::Mat ellipse_detect(const cv::Mat& src);
    cv::Mat HSV_detector(const cv::Mat& src);
    void Seed_Filling(const cv::Mat& binImg, cv::Mat& labelImg, int& labelNum, int(&ymin)[20], int(&ymax)[20], int(&xmin)[20], int(&xmax)[20]);

public:
    QImage image;

private:
    bool    isStop;
    bool    isRecording;
    int     timerDelay  = 40;
    double  fps         = 1000.0/timerDelay;
    const int maxWidth  = 640;
    const int maxHeight = 480;
    const double scale  = 0.5;
    cv::Mat     srcImage;
    cv::Mat     dstImage;
    cv::Mat     tmpFrame;
    cv::Mat     midFrame;
    QTimer*     timer;
    cv::VideoCapture    cap;
    cv::VideoWriter     vedio;
    std::string         vedioRecordingFileName;

signals:
    void noneCamera();
    void sendImage(const QImage image);
    void cameraEnable(const bool IsEnable);

public slots:
    void startCamera();
    void statusCatchFrame(const QString fileName, const bool isRecording);

};

#endif // CAMERATHREADCONTROLLER_H