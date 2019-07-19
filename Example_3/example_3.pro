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

QMAKE_CXXFLAGS += -ggdb3

CONFIG += c++11

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

release: DESTDIR = $$PWD/Release
debug:   DESTDIR = $$PWD/Debug

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/qrc
UI_DIR = $$DESTDIR/ui

SOURCES += main.cpp\
        mainwindow.cpp \
	ghost.cpp \
	polygonbody.cpp \
	wall.cpp \
    horizontalsliderstylesheet.cpp

HEADERS  += mainwindow.h \
	ghost.h \
	polygonbody.h \
	wall.h

FORMS    += mainwindow.ui

INCLUDEPATH +=	$$PWD/../Clipper/include \
		$$PWD/../Box2D/include \
		$$PWD/../Poly2Tri/include/poly2tri

DEPENDPATH +=	$$PWD/../Clipper/include \
		$$PWD/../Box2D/include \
		$$PWD/../Poly2Tri/include/poly2tri

unix:!macx: {
    contains(QT_ARCH, i386) {
	message("32-bit libs not built, you can build it manually")
    } else {
	LIBS +=	-L$$PWD/../Clipper/lib/linux_x86_64/ -lpolyclipping \
		-L$$PWD/../Box2D/lib/linux_x86_64/ -lBox2D \
		-L$$PWD/../Poly2Tri/lib/linux_x86_64/ -lpoly2tri
    }
}

win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
	message("32-bit libs not built, you can build it manually")
    } else {
	LIBS += -L$$PWD/../Clipper/lib/win_x86_64/ -lpolyclipping \
		-L$$PWD/../Box2D/lib/win_x86_64/ -lBox2D \
		-L$$PWD/../Poly2Tri/lib/win_x86_64/ -lpoly2tri
    }
}

RESOURCES += \
    res.qrc





