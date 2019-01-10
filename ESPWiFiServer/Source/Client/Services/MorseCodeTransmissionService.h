#if !defined(__MORSE_CODE_TRANSMISSION_SERVICE_H__)
#define __MORSE_CODE_TRANSMISSION_SERVICE_H__
#pragma once

#include "BinaryTransmissionService.h"

enum class EMorseCodeState
{
    Dot,
    Dash,
    Space
};

class CMorseCodeTransmissionService final : public CBinaryTransmissionService
{
public:
    using TransmissionFinishedCallback = std::function<void()>;

public:
    CMorseCodeTransmissionService() = default;
    virtual ~CMorseCodeTransmissionService() override final = default;

    virtual bool Initialize() override final;
    virtual void Finalize() override final;

    bool CheckCharacter(char character) const noexcept;
    bool CheckText(const std::string& text) const noexcept;

    bool TransmitText(const std::string& text, TransmissionFinishedCallback&& callback);

    void SetDotDuration(unsigned int durationInMiliseconds) noexcept;
    unsigned int GetDotDuration() const noexcept;

protected:
    virtual void OnFinishedTransmitting() override final;

private:
    bool LoadDictionary(const std::string& filename);
    bool TransmitCharacter(char character);
    bool TransmitMorseCodeState(EMorseCodeState state);

private:
    using MorseLetter = std::vector<EMorseCodeState>;

    std::unordered_map<char, const MorseLetter> m_MorseDictionary;
    unsigned int m_DotDurationInMiliSeconds = 500u;

    TransmissionFinishedCallback m_Callback;

    static const auto s_DashDurationToDotMultiplier = 3u;
    static const auto s_DotCharacter = '.';
    static const auto s_DashCharacter = '-';
};

#endif //__MORSE_CODE_TRANSMISSION_SERVICE_H__