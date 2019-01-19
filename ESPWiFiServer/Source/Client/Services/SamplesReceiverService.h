#if !defined(__SAMPLES_RECEIVER_SERVICE_H__)
#define __SAMPLES_RECEIVER_SERVICE_H__
#pragma once

#include "ClientServiceBase.h"

class CSamplesReceiverService : public CClientServiceBase
{
public:
    using ReceiveCallback = std::function<void(std::optional<int>)>;

public:
    CSamplesReceiverService() = default;
    virtual ~CSamplesReceiverService() override = default;

    virtual void Finalize() override;

    bool StartReceiving(unsigned short int samplesFrequency, ReceiveCallback&& callback);
    virtual bool EndReceiving();

    virtual void OnReceived(const std::vector<byte>& payload) override final;

private:
    ReceiveCallback m_ReceiveCallback;

    static const byte s_StartTransmissionCommand = 's';
    static const byte s_EndTransmissionCommand = 'e';
};

#endif //__SAMPLES_RECEIVER_SERVICE_H__