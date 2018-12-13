#if !defined(__SERVER_H__)
#define __SERVER_H__
#pragma once

#include "DataModel.h"
#include "Client/Client.h"

class IServerListener : public IListener
{
public:
    virtual ~IServerListener() override = default;

    virtual void OnClientConnected(const IClientSharedPtr& client) = 0;
    virtual void OnClientDisconnected(const IClientSharedPtr& client) = 0;
};

class IServer : public IDataModel
{
public:
    virtual ~IServer() override = default;

    virtual bool StartListening(unsigned short int port) = 0;
    virtual void VisitClients(const ConstVisitorType<IClientSharedPtr>& visitor) = 0;

    static IServerUniquePtr Create();
};

#endif //__SERVER_H__