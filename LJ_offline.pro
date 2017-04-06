#-------------------------------------------------
#
# Project created by QtCreator 2017-04-06T11:51:12
#
#-------------------------------------------------

QT       += core gui widgets network
CONFIG   += c++11

TARGET = LJ_offline
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

DISTFILES += \
    web/processing.js \
    web/style.css \
    web/index.html
