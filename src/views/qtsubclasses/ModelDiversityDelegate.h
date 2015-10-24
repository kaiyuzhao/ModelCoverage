#ifndef LINECHARTDELEGATE_H
#define LINECHARTDELEGATE_H

#include <QItemDelegate>

class ModelDiversityDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ModelDiversityDelegate(QWidget *parent = 0):QItemDelegate(parent){}

    /*QWidget * createEditor(QWidget * parent,
                           const QStyleOptionViewItem & option,
                           const QModelIndex & index) const;*/
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //void setEditorData(QWidget * editor, const QModelIndex & index) const;
    
signals:
    
public slots:
    
};

#endif // LINECHARTDELEGATE_H
