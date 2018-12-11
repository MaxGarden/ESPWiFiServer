#if !defined(__CLIENT_H__)
#define __CLIENT_H__
#pragma once

#include "DataModel.h"

struct Packet
{
    byte m_Type;
    std::vector<byte> m_Payload;
};

class IClientListener : public IListener
{
public:
    virtual ~IClientListener() override = default;

    virtual void OnReceived(const Packet& packet) = 0;

    virtual void OnClosed() = 0;
};

class IClient : public IDataModel
{
public:
    virtual ~IClient() override = default;

    virtual bool Send(const Packet& packet) = 0;

    static IClientUniquePtr Create();
};

#endif //__CLIENT_H__