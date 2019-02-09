#include "pch.h"
#include "MorseCodeReceiverService.h"
#include <sstream>
#include <fstream>

const double CMorseCodeReceiverService::s_UncertaintyFactor = 0.5;

bool CMorseCodeReceiverService::Initialize()
{
    if (!CSamplesToBinaryReceiverService::Initialize())
        return false;

    return LoadDictionary("Data/Services/MorseCodeDictionary.txt");
}

bool CMorseCodeReceiverService::StartReceiving(unsigned short int samplesFrequency, unsigned short int sendFrequency, SampleType binaryTreshold, unsigned short int binarySamplingFrequency, unsigned short int dotDurationInMiliseconds, ReceiveCallback&& morseCallback, CSamplesToBinaryReceiverService::ReceiveCallback&& binaryCallback, CSamplesReceiverService::ReceiveCallback&& samplesCallback)
{
    if (m_ReceiveCallback || !morseCallback)
    {
        DEBUG_ASSERT(false);
        return false;
    }

    const auto result = CSamplesToBinaryReceiverService::StartReceiving(samplesFrequency, sendFrequency, binaryTreshold, binarySamplingFrequency, [this, binaryCallback = std::move(binaryCallback)](auto&& states)
    {
        OnReceivedStates(states);

        if (binaryCallback)
            binaryCallback(std::move(states));
    }, std::move(samplesCallback));

    if (!result)
        return false;

    m_DotDurationInMiliseconds = dotDurationInMiliseconds;
    m_ReceiveCallback = std::move(morseCallback);
    m_ErrorInCharacterTransmission = false;
    m_CurrentLetter.clear();

    return true;
}

bool CMorseCodeReceiverService::EndReceiving()
{
    DEBUG_ASSERT(m_ReceiveCallback);
    if (!m_ReceiveCallback)
        return false;

    if (!CSamplesToBinaryReceiverService::EndReceiving())
        return false;

    m_ReceiveCallback(std::nullopt);
    m_ReceiveCallback = ReceiveCallback{};
    return true;
}

bool CMorseCodeReceiverService::LoadDictionary(const std::string& filename)
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
        }

        m_MorseDictionary.emplace(std::move(morseLetter), character);
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

void CMorseCodeReceiverService::OnReceivedStates(const std::vector<StateType>& states)
{
    DEBUG_ASSERT(m_ReceiveCallback);
    if (!m_ReceiveCallback)
        return;

    for (const auto& state : states)
    {
        const auto morseCodeState = DecodeMorseCodeState(state.first, state.second * 1000);
        if (!morseCodeState)
            m_ErrorInCharacterTransmission = true;
        else if (*morseCodeState == EMorseCodeState::CharacterSpace)
        {
            CScopedGuard guard{ [this]()
            {
                m_ErrorInCharacterTransmission = false;
                m_CurrentLetter.clear();
            } };

            if (!m_ErrorInCharacterTransmission)
            {
                const auto iterator = m_MorseDictionary.find(m_CurrentLetter);
                if (iterator == m_MorseDictionary.cend())
                    return;

                m_ReceiveCallback(iterator->second);
            }
        }
        else if (*morseCodeState == EMorseCodeState::Dot || *morseCodeState == EMorseCodeState::Dash)
            m_CurrentLetter.emplace_back(*morseCodeState);
    }
}

std::optional<EMorseCodeState> CMorseCodeReceiverService::DecodeMorseCodeState(bool state, unsigned short int stateDuration)
{
    const auto checkIfInRange = [stateDuration](auto expectedDuration)
    {
        return expectedDuration >= (1.0 - s_UncertaintyFactor) * stateDuration &&
            expectedDuration <= (1.0 + s_UncertaintyFactor) * stateDuration;
    };

    if (checkIfInRange(m_DotDurationInMiliseconds))
        return state ? EMorseCodeState::Dot : EMorseCodeState::StateSpace;
    else if (checkIfInRange(m_DotDurationInMiliseconds * s_DashDurationToDotMultiplier))
        return state ? EMorseCodeState::Dash : EMorseCodeState::CharacterSpace;

    return std::nullopt;
}