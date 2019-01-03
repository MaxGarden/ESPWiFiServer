#if !defined(__CLIENT_PAIR_VIEW_H__)
#define __CLIENT_PAIR_VIEW_H__
#pragma once

#include "ViewBase.h"
#include "ClientView.h"
#include "ClientController.h"
#include "Client/Services/DeviceIdService.h"

class IClientPairView : public IClientView
{
public:
    using ResolveDeviceTypeCallback = std::function<void(const std::optional<byte>&)>;

public:
    virtual ~IClientPairView() override = default;

    virtual bool ResolveDeviceType(ResolveDeviceTypeCallback&& callback) = 0;

    static IClientPairViewUniquePtr Create(const IClientBuildersProvider& buildersProvider, QWidget* parent = nullptr);
};

#endif //__CLIENT_PAIR_VIEW_H__