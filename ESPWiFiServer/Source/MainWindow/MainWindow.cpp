#include "pch.h"
#include "MainWindow.h"
#include "Server/ServerControllerListenerBase.h"
#include "client/ClientControllerListenerBase.h"
#include "Client/Services/DeviceIdService.h"
#include "Client/Services/ClientServiceBase.h"

class CMainWindowServerListener final : public CServerControllerListenerBase
{
public:
    CMainWindowServerListener(const IClientBuildersProvider& buildersProvider);
    virtual ~CMainWindowServerListener() override final = default;

    virtual void OnClientConnected(const IClientControllerSharedPtr& client) override final;

private:
    const IClientBuildersProvider& m_BuildersProvider;
};

class CMainWindowClientListener final : public CClientControllerListenerBase
{
    using UnregisterCallbackType = std::function<void()>;
public:
    CMainWindowClientListener() = default;
    virtual ~CMainWindowClientListener() override final = default;

    void SetUnregisterCallback(UnregisterCallbackType&& unregisterCallback);

    virtual void OnServicePaired(IClientService& service);

private:
    UnregisterCallbackType m_UnregisterCallback;
};

CMainWindowServerListener::CMainWindowServerListener(const IClientBuildersProvider& buildersProvider) :
    m_BuildersProvider(buildersProvider)
{
}

void CMainWindowServerListener::OnClientConnected(const IClientControllerSharedPtr& client)
{
    DEBUG_ASSERT(client);
    if (!client)
        return;

    if (!client->RegisterServiceFactory(std::make_unique<CDeviceIdServiceFactory>(*client, m_BuildersProvider)))
    {
        DEBUG_ASSERT(false);
        return; //TODO add logging
    }

    auto clientListener = std::make_unique<CMainWindowClientListener>();
    auto& clientRefListener = *clientListener;

    const auto listenerId = client->RegisterListener(std::move(clientListener));
    DEBUG_ASSERT(!listenerId);
    if (!listenerId)
        return; //TODO add logging

    clientRefListener.SetUnregisterCallback([listenerId = *listenerId, client]()
    {
        if (!client->UnregisterListener(listenerId))
            DEBUG_ASSERT(false);
    });

    client->PairServices(); //TODO, do not assume happy path
}

void CMainWindowClientListener::SetUnregisterCallback(UnregisterCallbackType&& unregisterCallback)
{
    m_UnregisterCallback = std::move(unregisterCallback);
}

void CMainWindowClientListener::OnServicePaired(IClientService& service)
{
    const auto deviceIdService = dynamic_cast<CDeviceIdService*>(&service);
    DEBUG_ASSERT(deviceIdService);
    if (!deviceIdService)
        return;

    DEBUG_ASSERT(m_UnregisterCallback);
    if (m_UnregisterCallback)
        m_UnregisterCallback();
}

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

    if (!m_ServerController->RegisterListener(std::make_unique<CMainWindowServerListener>(*m_BuildersProvider)))
    {
        DEBUG_ASSERT(false);
        return false;
    }

    return (m_Initialized = true);
}
