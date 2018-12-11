#if !defined(__CLIENT_CONTROLLER_H__)
#define __CLIENT_CONTROLLER_H__
#pragma once

#include "Controller.h"

class IClientControllerListener : public IListener
{
public:
    virtual ~IClientControllerListener() override = default;
};

class IClientController : public IController
{
public:
    virtual ~IClientController() override = default;

    static IClientControllerUniquePtr Create();
};

#endif //__CLIENT_CONTROLLER_H__