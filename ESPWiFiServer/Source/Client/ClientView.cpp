#include "pch.h"
#include "ClientView.h"
#include "ClientController.h"
#include "ClientControllerListenerBase.h"
#include "ViewBase.h"

class CClientView;
class CClientViewListener final : public CClientControllerListenerBase
{
public:
    CClientViewListener(CClientView& clientView);
    virtual ~CClientViewListener() = default;

    virtual void OnServicePaired(IClientService& service) override final;
    virtual void OnServiceUnparied(IClientService& service) override final;

private:
    CClientView& m_ClientView;
};

class CClientView final : public CViewBase<IClientView, IClientController>
{
public:
    CClientView() = default;
    virtual ~CClientView() override final = default;

    const std::string& GetName() const noexcept override final;

    void OnServicePaired(IClientService& service);
    void OnServiceUnpaired(IClientService& service);

protected:
    virtual void OnControllerChanged() override final;
    virtual IListenerUniquePtr CreateListener() override final;
};

CClientViewListener::CClientViewListener(CClientView& clientView) :
    m_ClientView(clientView)
{
}

void CClientViewListener::OnServicePaired(IClientService& service)
{
    m_ClientView.OnServicePaired(service);
}

void CClientViewListener::OnServiceUnparied(IClientService& service)
{
    m_ClientView.OnServiceUnpaired(service);
}


#include "Client/Services/ClientServiceBase.h"
#include "Client/Services/DeviceIdService.h"

const std::string& CClientView::GetName() const noexcept
{
    static std::string name = "test";
    return name;
}


void CClientView::OnServicePaired(IClientService& service)
{
    /*auto deviceIdService = dynamic_cast<CDeviceIdService&>(service);
    deviceIdService.RequestDeviceId([](const auto deviceId)
    {
        DEBUG_ASSERT(deviceId != 0);
    });*/
}

void CClientView::OnServiceUnpaired(IClientService& service)
{
    //TODO
}

void CClientView::OnControllerChanged()
{
    const auto& controller = GetController();
    if (!controller)
        return;

    //controller->RegisterServiceFactory(std::make_unique<CClientServiceFactory<CDeviceIdService>>("deviceid"));
    controller->PairServices();
}

IListenerUniquePtr CClientView::CreateListener()
{
    return std::make_unique<CClientViewListener>(*this);
}

IClientViewUniquePtr IClientView::Create()
{
    return std::make_unique<CClientView>();
}