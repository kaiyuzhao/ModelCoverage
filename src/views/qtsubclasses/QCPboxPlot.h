#ifndef QCPBOXPLOT_H
#define QCPBOXPLOT_H

#include "qcustomplot.h"

class QCPboxPlot : public QCPStatisticalBox
{
public:
    QCPboxPlot(QCPAxis *keyAxis, QCPAxis *valueAxis);

    void draw(QCPPainter *painter);
};

#endif // QCPBOXPLOT_H
