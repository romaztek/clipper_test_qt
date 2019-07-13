#-------------------------------------------------
#
# Project created by QtCreator 2019-07-11T16:43:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = example_3
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
	ghost.cpp \
	polygonbody.cpp \
	wall.cpp

HEADERS  += mainwindow.h \
	ghost.h \
	polygonbody.h \
	wall.h

FORMS    += mainwindow.ui

unix:!macx: LIBS += -L$$PWD/../Clipper/ -lpolyclipping

INCLUDEPATH += $$PWD/../Clipper
DEPENDPATH += $$PWD/../Clipper



unix:!macx: LIBS += -L$$PWD/../Box2D/ -lBox2D

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../

unix:!macx: PRE_TARGETDEPS += $$PWD/../Box2D/libBox2D.a

