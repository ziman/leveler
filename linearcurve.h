#ifndef LINEARCURVE_H
#define LINEARCURVE_H

#include <QList>
#include <QPoint>
#include <QPolygon>

class LinearCurve
{
    int steps;
    QList<QPoint> ctrl;

public:
    void setControlPoints(const QList<QPoint> & ctrl);
    QPolygon getPolyline();

    double value(double x);

    LinearCurve();
    LinearCurve(const LinearCurve & x);
    LinearCurve(int steps);
    LinearCurve(int steps, const QList<QPoint> & ctrl);
};

#endif // LINEARCURVE_H
