#include "CameraThreadController.h"


CameraThreadController::CameraThreadController(QObject *parent): QObject(parent)
{
    // 新建一个摄像头线程
    CameraThread* camera = new CameraThread();
    // 调用moveToThread将该任务交给workThread
    camera->moveToThread(&cameraThread);

    // 信号发射后启动线程工作
    connect(this, &CameraThreadController::cameraThreadStart, camera, &CameraThread::startCamera);
    // 该线程结束时销毁
    connect(&cameraThread, &QThread::finished, camera, &QObject::deleteLater);
    // 线程结束后发送信号，对结果进行处理
    connect(camera, &CameraThread::sendImage, this, &CameraThreadController::recvImageSlot);
    connect(this, &CameraThreadController::statusCameraThreadCatchFrame, camera, &CameraThread::statusCatchFrame);
    connect(camera, &CameraThread::cameraEnable, this, &CameraThreadController::cameraStatusSlot);
    connect(camera, &CameraThread::noneCamera, this, &CameraThreadController::recvNoneCameraSlot);

    //启动线程
    cameraThread.start();
    emit cameraThreadStart();
}

void CameraThreadController::recvNoneCameraSlot()
{
    emit noneCameraSignal();
}

void CameraThreadController::cameraStatusSlot(const bool isEnable)
{
    emit cameraEnableSignal(isEnable);
}

void CameraThreadController::recvImageSlot(const QImage qImage)
{
    emit sendImage(qImage);
}

void CameraThreadController::statusVedioRecordingSlot(const QString fileName, const bool isRecoding)
{
    emit statusCameraThreadCatchFrame(fileName, isRecoding);
}

CameraThreadController::~CameraThreadController()
{
    emit cameraEnableSignal(false);
    cameraThread.quit();
    cameraThread.wait();
}



CameraThread::~CameraThread()
{
    timer->stop();
    cap.release();
    delete timer;
}

void CameraThread::statusCatchFrame(const QString fileName, const bool _isRecording)
{
    if(_isRecording)
    {
        // 创建videowriter，用于写入帧
        vedioRecordingFileName = fileName.toStdString();
        vedio.open(vedioRecordingFileName,
                    cv::VideoWriter::fourcc('D', 'I', 'V', 'X'),
                    fps,
                    cv::Size(dstImage.cols, dstImage.rows),
                    true);
        this->isRecording = _isRecording;
    }
    else if(!_isRecording)
    {
        // 释放videowriter
        this->isRecording = _isRecording;
        vedio.release();
    }
}

void CameraThread::startCamera()
{
    // 新建一个计时器
    timer = new QTimer();
    // 连接计数器和帧读取函数
    connect(timer, &QTimer::timeout, this, &CameraThread::readframe);
    // 打开摄像头
    cap.open(cv::CAP_DSHOW);
    if (!cap.isOpened())
    {
        emit noneCamera();
        return;
    }
    // 发送打开信号
    emit cameraEnable(true);
    // 设定定时器，即为帧率为：1000/34 fps
    timer->start(timerDelay);
}

cv::Mat CameraThread::frameResize(const cv::Mat srcFrame)
{
    // 检测输入帧的分辨率，如果大于指定的大小就resize，直到满足要求
    midFrame = srcFrame;
    if(midFrame.cols > maxWidth && midFrame.rows >maxHeight)
    {
        while(midFrame.cols > maxWidth && midFrame.rows >maxHeight)
        {
            cv::resize(midFrame, tmpFrame,
                       cv::Size(static_cast<int>(midFrame.cols*scale),
                                static_cast<int>(midFrame.rows*scale)));
            midFrame = tmpFrame;
        }
    }
    else
    {
        return srcFrame;
    }
    return midFrame;
}

void CameraThread::readframe()
{
    // 从摄像头中读取一帧
    cap.read(srcImage);
    // 如果处于录制状态就将帧压入videowriter
    if(this->isRecording)
    {
        vedio << srcImage;
    }
    // 进行镜像操作
    cv::flip(frameResize(srcImage), dstImage, 1);
    // 将opencv的mat转换为qimage
    //image = MatImageToQt(ellipse_detect(dstImage));
    image = MatImageToQt(dstImage);
    // 将处理好的图片发送
    emit sendImage(image);
}

QImage CameraThread::MatImageToQt(const cv::Mat &src)
{
    // 将cv的mat转换为qt的qimage
    // CV_8UC1 8位无符号的单通道---灰度图片
    if(src.type() == CV_8UC1)
    {
        // 使用给定的大小和格式构造图像
        // QImage(int width, int height, Format format)
        QImage qImage(src.cols,src.rows,QImage::Format_Indexed8);
        // 扩展颜色表的颜色数目
        qImage.setColorCount(256);

        // 在给定的索引设置颜色
        for(int i = 0; i < 256; i ++)
        {
            // 得到一个黑白图
            qImage.setColor(i,qRgb(i,i,i));
        }
        // 复制输入图像,data数据段的首地址
        uchar *pSrc = src.data;
        for(int row = 0; row < src.rows; row ++)
        {
            // 遍历像素指针
            uchar *pDest = qImage.scanLine(row);
            // 从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中
            memcpy(pDest, pSrc, static_cast<size_t>(src.cols));
            // 图像层像素地址
            pSrc += src.step;
        }
        return qImage;
    }
    // 为3通道的彩色图片
    else if(src.type() == CV_8UC3)
    {
        // 得到图像的的首地址
        const uchar *pSrc = static_cast<const uchar*>(src.data);
        // 以src构造图片
        QImage qImage(pSrc, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_RGB888);
        // 在不改变实际图像数据的条件下，交换红蓝通道
        return qImage.rgbSwapped();
    }
    // 四通道图片，带Alpha通道的RGB彩色图像
    else if(src.type() == CV_8UC4)
    {
        const uchar *pSrc = static_cast<const uchar*>(src.data);
        QImage qImage(pSrc, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_ARGB32);
        // 返回图像的子区域作为一个新图像
        return qImage.copy();
    }
    else
    {
        return QImage();
    }
}
