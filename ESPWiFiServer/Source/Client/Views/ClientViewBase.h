#if !defined(__CLIENT_VIEW_BASE_H__)
#define __CLIENT_VIEW_BASE_H__
#pragma once

#include "Client/ClientView.h"
#include "Client/ClientController.h"
#include "Client/ClientControllerListenerBase.h"
#include "ViewBase.h"

class CClientViewBase;
class CClientViewBaseListener final : public CClientControllerListenerBase
{
public:
    CClientViewBaseListener(CClientViewBase& clientView);
    virtual ~CClientViewBaseListener() override final = default;

    virtual void OnServicePaired(IClientService& service) override final;
    virtual void OnServiceUnparied(IClientService& service) override final;

private:
    CClientViewBase & m_ClientView;
};

class CClientViewBase : public CViewBase<IClientView, IClientController>
{
    friend CClientViewBaseListener;
public:
    virtual ~CClientViewBase() override = default;

    virtual const std::string& GetName() const noexcept override;

protected:
    virtual void OnServicePaired(IClientService& service);
    virtual void OnServiceUnpaired(IClientService& service);

    virtual IListenerUniquePtr CreateListener() override final;
};

template<typename ClientViewType>
class CClientViewFactory final : public IClientViewFactory
{
public:
    CClientViewFactory() = default;
    virtual ~CClientViewFactory() override final = default;

    virtual IClientViewUniquePtr Create(QWidget* parent = nullptr) override final { return std::make_unique<ClientViewType>(parent); }
};


#endif //__CLIENT_VIEW_BASE_H__