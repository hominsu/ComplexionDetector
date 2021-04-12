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
    // �������ֻ�������
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
    // ��һ���ļ��Ի���
    QFileDialog* fileDialog = new QFileDialog(this);
    // �����ļ��Ի���Ļ�����Ϣ
    // �����ļ��Ի���ı���
    fileDialog->setWindowTitle(QString::fromLocal8Bit("��"));
    // ���ô򿪵�Ĭ��Ŀ¼
    fileDialog->setDirectory("");
    // ���ù�����
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
    // ���ļ��Ի��򣬻�ȡ�ļ�·��
    selectFile();
    // ѡ���ļ�������tabҳΪ����ʶ��
    ui->tabWidget->setCurrentIndex(1);
    // ��·�����뵽�б༭����
    ui->lineEditFilePath->setText(fileName);
    // ����ʵʱʶ��tab�еĿ�ʼ��ȡ����ť
    ui->pBtnFileStart->setEnabled(true);
    ui->pBtnFileCancel->setEnabled(false);

    // ��ʾ����ppt���ע������
    askPptControl();
}

void MainWindow::askPptControl()
{
    //QString::fromLocal8Bit("�Ƿ����PPT?"),QString::fromLocal8Bit("����PPTʱ��Ҫ��PPT�������ڶ��㣬�����޷���������"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    QString title = QString::fromLocal8Bit("ע��");
    QString text = QString::fromLocal8Bit("����PPTʱ��Ҫ��PPT�������ڶ��㣬��������޷���������\n\n�������Ҫ����PPT����ѡ��PPT�ļ��Ի����е��ȡ������");
    QMessageBox messagebox(QMessageBox::Icon::Information, title, text, QMessageBox::Yes);
    messagebox.exec();
}

void MainWindow::cameraStatusSlot(const bool isEnable)
{
    // ����������̻߳ش���״̬
    if (isEnable)
    {
        // ����Ѿ������������ùر�����ͷ��ť����ͼ��ť��¼�ư�ť
        ui->pBtnCloseCamera->setEnabled(true);
        ui->pBtnCatchPicture->setEnabled(true);
        ui->pBtnRecording->setEnabled(true);
    }
    else if (!isEnable)
    {
        // ���ô�����ͷ��ť
        ui->pBtnOpenCamera->setEnabled(true);
        // ���ڶ�����ǩҳ��
        ui->tabWidget->setTabEnabled(1, true);
        // �����ļ��˵��еĴ�ѡ��
        ui->actionOpenFile->setEnabled(true);
    }
}

void MainWindow::noneCameraSlot()
{
    if (cameraThread)
    {
        // �ر��߳�
        delete cameraThread;
        // �����Ļ
        ui->CameraView->clear();
        // ���ùر�����ͷ��ť����ͼ��ť��¼�ư�ť
        ui->pBtnCloseCamera->setEnabled(false);
        ui->pBtnCatchPicture->setEnabled(false);
        ui->pBtnRecording->setEnabled(false);

        //QMessageBox::critical(this, "title", "text", QMessageBox::No, QMessageBox::Yes);
        QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("û�м�⵽����ͷ"));
        //QMessageBox::warning(this, "error", "��ʾ:��ѡ���ļ���!");
    }
}

void MainWindow::onPBtnOpenCameraSlot()
{
    // ��ʾ����ppt���ע������
    askPptControl();

    // �½�һ������ͷ���Ƴ�Ա
    cameraThread = new CameraThreadController(this);
    // ���ӿ������ķ����źź������ڵĴ����
    connect(cameraThread, &CameraThreadController::sendImage, this, &MainWindow::readImageSlot);
    // ���������ڵ�¼���źźͿ�������¼�Ʋ�
    connect(this, &MainWindow::vedioRecording, cameraThread, &CameraThreadController::statusVedioRecordingSlot);
    connect(cameraThread, &CameraThreadController::cameraEnableSignal, this, &MainWindow::cameraStatusSlot);
    connect(cameraThread, &CameraThreadController::noneCameraSignal, this, &MainWindow::noneCameraSlot);
    connect(this, &MainWindow::stopReadCameraFrameSignal, cameraThread, &CameraThreadController::stopReadFrameSlot);

    // ���ô�����ͷ��ť
    ui->pBtnOpenCamera->setEnabled(false);
    // ���ڶ�����ǩҳ�ر�
    ui->tabWidget->setTabEnabled(1, false);
    // �����ļ��˵��еĴ�ѡ��
    ui->actionOpenFile->setEnabled(false);
}

void MainWindow::onPBtnCloseCameraSlot()
{
    if (cameraThread)
    {
        emit stopReadCameraFrameSignal();
        // �ر��߳�
        delete cameraThread;
        // �����Ļ
        ui->CameraView->clear();
        // ���ùر�����ͷ��ť����ͼ��ť��¼�ư�ť
        ui->pBtnCloseCamera->setEnabled(false);
        ui->pBtnCatchPicture->setEnabled(false);
        ui->pBtnRecording->setEnabled(false);
    }
}

