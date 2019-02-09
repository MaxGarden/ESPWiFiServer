#if !defined(__CHART_SAMPLES_BUFFER__)
#define __CHART_SAMPLES_BUFFER__
#pragma once

#include <QtCharts/QXYSeries>

QT_CHARTS_USE_NAMESPACE

template<typename SampleType>
class CChartSamplesBuffer final
{
public:
    CChartSamplesBuffer(QXYSeries* series, size_t samplesCount);
    ~CChartSamplesBuffer() = default;

    void Write(const std::vector<SampleType>& samples);

    std::optional<QPointF> GetMinimum() const noexcept;
    std::optional<QPointF> GetMaximum() const noexcept;

private:
    QXYSeries* m_Series;
    QVector<QPointF> m_Buffer;
    const size_t m_SamplesCount;
};

#include "ChartSamplesBuffer.inl"

#endif //__CHART_SAMPLES_BUFFER__