#include "pch.h"
#include "ServerListenerBase.h"

void CServerListenerBase::OnClientConnected(const IClientSharedPtr&)
{
    //to override
}

void CServerListenerBase::OnClientDisconnected(const IClientSharedPtr&)
{
    //to override
}