TARGET = tst_qdeclarativevideooutput_window

QT += multimedia-private qml testlib quick qtmultimediaquicktools-private
CONFIG += testcase

RESOURCES += qml.qrc

SOURCES += \
        tst_qdeclarativevideooutput_window.cpp

INCLUDEPATH += ../../../../src/imports/multimedia
