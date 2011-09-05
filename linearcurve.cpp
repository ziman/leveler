#include "linearcurve.h"

LinearCurve::LinearCurve()
{
    steps = 5;
}

LinearCurve::LinearCurve(const LinearCurve &x)
{
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
    return 0;
}

void LinearCurve::setControlPoints(const QList<QPoint> &ctrl)
{
    this->ctrl = ctrl;
}

QPolygon LinearCurve::getPolyline()
{

}
