#include "QTreeNodeItem.h"
#include "QTreeNodeEdge.h"
#include "CentralWidget.h"
#include <QStyleOptionGraphicsItem>
#include <QGraphicsItem>
#include <QString>

QTreeNodeItem::QTreeNodeItem(RInside & r, QTreeNodeItem* parent,
                             bool isLeftChild, CentralWidget * main,
                             QGraphicsScene* scene):
    QGraphicsObject(),
    m_x(0),m_y(0),m_width(330),m_height(150),
    m_xOffset(0),m_yOffset(0),m_R(r),
    m_main(main), m_scene(scene)
{
    qRegisterMetaType< QVector<int> >("QVector<int>");

    m_parent = parent;
    m_leftChild = 0;
    m_rightChild = 0;
    //m_edge = 0;
    m_isLeftChild = isLeftChild;

    m_rootEval = "tmp.tree <- coeff.clusters$clust.tree;";

    if(parent==0){
        m_branchEval = "branch <- tmp.tree";
    }else{
        if(m_isLeftChild){
            m_branchEval = parent->m_branchEval+"[[1]]";
        }else{
            m_branchEval = parent->m_branchEval+"[[2]]";
        }
    }

    m_memIdxEval = "cluster.memIdx <- as.vector(order.dendrogram(branch))"
            "[order(as.vector(order.dendrogram(branch)))];";

    initializeCluster();
    connect(this,SIGNAL(nodeSelected(QVector<int>)),
            m_main,SLOT(updateCoverageViewHighlight(QVector< int >)));

    connect(this,SIGNAL(localModelHighlighted(QString)),
            m_main,SLOT(updateRepModelCoeffs(QString)));

    this->setFlag(QGraphicsItem::ItemIsSelectable,true);
    this->setAcceptHoverEvents(true);
}

QTreeNodeItem::~QTreeNodeItem()
{
//    disconnect(this,SIGNAL(localModelHighlighted(QString)),
//            m_main,SLOT(updateRepModelCoeffs(QString)));
//    disconnect(this,SIGNAL(nodeSelected(QVector< int >)),
//               m_main,SLOT(updateCoverageViewHighlight(QVector< int >)));
    SAFE_DELETE(m_leftChild);
    SAFE_DELETE(m_rightChild);
    //SAFE_DELETE(m_edge);
}

void QTreeNodeItem::initializeCluster(){
    using namespace Rcpp;
    //compute the QVectors, if not computed yet,
    if(this->m_members.empty()){
        if(!m_main->m_qmutex.tryLock())
            return;
        try{
            QString strEval = m_rootEval+ m_branchEval+";"+m_memIdxEval;

            m_R.parseEval(strEval.toStdString());
            qDebug()<<strEval<<" success!";

            strEval = //"print(as.vector(rbind(cluster.memIdx*2-1,cluster.memIdx*2)));"
                    "coeff.clusters$index[as.vector(rbind(cluster.memIdx*2-1,cluster.memIdx*2))];";

            SEXP proxy = m_R.parseEval(strEval.toStdString());

            this->m_members = Rcpp::as< std::vector<int> >(proxy);

            m_clustIdx = QVector<int>::fromStdVector(
                        Rcpp::as<std::vector<int> >(m_R.parseEval("cluster.memIdx;")));


            strEval = "fivenum <- coeff.fivenum(coeff.clusters,cluster.memIdx);";
            m_R.parseEval(strEval.toStdString());
            qDebug()<<strEval<<" success!";

            m_coeff_min = QVector<double>::fromStdVector(
                        Rcpp::as<std::vector<double> >(
                            m_R.parseEval("fivenum$coeff.norm[1,];")));

            m_coeff_lq = QVector<double>::fromStdVector(
                        Rcpp::as<std::vector<double> >(
                            m_R.parseEval("fivenum$coeff.norm[2,];")));
            m_coeff_mid = QVector<double>::fromStdVector(
                        Rcpp::as<std::vector<double> >(
                            m_R.parseEval("fivenum$coeff.norm[3,];")));
            m_coeff_hq = QVector<double>::fromStdVector(
                        Rcpp::as<std::vector<double> >(
                            m_R.parseEval("fivenum$coeff.norm[4,];")));
            m_coeff_max = QVector<double>::fromStdVector(
                        Rcpp::as<std::vector<double> >(
                            m_R.parseEval("fivenum$coeff.norm[5,];")));

            m_names = Rcpp::as<std::vector<std::string> >(
                            m_R.parseEval("fivenum$coeff.name;"));

            m_coeff_mid_true_val = QVector<double>::fromStdVector(
                        Rcpp::as<std::vector<double> >(
                            m_R.parseEval("fivenum$coeff[3,];")));
            qDebug()<<strEval<<" success!";

            QString s;

            for(unsigned i = 0; i<this->m_names.size()-1;i++){
                s+=QString::fromStdString(m_names.at(i+1));
                s+=":";
                s+=QString::number(m_coeff_mid_true_val.at(i+1),'g',3);
                s+=" ";
            }
            this->setToolTip(s);
            m_main->m_qmutex.unlock();
        }catch(...){
            qDebug()<<"cluster index parse error!";
            m_main->m_qmutex.unlock();
        }
    }
}


