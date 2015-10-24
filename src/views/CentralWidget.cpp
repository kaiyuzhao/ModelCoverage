#include "CentralWidget.h"
#include "ui_CentralWidget.h"
#include "VariableListDelegate.h"
#include "GlyphDelegate.h"
#include "ModelDiversityDelegate.h"
#include "ModelCoverageDelegate.h"
#include "QTreeNodeItem.h"


#include <QStandardItem>
#include <QStandardItemModel>
#include <QDebug>
#include <QMessageBox>

#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <cassert>


//#include <QModelIndex>


CentralWidget::CentralWidget(RInside & R,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CentralWidget),
    m_R(R), m_varPartition(10),
    m_modelPartitionType(2),
    varTableCurrentRow(-2),
    m_itemRoot(0)
{
    ui->setupUi(this);
    initializeTabViewVarList();
    initializeQCpViewVarLocalFit();//;initializeVarLocalFitView();
    initializeTabViewModelList();
    initializeModelDiversView();
    initializeModelCoverageView();
    initializeModelClusteringView();
    //ui->graphicsViewModelClusters

}

CentralWidget::~CentralWidget()
{
    SAFE_DELETE(m_modelListModel);//if(m_modelListModel)delete m_modelListModel;
    SAFE_DELETE(m_varListModel);//if(m_varListModel)delete m_varListModel;
    SAFE_DELETE(m_modelDivstyModel);//if(m_modelDivstyModel)delete m_modelDivstyModel;
    SAFE_DELETE(m_modelCoverageModel);
    SAFE_DELETE(m_itemRoot);
    SAFE_DELETE(m_scene);
    //SAFE_DELETE(m_boxPlot);
    delete ui;
}


void CentralWidget::initializeTabViewVarList(){
    m_varListModel = 0;
    m_varListModel = new QStandardItemModel();
    m_mouseOnVarTable = false;

    connect(ui->tabViewVariableList,SIGNAL(clicked(QModelIndex)),
            this,SLOT(processVarListClick(QModelIndex)));

    connect(this,SIGNAL(variableSelected(int)),
            this,SLOT(updateDataView(int)),Qt::QueuedConnection);

    //    connect(this,SIGNAL(varListItemSelected(int,int)),
    //            this,SLOT(updateLocalFitView(int,int)));

    ui->tabViewVariableList->setMouseTracking(true);
    ui->tabViewVariableList->viewport()->setMouseTracking(true);

    //ui->tabViewVariableList->viewport()->setMouseTracking(true);
    ui->tabViewVariableList->installEventFilter(this);
    ui->tabViewVariableList->viewport()->installEventFilter(this);

    ui->tabViewVariableList->verticalHeader()->setVisible(false);
    ui->tabViewVariableList->setModel(m_varListModel);
    //m_varListModel->clear();
    ui->tabViewVariableList->horizontalHeader()
            ->setResizeMode(QHeaderView::ResizeToContents);
    ui->tabViewVariableList->verticalHeader()
            ->setResizeMode(QHeaderView::ResizeToContents);

    QFont f; f.setFamily(f.defaultFamily());f.setPointSize(7);
    ui->tabViewVariableList->horizontalHeader()->setFont(f);
    ui->tabViewVariableList->setFont(f);
    ui->tabViewVariableList->setEditTriggers(QTableView::NoEditTriggers);
    //ui->tabViewVariableList->resizeColumnsToContents();
    //ui->tabViewVariableList->resizeRowsToContents();

    vdele = new VariableListDelegate();
    ui->tabViewVariableList->setItemDelegate(vdele);
    vdele->setTabView(ui->tabViewVariableList);
    ui->colorSchemaGray->setGrayScale(true);

    ui->dataViewWidget->xAxis->setVisible(false);
    ui->dataViewWidget->yAxis->setVisible(false);
}

void CentralWidget::initializeQCpViewVarLocalFit(){
    ui->qcpPlotLocFitting->setMouseTracking(true);
    ui->qcpPlotLocFitting->installEventFilter(this);
    ui->qcpPlotLocFitting->xAxis->setVisible(false);
    ui->qcpPlotLocFitting->yAxis->setVisible(false);

    connect(this, SIGNAL(variableAndModelSelected(int,int)),
            this,SLOT(updateModelLocFitView(int,int)),Qt::QueuedConnection);
    //this->m_boxPlot = new QCPStatisticalBox(ui->qcpPlotLocFitting->xAxis,
    //ui->qcpPlotLocFitting->yAxis);
    //m_localFitBars = new QCPBars();
    //ui->qcpPlotLocFitting->addPlottable(m_boxPlot);
}

void CentralWidget::initializeTabViewModelList(){
    m_modelListModel = 0;
    m_modelListModel = new QStandardItemModel();

    connect(ui->tabViewModelOveralFitting,SIGNAL(clicked(QModelIndex)),
            this,SLOT(processModelListClick(QModelIndex)),Qt::QueuedConnection);


    connect(ui->tabViewModelOveralFitting,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(processModelListDoubleClick(QModelIndex)),Qt::QueuedConnection);

    connect(this,SIGNAL(modelListReady()),
            this,SLOT(updateModelListView()),Qt::QueuedConnection);

    ui->tabViewModelOveralFitting->setMouseTracking(true);
    ui->tabViewModelOveralFitting->installEventFilter(this);
    ui->tabViewModelOveralFitting->viewport()->setMouseTracking(true);
    ui->tabViewModelOveralFitting->viewport()->installEventFilter(this);

    //set the qtableview model here, then render the model glyph in the delegate;
    ui->tabViewModelOveralFitting->setModel(m_modelListModel);
    ui->tabViewModelOveralFitting->verticalHeader()->setVisible(false);
    ui->tabViewModelOveralFitting->setEditTriggers(QTableView::NoEditTriggers);

    ui->tabViewModelOveralFitting->verticalHeader()
            ->setResizeMode(QHeaderView::Fixed);
    //ui->tabViewModelOveralFitting->horizontalHeader()
    //->setResizeMode(QHeaderView::Fixed);

    //ui->tabViewModelOveralFitting->horizontalHeader()
    //     ->setResizeMode(QHeaderView::ResizeToContents)

    ui->tabViewModelOveralFitting
            ->setSelectionMode(QAbstractItemView::SingleSelection);

    GlyphDelegate* glyphdele = new GlyphDelegate();
    ui->tabViewModelOveralFitting->setItemDelegate(glyphdele);
}

void CentralWidget::initializeModelDiversView(){
    m_modelDivstyModel = 0;
    m_modelDivstyModel = new QStandardItemModel();
    m_mouseOnModelDiversTable = false;

    connect(ui->tabViewLocModelDiversity,SIGNAL(clicked(QModelIndex)),
            this,SLOT(processModelDiversityLstClick(QModelIndex)));

    ui->tabViewLocModelDiversity->setMouseTracking(true);
    ui->tabViewLocModelDiversity->installEventFilter(this);
    ui->tabViewLocModelDiversity->viewport()->setMouseTracking(true);
    ui->tabViewLocModelDiversity->viewport()->installEventFilter(this);

    ui->tabViewLocModelDiversity->setModel(m_modelDivstyModel);
    ui->tabViewLocModelDiversity->verticalHeader()->setVisible(false);
    ui->tabViewLocModelDiversity->horizontalHeader()->setVisible(false);

    ui->tabViewLocModelDiversity->verticalHeader()
            ->setResizeMode(QHeaderView::Fixed);
    ui->tabViewLocModelDiversity->setEditTriggers(QTableView::NoEditTriggers);

    ModelDiversityDelegate* lineDelegate = new ModelDiversityDelegate();
    ui->tabViewLocModelDiversity->setItemDelegate(lineDelegate);
}

