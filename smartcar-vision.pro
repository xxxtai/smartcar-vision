QT += core
QT -= gui

TARGET = smartcar-vision
CONFIG += console
CONFIG -= app_bundle

CONFIG += C++11

TEMPLATE = app

SOURCES += main.cpp \
    dector.cpp \
#    tx2uart.cpp
#    SerialPort.cpp \
#    stdafx.cpp

HEADERS += \
    dector.h \
#    tx2uart.h
#    SerialPort.h \
#    stdafx.h \
#    targetver.h

#INCLUDEPATH += /usr/include \
#/usr/include/opencv \
#/usr/include/opencv2

#LIBS += /usr/lib/libopencv_calib3d.so \
#/usr/lib/libopencv_core.so \
#/usr/lib/libopencv_features2d.so \
#/usr/lib/libopencv_flann.so \
#/usr/lib/libopencv_highgui.so \
#/usr/lib/libopencv_imgcodecs.so \
#/usr/lib/libopencv_imgproc.so \
#/usr/lib/libopencv_ml.so \
#/usr/lib/libopencv_objdetect.so \
#/usr/lib/libopencv_photo.so \
#/usr/lib/libopencv_shape.so \
#/usr/lib/libopencv_stitching.so \
#/usr/lib/libopencv_superres.so \
#/usr/lib/libopencv_videoio.so \
#/usr/lib/libopencv_video.so \
#/usr/lib/libopencv_videostab.so

INCLUDEPATH += C:/opencv/build/include\
               C:/opencv/build/include/opencv\
               C:/opencv/build/include/opencv2

LIBS +=-L C:/opencv/MinGW_build/lib/libopencv_*.a
