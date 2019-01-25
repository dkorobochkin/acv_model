#ifndef HIST1_H
#define HIST1_H
#include <QHBoxLayout>
#include <QPushButton>
#include "qcustomplot.h"

class Hist : public QWidget
{
    Q_OBJECT

    public: //constants

    enum
    {
        SCALE_X = 500, // Horizontal property of window.
        SCALE_Y = 400 // Vertical property of window.
    };

    public:
            explicit Hist(QWidget *parent = 0);
    public:
            void DrawHist(QVector<double>& brightnessHistogramVector,QVector<double>& valuesOfBrightness);

    private:
        QCustomPlot *customPlot;
        QHBoxLayout *vbox;
    private slots:
};

#endif // HIST1_H
