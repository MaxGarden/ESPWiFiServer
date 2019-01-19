#include "pch.h"
#include "ReceiverView.h"

CReceiverView::CReceiverView(QWidget* parent /*= nullptr*/) :
    CClientViewBase(parent)
{
    setupUi(this);
}

const std::string& CReceiverView::GetName() const noexcept
{
    static const std::string name = "Receiver";
    return name;
}

void CReceiverView::OnServicePaired(IClientService& service)
{
    if (const auto receiverService = dynamic_cast<CMorseCodeReceiverService*>(&service))
    {
        DEBUG_ASSERT(!m_ReceiverService);
        m_ReceiverService = receiverService;
        m_ReceiverService->StartReceiving(200, 1000000, 100, [](auto x)
        {

        });
    }

    RefreshView();
}

void CReceiverView::OnServiceUnpaired(IClientService& service)
{
    if (m_ReceiverService == &service)
        m_ReceiverService = nullptr;

    RefreshView();
}

void CReceiverView::RefreshView()
{
    if (m_ReceiverGroupBox)
        m_ReceiverGroupBox->setEnabled(m_ReceiverService != nullptr);
}