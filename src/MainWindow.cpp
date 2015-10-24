#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>
//#include <QString>

MainWindow::MainWindow(RInside & R, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_R(R)
{
    ui->setupUi(this);
    m_centralWdgt = new CentralWidget(m_R,this);
    this->setCentralWidget(m_centralWdgt);
    //this->m_rankRSqrsTableWdgt = new RankRSqrsTableWdgt(m_R);
    //QVBoxLayout *boxlyt = new QVBoxLayout(m_rankRSqrsTableWdgt);
    //boxlyt->addWidget(m_rankRSqrsTableWdgt);

    //QString workingDir = QDir::currentPath();
    //qDebug()<<workingDir;

    this->connect(this,SIGNAL(dataLoaded()),
                  this->m_centralWdgt,SLOT(dataLoaded()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief when the "open" item is clicked
 * process open action in R, load data into a data.frame in R;
 * using r function, read.csv
 */
void MainWindow::on_actionOpen_triggered()
{
    using namespace Rcpp;
    //qDebug()<<"test open";
    //using namespace RInside;
    //using namespace Rcpp;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File..."),QString(),
                                                    tr("CSV-Files(*.csv);;All Files (*)"));

    if(fileName.isEmpty()){
        //QMessageBox messageBox;
        //messageBox.critical(0,"Error","File "+fileName+" is corruped!");
        return;
    }

    if(!m_qmutex.tryLock())
        return;
    std::string linuxFileName = fileName.toStdString();
    std::replace(linuxFileName.begin(), linuxFileName.end(), '\\', '/');
    //load data into R environment;

    try{
        QString strEval =
                "df<-read.csv(file=\""+QString::fromStdString(linuxFileName)+"\");";
        m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<"success!";

        QString workingDir = QDir::currentPath();
        std::string dir = workingDir.toStdString();
        std::replace(dir.begin(), dir.end(), '\\', '/');
        strEval = "setwd(\""+QString::fromStdString(dir)+"\")";
        qDebug()<<strEval<<"processing...";
        m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<"success!";

        strEval = "source(\"utility.r\");";
        qDebug()<<strEval<<"processing...";
        m_R.parseEval(strEval.toStdString());
        qDebug()<<strEval<<"success!";

        m_qmutex.unlock();
        emit  this->dataLoaded();
    }catch(...){
        m_qmutex.unlock();
        QMessageBox messageBox;
        messageBox.critical(0,"Error","File "+fileName+" parsing failed!");
        return;
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}
