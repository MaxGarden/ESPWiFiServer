#if !defined(__TRANSMITTER_BUILDER_H__)
#define __TRANSMITTER_BUILDER_H__
#pragma once

#include "ClientBuilderBase.h"

class CTranmitterBuilder final : public CClientBuilderBase
{
public:
    CTranmitterBuilder() = default;
    virtual ~CTranmitterBuilder() override final = default;

    virtual bool Build(IClientController& controller) override final;
};

#endif //__TRANSMITTER_BUILDER_H__