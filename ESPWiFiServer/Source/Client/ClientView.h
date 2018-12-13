#if !defined(__CLIENT_VIEW_H__)
#define __CLIENT_VIEW_H__
#pragma once

#include "View.h"

class IClientView : public IView
{
public:
    virtual ~IClientView() override = default;
};

#endif //__CLIENT_VIEW_H__