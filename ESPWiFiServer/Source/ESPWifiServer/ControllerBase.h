#if !defined(__CONTROLLER_BASE_H__)
#define __CONTROLLER_BASE_H__
#pragma once

#include "ListenableBase.h"

template<typename SuperClass, typename DataModelType, typename ListenerType>
class CControllerBase : public CListenableBase<SuperClass, ListenerType>
{
public:
    CControllerBase() = default;
    virtual ~CControllerBase() override;

    virtual bool SetDataModel(const IDataModelSharedPtr& dataModel) override;

protected:
    virtual void OnDataModelChanged();
    virtual IListenerUniquePtr CreateListener();

    const std::shared_ptr<DataModelType>& GetDataModel() const noexcept;

    using Super = CControllerBase<SuperClass, DataModelType, ListenerType>;

private:
    std::shared_ptr<DataModelType> m_DataModel;
    ListenerHandle m_ListenerHandle;
};

#include "ControllerBase.inl"

#endif //__CONTROLLER_BASE_H__