#if !defined(__BINARY_TRANSMISSION_SERVICE_H__)
#define __BINARY_TRANSMISSION_SERVICE_H__
#pragma once

#include "ClientServiceBase.h"

class CBinaryTransmissionService : public CClientServiceBase
{
public:
    CBinaryTransmissionService() = default;
    virtual ~CBinaryTransmissionService() override = default;

    bool TransmitHighState(unsigned short int durationInMs);
    bool TransmitLowState(unsigned short int durationInMs);
    bool ClearQueue();

    virtual void OnReceived(const std::vector<byte>& payload) override final;

protected:
    virtual void OnFinishedCommand(bool clear);

private:
    bool TransmitCommand(byte type, unsigned short int argument);

private:
    std::queue<byte> m_SentCommands;

    static const byte s_HighStateTypeCommand = 'a';
    static const byte s_LowStateTypeCommand = 'n';
    static const byte s_ClearQueueCommand = 'c';
};

#endif //__BINARY_TRANSMISSION_SERVICE_H__