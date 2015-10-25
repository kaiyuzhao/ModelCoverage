### ModelCoverage

An interactive visual exploration tool based on [XmdvTool](http://davis.wpi.edu/xmdv/) for linear model refinment

#### Project Description

Linear models are commonly used to identify trends in data. While it is an easy task to build linear models using pre-selected variables, it is challenging to select the best variables from a large number of alternatives. Most metrics for selecting variables are global in nature, and thus not useful for identifying local patterns. In this work, we present an integrated framework with visual representations that allows the user to incrementally build and verify models in three model spaces that support local pattern discovery and summarization: model complementarity, model diversity, and model representivity. Visual representations are designed and implemented for each of the model spaces. Our visualizations enable the discovery of complementary variables, i.e., those that perform well in modeling different subsets of data points. They also support the isolation of local models based on a diversity measure. Furthermore, the system integrates a hierarchical representation to identify the outlier local trends and the local trends that share similar directions in the model space.

#### Dependencies

1. Qt 4.8.1
2. R 2.15.1
3. Rtools 2.15
4. RInside 0.2.10

#### How to build

It is strongly recommended to compile/build all the dependencies from source code using your system default c++ compiler, otherwise there will be linkage errors.

##### Windows

1. Install [R 2.15.1](http://cran.us.r-project.org/) and set enviornment variable R_HOME to the installation directory
2. Install [Qt 4.8.1](https://download.qt.io/archive/qt/4.8/4.8.1/) to your system
3. Download [Rtools 2.15](https://cran.r-project.org/bin/windows/Rtools/) and install it, you may want to use the compiler comes with Rtools as your default compiler for this project. Then rebuild Qt libraries using it.
4. Download [RInside](http://dirk.eddelbuettel.com/code/rinside.html) and follow the [Embedding Qt Example](http://dirk.eddelbuettel.com/blog/2011/03/25#rinside_and_qt) to setup your Qt.
5. Buid this project using Qt Creater

##### Mac

1. Download [R 2.15.1](http://cran.us.r-project.org/) source code and compile it using your system compiler
2. Download [Qt 4.8.1](https://download.qt.io/archive/qt/4.8/4.8.1/) to your system
4. Download [RInside](http://dirk.eddelbuettel.com/code/rinside.html) and follow the [Embedding Qt Example](http://dirk.eddelbuettel.com/blog/2011/03/25#rinside_and_qt) to setup your Qt.
5. Buid the project
