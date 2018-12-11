#if !defined(__CONTROLLER_H__)
#define __CONTROLLER_H__
#pragma once

#include "Listener.h"

class IController : public IListenable
{
public:
    virtual ~IController() = default;

    virtual bool SetDataModel(const IDataModelSharedPtr& dataModel) = 0;
};

#endif //__CONTROLLER_H__