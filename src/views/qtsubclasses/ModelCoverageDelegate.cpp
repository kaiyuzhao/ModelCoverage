#include "ModelCoverageDelegate.h"
#include "math.h"

#include <QPainter>
//#include <QWidget>
//#include <QStyleOptionViewItem>


ModelCoverageDelegate::ModelCoverageDelegate(QWidget *parent):
    QItemDelegate(parent)
{
    colSpectral << QColor("#9E0142")<<QColor("#D53E4F")<<QColor("#F46D43")
              <<QColor("#F46D43")<<QColor("#FDAE61")<<QColor("#FEE08B")
             <<QColor("#FFFFBF")<<QColor("#E6F598")<<QColor("#ABDDA4")
            <<QColor("#66C2A5")<<QColor("#3288BD")<<QColor("#5E4FA2");

    colPiYG <<QColor("#8E0152")<<QColor("#C51B7D")<<QColor("#DE77AE")
           <<QColor("#F1B6DA")<<QColor("#FDE0EF")<<QColor("#F7F7F7")
          <<QColor("#E6F5D0")<<QColor("#B8E186")<<QColor("#7FBC41")
         <<QColor("#4D9221")<<QColor("#276419");
}

void ModelCoverageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const{
    QString val="";
    if (qVariantCanConvert<QString>(index.data())){
        val = qVariantValue<QString>(index.data());
    }else{
        QItemDelegate::paint(painter, option, index);
        return;
    }
    QStringList cellContents = val.split(";");

    QString name = cellContents.takeFirst();
    QString uBound = cellContents.takeLast();
    QString lBound = cellContents.takeLast();

    bool u = false;
    bool l = false;
    float uBoundf = uBound.toFloat(&u);
    float lBoundf = lBound.toFloat(&l);
    if(!(u&l)){
        return;
    }
    QVector<double> values;

    for(int i = 0; i<cellContents.size(); i++){
        QString diversityVal = cellContents.at(i);
        values << diversityVal.toDouble();
    }

    int gridnum = ceil(sqrt((float)values.size()));

    int minTopMargin=10;
    int minBottomMargin=30;
    int minLeftMargin=10;
    int minRightMargin=10;

    int cawidth = option.rect.width()-minLeftMargin-minRightMargin;
    int caheight = option.rect.height()-minTopMargin-minBottomMargin;

    int coverageGridTotalWidth = cawidth>caheight?caheight:cawidth;

    int realWidth = coverageGridTotalWidth; //realheight == realWidth

    //shrink the grid to make border of 4 pixels width
    float stepSize = (float)coverageGridTotalWidth/(float)gridnum;
    int gridWidth = floor(stepSize)-2;

    //int realHeight = gridnum*coverageGridSize;

    int leftMargin = (option.rect.width()-realWidth)/2;

    for(int i = 0; i<values.size(); i++){
        int row = i/gridnum;
        int col = i%gridnum;
        int x = col*stepSize+leftMargin+option.rect.x();
        int y = row*stepSize+minTopMargin+option.rect.y();
        int width = gridWidth;
        int height = gridWidth;
        float value = values.at(i);
        float normVal = 0;
        if(value >uBoundf){
            normVal = 1;
        }else if(value <lBoundf){
            normVal = 0;
        }else{
            normVal = (value-lBoundf)/(uBoundf-lBoundf);
        }
        int colIdx = normVal*(this->colPiYG.size()-1);
        QColor c = colPiYG.at(colIdx);
        c.setAlpha(150);
        //painter->begin();
        //painter->setPen(c);
        //painter->setBrush(c);
        painter->fillRect(x,y,width,height,c);
        //painter->end();
    }
    QString text = "Partitions by "+name;
    int textX = option.rect.x()
            +0.5*(option.rect.width() - text.length()*option.font.pointSize()*2);
    int textY = option.rect.y()
            +minTopMargin+ realWidth + option.font.pointSize()*2;
    painter->setPen(Qt::black);
    painter->drawText(textX,textY,text);

}
