TEMPLATE = app
TARGET = httppp
CONFIG += largefile

QMAKE_CXXFLAGS += -Wextra
win32:QMAKE_RPATHDIR = # no rpath in linking

INCLUDEPATH += ../libqpcap
win32:LIBS += c:/WpdPack/Lib/wpcap.lib
!win32:LIBS += -lpcap -lqpcap -L../libqpcap
win32:CONFIG(release, declarative_debug|debug|release):LIBS += ../libqpcap-build-windows/release/qpcap.dll
win32:!CONFIG(release, declarative_debug|debug|release):LIBS += ../libqpcap-build-windows/debug/qpcap.dll

linux-g++ {
  UI_DIR = ../httppp-build-linux/ui
  UI_HEADERS_DIR = ../httppp-build-linux/ui/include
  UI_SOURCES_DIR = ../httppp-build-linux/ui/src
  OBJECTS_DIR = ../httppp-build-linux/obj
  RCC_DIR = ../httppp-build-linux/rcc
  MOC_DIR = ../httppp-build-linux/moc
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
    gui/main-gui.cpp

HEADERS += \
    gui/httpppmainwindow.h \
    gui/tcpconversationmodel.h \
    gui/tcppacketmodel.h \
    widget/enhancedtreeview.h \
    widget/enhancedtableview.h \
    gui/httphitmodel.h \
    util/csvwriter.h \
    widget/multipixmapbutton.h \
    global.h

FORMS += \
    gui/httpppmainwindow.ui

RESOURCES += \
    icons.qrc

# Windows application icon
RC_FILE += \
    httppp.rc

