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
    Source/Server/ServerListenerBase.cpp                                            \
    Source/Server/ServerImplementation.cpp                                          \
    Source/Server/ServerController.cpp                                              \
    Source/Server/ServerControllerListenerBase.cpp                                  \
    Source/Server/ServerView.cpp                                                    \
    Source/Server/ServerViewImplementation.cpp                                      \
    Source/Client/Client.cpp                                                        \
    Source/Client/ClientListenerBase.cpp                                            \
    Source/Client/ClientImplementation.cpp                                          \
    Source/Client/ClientController.cpp                                              \
    Source/Client/ClientControllerListenerBase.cpp                                  \
    Source/Client/ClientView.cpp                                                    \
    Source/Client/ClientPairView.cpp                                                \
    Source/Client/ClientBuilder.cpp                                                 \
    Source/Client/ClientService.cpp                                                 \
    Source/Client/Builders/ClientBuilderBase.cpp                                    \
    Source/Client/Builders/TransmitterBuilder.cpp                                   \
    Source/Client/Views/ClientViewBase.cpp                                          \
    Source/Client/Views/TransmitterView.cpp                                         \
    Source/Client/Services/ClientServiceBase.cpp                                    \      
    Source/Client/Services/DeviceIdService.cpp                                      \      
    Source/Client/Services/BinaryTransmissionService.cpp                            \      
    Source/Client/Services/MorseCodeTransmissionService.cpp                         \

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
    Source/Server/ServerListenerBase.h                                              \
    Source/Server/ServerImplementation.h                                            \
    Source/Server/ServerController.h                                                \
    Source/Server/ServerControllerListenerBase.h                                    \
    Source/Server/ServerView.h                                                      \
    Source/Server/ServerViewImplementation.h                                        \
    Source/Client/Client.h                                                          \
    Source/Client/ClientListenerBase.h                                              \
    Source/Client/ClientImplementation.h                                            \
    Source/Client/ClientController.h                                                \
    Source/Client/ClientControllerListenerBase.h                                    \
    Source/Client/ClientView.h                                                      \
    Source/Client/ClientPairView.h                                                  \
    Source/Client/ClientBuilder.h                                                   \
    Source/Client/ClientService.h                                                   \
    Source/Client/Builders/ClientBuilderBase.h                                      \
    Source/Client/Builders/TransmitterBuilder.h                                     \
    Source/Client/Views/ClientViewBase.h                                            \
    Source/Client/Views/TransmitterView.h                                           \
    Source/Client/Services/ClientServiceBase.h                                      \      
    Source/Client/Services/DeviceIdService.h                                        \      
    Source/Client/Services/BinaryTransmissionService.h                              \      
    Source/Client/Services/MorseCodeTransmissionService.h                           \
    

FORMS += \
    Source/MainWindow/UI/MainWindow.ui                                              \
    Source/Server/UI/ServerView.ui                                                  \
    Source/Client/Views/UI/TransmitterView.ui                                       \
    
CopyData.commands = $$quote(cmd /c xcopy /Y /S /I Data $${BINDIR})

QMAKE_EXTRA_TARGETS += CopyData
POST_TARGETDEPS += CopyData