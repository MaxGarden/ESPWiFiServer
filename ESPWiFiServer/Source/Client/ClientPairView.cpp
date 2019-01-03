#include "pch.h"
#include "ClientPairView.h"
#include "ClientController.h"
#include "ClientControllerListenerBase.h"
#include "ViewBase.h"

class CClientPairView;
class CClientPairViewListener final : public CClientControllerListenerBase
{
public:
    CClientPairViewListener(CClientPairView& clientPairView);
    virtual ~CClientPairViewListener() = default;

    virtual void OnServicePaired(IClientService& service) override final;

private:
    CClientPairView& m_ClientPairView;
};

class CClientPairView final : public CViewBase<IClientPairView, IClientController>
{
public:
    CClientPairView(const IClientBuildersProvider& buildersProvider, QWidget* parent = nullptr);
    virtual ~CClientPairView() override final = default;

    virtual const std::string& GetName() const noexcept override final;

    virtual bool ResolveDeviceType(ResolveDeviceTypeCallback&& callback) override final;

    void OnServicePaired(IClientService& service);

protected:
    virtual void OnControllerChanged() override final;
    virtual IListenerUniquePtr CreateListener() override final;

private:
    const IClientBuildersProvider& m_BuildersProvider;
    ResolveDeviceTypeCallback m_Callback;
};

CClientPairViewListener::CClientPairViewListener(CClientPairView& clientPairView) :
    m_ClientPairView(clientPairView)
{
}

void CClientPairViewListener::OnServicePaired(IClientService& service)
{
    m_ClientPairView.OnServicePaired(service);
}

CClientPairView::CClientPairView(const IClientBuildersProvider& buildersProvider, QWidget* parent /* = nullptr */) :
    m_BuildersProvider(buildersProvider),
    QWidget(parent)
{
}

const std::string& CClientPairView::GetName() const noexcept
{
    static const std::string name = "Unknown device (resolving)";
    return name;
}

bool CClientPairView::ResolveDeviceType(ResolveDeviceTypeCallback&& callback)
{
    const auto& controller = GetController();
    DEBUG_ASSERT(controller);
    if (!controller)
        return false;

    DEBUG_ASSERT(!m_Callback);
    if (m_Callback)
        return false;

    controller->PairServices();

    m_Callback = std::move(callback);
    return true;
}

void CClientPairView::OnServicePaired(IClientService& service)
{
    auto deviceIdService = dynamic_cast<CDeviceIdService*>(&service);
    DEBUG_ASSERT(deviceIdService);
    if (!deviceIdService)
        return;

    deviceIdService->Pair([this](const auto& deviceId)
    {
        m_Callback(deviceId);
        m_Callback = ResolveDeviceTypeCallback{};
    });
}

void CClientPairView::OnControllerChanged()
{
    const auto& controller = GetController();
    if (!controller)
        return;

    const auto registerResult = controller->RegisterServiceFactory(std::make_unique<CDeviceIdServiceFactory>(*controller, m_BuildersProvider));
    DEBUG_ASSERT(registerResult);
}

IListenerUniquePtr CClientPairView::CreateListener()
{
    return std::make_unique<CClientPairViewListener>(*this);
}

IClientPairViewUniquePtr IClientPairView::Create(const IClientBuildersProvider& clientBuildersProvider, QWidget* parent /*= nullptr*/)
{
    return std::make_unique<CClientPairView>(clientBuildersProvider, parent);
}