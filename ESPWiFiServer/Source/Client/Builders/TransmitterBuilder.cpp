#include "pch.h"
#include "TransmitterBuilder.h"
#include "Client/Services/ClientServiceBase.h"
#include "Client/Services/TransmissionService.h"

bool CTranmitterBuilder::Build(IClientController& controller)
{
    auto result = true;

    result &= controller.RegisterServiceFactory(std::make_unique<CClientServiceFactory<CTransmissionService>>("transmission"));

    return result;
}