#pragma once

#include <QtWidgets/QMainWindow>
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
    void vedioRecording(const QString fileName, const bool isRecording);
    void fileThreadStartSignal(const QString fileName);

private slots:
    void fileOpenActionSlot();
    void onPBtnOpenCameraSlot();
    void onPBtnCloseCameraSlot();
    void onPBtnCatchPictureSlot();
    void onPBtnRecordingSlot();
    void onPBtnFileStartSlot();
    void onPBtnFileCancelSlot();
    void readImageSlot(const QImage image);
    void readFrameSlot(const QImage frame);
    void cameraStatusSlot(const bool isEnable);
    void noneCameraSlot();
};
