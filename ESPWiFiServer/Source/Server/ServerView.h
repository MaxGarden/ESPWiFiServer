#if !defined(__SERVER_VIEW_H__)
#define __SERVER_VIEW_H__
#pragma once

#include "View.h"
#include <QWidget>

class IServerView : public IView, public virtual QWidget
{
public:
    virtual ~IServerView() override = default;

    virtual bool RegisterClientViewFactory(byte deviceId, IClientViewFactoryUniquePtr&& factory) = 0;
    virtual bool UnregisterClientViewFactory(byte deviceId) = 0;

    static IServerViewUniquePtr Create(const IClientBuildersProvider& buildersProvider, QWidget* parent = nullptr);
};

#endif //__SERVER_VIEW_H__