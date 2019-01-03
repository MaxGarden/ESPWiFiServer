#if !defined(__DEVICE_ID_SERVICE_H__)
#define __DEVICE_ID_SERVICE_H__
#pragma once

#include "ClientServiceBase.h"
#include "Client/ClientBuilder.h"

class CDeviceIdService final : public CClientServiceBase
{
    using RequestCallbackType = std::function<void(byte)>;

public:
    using PairCallbackType = std::function<void(const std::optional<byte>&)>;

    CDeviceIdService(IClientController& controller, const IClientBuildersProvider& buildersProvider);
    virtual ~CDeviceIdService() override final = default;

    bool Pair(PairCallbackType&& callback);

    virtual void OnReceived(const std::vector<byte>& payload) override final;

private:
    bool RequestDeviceId(RequestCallbackType&& callback);

private:
    IClientController& m_Controller;
    const IClientBuildersProvider& m_BuildersProvider;

    RequestCallbackType m_RequestCallback;

    static const byte s_DeviceType = 'S';
    static const byte s_ResolveDeviceTypeRequest = '?';
};

class CDeviceIdServiceFactory final : public IClientServiceFactory
{
public:
    CDeviceIdServiceFactory(IClientController& controller, const IClientBuildersProvider& buildersProvider);
    virtual ~CDeviceIdServiceFactory() override final = default;

    virtual const std::string& GetServiceName() const noexcept override final;
    virtual IClientServiceUniquePtr Create() override final;

private:
    IClientController& m_Controller;
    const IClientBuildersProvider& m_BuildersProvider;
};

#endif //__DEVICE_ID_SERVICE_H__