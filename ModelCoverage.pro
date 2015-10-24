#-------------------------------------------------
#
# Project created by QtCreator 2013-10-18T17:05:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModelCoverage


# this config is very important, it must be added to every Qt project with R integration;
CONFIG += console


TEMPLATE = app


SOURCES += src/main.cpp \
    src/MainWindow.cpp \
    src/qcustomplot/qcustomplot.cpp \
    src/color/ColorManager.cpp \
    src/color/ColorMap.cpp \
    src/color/ColorRamp.cpp \
    src/views/CentralWidget.cpp \
    src/views/qtsubclasses/GlyphDelegate.cpp \
    src/views/qtsubclasses/QCustomTableView.cpp \
    src/views/qtsubclasses/VariableListDelegate.cpp \
    src/views/qtsubclasses/ModelCoverageDelegate.cpp \
    src/views/qtsubclasses/ColorSchemaWidget.cpp \
    src/views/qtsubclasses/QModelCoverageWidget.cpp \
    src/views/qtsubclasses/QTreeNodeItem.cpp \
    src/views/qtsubclasses/QTreeNodeEdge.cpp \
    src/views/qtsubclasses/QCPboxPlot.cpp \
    src/views/qtsubclasses/ModelDiversityDelegate.cpp

HEADERS  += src/MainWindow.h \
    src/qcustomplot/qcustomplot.h \
    src/color/ColorAssign.h \
    src/color/ColorManager.h \
    src/color/ColorMap.h \
    src/color/ColorRamp.h \
    src/color/RGBt.h \
    src/views/CentralWidget.h \
    src/views/qtsubclasses/GlyphDelegate.h \
    src/views/qtsubclasses/QCustomTableView.h \
    src/views/qtsubclasses/VariableListDelegate.h \
    src/views/qtsubclasses/ModelCoverageDelegate.h \
    src/views/qtsubclasses/ColorSchemaWidget.h \
    src/views/qtsubclasses/QModelCoverageWidget.h \
    src/views/qtsubclasses/QTreeNodeItem.h \
    src/views/qtsubclasses/QTreeNodeEdge.h \
    src/views/qtsubclasses/QCPboxPlot.h \
    src/views/qtsubclasses/ModelDiversityDelegate.h

FORMS    += src/MainWindow.ui \
    src/views/CentralWidget.ui

INCLUDEPATH += src \
    src/color \
    src/qcustomplot \
    src/views \
    src/views/qtsubclasses


##----------------------start of configuring R environment-----------------------
## comment this out if you need a different version of R,
## and set set R_HOME accordingly as an environment variable
R_HOME =                $$system(R RHOME)

## include headers and libraries for R
RCPPFLAGS =             $$system($$R_HOME/bin/R CMD config --cppflags)
RLDFLAGS =              $$system($$R_HOME/bin/R CMD config --ldflags)
RBLAS =                 $$system($$R_HOME/bin/R CMD config BLAS_LIBS)
RLAPACK =               $$system($$R_HOME/bin/R CMD config LAPACK_LIBS)

## if you need to set an rpath to R itself, also uncomment
RRPATH =               -Wl,-rpath,$$R_HOME/lib

## include headers and libraries for Rcpp interface classes
RCPPINCL =              $$system($$R_HOME/bin/Rscript -e Rcpp:::CxxFlags\(\))
RCPPLIBS =              $$system($$R_HOME/bin/Rscript -e Rcpp:::LdFlags\(\))

## for some reason when building with Qt we get this each time
## so we turn unused parameter warnings off
RCPPWARNING =           -Wno-unused-parameter

## include headers and libraries for RInside embedding classes
RINSIDEINCL =           $$system($$R_HOME/bin/Rscript -e RInside:::CxxFlags\(\))
RINSIDELIBS =           $$system($$R_HOME/bin/Rscript -e RInside:::LdFlags\(\))

## compiler etc settings used in default make rules
QMAKE_CXXFLAGS += $$RCPPWARNING $$RCPPFLAGS $$RINSIDEINCL $$RCPPINCL
QMAKE_LIBS += $$RLDFLAGS $$RBLAS $$RLAPACK $$RINSIDELIBS $$RCPPLIBS
##------------------------end of configuring R environment-------------------



Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui
