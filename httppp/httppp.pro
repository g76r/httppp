QT += gui widgets
TEMPLATE = app
TARGET = httppp
CONFIG += largefile c++11

# git version in version.h
linux {
  version.target = version.h
  version.commands = ./version.sh > version.h
  version.depends = FORCE
  QMAKE_EXTRA_TARGETS += version
  PRE_TARGETDEPS += version.h
}

# git version directly as a -D, simpler but requires git on every build environment
#DEFINES += VERSION="\\\"$(shell git describe --always)\\\""

contains(QT_VERSION, ^4\\..*) {
  message("Cannot build with Qt version $${QT_VERSION}.")
  error("Use Qt 5.")
}

QMAKE_CXXFLAGS += -Wextra
win32:QMAKE_RPATHDIR = # no rpath in linking

INCLUDEPATH += ../libqpcap
LIBS += -lqpcap
!win32:LIBS += -lqpcap -L../libqpcap
win32:LIBS += c:/WpdPack/Lib/wpcap.lib
win32:release:LIBS += -L../build-libqpcap-windows/release
win32:debug:LIBS += -L../build-libqpcap-windows/debug

exists(/usr/bin/ccache):QMAKE_CXX = ccache g++
exists(/usr/bin/ccache):QMAKE_CXXFLAGS += -fdiagnostics-color=always
QMAKE_CXXFLAGS += -Wextra -Woverloaded-virtual
unix:debug:QMAKE_CXXFLAGS += -ggdb

linux-g++ {
  UI_DIR = ../httppp-build-linux/ui
  UI_HEADERS_DIR = ../httppp-build-linux/ui/include
  UI_SOURCES_DIR = ../httppp-build-linux/ui/src
  OBJECTS_DIR = ../httppp-build-linux/obj
  RCC_DIR = ../httppp-build-linux/rcc
  MOC_DIR = ../httppp-build-linux/moc
  #QMAKE_CXXFLAGS_DEBUG += -DMONOTHREAD
  #QMAKE_CXXFLAGS_DEBUG += -pg -fno-default-inline -fno-inline
  #QMAKE_LFLAGS_DEBUG += -pg
}

win32 {
  #QMAKE_CXXFLAGS_DEBUG += -DMONOTHREAD
}

SOURCES += \
    gui/httpppmainwindow.cpp \
    gui/tcpconversationmodel.cpp \
    gui/tcppacketmodel.cpp \
    widget/enhancedtreeview.cpp \
    widget/enhancedtableview.cpp \
    gui/httphitmodel.cpp \
    util/csvwriter.cpp \
    widget/multipixmapbutton.cpp \
    gui/main-gui.cpp \
    common/httpcustomfieldanalyzer.cpp \
    gui/tcpdata.cpp \
    gui/httpdata.cpp

HEADERS += \
    gui/httpppmainwindow.h \
    gui/tcpconversationmodel.h \
    gui/tcppacketmodel.h \
    widget/enhancedtreeview.h \
    widget/enhancedtableview.h \
    gui/httphitmodel.h \
    util/csvwriter.h \
    widget/multipixmapbutton.h \
    global.h \
    common/httpcustomfieldanalyzer.h \
    gui/tcpdata.h \
    gui/httpdata.h

FORMS += \
    gui/httpppmainwindow.ui

RESOURCES += \
    icons.qrc

# Windows application icon
RC_FILE += \
    httppp.rc

