#ifndef HIST_H
#define HIST_H
#include <qcustomplot.h>

class Hist : public QWidget
{
    Q_OBJECT

public:
        explicit Hist(QWidget *parent = 0);

private:
        QCustomPlot *customPlot;
private slots:

};

#endif // HIST_H
