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
    int lo = 1, hi = ctrl.count() - 1;
    int mid = (lo + hi) / 2;

    while (mid > 1 && lo < hi-1)
    {
        if (x <= ctrl.at(mid-1).x())
            hi = mid - 1;
        else if (x > ctrl.at(mid).x())
            lo = mid + 1;
        else
            break;
    }

    const QPoint &p1 = ctrl.at(mid-1);
    const QPoint &p2 = ctrl.at(mid);

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
