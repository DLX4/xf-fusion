QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
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

SOURCES += \
    Pyramids.cpp \
    gaussian_pyramid.cpp \
    laplacian_pyramid.cpp \
    opencv_utils.cpp \
    remapping_function.cpp

HEADERS += \
    gaussian_pyramid.h \
    laplacian_pyramid.h \
    opencv_utils.h \
    remapping_function.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    fusion.pro.user
