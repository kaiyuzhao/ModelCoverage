#include "VariableListDelegate.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QString>

#include <QDebug>


void VariableListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    //hoveredRow = -1;

    QString val="";
    if (qVariantCanConvert<QString>(index.data())){
        val = qVariantValue<QString>(index.data());
    }else{
        QItemDelegate::paint(painter, option, index);
        return;
    }

    bool isNumber = false;
    bool isSelected = option.state & QStyle::State_Selected;
    bool ishover  = option.state & QStyle::State_MouseOver;
    if(isSelected){
        //qDebug()<<"painter selected";
        painter->fillRect(option.rect,option.palette.highlight());
    }


    double doubleVal = val.toDouble(&isNumber);

    if(!isNumber){
        doubleVal = 0;
    }
    int v = (1-doubleVal)*255;
    //QFont f; f.setFamily(f.defaultFamily()); f.setPointSize(20);
    //painter->setFont(f);
    if (option.showDecorationSelected && (option.state & QStyle::State_Selected)){
         //painter->fillRect(option.rect,Qt::cyan);
         painter->setPen(Qt::black);
         painter->drawText(option.rect,Qt::AlignCenter,val);
    }else{
        painter->fillRect(option.rect,QColor::fromHsv(0,0,v));
        if(v<100){
            painter->setPen(Qt::white);
            painter->drawText(option.rect,Qt::AlignCenter,val);
        }else{
            painter->setPen(Qt::black);
            painter->drawText(option.rect,Qt::AlignCenter,val);
        }
    }
    if(ishover){
        //if(!m_view)return;
        //m_view->m_currentRow = index.row();
        QRect rect;
        rect.setTop(option.rect.top()+1);
        rect.setBottom(option.rect.bottom()-3);
        rect.setLeft(option.rect.left()+1);
        rect.setRight(option.rect.right()-3);
        QPen pen(Qt::red);
        painter->save();
        painter->setPen(pen);
        painter->drawRect(rect);
        painter->restore();
    }//
    return;
}

void VariableListDelegate::setTabView(QCustomTableView *view)
{
    this->m_view = view;
}

