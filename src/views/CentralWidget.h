#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)		{if(x) delete (x); x= 0;}
#endif

#include <QWidget>
#include <QModelIndex>
#include <QMutex>
#include <QColor>
#include <RInside.h>


class QCPBars;
class QCPStatisticalBox;

class QStandardItemModel;
class QStandardItem;
class QString;
class QGraphicsScene;
class QTreeNodeItem;
class VariableListDelegate;


//class QModelIndex;

namespace Ui {

class CentralWidget;

}


namespace ModelLocFitSchema{
    typedef enum {
    cl_scatter,
    cl_boxFillOuter,
    cl_boxFillInner,
    cl_boxFillMid,
    cl_boxPenOuter,
    cl_boxPenInner,
    cl_boxPenMid
    } COLOR;
}

/**
 * @brief The CentralWidget class
 */
class CentralWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CentralWidget(RInside & R,QWidget *parent = 0);
    ~CentralWidget();

    QMutex m_qmutex;
    
private:

    Ui::CentralWidget *ui;

    RInside & m_R;

    int m_varPartition;

    int m_modelPartitionType;

    VariableListDelegate* vdele;
    QVector< std::vector<std::string> > varNames;

    int varTableCurrentRow;

    bool m_mouseOnVarTable;
    bool m_mouseOnModelDiversTable;

    float m_coverageLBound;
    float m_coverageUBound;

    //for independent variale local correlation;
    QStandardItemModel* m_varListModel;

    //for a list of linear models;
    QStandardItemModel* m_modelListModel;

    //for a list of model diversity representation;
    QStandardItemModel* m_modelDivstyModel;

    //model coverages
    //a list of variables each partition the data space into local subsets
    //and a model is built for each subset;
    QStandardItemModel* m_modelCoverageModel;

    QGraphicsScene* m_scene;

    QTreeNodeItem* m_itemRoot;

    //QCPBars * m_localFitBars;

    //QCPStatisticalBox* m_boxPlot;

    //deals with ctrl/shift key modifer
    bool eventFilter(QObject* object, QEvent* event);

    bool modelDiversityTableEventFilter(QObject* obj, QEvent* event);

    bool varTableEventFilter(QObject* obj, QEvent* event);

    //deals with mouse event
    void mouseEventFilter(QObject* obj, QEvent* event);

    void varTableHoverEventFilter(QObject* obj, QEvent* event);

public slots:
    void dataLoaded();

    void processVarListClick(QModelIndex);

    void processModelListClick(QModelIndex);

    void processModelListDoubleClick(QModelIndex);

    void processModelDiversityLstClick(QModelIndex);

    void updateDataView(int row);

    void updateModelListView();

    void updateModelLocFitView(int varIdx, int modelIdx);

    void updateCoverageView();

    void updateCoverageViewHighlight(QVector<int>);

    void updateGraphicsView();

    void updateRepModelCoeffs(QString);

signals:
    void variableAndModelSelected(int varIdx, int modelIdx);

    void variableSelected(int row);

    void modelListReady();

    void modelCoverageReady();

    void colorSchemaChanged();

    void modelCoverageSelected();

private slots:
    void on_lBoundSlider_valueChanged(int value);

    void on_uBoundSlider_valueChanged(int value);

    void on_edt_indVarFilter_textChanged(const QString &arg1);

    void on_pb_clearIndVarFilter_clicked();

    void on_edt_modelDivFilter_textChanged(const QString &arg1);

    void on_pb_clearModelDivFilter_clicked();

    void on_sliderCutNum_valueChanged(int value);

private:
    void initializeTabViewVarList();
    void initializeQCpViewVarLocalFit();
    void initializeTabViewModelList();
    void initializeModelDiversView();
    void initializeModelCoverageView();
    void initializeModelClusteringView();

    void resetViews();

    QStandardItem* stdVectorToQSItem(const QString & scoreName,
                                  const std::vector<double> & scores) const;
    QColor color(ModelLocFitSchema::COLOR c);

};

#endif // CENTRALWIDGET_H