void QTreeNodeItem::addBranches()
{
    if(this->m_members.size()<4)
        //double tuple vector must at least have 4 elements to ensure the vector size is at least 2
        return;

    //else new left and right child here;
    //set yOffset to 0.5* yOffset;

    this->m_leftChild = new QTreeNodeItem(m_R,this,true, m_main,m_scene);

    this->m_leftChild->m_height = m_height;
    this->m_leftChild->m_width = m_width/2;
    //this->m_leftChild->m_yOffset = 0.5*m_yOffset;
    //this->m_leftChild->m_branchEval = m_branchEval+"[[1]]";
    this->m_leftChild->m_x = m_x;
    this->m_leftChild->m_y = m_y+m_height;
    //this->m_leftChild->addEdge();

    this->m_rightChild = new QTreeNodeItem(m_R,this,false, m_main,m_scene);

    this->m_rightChild->m_height = m_height;
    this->m_rightChild->m_width = m_width/2;
    //this->m_rightChild->m_yOffset = 0.5*m_yOffset;
    //this->m_rightChild->m_branchEval = m_branchEval+"[[2]]";
    this->m_rightChild->m_x = m_x+m_width/2;
    this->m_rightChild->m_y = m_y+m_height;
    //this->m_rightChild->addEdge();

    //we do not have to worry about whether [[2]] has any content;
    //because hierarchical clustering always has either 0 or 2 children;
    //under a particular node;
    m_leftChild->update();
    m_rightChild->update();
    if(m_scene!=0){
        m_scene->addItem(m_leftChild);
        m_scene->addItem(m_rightChild);
    }
}


QRectF QTreeNodeItem::boundingRect() const
{
     return QRectF(m_x,m_y,m_width,m_height);
}

void QTreeNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    QRectF rect = this->boundingRect();

    painter->fillRect(rect,Qt::white);
    if(this->isSelected()){
        painter->setPen(QPen(Qt::red));
        QRectF r = rect;
        r.setTop(rect.top());
        r.setBottom(rect.bottom()-1);
        r.setLeft(rect.left());
        r.setRight(rect.right()-1);
        painter->drawRect(rect);
        painter->drawRect(r);
    }else{
        painter->setPen(QPen(Qt::black));
        painter->drawRect(rect);
    }

    int numBars = this->m_coeff_mid_true_val.size()-1;
    float barWidth = rect.width()/numBars;

    QColor outter = QColor::fromHsv(0,0,90,70);
    QColor inner = QColor::fromHsv(0,0,70,100);
    QColor colorMid = QColor::fromHsv(0,0,50,200);


    float maxheight = rect.height()*0.8;
    float hoffset = maxheight*0.1;
    for(int i = 0; i<numBars; i++){

        float max = m_coeff_max.at(i+1);
        float hq = m_coeff_hq.at(i+1);
        float mid = m_coeff_mid.at(i+1);
        float lq = m_coeff_lq.at(i+1);
        float min = m_coeff_min.at(i+1);

        float x = i*barWidth+0.25*barWidth+rect.x();
        float y1 = maxheight - max*maxheight+rect.y()+hoffset;
        float y2 = maxheight - hq*maxheight+rect.y()+hoffset;
        float y3 = maxheight - mid*maxheight+rect.y()+hoffset;


        float w = barWidth*0.5;
        float h1 = (max-min)*maxheight;
        float h2 = (hq-lq)*maxheight;


        painter->fillRect(x,y1,w,h1,outter);
        painter->fillRect(x,y2,w,h2,inner);


        painter->save();
        painter->setPen(colorMid);
        painter->drawLine(x,y3,x+w,y3);


        if(m_coeff_mid_true_val.size()==m_coeff_highlight.size()){
            float highlight=m_coeff_highlight.at(i+1);
            float yh = maxheight - highlight*maxheight+rect.y()+hoffset;
            QPen p(QColor(200,0,0,150));
            p.setWidth(2);
            painter->setPen(p);
            painter->drawLine(x,yh,x+w,yh);
        }

        painter->restore();
    }
}

void QTreeNodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug()<<"press";
    if(event->button()==Qt::RightButton){
            QVector<int> idx;
            emit nodeSelected(idx);
    }
    QGraphicsItem::mousePressEvent(event);
    update();
}

void QTreeNodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug()<<"double click";
    //this->setSelected(!this->isSelected());
    qDebug()<<"selected "<<this->isSelected();
    if(this->isSelected()){
        QVector<int> idx = QVector<int>::fromStdVector(m_members);
        emit nodeSelected(idx);
    }


    if(this->m_leftChild==0 && this->m_rightChild==0){
        this->addBranches();
        connect(this,SIGNAL(passHighlightModel(int)),
                m_leftChild,SLOT(updateHighlightModel(int)));
        connect(this,SIGNAL(passHighlightModel(int)),
                m_rightChild,SLOT(updateHighlightModel(int)));
    }else{
        disconnect(this,SIGNAL(passHighlightModel(int)),
                m_leftChild,SLOT(updateHighlightModel(int)));
        disconnect(this,SIGNAL(passHighlightModel(int)),
                m_rightChild,SLOT(updateHighlightModel(int)));
        SAFE_DELETE(this->m_leftChild);
        SAFE_DELETE(this->m_rightChild);
    }
    m_scene->setSceneRect(m_scene->itemsBoundingRect());
    QGraphicsItem::mouseDoubleClickEvent(event);
    update();
}

void QTreeNodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug()<<"release";
    //this->setSelected(!this->isSelected());
    qDebug()<<"selected "<<this->isSelected();
    if(this->isSelected()){
        QVector<int> idx = QVector<int>::fromStdVector(m_members);
        //QGraphicsItem::hoverEnterEvent(event);
        emit nodeSelected(idx);
    }
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}

void QTreeNodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    //this->setSelected(true);
}


void QTreeNodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    //this->setSelected(false);
}


void QTreeNodeItem::updateHighlightModel(int memIdx)
{
    using namespace Rcpp;
    QString coeffStr = "";
    if(m_clustIdx.contains(memIdx)){
        //qDebug()<<"yes";
        //render the red line;
        QString strEval = //"print(coeff.clusters$coeff.norm);"
                "coeff.clusters$coeff.norm["+QString::number(memIdx)+",];";
        m_coeff_highlight = QVector<double>::fromStdVector(
                    Rcpp::as<std::vector<double> >(m_R.parseEval(strEval.toStdString())));
        //qDebug()<<strEval;

        strEval = //"print(coeff.clusters$coeff);"
                "coeff.clusters$coeff["+QString::number(memIdx)+",];";
        //qDebug()<<strEval<<"start";
        m_coeff_highlight_raw = QVector<double>::fromStdVector(
                    Rcpp::as<std::vector<double> >(m_R.parseEval(strEval.toStdString())));
        //qDebug()<<strEval;

        strEval =
                "coeff.clusters$coeff.name;";
        m_coeff_highlight_names = QVector<std::string>::fromStdVector(
                    Rcpp::as<std::vector<std::string> >(m_R.parseEval(strEval.toStdString())));
        //qDebug()<<strEval;
        for(int i = 1; i<m_coeff_highlight_names.size() && i<m_coeff_highlight_raw.size(); i++){
            coeffStr+=QString::fromStdString(m_coeff_highlight_names.at(i));
            coeffStr+=": ";
            coeffStr+=QString::number(m_coeff_highlight_raw.at(i),'g',2);
            coeffStr+="  ";
        }
        this->update();
        //this->m_main->m_qmutex.unlock();
        emit passHighlightModel(memIdx);
    }else{
        //qDebug()<<"no";
        m_coeff_highlight.clear();
        this->update();
        //this->m_main->m_qmutex.unlock();
        emit passHighlightModel(memIdx);
    }
    emit localModelHighlighted(coeffStr);
}
