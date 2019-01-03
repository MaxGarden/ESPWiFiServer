#if !defined(__TRANSMITTER_VIEW__)
#define __TRANSMITTER_VIEW__
#pragma once

#include "ClientViewBase.h"
#include "Client/Services/TransmissionService.h"

class CTransmitterView : public CClientViewBase
{
    Q_OBJECT;

public:
    CTransmitterView(QWidget* parent = nullptr);
    virtual ~CTransmitterView() override final = default;

    const std::string& GetName() const noexcept override final;

protected:
    virtual void OnServicePaired(IClientService& service) override final;
    virtual void OnServiceUnpaired(IClientService& service) override final;

private:
    CTransmissionService* m_TransmissionService = nullptr;
};

#endif //__TRANSMITTER_VIEW__