void CentralWidget::initializeModelCoverageView(){
    m_modelCoverageModel = 0;

    //the range of Rsquare is 0 to 1;
    m_coverageLBound = 0;
    m_coverageUBound = 1;

    //this->connect()
    connect(this,SIGNAL(colorSchemaChanged()),
            this,SLOT(updateCoverageView()));

    ui->lBoundSlider->setRange(0,90);
    ui->uBoundSlider->setRange(10,100);

    QPalette modelCoveragePal = ui->viewModelCoverage->palette();
    modelCoveragePal.setColor(ui->viewModelCoverage->backgroundRole(), Qt::white);
    ui->viewModelCoverage->setPalette(modelCoveragePal);

    QPalette scrollAreaPal = ui->scrollArea->palette();
    scrollAreaPal.setColor(ui->scrollArea->backgroundRole(),Qt::white);
    ui->scrollArea->setPalette(scrollAreaPal);

    ui->viewModelCoverage->setMinimumSize(ui->scrollArea->size());
    ui->scrollArea->setWidgetResizable(true);
}


void CentralWidget::initializeModelClusteringView(){
    m_itemRoot = 0;
    m_scene = new QGraphicsScene(ui->graphicsViewRepresentivity);
    ui->graphicsViewRepresentivity->setScene(m_scene);
    ui->graphicsViewRepresentivity->setMouseTracking(true);
    ui->graphicsViewRepresentivity->viewport()->setMouseTracking(true);
    connect(this,SIGNAL(modelCoverageSelected()),
            this,SLOT(updateGraphicsView()));
}

/**
 * @brief CentralWidget::dataLoaded
 * data is loaded into a data.frame in R
 * name is "df"
 * call varRank <- localfit.rank(df,ctype="2",maxbin=7,filter=0.0);
 */
void CentralWidget::dataLoaded(){
    using namespace Rcpp;

    this->resetViews();

    std::vector<std::string> varNames;

    try{
        QString strEval = "model.list<-NULL;";
        m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<"success!";

        strEval = "varRank <- localfit.rank(df);";
        m_R.parseEval(strEval.toStdString());

        qDebug()<<strEval<<"processing...";
        //result <- list("order"=o,"rsquare"=r,"names"=ind.names);
        strEval = "varRank$rsquare[varRank$order];";
        QVector<double> rsquare = QVector<double>::fromStdVector(
                    Rcpp::as<std::vector<double> >(m_R.parseEval(strEval.toStdString())));
        qDebug()<<strEval<<"success!";

        strEval = "varRank$names[varRank$order];";
        qDebug()<<strEval<<"processing...";
        SEXP proxy = m_R.parseEval(strEval.toStdString());
        varNames = Rcpp::as< std::vector< std::string> >(proxy);
        qDebug()<<strEval<<"success!";

        //nc = scores.ncol();
        //nr = scores.nrow();

        m_varListModel->clear();
        m_varListModel->setRowCount(rsquare.size()); //nr, nc+1
        m_varListModel->setColumnCount(2); //name and rsquare;

        for (int row = 0; row < m_varListModel->rowCount(); ++row)
        {
            std::string header = varNames[row];
            QString rVal = QString::number(rsquare.at(row),'g',3);
            QStandardItem* rowHeader = new QStandardItem(QString::fromStdString(header));
            m_varListModel->setItem(row, 0, rowHeader);
            QStandardItem* rItem = new QStandardItem(rVal);
            m_varListModel->setItem(row, 1, rItem);
        }
        m_varListModel->setHeaderData(0,Qt::Horizontal,"Name");

        QString s = QString::fromUtf8("\u00B2");
        QString header = "R"+s;//+" "+QString::number(column+1)+" bins";
        m_varListModel->setHeaderData(1,Qt::Horizontal,header);

    }catch(...){

        QMessageBox messageBox;

        messageBox.critical(0,"Error","File parsing failed!");

    }
}

void CentralWidget::resetViews(){

    m_varListModel->clear();
    m_modelListModel->clear();
    m_modelDivstyModel->clear();

    ui->viewModelCoverage->m_values.clear();
    ui->viewModelCoverage->m_highlight.clear();
    ui->viewModelCoverage->repaint();
    m_scene->clear();

    /*SAFE_DELETE(m_modelListModel);
    //SAFE_DELETE(m_modelCoverageModel);
    //SAFE_DELETE(m_modelDivstyModel);
    //SAFE_DELETE(m_varListModel);*/

    //if(m_modelListModel){delete m_modelListModel;m_modelListModel=0;}
    //if(m_modelCoverageModel){delete m_modelCoverageModel;m_modelCoverageModel=0;}
    //if(m_modelDivstyModel){delete m_modelDivstyModel;m_modelDivstyModel=0;}
    //if(m_varListModel){delete m_varListModel;m_varListModel=0;}
    QCustomPlot* p1 = ui->qcpPlotLocFitting;

    p1->clearGraphs();
    p1->clearFocus();
    p1->clearPlottables();
    p1->clearItems();
    p1->clearMask();

    p1->xAxis->setVisible(false);
    p1->yAxis->setVisible(false);
    p1->replot();

    QCustomPlot* p2 = ui->qcpPlotLocFitting;
    p2->clearGraphs();
    p2->clearFocus();
    p2->clearPlottables();
    p2->clearItems();
    p2->clearMask();
    p2->xAxis->setVisible(false);
    p2->yAxis->setVisible(false);
    p2->replot();

    QCustomPlot* p3 = ui->dataViewWidget;
    p3->clearGraphs();
    p3->clearFocus();
    p3->clearPlottables();
    p3->clearItems();
    p3->clearMask();
    p3->xAxis->setVisible(false);
    p3->yAxis->setVisible(false);
    p3->replot();
}

/**
 * @brief CentralWidget::processVarListClick
 * @param index
 */
void CentralWidget::processVarListClick(QModelIndex index){
    int row = index.row();
    int modelIdx = 0;
    ui->tabViewVariableList->selectRow(row);

    emit variableSelected(row);

    //ui->tabViewModelOveralFitting only allows single selection
    //so there should be only one row selected;
    QModelIndexList list =
            ui->tabViewModelOveralFitting->selectionModel()->selectedIndexes();

    if(!list.empty()){
        modelIdx = list.takeLast().row();
    }//if list is empty, then default modelInx is 0;
    //thus the newly created model is used;

    //qDebug()<<"processModelListClick";
    //qDebug()<<row<<" row ";
    //qDebug()<<modelIdx<<" modelIdx ";
    emit variableAndModelSelected(row,modelIdx);
}


/**
 * @brief CentralWidget::updateModelDiversityView
 * @param index
 */
