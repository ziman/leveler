#include "linearcurve.h"

#include <cmath>

LinearCurve::LinearCurve()
{
    steps = 5;
}

LinearCurve::LinearCurve(const LinearCurve &x)
{
    this->ctrl = x.ctrl;
    this->steps = x.steps;
}

LinearCurve::LinearCurve(int steps)
{
    this->steps = steps;
}

LinearCurve::LinearCurve(int steps, const QList<QPoint> &ctrl)
{
    this->steps = steps;
    this->ctrl = ctrl;
}

double LinearCurve::value(double x)
{
    int mid = 1;
    while (ctrl.at(mid).x() < x && mid < ctrl.count()-1)
        ++mid;

    const QPoint &p1 = ctrl.at(mid);
    const QPoint &p2 = ctrl.at(mid-1);

    return p1.y() + (lround(x) - p1.x()) * (p2.y() - p1.y()) / (p2.x() - p1.x());
}

void LinearCurve::setControlPoints(const QList<QPoint> &ctrl)
{
    this->ctrl = ctrl;
}

QPolygon LinearCurve::getPolyline()
{
    QPolygon poly;
    for (int i = 0; i < ctrl.count(); ++i)
        poly.append(ctrl[i]);
    return poly;
}
