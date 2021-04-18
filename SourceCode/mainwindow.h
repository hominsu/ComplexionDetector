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

    QTcpSocket* mp_clientSocket = NULL;//��ʼ���յ�socket�ͻ�������

    QTcpServer* mp_TCPServer = NULL;//�����socket
    QTcpSocket* mp_TCPSocket = NULL;

private:
    void connectServer();//����˽�������
    void sendData(const int action);//���ѽ������ӵķ���˷�������

    void InitSocket();//��ʼ�����񣬿�ʼ��ָ���˿ڼ���

signals:
    void addClientActionSignals(const int action);

private slots:
    void setIPSlot(QString str);
    void setSwCancel();

    void ServerReadData();//��������
    void ServerNewConnection();//�����µ�socket����
    void sServerDisConnection();//socket�Ͽ�ʱ�ĶϿ���ʾ����ʵ���ô�
};
