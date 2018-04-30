#-------------------------------------------------
#
# Project created by QtCreator 2017-07-14T23:30:31
#
#-------------------------------------------------

QT       += core gui
QT += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Arrow_ellipse
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += windowsOS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    node.cpp

RESOURCES = application.qrc

HEADERS += \
        mainwindow.h \
    ../notepad/node.h \
    node.h

FORMS += \
        mainwindow.ui
HOMEDIR = $$(HOME)
include($$HOMEDIR/qextserialport/src/qextserialport.pri)
#include(C:\Users\pramod\Desktop\group3\astra\QT\qextserialport-master\src\qextserialport.pri)
