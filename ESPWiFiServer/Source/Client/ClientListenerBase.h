#if !defined(__CLIENT_LISTENER_BASE_H__)
#define __CLIENT_LISTENER_BASE_H__
#pragma once

#include "Client.h"

class CClientListenerBase : public IClientListener
{
public:
    virtual ~CClientListenerBase() override = default;

    virtual void OnReceived(const Packet& packet) override;

    virtual void OnConnected() override;
    virtual void OnDisconnected() override;
};

#endif //__CLIENT_LISTENER_BASE_H__