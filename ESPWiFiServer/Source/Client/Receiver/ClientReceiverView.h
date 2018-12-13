#if !defined(__CLIENT_RECEIVER_VIEW_H__)
#define __CLIENT_RECEIVER_VIEW_H__
#pragma once

#include "Client/ClientViewBase.h"

class CClientReceiverView final : public CClientViewBase
{
public:
    CClientReceiverView() = default;
    virtual ~CClientReceiverView() override final = default;
};

#endif //__CLIENT_RECEIVER_VIEW_H__