
#-------------------------------------------------
#
# Project created by QtCreator 2018-09-27T22:52:09
#
#-------------------------------------------------

QT += core gui
QT += widgets
QT += network
QT += sql

TARGET = ../project/server
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    database.cpp \
    myserver.cpp \
    loginandsmsfunct.cpp \
    sslserver.cpp

HEADERS += \
    myserver.h \
    database.h \
    loginandsmsfunct.h \
    sslserver.h

LIBS += -lcommon -L$$OUT_PWD/../libs/
INCLUDEPATH += ../common

RESOURCES += \
    cert.qrc