void CentralWidget::processModelListClick(QModelIndex index){
    using namespace Rcpp;
    int modelIdx = index.row();
    int row = 0;
    //ui->tabViewModelOveralFitting->blockSignals(true);
    //ui->tabViewModelOveralFitting->selectRow(modelIdx);
    //ui->tabViewModelOveralFitting->blockSignals(false);

    QModelIndexList list =
            ui->tabViewVariableList->selectionModel()->selectedRows();
    if(!list.empty()){
        row = list.takeLast().row();
    }
    //qDebug()<<"processModelListClick";
    //qDebug()<<row<<" row ";
    //qDebug()<<modelIdx<<" modelIdx ";
    emit variableAndModelSelected(row,modelIdx);
}

/**
 * @brief CentralWidget::processModelListDoubleClick
 */
void CentralWidget::processModelListDoubleClick(QModelIndex index)
{
    using namespace Rcpp;

    int selectedRow = index.row();
    ui->tabViewModelOveralFitting->blockSignals(true);
    ui->tabViewModelOveralFitting->selectRow(selectedRow);
    ui->tabViewModelOveralFitting->blockSignals(false);

    if(!m_qmutex.tryLock()){
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    int rowCount;

    try{
        QString strEval;
        strEval = "model.fit <- model.list[["+QString::number(selectedRow+1)+"]];";
        m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<" success!";

        strEval = "coverages <- model.coverage(model.fit, df, type="
                +QString::number(m_modelPartitionType)+");";

        m_R.parseEval(strEval.toStdString()); //this line takes really long time; 10 -20 sec or so;
        qDebug()<<strEval<<" success!";

        strEval = "diversities <- model.diversity(coverages);";
        m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<" success!";

        strEval = "length(diversities$diversity.vec);";
        SEXP proxy = m_R.parseEval(strEval.toStdString());
        rowCount = Rcpp::as<int>(proxy);
        qDebug()<<strEval<<" success!";

        qDebug()<<"rowCount " <<rowCount;

        //SAFE_DELETE(m_modelDivstyModel);
        //if(m_modelDivstyModel) delete m_modelDivstyModel;
        m_modelDivstyModel->clear();
        m_modelDivstyModel->setRowCount(rowCount);
        m_modelDivstyModel->setColumnCount(1);
        int width = 240;
        int margin = 25;
        //if(parentContainer!=0){
        width = ui->tabViewLocModelDiversity->width();
        //}
        ui->tabViewLocModelDiversity->setColumnWidth(0,(width-margin));
        int height = (int)((width-margin)*0.618);
        qDebug()<<"height "<<height;
        ui->tabViewLocModelDiversity->verticalHeader()
                ->setDefaultSectionSize(height);

        for(int i = 0; i< rowCount; i++){
            qDebug()<<strEval<<" processing!";
            strEval = "diversities$diversity.vec[[diversities$diversity.rank["
                    +QString::number(i+1)+"]]];";
            proxy = m_R.parseEval(strEval.toStdString());
            std::vector<double> diverseScore =
                    Rcpp::as< std::vector< double > > (proxy);
            qDebug()<<strEval<<" success!";
            //diverseScores << diverseScore;

            qDebug()<<strEval<<" processing!";
            strEval = "diversities$diversity.names[[diversities$diversity.rank["
                    +QString::number(i+1)+"]]];";
            proxy = m_R.parseEval(strEval.toStdString());
            std::string scoreName =
                    Rcpp::as< std::string > (proxy);
            qDebug()<<strEval<<" success!";

            //diverseScoreNames <<QString::fromStdString(diverseScore);
            QStandardItem* contentItem =
                    this->stdVectorToQSItem(QString::fromStdString(scoreName),diverseScore);
            m_modelDivstyModel->setItem(i,0,contentItem);
        }

        //QObject* obj = ui->tabViewLocModelDiversity->parent();
        //QGroupBox* parentContainer = dynamic_cast<QGroupBox*>(obj);
        QApplication::restoreOverrideCursor();
        m_qmutex.unlock();
    }catch(...){
        QApplication::restoreOverrideCursor();
        m_qmutex.unlock();
        qDebug()<<"model coverage calculation error!";
    }
    return;
}


/**
 * @brief CentralWidget::processModelDiversityLstClick
 * @param index
 */
void CentralWidget::processModelDiversityLstClick(QModelIndex index){
    using namespace Rcpp;
    //handel single selection;
    if(Qt::ShiftModifier == QApplication::keyboardModifiers()
            ||Qt::ControlModifier==QApplication::keyboardModifiers()){
        return;
    }
    int row = index.row();

    QModelCoverageWidget* modelWdgt = ui->viewModelCoverage;

    modelWdgt->m_values.clear();
    modelWdgt->m_names.clear();
    modelWdgt->m_highlight.clear();

    if(!m_qmutex.tryLock()){
        return;
    }
    try{
        //this function only handles single selection, there is no way to check if
        //modifer key is pressed in this function
        //so, support of multiple selection can be found in the eventFilter;

        QString strEval = "as.vector(t(coverages[[diversities$diversity.rank["
                +QString::number(row+1)
                +"]]]$coverage));"; //"";
        SEXP proxy = m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<" success!";
        std::vector<double> coverageVec = Rcpp::as< std::vector<double> >(proxy);

        strEval = "coverages[[diversities$diversity.rank["
                +QString::number(row+1)
                +"]]]$name;";
        proxy = m_R.parseEval(strEval.toStdString());
        std::string coverageName = Rcpp::as< std::string >(proxy);

        strEval = "coeff.clusters <- coeff.clustering(coverages,diversities$diversity.rank["
                +QString::number(row+1)
                +"]);";
        m_R.parseEval(strEval.toStdString());

        modelWdgt->addValues(coverageVec,coverageName);
        qDebug()<<modelWdgt->size();
        //ui->viewModelCoverage->setMinimumSize(ui->viewModelCoverage->size());
        ui->scrollArea->setWidgetResizable(true);
        //modelWdgt->repaint();
        this->m_scene->clear();
        m_qmutex.unlock();
    }catch(...){
        QMessageBox message;
        message.critical(0,"Error","model coverage result was not cached properly.");
        m_qmutex.unlock();
    }
    emit modelCoverageSelected();
}

/**
 * @brief CentralWidget::stdVectorToQSItem
 * @param itemName
 * @param itemValues
 * @return a QStandardItem that contains itemName (first element)
 * and itemvalues (the remining elements) with semicolon as delimiter;
 *
 */
QStandardItem* CentralWidget::stdVectorToQSItem(const QString & itemName,
                                                const std::vector<double> & itemValues) const{
    QString content = "";
    content+=itemName;
    //content+=";";
    for(unsigned i = 0 ; i< itemValues.size(); i++){
        double temp = itemValues.at(i);
        content+=";";
        content+=QString::number(temp);
    }
    return(new QStandardItem(content));
}

/**
 * @brief CentralWidget::color
 * @param c
 * @return
 */
QColor CentralWidget::color(ModelLocFitSchema::COLOR c)
{
    switch(c){
    case ModelLocFitSchema::cl_scatter:
        return QColor(150, 150, 150, 50);
        break;
    case ModelLocFitSchema::cl_boxFillInner:
        return QColor(0,0,255,100);
        break;
    case ModelLocFitSchema::cl_boxFillMid:
        return QColor(0,0,255,180);
        break;
    case ModelLocFitSchema::cl_boxFillOuter:
        return QColor(0,0,255,50);
        break;
    case ModelLocFitSchema::cl_boxPenInner:
        return QColor(255,0,0,150);
        break;
    case ModelLocFitSchema::cl_boxPenMid:
        return QColor(255,0,0,180);
        break;
    case ModelLocFitSchema::cl_boxPenOuter:
        return QColor(255,0,0,100);
        break;
    default:
        return QColor();
        break;
    }
}


/**
 * @brief CentralWidget::updateLocalFit update the local fiting of
 * given independent variable and the corresponding number of partitions;
 * @param row indicate which variable is selected;
 * @param col indicate number of partitions;
 *  first step, grab data from m_R,
 *  second step, update qcplot view;
 *  @deprecated the view for local fiting is confusing, deprecated for now;
 */
/*void CentralWidget::updateLocalFitView(int row, int col){
    //this->ui->wdgtVarLocalFitStatus
    //varRank$names[varRank$order[1]] returns the name of the variable at 1st row;
    //varRank$lfit.list$DEBTEQ[7] returns a vector of length 7
    using namespace Rcpp;

    if(col==0){
        //clicked on row header, do nothing
        return;
    }

    try{

        if(!m_qmutex.tryLock()){
            return;
        }

        QString varIdx = QString::number(row+1);
        QString strEval = "varRank$names[varRank$order["+varIdx+"]];";

        //qDebug()<<strEval<<" processing!";
        SEXP proxy = m_R.parseEval(strEval.toStdString());
        std::string strVarName = Rcpp::as < std::string > (proxy);
        //qDebug()<<strEval<<" success!";

        QString qstrVarName = QString::fromStdString(strVarName).trimmed();
        QString qstrNumBins = QString::number(col).trimmed();
        //qDebug()<<" qstrVarName "<<qstrVarName;

        strEval = "varRank$lfit.list$"+qstrVarName+"[["+qstrNumBins+"]];";
        //qDebug()<<strEval<<" processing!";
        proxy = m_R.parseEval(strEval.toStdString());
        //qDebug()<<strEval<<" success!";
        std::vector<double> locfit = Rcpp::as< std::vector<double> >(proxy);
        //qDebug()<<strEval<<" success!";


        strEval = "varRank$ranges$"+qstrVarName+"[["+qstrNumBins+"]];";
        //qDebug()<<strEval<<" processing!";
        proxy = m_R.parseEval(strEval.toStdString());
        std::vector<std::string> locfitRanges = Rcpp::as< std::vector<std::string> >(proxy);
        //qDebug()<<strEval<<" success!";

        if(locfit.size()!=(unsigned)col){
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Unexpected R indexing error!");
        }

        if(locfitRanges.size()!=(unsigned)col){
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Unexpected R indexing error!");
        }

        QCustomPlot* customPlot =ui->qcpPlotLocFitting;

        customPlot->xAxis->setVisible(true);
        customPlot->yAxis->setVisible(true);


        // now we can modify properties of m_localFitBars:
        //m_localFitBars->setName("bar chart name");
        //customPlot->setWindowTitle("atest");

        QVector<double> keyData;
        QVector<double> valueData;
        QVector<QString> labelData;
        //double offset = 0.1;
        for(unsigned i = 0; i< locfit.size(); i++){
            keyData << (i+1);
            valueData << locfit[i];
            labelData << QString::fromStdString(locfitRanges[i]);
        }

        m_localFitBars->setData(keyData, valueData);

        m_localFitBars->rescaleValueAxis();
        m_localFitBars->setAntialiased(true);

        customPlot->xAxis->setAutoTicks(false);
        customPlot->xAxis->setAutoTickLabels(false);
        customPlot->xAxis->setTickVector(keyData);
        customPlot->xAxis->setTickVectorLabels(labelData);
        customPlot->xAxis->setTickLabelRotation(-25);

        customPlot->xAxis->setRange(0,locfit.size()+1);
        customPlot->xAxis->setLabel("Partitions");
        customPlot->xAxis->setAntialiased(true);
        customPlot->yAxis->setLabel("Relevance");
        customPlot->replot();
        m_qmutex.unlock();
    }catch(...){
        m_qmutex.unlock();
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Unexpected R indexing error!");
    }

}*/


/**
 * @brief CentralWidget::updateDataView
 * @param row
 */
void CentralWidget::updateDataView(int row)
{
    using namespace Rcpp;

    try{

        if(!m_qmutex.tryLock()){
            return;
        }

        QString varIdx = QString::number(row+1);
        QString strEval = "varRank$names[varRank$order["+varIdx+"]];";

        qDebug()<<strEval<<" processing!";
        SEXP proxy = m_R.parseEval(strEval.toStdString());
        QString varName = QString::fromStdString(
                    Rcpp::as < std::string > (proxy)).trimmed();
        qDebug()<<strEval<<" success!";

        //QString qstrVarName = (strVarName).trimmed();
        //QString qstrNumBins = QString::number(col).trimmed();
        //qDebug()<<" qstrVarName "<<qstrVarName;

        strEval = "df$"+varName+";";
        //qDebug()<<strEval<<" processing!";
        proxy = m_R.parseEval(strEval.toStdString());
        //qDebug()<<strEval<<" success!";
        QVector<double> xVals = QVector<double>::fromStdVector(
                    Rcpp::as< std::vector<double> >(proxy));
        qDebug()<<strEval<<" success!";

        strEval = "colnames(df)[ncol(df)];";
        proxy = m_R.parseEval(strEval.toStdString());
        QString yName = QString::fromStdString(Rcpp::as<std::string>(proxy));
        qDebug()<<strEval<<" success!";

        strEval = "df$"+yName+";";
        proxy = m_R.parseEval(strEval.toStdString());
        QVector<double> yVals = QVector<double>::fromStdVector(
                    Rcpp::as< std::vector<double> >(proxy));
        qDebug()<<strEval<<" success!";

        QCustomPlot* dataPlot =ui->dataViewWidget;

        dataPlot->xAxis->setVisible(true);
        dataPlot->yAxis->setVisible(true);

        dataPlot->clearGraphs();
        dataPlot->addGraph();

        QCPGraph* g = dataPlot->graph(0);
        g->setData(xVals,yVals);
        g->setAntialiased(true);
        g->setPen(color(ModelLocFitSchema::cl_scatter));

        g->setLineStyle(QCPGraph::lsNone);
        g->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

        dataPlot->rescaleAxes();
        dataPlot->xAxis->setLabel(varName);
        dataPlot->yAxis->setLabel(yName);
        dataPlot->replot();
        m_qmutex.unlock();

        /*customPlot->xAxis->setAutoTicks(false);
        customPlot->xAxis->setAutoTickLabels(false);
        customPlot->xAxis->setTickVector(keyData);
        customPlot->xAxis->setTickVectorLabels(labelData);
        customPlot->xAxis->setTickLabelRotation(-25);

        customPlot->xAxis->setRange(0,locfit.size()+1);
        customPlot->xAxis->setLabel("Partitions");
        customPlot->xAxis->setAntialiased(true);
        customPlot->yAxis->setLabel("Relevance");*/
    }catch(...){
        m_qmutex.unlock();
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Unexpected R indexing error!");
    }

}

/**
 * @brief CentralWidget::updateModelList
 */
void CentralWidget::updateModelListView(){
    using namespace Rcpp;

    //3 columns, 1:model with coefficients, 2:RMSE, 3: Rsquared;

    int tableWidth = 3; //number of columns of the table;


    int numModels = 0;

    if(!m_qmutex.tryLock()){
        return;
    }

    varNames.clear();
    QVector< std::vector<double> > varCoeffs;


    QVector<std::vector<int> > varIdx;

    QVector<double> vrmse;
    QVector<double> vrsq;

    try{
        QString strEval = "length(model.list);";
        SEXP proxy = m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<" success!";

        numModels = Rcpp::as<int>(proxy);
        //qDebug()<<numModels<<" num of models!";

        m_modelListModel->clear();
        m_modelListModel->setRowCount(numModels);
        m_modelListModel->setColumnCount(tableWidth);
        //get model RMSE, RSquared
        strEval = "status.list <- model.status(model.list,varRank);";
        m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<" success!";

        for(int i = 0; i<numModels; i++){
            strEval = "status.list[["+QString::number(i+1)+"]]$coefficients.names;";
            proxy = m_R.parseEval(strEval.toStdString());
            std::vector<std::string> names = Rcpp::as< std::vector<std::string> >(proxy);
            //qDebug()<<"coeff names "<<names.size();
            varNames<<names;
            //qDebug()<<strEval<<" success!";

            strEval = "status.list[["+QString::number(i+1)+"]]$coefficients;";
            proxy = m_R.parseEval(strEval.toStdString());
            std::vector<double> coeffs = Rcpp::as< std::vector<double> >(proxy);
            //qDebug()<<"coeffs "<<coeffs.size();
            varCoeffs<<coeffs;
            //qDebug()<<strEval<<" success!";

            strEval = "status.list[["+QString::number(i+1)+"]]$coefficients.idx;";
            proxy = m_R.parseEval(strEval.toStdString());
            std::vector<int> idx = Rcpp::as< std::vector<int> >(proxy);
            //qDebug()<<"coeff index "<<names.size();
            varIdx<<idx;
            //qDebug()<<strEval<<" success!";


            strEval = "status.list[["+QString::number(i+1)+"]]$RMSE;";
            proxy = m_R.parseEval(strEval.toStdString());
            double rmse = Rcpp::as<double>(proxy);
            vrmse<<rmse;
            //qDebug()<<strEval<<" success!";

            strEval = "status.list[["+QString::number(i+1)+"]]$RSQ;";
            proxy = m_R.parseEval(strEval.toStdString());
            double rsq = Rcpp::as<double>(proxy);
            vrsq <<rsq;
            //qDebug()<<strEval<<" success!";
        }
        //qDebug()<<"model "<<vrsq.size();

        //now, render the qtableview based on model status;
        //need another delegate for model list table view;
        //initial thoughts are to put the variables and coefficients into a QString
        //then parse it in the delegate;

        //int width = tableWidth;

        //ui->tabViewModelOveralFitting->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
        //QModelIndexList selIdx = this->ui->tabViewVariableList
        // ->selectionModel()->selectedIndexes();

        // bug here is that when only two variables are selected, crashes;
        //qDebug()<<"selIdx "<<selIdx.size()<<" "<<selIdx;

        int height = varNames.size();

        //start;size;name1:val1:idx;name2:val2:idx2;name3:val3:idx3;

        for(int row = 0; row<height; row++){
            QString qstr="";

            std::vector<std::string> vars = varNames.at(row);
            std::vector<double> coeffs = varCoeffs.at(row);
            std::vector<int> index = varIdx.at(row);

            int startIdx = 0;

            qstr+= QString::number(startIdx);
            qstr+=";";

            int rowCount = this->m_varListModel->rowCount();

            qstr+=QString::number(rowCount)+";";

            //qDebug()<<"model size"<<qstr;
            // skip the intercept for now; use i+1
            for(unsigned i = 0;i<vars.size();i++){
                //qDebug()<<"i "<<i;
                //qDebug()<<"vars.size() "<<vars.size();
                qstr+= QString::fromStdString(vars.at(i));
                qstr+= ":";
                qstr+= QString::number(coeffs.at(i));
                qstr+= ":";
                qstr+= QString::number(i);//index.at(i)
                qstr += ";";
            }
            QString result = qstr.left(qstr.length()-1);
            //qstr = qstr.remove(qstr.length()-1,1);

            //qDebug()<<"model content"<<result;

            //first column: variable name, variable coeffs
            QStandardItem* item = new QStandardItem(result);
            m_modelListModel->setItem(row, 0, item);

            //second column: rmse
            QString qstr2 = QString::number(vrmse.at(row));
            QStandardItem* item2 = new QStandardItem(qstr2);
            m_modelListModel->setItem(row, 1, item2);

            //third column: rsqured
            QString qstr3 = QString::number(vrsq.at(row));
            QStandardItem* item3 = new QStandardItem(qstr3);
            m_modelListModel->setItem(row, 2, item3);
            //
            //ui->tabViewModelOveralFitting->setRowHeight(row,200);
        }

        ui->tabViewModelOveralFitting->setColumnWidth( 0, 250 );
        ui->tabViewModelOveralFitting->setColumnWidth( 1, 100 );
        ui->tabViewModelOveralFitting->setColumnWidth( 2, 100 );
        //ui->tabViewModelOveralFitting->verticalHeader()->setResizeMode();
        ui->tabViewModelOveralFitting->verticalHeader()->setDefaultSectionSize(30);

        m_modelListModel->setHeaderData(0,Qt::Horizontal,"Model");
        m_modelListModel->setHeaderData(1,Qt::Horizontal,"RMSE");
        m_modelListModel->setHeaderData(2,Qt::Horizontal,
                                        "R"+QString::fromUtf8("\u00B2"));

        m_qmutex.unlock();
    }catch(...){
        QMessageBox message;
        message.critical(this,"Error","Model Building Failed!");
        m_qmutex.unlock();
    }
}

/**
 * @brief CentralWidget::updateModelLocFitView
 * @param varIdx default 0
 * @param modelIdx default 0
 */
void CentralWidget::updateModelLocFitView(int varIdx, int modelIdx)
{
    using namespace Rcpp;

    if(!m_qmutex.tryLock()){
        return;
    }

    QCustomPlot* p = ui->qcpPlotLocFitting;
    try{

        QString strEval = "varRank$names[varRank$order["
                +QString::number(varIdx+1)+"]];";
        SEXP proxy = m_R.parseEval(strEval.toStdString());
        QString varName = QString::fromStdString(
                    Rcpp::as < std::string > (proxy)).trimmed();

        strEval = "boxplot <- locfit.boxplot(df,model.list=model.list,model.idx="
                +QString::number(modelIdx+1)+",v2=\""+varName+"\",cut="
                +QString::number(m_varPartition)+");";

        m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<" success!";

        strEval = "is.null(boxplot);";
        bool notExistModel = Rcpp::as<bool>(m_R.parseEval(strEval.toStdString()));
        qDebug()<<strEval<<" success!";

        qDebug()<<notExistModel;

        if(notExistModel){m_qmutex.unlock();return;}

        //qDebug()<<strEval<<" processing!";
        int ncol = Rcpp::as<int>(m_R.parseEval("ncol(boxplot$stats);"));
        int nrow = Rcpp::as<int>(m_R.parseEval("nrow(boxplot$stats);"));

        double xmin = Rcpp::as<double>(m_R.parseEval("boxplot$xmin;"));
        double xmax = Rcpp::as<double>(m_R.parseEval("boxplot$xmax"));

        double ymin = Rcpp::as<double>(m_R.parseEval("boxplot$ymin;"));
        double ymax = Rcpp::as<double>(m_R.parseEval("boxplot$ymax"));

        p->clearPlottables();

        QVector<double> keyData;
        //QVector<QString> keyLabel;

        //QVector<QCPGraph* > boxplots;

        for(int i = 0; i<ncol; i++)
        {
            strEval = "boxplot$ranges["+QString::number(i+1)+"]";
            proxy = m_R.parseEval(strEval.toStdString());
            double key = Rcpp::as<double>(proxy);

            strEval = "boxplot$stats[,"+QString::number(i+1)+"]";
            proxy = m_R.parseEval(strEval.toStdString());
            QVector<double> vals = QVector<double>::fromStdVector(
                        Rcpp::as< std::vector<double> >(proxy));
            qDebug()<<strEval<<" success!";
            if(vals.size()!=5){
                p->clearFocus();
                p->clearGraphs();
                p->clearPlottables();
                p->clearItems();
                p->clearMask();
                m_qmutex.unlock();
                return;
            }
            QCPGraph* solidLine = new QCPGraph(p->xAxis,p->yAxis);
            solidLine->setPen(color(ModelLocFitSchema::cl_boxPenInner));
            solidLine->addData(key,vals.at(1));
            solidLine->addData(key,vals.at(3));
            p->addPlottable(solidLine);

            QCPGraph* dashLine1 = new QCPGraph(p->xAxis,p->yAxis);
            QCPGraph* dashLine2 = new QCPGraph(p->xAxis,p->yAxis);
            dashLine1->setPen(color(ModelLocFitSchema::cl_boxPenOuter));
            dashLine2->setPen(color(ModelLocFitSchema::cl_boxPenOuter));
            dashLine1->addData(key,vals.at(0));
            dashLine1->addData(key,vals.at(1));
            dashLine2->addData(key,vals.at(3));
            dashLine2->addData(key,vals.at(4));
            p->addPlottable(dashLine1);
            p->addPlottable(dashLine2);
            //keyLabel << QString::fromStdString(key);
            keyData << key;
        }
        QVector<QCPGraph*> graphs;
        QVector<QCPGraph*> scatters;
        for(int i = 0; i<nrow; i++){//nrow is always five;
            strEval = "boxplot$stats.current["+QString::number(i+1)+",]";
            proxy = m_R.parseEval(strEval.toStdString());
            QVector<double> vals = QVector<double>::fromStdVector(
                        Rcpp::as< std::vector<double> >(proxy));
            QCPGraph* curve = new QCPGraph(p->xAxis,p->yAxis);
            curve->addData(keyData,vals);
            //curve->setLineStyle(QCPGraph::lsNone);
            curve->setPen(QPen(QColor(0,0,0,0))); //no color;
            //curve->rescaleAxes();
            p->addPlottable(curve);
            graphs <<curve;
            strEval = "boxplot$stats["+QString::number(i+1)+",]";
            proxy = m_R.parseEval(strEval.toStdString());
            QVector<double> valsnew = QVector<double>::fromStdVector(
                        Rcpp::as< std::vector<double> >(proxy));
            QCPGraph* scatter = new QCPGraph(p->xAxis,p->yAxis);
            scatter->addData(keyData,valsnew);
            //QColor(255,0,0,150)
            scatter->setPen(QPen(color(ModelLocFitSchema::cl_boxPenInner)));
            scatter->setLineStyle(QCPGraph::lsNone);
            scatter->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));
            //scatter->rescaleAxes();
            p->addPlottable(scatter);
            scatters << scatter;
        }
        for(int i = 0; i<graphs.size()/2; i++){
            QCPGraph* channelFill = graphs.at(graphs.size()-1-i);
            graphs.at(i)->setChannelFillGraph(channelFill);
        }
        graphs.at(0)->setBrush(color(ModelLocFitSchema::cl_boxFillOuter));//QColor(0,0,255,50)
        graphs.at(1)->setBrush(color(ModelLocFitSchema::cl_boxFillInner));//QColor(0,0,255,100)
        graphs.at(2)->setPen(color(ModelLocFitSchema::cl_boxFillMid));//QColor(0,0,255,180)
        graphs.at(2)->setLineStyle(QCPGraph::lsLine);
        scatters.at(2)->setPen(QPen(color(ModelLocFitSchema::cl_boxPenMid)));//QColor(255,0,0,200)
        scatters.at(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 5));

        p->xAxis->setRange(xmin,xmax);
        p->yAxis->setRange(ymin,ymax);

        if(this->m_modelListModel->rowCount()<1&&
               this->m_modelListModel->columnCount()<1){
            qDebug()<<"model comparison error: modelListModel rowCount <1!";
            throw;
        }
        std::vector<std::string> names = varNames.at(modelIdx);
        QString text;
        text+="{";
        for(unsigned i = 1; i < names.size(); i++){
            text +=QString::fromStdString(names.at(i));
            text += ", ";
        }
        text.chop(2);
        //text.remove(text.length()-1,1);
        text+="}";
        //QStandardItem* item = this->m_modelListModel->item(modelIdx,0);
        //QString text = GlyphDelegate::modelTableItemToText(item->text());
        p->xAxis->setLabel(""+varName);
        p->yAxis->setLabel("Model on "+text+"\nvs\nModel on {"+varName+"}");
        qDebug()<<ymin<<ymax<<xmin<<xmax;
        p->rescaleAxes();
        p->xAxis->setVisible(true);
        p->yAxis->setVisible(true);
        p->replot();

    }catch(...){
        qDebug()<<"model comparison error!";
        m_qmutex.unlock();
    }
    m_qmutex.unlock();
}

