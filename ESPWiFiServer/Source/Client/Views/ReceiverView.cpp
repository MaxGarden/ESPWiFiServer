#include "pch.h"
#include "ReceiverView.h"
#include <QMessageBox>

CReceiverView::CReceiverView(QWidget* parent /*= nullptr*/) :
    CClientViewBase(parent)
{
    setupUi(this);
    SetupCharts();
}

const std::string& CReceiverView::GetName() const noexcept
{
    static const std::string name = "Receiver";
    return name;
}

void CReceiverView::OnServicePaired(IClientService& service)
{
    if (const auto receiverService = dynamic_cast<CSamplesToBinaryReceiverService*>(&service))
    {
        DEBUG_ASSERT(!m_ReceiverService);
        m_ReceiverService = receiverService;
    }

    RefreshView();
}

void CReceiverView::OnServiceUnpaired(IClientService& service)
{
    if (m_ReceiverService == &service)
        m_ReceiverService = nullptr;

    RefreshView();
}

void CReceiverView::SetupCharts()
{
    SetupChart(m_AnalogChartSeries, m_AnalogSamplesChart, "Analog samples");
    SetupChart(m_BinaryChartSeries, m_BinarySamplesChart, "Binary samples");
}

void CReceiverView::SetupChart(QLineSeries*& series, std::unique_ptr<QChart>& chart, const QString& title)
{
    if (!m_SamplesChartGroupBox)
        return;

    series = new QLineSeries{ this };
    chart = std::make_unique<QChart>(nullptr);

    if (const auto legend = chart->legend())
        legend->hide();

    chart->addSeries(series);
    chart->createDefaultAxes();

    chart->setTitle(title);

    const auto chartView = new QChartView{ chart.get() };
    chartView->setRenderHint(QPainter::Antialiasing);

    if (const auto layout = m_SamplesChartGroupBox->layout())
        layout->addWidget(chartView);
}

void CReceiverView::RefreshView()
{
    if (m_ReceiverGroupBox)
        m_ReceiverGroupBox->setEnabled(m_ReceiverService != nullptr);

    const auto isReceiving = m_ReceiverService ? std::optional<bool>{m_ReceiverService->IsReceiving()} : std::nullopt;

    if (m_SettingsGroupBox)
        m_SettingsGroupBox->setEnabled(!isReceiving.value_or(true));

    if (m_StopReceivingPushButton)
        m_StopReceivingPushButton->setEnabled(isReceiving.value_or(false));
}

void CReceiverView::AddAnalogSamplesToChart(std::vector<int>&& samples)
{
    if (!m_AnalogSeriesBuffer || samples.empty())
        return;

    m_AnalogSeriesBuffer->Write(samples);

    if (const auto axis = m_AnalogSamplesChart->axisY())
    {
        const auto minimum = m_AnalogSeriesBuffer->GetMinimum().value_or(QPointF{}).y();
        const auto maximum = m_AnalogSeriesBuffer->GetMaximum().value_or(QPointF{}).y();
        axis->setRange(minimum, maximum);
    }
}

void CReceiverView::AddBinarySamplesToChart(std::vector<CMorseCodeReceiverService::StateType>&& states)
{
    if (!m_BinarySeriesBuffer || states.empty())
        return;

    for (const auto& state : states)
        m_BinarySeriesBuffer->Write(state.first ? 1 : 0, state.second * m_BinarySeriesBuffer->GetSamplesCount());
}

void CReceiverView::OnStartReceivingButtonClicked()
{
    DEBUG_ASSERT(m_ReceiverService);
    if (!m_ReceiverService)
        return;

    const auto isReceiving = m_ReceiverService->IsReceiving();
    DEBUG_ASSERT(!isReceiving);
    if (isReceiving)
        return;

    const auto getFrequency = [](auto spinbox)
    {
        if (!spinbox)
        {
            DEBUG_ASSERT(false);
            return 1;
        }

        return std::max(1, spinbox->value());
    };

    const auto samplesFrequency = getFrequency(m_SamplesFrequencySpinBox);
    const auto sendFrequency = getFrequency(m_SendFrequencySpinBox);
    const auto binarySamplingFrequency = getFrequency(m_BinarySamplingFrequencySpinBox);

    const auto treshold = static_cast<unsigned short int>(m_TresholdSpinBox ? m_TresholdSpinBox->value() : 0);

    DEBUG_ASSERT(!m_AnalogSeriesBuffer);
    if (m_AnalogChartSeries)
        m_AnalogSeriesBuffer = std::make_unique<CChartSamplesBuffer<CMorseCodeReceiverService::SampleType>>(m_AnalogChartSeries, samplesFrequency);

    DEBUG_ASSERT(m_BinaryChartSeries);
    if(m_BinaryChartSeries)
        m_BinarySeriesBuffer = std::make_unique<CChartSamplesBuffer<byte>>(m_BinaryChartSeries, samplesFrequency);

    const auto result = m_ReceiverService->StartReceiving(samplesFrequency, sendFrequency, treshold, binarySamplingFrequency,
    [this](auto&& states)
    {
        AddBinarySamplesToChart(std::move(states));
    },
    [this](auto&& samples)
    {
        AddAnalogSamplesToChart(std::move(samples));
    });

    DEBUG_ASSERT(result);
    if (!result)
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot start receiving!"));

        m_AnalogSeriesBuffer.reset();
        m_BinarySeriesBuffer.reset();
    }
    else
    {
        if (const auto axis = m_AnalogSamplesChart->axisX())
            axis->setRange(0, samplesFrequency);

        if (const auto axis = m_BinarySamplesChart->axisX())
            axis->setRange(0, samplesFrequency);
    }

    RefreshView();
}

void CReceiverView::OnStopReceivingButtonClicked()
{
    DEBUG_ASSERT(m_ReceiverService);
    if (!m_ReceiverService)
        return;

    const auto isReceiving = m_ReceiverService->IsReceiving();
    DEBUG_ASSERT(isReceiving);
    if (!isReceiving)
        return;

    const auto result = m_ReceiverService->EndReceiving();
    DEBUG_ASSERT(result);
    if (!result)
        QMessageBox::critical(this, tr("Error"), tr("Cannot end receiving!"));
    else
    {
        m_AnalogSeriesBuffer.reset();
        m_BinarySeriesBuffer.reset();
    }

    RefreshView();
}