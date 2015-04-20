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
    imageclipper.cpp \
    textimagecreator.cpp \
    ruleeditor.cpp

HEADERS  += mainwindow.h \
    imageclipper.h \
    textimagecreator.h \
    ruleeditor.h

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

win32{
INCLUDEPATH += E:/Tools/opencv/build/include

CONFIG(debug):
{
LIBS += -LE:/Tools/opencv/build/x86/vc12/bin
LIBS += -LE:/Tools/opencv/build/x86/vc12/lib \
    -lopencv_core248d \
    -lopencv_imgproc248d \
    -lopencv_highgui248d \
    -lopencv_features2d248d
}

CONFIG(release):
{
LIBS += -LE:/Tools/opencv/build/x86/vc12/bin
LIBS += -LE:/Tools/opencv/build/x86/vc12/lib \
    -lopencv_core248 \
    -lopencv_imgproc248 \
    -lopencv_highgui248 \
    -lopencv_features2d248
}
}

RESOURCES += \
    ImageClipping.qrc \
    ImageClipping.qrc
