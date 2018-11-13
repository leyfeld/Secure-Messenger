QT += core
QT += network
QT += quick quickcontrols2
QT +=network
QT +=sql
QT += widgets

CONFIG += c++11
TEMPLATE = app
TARGET = ../project/client

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    chatprotocol.cpp\
    qmlconnect.cpp \
    database.cpp \
    myfile.cpp \
    threadsend.cpp \
    cryptoworker.cpp\
    securepassword.cpp
RESOURCES += \
    qml.qrc

RESOURCES += \
    cert.qrc

HEADERS += \
    qmlconnect.h \
    database.h \
    chatprotocol.h \
    myfile.h\
    threadsend.h \
    cryptoworker.h\
    securepassword.h
LIBS += -L$$PWD/../openssl1.0.2p/lib -lssl -lcrypto
INCLUDEPATH += ../openssl1.0.2p/include/
LIBS += -lcommon -L$$OUT_PWD/../libs/
message($$OUT_PWD);
INCLUDEPATH += ../common
