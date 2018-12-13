#include "pch.h"
#include "ClientViewBase.h"

CClientViewBaseListener::CClientViewBaseListener(CClientViewBase& clientView) :
    m_ClientView(clientView)
{
}

void CClientViewBaseListener::OnServicePaired(IClientService& service)
{
    m_ClientView.OnServicePaired(service);
}

void CClientViewBaseListener::OnServiceUnparied(IClientService& service)
{
    m_ClientView.OnServiceUnpaired(service);
}

void CClientViewBase::OnServicePaired(IClientService&)
{
    //to override
}

void CClientViewBase::OnServiceUnpaired(IClientService&)
{
    //to override
}

IListenerUniquePtr CClientViewBase::CreateListener()
{
    return std::make_unique<CClientViewBaseListener>(*this);
}