#include "pch.h"
#include "ReceiverBuilder.h"
#include "Client/Services/ClientServiceBase.h"
#include "Client/Services/MorseCodeReceiverService.h"

bool CReceiverBuilder::Build(IClientController& controller)
{
    auto result = true;

    result &= controller.RegisterServiceFactory(std::make_unique<CClientServiceFactory<CSamplesReceiverService>>("samples"));

    return result;
}