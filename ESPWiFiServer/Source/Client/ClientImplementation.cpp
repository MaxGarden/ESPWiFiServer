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

    sentDataSize += m_Socket->write(reinterpret_cast<const char*>(&packetSize), sizeof(Packet));
    sentDataSize += m_Socket->write(reinterpret_cast<const char*>(&packet.Type), sizeof(byte));
    sentDataSize += m_Socket->write(reinterpret_cast<const char*>(packet.Payload.data()), packet.Payload.size() * sizeof(byte));

    DEBUG_ASSERT(sentDataSize == expectedSentDataSize);
    return sentDataSize == expectedSentDataSize;
}

void CClientImplementation::SetSocket(QTcpSocket* clientSocket)
{
    DEBUG_ASSERT(m_Socket == nullptr);
    if (m_Socket)
        CleanUp(*m_Socket);

    m_Socket = clientSocket;

    if (m_Socket)
        Setup(*m_Socket);
}

void CClientImplementation::Setup(QTcpSocket& socket)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(OnReadReady()));
}

void CClientImplementation::CleanUp(QTcpSocket& socket)
{
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

    memcpy(&receivedPacket.Type, packet.constData(), sizeof(byte));
    memcpy(receivedPacket.Payload.data(), (packet.constData() + sizeof(byte)), receivedPacket.Payload.size());
    m_QueuedPacketSize = 0u;

    NotifyListeners(&IClientListener::OnReceived, receivedPacket);
}

IClientUniquePtr IClient::Create()
{
    return std::make_unique<CClientImplementation>();
}