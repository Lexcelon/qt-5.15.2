TARGET = qtquickcontrols2universalstyleplugin
TARGETPATH = QtQuick/Controls.2/Universal

IMPORT_NAME = QtQuick.Controls.Universal
IMPORT_VERSION = 2.$$QT_MINOR_VERSION

QT += qml quick
QT_PRIVATE += core-private gui-private qml-private quick-private quicktemplates2-private quickcontrols2-private

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

include(universal.pri)

OTHER_FILES += \
    qmldir \
    $$QML_FILES

SOURCES += \
    $$PWD/qtquickcontrols2universalstyleplugin.cpp

RESOURCES += \
    $$PWD/qtquickcontrols2universalstyleplugin.qrc

CONFIG += no_cxx_module install_qml_files builtin_resources qtquickcompiler
load(qml_plugin)

requires(qtConfig(quickcontrols2-universal))
