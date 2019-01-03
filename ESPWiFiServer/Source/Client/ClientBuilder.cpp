#include "pch.h"
#include "ClientBuilder.h"

class CClientBuilderProvider final : public IClientBuildersProvider
{
public:
    CClientBuilderProvider() = default;
    virtual ~CClientBuilderProvider() override final = default;

    virtual bool RegisterBuilder(byte deviceId, IClientBuilderUniquePtr&& builder) override final;
    virtual bool UnregisterBuilder(byte deviceId) override final;

    virtual IClientBuilder* GetBuilder(byte deviceId) const noexcept override final;

private:
    std::map<byte, IClientBuilderUniquePtr> m_ClientBuilders;
};

bool CClientBuilderProvider::RegisterBuilder(byte deviceId, IClientBuilderUniquePtr&& builder)
{
    const auto iterator = m_ClientBuilders.find(deviceId);
    DEBUG_ASSERT(iterator == m_ClientBuilders.cend());
    if (iterator != m_ClientBuilders.cend())
        return false;

    m_ClientBuilders.emplace(deviceId, std::move(builder));
    return true;
}

bool CClientBuilderProvider::UnregisterBuilder(byte deviceId)
{
    const auto result = m_ClientBuilders.erase(deviceId) == 1;
    DEBUG_ASSERT(result);
    return result;
}

IClientBuilder* CClientBuilderProvider::GetBuilder(byte deviceId) const noexcept
{
    const auto iterator = m_ClientBuilders.find(deviceId);
    DEBUG_ASSERT(iterator != m_ClientBuilders.cend());
    if (iterator == m_ClientBuilders.cend())
        return nullptr;

    return iterator->second.get();
}

IClientBuildersProviderUniquePtr IClientBuildersProvider::Create()
{
    return std::make_unique<CClientBuilderProvider>();
}