#include "pch.h"
#include "SamplesToBinaryReceiverService.h"

bool CSamplesToBinaryReceiverService::StartReceiving(unsigned short int samplesFrequency, int binaryTreshold, ReceiveCallback&& callback)
{
    if (m_ReceiveCallback || !callback)
    {
        DEBUG_ASSERT(false);
        return false;
    }

    const auto result = CSamplesReceiverService::StartReceiving(samplesFrequency, [this](auto sample)
    {
        if (sample)
            OnReceivedSample(*sample);
    });

    if (!result)
        return false;

    m_SampleDuration = 1.0 / samplesFrequency;
    m_BinaryTreshold = binaryTreshold;
    m_ReceiveCallback = std::move(callback);

    return true;
}

bool CSamplesToBinaryReceiverService::EndReceiving()
{
    DEBUG_ASSERT(m_ReceiveCallback);
    if (!m_ReceiveCallback)
        return false;

    if (!CSamplesReceiverService::EndReceiving())
        return false;

    m_ReceiveCallback(std::nullopt, 0u);
    m_ReceiveCallback = ReceiveCallback();
    return true;
}

void CSamplesToBinaryReceiverService::OnReceivedSample(int sample)
{
    DEBUG_ASSERT(m_ReceiveCallback);
    if (!m_ReceiveCallback)
        return;

    const auto sampleState = sample >= m_BinaryTreshold;
    if (!m_CurrentState || *m_CurrentState != sampleState)
    {
        if (m_CurrentState)
            m_ReceiveCallback(*m_CurrentState, m_CurrentStateDuration);

        m_CurrentState = sampleState;
        m_CurrentStateDuration = m_SampleDuration;
    }
    else if (*m_CurrentState == sampleState)
        m_CurrentStateDuration += m_SampleDuration;
}