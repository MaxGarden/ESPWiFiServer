#if !defined(__VIEW_BASE_H__)
#define __VIEW_BASE_H__
#pragma once

#include "View.h"

template<typename SuperClass, typename ControllerType>
class CEditorViewBase : public SuperClass
{
public:
    CEditorViewBase() = default;
    virtual ~CEditorViewBase() override = default;

    virtual bool SetController(const IEditorControllerSharedPtr& controller) override;

protected:
    virtual void OnControllerChanged();
    virtual IEditorListenerUniquePtr CreateListener();

    const std::shared_ptr<ControllerType>& GetController() const noexcept;

    using Super = CEditorViewBase<SuperClass, ControllerType>;

private:
    std::shared_ptr<ControllerType> m_Controller;
    ListenerHandle m_ListenerHandle;
};

#include "ViewBase.inl"

#endif //__CURVE_EDITOR_VIEW_BASE_H__