/**
 * @brief CentralWidget::updateCoverageView
 */
void CentralWidget::updateCoverageView(){
    //render the color grid map for the smoothing partitioning;
    //read modelCoverageTable, parse the string;
    //remove the last two values; old lower bound and upper bound
    //add two new values; new lower bound and upper bound
    this->ui->viewModelCoverage->m_lBoundF = this->m_coverageLBound;
    this->ui->viewModelCoverage->m_uBoundF = this->m_coverageUBound;
    this->ui->viewModelCoverage->repaint();
}

/**
 * @brief CentralWidget::updateCoverageViewHighlight
 * @param highlightIdx
 */
void CentralWidget::updateCoverageViewHighlight(QVector<int> highlightIdx)
{
    //qDebug()<<"highlight size" << highlightIdx.size();
    //qDebug()<<"" << highlightIdx;
    ui->viewModelCoverage->setHighlightIdx(highlightIdx);
    ui->viewModelCoverage->repaint();
}


/**
 * @brief CentralWidget::eventFilter
 * @param object
 * @param event
 * @return
 */
bool CentralWidget::eventFilter(QObject* obj, QEvent* event){
    using namespace Rcpp;

    mouseEventFilter(obj,event);

    //varTableHoverEventFilter(obj,event);

    if(event->type()==QEvent::KeyRelease){
        QKeyEvent* pKeyEvent=static_cast<QKeyEvent*>(event);
        if(pKeyEvent->key()==Qt::Key_Shift ||
                pKeyEvent->key()==Qt::Key_Control){
            if(m_mouseOnVarTable){
                return varTableEventFilter(obj,event);
            }
            if(m_mouseOnModelDiversTable){
                return modelDiversityTableEventFilter(obj,event);
            }
        }
    }
    return QWidget::eventFilter(obj,event);
}


