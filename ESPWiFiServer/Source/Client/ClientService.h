#if !defined(__CLIENT_SERVICE_H__)
#define __CLIENT_SERVICE_H__
#pragma once

#include "Client.h"

class IClientServiceConnection
{
public:
    virtual ~IClientServiceConnection() = default;

    virtual bool Send(std::vector<byte>&& payload) = 0;
};

class IClientService
{
public:
    virtual ~IClientService() = default;

    virtual void OnBind(const IClientServiceConnectionSharedPtr& connection) = 0;
    virtual void OnUnbind(const IClientServiceConnectionSharedPtr& connection) = 0;

    virtual void OnReceived(const std::vector<byte>& payload) = 0;
};

class IClientServiceFactory
{
public:
    virtual ~IClientServiceFactory() = default;

    virtual byte GetServiceId() const noexcept = 0;

    virtual IClientServiceUniquePtr Create() = 0;
};

#endif //__CLIENT_SERVICE_H__