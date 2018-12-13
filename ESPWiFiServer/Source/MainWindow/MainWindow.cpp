#include "pch.h"
#include "MainWindow.h"

CMainWindow::CMainWindow() :
    QMainWindow(nullptr),
    m_Server(IServer::Create())
{
    setupUi(this);
    DEBUG_ASSERT(m_Server->StartListening(1234));
}