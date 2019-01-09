#ifndef HIST1_H
#define HIST1_H
#include <QHBoxLayout>
#include <QPushButton>
#include "qcustomplot.h"

class Hist : public QWidget
{
    Q_OBJECT

    public:
            explicit Hist(QWidget *parent = 0);
    public:
            void DrawHist(QVector<double>* mas,QVector<double>* mas2);

    private:
        QCustomPlot *customPlot;
        QHBoxLayout *vbox;
    private slots:
};

#endif // HIST1_H
