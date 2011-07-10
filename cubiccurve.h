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
        double eval(double u);
    };
    int steps;
    QList<Segment> X, Y;
    QList<QPoint> points;

    static QList<Segment> calcNaturalCubic(int n, const double * x);

    public:
        void setControlPoints(const QList<QPoint> & ctrl);
        QPolygon getPolyline();

        CubicCurve();
        CubicCurve(const CubicCurve & x);
        CubicCurve(int steps);
        CubicCurve(int steps, const QList<QPoint> & ctrl);
};

#endif // CUBICCURVE_H
