#-------------------------------------------------
#
# Project created by QtCreator 2014-12-05T22:27:44
#
#-------------------------------------------------

#Qt Version 5.3.2

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NFC
TEMPLATE = app


SOURCES += main.cpp\
        mainprocess.cpp

HEADERS  += mainprocess.h

FORMS    += mainprocess.ui

message(Qt version: $$[QT_VERSION])

include(./pn532/pn532.pri)
include(./general_view/general_view.pri)
include(./view/view.pri)
