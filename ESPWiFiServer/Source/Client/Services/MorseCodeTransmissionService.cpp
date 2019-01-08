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
        OnFinishedCommand(true);

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

    ++m_TransmissionCounter;

    for (const auto character : text)
    {
        if (!TransmitCharacter(character))
        {
            DEBUG_ASSERT(false);
            return false;
        }
    }

    OnFinishedCommand(false);

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

bool CMorseCodeTransmissionService::IsTransmitting() const noexcept
{
    return m_TransmissionCounter > 0;
}

void CMorseCodeTransmissionService::OnFinishedCommand(bool clear)
{
    const auto safeInvokeCallback = [this](bool wholeTransmission)
    {
        DEBUG_ASSERT(m_Callback);
        if (!m_Callback)
            return;

        TransmissionFinishedCallback callback;
        m_Callback.swap(callback);
        callback(wholeTransmission);
    };

    if (clear)
    {
        m_TransmissionCounter = 0;
        safeInvokeCallback(false);
        return;
    }

    DEBUG_ASSERT(m_TransmissionCounter > 0);
    if (m_TransmissionCounter <= 0)
        return;

    if (--m_TransmissionCounter == 0)
        safeInvokeCallback(true);
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
    ++m_TransmissionCounter;
    switch (state)
    {
    case EMorseCodeState::Dot:
        return TransmitHighState(m_DotDurationInMiliSeconds);
    case EMorseCodeState::Dash:
        return TransmitHighState(s_DashDurationToDotMultiplier * m_DotDurationInMiliSeconds);
    case EMorseCodeState::Space:
        return TransmitLowState(m_DotDurationInMiliSeconds);
    default:
        --m_TransmissionCounter;
        DEBUG_ASSERT(false);
        return false;
    }
}