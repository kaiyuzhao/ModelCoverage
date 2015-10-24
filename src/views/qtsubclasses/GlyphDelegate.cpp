#include "GlyphDelegate.h"
#include <limits>
#include <cmath>
#include <math.h>

#include <QPainter>
#include <QItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QRect>
#include <QPoint>
#include <QPolygon>
#include <QPalette>

#include <QDebug>


void GlyphDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{

    //QItemDelegate::drawFocus(painter, option, option.rect);

    if(index.column()==0){//
        //first column;  do customized rendering here;

        //first parse the string;

        QString val="";
        if (qVariantCanConvert<QString>(index.data())){
            val = qVariantValue<QString>(index.data());
        }else{
            QItemDelegate::paint(painter, option, index);
            return;
        }
        bool isSelected = option.state & QStyle::State_Selected;
        if(isSelected){
            painter->fillRect(option.rect, option.palette.highlight());
        }
        QString text = GlyphDelegate::modelTableItemToText(val);
        painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignLeft, text);
        //this->glyphRendering(painter,option,names,coeffs,signs,idx,startVar,numVars);
       // glyphRendering(painter,option,names,coeffs,signs,idx,startVar,numVars);
    }else{
        QItemDelegate::paint(painter, option, index);
    }

}


void GlyphDelegate::glyphRendering(QPainter * painter,
                                   const QStyleOptionViewItem &option,
                                   QVector<QString> names,
                                   QVector<double> coeffs, //absolute values;
                                   QVector<bool> signs,
                                   QVector<int> index,
                                   int start,
                                   int size) const
{

    int width= 0.5*(option.rect.height()+option.rect.width());
    QPoint origin = option.rect.center();

    QPolygon meshes;
    QPolygon surfaces;

    float minRadian = M_PI;
    float maxRadian = 0;

    for(int i = 0; i< index.size(); i++){

        double ratio = (double)(i-start)/(double)(index.size() - start);

        double maxcoe = this->max(coeffs);

        double mincoe = this->min(coeffs);



        float length;

        if(abs(maxcoe-mincoe)<0.001){
            length =
                           (double)(width*0.05)+
                           (double)(width*0.20);
        }else{
            length =
                           (double)(width*0.05)+
                           (double)(width*0.20)*(double)(coeffs.at(i)-mincoe)/(double)(maxcoe-mincoe);
        }

        float degree = (ratio*M_PI*2);

        if(minRadian>degree) minRadian=degree;

        if(maxRadian<degree) maxRadian=degree;

        int x = length*std::sin(degree)+origin.x();
        int y = origin.y()+length*std::cos(degree);

        meshes << QPoint(x,y) << origin;

        surfaces << QPoint(x,y);

        //float textRadius = (double)(width*0.30);
        int txt_x = (length+(double)width*0.05)*std::sin(degree)+origin.x();

        if(degree>M_PI){
            txt_x-=names.at(i).size()*5;
        }

        int txt_y = origin.y()+(length+(double)width*0.05)*std::cos(degree);

        painter->setPen(Qt::black);
        painter->drawText(txt_x,txt_y,names.at(i));
    }

    if(surfaces.size()>0&&(maxRadian-minRadian)>M_PI){
        //surfaces << origin;
        surfaces << surfaces.at(0);
    }

    painter->setPen(QColor(200,0,0,200));

    painter->drawPolyline(meshes);
}

double GlyphDelegate::max(QVector<double> qvec) const
{

    double max = 0;

    for(int i = 0; i<qvec.size(); i++){
        if(qvec.at(i)>max){
            max = qvec.at(i);
        }
    }
    return max;
}

double GlyphDelegate::min(QVector<double> qvec) const
{

    double min = std::numeric_limits<double>::max();

    for(int i = 0; i<qvec.size(); i++){
        if(qvec.at(i)<min){
            min = qvec.at(i);
        }
    }
    return min;
}


QString GlyphDelegate::modelTableItemToText(QString val)
{
    QStringList list = val.split(";");

    //qDebug()<<val;

    int startVar = list.takeFirst().toInt();

    int numVars = list.takeFirst().toInt();

    //start;size;name1:val1:idx;name2:val2:idx2;name3:val3:idx3;
    QVector<QString> names;
    QVector<double> coeffs;
    QVector<bool> signs;
    QVector<int> idx;

    for(int i = 0; i< list.size(); i++){
        QString temp = list.at(i);
        QStringList elems = temp.split(":");

        QString elmName = elems.at(0);
        names<<elmName;

        QString elmVal = elems.at(1);
        signs<<(elmVal.toDouble()>=0);
        double val = elmVal.toDouble();
        coeffs<<val;

        int elmIdx = elems.at(2).toInt();
        idx<<elmIdx;
    }

    QString text = "";

    for(int i = 0; i<coeffs.size(); i++){
        if(signs.at(i)){
            text+="+";
        }
        text += QString::number(coeffs.at(i),'g',3);
        if(i!=0)
            text += names.at(i);
    }
    return text;
}
