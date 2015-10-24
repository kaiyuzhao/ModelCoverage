#ifndef MODELCOVERAGEDELEGATE_H
#define MODELCOVERAGEDELEGATE_H

#include <QItemDelegate>
#include <QVector>
#include <QColor>

class ModelCoverageDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ModelCoverageDelegate(QWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

    QVector<QColor> colSpectral;
    QVector<QColor> colPiYG;
signals:
    
public slots:
    
};

#endif // MODELCOVERAGEDELEGATE_H
