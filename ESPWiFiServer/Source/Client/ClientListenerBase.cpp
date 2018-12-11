#include "pch.h"
#include "ClientListenerBase.h"

void CClientListenerBase::OnReceived(const Packet&)
{
    //to override
}

void CClientListenerBase::OnConnected()
{
    //to override
}

void CClientListenerBase::OnDisconnected()
{
    //to override
}