#include "pch.h"
#include "DeviceIdService.h"

bool CDeviceIdService::RequestDeviceId(CalllbackType&& callback)
{
    DEBUG_ASSERT(callback);
    if (!callback)
        return false;

    const auto result = Send({ s_ResolveDeviceTypeRequest });
    if (result)
        m_Callbacks.emplace_back(std::move(callback));

    return result;
}

void CDeviceIdService::OnReceived(const std::vector<byte>& payload)
{
    if (payload.size() != 1)
        return;

    const auto message = payload.front();
    if (message == s_ResolveDeviceTypeRequest)
        Send({ s_DeviceType });
    else
        NotifyCallbacks(message);
}

void CDeviceIdService::NotifyCallbacks(byte deviceType)
{
    for (const auto& callback : m_Callbacks)
    {
        if (callback)
            callback(deviceType);
    }

    m_Callbacks.clear();
}