#if !defined(__CLIENT_IMPLEMENTATION_H__)
#define __CLIENT_IMPLEMENTATION_H__
#pragma once

#include <QObject>
#include <QTcpSocket>
#include "Client.h"
#include "ListenableBase.h"

using PacketSizeType = unsigned short int;

class CClientImplementation final : public QObject, public CListenableBase<IClient, IClientListener>
{
    Q_OBJECT
public:
    CClientImplementation() = default;
    virtual ~CClientImplementation() override final = default;

    virtual bool IsConnected() const noexcept override final;
    virtual bool Send(const Packet& packet) override final;

    virtual bool Disconnect() override final;

    virtual void SetSocket(QTcpSocket* clientSocket) override final;

private:
    void Setup(QTcpSocket& socket);
    void CleanUp(QTcpSocket& socket);

    void TryReadHeader();
    void TryReadPacket();

private slots:
    void OnReadReady();

    void OnConnected();
    void OnDisconnected();

    void OnError(QAbstractSocket::SocketError socketError);

private:
    PacketSizeType m_QueuedPacketSize = 0u;
    QTcpSocket* m_Socket = nullptr;
};

#endif //__CLIENT_IMPLEMENTATION_H__