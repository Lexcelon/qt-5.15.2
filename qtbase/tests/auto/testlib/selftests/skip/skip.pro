SOURCES += tst_skip.cpp
QT = core testlib

mac:CONFIG -= app_bundle
CONFIG -= debug_and_release_target

TARGET = skip

include($$QT_SOURCE_TREE/src/testlib/selfcover.pri)