/**
 * @brief CentralWidget::mouseEventFilter
 * @param obj
 * @param event
 */
void CentralWidget::mouseEventFilter(QObject* obj, QEvent* event){
    if(event->type()==QEvent::MouseMove){
        //moues event;
        QMouseEvent * me = static_cast<QMouseEvent*>(event);
        m_mouseOnVarTable=false;
        m_mouseOnModelDiversTable = false;

        if(obj==ui->tabViewVariableList){
            QTableView* table = static_cast<QTableView*>(obj);
            if(table->rect().contains(me->pos())){
                this->m_mouseOnVarTable=true;
            }
        }
        if(obj==ui->tabViewVariableList->viewport()){
            QWidget* tableVport = static_cast<QWidget*>(obj);
            if(tableVport->rect().contains(me->pos())){
                this->m_mouseOnVarTable=true;
            }
        }
        if(obj==ui->tabViewLocModelDiversity){
            QTableView* table = static_cast<QTableView*>(obj);
            if(table->rect().contains(me->pos())){
                this->m_mouseOnModelDiversTable=true;
            }
        }
        if(obj==ui->tabViewLocModelDiversity->viewport()){
            QWidget* tableVport = static_cast<QWidget*>(obj);
            if(tableVport->rect().contains(me->pos())){
                this->m_mouseOnModelDiversTable=true;
            }
        }
    }
}

