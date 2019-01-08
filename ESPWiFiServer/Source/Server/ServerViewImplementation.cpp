#include "pch.h"
#include "ServerViewImplementation.h"
#include "ServerControllerListenerBase.h"
#include "Client/ClientPairView.h"

class CServerView;
class CServerViewListener final : public CServerControllerListenerBase
{
public:
    CServerViewListener(CServerViewImplementation& serverView);
    virtual ~CServerViewListener() override final = default;

    virtual void OnClientConnected(const IClientControllerSharedPtr& client) override final;
    virtual void OnClientDisconnected(const IClientControllerSharedPtr& client) override final;

private:
    CServerViewImplementation& m_ServerView;
};

CServerViewListener::CServerViewListener(CServerViewImplementation& serverView) :
    m_ServerView(serverView)
{
}

void CServerViewListener::OnClientConnected(const IClientControllerSharedPtr& client)
{
    m_ServerView.OnClientConnected(client);
}

void CServerViewListener::OnClientDisconnected(const IClientControllerSharedPtr& client)
{
    m_ServerView.OnClientDisconnected(client);
}

CServerViewImplementation::CServerViewImplementation(const IClientBuildersProvider& buildersProvider, QWidget* parent /*= nullptr*/) :
    m_BuildersProvider(buildersProvider),
    QWidget(parent)
{
    setupUi(this);
}

bool CServerViewImplementation::RegisterClientViewFactory(byte deviceId, IClientViewFactoryUniquePtr&& factory)
{
    const auto iterator = m_ClientsFactories.find(deviceId);
    DEBUG_ASSERT(iterator == m_ClientsFactories.cend());
    if (iterator != m_ClientsFactories.cend())
        return false;

    m_ClientsFactories.emplace(deviceId, std::move(factory));
    return true;
}

bool CServerViewImplementation::UnregisterClientViewFactory(byte deviceId)
{
    return m_ClientsFactories.erase(deviceId) == 1;
}

void CServerViewImplementation::OnClientConnected(const IClientControllerSharedPtr& clientController)
{
    const auto fail = [&clientController]()
    {
        DEBUG_ASSERT(false);
        const auto result = clientController->Disconnect();
        DEBUG_ASSERT(result);
    };

    auto clientPairView = IClientPairView::Create(m_BuildersProvider, this);
    if (!clientPairView)
        return fail();

    if (!clientPairView->SetController(clientController))
        return fail();

    const auto clientPairViewRaw = clientPairView.get();
    AddClientViewTab(clientController, std::move(clientPairView));

    clientPairViewRaw->ResolveDeviceType([this, clientPairViewRaw](const auto& deviceId)
    {
        OnResolvedDeviceType(*clientPairViewRaw, deviceId);
    });
}

void CServerViewImplementation::OnClientDisconnected(const IClientControllerSharedPtr& clientController)
{
    const auto iterator = m_Clients.find(clientController);
    DEBUG_ASSERT(iterator != m_Clients.end());
    if (iterator == m_Clients.end())
        return;

    const auto& clientView = iterator->second;
    clientView->SetController(nullptr);

    DEBUG_ASSERT(m_ClientsViewTabWidget);
    if (m_ClientsViewTabWidget)
    {
        const auto viewWidget = clientView->GetViewWidget();
        DEBUG_ASSERT(viewWidget);

        if (viewWidget)
            m_ClientsViewTabWidget->removeTab(m_ClientsViewTabWidget->indexOf(viewWidget));
    }

    m_Clients.erase(iterator);
}

void CServerViewImplementation::OnControllerChanged()
{
    while (!m_Clients.empty())
    {
        const auto iterator = m_Clients.begin();
        if (!iterator->first || !iterator->second)
            m_Clients.erase(iterator);
        else
            OnClientDisconnected(iterator->first);
    }

    const auto& controller = GetController();
    if (!controller)
        return;

    controller->VisitClients([this](const auto& client)
    {
        OnClientConnected(client);
    });
}

IListenerUniquePtr CServerViewImplementation::CreateListener()
{
    return std::make_unique<CServerViewListener>(*this);
}

void CServerViewImplementation::OnResolvedDeviceType(const IClientPairView& clientPairView, const std::optional<byte>& deviceId)
{
    const auto iterator = std::find_if(m_Clients.cbegin(), m_Clients.cend(), [&clientPairView](const auto& pair)
    {
        return pair.second.get() == &clientPairView;
    });

    DEBUG_ASSERT(iterator != m_Clients.cend());
    if (iterator == m_Clients.cend())
        return;

    if (deviceId == std::nullopt)
    {
        const auto result = iterator->first->Disconnect();
        DEBUG_ASSERT(result);
    }
    else
        CreateClientView(iterator->first, *deviceId);
}

void CServerViewImplementation::CreateClientView(IClientControllerSharedPtr clientController, byte deviceId)
{
    DEBUG_ASSERT(clientController);
    if (!clientController)
        return;

    OnClientDisconnected(clientController);

    const auto fail = [&clientController]()
    {
        DEBUG_ASSERT(false);
        const auto result = clientController->Disconnect();
        DEBUG_ASSERT(result);
    };

    const auto iterator = m_ClientsFactories.find(deviceId);
    if (iterator == m_ClientsFactories.cend())
        return fail();

    const auto& factory = iterator->second;
    if (!factory)
        return fail();

    auto clientView = factory->Create();
    if (!clientView)
        return fail();

    if (!clientView->SetController(clientController))
        return fail();

    AddClientViewTab(clientController, std::move(clientView));
}

void CServerViewImplementation::AddClientViewTab(const IClientControllerSharedPtr& clientController, IClientViewUniquePtr&& clientView)
{
    DEBUG_ASSERT(clientController);
    if (!clientController)
        return;

    DEBUG_ASSERT(clientView);
    if (!clientView)
        return;

    const auto clientViewWidget = clientView->GetViewWidget();
    DEBUG_ASSERT(clientViewWidget);

    const auto clientTabName = QString::fromStdString(clientView->GetName());

    m_Clients.emplace(clientController, std::move(clientView));
    DEBUG_ASSERT(m_ClientsViewTabWidget);
    if (m_ClientsViewTabWidget && clientViewWidget)
        m_ClientsViewTabWidget->addTab(clientViewWidget, clientTabName);

}

IServerViewUniquePtr IServerView::Create(const IClientBuildersProvider& buildersProvider, QWidget* parent /* = nullptr */)
{
    return std::make_unique<CServerViewImplementation>(buildersProvider, parent);
}