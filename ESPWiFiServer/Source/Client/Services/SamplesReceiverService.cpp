#include "pch.h"
#include "SamplesReceiverService.h"

void CSamplesReceiverService::Finalize()
{
    if (m_ReceiveCallback)
        EndReceiving();
}

bool CSamplesReceiverService::StartReceiving(unsigned short int samplesFrequency, unsigned short int sendFrequency, ReceiveCallback&& callback)
{
    if (m_ReceiveCallback || !callback || samplesFrequency <= 0)
    {
        DEBUG_ASSERT(false);
        return false;
    }

    const auto sendResult = Send({ s_StartTransmissionCommand, static_cast<byte>(samplesFrequency), static_cast<byte>(samplesFrequency >> 8),
                                                               static_cast<byte>(sendFrequency), static_cast<byte>(sendFrequency >> 8),
                                                               static_cast<byte>(m_TransmissionMedium)});
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

    m_ReceiveCallback({});
    m_ReceiveCallback = ReceiveCallback{};
    return true;
}

bool CSamplesReceiverService::IsReceiving() const noexcept
{
    return !!m_ReceiveCallback;
}

void CSamplesReceiverService::SetTransmissionMedium(TransmissionMedium medium)
{
    m_TransmissionMedium = medium;
}

TransmissionMedium CSamplesReceiverService::GetTransmissionMedium() const noexcept
{
    return m_TransmissionMedium;
}

void CSamplesReceiverService::OnReceived(const std::vector<byte>& payload)
{
    if (!m_ReceiveCallback)
        return;

    const auto payloadSize = payload.size();
    DEBUG_ASSERT(payloadSize % sizeof(SampleType) == 0);
    if (payloadSize % sizeof(SampleType) != 0)
        return;

    std::vector<SampleType> samples(payloadSize / sizeof(SampleType));
    memcpy(&samples[0], payload.data(), payloadSize);
    m_ReceiveCallback(std::move(samples));
}