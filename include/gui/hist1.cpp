#include "hist1.h"
#include "qcustomplot.h"


Hist::Hist(QWidget *parent) :
    QWidget(parent)
{
    resize(500,400);
    setWindowTitle("Histogram");
    customPlot = new QCustomPlot(this);

    vbox = new QHBoxLayout(this);

    vbox->addWidget(customPlot);
    setLayout(vbox);
}

void Hist::DrawHist(QVector<double>* mas, QVector<double>* mas2)
{
    customPlot->addGraph();
    customPlot->graph(0)->setData(*mas2, *mas);
    // задаем имена осей координат
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    int max=(*mas)[0];
        for (int i=1;i<255;i++)
            if ((*mas)[i]>max)
                max=(*mas)[i];
    // задаем размеры осей
    customPlot->xAxis->setRange(0, 255);
    customPlot->yAxis->setRange(0, max);
    customPlot->replot();
};
