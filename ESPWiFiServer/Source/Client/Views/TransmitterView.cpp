#include "pch.h"
#include "TransmitterView.h"

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
    if (const auto transmissionService = dynamic_cast<CTransmissionService*>(&service))
    {
        DEBUG_ASSERT(!m_TransmissionService);
        m_TransmissionService = transmissionService;
    }
}

void CTransmitterView::OnServiceUnpaired(IClientService& service)
{
    if (m_TransmissionService == &service)
        m_TransmissionService = nullptr;
}

void CTransmitterView::OnTransmitButtonClicked()
{
    if (m_TransmissionService)
        m_TransmissionService->TransmitHighState(500);
}