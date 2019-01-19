#include "pch.h"
#include "SamplesReceiverService.h"

void CSamplesReceiverService::Finalize()
{
    if (m_ReceiveCallback)
        EndReceiving();
}

bool CSamplesReceiverService::StartReceiving(unsigned short int samplesFrequency, ReceiveCallback&& callback)
{
    if (m_ReceiveCallback || !callback || samplesFrequency <= 0)
    {
        DEBUG_ASSERT(false);
        return false;
    }

    const auto sendResult = Send({ s_StartTransmissionCommand, static_cast<byte>(samplesFrequency), static_cast<byte>(samplesFrequency >> 8) });
    DEBUG_ASSERT(sendResult);
    if (!sendResult)
        return false;

    m_ReceiveCallback = std::move(callback);
    return true;
}

bool CSamplesReceiverService::EndReceiving()
{
    DEBUG_ASSERT(m_ReceiveCallback);
    if (!m_ReceiveCallback)
        return false;

    const auto sendResult = Send({ s_EndTransmissionCommand });
    DEBUG_ASSERT(sendResult);
    if (!sendResult)
        return false;

    m_ReceiveCallback(std::nullopt);
    m_ReceiveCallback = ReceiveCallback{};
    return true;
}

void CSamplesReceiverService::OnReceived(const std::vector<byte>& payload)
{
    DEBUG_ASSERT(m_ReceiveCallback);
    if (!m_ReceiveCallback)
        return;

    DEBUG_ASSERT(payload.size() == 4);
    if (payload.size() != 4)
        return;

    int sample;
    memcpy(&sample, payload.data(), payload.size());

    m_ReceiveCallback(sample);
}