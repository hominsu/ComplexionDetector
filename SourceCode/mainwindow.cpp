#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createConnect();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createConnect()
{
    // 建立部分基础连接
    connect(ui->tBtnOpenFile, &QToolButton::clicked, this, &MainWindow::fileOpenActionSlot);
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::fileOpenActionSlot);
    connect(ui->pBtnOpenCamera, &QPushButton::clicked, this, &MainWindow::onPBtnOpenCameraSlot);
    connect(ui->pBtnCloseCamera, &QPushButton::clicked, this, &MainWindow::onPBtnCloseCameraSlot);
    connect(ui->pBtnCatchPicture, &QPushButton::clicked, this, &MainWindow::onPBtnCatchPictureSlot);
    connect(ui->pBtnRecording, &QPushButton::clicked, this, &MainWindow::onPBtnRecordingSlot);
    connect(ui->pBtnFileStart, &QPushButton::clicked, this, &MainWindow::onPBtnFileStartSlot);
    connect(ui->pBtnFileCancel, &QPushButton::clicked, this, &MainWindow::onPBtnFileCancelSlot);
}

void MainWindow::selectFile()
{
    // 打开一个文件对话框
    QFileDialog* fileDialog = new QFileDialog(this);
    // 设置文件对话框的基本信息
    // 设置文件对话框的标题
    fileDialog->setWindowTitle(QString::fromLocal8Bit("打开"));
    // 设置打开的默认目录
    fileDialog->setDirectory("");
    // 设置过滤器
    QStringList nameFilters;
    nameFilters << QString::fromLocal8Bit("Images (*.jpg *.jpeg)")
        << QString::fromLocal8Bit("Vedio (*.mp4 *.mkv *avi)");
    fileDialog->setNameFilters(nameFilters);

    QStringList _fileName;
    if (fileDialog->exec())
    {
        _fileName = fileDialog->selectedFiles();
    }

    if (!_fileName.isEmpty())
    {
        this->fileName = _fileName[0];
    }
}

void MainWindow::fileOpenActionSlot()
{
    // 打开文件对话框，获取文件路径
    selectFile();
    // 选择文件后，设置tab页为本地识别
    ui->tabWidget->setCurrentIndex(1);
    // 将路径加入到行编辑栏中
    ui->lineEditFilePath->setText(fileName);
    // 启用实时识别tab中的开始和取消按钮
    ui->pBtnFileStart->setEnabled(true);
    ui->pBtnFileCancel->setEnabled(false);

    // 提示控制ppt相关注意事项
    askPptControl();
}

void MainWindow::askPptControl()
{
    //QString::fromLocal8Bit("是否控制PPT?"),QString::fromLocal8Bit("控制PPT时需要将PPT程序置于顶层，否则无法接收命令"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    QString title = QString::fromLocal8Bit("注意");
    QString text = QString::fromLocal8Bit("控制PPT时需要将PPT程序置于顶层，否则程序无法接收命令\n\n如果不需要控制PPT，在选择PPT文件对话框中点击取消即可");
    QMessageBox messagebox(QMessageBox::Icon::Information, title, text, QMessageBox::Yes);
    messagebox.exec();
}

void MainWindow::cameraStatusSlot(const bool isEnable)
{
    // 处理摄像机线程回传的状态
    if (isEnable)
    {
        // 如果已经开启，就启用关闭摄像头按钮、截图按钮、录制按钮
        ui->pBtnCloseCamera->setEnabled(true);
        ui->pBtnCatchPicture->setEnabled(true);
        ui->pBtnRecording->setEnabled(true);
    }
    else if (!isEnable)
    {
        // 启用打开摄像头按钮
        ui->pBtnOpenCamera->setEnabled(true);
        // 将第二个标签页打开
        ui->tabWidget->setTabEnabled(1, true);
        // 启用文件菜单中的打开选项
        ui->actionOpenFile->setEnabled(true);
    }
}

