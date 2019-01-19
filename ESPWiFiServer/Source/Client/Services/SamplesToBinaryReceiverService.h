#if !defined(__SAMPLES_TO_BINARY_RECEIVER_SERVICE_H__)
#define __SAMPLES_TO_BINARY_RECEIVER_SERVICE_H__
#pragma once

#include "SamplesReceiverService.h"

class CSamplesToBinaryReceiverService : public CSamplesReceiverService
{
public:
    using ReceiveCallback = std::function<void(std::optional<bool>, unsigned short int)>;

public:
    CSamplesToBinaryReceiverService() = default;
    virtual ~CSamplesToBinaryReceiverService() override = default;

    bool StartReceiving(unsigned short int samplesFrequency, int binaryTreshold, ReceiveCallback&& callback);
    virtual bool EndReceiving() override;

private:
    void OnReceivedSample(int sample);

private:
    ReceiveCallback m_ReceiveCallback;
    int m_BinaryTreshold;
    double m_SampleDuration;

    std::optional<bool> m_CurrentState;
    double m_CurrentStateDuration;
};

#endif //__SAMPLES_TO_BINARY_RECEIVER_SERVICE_H__