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

template<byte serviceId, typename ClientServiceType>
class CClientServiceFactory final : public IClientServiceFactory
{
public:
    CClientServiceFactory() = default;
    virtual ~CClientServiceFactory() override final = default;

    virtual byte getServiceId() const noexcept override final { return serviceId; }
    virtual IClientServiceUniquePtr Create() override final { return std::make_unique<ClientServiceType>(); }
};

#endif //__CLIENT_SERVICE_BASE_H__