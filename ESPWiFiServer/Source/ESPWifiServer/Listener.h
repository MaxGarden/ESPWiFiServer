#if !defined(__LISTENER_H__)
#define __LISTENER_H__
#pragma once

using ListenerHandle = size_t;
const ListenerHandle InvalidListenerHandle = 0;

class IListener
{
public:
    virtual ~IListener() = default;
};

class IListenable
{
public:
    virtual ~IListenable() = default;

    virtual std::optional<ListenerHandle> RegisterListener(IListenerUniquePtr&& listener) = 0;
    virtual bool UnregisterListener(const ListenerHandle& handle) = 0;
};

#endif //__LISTENER_H__