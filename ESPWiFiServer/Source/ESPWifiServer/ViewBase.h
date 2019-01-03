#if !defined(__VIEW_BASE_H__)
#define __VIEW_BASE_H__
#pragma once

#include "View.h"

template<typename SuperClass, typename ControllerType>
class CViewBase : public SuperClass
{
public:
    CViewBase() = default;
    virtual ~CViewBase() override = default;

    virtual bool SetController(const IControllerSharedPtr& controller) override;

protected:
    virtual void OnControllerChanged();
    virtual IListenerUniquePtr CreateListener();

    const std::shared_ptr<ControllerType>& GetController() const noexcept;

    using Super = CViewBase<SuperClass, ControllerType>;

private:
    std::shared_ptr<ControllerType> m_Controller;
    ListenerHandle m_ListenerHandle;
};

#include "ViewBase.inl"

#endif //__VIEW_BASE_H__