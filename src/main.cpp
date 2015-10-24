#include "MainWindow.h"
#include <QApplication>
#include "RInside.h"

int main(int argc, char *argv[])
{

    RInside R;
    QApplication a(argc, argv);
    MainWindow w(R);
    w.showFullScreen();
    //w.show();
    
    return a.exec();
}
