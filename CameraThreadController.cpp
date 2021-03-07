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
    image = MatImageToQt(ellipse_detect(dstImage));
    // 将处理好的图片发送
    emit sendImage(image);
}

cv::Mat CameraThread::ellipse_detect(const cv::Mat& src)
{
    cv::Mat img, ycrcb_image, gray_image;
    img = src.clone();
    cvtColor(img, gray_image, cv::COLOR_BGR2GRAY);
    cvtColor(img, ycrcb_image, cv::COLOR_BGR2YCrCb);

    cv::Mat skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);
    ellipse(skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2), 43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);
    
    for (int i = 0; i < img.cols; i++)
    {
        for (int j = 0; j < img.rows; j++)
        {
            cv::Vec3b ycrcb = ycrcb_image.at<cv::Vec3b>(j, i);
            if (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0)
            {
                gray_image.at<uchar>(j, i) = 255;
            }
            else
            {
                gray_image.at<uchar>(j, i) = 0;
            }
        }
    }

    erode(gray_image, gray_image, cv::Mat());
    dilate(gray_image, gray_image, cv::Mat());

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    cv::Mat fg;
    cv::morphologyEx(gray_image, fg, cv::MORPH_OPEN, kernel, cv::Point(), 1);
    cv::erode(gray_image, fg, kernel, cv::Point(), 5);

    cv::Mat bg;
    cv::morphologyEx(gray_image, bg, cv::MORPH_CLOSE, kernel, cv::Point(), 1);
    //cv::dilate(gray_image, bg, cv::Mat(), cv::Point(), 5);

    // 超过阈值 则 值变为 0,其他为 128 黑白二值反转(反转二值阈值化)
    cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);

    cv::Mat markers(gray_image.size(), CV_8U, cv::Scalar(0));
    markers = fg + bg;

    //return markers;

    // 分水岭算法
    markers.convertTo(markers, CV_32S);
    cv::watershed(img, markers);
    cv::Mat waterShed;
    markers.convertTo(waterShed, CV_8U);

    // 8向种子算法，给边框做标记
    cv::Mat labelImg;
    int label, ymin[20], ymax[20], xmin[20], xmax[20];
    Seed_Filling(waterShed, labelImg, label, ymin, ymax, xmin, xmax);

    // 统计一下区域中的肤色区域比例
    float fuseratio[20];
    for (int k = 0; k < label; k++)
    {
        fuseratio[k] = 1;
        if (((xmax[k] - xmin[k] + 1) > 50) && ((xmax[k] - xmin[k] + 1) < 300) && ((ymax[k] - ymin[k] + 1) > 150) && ((ymax[k] - ymin[k] + 1) < 450))
        {
            int fusepoint = 0;
            for (int j = ymin[k]; j < ymax[k]; j++)
            {
                uchar* current = waterShed.ptr< uchar>(j);
                for (int i = xmin[k]; i < xmax[k]; i++)
                {
                    if (current[i] == 255)
                        fusepoint++;
                }
            }
            fuseratio[k] = float(fusepoint) / ((xmax[k] - xmin[k] + 1) * (ymax[k] - ymin[k] + 1));
        }
    }

    for (int i = 0; i < label; i++)
    {
        if ((fuseratio[i] < 0.5))
        {
            cv::rectangle(img, cv::Rect(xmin[i], ymin[i], xmax[i] - xmin[i], ymax[i] - ymin[i])
                , cv::Scalar(0, 255, 0), 2, 8);
        }
    }

    return img;
}