/*void CentralWidget::varTableHoverEventFilter(QObject *obj, QEvent *event)
{
    using namespace Rcpp;

    if(obj==ui->tabViewVariableList || obj == ui->tabViewVariableList->viewport()){
        //QTableView* table = static_cast<QTableView*>(obj);
        //if(table->rect().contains(me->pos())){
        //this->m_mouseOnVarTable=true;
        //}
        if(varTableCurrentRow==ui->tabViewVariableList->m_currentRow){
            return;
        }else{
            varTableCurrentRow = ui->tabViewVariableList->m_currentRow;
        }

        int v2 = varTableCurrentRow;
        if(v2==-1){
            return;
        }
        QModelIndexList list;
        list = ui->tabViewVariableList->selectionModel()->selectedIndexes();
        if(list.empty()){
            return;
        }
        int v1 = list.takeLast().row();

        if(!m_qmutex.tryLock()){
            return;
        }
        try{
            QString strEval = "varRank$names[varRank$order["+QString::number(v1+1)+"]];";

            qDebug()<<strEval<<" processing!";
            SEXP proxy = m_R.parseEval(strEval.toStdString());
            QString var1 = QString::fromStdString(
                        Rcpp::as < std::string > (proxy)).trimmed();

            //QString varIdx = ;
            strEval = "varRank$names[varRank$order["+QString::number(v2+1)+"]];";

            qDebug()<<strEval<<" processing!";
            proxy = m_R.parseEval(strEval.toStdString());
            QString var2 = QString::fromStdString(
                        Rcpp::as < std::string > (proxy)).trimmed();
            //locfit.boxplot <- function(df,v1,v2=NULL,cut=10,type=1){

            strEval = "boxplot <- locfit.boxplot(df,v1=\""+var1+"\",v2=\""+var2+"\")";
            m_R.parseEval(strEval.toStdString());

            strEval = "ncol(boxplot$stats);";
            int ncol = Rcpp::as<int>(m_R.parseEval(strEval.toStdString()));

            ui->qcpPlotLocFitting->clearPlottables();

            for(int i = 0; i<ncol; i++){

                strEval = "boxplot$stats[,"+QString::number(i+1)+"]";
                proxy = m_R.parseEval(strEval.toStdString());
                QVector<double> vals = QVector<double>::fromStdVector(
                            Rcpp::as< std::vector<double> >(proxy));
                qDebug()<<vals;
                QCPStatisticalBox * box = new QCPStatisticalBox(ui->qcpPlotLocFitting->xAxis,
                                                                ui->qcpPlotLocFitting->yAxis);
                assert(vals.size()==5);
                box->setData(i+1,vals.at(0),vals.at(1),vals.at(2),vals.at(3),vals.at(4));
                ui->qcpPlotLocFitting->addPlottable(box);
            }
            ui->qcpPlotLocFitting->xAxis->setVisible(true);
            ui->qcpPlotLocFitting->yAxis->setVisible(true);
            ui->qcpPlotLocFitting->rescaleAxes();
            ui->qcpPlotLocFitting->replot();
            qDebug()<<var1<<" "<<var2;

            m_qmutex.unlock();
        }catch(...){
            qDebug()<<"hover failed";
            m_qmutex.unlock();
        }
    }
}
*/


