CONFIG -= flat
TEMPLATE = app
TARGET = ESPWiFiServer
BINDIR = $$OUT_PWD/bin
DESTDIR = $$BINDIR
QT += core gui widgets network

CONFIG += precompile_header suppress_vcproj_warnings
QMAKE_CXXFLAGS += /std:c++latest

PRECOMPILED_HEADER = Source/pch.h

INCLUDEPATH += $$PWD/Source $$PWD/Source/ESPWiFiServer

SOURCES += \
    Source/main.cpp                                                                 \
    Source/ESPWiFiServer/Listener.cpp                                               \
    Source/ESPWiFiServer/DataModel.cpp                                              \
    Source/ESPWiFiServer/Controller.cpp                                             \
    Source/ESPWiFiServer/View.cpp                                                   \
    Source/ESPWiFiServer/Utilities.cpp                                              \
    Source/MainWindow/MainWindow.cpp                                                \
    Source/Server/Server.cpp                                                        \
    Source/Client/Client.cpp                                                        \
    Source/Client/ClientListenerBase.cpp                                            \
    Source/Client/ClientImplementation.cpp                                          \
    Source/Client/ClientController.cpp                                              \
    Source/Client/ClientView.cpp                                                    \
    Source/Client/ClientService.cpp                                                 \

HEADERS += \
    Source/Pointers.h                                                               \
    Source/ESPWiFiServer/Listener.h                                                 \
    Source/ESPWiFiServer/ListenableBase.h                                           \
    Source/ESPWiFiServer/ListenableBase.inl                                         \
    Source/ESPWiFiServer/DataModel.h                                                \
    Source/ESPWiFiServer/Controller.h                                               \
    Source/ESPWiFiServer/ControllerBase.h                                           \
    Source/ESPWiFiServer/ControllerBase.inl                                         \
    Source/ESPWiFiServer/View.h                                                     \
    Source/ESPWiFiServer/ViewBase.h                                                 \
    Source/ESPWiFiServer/ViewBase.inl                                               \
    Source/ESPWiFiServer/Utilities.h                                                \
    Source/MainWindow/MainWindow.h                                                  \
    Source/Server/Server.h                                                          \
    Source/Client/Client.h                                                          \
    Source/Client/ClientListenerBase.h                                              \
    Source/Client/ClientImplementation.h                                            \
    Source/Client/ClientController.h                                                \
    Source/Client/ClientView.h                                                      \
    Source/Client/ClientService.h                                                   \
    

FORMS += \
    Source/MainWindow/UI/MainWindow.ui                                              \