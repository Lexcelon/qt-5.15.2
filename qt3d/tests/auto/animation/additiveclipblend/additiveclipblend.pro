TEMPLATE = app

TARGET = tst_additiveclipblend

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += \
    tst_additiveclipblend.cpp

include(../../core/common/common.pri)
