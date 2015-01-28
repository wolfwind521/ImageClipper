#-------------------------------------------------
#
# Project created by QtCreator 2015-01-26T14:44:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageClipping
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageclipper.cpp

HEADERS  += mainwindow.h \
    imageclipper.h

FORMS    += mainwindow.ui

CONFIG(debug, release|debug):DEFINES += _DEBUG

!win32 {
INCLUDEPATH += /usr/local/Cellar/opencv/2.4.9/include
LIBS += -L/usr/local/Cellar/opencv/2.4.9/lib/ \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_highgui \
    -lopencv_features2d
}
