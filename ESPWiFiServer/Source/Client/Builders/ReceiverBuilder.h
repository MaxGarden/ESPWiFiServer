#if !defined(__RECEIVER_BUILDER_H__)
#define __RECEIVER_BUILDER_H__
#pragma once

#include "ClientBuilderBase.h"

class CReceiverBuilder final : public CClientBuilderBase
{
public:
    CReceiverBuilder() = default;
    virtual ~CReceiverBuilder() override final = default;

    virtual bool Build(IClientController& controller) override final;
};

#endif //__RECEIVER_BUILDER_H__