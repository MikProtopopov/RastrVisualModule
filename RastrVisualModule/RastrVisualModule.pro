#-------------------------------------------------
#
# Project created by QtCreator 2015-10-13T10:36:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Diplom
TEMPLATE = app

#QMAKE_LFLAGS += /INCREMENTAL:NO

SOURCES += main.cpp\
    dialog.cpp \
    startwindow.cpp \
    paintrastr.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    helpwindow.cpp

HEADERS  += \
    dialog.h \
    startwindow.h \
    rastrmanipulation.h \
    paintrastr.h \
    mainwindow.h \
    qcustomplot.h \
    helpwindow.h

FORMS    += \
    dialog.ui \
    startwindow.ui \
    mainwindow.ui \
    helpwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-RastrManipulationLibrary-Qt5_5_0_Static-Release/release/ -lRastrManipulationLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-RastrManipulationLibrary-Qt5_5_0_Static-Release/debug/ -lRastrManipulationLibrary
else:unix: LIBS += -L$$PWD/../build-RastrManipulationLibrary-Qt5_5_0_Static-Release/ -lRastrManipulationLibrary

INCLUDEPATH += $$PWD/../RastrManipulationLibrary
DEPENDPATH += $$PWD/../RastrManipulationLibrary
