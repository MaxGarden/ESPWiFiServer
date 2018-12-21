#include "pch.h"
#include "DeviceIdService.h"

CDeviceIdService::CDeviceIdService(IClientController& controller, const IClientBuildersProvider& buildersProvider) :
    m_Controller(controller),
    m_BuildersProvider(buildersProvider)
{
}

bool CDeviceIdService::Pair(PairCallbackType&& callback)
{
    DEBUG_ASSERT(callback);
    if (!callback)
        return false;

    const auto requestCallback = [this, callback = std::move(callback)](auto deviceId)
    {
        const auto builder = m_BuildersProvider.GetBuilder(deviceId);
        DEBUG_ASSERT(builder);
        if (!builder)
            return;

        const auto buildResult = builder->Build(m_Controller);
        DEBUG_ASSERT(buildResult);

        m_Controller.PairServices();
        callback(buildResult);
    };

    if (!RequestDeviceId(requestCallback))
    {
        callback(false);
        return false;
    }

    return true;
}

bool CDeviceIdService::RequestDeviceId(RequestCallbackType&& callback)
{
    DEBUG_ASSERT(callback);
    DEBUG_ASSERT(!m_RequestCallback);
    if (m_RequestCallback || !callback)
        return false;

    m_RequestCallback = std::move(callback);
    const auto result = Send({ s_ResolveDeviceTypeRequest });
    if (!result)
        m_RequestCallback = RequestCallbackType{};

    return result;
}

void CDeviceIdService::OnReceived(const std::vector<byte>& payload)
{
    if (payload.size() != 1)
        return;

    const auto message = payload.front();
    if (message == s_ResolveDeviceTypeRequest)
        Send({ s_DeviceType });
    else if (m_RequestCallback)
    {
        m_RequestCallback(message);
        m_RequestCallback = RequestCallbackType{};
    }
}

CDeviceIdServiceFactory::CDeviceIdServiceFactory(IClientController& controller, const IClientBuildersProvider& buildersProvider) :
    m_Controller(controller),
    m_BuildersProvider(buildersProvider)
{
}

const std::string& CDeviceIdServiceFactory::GetServiceName() const noexcept
{
    static const std::string serviceName = "deviceid";
    return serviceName;
}

IClientServiceUniquePtr CDeviceIdServiceFactory::Create()
{
    return std::make_unique<CDeviceIdService>(m_Controller, m_BuildersProvider);
}