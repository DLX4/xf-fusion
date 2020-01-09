QT += core gui

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += opencv

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-opencv-opencl-opengl-multithreaded
TEMPLATE = app

DEFINES += APP_VERSION=\\\"1.3.3\\\"

INCLUDEPATH += D:\opencv24\opencv24-qt5\include\opencv \
             D:\opencv24\opencv24-qt5\include\opencv2 \
             D:\opencv24\opencv24-qt5\include


LIBS +=  D:\opencv24\opencv24-qt5\lib\libopencv_calib3d2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_contrib2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_core2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_features2d2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_flann2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_gpu2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_highgui2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_imgproc2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_legacy2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_ml2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_nonfree2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_objdetect2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_ocl2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_photo2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_stitching2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_ts2411.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_video2411.dll.a \
        D:\opencv24\opencv24-qt5\lib\libopencv_videostab2411.dll.a \

LIBS += -lOpenCL -locl_util

SOURCES += main.cpp \
    MainWindow.cpp \
    MatToQImage.cpp \
    FrameLabel.cpp \
    CameraView.cpp \
    ProcessingThread.cpp \
    CaptureThread.cpp \
    CameraGrab.cpp \
    CameraConnectDialog.cpp \
    ImageProcessingSettingsDialog.cpp \
    SharedImageBuffer.cpp \
    Gesture.cpp \
    WaveSimulationThread.cpp \
    WaveDisplayWidget.cpp \
    GeometryEngine.cpp

HEADERS += \
    MainWindow.h \
    Config.h \
    MatToQImage.h \
    FrameLabel.h \
    Structures.h \
    CameraView.h \
    ProcessingThread.h \
    CaptureThread.h \
    CameraGrab.h \
    CameraConnectDialog.h \
    ImageProcessingSettingsDialog.h \
    SharedImageBuffer.h \
    Buffer.h \
    Gesture.h \
    WaveSimulationThread.h \
    phillips.dsp_h \
    WaveDisplayWidget.h \
    GeometryEngine.h

FORMS += \
    MainWindow.ui \
    CameraView.ui \
    CameraConnectDialog.ui \
    ImageProcessingSettingsDialog.ui

RESOURCES += \
    shaders.qrc \
    textures.qrc

QMAKE_CXXFLAGS += -Wall
