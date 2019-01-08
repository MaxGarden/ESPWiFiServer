#include "pch.h"
#include "TransmitterBuilder.h"
#include "Client/Services/ClientServiceBase.h"
#include "Client/Services/MorseCodeTransmissionService.h"

bool CTranmitterBuilder::Build(IClientController& controller)
{
    auto result = true;

    result &= controller.RegisterServiceFactory(std::make_unique<CClientServiceFactory<CMorseCodeTransmissionService>>("transmission"));

    return result;
}