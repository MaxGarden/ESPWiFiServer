#if !defined(__SERVER_VIEW_IMPLEMENTATION_H__)
#define __SERVER_VIEW_IMPLEMENTATION_H__
#pragma once

#include "ViewBase.h"
#include "ServerView.h"
#include "ServerController.h"
#include "Client/ClientView.h"
#include "Client/ClientController.h"
#include "ui_ServerView.h"

class CServerViewImplementation final : public CViewBase<IServerView, IServerController>, public Ui::ServerView
{
    Q_OBJECT

public:
    CServerViewImplementation(const IClientBuildersProvider& buildersProvider, QWidget* parent = nullptr);
    virtual ~CServerViewImplementation() override final = default;

    virtual bool RegisterClientViewFactory(byte deviceId, IClientViewFactoryUniquePtr&& factory) override final;
    virtual bool UnregisterClientViewFactory(byte deviceId) override final;

    void OnClientConnected(const IClientControllerSharedPtr& clientController);
    void OnClientDisconnected(const IClientControllerSharedPtr& clientController);

protected:
    virtual void OnControllerChanged() override final;
    virtual IListenerUniquePtr CreateListener() override final;

private:
    void OnResolvedDeviceType(const IClientPairView& clientPairView, const std::optional<byte>& deviceId);
    void CreateClientView(IClientControllerSharedPtr clientController, byte deviceId);
    void AddClientViewTab(const IClientControllerSharedPtr& clientController, IClientViewUniquePtr&& clientView);

private:
    const IClientBuildersProvider& m_BuildersProvider;
    std::map<byte, IClientViewFactoryUniquePtr> m_ClientsFactories;
    std::map<IClientControllerSharedPtr, IClientViewUniquePtr> m_Clients;
};

#endif //__SERVER_VIEW_IMPLEMENTATION_H__