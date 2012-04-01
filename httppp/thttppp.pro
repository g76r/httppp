TEMPLATE = app
CONFIG -= console
TARGET = thttppp
CONFIG += largefile

QMAKE_CXXFLAGS += -Wextra
#QMAKE_RPATHDIR = # no rpath in linking

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

SOURCES += tui/main.cpp \
    tui/printer.cpp

HEADERS += \
    tui/printer.h
