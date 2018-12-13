#if !defined(__SERVER_CONTROLLER_LISTENER_BASE_H__)
#define __SERVER_CONTROLLER_LISTENER_BASE_H__
#pragma once

#include "ServerController.h"

class CServerControllerListenerBase : public IServerControllerListener
{
public:
    CServerControllerListenerBase() = default;
    virtual ~CServerControllerListenerBase() override = default;

    virtual void OnClientConnected(const IClientControllerSharedPtr& client) override;
    virtual void OnClientDisconnected(const IClientControllerSharedPtr& client) override;
};

#endif //__SERVER_CONTROLLER_LISTENER_BASE_H__