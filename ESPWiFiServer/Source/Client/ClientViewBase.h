#if !defined(__CLIENT_VIEW_BASE_H__)
#define __CLIENT_VIEW_BASE_H__
#pragma once

#include "ClientView.h"
#include "ClientController.h"
#include "ClientControllerListenerBase.h"
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

protected:
    virtual void OnServicePaired(IClientService& service);
    virtual void OnServiceUnpaired(IClientService& service);

    virtual IListenerUniquePtr CreateListener() override final;
};

#endif //__CLIENT_VIEW_BASE_H__