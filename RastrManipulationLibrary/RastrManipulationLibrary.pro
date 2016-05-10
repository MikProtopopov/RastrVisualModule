#-------------------------------------------------
#
# Project created by QtCreator 2016-05-10T17:35:29
#
#-------------------------------------------------

QT       -= gui

TARGET = RastrManipulationLibrary
TEMPLATE = lib

DEFINES += RASTRMANIPULATIONLIBRARY_LIBRARY

SOURCES += rastrmanipulationlibrary.cpp \
    rastrmanipulation.cpp

HEADERS += rastrmanipulationlibrary.h\
        rastrmanipulationlibrary_global.h \
    rastrmanipulation.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