void MainWindow::noneCameraSlot()
{
    if (cameraThread)
    {
        // 关闭线程
        delete cameraThread;
        // 清空屏幕
        ui->CameraView->clear();
        // 禁用关闭摄像头按钮、截图按钮、录制按钮
        ui->pBtnCloseCamera->setEnabled(false);
        ui->pBtnCatchPicture->setEnabled(false);
        ui->pBtnRecording->setEnabled(false);

        //QMessageBox::critical(this, "title", "text", QMessageBox::No, QMessageBox::Yes);
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("没有检测到摄像头"));
        //QMessageBox::warning(this, "error", "提示:请选择文件夹!");
    }
}

void MainWindow::onPBtnOpenCameraSlot()
{
    // 提示控制ppt相关注意事项
    askPptControl();

    // 新建一个摄像头控制成员
    cameraThread = new CameraThreadController(this);
    // 连接控制器的发送信号和主窗口的处理槽
    connect(cameraThread, &CameraThreadController::sendImage, this, &MainWindow::readImageSlot);
    // 连接主窗口的录制信号和控制器的录制槽
    connect(this, &MainWindow::vedioRecording, cameraThread, &CameraThreadController::statusVedioRecordingSlot);
    connect(cameraThread, &CameraThreadController::cameraEnableSignal, this, &MainWindow::cameraStatusSlot);
    connect(cameraThread, &CameraThreadController::noneCameraSignal, this, &MainWindow::noneCameraSlot);
    connect(this, &MainWindow::stopReadCameraFrameSignal, cameraThread, &CameraThreadController::stopReadFrameSlot);

    // 禁用打开摄像头按钮
    ui->pBtnOpenCamera->setEnabled(false);
    // 将第二个标签页关闭
    ui->tabWidget->setTabEnabled(1, false);
    // 禁用文件菜单中的打开选项
    ui->actionOpenFile->setEnabled(false);
}

void MainWindow::onPBtnCloseCameraSlot()
{
    if (cameraThread)
    {
        emit stopReadCameraFrameSignal();
        // 关闭线程
        delete cameraThread;
        // 清空屏幕
        ui->CameraView->clear();
        // 禁用关闭摄像头按钮、截图按钮、录制按钮
        ui->pBtnCloseCamera->setEnabled(false);
        ui->pBtnCatchPicture->setEnabled(false);
        ui->pBtnRecording->setEnabled(false);
    }
}

void MainWindow::readImageSlot(const QImage image, int action)
{
    // 清空上一帧中在结果框中显示的图标
    ui->actionActionView->clear();

    // 将一帧显示到label中
    ui->CameraView->setPixmap(QPixmap::fromImage(image));
    cameraImage = image;

    displayAction(action);

    std::cout << "action: " << action << std::endl;
}

void MainWindow::onPBtnCatchPictureSlot()
{
    if (cameraThread != nullptr)
    {
        // 获取当前时间做文件名
        QDateTime currentTime = QDateTime::currentDateTime();
        QDir dir(imageCatchingDirectory);
        // 将默认地址的相对路径转换为绝对路劲
        dir.makeAbsolute();
        // 如果目录不存在创建默认目录
        if (!dir.exists())
        {
            dir.mkdir(dir.path());
        }
        // 写入图片的路径
        QString pictureName = dir.path();
        pictureName += "/";
        pictureName += currentTime.toString("yyyy_MM_dd_hh_mm_ss_zzz");
        pictureName += ".jpg";
        // 保存截图
        cameraImage.save(pictureName, "JPG", 100);
    }
}

void MainWindow::onPBtnRecordingSlot()
{
    if (cameraThread != nullptr && isRecording == false)
    {
        isRecording = true;
        // 获取当前时间做文件名
        QDateTime currentTime = QDateTime::currentDateTime();
        QDir dir(vedioRecordingDirectory);
        // 将默认地址的相对路径转换为绝对路劲
        dir.makeAbsolute();
        // 如果目录不存在创建默认目录
        if (!dir.exists())
        {
            dir.mkdir(dir.path());
        }
        // 拼接出录制文件的文件路径
        vedioName = dir.path();
        vedioName += "/";
        vedioName += currentTime.toString("yyyy_MM_dd_hh_mm_ss_zzz");
        vedioName += ".avi";
        // 触发录制信号
        emit vedioRecording(vedioName, true);
        // 录制按钮的状态转换为录制中的状态
        ui->pBtnRecording->setText(QString::fromLocal8Bit("停止录制"));
    }
    else if (cameraThread != nullptr && isRecording == true)
    {
        isRecording = false;
        // 结束录制
        emit vedioRecording(vedioName, false);
        // 切换回未录制的状态
        ui->pBtnRecording->setText(QString::fromLocal8Bit("录制"));
    }
}

