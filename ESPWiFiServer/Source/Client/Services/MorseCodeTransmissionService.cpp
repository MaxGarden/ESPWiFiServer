#include "pch.h"
#include "MorseCodeTransmissionService.h"
#include <sstream>
#include <fstream>
#include <windows.h>

bool CMorseCodeTransmissionService::Initialize()
{
    if (!CBinaryTransmissionService::Initialize())
        return false;

    return LoadDictionary("Data/Services/MorseCodeDictionary.txt");
}

void CMorseCodeTransmissionService::Finalize()
{
    if (IsTransmitting())
        OnFinishedTransmitting();

    CBinaryTransmissionService::Finalize();
}

bool CMorseCodeTransmissionService::CheckCharacter(char character) const noexcept
{
    return m_MorseDictionary.find(character) != m_MorseDictionary.cend();
}

bool CMorseCodeTransmissionService::CheckText(const std::string& text) const noexcept
{
    for (auto character : text)
    {
        if (!CheckCharacter(character))
            return false;
    }

    return true;
}

bool CMorseCodeTransmissionService::TransmitText(const std::string& text, TransmissionFinishedCallback&& callback)
{
    if (IsTransmitting() || m_Callback || !CheckText(text))
    {
        DEBUG_ASSERT(false);
        return false;
    }

    m_Callback = callback;

    for (const auto character : text)
    {
        if (!TransmitCharacter(character))
        {
            DEBUG_ASSERT(false);
            return false;
        }
    }

    return true;
}

void CMorseCodeTransmissionService::SetDotDuration(unsigned int durationInMiliseconds) noexcept
{
    m_DotDurationInMiliSeconds = durationInMiliseconds;
}

unsigned int CMorseCodeTransmissionService::GetDotDuration() const noexcept
{
    return m_DotDurationInMiliSeconds;
}

void CMorseCodeTransmissionService::OnFinishedTransmitting()
{
    DEBUG_ASSERT(m_Callback);
    if (!m_Callback)
        return;

    TransmissionFinishedCallback callback;
    m_Callback.swap(callback);
    callback();
}

bool CMorseCodeTransmissionService::LoadDictionary(const std::string& filename)
{
    std::ifstream file{ filename };
    if (!file.good())
    {
        DEBUG_ASSERT(false);
        return false;
    }

    static const auto getMorseState = [](auto character) -> std::optional<EMorseCodeState>
    {
        switch (character)
        {
        case s_DotCharacter:
            return EMorseCodeState::Dot;
        case s_DashCharacter:
            return EMorseCodeState::Dash;
        default:
            DEBUG_ASSERT(false);
            return std::nullopt;
        }
    };

    const auto addMorseLetter = [this](auto character, const auto& morseState)
    {
        if (CheckCharacter(character))
        {
            DEBUG_ASSERT(false);
            return false;
        }

        MorseLetter morseLetter;

        for (auto currentCharacter : morseState)
        {
            const auto state = getMorseState(currentCharacter);
            if (!state)
            {
                DEBUG_ASSERT(false);
                return false;
            }

            morseLetter.emplace_back(*state);
            morseLetter.emplace_back(EMorseCodeState::Space);
        }

        m_MorseDictionary.emplace(character, std::move(morseLetter));
        return true;
    };

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream stream{ line };
        char character;
        std::string morseState;

        if (!(stream >> character >> morseState))
        {
            DEBUG_ASSERT(false);
            return false;
        }

        if (!addMorseLetter(character, morseState))
            return false;
    }

    return true;
}

bool CMorseCodeTransmissionService::TransmitCharacter(char character)
{
    if (!CheckCharacter(character))
    {
        DEBUG_ASSERT(false);
        return false;
    }

    const auto& morseLetter = m_MorseDictionary.at(character);

    for (const auto state : morseLetter)
    {
        if (!TransmitMorseCodeState(state))
        {
            DEBUG_ASSERT(false);
            return false;
        }
    }

    return true;
}

bool CMorseCodeTransmissionService::TransmitMorseCodeState(EMorseCodeState state)
{
    switch (state)
    {
    case EMorseCodeState::Dot:
        return TransmitHighState(m_DotDurationInMiliSeconds);
    case EMorseCodeState::Dash:
        return TransmitHighState(s_DashDurationToDotMultiplier * m_DotDurationInMiliSeconds);
    case EMorseCodeState::Space:
        return TransmitLowState(m_DotDurationInMiliSeconds);
    default:
        DEBUG_ASSERT(false);
        return false;
    }
}