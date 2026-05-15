#-------------------------------------------------
#
# Lane-Detection System created by Lyuwenshan at 2023/5/30
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = camera_test
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += /usr/local/include\
               /usr/local/include/opencv4

LIBS += /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so    \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_imgcodecs.so\
        /usr/lib/aarch64-linux-gnu/libopencv_shape.so.4.2.0\
        /usr/local/lib/libopencv_videoio.so


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        sysinfolinuximpl.cpp

HEADERS += \
        mainwindow.h \
        sysinfolinuximpl.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    res.qrc
