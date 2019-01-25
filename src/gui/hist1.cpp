#include "hist1.h"
#include "qcustomplot.h"


Hist::Hist(QWidget *parent) :
    QWidget(parent)
{
    resize(SCALE_X,SCALE_Y);
    setWindowTitle("Histogram");
    customPlot = new QCustomPlot(this);

    vbox = new QHBoxLayout(this);

    vbox->addWidget(customPlot);
    setLayout(vbox);
}

void Hist::DrawHist(QVector<double>& brightnessHistogramVector, QVector<double>& valuesOfBrightness)
{
    customPlot->addGraph();
    customPlot->graph(0)->setData(valuesOfBrightness, brightnessHistogramVector);

    // Setting names of axes.
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");

    int max=(brightnessHistogramVector)[0];
        for (int i=1;i<255;i++)
            if ((brightnessHistogramVector)[i]>max)
                max=(brightnessHistogramVector)[i];

    // Setting axes proportions.
    customPlot->xAxis->setRange(0, 255);
    customPlot->yAxis->setRange(0, max);
    customPlot->replot();
};
