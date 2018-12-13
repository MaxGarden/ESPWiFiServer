#if !defined(__CLIENT_TRANSMITTER_VIEW_H__)
#define __CLIENT_TRANSMITTER_VIEW_H__
#pragma once

#include "Client/ClientViewBase.h"

class CClientTransmitterView final : public CClientViewBase
{
public:
    CClientTransmitterView() = default;
    virtual ~CClientTransmitterView() override final = default;
};

#endif //__CLIENT_TRANSMITTER_VIEW_H__