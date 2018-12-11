#if !defined(__DATA_MODEL_H__)
#define __DATA_MODEL_H__
#pragma once

#include "Listener.h"

class IDataModel : public IListenable
{
public:
    virtual ~IDataModel() = default;
};

#endif //__DATA_MODEL_H__