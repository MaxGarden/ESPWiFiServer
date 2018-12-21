#if !defined(__CLIENT_BUILDER_H__)
#define __CLIENT_BUILDER_H__
#pragma once

#include "ClientController.h"

class IClientBuilder
{
public:
    virtual ~IClientBuilder() = default;

    virtual bool Build(IClientController& controller) = 0;
};

class IClientBuildersProvider
{
public:
    virtual ~IClientBuildersProvider() = default;

    virtual bool RegisterBuilder(byte deviceId, IClientBuilderUniquePtr&& builder) = 0;
    virtual bool UnregisterBuilder(byte deviceId) = 0;

    virtual IClientBuilder* GetBuilder(byte deviceId) const noexcept = 0;

    static IClientBuildersProviderUniquePtr Create();
};

#endif //__CLIENT_BUILDER_H__