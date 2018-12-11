template<typename SuperClass, typename ListenerType>
std::optional<ListenerHandle> CListenableBase<SuperClass, ListenerType>::RegisterListener(IListenerUniquePtr&& listener)
{
    const auto curveEditorListener = dynamic_cast<ListenerType*>(listener.get());
    if (!curveEditorListener)
        return std::nullopt;

    m_Listeners.emplace_back(std::unique_ptr<ListenerType>(curveEditorListener));
    return reinterpret_cast<ListenerHandle>(listener.release());
}

template<typename SuperClass, typename ListenerType>
bool CListenableBase<SuperClass, ListenerType>::UnregisterListener(const ListenerHandle& handle)
{
    const auto iterator = std::find_if(m_Listeners.begin(), m_Listeners.end(), [&handle](const auto& listener)
    {
        return reinterpret_cast<ListenerHandle>(listener.get()) == handle;
    });

    if (iterator == m_Listeners.end())
        return false;

    iterator->reset();
    return true;
}

template<typename SuperClass, typename ListenerType>
template<typename ListenerMethod, typename... Arguments>
void CListenableBase<SuperClass, ListenerType>::NotifyListeners(ListenerMethod method, Arguments&&... arguments)
{
    for (const auto& listener : m_Listeners)
    {
        if (listener)
            (listener.get()->*method)(std::forward<Arguments>(arguments)...);
    }

    RemoveFromContainer(m_Listeners, nullptr);
}