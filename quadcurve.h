#ifndef QUADCURVE_H
#define QUADCURVE_H

#include <QList>
#include <QPoint>
#include <QPolygon>

class QuadCurve
{
    int steps;
    QList<QPoint> points;
    QList<QPoint> ctrl;

    public:
        void setControlPoints(const QList<QPoint> & ctrl);
        QPolygon getPolyline();

        double value(double x);

        QuadCurve();
        QuadCurve(const QuadCurve & x);
        QuadCurve(int steps);
        QuadCurve(int steps, const QList<QPoint> & ctrl);
};

#endif // QUADCURVE_H
