#if !defined(__CLIENT_BUILDER_BASE_H__)
#define __CLIENT_BUILDER_BASE_H__
#pragma once

#include "Client/ClientBuilder.h"
#include "Client/ClientController.h"

class CClientBuilderBase : public IClientBuilder
{
public:
    CClientBuilderBase() = default;
    virtual ~CClientBuilderBase() override = default;

    virtual bool Build(IClientController& controller) override;
};

#endif //__CLIENT_BUILDER_BASE_H__