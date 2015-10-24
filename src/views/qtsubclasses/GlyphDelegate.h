#ifndef GLYPHDELEGATE_H
#define GLYPHDELEGATE_H

//#ifndef PI
//#define
//#define PI 3.141592653589793238462

#include <QItemDelegate>



class GlyphDelegate : public QItemDelegate
{
    Q_OBJECT

    //static const double PI= ;

public:
    GlyphDelegate(QWidget *parent = 0) : QItemDelegate(parent) {}

    static QString modelTableItemToText(QString str);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;

    void glyphRendering(QPainter * painter,
                        const QStyleOptionViewItem &option,
                        QVector<QString> names,
                        QVector<double> coeffs, //absolute values;
                        QVector<bool> signs,
                        QVector<int> index,
                        int start,
                        int size) const;

    double max(QVector<double> qvec) const;

    double min(QVector<double> qvec) const;

signals:
    
public slots:
    
};

#endif // GLYPHDELEGATE_H
