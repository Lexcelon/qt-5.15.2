TEMPLATE = app
QT       += remoteobjects core testlib
QT       -= gui

TARGET = restart_server
DESTDIR = ./
CONFIG   += c++11
CONFIG   -= app_bundle

REPC_SOURCE = $$PWD/../subclass.rep

SOURCES += main.cpp \
    mytestserver.cpp

HEADERS += \
    mytestserver.h

INCLUDEPATH += $$PWD
