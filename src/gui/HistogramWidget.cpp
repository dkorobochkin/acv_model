#include "HistogramWidget.h"
#include "qcustomplot.h"
#include <QVector>

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent)
{
    resize(DEFAULT_SCALE_X,DEFAULT_SCALE_Y);
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
    customPlot->graph(0)->setData(QVector<double>::fromStdVector(valuesOfBrightness),QVector<double>::fromStdVector(brightnessHistogram));

    // Setting names of axes.
    customPlot->xAxis->setLabel("Pixel brightness");
    customPlot->yAxis->setLabel("Quantity of pixels");
    int max=(brightnessHistogram)[acv::Image::MIN_PIXEL_VALUE];
        for (int i=1;i<acv::Image::MAX_PIXEL_VALUE;i++)
            if ((brightnessHistogram)[i]>max)
                max=(brightnessHistogram)[i];

    // Setting axes proportions.
    customPlot->xAxis->setRange(acv::Image::MIN_PIXEL_VALUE, acv::Image::MAX_PIXEL_VALUE);
    customPlot->yAxis->setRange(0, max);
    customPlot->replot();
};
