#include "HistogramWidget.h"
#include "qcustomplot.h"
#include <QVector>
#include "AImage.h"

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent)
{
    resize(DEFAULT_SCALE_X, DEFAULT_SCALE_Y);
    setWindowTitle("Histogram");
    customPlot = new QCustomPlot(this);

    vbox = new QHBoxLayout(this);

    vbox->addWidget(customPlot);
    setLayout(vbox);
}

void HistogramWidget::DrawHist(std::vector<double>& brightnessHistogram, std::vector<double>& valuesOfBrightness)
//brightnessHistogram - axis Y; valuesOfBrightness - axis X.
{
    customPlot->addGraph();
    customPlot->graph(0)->setData(QVector<double>::fromStdVector(valuesOfBrightness), QVector<double>::fromStdVector(brightnessHistogram));

    // Setting names of axes.
    customPlot->xAxis->setLabel("Pixel brightness");
    customPlot->yAxis->setLabel("Quantity of pixels");

    // Setting axes proportions.
    customPlot->xAxis->setRange(AImage::MIN_PIXEL_VALUE, AImage::MAX_PIXEL_VALUE);
    customPlot->yAxis->setRange(0, *std::max_element(brightnessHistogram.begin(), brightnessHistogram.end()));
    customPlot->replot();
};
