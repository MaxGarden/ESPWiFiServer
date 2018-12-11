#pragma once
#if !defined(__VIEW_H__)
#define __VIEW_H__

#include "DataModel.h"
#include "Controller.h"

class IView
{
public:
    virtual ~IView() = default;

    virtual bool SetController(const IControllerSharedPtr& controller) = 0;
};

class IViewFactory
{
public:
    virtual ~IViewFactory() = default;
    virtual std::unique_ptr<IView> Create() = 0;
};

#endif //__VIEW_H__