#if !defined(__CLIENT_SERVICE_BASE_H__)
#define __CLIENT_SERVICE_BASE_H__
#pragma once

#include "Client/ClientService.h"

class CClientServiceBase : public IClientService
{
public:
    CClientServiceBase() = default;
    virtual ~CClientServiceBase() override = default;

    virtual void OnBind(const IClientServiceConnectionSharedPtr& connection) override;
    virtual void OnUnbind(const IClientServiceConnectionSharedPtr& connection) override;

    virtual void OnReceived(const std::vector<byte>& payload) override;

protected:
    bool Send(std::vector<byte>&& payload);
    const IClientServiceConnectionSharedPtr& GetConnection() const noexcept;

private:
    IClientServiceConnectionSharedPtr m_Connection;
};

template<typename ClientServiceType>
class CClientServiceFactory final : public IClientServiceFactory
{
public:
    CClientServiceFactory(std::string&& serviceName) : m_ServiceName(std::move(serviceName)) {};
    virtual ~CClientServiceFactory() override final = default;

    virtual const std::string& GetServiceName() const noexcept override final { return m_ServiceName; }
    virtual IClientServiceUniquePtr Create() override final { return std::make_unique<ClientServiceType>(); }

private:
    const std::string m_ServiceName;
};

#endif //__CLIENT_SERVICE_BASE_H__