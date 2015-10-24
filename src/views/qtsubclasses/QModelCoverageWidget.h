#ifndef QMODELCOVERAGEWIDGET_H
#define QMODELCOVERAGEWIDGET_H

#include <QWidget>
#include <QMutex>

class QModelCoverageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QModelCoverageWidget(QWidget *parent = 0);

    //display 4 rows and 3 columns
    void paintEvent(QPaintEvent*);

    QVector< QVector<double> > m_values;

    QVector<QRect> m_gridPos;

    QVector<QVector<QRect> > m_cellPos;

    QVector<QString> m_names;

    QVector <QColor> m_schema;

    float m_lBoundF; //0

    float m_uBoundF; //1

    void addValues(std::vector<double> stdVals,std::string stdNames);

    void addValues(std::vector<double> stdVals,
                   std::vector<double> stdLow,
                   std::vector<double> stdHigh,
                   std::string stdNames);

    void setHighlightIdx(QVector<int> highlightIdx);


    QVector <QVector <bool> > m_highlight;

    QVector <QVector <double> > m_lowerBound;
    QVector <QVector <double> > m_upperBound;

    void mouseMoveEvent(QMouseEvent * event);

    int whichGrid(QMouseEvent* event);

    int whichCell(QMouseEvent * event, int gridIdx);
    //void setColors(QV);
    //note that the actual parent of this widget is scrollArea
    //using the parent of scrollArea can tell this widget the actual size;
private:
    //number of rows; each row contains ncols color grids
    //int m_nRows;
    //number of cols;
    //int m_nCols;
    QMutex m_qmutex;

    int m_disRows; //nRows>4?nRows:4 number of rows displayed
    int m_disCols;
    //int disCols = nCols;

    int minTopMargin;//=10;
    int minBottomMargin;//=30;
    int minLeftMargin;//=10;
    int minRightMargin;//=10;

    //size of parent Container: the groupbox;
    int m_widthParent;
    int m_heightParent;

    //QVector < int > m_highlightIdx;
    int hoverGrid;
    int hoverCell;

    int hoverRow;
    int hoverColumn;

    double normalize(double value, double lBoundf,double uBoundf);

    int gridIdxCellIdxtoMemIdx(int gridIdx, int cellIdx);

signals:
    void dataAdded();
    void cellHovered(int memIdx);
public slots:
    void updateView();
};

#endif // QMODELCOVERAGEWIDGET_H
