QT += core network
QT -= gui

CONFIG += c++11

TARGET = AGNSSAgentServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    agnssagentserver.cpp \
    casicAgnssAidIni.c

HEADERS += \
    agnssagentserver.h \
    casicAgnssAidIni.h

DISTFILES += \
    README-old.txt \
    README.txt
