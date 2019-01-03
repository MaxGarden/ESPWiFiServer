#include "pch.h"
#include "MainWindow.h"
#include "Server/ServerControllerListenerBase.h"
#include "client/ClientControllerListenerBase.h"
#include "Client/Services/DeviceIdService.h"
#include "Client/Services/ClientServiceBase.h"

CMainWindow::CMainWindow() :
    QMainWindow(nullptr)
{
    setupUi(this);
}

bool CMainWindow::Initialize(unsigned short int serverPort)
{
    DEBUG_ASSERT(!m_Initialized);
    if (m_Initialized)
        return false;

    m_Server = IServer::Create();
    DEBUG_ASSERT(m_Server);
    if (!m_Server)
        return false;

    m_ServerController = IServerController::Create();
    DEBUG_ASSERT(m_ServerController);
    if (!m_ServerController)
        return false;

    if (!m_ServerController->SetDataModel(m_Server))
    {
        DEBUG_ASSERT(false);
        return false;
    }

    if (!m_Server->StartListening(serverPort))
    {
        DEBUG_ASSERT(false);
        return false;
    }

    m_BuildersProvider = IClientBuildersProvider::Create();
    DEBUG_ASSERT(m_BuildersProvider);
    if (!m_BuildersProvider)
        return false;

    if (!BuildClientBuildersProvider(*m_BuildersProvider))
    {
        DEBUG_ASSERT(false);
        return false;
    }

    DEBUG_ASSERT(m_ServerViewWidget);
    if (!m_ServerViewWidget)
        return false;

    m_ServerView = IServerView::Create(*m_BuildersProvider, m_ServerViewWidget);
    DEBUG_ASSERT(m_ServerView);
    if (!m_ServerView)
        return false;

    if (!m_ServerView->SetController(m_ServerController))
    {
        DEBUG_ASSERT(false);
        return false;
    }

    if (!RegistersClientsViews(*m_ServerView))
    {
        DEBUG_ASSERT(false);
        return false;
    }

    return (m_Initialized = true);
}

#include "Client/Builders/TransmitterBuilder.h"

bool CMainWindow::BuildClientBuildersProvider(IClientBuildersProvider& buildersProvider)
{
    auto result = true;

    result &= buildersProvider.RegisterBuilder('S', std::make_unique<CTranmitterBuilder>());

    return result;
}

#include "Client/Views/TransmitterView.h"

bool CMainWindow::RegistersClientsViews(IServerView& serverView)
{
    auto result = true;

    result &= serverView.RegisterClientViewFactory('S', std::make_unique<CClientViewFactory<CTransmitterView>>());

    return result;
}