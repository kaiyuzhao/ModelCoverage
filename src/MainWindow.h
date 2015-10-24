#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>

#include "CentralWidget.h"

#include <RInside.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(RInside & R, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

signals:
    void dataLoaded();

private:
    Ui::MainWindow *ui;
    RInside & m_R;
    QMutex m_qmutex;

    //views
    CentralWidget* m_centralWdgt;

};

#endif // MAINWINDOW_H
