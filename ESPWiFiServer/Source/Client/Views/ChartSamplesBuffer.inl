template<typename SampleType>
CChartSamplesBuffer<SampleType>::CChartSamplesBuffer(QXYSeries* series, size_t samplesCount) :
    m_Series{ series },
    m_SamplesCount{ samplesCount }
{
    DEBUG_ASSERT(m_Series);
}

template<typename SampleType>
void CChartSamplesBuffer<SampleType>::Write(const std::vector<SampleType>& samples)
{
    DEBUG_ASSERT(m_Series);
    if (!m_Series)
        return;

    if (m_Buffer.isEmpty())
    {
        m_Buffer.reserve(m_SamplesCount);
        for (auto i = 0u; i < m_SamplesCount; ++i)
            m_Buffer.append(QPointF(i, 0));
    }

    const auto availableSamples = samples.size();
    auto start = 0u;
    if (availableSamples < m_SamplesCount)
    {
        start = m_SamplesCount - availableSamples;
        for (auto i = 0u; i < start; ++i)
            m_Buffer[i].setY(m_Buffer.at(i + availableSamples).y());
    }

    SampleType value;
    for (auto i = start; i < m_SamplesCount; ++i)
    {
        memcpy(&value, &samples[i - start], sizeof(SampleType));
        m_Buffer[i].setY(value);
    }

    m_Series->replace(m_Buffer);
}

template<typename SampleType>
void CChartSamplesBuffer<SampleType>::Write(SampleType sample, size_t count)
{
    DEBUG_ASSERT(m_Series);
    if (!m_Series)
        return;

    if (m_Buffer.isEmpty())
    {
        m_Buffer.reserve(m_SamplesCount);
        for (auto i = 0u; i < m_SamplesCount; ++i)
            m_Buffer.append(QPointF(i, 0));
    }

    const auto availableSamples = count;
    auto start = 0u;
    if (availableSamples < m_SamplesCount)
    {
        start = m_SamplesCount - availableSamples;
        for (auto i = 0u; i < start; ++i)
            m_Buffer[i].setY(m_Buffer.at(i + availableSamples).y());
    }

    for (auto i = start; i < m_SamplesCount; ++i)
        m_Buffer[i].setY(sample);

    m_Series->replace(m_Buffer);
}


template<typename SampleType>
std::optional<QPointF> CChartSamplesBuffer<SampleType>::GetMinimum() const noexcept
{
    const auto iterator = std::min_element(m_Buffer.cbegin(), m_Buffer.cend(), [](const auto& first, const auto& second)
    {
        return first.y() < second.y();
    });

    return iterator == m_Buffer.cend() ? std::nullopt : std::optional<QPointF>{ *iterator };
}

template<typename SampleType>
std::optional<QPointF> CChartSamplesBuffer<SampleType>::GetMaximum() const noexcept
{
    const auto iterator = std::max_element(m_Buffer.cbegin(), m_Buffer.cend(), [](const auto& first, const auto& second)
    {
        return first.y() < second.y();
    });

    return iterator == m_Buffer.cend() ? std::nullopt : std::optional<QPointF>{ *iterator };
}

template<typename SampleType>
size_t CChartSamplesBuffer<SampleType>::GetSamplesCount() const noexcept
{
    return m_SamplesCount;
}