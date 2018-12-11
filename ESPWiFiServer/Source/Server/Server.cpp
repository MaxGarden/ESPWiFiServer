#include "pch.h"
#include "Server.h"


CServer::CServer(QObject* parent /* = nullptr */) :
    QObject(parent),
    m_Server(this)
{
    Setup();
}

CServer::~CServer()
{
    if (m_Server.isListening())
        m_Server.close();
}

bool CServer::StartListening(unsigned short int port)
{
    DEBUG_ASSERT(!m_Server.isListening());
    if (m_Server.isListening())
        return false;

    return m_Server.listen(QHostAddress::AnyIPv4, port);
}

void CServer::Setup()
{
    connect(&m_Server, &QTcpServer::newConnection, this, &CServer::OnNewConnection);
}

void CServer::OnNewConnection()
{
    const auto iterator = std::remove_if(m_Clients.begin(), m_Clients.end(), [](const auto& client)
    {
        return !client || client->IsConnected();
    });

    m_Clients.erase(iterator, m_Clients.end());

    const auto clientSocket = m_Server.nextPendingConnection();
    DEBUG_ASSERT(clientSocket);
    if (!clientSocket)
        return;

    auto client = IClient::Create();
    client->SetSocket(clientSocket);

    m_Clients.emplace_back(std::move(client));
}