#include "QModelCoverageWidget.h"
#include "ColorManager.h"
#include "ColorRamp.h"
#include "ColorMap.h"

#include <QPainter>
#include <math.h>
#include <QMouseEvent>
#include <QDebug>
#include <cassert>

QModelCoverageWidget::QModelCoverageWidget(QWidget *parent) :
    QWidget(parent), m_lBoundF(0), m_uBoundF(1), m_disRows(5), m_disCols(3),
    minTopMargin(10),minBottomMargin(30),minLeftMargin(10),minRightMargin(10)
{
    this->setMouseTracking(true);
    this->hoverGrid =-1;
    this->hoverCell = -1;

    hoverRow = -1;
    hoverColumn = -1;

    this->m_heightParent = parent->height();
    this->m_widthParent = parent->width();
    ColorManager cm;

    ColorRamp* ramp = cm.getPredefinedColorRamp(ColorManager::SEQ_YLGNBU);
    ColorMap map(ramp);
    for(int i = 0; i< 20; i++){
        unsigned long color = map.getColor((float)i/(float)20);
        //qDebug()<<"color "<<color;
        int b = color & 0xFF;
        color = color >>8;
        int g =  color & 0xFF;
        color = color>>8;
        int r = color & 0xFF;
        color = color >>8;
        int a =color & 0xFF;
        m_schema << QColor(r,g,b,a);
    }

//    this->m_schema<<QColor("#8E0152")<<QColor("#C51B7D")<<QColor("#DE77AE")
//                 <<QColor("#F1B6DA")<<QColor("#FDE0EF")<<QColor("#F7F7F7")
//                <<QColor("#E6F5D0")<<QColor("#B8E186")<<QColor("#7FBC41")
//               <<QColor("#4D9221")<<QColor("#276419");
    //qDebug()<<"QModelCoverageWidget";

    connect(this,SIGNAL(dataAdded()),
            this,SLOT(updateView()),Qt::QueuedConnection);
}

void QModelCoverageWidget::paintEvent(QPaintEvent*){
    //qDebug()<<"QModelCoverageWidget paint";
    QPainter p(this);

    p.fillRect(0,0,width(),height(),Qt::white);

    if(this->m_cellPos.isEmpty()||this->m_values.isEmpty()){
        return;
    }

    if(this->m_cellPos.size()!=this->m_values.size()){
        //still updating;
        return;
    }
    bool isHighlight = false;

    for(int i = 0; i<m_cellPos.size();i++){

        QVector<double> cellVals = m_values.at(i);
        QVector<QRect> oneGrid = m_cellPos.at(i);
        if(cellVals.size() != oneGrid.size()){
            return;
        }

        QVector<bool> highlightVec;

        if(i<m_highlight.size()){
            highlightVec= this->m_highlight.at(i);
        }

        //int cellNum = ceil(sqrt((float)cellVals.size()));

        QString text = "Partitions by "+m_names.at(i);

        int textX = this->m_gridPos.at(i).left();
        int textY = this->m_gridPos.at(i).bottom()-20;

        p.setPen(Qt::black);
        p.drawText(textX,textY,text);

        bool isMouseHover = false;
        if(i==this->hoverGrid){
            isMouseHover = true;
        }

        for(int j = 0; j<oneGrid.size(); j++){

            QRect rect = oneGrid.at(j);
            float value = cellVals.at(j);

            if(highlightVec.size()>j){
                isHighlight = highlightVec.at(j);
            }

            float nVal = normalize(value,m_lBoundF,m_uBoundF);

            int colorIdx = nVal*(this->m_schema.size()-1);
            assert(colorIdx <m_schema.size());
            QColor c = m_schema.at(colorIdx);

            if(isMouseHover){
                if(j==hoverCell)
                {
                    c = QColor(200,0,0,200);//this->palette().highlight().color();
                }
            }

            p.fillRect(rect,c);

            if(isHighlight){
                p.setPen(Qt::red);
                p.drawRect(rect);
            }
        }
    }
    //qDebug()<<"paint success;";
}

