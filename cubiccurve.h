#ifndef CUBICCURVE_H
#define CUBICCURVE_H

#include <QList>
#include <QPoint>
#include <QPolygon>

class CubicCurve
{
    struct Segment {
        double a,b,c,d;
        Segment(double a, double b, double c, double d);
        Segment();
        Segment(const Segment & x);
        double eval(double u) const;
    };
    int steps;
    QList<Segment> X, Y;
    QList<QPoint> points;
    QList<QPoint> ctrl;

    static QList<Segment> calcNaturalCubic(int n, const double * x);

    public:
        void setControlPoints(const QList<QPoint> & ctrl);
        QPolygon getPolyline();

        double value(double x);

        CubicCurve();
        CubicCurve(const CubicCurve & x);
        CubicCurve(int steps);
        CubicCurve(int steps, const QList<QPoint> & ctrl);
};

#endif // CUBICCURVE_H
