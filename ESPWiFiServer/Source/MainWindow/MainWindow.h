#if !defined(__MAIN_WINDOW_H__)
#define __MAIN_WINDOW_H
#pragma once

#include <QMainWindow>
#include "Server/Server.h"
#include "Server/ServerController.h"
#include "Server/ServerView.h"
#include "Client/ClientBuilder.h"
#include "ui_MainWindow.h"

class CMainWindow final : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    CMainWindow();
    virtual ~CMainWindow() override final = default;

    bool Initialize(unsigned short int serverPort);

private:
    IServerSharedPtr m_Server;
    IServerControllerSharedPtr m_ServerController;
    IServerViewUniquePtr m_ServerView;

    IClientBuildersProviderUniquePtr m_BuildersProvider;

    bool m_Initialized = false;
};

#endif //__MAIN_WINDOW_H__