cv::Mat CameraThread::HCbCr_detect(const cv::Mat& src)
{
    cv::Mat img = src.clone();
    cv::Mat hsv_image,ycrcb_image, gray_image;
    std::vector<cv::Mat> hsv_channels, ycrcb_channels;
    cv::Mat Y, Cr, Cb, H;

    cvtColor(img, gray_image, cv::COLOR_BGR2GRAY);  //转换为灰度图
    cvtColor(img, hsv_image, cv::COLOR_BGR2HSV);    //转换成到HSV空间
    cvtColor(img, ycrcb_image, cv::COLOR_BGR2YCrCb);    //转换为YCrCb空间

    split(ycrcb_image, ycrcb_channels);
    split(hsv_image, hsv_channels);

    Cr = ycrcb_channels.at(1);
    Cb = ycrcb_channels.at(2);
    H = hsv_channels.at(0);


    for (int j = 1; j < Cr.rows - 1; j++)	// 遍历图像像素点
    {
        uchar* currentCr = Cr.ptr< uchar>(j);
        uchar* currentCb = Cb.ptr< uchar>(j);
        uchar* currentH = H.ptr< uchar>(j);
        uchar* current = gray_image.ptr< uchar>(j);

        for (int i = 1; i < Cb.cols - 1; i++)
        {
            if ((currentCr[i] >= 135) && (currentCr[i] <= 170) && (currentCb[i] >= 94) && (currentCb[i] <= 125) && (currentH[i] >= 1) && (currentH[i] <= 23))
                current[i] = 255;
            else
                current[i] = 0;
        }
    }

    erode(gray_image, gray_image, cv::Mat());
    dilate(gray_image, gray_image, cv::Mat());

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    cv::Mat fg;
    cv::morphologyEx(gray_image, fg, cv::MORPH_OPEN, kernel, cv::Point(), 1);
    cv::erode(gray_image, fg, kernel, cv::Point(), 5);

    cv::Mat bg;
    cv::morphologyEx(gray_image, bg, cv::MORPH_CLOSE, kernel, cv::Point(), 1);
    //cv::dilate(gray_image, bg, cv::Mat(), cv::Point(), 5);

    // 超过阈值 则 值变为 0,其他为 128 黑白二值反转(反转二值阈值化)
    cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);

    cv::Mat markers(gray_image.size(), CV_8U, cv::Scalar(0));
    markers = fg + bg;

    //return markers;

    // 分水岭算法
    markers.convertTo(markers, CV_32S);
    cv::watershed(img, markers);
    cv::Mat waterShed;
    markers.convertTo(waterShed, CV_8U);

    // 8向种子算法，给边框做标记
    cv::Mat labelImg;
    int label, ymin[20], ymax[20], xmin[20], xmax[20];
    Seed_Filling(waterShed, labelImg, label, ymin, ymax, xmin, xmax);

    // 统计一下区域中的肤色区域比例
    float fuseratio[20];
    for (int k = 0; k < label; k++)
    {
        fuseratio[k] = 1;
        if (((xmax[k] - xmin[k] + 1) > 50) && ((xmax[k] - xmin[k] + 1) < 300) && ((ymax[k] - ymin[k] + 1) > 150) && ((ymax[k] - ymin[k] + 1) < 450))
        {
            int fusepoint = 0;
            for (int j = ymin[k]; j < ymax[k]; j++)
            {
                uchar* current = waterShed.ptr< uchar>(j);
                for (int i = xmin[k]; i < xmax[k]; i++)
                {
                    if (current[i] == 255)
                        fusepoint++;
                }
            }
            fuseratio[k] = float(fusepoint) / ((xmax[k] - xmin[k] + 1) * (ymax[k] - ymin[k] + 1));
        }
    }

    for (int i = 0; i < label; i++)
    {
        if ((fuseratio[i] < 0.5))
        {
            cv::rectangle(img, cv::Rect(xmin[i], ymin[i], xmax[i] - xmin[i], ymax[i] - ymin[i])
                , cv::Scalar(0, 255, 0), 2, 8);
        }
    }

    return img;
}

cv::Mat CameraThread::HSV_detector(const cv::Mat& src)
{
    cv::Mat img, hsv_image, gray_image;
    img = src.clone();
    cvtColor(img, gray_image, cv::COLOR_BGR2GRAY);
    cvtColor(img, hsv_image, cv::COLOR_BGR2HSV); //首先转换成到HSV空间

    cv::Mat output_mask = cv::Mat::zeros(src.size(), CV_8UC1);
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            cv::Vec3b hsv = hsv_image.at<cv::Vec3b>(i, j);
            if (hsv[0] >= 0 && hsv[0] <= 20 && hsv[1] >= 48 && hsv[2] >= 50)
            {
                gray_image.at<uchar>(i, j) = 255;
            }
            else
            {
                gray_image.at<uchar>(i, j) = 0;
            }
        }
    }
    
    erode(gray_image, gray_image, cv::Mat());
    dilate(gray_image, gray_image, cv::Mat());

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    cv::Mat fg;
    cv::morphologyEx(gray_image, fg, cv::MORPH_OPEN, kernel, cv::Point(), 1);
    cv::erode(gray_image, fg, kernel, cv::Point(), 5);

    cv::Mat bg;
    cv::morphologyEx(gray_image, bg, cv::MORPH_CLOSE, kernel, cv::Point(), 1);
    //cv::dilate(gray_image, bg, cv::Mat(), cv::Point(), 5);

    // 超过阈值 则 值变为 0,其他为 128 黑白二值反转(反转二值阈值化)
    cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);

    cv::Mat markers(gray_image.size(), CV_8U, cv::Scalar(0));
    markers = fg + bg;

    //return markers;

    // 分水岭算法
    markers.convertTo(markers, CV_32S);
    cv::watershed(img, markers);
    cv::Mat waterShed;
    markers.convertTo(waterShed, CV_8U);

    // 8向种子算法，给边框做标记
    cv::Mat labelImg;
    int label, ymin[20], ymax[20], xmin[20], xmax[20];
    Seed_Filling(waterShed, labelImg, label, ymin, ymax, xmin, xmax);

    // 统计一下区域中的肤色区域比例
    float fuseratio[20];
    for (int k = 0; k < label; k++)
    {
        fuseratio[k] = 1;
        if (((xmax[k] - xmin[k] + 1) > 50) && ((xmax[k] - xmin[k] + 1) < 300) && ((ymax[k] - ymin[k] + 1) > 150) && ((ymax[k] - ymin[k] + 1) < 450))
        {
            int fusepoint = 0;
            for (int j = ymin[k]; j < ymax[k]; j++)
            {
                uchar* current = waterShed.ptr< uchar>(j);
                for (int i = xmin[k]; i < xmax[k]; i++)
                {
                    if (current[i] == 255)
                        fusepoint++;
                }
            }
            fuseratio[k] = float(fusepoint) / ((xmax[k] - xmin[k] + 1) * (ymax[k] - ymin[k] + 1));
        }
    }

    for (int i = 0; i < label; i++)
    {
        if ((fuseratio[i] < 0.5))
        {
            cv::rectangle(img, cv::Rect(xmin[i], ymin[i], xmax[i] - xmin[i], ymax[i] - ymin[i])
                , cv::Scalar(0, 255, 0), 2, 8);
        }
    }

    return img;
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

