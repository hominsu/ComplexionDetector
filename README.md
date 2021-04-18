# ComplexionDetector

## 项目概述

​		通过 C++ Qt 开发的具有摄像头采集和媒体文件读取以及网络通讯功能的主界面，通过libtorch来导入pytorch模型来实现模型实时推导，具有较高的简便性和可用性。

## 开发环境

- VS Studio 2019
- Qt 5
- OpenCV 4.5.1
- libtorch 1.8.1

## 测试环境

- Windows 10 专业版 19042.928
- Cuda 10.2
- Cudnn 8.1.1

## 项目结构

```shell
./SourceCode/
├── CameraThreadController.cpp	// camera线程控制
├── CameraThreadController.h
├── Detected	// 模型推导以及NMS
│   ├── detected.cpp
│   └── detected.h
├── FileThreadController.cpp	// media线程控制
├── FileThreadController.h
├── PPTControl	// ppt控制
│   ├── CarryMove.cpp
│   ├── CarryMove.h
│   ├── PPtController.cpp
│   ├── PPtController.h
│   ├── keyboard.cpp
│   ├── keyboard.h
│   ├── mouse.cpp
│   ├── mouse.h
│   ├── pptControl.cpp
│   └── pptControl.h
├── action	// 动作对应显示的图片
│   ├── dianji.png
│   ├── pingyi.png
│   ├── suofang.png
│   ├── xuanzhuan.png
│   └── zhuaqu.png
├── main.cpp
├── mainwindow.cpp	// 主界面
├── mainwindow.h
├── mainwindow.qrc	// 主界面的资源文件
├── mainwindow.ui
├── socketwindow.cpp	// Socket连接对话框
├── socketwindow.h
└── socketwindow.ui

3 directories, 29 files
```

