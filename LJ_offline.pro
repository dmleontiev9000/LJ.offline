#-------------------------------------------------
#
# Project created by QtCreator 2017-04-06T11:51:12
#
#-------------------------------------------------

QT       += core gui widgets network websockets sql
CONFIG   += c++11

TARGET = LJ_offline
TEMPLATE = app


SOURCES += main.cpp\
    engine.cpp \
    socketwrapper.cpp \
    dialog.cpp \
    filebackend.cpp \
    dbbackend.cpp \
    backend.cpp

HEADERS  += \
    engine.h \
    socketwrapper.h \
    dialog.h \
    filebackend.h \
    dbbackend.h \
    backend.h

DISTFILES += \
    web/processing.js \
    web/style.css \
    web/index.html

RESOURCES += \
    resources.qrc
