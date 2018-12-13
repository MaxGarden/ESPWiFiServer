#if !defined(__CLIENT_CONTROLLER_LISTENER_BASE_H__)
#define __CLIENT_CONTROLLER_LISTENER_BASE_H__
#pragma once

#include "ClientController.h"

class CClientControllerListenerBase : public IClientControllerListener
{
public:
    virtual ~CClientControllerListenerBase() override = default;

    virtual void OnServicePaired(IClientService& service) override;
    virtual void OnServiceUnparied(IClientService& service) override;
};

#endif //__CLIENT_CONTROLLER_LISTENER_BASE_H__