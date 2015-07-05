#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T12:57:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = checkitgui
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
    checkit.c \
    vfatattr.c \
    ntfsattr.c \
    crc64.c \
    mainwindow.cpp \
    checkit_attr.c \
    errorlog.cpp

HEADERS  += \
    crc64.h \
    checkit.h \
    mainwindow.h \
    fsmagic.h \
    checkit_attr.h \
    errorlog.h \
    checkitgui.h

FORMS    += \
    mainwindow.ui \
    about.ui \
    manual.ui \
    errorlog.ui

RESOURCES += \
    graphics.qrc

DISTFILES += \
    checkitgui.spec
