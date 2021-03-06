TEMPLATE = app
TARGET = qmlvideofx

QT += quick multimedia
android: qtHaveModule(androidextras) {
    QT += androidextras
    DEFINES += REQUEST_PERMISSIONS_ON_ANDROID
}

SOURCES += filereader.cpp main.cpp
HEADERS += filereader.h trace.h

RESOURCES += qmlvideofx.qrc

include($$PWD/../snippets/performancemonitor/performancemonitordeclarative.pri)

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/video/qmlvideofx
INSTALLS += target

QMAKE_INFO_PLIST = Info.plist

EXAMPLE_FILES += \
    qmlvideofx.png \
    qmlvideofx.svg
