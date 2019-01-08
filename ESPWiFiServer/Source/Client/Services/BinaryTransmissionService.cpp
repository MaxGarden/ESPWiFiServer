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

void CBinaryTransmissionService::OnReceived(const std::vector<byte>& payload)
{
    DEBUG_ASSERT(!payload.empty());
    if (payload.empty())
        return;

    DEBUG_ASSERT(!m_SentCommands.empty());
    if (m_SentCommands.empty())
        return;

    const auto command = payload.front();
    if (command != m_SentCommands.front())
    {
        DEBUG_ASSERT(false);
        return;
    }

    m_SentCommands.pop();

    if (command == s_ClearQueueCommand)
        m_SentCommands = std::queue<byte>{};

    OnFinishedCommand(command == s_ClearQueueCommand);
}

void CBinaryTransmissionService::OnFinishedCommand(bool)
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

    m_SentCommands.emplace(type);
    return true;
}