// 八邻接种子算法，并返回每块区域的边缘框
void CameraThread::Seed_Filling(const cv::Mat& binImg, cv::Mat& labelImg, int& labelNum, int(&ymin)[20], int(&ymax)[20], int(&xmin)[20], int(&xmax)[20])
{
    if (binImg.empty() ||
        binImg.type() != CV_8UC1)// 如果图像是空或者格式不正确就返回
    {
        return;
    }

    labelImg.release();
    binImg.convertTo(labelImg, CV_32SC1);// 矩阵数据类型转换
    int label = 0;
    int rows = binImg.rows;
    int cols = binImg.cols;
    for (int i = 1; i < rows - 1; i++)
    {
        int* data = labelImg.ptr<int>(i);
        for (int j = 1; j < cols - 1; j++)
        {

            if (data[j] == 0)
            {
                std::stack<std::pair<int, int>> neighborPixels;// std::pair主要的作用是将两个数据组合成一个数据，两个数据可以是同一类型或者不同类型。
                                                               // pair是一个模板结构体，其主要的两个成员变量是first和second，这两个变量可以直接使用。
                neighborPixels.push(std::pair<int, int>(j, i));// 向栈顶插入元素像素位置: <j,i>
                ymin[label] = i;
                ymax[label] = i;
                xmin[label] = j;
                xmax[label] = j;
                while (!neighborPixels.empty())
                {
                    std::pair<int, int> curPixel = neighborPixels.top();// 如果与上一行中一个团有重合区域，则将上一行的那个团的标号赋给它 
                    int curX = curPixel.first;
                    int curY = curPixel.second;
                    labelImg.at<int>(curY, curX) = 255;
                    neighborPixels.pop();	// 出栈

                    if ((curX > 0) && (curY > 0) && (curX < (cols - 1)) && (curY < (rows - 1)))
                    {
                        if (labelImg.at<int>(curY - 1, curX) == 0)                      //上
                        {
                            neighborPixels.push(std::pair<int, int>(curX, curY - 1));
                            //ymin[label] = curY - 1;
                        }

                        if (labelImg.at<int>(curY + 1, curX) == 0)                      //下
                        {
                            neighborPixels.push(std::pair<int, int>(curX, curY + 1));
                            if ((curY + 1) > ymax[label])
                                ymax[label] = curY + 1;
                        }

                        if (labelImg.at<int>(curY, curX - 1) == 0)                      //左
                        {
                            neighborPixels.push(std::pair<int, int>(curX - 1, curY));
                            if ((curX - 1) < xmin[label])
                                xmin[label] = curX - 1;
                        }

                        if (labelImg.at<int>(curY, curX + 1) == 0)                      //右
                        {
                            neighborPixels.push(std::pair<int, int>(curX + 1, curY));
                            if ((curX + 1) > xmax[label])
                                xmax[label] = curX + 1;
                        }

                        if (labelImg.at<int>(curY - 1, curX - 1) == 0)                  //左上
                        {
                            neighborPixels.push(std::pair<int, int>(curX - 1, curY - 1));
                            //ymin[label] = curY - 1;
                            if ((curX - 1) < xmin[label])
                                xmin[label] = curX - 1;
                        }
                        if (labelImg.at<int>(curY + 1, curX + 1) == 0)                  //右下
                        {
                            neighborPixels.push(std::pair<int, int>(curX + 1, curY + 1));
                            if ((curY + 1) > ymax[label])
                                ymax[label] = curY + 1;
                            if ((curX + 1) > xmax[label])
                                xmax[label] = curX + 1;
                        }

                        if (labelImg.at<int>(curY + 1, curX - 1) == 0)                  //左下
                        {
                            neighborPixels.push(std::pair<int, int>(curX - 1, curY + 1));
                            if ((curY + 1) > ymax[label])
                                ymax[label] = curY + 1;
                            if ((curX - 1) < xmin[label])
                                xmin[label] = curX - 1;
                        }

                        if (labelImg.at<int>(curY - 1, curX + 1) == 0)                  //右上
                        {
                            neighborPixels.push(std::pair<int, int>(curX + 1, curY - 1));
                            //ymin[label] = curY - 1;
                            if ((curX + 1) > xmax[label])
                                xmax[label] = curX + 1;
                        }
                    }
                }
                ++label; // 没有重复的团，开始新的标签
            }
        }
    }
    labelNum = label;
}
