#include "pch.h"
#include "ServerController.h"
#include "ControllerBase.h"
#include "ServerListenerBase.h"
#include "Client/ClientController.h"

class CServerController;
class CServerControllerListener final : public CServerListenerBase
{
public:
    CServerControllerListener(CServerController& serverController);
    virtual ~CServerControllerListener() override = default;

    virtual void OnClientConnected(const IClientSharedPtr& client) override final;
    virtual void OnClientDisconnected(const IClientSharedPtr& client) override final;

private:
    CServerController& m_ServerController;
};

class CServerController final : public CControllerBase<IServerController, IServer, IServerControllerListener>
{
public:
    CServerController() = default;
    virtual ~CServerController() override = default;

    virtual void VisitClients(const ConstVisitorType<IClientControllerSharedPtr>& visitor) override final;

    void OnClientConnected(const IClientSharedPtr& client);
    void OnClientDisconnected(const IClientSharedPtr& client);

protected:
    virtual void OnDataModelChanged() override final;
    virtual IListenerUniquePtr CreateListener() override final;

private:
    std::map<IClientSharedPtr, IClientControllerSharedPtr> m_Clients;
};

CServerControllerListener::CServerControllerListener(CServerController& serverController) :
    m_ServerController(serverController)
{
}

void CServerController::VisitClients(const ConstVisitorType<IClientControllerSharedPtr>& visitor)
{
    VisitObjectsMap(m_Clients, visitor);
}

void CServerControllerListener::OnClientConnected(const IClientSharedPtr& client)
{
    m_ServerController.OnClientConnected(client);
}

void CServerControllerListener::OnClientDisconnected(const IClientSharedPtr& client)
{
    m_ServerController.OnClientDisconnected(client);
}

void CServerController::OnClientConnected(const IClientSharedPtr& client)
{
    const auto clientController = IClientControllerSharedPtr{ IClientController::Create() };
    DEBUG_ASSERT(clientController);
    if (!clientController)
        return;

    if (!clientController->SetDataModel(client))
    {
        DEBUG_ASSERT(false);
        return;
    }

    m_Clients.emplace(client, clientController);
    NotifyListeners(&IServerControllerListener::OnClientConnected, clientController);
}

void CServerController::OnClientDisconnected(const IClientSharedPtr& client)
{
    const auto iterator = m_Clients.find(client);
    DEBUG_ASSERT(iterator != m_Clients.end());
    if (iterator == m_Clients.end())
        return;

    const auto& clientController = iterator->second;
    clientController->SetDataModel(nullptr);

    NotifyListeners(&IServerControllerListener::OnClientDisconnected, clientController);
    m_Clients.erase(iterator);
}

void CServerController::OnDataModelChanged()
{
    while (!m_Clients.empty())
    {
        const auto iterator = m_Clients.begin();
        if (!iterator->first || !iterator->second)
            m_Clients.erase(iterator);
        else
            OnClientDisconnected(iterator->first);
    }

    const auto& dataModel = GetDataModel();
    if (!dataModel)
        return;

    dataModel->VisitClients([this](const auto& client)
    {
        OnClientConnected(client);
    });
}

IListenerUniquePtr CServerController::CreateListener()
{
    return std::make_unique<CServerControllerListener>(*this);
}

IServerControllerUniquePtr IServerController::Create()
{
    return std::make_unique<CServerController>();
}