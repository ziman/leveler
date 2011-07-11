#include "cubiccurve.h"
#include <cmath>

double CubicCurve::value(double x)
{
    if (x < ctrl[0].x())
        return 0;

    int i = 1;
    while (i < ctrl.count() && ctrl[i].x() < x)
        ++i;

    if (i >= ctrl.count())
        return 255;

    const Segment & seg = X[i-1];
    double ulo = 0;
    double uhi = 1;
    double u = 0.5;
    double epsilon = 1.0e-6;
    double xepsilon = 0.5;
    while (ulo + epsilon < uhi)
    {
        u = (ulo+uhi) / 2;
        double ux = seg.eval(u);
        if (abs(ux-x) < xepsilon)
            break;

        if (x < ux)
            uhi = u;
        else
            ulo = u;
    }

    return Y[i-1].eval(u);
}

CubicCurve::Segment::Segment(double a, double b, double c, double d)
{
    this->a = a; this->b = b;
    this->c = c; this->d = d;
}

CubicCurve::Segment::Segment()
{
    a = b = c = d = 0;
}

CubicCurve::Segment::Segment(const CubicCurve::Segment & x)
    : a(x.a), b(x.b), c(x.c), d(x.d)
{}

double CubicCurve::Segment::eval(double u) const {
    return (((d*u) + c)*u + b)*u + a;
}

QList<CubicCurve::Segment> CubicCurve::calcNaturalCubic(int n, const double * x) {
    --n;
    double * gamma = new double[n+1];
    double * delta = new double[n+1];
    double * D = new double[n+1];

    gamma[0] = 0.5;
    for (int i = 1; i < n; i++)
        gamma[i] = 1/(4-gamma[i-1]);
    gamma[n] = 1/(2-gamma[n-1]);

    delta[0] = 3*(x[1]-x[0])*gamma[0];
    for (int i = 1; i < n; i++)
        delta[i] = (3*(x[i+1]-x[i-1])-delta[i-1])*gamma[i];
    delta[n] = (3*(x[n]-x[n-1])-delta[n-1])*gamma[n];

    D[n] = delta[n];
    for (int i = n-1; i >= 0; i--)
        D[i] = delta[i] - gamma[i]*D[i+1];

    QList<Segment> result;
    for (int i = 0; i < n; i++)
        result.append(Segment(x[i], D[i], 3*(x[i+1] - x[i]) - 2*D[i] - D[i+1], 2*(x[i] - x[i+1]) + D[i] + D[i+1]));

    delete gamma;
    delete delta;
    delete D;
    return result;
}

void CubicCurve::setControlPoints(const QList<QPoint> & ctrl)
{
    this->ctrl = ctrl;
    if (ctrl.count() >= 2) {
        int n = ctrl.count();
        double * xpoints = new double[n];
        double * ypoints = new double[n];
        for (int i = 0; i < ctrl.count(); ++i)
        {
            xpoints[i] =  ctrl[i].x();
            ypoints[i] =  ctrl[i].y();
        }
        X = calcNaturalCubic(n, xpoints);
        Y = calcNaturalCubic(n, ypoints);
        delete xpoints;
        delete ypoints;

        points.clear();
        points.append(QPoint(lround(X[0].eval(0)), lround(Y[0].eval(0))));
        for (int i = 0; i < X.count(); i++) {
            for (int j = 1; j <= steps; j++) {
                double u = j / (double) steps;
                points.append(QPoint(lround(X[i].eval(u)), lround(Y[i].eval(u))));
            }
        }
    }
}

CubicCurve::CubicCurve()
{
    steps = 5;
}

CubicCurve::CubicCurve(int steps)
{
    this->steps = steps;
}

CubicCurve::CubicCurve(int steps, const QList<QPoint> & ctrl)
{
    this->steps = steps;
    setControlPoints(ctrl);
}

CubicCurve::CubicCurve(const CubicCurve & x)
{
    this->steps = x.steps;
    this->X = x.X;
    this->Y = x.Y;
    this->points = x.points;
}

QPolygon CubicCurve::getPolyline()
{
    QPolygon poly;
    for (int i = 0; i < points.count(); ++i)
        poly.append(points[i]);
    return poly;
}
