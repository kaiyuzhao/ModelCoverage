#ifndef COLORSCHEMAWIDGET_H
#define COLORSCHEMAWIDGET_H

#include <QWidget>
#include <QVector>
#include <QColor>

class ColorSchemaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorSchemaWidget(QWidget *parent = 0);
    ~ColorSchemaWidget();

    void paintEvent(QPaintEvent*);

    void setGrayScale(bool);

private:
    QVector<QColor> colPiYG;

    bool isGrayScale;


signals:
    
public slots:
    
};

#endif // COLORSCHEMAWIDGET_H
