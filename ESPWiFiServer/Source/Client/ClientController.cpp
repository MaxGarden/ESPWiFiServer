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
    CClientController& m_Controller;
};

class CClientServiceConnection final : public IClientServiceConnection
{
public:
    CClientServiceConnection(byte servicePort, CClientController* controller);
    virtual ~CClientServiceConnection() override final = default;

    virtual bool Send(std::vector<byte>&& payload) override;

    void invalidate();

private:
    const byte m_servicePort;
    CClientController* m_Controller;
};

class CClientController final : public CControllerBase<IClientController, IClient, IClientControllerListener>
{
    using PairRequest = std::function<void(byte)>;

public:
    CClientController() = default;
    virtual ~CClientController() override final = default;

    virtual bool RegisterServiceFactory(IClientServiceFactoryUniquePtr&& serviceFactory) override final;
    virtual void VisitServices(const VisitorType<IClientService>& visitor) const override final;

    virtual void PairServices() override final;

    virtual bool Disconnect() override final;

    bool Send(byte servicePort, std::vector<byte>&& payload);

    void OnReceived(const Packet& packet);

    void OnConnected();
    void OnDisconnected();

protected:
    virtual void OnDataModelChanged() override final;
    virtual IListenerUniquePtr CreateListener() override final;

private:
    void UnpairServices();

    void SendPairRequest(IClientServiceFactory& factory);
    void OnPairResponse(const std::vector<byte>& payload);
    void OnPairResponse(const std::string& serviceName, byte servicePort);

private:
    struct ServiceData
    {
        const byte servicePort;
        IClientServiceUniquePtr Service;
        IClientServiceConnectionSharedPtr Connection;
    };

    std::vector<IClientServiceFactoryUniquePtr> m_ServicesFactories;
    std::map<byte, ServiceData> m_PairedServices;
    bool m_IsConnected = false;

    std::map<std::string, IClientServiceFactory&> m_PairRequests;

    static const byte s_PairServicePort = 0;
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

CClientServiceConnection::CClientServiceConnection(byte servicePort, CClientController* controller) :
    m_servicePort(servicePort),
    m_Controller(controller)
{
}

bool CClientServiceConnection::Send(std::vector<byte>&& payload)
{
    DEBUG_ASSERT(m_Controller);
    if (!m_Controller)
        return false;

    return m_Controller->Send(m_servicePort, std::move(payload));
}

void CClientServiceConnection::invalidate()
{
    m_Controller = nullptr;
}

bool CClientController::RegisterServiceFactory(IClientServiceFactoryUniquePtr&& serviceFactory)
{
    if (!serviceFactory)
        return false;

    m_ServicesFactories.emplace_back(std::move(serviceFactory));
    return true;
}

void CClientController::VisitServices(const VisitorType<IClientService>& visitor) const
{
    if (!visitor)
        return;

    VisitObjectsMap(m_PairedServices, [&visitor](const auto& serviceData)
    {
        if (const auto& service = serviceData.Service)
            visitor(*service);
    });
}

void CClientController::PairServices()
{
    UnpairServices();

    VisitPointersContainer(m_ServicesFactories, [this](auto& serviceFactory)
    {
        SendPairRequest(serviceFactory);
    });
}

bool CClientController::Disconnect()
{
    const auto& dataModel = GetDataModel();
    if (!dataModel)
        return false;

    return dataModel->Disconnect();
}

bool CClientController::Send(byte servicePort, std::vector<byte>&& payload)
{
    DEBUG_ASSERT(m_IsConnected);
    if (!m_IsConnected)
        return false;

    const auto client = GetDataModel();
    DEBUG_ASSERT(client);
    if (!client)
        return false;

    return client->Send(Packet{ servicePort, std::move(payload) });
}

void CClientController::OnReceived(const Packet& packet)
{
    DEBUG_ASSERT(m_IsConnected);
    if (!m_IsConnected)
        return;

    const auto servicePort = packet.ServicePort;

    if (servicePort == s_PairServicePort)
        return OnPairResponse(packet.Payload);

    const auto iterator = m_PairedServices.find(servicePort);
    DEBUG_ASSERT(iterator != m_PairedServices.cend());
    if (iterator == m_PairedServices.cend())
        return;

    if (const auto& service = iterator->second.Service)
        service->OnReceived(packet.Payload);
}

void CClientController::OnConnected()
{
    DEBUG_ASSERT(!m_IsConnected);
    if (m_IsConnected)
        return;

    m_IsConnected = true;
}

void CClientController::OnDisconnected()
{
    DEBUG_ASSERT(m_IsConnected);
    if (!m_IsConnected)
        return;

    UnpairServices();

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

IListenerUniquePtr CClientController::CreateListener()
{
    return std::make_unique<CClientControllerListener>(*this);
}

void CClientController::UnpairServices()
{
    VisitObjectsMap(m_PairedServices, [this](auto& serviceData)
    {
        const auto& service = serviceData.Service;
        if (!service)
            return;

        auto& connection = serviceData.Connection;

        service->OnUnbind(connection);

        if (const auto serviceConnection = dynamic_cast<CClientServiceConnection*>(connection.get()))
            serviceConnection->invalidate();

        connection.reset();

        NotifyListeners(&IClientControllerListener::OnServiceUnparied, *service);

        service->Finalize();
    });

    m_PairedServices.clear();
    m_PairRequests.clear();
}

void CClientController::SendPairRequest(IClientServiceFactory& factory)
{
    const auto& serviceName = factory.GetServiceName();
    std::vector<byte> payload(serviceName.cbegin(), serviceName.cend());
    const auto result = Send(s_PairServicePort, std::move(payload));
    DEBUG_ASSERT(result);
    if (result)
        m_PairRequests.emplace(serviceName, factory);
}

void CClientController::OnPairResponse(const std::vector<byte>& payload)
{
    DEBUG_ASSERT(!payload.empty());
    if (payload.empty())
        return;

    auto payloadOffset = 0u;
    byte servicePort;
    memcpy(&servicePort, payload.data(), sizeof(servicePort));
    payloadOffset += sizeof(servicePort);

    std::string serviceName;
    serviceName.resize(payload.size() - payloadOffset);

    memcpy(reinterpret_cast<void*>(serviceName.data()), payload.data() + payloadOffset, serviceName.size());
    OnPairResponse(serviceName, servicePort);
}

void CClientController::OnPairResponse(const std::string& serviceName, byte servicePort)
{
    const auto iterator = m_PairRequests.find(serviceName);
    DEBUG_ASSERT(iterator != m_PairRequests.cend());
    if (iterator == m_PairRequests.cend())
        return;

    auto& factory = iterator->second;

    auto service = factory.Create();
    m_PairRequests.erase(iterator);

    DEBUG_ASSERT(service);
    if (!service)
        return;

    if (!service->Initialize())
    {
        DEBUG_ASSERT(false);
        return;
    }

    const auto serviceRaw = service.get();
    const auto serviceConnection = std::make_shared<CClientServiceConnection>(servicePort, this);
    m_PairedServices.emplace(servicePort, ServiceData{ servicePort, std::move(service), serviceConnection });

    serviceRaw->OnBind(serviceConnection);
    NotifyListeners(&IClientControllerListener::OnServicePaired, *serviceRaw);
}

IClientControllerUniquePtr IClientController::Create()
{
    return std::make_unique<CClientController>();
}