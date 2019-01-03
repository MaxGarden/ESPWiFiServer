#if !defined(__CLIENT_CONTROLLER_H__)
#define __CLIENT_CONTROLLER_H__
#pragma once

#include "Controller.h"
#include "ClientService.h"

class IClientControllerListener : public IListener
{
public:
    virtual ~IClientControllerListener() override = default;

    virtual void OnServicePaired(IClientService& service) = 0;
    virtual void OnServiceUnparied(IClientService& service) = 0;
};

class IClientController : public IController
{
public:
    virtual ~IClientController() override = default;

    virtual bool RegisterServiceFactory(IClientServiceFactoryUniquePtr&& serviceFactory) = 0;
    virtual void VisitServices(const VisitorType<IClientService>& visitor) const = 0;

    virtual void PairServices() = 0;

    virtual bool Disconnect() = 0;

    static IClientControllerUniquePtr Create();
};

#endif //__CLIENT_CONTROLLER_H__