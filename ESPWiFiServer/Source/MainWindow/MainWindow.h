#if !defined(__MAIN_WINDOW_H__)
#define __MAIN_WINDOW_H
#pragma once

#include <QMainWindow>
#include <Server/Server.h>
#include "ui_MainWindow.h"

class CMainWindow final : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    CMainWindow();
    virtual ~CMainWindow() override final = default;

private:
    IServerUniquePtr m_Server;
};

#endif //__MAIN_WINDOW_H__