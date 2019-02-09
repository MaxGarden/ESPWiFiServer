#if !defined(__SAMPLES_TO_BINARY_RECEIVER_SERVICE_H__)
#define __SAMPLES_TO_BINARY_RECEIVER_SERVICE_H__
#pragma once

#include "SamplesReceiverService.h"

class CSamplesToBinaryReceiverService : public CSamplesReceiverService
{
public:
    using StateType = std::pair<bool, double>;
    using ReceiveCallback = std::function<void(std::vector<StateType>&&)>;

public:
    CSamplesToBinaryReceiverService() = default;
    virtual ~CSamplesToBinaryReceiverService() override = default;

    bool StartReceiving(unsigned short int samplesFrequency, unsigned short int sendFrequency, SampleType binaryTreshold, unsigned short int binarySamplingFrequency, ReceiveCallback&& binaryCallback, CSamplesReceiverService::ReceiveCallback&& samplesCallback = CSamplesReceiverService::ReceiveCallback{});
    virtual bool EndReceiving() override;

private:
    void OnReceivedSamples(const std::vector<SampleType>& samples);

private:
    ReceiveCallback m_ReceiveCallback;
    int m_BinaryTreshold;
    double m_SampleDuration;
    double m_BinarySamplingDuration;

    SampleType m_MaximumSampleValue;
    double m_CurrentBinarySamplingDuration;

    std::optional<bool> m_CurrentState;
    double m_CurrentStateDuration;
};

#endif //__SAMPLES_TO_BINARY_RECEIVER_SERVICE_H__