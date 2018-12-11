template<typename SuperClass, typename ControllerType>
void CEditorViewBase<SuperClass, ControllerType>::OnControllerChanged()
{
    //to override
}

template<typename SuperClass, typename ControllerType>
IEditorListenerUniquePtr CEditorViewBase<SuperClass, ControllerType>::CreateListener()
{
    //to override
    return nullptr;
}

template<typename SuperClass, typename ControllerType>
const std::shared_ptr<ControllerType>& CEditorViewBase<SuperClass, ControllerType>::GetController() const noexcept
{
    return m_Controller;
}

template<typename SuperClass, typename ControllerType>
bool CEditorViewBase<SuperClass, ControllerType>::SetController(const IEditorControllerSharedPtr& controller)
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

    const auto curveEditorController = std::dynamic_pointer_cast<ControllerType>(controller);
    if (!curveEditorController)
        return false;

    resetListener();
    m_Controller = std::move(curveEditorController);

    if (auto listener = CreateListener())
    {
        const auto listenerHandle = m_Controller->RegisterListener(std::move(listener));
        EDITOR_ASSERT(listenerHandle);
        m_ListenerHandle = listenerHandle.value_or(m_ListenerHandle);
    }

    OnControllerChanged();

    return true;
}