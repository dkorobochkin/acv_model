#include "hist.h"
#include "qcustomplot.h"
#include "mainwindow.h"


Hist::Hist(QWidget *parent):
    QWidget(parent)
{
    resize(500,400);
    setWindowTitle("Histogram");
    customPlot = new QCustomPlot(this);
    vbox = new QVBoxLayout(this);

    vbox->addWidget(customPlot);
    connect(MainWindow::mImgCreateBrightnessHistogramAction, SIGNAL(triggered()), this, SLOT(CreateBrightnessHistogram()));
}
