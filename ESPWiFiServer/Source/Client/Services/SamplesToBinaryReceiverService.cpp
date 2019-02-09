#include "pch.h"
#include "SamplesToBinaryReceiverService.h"

bool CSamplesToBinaryReceiverService::StartReceiving(unsigned short int samplesFrequency, unsigned short int sendFrequency, SampleType binaryTreshold, unsigned short int binarySamplingFrequency, ReceiveCallback&& binaryCallback, CSamplesReceiverService::ReceiveCallback&& samplesCallback)
{
    if (m_ReceiveCallback || !binaryCallback)
    {
        DEBUG_ASSERT(false);
        return false;
    }

    const auto result = CSamplesReceiverService::StartReceiving(samplesFrequency, sendFrequency, [this, samplesCallback = std::move(samplesCallback)](auto&& samples)
    {
        OnReceivedSamples(samples);
        if (samplesCallback)
            samplesCallback(std::move(samples));
    });

    if (!result)
        return false;

    m_SampleDuration = 1.0 / std::max<unsigned short int>(1u, samplesFrequency);
    m_BinarySamplingDuration = 1.0 / std::max<unsigned short int>(1u, binarySamplingFrequency);
    m_BinaryTreshold = binaryTreshold;
    m_CurrentStateDuration = 0.0;
    m_MaximumSampleValue = SampleType{};
    m_ReceiveCallback = std::move(binaryCallback);

    return true;
}

bool CSamplesToBinaryReceiverService::EndReceiving()
{
    DEBUG_ASSERT(m_ReceiveCallback);
    if (!m_ReceiveCallback)
        return false;

    if (!CSamplesReceiverService::EndReceiving())
        return false;

    m_ReceiveCallback(std::vector<StateType>{});
    m_ReceiveCallback = ReceiveCallback{};
    return true;
}

void CSamplesToBinaryReceiverService::OnReceivedSamples(const std::vector<SampleType>& samples)
{
    DEBUG_ASSERT(m_ReceiveCallback);
    if (!m_ReceiveCallback)
        return;

    std::vector<StateType> states;
    for (const auto& sample : samples)
    {
        m_CurrentBinarySamplingDuration += m_SampleDuration;
        m_MaximumSampleValue = std::max(m_MaximumSampleValue, sample);
        m_CurrentStateDuration += m_SampleDuration;

        if(m_CurrentBinarySamplingDuration < m_BinarySamplingDuration)
            continue;

        const auto sampleState = m_MaximumSampleValue >= m_BinaryTreshold;
        if (!m_CurrentState || *m_CurrentState != sampleState)
        {
            if (m_CurrentState)
                states.emplace_back(*m_CurrentState, m_CurrentStateDuration);

            m_CurrentState = sampleState;
            m_CurrentStateDuration = 0;
        }

        m_MaximumSampleValue = SampleType{};
        m_CurrentBinarySamplingDuration = 0.0;
    }

    if (!states.empty())
        m_ReceiveCallback(std::move(states));
}