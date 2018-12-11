#if !defined(__UTILITIES_H__)
#define __UTILITIES_H__
#pragma once

template<typename Type>
using VisitorType = std::function<void(Type&)>;

template<typename Type>
using ConstVisitorType = std::function<void(const Type&)>;

template<typename Type>
using InterruptibleVisitorType = std::function<bool(Type&)>; //continues visiting only if visitor returns true

template<typename Type>
using ConstInterruptibleVisitorType = std::function<bool(const Type&)>;

template<typename ContainerType, typename InterruptibleVisitorType>
inline void VisitPointersContainerInterruptible(ContainerType& container, InterruptibleVisitorType& visitor, bool reverse = false) noexcept
{
    if (reverse)
    {
        for (auto iterator = container.rbegin(); iterator != container.rend(); ++iterator)
        {
            if (auto& element = *iterator)
            {
                if (!visitor(*element))
                    return;
            }
        }
    }
    else
    {
        for (auto& element : container)
        {
            if (element)
            {
                if (!visitor(*element))
                    return;
            }
        }
    }
}

template<typename ContainerType, typename VisitorType>
inline void VisitPointersContainer(ContainerType& container, VisitorType& visitor, bool reverse = false) noexcept
{
    const auto proxyVisitor = [&visitor](auto& element)
    {
        visitor(element);
        return true;
    };

    VisitPointersContainerInterruptible(container, proxyVisitor, reverse);
}

template<typename ContainerType, typename InterruptibleVisitorType>
inline void VisitObjectsContainerInterruptible(ContainerType& container, InterruptibleVisitorType& visitor, bool reverse = false) noexcept
{
    if (reverse)
    {
        for (auto iterator = container.rbegin(); iterator != container.rend(); ++iterator)
        {
            if (!visitor(*iterator))
                return;
        }
    }
    else
    {
        for (auto& element : container)
        {
            if (!visitor(element))
                return;
        }
    }
}

template<typename ContainerType, typename VisitorType>
inline void VisitObjectsContainer(ContainerType& container, VisitorType& visitor, bool reverse = false) noexcept
{
    const auto proxyVisitor = [&visitor](auto& element)
    {
        visitor(element);
        return true;
    };

    VisitObjectsContainerInterruptible(container, proxyVisitor, reverse);
}

template<typename ContainerType, typename InterruptibleVisitorType>
inline void VisitPointersMapInterruptible(ContainerType& container, InterruptibleVisitorType& visitor, bool reverse = false) noexcept
{
    if (reverse)
    {
        for (auto iterator = container.rbegin(); iterator != container.rend(); ++iterator)
        {
            if (const auto& value = iterator->second)
            {
                if (!visitor(*value))
                    return;
            }
        }
    }
    else
    {
        for (auto& pair : container)
        {
            if (auto& value = pair.second)
                visitor(*value);
        }
    }
}

template<typename ContainerType, typename VisitorType>
inline void VisitPointersMap(ContainerType& container, VisitorType& visitor, bool reverse = false) noexcept
{
    const auto proxyVisitor = [&visitor](auto& element)
    {
        visitor(element);
        return true;
    };

    VisitPointersMapInterruptible(container, proxyVisitor, reverse);
}

template<typename ContainerType, typename InterruptibleVisitorType>
inline void VisitObjectsMapInterruptible(ContainerType& container, InterruptibleVisitorType& visitor, bool reverse = false) noexcept
{
    if (reverse)
    {
        for (auto iterator = container.rbegin(); iterator != container.rend(); ++iterator)
        {
            if (!visitor(iterator->second))
                return;
        }
    }
    else
    {
        for (auto& pair : container)
            visitor(pair.second);
    }
}

template<typename ContainerType, typename VisitorType>
inline void VisitObjectsMap(ContainerType& container, VisitorType& visitor, bool reverse = false) noexcept
{
    const auto proxyVisitor = [&visitor](auto& element)
    {
        visitor(element);
        return true;
    };

    VisitObjectsMapInterruptible(container, proxyVisitor, reverse);
}

template<typename ContainerType, typename ValueType>
inline const auto RemoveFromContainer(ContainerType& container, const ValueType& value)
{
    const auto iterator = std::remove(container.begin(), container.end(), value);

    if (iterator == container.end())
        return false;

    container.erase(iterator, container.end());
    return true;
}

template<typename KeyType, typename ValueType>
inline const auto RemoveFromContainer(std::map<KeyType, ValueType>& map, const KeyType& key)
{
    const auto iterator = map.find(key);

    if (iterator == map.end())
        return false;

    map.erase(iterator);
    return true;
}

template<typename ViewComponentType, typename EditorViewType>
inline const auto GetViewComponent(EditorViewType& editorView)
{
    std::shared_ptr<ViewComponentType> result;

    editorView.VisitViewComponents([&result](const auto& component)
    {
        return (result = std::dynamic_pointer_cast<ViewComponentType>(component)) == nullptr;
    });

    return result;
}

class CScopedGuard
{
public:
    CScopedGuard(std::function<void()>&& callback) noexcept :
        m_Callback(std::move(callback)){}

    ~CScopedGuard()
    {
        if (m_Callback)
            m_Callback();
    }

private:
    const std::function<void()> m_Callback;
};

#endif //__UTILITES_H__