#if !defined(__TRANSMISSION_SERVICE_H__)
#define __TRANSMISSION_SERVICE_H__
#pragma once

#include "ClientServiceBase.h"

class CTransmissionService final : public CClientServiceBase
{
public:
    CTransmissionService() = default;
    virtual ~CTransmissionService() override final = default;

    bool TransmitHighState(unsigned short int durationInMs);
    bool TransmitLowState(unsigned short int durationInMs);
    bool ClearQueue();

    virtual void OnReceived(const std::vector<byte>& payload) override final;

private:
    bool TransmitCommand(byte type, unsigned short int argument);

    static const byte s_HighStateTypeCommand = 'a';
    static const byte s_LowStateTypeCommand = 'n';
    static const byte s_ClearQueueCommand = 'c';
};

#endif //__TRANSMISSION_SERVICE_H__