QT       += core
#QT       -= gui
QT += network
LIBS += -lpcap

TARGET = httppp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

linux-g++ {
  UI_DIR = ../httppp-build-linux/ui
  UI_HEADERS_DIR = ../httppp-build-linux/ui/include
  UI_SOURCES_DIR = ../httppp-build-linux/ui/src
  OBJECTS_DIR = ../httppp-build-linux/obj
  RCC_DIR = ../httppp-build-linux/rcc
  MOC_DIR = ../httppp-build-linux/moc
}
macx {
  UI_DIR = ../httppp-build-macos/ui
  UI_HEADERS_DIR = ../httppp-build-macos/ui/include
  UI_SOURCES_DIR = ../httppp-build-macos/ui/src
  OBJECTS_DIR = ../httppp-build-macos/obj
  RCC_DIR = ../httppp-build-macos/rcc
  MOC_DIR = ../httppp-build-macos/moc
}
QMAKE_CXXFLAGS += -Wextra
#QMAKE_RPATHDIR = # no rpath in linking

SOURCES += main.cpp \
    pcapengine.cpp \
    pcapthread.cpp \
    pcaplayer1packet.cpp \
    printer.cpp \
    pcapethernetpacket.cpp \
    pcapipv4packet.cpp \
    pcapipv4stack.cpp \
    pcaplayer2packet.cpp \
    pcaplayer3packet.cpp \
    pcapethernetstack.cpp \
    pcaptcpstack.cpp \
    pcaptcpconversation.cpp \
    pcaptcppacket.cpp \
    httpppmainwindow.cpp \
    tcpconversationmodel.cpp \
    tcppacketmodel.cpp \
    httprequestsmodel.cpp \
    widget/enhancedtreeview.cpp \
    widget/enhancedtableview.cpp

win32:INCLUDEPATH += win32_include

HEADERS += \
    pcapengine.h \
    pcapthread.h \
    pcaplayer1packet.h \
    printer.h \
    pcapethernetpacket.h \
    pcapipv4packet.h \
    pcapipv4stack.h \
    pcaplayer2packet.h \
    pcaplayer3packet.h \
    pcapethernetstack.h \
    pcaptcpstack.h \
    pcaptcpconversation.h \
    pcaptcppacket.h \
    httpppmainwindow.h \
    tcpconversationmodel.h \
    tcppacketmodel.h \
    httprequestsmodel.h \
    widget/enhancedtreeview.h \
    widget/enhancedtableview.h

FORMS += \
    httpppmainwindow.ui
