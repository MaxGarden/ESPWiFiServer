#include "pch.h"
#include "Server.h"

CServer::CServer(QObject* parent /* = nullptr */) :
    QObject(parent),
    m_server(this)
{
    Setup();
}

CServer::~CServer()
{
    if (m_server.isListening())
        m_server.close();
}

bool CServer::StartListening(unsigned short int port)
{
    DEBUG_ASSERT(!m_server.isListening());
    if (m_server.isListening())
        return false;

    return m_server.listen(QHostAddress::LocalHost, port);
}

void CServer::Setup()
{
    connect(&m_server, &QTcpServer::newConnection, this, &CServer::OnNewConnection);
}

void CServer::OnNewConnection()
{

}