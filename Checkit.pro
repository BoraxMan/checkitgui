#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T12:57:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Checkit
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x


SOURCES += main.cpp\
    checkit.c \
    vfatattr.c \
    ntfsattr.c \
    crc64.c \
    mainwindow.cpp

HEADERS  += \
    crc64.h \
    checkit.h \
    mainwindow.h \
    fsmagic.h

FORMS    += \
    mainwindow.ui \
    about.ui

RESOURCES += \
    graphics.qrc
