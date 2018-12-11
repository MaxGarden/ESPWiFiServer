template<typename SuperClass, typename DataModelType, typename ListenerType>
CControllerBase<SuperClass, DataModelType, ListenerType>::~CControllerBase()
{
    SetDataModel(nullptr);
}

template<typename SuperClass, typename DataModelType, typename ListenerType>
bool CControllerBase<SuperClass, DataModelType, ListenerType>::SetDataModel(const IDataModelSharedPtr& dataModel)
{
    const auto resetListener = [this]()
    {
        if (m_DataModel)
            m_DataModel->UnregisterListener(m_ListenerHandle);

        m_ListenerHandle = InvalidListenerHandle;
    };

    if (!dataModel)
    {
        resetListener();
        m_DataModel.reset();
        OnDataModelChanged();
        return true;
    }

    const auto curveEditorDataModel = std::dynamic_pointer_cast<DataModelType>(dataModel);
    if (!curveEditorDataModel)
        return false;

    resetListener();
    m_DataModel = std::move(curveEditorDataModel);

    if (auto listener = CreateListener())
    {
        const auto listenerHandle = m_DataModel->RegisterListener(std::move(listener));
        DEBUG_ASSERT(listenerHandle);
        m_ListenerHandle = listenerHandle.value_or(m_ListenerHandle);
    }

    OnDataModelChanged();

    return true;
}

template<typename SuperClass, typename DataModelType, typename ListenerType>
void CControllerBase<SuperClass, DataModelType, ListenerType>::OnDataModelChanged()
{
    //to override
}

template<typename SuperClass, typename DataModelType, typename ListenerType>
IListenerUniquePtr CControllerBase<SuperClass, DataModelType, ListenerType>::CreateListener()
{
    //to override
    return nullptr;
}

template<typename SuperClass, typename DataModelType, typename ListenerType>
const std::shared_ptr<DataModelType>& CControllerBase<SuperClass, DataModelType, ListenerType>::GetDataModel() const noexcept
{
    return m_DataModel;
}