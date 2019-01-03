#if !defined(__CLIENT_H__)
#define __CLIENT_H__
#pragma once

#include "DataModel.h"
#include <QTcpServer>

struct Packet
{
    byte ServicePort;
    std::vector<byte> Payload;
};

class IClientListener : public IListener
{
public:
    virtual ~IClientListener() override = default;

    virtual void OnReceived(const Packet& packet) = 0;

    virtual void OnConnected() = 0;
    virtual void OnDisconnected() = 0;
};

class IClient : public IDataModel
{
public:
    virtual ~IClient() override = default;

    virtual bool IsConnected() const noexcept = 0;
    virtual bool Send(const Packet& packet) = 0;

    virtual bool Disconnect() = 0;

    virtual void SetSocket(QTcpSocket* clientSocket) = 0;

    static IClientUniquePtr Create();
};

#endif //__CLIENT_H__