/**
 * @brief CentralWidget::varTableEventFilter
 * @param obj
 * @param event
 * @return
 */
bool CentralWidget::varTableEventFilter(QObject* obj, QEvent* event){
    using namespace Rcpp;
    QKeyEvent* pKeyEvent=static_cast<QKeyEvent*>(event);
    if(pKeyEvent->key()==Qt::Key_Shift ||
            pKeyEvent->key()==Qt::Key_Control){
        qDebug()<<"ctrl shift pressed";
        if(!m_qmutex.tryLock())
            return true;
        try{
            QModelIndexList selIdx = this->ui->tabViewVariableList
                    ->selectionModel()->selectedIndexes();
            NumericVector rowSelected(selIdx.size());
            for(int i = 0;i<selIdx.size(); i++){
                rowSelected[i] = selIdx.at(i).row();
                //qDebug()<<"row selected "<<selIdx.at(i).row();
            }

            m_R["varRank.rows"] = rowSelected;
            //build model;
            QString strEval = "model.vars <- varRank$order[unique(varRank.rows+1)];"
                    "model <- model.train(model.vars, df);"
                    "if(!exists(\"model.list\")){model.list=list();"
                    "model.list <- c(list(model),model.list);"
                    "}else{"
                    "model.list <- c(list(model),model.list);"
                    "};";
            m_R.parseEval(strEval.toStdString());
            qDebug()<<strEval<<" success!";
            m_qmutex.unlock();
        }catch(...){
            m_qmutex.unlock();
            QMessageBox messageBox;
            messageBox.critical(0,"Error","model building failed!");
            return false;
        }

        emit modelListReady();
    }
    return true;
}


/**
 * @brief CentralWidget::modelDiversityTableEventFilter
 * @param obj
 * @param event
 * @return
 */
