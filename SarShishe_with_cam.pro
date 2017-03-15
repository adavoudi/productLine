#-------------------------------------------------
#
# Project created by QtCreator 2016-01-25T19:14:55
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    cv_toolbox.cpp \
    sarshishe.cpp \
    camera.cpp \
    hardwarereader.cpp

HEADERS  += mainwindow.h \
    cv_toolbox.h \
    sarshishe.h \
    camera.h \
    hardwarereader.h

FORMS    += mainwindow.ui


LIBS += -L"C:\Users\Alireza\Documents\The Imaging Source Europe GmbH\TIS Grabber DLL\bin\win32" -ltisgrabber
INCLUDEPATH += "C:\Users\Alireza\Documents\The Imaging Source Europe GmbH\TIS Grabber DLL\include"


INCLUDEPATH += D:\opencv2\build\include\

CONFIG(release, debug|release) {

    TARGET = SarShishe_with_cam
    LIBS += -LD:\opencv2\build\x86\vc12\lib \
        -lopencv_core2410 \
        -lopencv_highgui2410 \
        -lopencv_imgproc2410 \
        -lopencv_features2d2410 \
        -lopencv_calib3d2410 \
        -lopencv_objdetect2410 \
        -lopencv_contrib2410 \
        -lopencv_ml2410
}else {

    TARGET = SarShishe_with_cam_D
    LIBS += -LD:\opencv2\build\x86\vc12\lib \
        -lopencv_core2410d \
        -lopencv_highgui2410d \
        -lopencv_imgproc2410d \
        -lopencv_features2d2410d \
        -lopencv_calib3d2410d \
        -lopencv_objdetect2410d \
        -lopencv_contrib2410d \
        -lopencv_ml2410d
}
