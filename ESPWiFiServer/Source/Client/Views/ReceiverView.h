#if !defined(__RECEIVER_VIEW__)
#define __RECEIVER_VIEW__
#pragma once

#include "ClientViewBase.h"
#include "Client/Services/MorseCodeReceiverService.h"
#include "ui_ReceiverView.h"
#include "ChartSamplesBuffer.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

class CReceiverView : public CClientViewBase, public Ui::ReceiverView
{
    Q_OBJECT

public:
    CReceiverView(QWidget* parent = nullptr);
    virtual ~CReceiverView() override final = default;

    const std::string& GetName() const noexcept override final;

protected:
    virtual void OnServicePaired(IClientService& service) override final;
    virtual void OnServiceUnpaired(IClientService& service) override final;

private:
    void SetupCharts();
    void SetupChart(QLineSeries*& series, std::unique_ptr<QChart>& chart, const QString& title);
    void RefreshView();
    void AddAnalogSamplesToChart(std::vector<CMorseCodeReceiverService::SampleType>&& sample);
    void AddBinarySamplesToChart(std::vector<CMorseCodeReceiverService::StateType>&& states);

private slots:
    void OnStartReceivingButtonClicked();
    void OnStopReceivingButtonClicked();

private:
    CSamplesToBinaryReceiverService * m_ReceiverService = nullptr;

    QLineSeries* m_AnalogChartSeries = nullptr;
    std::unique_ptr<QChart> m_AnalogSamplesChart;
    std::unique_ptr<CChartSamplesBuffer<CMorseCodeReceiverService::SampleType>> m_AnalogSeriesBuffer;

    QLineSeries* m_BinaryChartSeries = nullptr;
    std::unique_ptr<QChart> m_BinarySamplesChart;
    std::unique_ptr<CChartSamplesBuffer<byte>> m_BinarySeriesBuffer;
};

#endif //__RECEIVER_VIEW__