#ifndef HISTOGRAM_WIDGET_H
#define HISTOGRAM_WIDGET_H
#include <QHBoxLayout>
#include <QPushButton>
#include "qcustomplot.h"
#include "Image.h"

class HistogramWidget : public QWidget
{
    Q_OBJECT

    public: //constants

    enum
    {
        DEFAULT_SCALE_X = 500, // Default length of the brightness histogram window.
        DEFAULT_SCALE_Y = 400 // Default heigth of the brightness histogram window.
    };

    public:
            explicit HistogramWidget(QWidget *parent = 0);
    public:
            void DrawHist(std::vector<double>& brightnessHistogram,std::vector<double>& valuesOfBrightness);

    private:
        QCustomPlot *customPlot;
        QHBoxLayout *vbox;
    private slots:
};

#endif // HISTOGRAM_WIDGET_H