bool CentralWidget::modelDiversityTableEventFilter(QObject* obj, QEvent* event){
    using namespace Rcpp;
    //int row = index.row();
    //set the row and column rect size using the table size;

    //int rowCount = std::ceil((float)row/2);
    //the displayable layout is fixed: 2 rows and 3 columns,
    //as we only display one grid colormap here responding to single selection;

    if(!m_qmutex.tryLock()){
        return QWidget::eventFilter(obj,event);
    }
    try{
        //this->m_modelCoverageModel->clear();

        QModelCoverageWidget* modelWdgt = ui->viewModelCoverage;

        modelWdgt->m_values.clear();
        modelWdgt->m_names.clear();
        modelWdgt->m_highlight.clear();

        QModelIndexList qidxList =
                ui->tabViewLocModelDiversity->selectionModel()->selectedIndexes();
        if(qidxList.empty())
            return true;
        //        float listSize = (float)qidxList.size();

        //        int tmp = (int)std::ceil(listSize/2);
        //        int rowCount = 3>tmp?3:tmp;
        //        m_modelCoverageModel->setRowCount(rowCount);

        QString strEval = "";
        SEXP proxy = 0;

        strEval = "cluster.var.list<-NULL;";
        m_R.parseEval(strEval.toStdString());

        for(int i = 0; i<qidxList.size();i++){
            int row = qidxList.at(i).row();
            strEval = "cluster.var.list <- c(cluster.var.list, diversities$diversity.rank["
                    +QString::number(row+1)
                    +"]);";
            m_R.parseEval(strEval.toStdString());
            qDebug()<<strEval<<" success";

            strEval = "as.vector(t(coverages[[diversities$diversity.rank["
                    +QString::number(row+1)
                    +"]]]$coverage));";
            proxy = m_R.parseEval(strEval.toStdString());
            qDebug()<<strEval<<" success!";
            std::vector<double> coverageVec = Rcpp::as< std::vector<double> >(proxy);

            strEval = "coverages[[diversities$diversity.rank["
                    +QString::number(row+1)
                    +"]]]$name;";
            proxy = m_R.parseEval(strEval.toStdString());
            std::string coverageName = Rcpp::as< std::string >(proxy);

            modelWdgt->addValues(coverageVec,coverageName);
            //qDebug()<<modelWdgt->size();
            //ui->viewModelCoverage->setMinimumSize(ui->viewModelCoverage->size());
            ui->scrollArea->setWidgetResizable(true);
            //modelWdgt->repaint();
        }
        strEval = "coeff.clusters <- coeff.clustering(coverages,cluster.var.list);";
        m_R.parseEval(strEval.toStdString());
        this->m_scene->clear();
        m_qmutex.unlock();
    }catch(...){
        QMessageBox message;
        message.critical(0,"Error","Model Coverage Multi-selection Exception!");
        m_qmutex.unlock();
    }
    emit modelCoverageSelected();
    return true;
}

/**
 * @brief CentralWidget::updateGraphicsView
 */
void CentralWidget::updateGraphicsView(){
    //SAFE_DELETE(m_itemRoot);
    m_itemRoot = new QTreeNodeItem(m_R,0,false,this,m_scene);
    connect(ui->viewModelCoverage,SIGNAL(cellHovered(int)),
            this->m_itemRoot,SLOT(updateHighlightModel(int)),Qt::UniqueConnection);
    m_scene->addItem(m_itemRoot);
}

void CentralWidget::updateRepModelCoeffs(QString args)
{
    ui->lblModelCoeffs->clear();
    ui->lblModelCoeffs->setStyleSheet("QLabel { color : red; }");
    ui->lblModelCoeffs->setText(args);
}

/**
 * @brief CentralWidget::on_lBoundSlider_valueChanged
 * @param value
 */
void CentralWidget::on_lBoundSlider_valueChanged(int value)
{
    //maintain difference of 9;
    int valDiff = 9;
    int uValue = ui->uBoundSlider->value();
    if(value+valDiff >= uValue){
        ui->uBoundSlider->blockSignals(true);
        ui->uBoundSlider->setValue(value+valDiff);
        ui->uBoundSlider->blockSignals(false);
    }
    uValue = ui->uBoundSlider->value();
    //range is 0 to 100;
    //update
    this->m_coverageLBound = ((float)value)/100;
    this->m_coverageUBound = ((float)uValue)/100;

    ui->lblColorLBound->setText(QString::number(this->m_coverageLBound));
    ui->lblColorUBound->setText(QString::number(this->m_coverageUBound));
    emit colorSchemaChanged();
}

/**
 * @brief CentralWidget::on_uBoundSlider_valueChanged
 * @param value
 */
void CentralWidget::on_uBoundSlider_valueChanged(int value)
{
    //maintain difference of 9;
    int valDiff = 9;
    int lValue = ui->lBoundSlider->value();
    if(value-valDiff <= lValue){
        ui->lBoundSlider->blockSignals(true);
        ui->lBoundSlider->setValue(value-valDiff);
        ui->lBoundSlider->blockSignals(false);
    }
    lValue = ui->lBoundSlider->value();
    this->m_coverageLBound = ((float)lValue)/100;
    this->m_coverageUBound = ((float)value)/100;

    ui->lblColorLBound->setText(QString::number(this->m_coverageLBound));
    ui->lblColorUBound->setText(QString::number(this->m_coverageUBound));

    emit colorSchemaChanged();
}

/**
 * @brief CentralWidget::on_edt_indVarFilter_textChanged
 * @param arg1
 */
void CentralWidget::on_edt_indVarFilter_textChanged(const QString &arg1)
{
    QString filter = arg1;
    for( int i = 0; i < m_varListModel->rowCount(); ++i )
    {
        bool match = false;
        QStandardItem *item = m_varListModel->item(i, 0);
        if(item->text().toLower().contains(filter.toLower()) )
        {
            match = true;
        }
        ui->tabViewVariableList->setRowHidden(i,!match);
    }
}

/**
 * @brief CentralWidget::on_pb_clearIndVarFilter_clicked
 */
void CentralWidget::on_pb_clearIndVarFilter_clicked()
{
    ui->edt_indVarFilter->setText("");
}

void CentralWidget::on_edt_modelDivFilter_textChanged(const QString &arg1)
{
    QString filter = arg1;
    for( int i = 0; i < m_varListModel->rowCount(); ++i )
    {
        bool match = false;
        QStandardItem *item =m_modelDivstyModel->item(i, 0);
        qDebug()<<item->text();
        QStringList cellContents = item->text().split(";");
        QString targetTxt = cellContents.takeFirst();
        if(targetTxt.toLower().contains(filter.toLower()) )
        {
            match = true;
        }
        ui->tabViewLocModelDiversity->setRowHidden(i,!match);
    }
}

void CentralWidget::on_pb_clearModelDivFilter_clicked()
{
    ui->edt_modelDivFilter->setText("");
}

void CentralWidget::on_sliderCutNum_valueChanged(int value)
{
    this->m_varPartition = value;
    int varId = 0;
    int modelId = 0;

    QModelIndexList vars =
            ui->tabViewVariableList->selectionModel()->selectedRows();
    if(vars.empty()){
        return;
    }
    varId = vars.takeLast().row();
    if(m_modelListModel->rowCount()<1){
        return;
    }
    QModelIndexList models =
            ui->tabViewModelOveralFitting->selectionModel()->selectedRows();
    if(!models.empty()){
        modelId = models.takeLast().row();
    }

    emit variableAndModelSelected(varId,modelId);
}
