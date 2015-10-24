#ifndef QCUSTOMTABLEVIEW_H
#define QCUSTOMTABLEVIEW_H

#include <QTableView>
#include <QWidget>

class QCustomTableView : public QTableView
{
public:
    QCustomTableView(QWidget* parent = 0) : QTableView(parent),m_currentRow(-1){}

    int m_currentRow;

    //void setModel(QAbstractItemModel* model);
};

#endif // QCUSTOMTABLEVIEW_H
