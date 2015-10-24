#ifndef VARIABLELISTDELEGATE_H
#define VARIABLELISTDELEGATE_H

#include <QItemDelegate>
#include "QCustomTableView.h"

class VariableListDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    int hoveredRow;

    VariableListDelegate(QWidget *parent = 0) :
        QItemDelegate(parent),hoveredRow(-1),m_view(0) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;

    void setTabView(QCustomTableView* view);
private:
    QCustomTableView* m_view;

signals:
    
public slots:
    
};

#endif // VARIABLELISTDELEGATE_H
