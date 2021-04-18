#pragma once

#include <QtWidgets/QMainWindow>
#include <QDir>
#include <QAction>
#include <QDateTime>
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>

#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

#undef slots
#include <torch/script.h>
#define slots Q_SLOTS

#include "socketwindow.h"
#include "ui_mainwindow.h"
#include "CameraThreadController.h"
#include "FileThreadController.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void createConnect();
    void selectFile();
    void askPptControl();
    void askClient();
    bool askNoneCuda();
    void displayAction(int action);


private:
    Ui::MainWindow* ui;
    CameraThreadController* cameraThread;
    FileThreadController* fileThread;
    QString     fileName;
    QString     vedioName;
    QImage      cameraImage;
    QString     imageCatchingDirectory = "images/";
    QString     vedioRecordingDirectory = "vedios/";
    bool        isRecording = false;

signals:
    void sendImageDetected(const QImage qimage);
    void vedioRecording(const QString fileName, const bool isRecording);
    void fileThreadStartSignal(const QString fileName);
    void stopReadCameraFrameSignal();
    void stopReadFrameSignal();

private slots:
    void fileOpenActionSlot();
    void onPBtnOpenCameraSlot();
    void onPBtnCloseCameraSlot();
    void onPBtnCatchPictureSlot();
    void onPBtnRecordingSlot();
    void onPBtnFileStartSlot();
    void onPBtnFileCancelSlot();
    void readImageSlot(const QImage image, int action);
    void readFrameSlot(const QImage frame, int action);
    void cameraStatusSlot(const bool isEnable);
    void noneCameraSlot();


private:
    bool isClient = true;
    QString ip;
    const int port = 3490;

    SocketWindow* sw;

    QTcpSocket* mp_clientSocket = NULL;//初始化空的socket客户端连接

    QTcpServer* mp_TCPServer = NULL;//服务端socket
    QTcpSocket* mp_TCPSocket = NULL;

private:
    void connectServer();//服务端建立连接
    void sendData(const int action);//给已建立连接的服务端发送数据

    void InitSocket();//初始化服务，开始在指定端口监听

signals:
    void addClientActionSignals(const int action);

private slots:
    void setIPSlot(QString str);
    void setSwCancel();

    void ServerReadData();//接收数据
    void ServerNewConnection();//建立新的socket连接
    void sServerDisConnection();//socket断开时的断开提示，无实际用处
};
