TARGET   = Qt3DExtras
MODULE   = 3dextras
QT      += core-private 3dcore 3dcore-private 3drender 3drender-private 3dinput 3dlogic

DEFINES += QT3DEXTRAS_LIBRARY
DEFINES += BUILD_QT3D_MODULE

load(qt_module)

include (geometries/geometries.pri)
include (3dtext/3dtext.pri)
include (defaults/defaults.pri)
include (text/text.pri)

HEADERS += \
    qt3dextras_global.h

RESOURCES += $$PWD/extras.qrc

