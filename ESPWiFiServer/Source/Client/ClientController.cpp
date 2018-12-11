#include "pch.h"
#include "Client.h"
#include "ClientController.h"
#include "ClientService.h"
#include "ControllerBase.h"
#include "ClientListenerBase.h"

class CClientController;

class CClientControllerListener final : public CClientListenerBase
{
public:
    CClientControllerListener(CClientController& controller);
    virtual ~CClientControllerListener() override final = default;

    virtual void OnReceived(const Packet& packet) override final;

    virtual void OnConnected() override final;
    virtual void OnDisconnected() override final;

private:
    CClientController & m_Controller;
};

class CClientServiceConnection final : public IClientServiceConnection
{
public:
    CClientServiceConnection(byte serviceId, CClientController* controller);
    virtual ~CClientServiceConnection() override final = default;

    virtual bool Send(std::vector<byte>&& payload) override;

    void invalidate();

private:
    const byte m_ServiceId;
    CClientController* m_Controller;
};

class CClientController final : public CControllerBase<IClientController, IClient, IClientControllerListener>
{
public:
    CClientController() = default;
    virtual ~CClientController() override final = default;

    virtual bool RegisterService(IClientServiceFactory& factory) override final;
    virtual void VisitServices(const std::function<void(IClientService &)>& visitor) const override final;

    bool Send(byte serviceId, std::vector<byte>&& payload);

    void OnReceived(const Packet& packet);

    void OnConnected();
    void OnDisconnected();

protected:
    virtual void OnDataModelChanged() override final;

private:
    struct ServiceData
    {
        const byte ServiceId;
        IClientServiceUniquePtr Service;
        IClientServiceConnectionSharedPtr Connection;
    };

    std::map<byte, ServiceData> m_Services;
    bool m_IsConnected = false;
};

CClientControllerListener::CClientControllerListener(CClientController& controller) :
    m_Controller(controller)
{
}

void CClientControllerListener::OnReceived(const Packet& packet)
{
    m_Controller.OnReceived(packet);
}

void CClientControllerListener::OnConnected()
{
    m_Controller.OnConnected();
}

void CClientControllerListener::OnDisconnected()
{
    m_Controller.OnDisconnected();
}

CClientServiceConnection::CClientServiceConnection(byte serviceId, CClientController* controller) :
    m_ServiceId(serviceId),
    m_Controller(controller)
{
}

bool CClientServiceConnection::Send(std::vector<byte>&& payload)
{
    DEBUG_ASSERT(m_Controller);
    if (!m_Controller)
        return false;

    return m_Controller->Send(m_ServiceId, std::move(payload));
}

void CClientServiceConnection::invalidate()
{
    m_Controller = nullptr;
}

bool CClientController::RegisterService(IClientServiceFactory& factory)
{
    const auto serviceId = factory.GetServiceId();
    if (m_Services.find(serviceId) != m_Services.cend())
    {
        DEBUG_ASSERT(false);
        return false;
    }

    auto service = factory.Create();
    DEBUG_ASSERT(service);
    if (!service)
        return false;

    m_Services.emplace(serviceId, ServiceData{ serviceId, std::move(service), nullptr });
    return true;
}

void CClientController::VisitServices(const std::function<void(IClientService &)>& visitor) const
{
    if (!visitor)
        return;

    VisitObjectsMap(m_Services, [&visitor](const auto& serviceData)
    {
        if (const auto& service = serviceData.Service)
            visitor(*service);
    });
}

bool CClientController::Send(byte serviceId, std::vector<byte>&& payload)
{
    DEBUG_ASSERT(m_IsConnected);
    if (!m_IsConnected)
        return false;

    const auto client = GetDataModel();
    DEBUG_ASSERT(client);
    if (!client)
        return false;

    return client->Send(Packet{ serviceId, std::move(payload) });
}

void CClientController::OnReceived(const Packet& packet)
{
    DEBUG_ASSERT(m_IsConnected);
    if (!m_IsConnected)
        return;

    const auto serviceId = packet.Type;

    const auto iterator = m_Services.find(serviceId);
    DEBUG_ASSERT(iterator != m_Services.cend());
    if (iterator == m_Services.cend())
        return;

    if (const auto& service = iterator->second.Service)
        service->OnReceived(packet.Payload);
}

void CClientController::OnConnected()
{
    DEBUG_ASSERT(!m_IsConnected);
    if (m_IsConnected)
        return;

    VisitObjectsMap(m_Services, [this](auto& serviceData)
    {
        const auto& service = serviceData.Service;
        if (!service)
            return;

        const auto serviceConnection = std::make_shared<CClientServiceConnection>(serviceData.ServiceId, this);
        service->OnBind(serviceConnection);
    });

    m_IsConnected = false;
}

void CClientController::OnDisconnected()
{
    DEBUG_ASSERT(m_IsConnected);
    if (!m_IsConnected)
        return;

    VisitObjectsMap(m_Services, [](auto& serviceData)
    {
        const auto& service = serviceData.Service;
        if (!service)
            return;

        auto& connection = serviceData.Connection;

        service->OnUnbind(connection);

        if (const auto serviceConnection = dynamic_cast<CClientServiceConnection*>(connection.get()))
            serviceConnection->invalidate();

        connection.reset();
    });

    m_IsConnected = false;
}

void CClientController::OnDataModelChanged()
{
    const auto dataModel = GetDataModel();
    if (!dataModel)
    {
        if (m_IsConnected)
            OnDisconnected();
    }
    else
    {
        if (dataModel->IsConnected() == m_IsConnected)
            return;

        if (m_IsConnected)
            OnDisconnected();
        else
            OnConnected();
    }
}

IClientControllerUniquePtr IClientController::Create()
{
    return std::make_unique<CClientController>();
}