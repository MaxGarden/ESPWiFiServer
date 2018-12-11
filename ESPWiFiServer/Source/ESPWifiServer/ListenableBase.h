#if !defined(__LISTENABLE_BASE_H__)
#define __LISTENABLE_BASE_H__
#pragma once

#include "Listener.h"

template<typename SuperClass, typename ListenerType>
class CListenableBase : public SuperClass
{
public:
    CListenableBase() = default;
    virtual ~CListenableBase() override = default;

    virtual std::optional<ListenerHandle> RegisterListener(IListenerUniquePtr&& listener) override;
    virtual bool UnregisterListener(const ListenerHandle& handle) override;

protected:
    template<typename ListenerMethod, typename... Arguments>
    void NotifyListeners(ListenerMethod method, Arguments&&... arguments);

private:
    std::vector<std::unique_ptr<ListenerType>> m_Listeners;
};

#include "ListenableBase.inl"

#endif //__LISTENABLE_BASE_H__