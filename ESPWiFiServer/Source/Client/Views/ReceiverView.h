#if !defined(__RECEIVER_VIEW__)
#define __RECEIVER_VIEW__
#pragma once

#include "ClientViewBase.h"
#include "Client/Services/MorseCodeReceiverService.h"
#include "ui_ReceiverView.h"

class CReceiverView : public CClientViewBase, public Ui::ReceiverView
{
    Q_OBJECT

public:
    CReceiverView(QWidget* parent = nullptr);
    virtual ~CReceiverView() override final = default;

    const std::string& GetName() const noexcept override final;

protected:
    virtual void OnServicePaired(IClientService& service) override final;
    virtual void OnServiceUnpaired(IClientService& service) override final;

private:
    void RefreshView();

private:
    CMorseCodeReceiverService* m_ReceiverService = nullptr;
};

#endif //__RECEIVER_VIEW__