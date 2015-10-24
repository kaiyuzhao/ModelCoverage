#include "ModelDiversityDelegate.h"
#include "qcustomplot.h"



/*QWidget * LineChartDelegate::createEditor(QWidget * parent,
                       const QStyleOptionViewItem & option,
                       const QModelIndex & index) const{

    qDebug()<<"qcustomplot";
    QCustomPlot* qcplot = new QCustomPlot(parent);


    //return(qcplot);
    return qcplot;//QItemDelegate::createEditor(parent,option, index);

}*/

/*void LineChartDelegate::setEditorData
 *(QWidget * editor, const QModelIndex & index) const{
    editor = new QCustomPlot();
    //QItemDelegate::setEditorData(editor,index);
}*/

void ModelDiversityDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const{
    QString val="";
    if (qVariantCanConvert<QString>(index.data())){
        val = qVariantValue<QString>(index.data());
    }else{
        QItemDelegate::paint(painter, option, index);
        return;
    }

    //val is semicolon delimited
    //parse the val into:
    //1: QString name; first element;
    //2: QVector of doubles; the reset;

    QStringList cellContents = val.split(";");

    QString name = cellContents.takeFirst();

    QVector<double> values;
    QVector<double> partitionids;

    for(int i = 0; i<cellContents.size(); i++){
        QString diversityVal = cellContents.at(i);
        values << diversityVal.toDouble();
        partitionids << (double)i;
    }

    bool isSelected = option.state & QStyle::State_Selected;

    QItemDelegate::paint(painter,option,index);

    QCustomPlot qcplot;

    //qcplot.clearGraphs();
    qcplot.addGraph();

    qcplot.graph(0)->addData(partitionids,values);
    qcplot.graph(0)->setPen(QColor(255,0,0,127));
    //qcplot.graph(0)->setLineStyle(QCPGraph::);
    //qcplot.graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle, 2));

    qcplot.xAxis->setRange(0,cellContents.size());
    qcplot.xAxis->setAutoTickCount(4);

    qcplot.yAxis->setRange(-0.1,1);
    //qcplot.rescaleAxes();
    qcplot.yAxis->setAutoTickCount(2);

    qcplot.xAxis->setAntialiased(true);
    qcplot.xAxis->setLabel("Partition Index");
    qcplot.yAxis->setAntialiased(true);
    qcplot.yAxis->setLabel("R"+QString::fromUtf8("\u00B2"));

    qcplot.plotLayout()->insertRow(0);


    QCPPlotTitle title(&qcplot, "R"+QString::fromUtf8("\u00B2")+" distribution over "+name);
    QFont f; f.setFamily(f.defaultFamily());f.setPointSize(7);

    title.setFont(f);

    qcplot.plotLayout()->addElement(0, 0, &title);

    qcplot.plotLayout()->setMinimumSize(5,5);
    qcplot.plotLayout()->setRowSpacing(0);
    qcplot.plotLayout()->setRowStretchFactor(0,0.5);

    qcplot.replot();

    QPixmap buf = qcplot.toPixmap(option.rect.width(),option.rect.height());

    //this->ui->customPlotWidget->xAxis->setLabel(m_xName);
    //this->ui->customPlotWidget->yAxis->setLabel(m_yName);

    //qcplot->toPainter (painter);
    painter->drawPixmap(option.rect,buf);
    if(isSelected){
        painter->save();
        painter->setPen(QPen(option.palette.highlight().color()));
        QRect rect;
        rect.setTop(option.rect.top()+1);
        rect.setBottom(option.rect.bottom()-3);
        rect.setLeft(option.rect.left()+1);
        rect.setRight(option.rect.right()-3);
        painter->drawRect(rect);
        painter->restore();
    }
}
