#include "pch.h"
#include "ClientImplementation.h"

bool CClientImplementation::IsConnected() const noexcept
{
    return m_Socket && m_Socket->isOpen();
}

bool CClientImplementation::Send(const Packet& packet)
{
    DEBUG_ASSERT(m_Socket && m_Socket->isOpen());
    if (!m_Socket || !m_Socket->isOpen())
        return false;

    const auto packetRawSize = sizeof(byte) + packet.Payload.size();

    if (packetRawSize > std::numeric_limits<PacketSizeType>::max())
        return false;

    const auto packetSize = static_cast<PacketSizeType>(packetRawSize);
    const auto expectedSentDataSize = packetSize + sizeof(PacketSizeType);

    auto sentDataSize = 0u;

    sentDataSize += m_Socket->write(reinterpret_cast<const char*>(&packetSize), sizeof(PacketSizeType));
    sentDataSize += m_Socket->write(reinterpret_cast<const char*>(&packet.ServicePort), sizeof(byte));
    sentDataSize += m_Socket->write(reinterpret_cast<const char*>(packet.Payload.data()), packet.Payload.size() * sizeof(byte));

    DEBUG_ASSERT(sentDataSize == expectedSentDataSize);
    return sentDataSize == expectedSentDataSize;
}

bool CClientImplementation::Disconnect()
{
    const auto isConnected = IsConnected();
    DEBUG_ASSERT(isConnected);
    if (!isConnected)
        return false;

    DEBUG_ASSERT(m_Socket);
    if (!m_Socket)
        return false;

    m_Socket->close();
    return true;
}

void CClientImplementation::SetSocket(QTcpSocket* clientSocket)
{
    DEBUG_ASSERT(m_Socket == nullptr);
    if (m_Socket)
        CleanUp(*m_Socket);

    if (IsConnected())
        OnDisconnected();

    m_Socket = clientSocket;

    if (m_Socket)
        Setup(*m_Socket);

    if (IsConnected())
        OnConnected();
}

void CClientImplementation::Setup(QTcpSocket& socket)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(OnReadReady()));
    connect(&socket, SIGNAL(connected()), this, SLOT(OnConnected()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));
}

void CClientImplementation::CleanUp(QTcpSocket& socket)
{
    disconnect(&socket, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));
    disconnect(&socket, SIGNAL(connected()), this, SLOT(OnConnected()));
    disconnect(&socket, SIGNAL(readyRead()), this, SLOT(OnReadReady()));
}

void CClientImplementation::OnReadReady()
{
    DEBUG_ASSERT(m_Socket);
    if (!m_Socket)
        return;

    if(m_QueuedPacketSize == 0)
        TryReadHeader();

    if (m_QueuedPacketSize != 0)
        TryReadPacket();
}

void CClientImplementation::OnConnected()
{
    NotifyListeners(&IClientListener::OnConnected);
}

void CClientImplementation::OnDisconnected()
{
    NotifyListeners(&IClientListener::OnDisconnected);
}

void CClientImplementation::TryReadHeader()
{
    static const auto headerSize = sizeof(PacketSizeType);
    if (m_Socket->bytesAvailable() < headerSize)
        return;

    const auto packetSize = m_Socket->read(headerSize);

    DEBUG_ASSERT(packetSize.size() == headerSize);
    if (packetSize.size() != headerSize)
        return;

    memcpy(&m_QueuedPacketSize, packetSize.constData(), headerSize);
}

void CClientImplementation::TryReadPacket()
{
    if (m_Socket->bytesAvailable() < m_QueuedPacketSize)
        return;

    const auto packet = m_Socket->read(m_QueuedPacketSize);

    DEBUG_ASSERT(packet.size() == m_QueuedPacketSize);
    if (packet.size() != m_QueuedPacketSize)
        return;

    Packet receivedPacket;

    receivedPacket.Payload.resize(m_QueuedPacketSize - sizeof(byte));

    memcpy(&receivedPacket.ServicePort, packet.constData(), sizeof(byte));
    memcpy(receivedPacket.Payload.data(), (packet.constData() + sizeof(byte)), receivedPacket.Payload.size());
    m_QueuedPacketSize = 0u;

    NotifyListeners(&IClientListener::OnReceived, receivedPacket);
}

IClientUniquePtr IClient::Create()
{
    return std::make_unique<CClientImplementation>();
}