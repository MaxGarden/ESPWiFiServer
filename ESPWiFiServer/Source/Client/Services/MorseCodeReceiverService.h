#if !defined(__MORSE_CODE_RECEIVER_SERVICE_H__)
#define __MORSE_CODE_RECEIVER_SERVICE_H__
#pragma once

#include "SamplesToBinaryReceiverService.h"
#include "MorseCodeTransmissionService.h"

class CMorseCodeReceiverService final : public CSamplesToBinaryReceiverService
{
public:
    using ReceiveCallback = std::function<void(std::optional<char>)>;

public:
    CMorseCodeReceiverService() = default;
    virtual ~CMorseCodeReceiverService() override = default;

    virtual bool Initialize() override final;

    bool StartReceiving(unsigned short int samplesFrequency, int binaryTreshold, unsigned short int dotDurationInMiliseconds, ReceiveCallback&& callback);
    virtual bool EndReceiving() override final;

private:
    bool LoadDictionary(const std::string& filename);
    void OnReceivedState(bool state, unsigned short int stateDuration);

    std::optional<EMorseCodeState> DecodeMorseCodeState(bool state, unsigned short int stateDuration);

private:
    ReceiveCallback m_ReceiveCallback;
    unsigned short int m_DotDurationInMiliseconds;
    bool m_ErrorInCharacterTransmission = false;
    MorseLetter m_CurrentLetter;

    std::map<MorseLetter, const char> m_MorseDictionary;

    static const auto s_DashDurationToDotMultiplier = 3u;
    static const auto s_CharacterSpaceToStateSpaceMultipiler = 3u;
    static const auto s_DotCharacter = '.';
    static const auto s_DashCharacter = '-';

    static const double s_UncertaintyFactor;
};

#endif //__MORSE_CODE_RECEIVER_SERVICE_H__