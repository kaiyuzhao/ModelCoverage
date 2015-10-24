#ifndef QTREENODEITEM_H
#define QTREENODEITEM_H

#include <QGraphicsObject>
#include <QPainter>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include <RInside.h>
#include <QDebug>
#include <QObject>
#include <QVector>

//#include "CentralWidget.h"
class CentralWidget;
class QTreeNodeEdge;
//#include <QGraphicsSceneMouseEvent>

//#ifndef SAFE_DELETE
//#define SAFE_DELETE(x)		{if(x) delete (x); x= 0;}
//#endif

class QTreeNodeItem : public QGraphicsObject//, public
{
    Q_OBJECT
public:


    //parent is 0 for root node;
    QTreeNodeItem(RInside & r, QTreeNodeItem* parent,
                  bool isLeftChild, CentralWidget* main,
                  QGraphicsScene* scene);

    ~QTreeNodeItem();

    //split current node into two branches;
    //do nothing if current node is a leaf;
    void addBranches();

    void addEdge();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    //members: each member contains two values:
    //index of the variable, at location 2*i
    //index of partition on the variable, at location 2*i+1;
    //to get this vector , run "coeff.clusters$index[c(memberIdx,memberIdx+1)];"
    std::vector<int> m_members;

    QVector<int> m_clustIdx;
    QVector<double> m_coeff_highlight;
    QVector<double> m_coeff_highlight_raw;
    QVector<std::string> m_coeff_highlight_names;

    QVector<double> m_coeff_min;
    QVector<double> m_coeff_max;
    QVector<double> m_coeff_mid;
    QVector<double> m_coeff_lq;
    QVector<double> m_coeff_hq;

    QVector<double> m_coeff_mid_true_val;

    std::vector<std::string> m_names;

    float m_x;
    float m_y;

    float m_width;
    float m_height;

    float m_xOffset;
    float m_yOffset;


private:
    RInside & m_R;

    CentralWidget * m_main;

    QGraphicsScene * m_scene;

    //this command extracts values of all leaves at current node;
    //this command is in the format of:
     //"tmp.tree <- as.dendrogram(clust);"
    QString m_rootEval;

    //"as.vector(order.dendrogram(branch));"
    QString m_memIdxEval;

    //index of the total number of local models;
    //memberIdx is a collection of indicies in the current treeNode;
    //togetthis vector, run strEval = m_rootEval+m_branchEval+m_memIdxEval;
    //QVector<int> m_memberIdx;

    //pointer to its parent;
    QTreeNodeItem* m_parent;

    //QTreeNodeEdge* m_edge; //edge to parent;

    //pointer to the root of two brunches
    QTreeNodeItem* m_leftChild;

    QTreeNodeItem* m_rightChild;

    //QGraphicsView* m_canvas;

    bool m_isLeftChild;

    //bool pressed;

    void initializeCluster();

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

signals:
    void nodeSelected(QVector<int>);

    //pass this signal down to its two children;
    void passHighlightModel(int memIdx);

    void localModelHighlighted(QString);

public slots:

    void updateHighlightModel(int memIdx);

    //void updateHighlightModelfromParent(int gridIdx, int diagIdx);

public:
    ////"branch <- tmp.tree"
    //tmp.tree must be extended in the child nodes by
    // either adding suffix [[1]] or [[2]] (for left or right child)
    QString m_branchEval;
};

#endif // QTREENODEITEM_H
