#if !defined(__BINARY_TRANSMISSION_SERVICE_H__)
#define __BINARY_TRANSMISSION_SERVICE_H__
#pragma once

#include "ClientServiceBase.h"

enum class TransmissionMedium : char
{
    Sound,
    Infrared
};

class CBinaryTransmissionService : public CClientServiceBase
{
public:
    CBinaryTransmissionService() = default;
    virtual ~CBinaryTransmissionService() override = default;

    bool BeginTransaction();
    bool EndTransaction();

    bool TransmitHighState(unsigned short int durationInMs);
    bool TransmitLowState(unsigned short int durationInMs);
    bool ClearQueue();
    bool IsTransmitting() const noexcept;

    bool SetTransmissionMedium(TransmissionMedium medium);
    TransmissionMedium GetTransmissionMedium() const noexcept;

    virtual void OnReceived(const std::vector<byte>& payload) override final;

protected:
    virtual void OnTransmissionEnded(bool success);

private:
    bool TransmitCommand(byte type, unsigned short int argument);

private:
    bool m_IsWaitingForResponse = false;
    bool m_IsTransactionBegun = false;
    std::vector<byte> m_TransactionData;
    TransmissionMedium m_TransmissionMedium = TransmissionMedium::Sound;

    static const byte s_HighStateTypeCommand = 'a';
    static const byte s_LowStateTypeCommand = 'n';
    static const byte s_ClearQueueCommand = 'c';
    static const byte s_TransmissionMediumCommand = 'm';
    static const byte s_FinishedTransmittingCommand = 'f';
};

#endif //__BINARY_TRANSMISSION_SERVICE_H__