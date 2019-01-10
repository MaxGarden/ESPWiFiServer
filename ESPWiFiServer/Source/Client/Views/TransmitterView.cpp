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

    const auto textToTransmit = [this]()
    {
        if (m_TextToTransmitTextEdit)
            return m_TextToTransmitTextEdit->toPlainText().toUtf8().toStdString();

        return std::string{};
    }();

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

    if(m_TransmissionGroupBox)
        m_TransmissionGroupBox->setEnabled(false);

    if (m_AbortPushButton)
        m_AbortPushButton->setEnabled(true);

    m_TransmissionService->TransmitText(textToTransmit, [this]()
    {
        QMessageBox::information(this, tr("Information"), tr("Transmission done."));

        if (m_TransmissionGroupBox)
            m_TransmissionGroupBox->setEnabled(true);

        if (m_AbortPushButton)
            m_AbortPushButton->setEnabled(false);
    });
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
    DEBUG_ASSERT(durationInMiliseconds > 0);
    if (durationInMiliseconds <= 0)
        return;

    DEBUG_ASSERT(m_TransmissionService);
    if (!m_TransmissionService)
        return RefreshView();

    m_TransmissionService->SetDotDuration(durationInMiliseconds);
}

void CTransmitterView::RefreshView()
{
    if (m_TransmissionGroupBox)
        m_TransmissionGroupBox->setEnabled(m_TransmissionService != nullptr);

    if (m_DotDurationSpinBox && m_TransmissionService)
        m_DotDurationSpinBox->setValue(m_TransmissionService->GetDotDuration());
}