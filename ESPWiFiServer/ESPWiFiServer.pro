CONFIG -= flat
TEMPLATE = app
TARGET = ESPWiFiServer
BINDIR = $$OUT_PWD/bin
DESTDIR = $$BINDIR
QT += core gui widgets

CONFIG += precompile_header suppress_vcproj_warnings
QMAKE_CXXFLAGS += /std:c++latest

PRECOMPILED_HEADER = Source/pch.h

INCLUDEPATH += $$PWD/Source

SOURCES += \
    Source/main.cpp                                                                 \     
    Source/MainWindow/MainWindow.cpp                                                \

HEADERS += \
    Source/Pointers.h                                                               \
    Source/MainWindow/MainWindow.h                                                  \
    

FORMS += \
    Source/MainWindow/UI/MainWindow.ui                                              \