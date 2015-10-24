#include "QTreeNodeEdge.h"

#include <QDebug>
#include <QPen>
#include <QPainter>

QTreeNodeEdge::QTreeNodeEdge() :
     QObject(),
     QGraphicsLineItem()
{
    this->setFlag(QGraphicsItem::ItemIsMovable);
}

void QTreeNodeEdge::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    QPen _Pen;
    _Pen.setColor(Qt::black);
    painter->setPen(_Pen);
    painter->drawLine(this->line());
    //qDebug()<<this->line();
}
