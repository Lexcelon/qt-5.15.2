CONFIG += testcase
TARGET = tst_qlockfile
SOURCES += tst_qlockfile.cpp

QT = core-private testlib concurrent
win32:!winrt: QMAKE_USE += advapi32
