TEMPLATE = app

TARGET = tst_qmouseevent

QT += 3dcore 3dcore-private 3dinput 3dinput-private testlib

CONFIG += testcase

SOURCES += tst_qmouseevent.cpp

include(../commons/commons.pri)
