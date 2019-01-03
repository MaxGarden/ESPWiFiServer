template<typename SuperClass, typename ControllerType>
void CViewBase<SuperClass, ControllerType>::OnControllerChanged()
{
    //to override
}

template<typename SuperClass, typename ControllerType>
IListenerUniquePtr CViewBase<SuperClass, ControllerType>::CreateListener()
{
    //to override
    return nullptr;
}

template<typename SuperClass, typename ControllerType>
const std::shared_ptr<ControllerType>& CViewBase<SuperClass, ControllerType>::GetController() const noexcept
{
    return m_Controller;
}

template<typename SuperClass, typename ControllerType>
bool CViewBase<SuperClass, ControllerType>::SetController(const IControllerSharedPtr& controller)
{
    const auto resetListener = [this]()
    {
        if (m_Controller)
            m_Controller->UnregisterListener(m_ListenerHandle);

        m_ListenerHandle = InvalidListenerHandle;
    };

    if (!controller)
    {
        resetListener();
        m_Controller.reset();
        OnControllerChanged();
        return true;
    }

    const auto castedController = std::dynamic_pointer_cast<ControllerType>(controller);
    if (!castedController)
        return false;

    resetListener();
    m_Controller = std::move(castedController);

    if (auto listener = CreateListener())
    {
        const auto listenerHandle = m_Controller->RegisterListener(std::move(listener));
        DEBUG_ASSERT(listenerHandle);
        m_ListenerHandle = listenerHandle.value_or(m_ListenerHandle);
    }

    OnControllerChanged();

    return true;
}