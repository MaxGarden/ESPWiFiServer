#include "pch.h"
#include "ServerControllerListenerBase.h"

void CServerControllerListenerBase::OnClientConnected(const IClientControllerSharedPtr&)
{
    //to override
}

void CServerControllerListenerBase::OnClientDisconnected(const IClientControllerSharedPtr&)
{
    //to override
}