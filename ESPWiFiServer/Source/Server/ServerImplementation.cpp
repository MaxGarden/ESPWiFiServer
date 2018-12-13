#include "pch.h"
#include "ServerImplementation.h"

CServerImplementation::CServerImplementation(QObject* parent /* = nullptr */) :
    QObject(parent),
    m_Server(this)
{
    Setup();
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

void CServerImplementation::Setup()
{
    connect(&m_Server, &QTcpServer::newConnection, this, &CServerImplementation::OnNewConnection);
}

void CServerImplementation::OnNewConnection()
{
    const auto iterator = std::remove_if(m_Clients.begin(), m_Clients.end(), [](const auto& client)
    {
        return !client || client->IsConnected();
    });

    for (auto current = iterator; current != m_Clients.cend(); ++current)
    {
        if (const auto& client = *current)
            NotifyListeners(&IServerListener::OnClientDisconnected, client);
    }

    m_Clients.erase(iterator, m_Clients.end());

    const auto clientSocket = m_Server.nextPendingConnection();
    DEBUG_ASSERT(clientSocket);
    if (!clientSocket)
        return;

    auto client = IClient::Create();
    DEBUG_ASSERT(client);
    if (!client)
        return;

    client->SetSocket(clientSocket);

    m_Clients.emplace_back(std::move(client));
    NotifyListeners(&IServerListener::OnClientConnected, m_Clients.back());
}

IServerUniquePtr IServer::Create()
{
    return std::make_unique<CServerImplementation>();
}