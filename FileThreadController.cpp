#include "FileThreadController.h"

FileThreadController::FileThreadController(QObject *parent): QObject(parent)
{
    FileThread* file = new FileThread();
    file->moveToThread(&fileThread);

    // 信号发射后启动线程工作
    connect(this, &FileThreadController::fileThreadStartSignal, file, &FileThread::fileRead);
    // 该线程结束时销毁
    connect(&fileThread, &QThread::finished, file, &QObject::deleteLater);
    // 线程结束后发送信号，对结果进行处理
    connect(file, &FileThread::sendFileFrame, this, &FileThreadController::recvFrameSlot);

    fileThread.start();
}

FileThreadController::~FileThreadController()
{
    // 关闭线程
    fileThread.quit();
    fileThread.wait();
}

void FileThreadController::fileThreadStartSlot(const QString fileName)
{
    emit fileThreadStartSignal(fileName);
}

void FileThreadController::recvFrameSlot(const QImage qImage)
{
    emit sendImage(qImage);
}



void FileThread::fileRead(const QString fileName)
{
    // 对输入的文件地址做裁剪，得到文件扩展名
    std::string str = fileName.toStdString();
    size_t lastPath = str.find_last_of("/");
    std::string name = str.substr(lastPath + 1);
    size_t lastExt = name.find_last_of(".");
    std::string ext = name.substr(lastExt + 1);

    // 检测扩展名，分别处理图片和视频
    if(ext == "jpg" || ext == "jpeg")
    {
        startImageRead(str);
    }
    else if(ext == "mp4" || ext == "mkv" || ext == "avi")
    {
        startVideoRead(str);
    }
}

cv::Mat FileThread::frameResize(const cv::Mat srcFrame)
{
    // 检测输入帧，如果大于预设值就resize，知道符合预设值
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

void FileThread::startImageRead(const std::string fileName)
{
    srcImage = cv::imread(fileName);
    // 检测分辨率，违规就resize
    dstImage = MatImageToQt(frameResize(srcImage));
    emit sendFileFrame(dstImage);
}

void FileThread::startVideoRead(const std::string fileName)
{
    // 打开文件
    cap.open(fileName);

    double fps = cap.get(cv::CAP_PROP_FPS);

    double frameCount = cap.get(cv::CAP_PROP_FRAME_COUNT);
    double currentFrame = cap.get(cv::CAP_PROP_POS_FRAMES);

    // 遍历每一帧
    while(currentFrame < frameCount)
    {
        cap.read(srcFrame);
        // 检测分辨率，违规就resize
        dstFrame = MatImageToQt(frameResize(srcFrame));
        emit sendFileFrame(dstFrame);
        currentFrame = cap.get(cv::CAP_PROP_POS_FRAMES);
        cv::waitKey(static_cast<int>(1000/fps));
    }
    cap.release();
}

QImage FileThread::MatImageToQt(const cv::Mat &src)
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
