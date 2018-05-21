QT += core
QT -= gui
QT += network

TARGET = smartcar-vision
CONFIG += console
CONFIG -= app_bundle

CONFIG += C++11

TEMPLATE = app

SOURCES += main.cpp \
    dector.cpp \
    posix_qextserialport.cpp \
    qextserialbase.cpp \
    serial.cpp \
    socketclient.cpp

HEADERS += \
    dector.h \
    posix_qextserialport.h \
    qextserialbase.h \
    serial.h \
    socketclient.h

INCLUDEPATH += /usr/local/include \
/usr/local/include/opencv \
/usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_*.so \
