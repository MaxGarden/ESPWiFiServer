#include "pch.h"
#include "ServerImplementation.h"
#include "Client/ClientListenerBase.h"
#include <QTimer>

class CServerClientListener final : public CClientListenerBase
{
public:
    CServerClientListener(CServerImplementation& server);
    virtual ~CServerClientListener() override final = default;

    virtual void OnDisconnected() override final;

private:
    CServerImplementation& m_Server;
};

CServerClientListener::CServerClientListener(CServerImplementation& server) :
    m_Server(server)
{
}

void CServerClientListener::OnDisconnected()
{
    m_Server.RefreshClientsStates();
}

CServerImplementation::CServerImplementation(QObject* parent /* = nullptr */) :
    QObject(parent),
    m_Server(this)
{
    Setup();

    const auto refreshTimer = new QTimer(this);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(OnRefreshTimer()));
    refreshTimer->start(500);
}

CServerImplementation::~CServerImplementation()
{
    if (m_Server.isListening())
        m_Server.close();
}

bool CServerImplementation::StartListening(unsigned short int port)
{
    DEBUG_ASSERT(!m_Server.isListening());
    if (m_Server.isListening())
        return false;

    return m_Server.listen(QHostAddress::AnyIPv4, port);
}

void CServerImplementation::VisitClients(const ConstVisitorType<IClientSharedPtr>& visitor)
{
    VisitObjectsContainer(m_Clients, visitor);
}

void CServerImplementation::RefreshClientsStates()
{
    const auto iterator = std::remove_if(m_Clients.begin(), m_Clients.end(), [](const auto& client)
    {
        return !client || !client->IsConnected();
    });

    for (auto current = iterator; current != m_Clients.cend(); ++current)
    {
        if (const auto& client = *current)
        {
            NotifyListeners(&IServerListener::OnClientDisconnected, client);
            m_ClientsToErase.emplace_back(client);
        }
    }

    m_Clients.erase(iterator, m_Clients.end());
}

void CServerImplementation::Setup()
{
    connect(&m_Server, &QTcpServer::newConnection, this, &CServerImplementation::OnNewConnection);
}

void CServerImplementation::OnNewConnection()
{
    const auto clientSocket = m_Server.nextPendingConnection();
    DEBUG_ASSERT(clientSocket);
    if (!clientSocket)
        return;

    auto client = IClient::Create();
    DEBUG_ASSERT(client);
    if (!client)
        return;

    if (!client->RegisterListener(std::make_unique<CServerClientListener>(*this)))
    {
        DEBUG_ASSERT(false);
        return;
    }

    client->SetSocket(clientSocket);

    m_Clients.emplace_back(std::move(client));
    NotifyListeners(&IServerListener::OnClientConnected, m_Clients.back());
}

void CServerImplementation::OnRefreshTimer()
{
    RefreshClientsStates();
    m_ClientsToErase.clear();

}

IServerUniquePtr IServer::Create()
{
    return std::make_unique<CServerImplementation>();
}