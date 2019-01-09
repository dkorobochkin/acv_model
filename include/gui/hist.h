#ifndef HIST_H
#define HIST_H
#include <QVBoxLayout>
#include <QPushButton>
#include "qcustomplot.h"

class Hist : public QWidget
{
    Q_OBJECT

    public:
            explicit Hist(QWidget *parent = 0);
    public:
            void DrawHist(QVector<double>& mas,QVector<double>& mas2);

    private:
        QCustomPlot *customPlot;
        QVBoxLayout *vbox;
    private slots:
};

#endif // HIST_H
