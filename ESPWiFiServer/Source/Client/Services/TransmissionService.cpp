#include "pch.h"
#include "TransmissionService.h"

bool CTransmissionService::TransmitHighState(unsigned short int durationInMs)
{
    return TransmitCommand(s_HighStateTypeCommand, durationInMs);
}

bool CTransmissionService::TransmitLowState(unsigned short int durationInMs)
{
    return TransmitCommand(s_LowStateTypeCommand, durationInMs);
}

bool CTransmissionService::ClearQueue()
{
    return TransmitCommand(s_ClearQueueCommand, 0);
}

void CTransmissionService::OnReceived(const std::vector<byte>&)
{
    //TODO
}

bool CTransmissionService::TransmitCommand(byte type, unsigned short int argument)
{
    const auto& connection = GetConnection();
    DEBUG_ASSERT(connection);
    if (!connection)
        return false;

    return connection->Send({ type, static_cast<byte>(argument << 8), static_cast<byte>(argument) });
}