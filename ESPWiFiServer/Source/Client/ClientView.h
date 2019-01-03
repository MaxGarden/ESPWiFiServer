#if !defined(__CLIENT_VIEW_H__)
#define __CLIENT_VIEW_H__
#pragma once

#include "View.h"
#include <QWidget>

class IClientView : public IView, public virtual QWidget
{
public:
    virtual ~IClientView() override = default;

    virtual const std::string& GetName() const noexcept = 0;
};

class IClientViewFactory
{
public:
    virtual ~IClientViewFactory() = default;

    virtual IClientViewUniquePtr Create(QWidget* parent = nullptr) = 0;
};

#endif //__CLIENT_VIEW_H__