void QModelCoverageWidget::addValues(std::vector<double> stdVals, std::string name){
    m_values<<QVector<double>::fromStdVector(stdVals);
    m_names<<QString::fromStdString(name);
    //qDebug()<<m_values;
    //resize the widget here;
    int numGrid = this->m_values.size();
    if(numGrid>this->m_disCols*this->m_disRows){
        int gridHeight = m_heightParent/m_disRows;
        int currentHeight = ceil((double)numGrid/3)*gridHeight;
        this->setMinimumSize(this->width(),currentHeight);
    }
    emit dataAdded();
}

void QModelCoverageWidget::addValues(std::vector<double> stdVals,
                                     std::vector<double> stdLow,
                                     std::vector<double> stdHigh,
                                     std::string stdNames)
{
    m_values<<QVector<double>::fromStdVector(stdVals);

    m_lowerBound << QVector<double>::fromStdVector(stdLow);
    m_upperBound << QVector<double>::fromStdVector(stdHigh);

    m_names<<QString::fromStdString(stdNames);

    int numGrid = this->m_values.size();
    if(numGrid>this->m_disCols*this->m_disRows){
        int gridHeight = m_heightParent/m_disRows;
        int currentHeight = ceil((double)numGrid/3)*gridHeight;
        this->setMinimumSize(this->width(),currentHeight);
    }
    emit dataAdded();
}

double QModelCoverageWidget::normalize(double value,
                                       double lBoundf,double uBoundf){
    float normVal = 0;
    if(value >uBoundf){
        normVal = 1;
    }else if(value <lBoundf){
        normVal = 0;
    }else{
        normVal = (value-lBoundf)/(uBoundf-lBoundf);
    }
    return normVal;
}

int QModelCoverageWidget::gridIdxCellIdxtoMemIdx(int gridIdx, int cellIdx)
{
    qDebug()<<"gridIdx "<<gridIdx<<"cellIdx "<<cellIdx;
    if(gridIdx >= m_cellPos.size())
        return -1;

    int count = 0;
    for(int i =0; i<gridIdx; i++){
        int size = m_cellPos.at(i).size();
        int rowSize = sqrt(size);
        int diagSize = (size-rowSize)/2+rowSize;
        count+=diagSize;
    }
    qDebug()<<"count "<<count;
    int sizeCurrentGrid = m_cellPos.at(gridIdx).size();
    qDebug()<<" sizeCurrentGrid "<<sizeCurrentGrid;
    int s = sqrt(sizeCurrentGrid);
    int r = cellIdx/s;
    int c = cellIdx%s;
    int diagIdx;
    if(c>=r){
        diagIdx = r*(s+(s-r+1))/2+c-r;
    }else{
        diagIdx = c*(s+(s-c+1))/2+r-c;
    }

    qDebug()<<"diagIdx "<<diagIdx;
    int memIdx = diagIdx+count;
    return memIdx;
}

void QModelCoverageWidget::setHighlightIdx(QVector<int> highlightIdx){
    //qDebug()<<"set highlight idx;";
    //this->m_highlightIdx = highlightIdx;
    QVector<int> tmp = highlightIdx;
    m_highlight.clear();
    QVector< QVector <int> > split;
    for(int i = 0; i< this->m_values.size(); i++){
        QVector<bool> isSelected;
        QVector<int> splitElem;
        for(int j = 0; j<this->m_values.at(i).size(); j++){
            isSelected<<false;
        }
        split<<splitElem;
        this->m_highlight<<isSelected;
    }
    for(int i = 0; i<tmp.size()/2; i++){
        //variable idx i*2
        //partition idx;
        int varIdx = tmp.at(i*2);
        int parIdx = tmp.at(i*2+1);
        split[varIdx]<<parIdx;
    }
    for(int i = 0; i<m_values.size(); i++){
        int mtxSize = ceil(sqrt((float)this->m_values.at(i).size()));
        int count = 0;
        int p = 0;
        QVector<int> parIdxs = split.at(i);
        for(int j = 0; j< mtxSize; j++){
            for( int k = j; k<mtxSize; k++){
                if(p<parIdxs.size()&&count == parIdxs.at(p)){
                    p++;
                    m_highlight[i][j*mtxSize+k] = true;
                    m_highlight[i][k*mtxSize+j] = true;
                }
                count++;
            }
        }
    }
}

