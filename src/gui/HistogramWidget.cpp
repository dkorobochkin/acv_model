#include "HistogramWidget.h"
#include "qcustomplot.h"


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

void HistogramWidget::DrawHist(QVector<double>& brightnessHistogram, QVector<double>& valuesOfBrightness)
//brightnessHistogram - axis Y; valuesOfBrightness - axis X.
{
    customPlot->addGraph();
    customPlot->graph(0)->setData(valuesOfBrightness, brightnessHistogram);

    // Setting names of axes.
    customPlot->xAxis->setLabel("Pixel brightness");
    customPlot->yAxis->setLabel("Quantity of pixels");

    int max=(brightnessHistogram)[0];
        for (int i=1;i<255;i++)
            if ((brightnessHistogram)[i]>max)
                max=(brightnessHistogram)[i];

    // Setting axes proportions.
    customPlot->xAxis->setRange(0, 255);
    customPlot->yAxis->setRange(0, max);
    customPlot->replot();
};
