#ifndef TONEMAP_H
#define TONEMAP_H

#include <QWidget>
#include <QList>
#include <QPoint>

#include "cubiccurve.h"

class ToneMap : public QWidget
{
Q_OBJECT
public:
    explicit ToneMap(QWidget *parent = 0);
    int value(int x);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void resizeEvent (QResizeEvent * event);

signals:

public slots:

private:
    QList<QPoint> points;  ///< mapping points
    QList<QPoint> dpoints; ///< displayed points
    CubicCurve curve, dcurve;
    bool mouseDown;
    int W, H;
    int selectedPoint, grabbedPoint;

    static int nearestPoint(const QList<QPoint> & pts, QPoint p, double * bestDist = 0);
    void refreshPoints();

    QPoint fromDisplay(const QPoint & p);
    QPoint toDisplay(const QPoint & p);
};

#endif // TONEMAP_H
