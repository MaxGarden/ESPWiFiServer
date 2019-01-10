#include "pch.h"
#include "BinaryTransmissionService.h"

bool CBinaryTransmissionService::TransmitHighState(unsigned short int durationInMs)
{
    return TransmitCommand(s_HighStateTypeCommand, durationInMs);
}

bool CBinaryTransmissionService::TransmitLowState(unsigned short int durationInMs)
{
    return TransmitCommand(s_LowStateTypeCommand, durationInMs);
}

bool CBinaryTransmissionService::ClearQueue()
{
    return TransmitCommand(s_ClearQueueCommand, 0);
}

bool CBinaryTransmissionService::IsTransmitting() const noexcept
{
    return m_IsWaitingForResponse;
}

void CBinaryTransmissionService::OnReceived(const std::vector<byte>& payload)
{
    DEBUG_ASSERT(!payload.empty());
    if (payload.empty())
        return;

    DEBUG_ASSERT(m_IsWaitingForResponse);
    if (!m_IsWaitingForResponse)
        return;

    const auto command = payload.front();
    DEBUG_ASSERT(command == s_ClearQueueCommand);
    if (command != s_ClearQueueCommand)
        return;

    m_IsWaitingForResponse = false;
    OnFinishedTransmitting();
}

void CBinaryTransmissionService::OnFinishedTransmitting()
{
    //to override
}

bool CBinaryTransmissionService::TransmitCommand(byte type, unsigned short int argument)
{
    const auto& connection = GetConnection();
    DEBUG_ASSERT(connection);
    if (!connection)
        return false;

    const auto result = connection->Send({ type, static_cast<byte>(argument), static_cast<byte>(argument >> 8) });
    if (!result)
    {
        DEBUG_ASSERT(false);
        return false;
    }

    m_IsWaitingForResponse = true;
    return true;
}