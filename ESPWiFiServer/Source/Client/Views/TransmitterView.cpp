#include "pch.h"
#include "TransmitterView.h"
#include <QMessageBox>
#include <sstream>

CTransmitterView::CTransmitterView(QWidget* parent /*= nullptr*/) :
    CClientViewBase(parent)
{
    setupUi(this);
}

const std::string& CTransmitterView::GetName() const noexcept
{
    static const std::string name = "Transmitter";
    return name;
}

void CTransmitterView::OnServicePaired(IClientService& service)
{
    if (const auto transmissionService = dynamic_cast<CMorseCodeTransmissionService*>(&service))
    {
        DEBUG_ASSERT(!m_TransmissionService);
        m_TransmissionService = transmissionService;
        SetupTransmissionMediumComboBox();
    }

    RefreshView();
}

void CTransmitterView::OnServiceUnpaired(IClientService& service)
{
    if (m_TransmissionService == &service)
        m_TransmissionService = nullptr;

    RefreshView();
}

void CTransmitterView::OnTransmitButtonClicked()
{
    DEBUG_ASSERT(m_TransmissionService);
    if (!m_TransmissionService)
        return RefreshView();

    const auto isTransmitting = m_TransmissionService->IsTransmitting();
    DEBUG_ASSERT(!isTransmitting);
    if (isTransmitting)
        return;

    auto textToTransmit = [this]()
    {
        if (m_TextToTransmitTextEdit)
            return m_TextToTransmitTextEdit->toPlainText().toUtf8().toStdString();

        return std::string{};
    }();

    if (textToTransmit.empty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot transmit empty text!"));
        return;
    }

    if (textToTransmit.size() > 1000) //TODO fix this
    {
        QMessageBox::critical(this, tr("Error"), tr("Maximum text length is 1000"));
        return;
    }

    const auto notAllowedCharacters = [this, &textToTransmit]
    {
        std::set<char> result;

        for (auto character : textToTransmit)
        {
            if (!m_TransmissionService->CheckCharacter(character))
                result.emplace(character);
        }
        return result;
    }();

    if (!notAllowedCharacters.empty())
    {
        std::ostringstream stream;
        std::copy(notAllowedCharacters.cbegin(), notAllowedCharacters.cend(), std::ostream_iterator<char>(stream, ","));
        QMessageBox::critical(this, tr("Error"), QString{ "Text contains not allowed characters: %1" }.arg(QString::fromStdString(stream.str())));
        return;
    }

    const auto result = m_TransmissionService->TransmitText(std::move(textToTransmit), [this](auto success)
    {
        if (success)
            QMessageBox::information(this, tr("Information"), tr("Transmission done."));
        else
            QMessageBox::warning(this, tr("Warning"), tr("Transmission was aborted."));

        if (m_AbortPushButton)
            m_AbortPushButton->setEnabled(false);

        RefreshView();
    });

    DEBUG_ASSERT(result);
    if (!result)
    {
        QMessageBox::critical(this, tr("Error"), tr("Transmission failed!"));
        return RefreshView();
    }

    if (m_TransmissionGroupBox)
        m_TransmissionGroupBox->setEnabled(false);

    if (m_AbortPushButton)
        m_AbortPushButton->setEnabled(true);
}

void CTransmitterView::OnAbortButtonClicked()
{
    DEBUG_ASSERT(m_TransmissionService);
    if (!m_TransmissionService)
        return RefreshView();

    const auto isTransmitting = m_TransmissionService->IsTransmitting();
    DEBUG_ASSERT(isTransmitting);
    if (!isTransmitting)
        return;

    const auto clearResult = m_TransmissionService->ClearQueue();
    DEBUG_ASSERT(clearResult);
}

void CTransmitterView::OnDotDurationChanged(int durationInMiliseconds)
{
    DEBUG_ASSERT(m_TransmissionService);
    if (!m_TransmissionService)
        return RefreshView();

    if (!m_TransmissionService->SetDotDuration(durationInMiliseconds))
    {
        QMessageBox::critical(this, tr("Error"), tr("Invalid dot duration value!"));
        return RefreshView();
    }
}

void CTransmitterView::OnTransmissionMediumChanged(int index)
{
    DEBUG_ASSERT(m_TransmissionService);
    if (!m_TransmissionService)
        return RefreshView();

    const auto result = m_TransmissionService->SetTransmissionMedium(static_cast<TransmissionMedium>(index));
    DEBUG_ASSERT(result);
    if (!result && m_TransmissionMediumComboBox)
        m_TransmissionMediumComboBox->setCurrentIndex(static_cast<int>(m_TransmissionService->GetTransmissionMedium()));
}

void CTransmitterView::RefreshView()
{
    if (m_TransmissionGroupBox)
        m_TransmissionGroupBox->setEnabled(m_TransmissionService != nullptr);

    if (m_DotDurationSpinBox && m_TransmissionService)
        m_DotDurationSpinBox->setValue(m_TransmissionService->GetDotDuration());
}

void CTransmitterView::SetupTransmissionMediumComboBox()
{
    if (!m_TransmissionMediumComboBox)
        return;

    m_TransmissionMediumComboBox->clear();
    m_TransmissionMediumComboBox->addItems({ tr("Sound"), tr("Infrared") });
}