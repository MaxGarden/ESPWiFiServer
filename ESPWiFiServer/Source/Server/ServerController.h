#if !defined(__SERVER_CONTROLLER_H__)
#define __SERVER_CONTROLLER_H__
#pragma once

#include "Controller.h"

class IServerControllerListener : public IListener
{
public:
    virtual ~IServerControllerListener() override = default;

    virtual void OnClientConnected(const IClientControllerSharedPtr& client) = 0;
    virtual void OnClientDisconnected(const IClientControllerSharedPtr& client) = 0;
};

class IServerController : public IController
{
public:
    virtual ~IServerController() override = default;

    virtual void VisitClients(const ConstVisitorType<IClientControllerSharedPtr>& visitor) = 0;

    static IServerControllerUniquePtr Create();
};

#endif //__SERVER_CONTROLLER_H__