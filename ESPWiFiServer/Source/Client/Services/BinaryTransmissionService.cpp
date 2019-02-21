#include "pch.h"
#include "BinaryTransmissionService.h"

bool CBinaryTransmissionService::BeginTransaction()
{
    DEBUG_ASSERT(!m_IsTransactionBegun);
    if (m_IsTransactionBegun)
        return false;

    DEBUG_ASSERT(m_TransactionData.empty());
    if (!m_TransactionData.empty())
        m_TransactionData.clear();

    return (m_IsTransactionBegun = true);
}

bool CBinaryTransmissionService::EndTransaction()
{
    CScopedGuard scopedGuard{ [this]()
    {
        m_TransactionData.clear();
        m_IsTransactionBegun = false;
    } };

    DEBUG_ASSERT(m_IsTransactionBegun);
    if (!m_IsTransactionBegun)
        return false;

    const auto& connection = GetConnection();
    DEBUG_ASSERT(connection);
    if (!connection)
        return false;

    if (!connection->Send(std::move(m_TransactionData)))
    {
        DEBUG_ASSERT(false);
        m_IsWaitingForResponse = false;
        return false;
    }

    return true;
}

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

bool CBinaryTransmissionService::SetTransmissionMedium(TransmissionMedium medium)
{
    if (m_TransmissionMedium == medium)
        return true;

    if (TransmitCommand(s_TransmissionMediumCommand, static_cast<unsigned short int>(medium)))
    {
        m_TransmissionMedium = medium;
        return true;
    }

    return false;
}

TransmissionMedium CBinaryTransmissionService::GetTransmissionMedium() const noexcept
{
    return m_TransmissionMedium;
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
    const auto isFinishedSuccessful = command == s_FinishedTransmittingCommand;
    const auto isTransmissionEnded = isFinishedSuccessful || command == s_ClearQueueCommand;

    m_IsWaitingForResponse = false;

    if(isTransmissionEnded)
        OnTransmissionEnded(isFinishedSuccessful);
}

void CBinaryTransmissionService::OnTransmissionEnded(bool)
{
    //to override
}

bool CBinaryTransmissionService::TransmitCommand(byte type, unsigned short int argument)
{
    const std::array<byte, 3> dataToSend = { type, static_cast<byte>(argument), static_cast<byte>(argument >> 8)};

    if (m_IsTransactionBegun)
    {
        for (auto currentByte : dataToSend)
            m_TransactionData.emplace_back(currentByte);

        return (m_IsWaitingForResponse = true);
    }

    const auto& connection = GetConnection();
    DEBUG_ASSERT(connection);
    if (!connection)
        return false;

    const auto result = connection->Send({ dataToSend[0], dataToSend[1], dataToSend[2] });
    if (!result)
    {
        DEBUG_ASSERT(false);
        return false;
    }

    return (m_IsWaitingForResponse = true);
}