#ifndef QTREENODEEDGE_H
#define QTREENODEEDGE_H

#include <QGraphicsLineItem>
#include <QObject>

class QTreeNodeEdge : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    explicit QTreeNodeEdge();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    
signals:
    
public slots:
    
};

#endif // QTREENODEEDGE_H
