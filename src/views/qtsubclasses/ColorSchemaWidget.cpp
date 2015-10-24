#include "ColorSchemaWidget.h"
#include "ColorManager.h"
#include "ColorRamp.h"
#include "ColorMap.h"

#include <QPainter>
#include <QRegion>
#include <QDebug>


ColorSchemaWidget::ColorSchemaWidget(QWidget *parent) :
    QWidget(parent)
{
    isGrayScale = false;
//    colPiYG <<QColor("#8E0152")<<QColor("#C51B7D")<<QColor("#DE77AE")
//           <<QColor("#F1B6DA")<<QColor("#FDE0EF")<<QColor("#F7F7F7")
//          <<QColor("#E6F5D0")<<QColor("#B8E186")<<QColor("#7FBC41")
//         <<QColor("#4D9221")<<QColor("#276419");
//    QColor start=QColor("8E0152");
    ColorManager cm;

    ColorRamp* ramp = cm.getPredefinedColorRamp(ColorManager::SEQ_YLGNBU);
    ColorMap map(ramp);
    for(int i = 0; i< 100; i++){
        unsigned long color = map.getColor((float)i/(float)100);
        //qDebug()<<"color "<<color;
        int b = color & 0xFF;
        color = color >>8;
        int g =  color & 0xFF;
        color = color>>8;
        int r = color & 0xFF;
        color = color >>8;
        int a =color & 0xFF;
        colPiYG << QColor(r,g,b,a);
    }
}
ColorSchemaWidget::~ColorSchemaWidget(){

}

void ColorSchemaWidget::paintEvent(QPaintEvent*){
    QPainter painter(this);


    int width = this->rect().width();
    int height = this->rect().height();

    if(!isGrayScale){
        int numColors = colPiYG.size();

        int gridSize = width / numColors;

        for(int i = 0; i< numColors; i++){
            int x = i*gridSize;
            int y = 0;
            int w = gridSize;
            int h = height;
            painter.fillRect(x,y,w,h,colPiYG.at(i));
        }
    }else{
        int numColors = 20;

        int gridSize = width / numColors;

        for(int i = 0; i< numColors; i++){
            int x = i*gridSize;
            int y = 0;
            int w = gridSize;
            int h = height;
            painter.fillRect(x,y,w,h,QColor::fromHsv(0,0,255-i*255/numColors));
        }
    }
}

void ColorSchemaWidget::setGrayScale(bool b)
{
    isGrayScale = b;
}