void MainWindow::onPBtnFileStartSlot()
{
    // 新建一个文件读取线程
    fileThread = new FileThreadController(this);

    // 连接GUI和线程的信号和槽
    connect(this, &MainWindow::fileThreadStartSignal, fileThread, &FileThreadController::fileThreadStartSlot);
    connect(fileThread, &FileThreadController::sendImage, this, &MainWindow::readFrameSlot);
    connect(this, &MainWindow::stopReadFrameSignal, fileThread, &FileThreadController::stopReadFrameSlot);
    // 触发开始读取的信号
    emit fileThreadStartSignal(this->fileName);
    // 设置开始按钮和取消按钮
    ui->pBtnFileStart->setEnabled(false);
    ui->pBtnFileCancel->setEnabled(true);
    ui->tabWidget->setTabEnabled(0, false);
    ui->actionOpenFile->setEnabled(false);
    ui->tBtnOpenFile->setEnabled(false);

}

void MainWindow::onPBtnFileCancelSlot()
{
    if (fileThread)
    {
        ui->pBtnFileCancel->setEnabled(false);

        emit stopReadFrameSignal();
        // 关闭连接
        disconnect(this, &MainWindow::fileThreadStartSignal, fileThread, &FileThreadController::fileThreadStartSlot);
        disconnect(fileThread, &FileThreadController::sendImage, this, &MainWindow::readFrameSlot);
        disconnect(this, &MainWindow::stopReadFrameSignal, fileThread, &FileThreadController::stopReadFrameSlot);

        // 释放线程
        delete fileThread;

        // 清空lable
        ui->FileView->clear();

        // 设置开始按钮和取消按钮的状态
        ui->pBtnFileStart->setEnabled(true);
        ui->tabWidget->setTabEnabled(0, true);
        ui->actionOpenFile->setEnabled(true);
        ui->tBtnOpenFile->setEnabled(true);
    }
}

void MainWindow::readFrameSlot(const QImage frame, int action)
{
    // 清空上一帧中在结果框中显示的图标
    ui->actionActionView->clear();

    // 将摄像机线程返回的帧显示在lable中
    ui->FileView->setPixmap(QPixmap::fromImage(frame));

    displayAction(action);

    std::cout << "action: " << action << std::endl;
}

void MainWindow::displayAction(int action)
{
    // 设置图片居中
    ui->actionActionView->setAlignment(Qt::AlignCenter);

    // 显示对应动作的图标
    QPixmap pixmap;
    switch (action)
    {
    case 0:
        pixmap = QPixmap(":/action/action/dianji.png");
        ui->actionActionView->setPixmap(pixmap.scaled(ui->actionActionView->width() / 2,
            ui->actionActionView->width() / 2,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
        break;

    case 1:
        pixmap = QPixmap(":/action/action/pingyi.png");
        ui->actionActionView->setPixmap(pixmap.scaled(ui->actionActionView->width() / 2,
            ui->actionActionView->width() / 2,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
        break;

    case 2:
        pixmap = QPixmap(":/action/action/suofang.png");
        ui->actionActionView->setPixmap(pixmap.scaled(ui->actionActionView->width() / 2,
            ui->actionActionView->width() / 2,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
        break;

    case 3:
        pixmap = QPixmap(":/action/action/zhuaqu.png");
        ui->actionActionView->setPixmap(pixmap.scaled(ui->actionActionView->width() / 2,
            ui->actionActionView->width() / 2,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
        break;

    case 4:
        pixmap = QPixmap(":/action/action/xuanzhuan.png");
        ui->actionActionView->setPixmap(pixmap.scaled(ui->actionActionView->width() / 2,
            ui->actionActionView->width() / 2,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
        break;

    default:
        break;
    }
}
