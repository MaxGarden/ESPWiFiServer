#if !defined(__SERVER_LISTENER_BASE_H__)
#define __SERVER_LISTENER_BASE_H__
#pragma once

#include "Server.h"

class CServerListenerBase : public IServerListener
{
public:
    CServerListenerBase() = default;
    virtual ~CServerListenerBase() override = default;

    virtual void OnClientConnected(const IClientSharedPtr& client) override;
    virtual void OnClientDisconnected(const IClientSharedPtr& client) override;
};

#endif //__SERVER_LISTENER_BASE_H__