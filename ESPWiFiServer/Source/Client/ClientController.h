#if !defined(__CLIENT_CONTROLLER_H__)
#define __CLIENT_CONTROLLER_H__
#pragma once

#include "Controller.h"
#include "ClientService.h"

class IClientControllerListener : public IListener
{
public:
    virtual ~IClientControllerListener() override = default;
};

class IClientController : public IController
{
public:
    virtual ~IClientController() override = default;

    virtual bool RegisterService(IClientServiceFactory& factory) = 0;
    virtual void VisitServices(const std::function<void(IClientService&)>& visitor) const = 0;

    static IClientControllerUniquePtr Create();
};

#endif //__CLIENT_CONTROLLER_H__