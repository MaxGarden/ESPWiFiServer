#if !defined(__MORSE_CODE_TRANSMISSION_SERVICE_H__)
#define __MORSE_CODE_TRANSMISSION_SERVICE_H__
#pragma once

#include "BinaryTransmissionService.h"

enum class EMorseCodeState
{
    Dot,
    Dash,
    StateSpace,
    CharacterSpace
};
using MorseLetter = std::vector<EMorseCodeState>;

class CMorseCodeTransmissionService final : public CBinaryTransmissionService
{
public:
    using TransmissionFinishedCallback = std::function<void(bool)>;

public:
    CMorseCodeTransmissionService() = default;
    virtual ~CMorseCodeTransmissionService() override final = default;

    virtual bool Initialize() override final;
    virtual void Finalize() override final;

    bool CheckCharacter(char character) const noexcept;
    bool CheckText(const std::string& text) const noexcept;

    bool TransmitText(std::string&& text, TransmissionFinishedCallback&& callback);

    void SetDotDuration(unsigned int durationInMiliseconds) noexcept;
    unsigned int GetDotDuration() const noexcept;

protected:
    virtual void OnTransmissionEnded(bool success) override final;

private:
    bool LoadDictionary(const std::string& filename);
    bool TransmitCharacter(char character);
    bool TransmitMorseCodeState(EMorseCodeState state);

private:
    size_t m_TextToTransmissionOffset = 0;
    std::string m_TextToTransmission;

    std::unordered_map<char, const MorseLetter> m_MorseDictionary;
    unsigned int m_DotDurationInMiliSeconds = 50u;

    TransmissionFinishedCallback m_Callback;

    static const auto s_DashDurationToDotMultiplier = 3u;
    static const auto s_CharacterSpaceToStateSpaceMultipiler = 3u;
    static const auto s_DotCharacter = '.';
    static const auto s_DashCharacter = '-';

    static const auto s_MaxLettersNumberInOneTransmissionChunk = 32;
};

#endif //__MORSE_CODE_TRANSMISSION_SERVICE_H__