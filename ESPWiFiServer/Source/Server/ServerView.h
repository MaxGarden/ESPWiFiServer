#if !defined(__SERVER_VIEW_H__)
#define __SERVER_VIEW_H__
#pragma once

#include "View.h"

class IServerView : public IView
{
public:
    virtual ~IServerView() override = default;
};

#endif //__SERVER_VIEW_H__