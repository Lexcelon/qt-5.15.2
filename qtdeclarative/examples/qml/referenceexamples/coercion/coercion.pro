QT = core qml

CONFIG += qmltypes
QML_IMPORT_NAME = People
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += main.cpp \
           person.cpp \
           birthdayparty.cpp
HEADERS += person.h \
           birthdayparty.h
RESOURCES += coercion.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/qml/referenceexamples/coercion
INSTALLS += target
