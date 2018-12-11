#if !defined(__DEVICE_TYPE_RESOLVER_SERVICE_H__)
#define __DEVICE_TYPE_RESOLVER_SERVICE_H__
#pragma once

#include "ClientServiceBase.h"

class CDeviceTypeResolverService final : public CClientServiceBase
{
public:
    using CalllbackType = std::function<void(byte)>;

    CDeviceTypeResolverService() = default;
    virtual ~CDeviceTypeResolverService() override final = default;

    virtual void OnReceived(const std::vector<byte>& payload) override final;

    bool ResolveDeviceType(CalllbackType&& callback);

private:
    void NotifyCallbacks(byte deviceType);

private:
    std::vector<CalllbackType> m_Callbacks;

    static const byte s_DeviceType = 'S';
    static const byte s_ResolveDeviceTypeRequest = '?';
};

#endif //__DEVICE_TYPE_RESOLVER_SERVICE_H__