#-------------------------------------------------
#
# Project created by QtCreator 2019-07-11T16:43:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = example_2
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

#QMAKE_CXXFLAGS_DEBUG += -gdwarf-3

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH +=	$$PWD/../Clipper/include

DEPENDPATH +=	$$PWD/../Clipper/include

unix:!macx: {
    contains(QT_ARCH, i386) {
	message("32-bit libs not built, you can build it manually")
    } else {
        LIBS +=	-L$$PWD/../Clipper/lib/linux_x86_64/ -lpolyclipping
    }
}

win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += -L$$PWD/../Clipper/lib/win_x86/ -lpolyclipping
    } else {
        LIBS += -L$$PWD/../Clipper/lib/win_x86/ -lpolyclipping
    }
}