void QModelCoverageWidget::mouseMoveEvent(QMouseEvent * event)
{
    //qDebug()<<"mouse move "<<event->pos();
    hoverGrid = whichGrid(event);
    hoverCell = whichCell(event,hoverGrid);
    this->repaint();
}

int QModelCoverageWidget::whichGrid(QMouseEvent *event)
{
    //QPoint mousePoint = event->pos();
    hoverRow = -1;
    hoverColumn = -1;
    //qDebug()<<"m_gridPos.size() "<<m_gridPos.size();
    for(int i = 0; i< m_gridPos.size(); i++){
        if(m_gridPos.at(i).contains(event->pos())){
            return i;
        }
    }
    return -1;
}

int QModelCoverageWidget::whichCell(QMouseEvent *event, int gridIdx)
{
    hoverRow = -1;
    hoverColumn = -1;

    if(gridIdx>=this->m_cellPos.size()||gridIdx<0){
        return -1;
    }

    QVector<QRect> oneGrid = this->m_cellPos.at(gridIdx);
    for(int i = 0; i<oneGrid.size(); i++){
        if(oneGrid.at(i).contains(event->pos())){
            int memIdx = gridIdxCellIdxtoMemIdx(gridIdx,i);
            qDebug()<<"cell hovered"<<memIdx;
            emit cellHovered(memIdx+1);
            return i;
        }
    }
    return -1;
}

void QModelCoverageWidget::updateView()
{
    if(!m_qmutex.tryLock()){
        //qDebug()<<"shit";
        return;
    }
    this->m_gridPos.clear();
    //this->m_gridPos.resize(0);
    this->m_cellPos.clear();
    //this->m_gridPos.resize(0);

    if(this->m_values.isEmpty()){
        return;
    }
    if(this->m_values.size()!=this->m_names.size()){
        return;
    }
    //qDebug()<<"QModelCoverageWidget paint";
    int gridWidth = (m_widthParent-minRightMargin*3)/m_disCols;
    int gridHeight = m_heightParent/m_disRows;

    int canvasWidth = gridWidth-minLeftMargin-minRightMargin;
    int canvasHeight = gridHeight-minTopMargin-minBottomMargin;
    int canvasRealWidth =
            canvasWidth>canvasHeight?canvasHeight:canvasWidth;

    int leftMargin = (gridWidth-canvasRealWidth)/2;

    for(int i = 0; i<m_values.size();i++){
        int r = i/m_disCols;
        int c = i%m_disCols;

        int left = c*gridWidth;
        int top = r*gridHeight;

        m_gridPos << QRect(left,top,gridWidth,gridHeight);

        QVector<double> cellVals = m_values.at(i);

        QVector<bool> highlightVec ;//= m_highlight.at(i);
        if(i<m_highlight.size()){
            highlightVec= this->m_highlight.at(i);
        }

        int cellNum = ceil(sqrt((float)cellVals.size()));
        float cellStepSize = (float)canvasRealWidth/(float)cellNum;
        int cellRealWidth = floor(cellStepSize)-2;

        QVector<QRect> oneGrid(m_values.at(i).size());
        for(int j = 0; j<cellVals.size(); j++){
            int row = j/cellNum;
            int col = j%cellNum;
            int x = col*cellStepSize+leftMargin+left;
            int y = row*cellStepSize+minTopMargin+top;

            int width = cellRealWidth;
            int height = cellRealWidth;

            oneGrid[j] = QRect(x,y,width,height);
        }
        m_cellPos << oneGrid;
    }
    m_qmutex.unlock();
    this->repaint();
}