void MainWindow::readImageSlot(const QImage image, int action)
{
    // �����һ֡���ڽ��������ʾ��ͼ��
    ui->actionActionView->clear();

    // ��һ֡��ʾ��label��
    ui->CameraView->setPixmap(QPixmap::fromImage(image));
    cameraImage = image;

    displayAction(action);

    std::cout << "action: " << action << std::endl;
}

void MainWindow::onPBtnCatchPictureSlot()
{
    if (cameraThread != nullptr)
    {
        // ��ȡ��ǰʱ�����ļ���
        QDateTime currentTime = QDateTime::currentDateTime();
        QDir dir(imageCatchingDirectory);
        // ��Ĭ�ϵ�ַ�����·��ת��Ϊ����·��
        dir.makeAbsolute();
        // ���Ŀ¼�����ڴ���Ĭ��Ŀ¼
        if (!dir.exists())
        {
            dir.mkdir(dir.path());
        }
        // д��ͼƬ��·��
        QString pictureName = dir.path();
        pictureName += "/";
        pictureName += currentTime.toString("yyyy_MM_dd_hh_mm_ss_zzz");
        pictureName += ".jpg";
        // �����ͼ
        cameraImage.save(pictureName, "JPG", 100);
    }
}

void MainWindow::onPBtnRecordingSlot()
{
    if (cameraThread != nullptr && isRecording == false)
    {
        isRecording = true;
        // ��ȡ��ǰʱ�����ļ���
        QDateTime currentTime = QDateTime::currentDateTime();
        QDir dir(vedioRecordingDirectory);
        // ��Ĭ�ϵ�ַ�����·��ת��Ϊ����·��
        dir.makeAbsolute();
        // ���Ŀ¼�����ڴ���Ĭ��Ŀ¼
        if (!dir.exists())
        {
            dir.mkdir(dir.path());
        }
        // ƴ�ӳ�¼���ļ����ļ�·��
        vedioName = dir.path();
        vedioName += "/";
        vedioName += currentTime.toString("yyyy_MM_dd_hh_mm_ss_zzz");
        vedioName += ".avi";
        // ����¼���ź�
        emit vedioRecording(vedioName, true);
        // ¼�ư�ť��״̬ת��Ϊ¼���е�״̬
        ui->pBtnRecording->setText(QString::fromLocal8Bit("ֹͣ¼��"));
    }
    else if (cameraThread != nullptr && isRecording == true)
    {
        isRecording = false;
        // ����¼��
        emit vedioRecording(vedioName, false);
        // �л���δ¼�Ƶ�״̬
        ui->pBtnRecording->setText(QString::fromLocal8Bit("¼��"));
    }
}

void MainWindow::onPBtnFileStartSlot()
{
    // �½�һ���ļ���ȡ�߳�
    fileThread = new FileThreadController(this);

    // ����GUI���̵߳��źźͲ�
    connect(this, &MainWindow::fileThreadStartSignal, fileThread, &FileThreadController::fileThreadStartSlot);
    connect(fileThread, &FileThreadController::sendImage, this, &MainWindow::readFrameSlot);
    connect(this, &MainWindow::stopReadFrameSignal, fileThread, &FileThreadController::stopReadFrameSlot);
    // ������ʼ��ȡ���ź�
    emit fileThreadStartSignal(this->fileName);
    // ���ÿ�ʼ��ť��ȡ����ť
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
        // �ر�����
        disconnect(this, &MainWindow::fileThreadStartSignal, fileThread, &FileThreadController::fileThreadStartSlot);
        disconnect(fileThread, &FileThreadController::sendImage, this, &MainWindow::readFrameSlot);
        disconnect(this, &MainWindow::stopReadFrameSignal, fileThread, &FileThreadController::stopReadFrameSlot);

        // �ͷ��߳�
        delete fileThread;

        // ���lable
        ui->FileView->clear();

        // ���ÿ�ʼ��ť��ȡ����ť��״̬
        ui->pBtnFileStart->setEnabled(true);
        ui->tabWidget->setTabEnabled(0, true);
        ui->actionOpenFile->setEnabled(true);
        ui->tBtnOpenFile->setEnabled(true);
    }
}

void MainWindow::readFrameSlot(const QImage frame, int action)
{
    // �����һ֡���ڽ��������ʾ��ͼ��
    ui->actionActionView->clear();

    // ��������̷߳��ص�֡��ʾ��lable��
    ui->FileView->setPixmap(QPixmap::fromImage(frame));

    displayAction(action);

    std::cout << "action: " << action << std::endl;
}

void MainWindow::displayAction(int action)
{
    // ����ͼƬ����
    ui->actionActionView->setAlignment(Qt::AlignCenter);

    // ��ʾ��Ӧ������ͼ��
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
