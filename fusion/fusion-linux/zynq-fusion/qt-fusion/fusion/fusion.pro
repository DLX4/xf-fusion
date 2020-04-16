QT       += core
QT       -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += opencv
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += /data1/opencv/include/opencv \
             /data1/opencv/include/opencv2 \
             /data1/opencv/include/

LIBS +=  /data1/opencv/lib/libopencv_calib3d.so.2.4 \
        /data1/opencv/lib/libopencv_contrib.so.2.4 \
        /data1/opencv/lib/libopencv_core.so.2.4 \
        /data1/opencv/lib/libopencv_features2d.so.2.4 \
        /data1/opencv/lib/libopencv_flann.so.2.4 \
        /data1/opencv/lib/libopencv_gpu.so.2.4 \
        /data1/opencv/lib/libopencv_highgui.so.2.4 \
        /data1/opencv/lib/libopencv_imgproc.so.2.4 \
        /data1/opencv/lib/libopencv_legacy.so.2.4 \
        /data1/opencv/lib/libopencv_ml.so.2.4 \
        /data1/opencv/lib/libopencv_nonfree.so.2.4 \
        /data1/opencv/lib/libopencv_objdetect.so.2.4 \
        /data1/opencv/lib/libopencv_ocl.so.2.4 \
        /data1/opencv/lib/libopencv_photo.so.2.4 \
        /data1/opencv/lib/libopencv_stitching.so.2.4 \
        /data1/opencv/lib/libopencv_ts.a \
        /data1/opencv/lib/libopencv_video.so.2.4 \
        /data1/opencv/lib/libopencv_videostab.so.2.4 \

HEADERS += \
    laplacian_pyramid.h \
    opencv_utils.h \
    remapping_function.h \
    gaussian_pyramid.h

SOURCES += \
    Pyramids.cpp \
    remapping_function.cpp \
    gaussian_pyramid.cpp \
    laplacian_pyramid.cpp \
    opencv_utils.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /data1/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



DISTFILES += \
    fusion.pro.user
