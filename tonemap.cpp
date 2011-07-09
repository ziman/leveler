#include "tonemap.h"

#include <QPainter>
#include <QVector>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <cmath>

struct Cubic {
    float a,b,c,d;         /* a + b*u + c*u^2 +d*u^3 */

    Cubic(float a, float b, float c, float d)
    {
        this->a = a; this->b = b;
        this->c = c; this->d = d;
    }

    Cubic()
    { a = b = c = d = 0; }

    Cubic(const Cubic & x)
        : a(x.a), b(x.b), c(x.c), d(x.d)
    {}

    float eval(float u) {
        return (((d*u) + c)*u + b)*u + a;
    }
};

QVector<Cubic> calcNaturalCubic(const QVector<float> & x) {
    int n = x.count()-1;
    QVector<float> gamma, delta, D;
    gamma.resize(n+1);
    delta.resize(n+1);
    D.resize(n+1);
    int i;
    /* We solve the equation
     [2 1       ] [D[0]]   [3(x[1] - x[0])  ]
     |1 4 1     | |D[1]|   |3(x[2] - x[0])  |
     |  1 4 1   | | .  | = |      .         |
     |    ..... | | .  |   |      .         |
     |     1 4 1| | .  |   |3(x[n] - x[n-2])|
     [       1 2] [D[n]]   [3(x[n] - x[n-1])]

     by using row operations to convert the matrix to upper triangular
     and then back sustitution.  The D[i] are the derivatives at the knots.
     */

    gamma[0] = 0.5;
    for ( i = 1; i < n; i++) {
        gamma[i] = 1/(4-gamma[i-1]);
    }
    gamma[n] = 1/(2-gamma[n-1]);

    delta[0] = 3*(x[1]-x[0])*gamma[0];
    for ( i = 1; i < n; i++) {
        delta[i] = (3*(x[i+1]-x[i-1])-delta[i-1])*gamma[i];
    }
    delta[n] = (3*(x[n]-x[n-1])-delta[n-1])*gamma[n];

    D[n] = delta[n];
    for ( i = n-1; i >= 0; i--) {
        D[i] = delta[i] - gamma[i]*D[i+1];
    }

    /* now compute the coefficients of the cubics */
    QVector<Cubic> C;
    for ( i = 0; i < n; i++)
        C.append(Cubic((float)x[i], D[i], 3*(x[i+1] - x[i]) - 2*D[i] - D[i+1], 2*(x[i] - x[i+1]) + D[i] + D[i+1]));
    return C;
}


/* draw a cubic spline */
QVector<QPoint> paintCubic(QVector<QPoint> & ctrl) {
    static const int STEPS = 12;
    QVector<QPoint> pts;
    if (ctrl.count() >= 2) {
        QVector<float> xpoints, ypoints;
        for (int i = 0; i < ctrl.count(); ++i)
        {
            xpoints.append((float) ctrl[i].x());
            ypoints.append((float) ctrl[i].y());
        }
        QVector<Cubic> X = calcNaturalCubic(xpoints);
        QVector<Cubic> Y = calcNaturalCubic(ypoints);

        /* very crude technique - just break each segment up into steps lines */
        pts.append(QPoint((int) lround(X[0].eval(0)),
                   (int) lround(Y[0].eval(0))));
        for (int i = 0; i < X.count(); i++) {
            for (int j = 1; j <= STEPS; j++) {
                float u = j / (float) STEPS;
                pts.append(QPoint(lround(X[i].eval(u)),
                           lround(Y[i].eval(u))));
            }
        }
    }
    return pts;
}

ToneMap::ToneMap(QWidget *parent) :
    QWidget(parent)
{
    points.append(QPoint(0,0));
    points.append(QPoint(16384,32768));
    points.append(QPoint(32768, 16384));
    points.append(QPoint(65535, 65535));

    mouseDown = false;
}

int ToneMap::value(int x)
{
    int lo = 0, hi = points.count()-1;
    while (lo+1 < hi)
    {
        int mid = (lo + hi) / 2;
        int midx = points[mid].x();
        if (x < midx)
            hi = mid;
        else if (x > midx)
            lo = mid;
        else
            return points[mid].y();
    }

    int rx = points[hi].x();
    int lx = points[lo].x();
    return ((rx - x) * points[lo].y() + (x - lx) * points[hi].y()) / (rx - lx);
}

void ToneMap::mousePressEvent(QMouseEvent * event)
{
    mouseDown = true;

    int x = event->x() / this->rect().width();

    int lo = 0, hi = points.count()-1;
    while (lo+1 < hi)
    {
        int mid = (lo + hi) / 2;
        int midx = points[mid].x();
        if (x < midx)
            hi = mid;
        else if (x > midx)
            lo = mid;
        else
            break;
    }

    if (abs(points[lo].x() - x) < 5)
    {
        qDebug("click!");
    }
    else
    {
        qDebug("new point added");
    }

    mouseMoveEvent(event);
}

void ToneMap::mouseReleaseEvent(QMouseEvent * event)
{
    mouseDown = false;
}

void ToneMap::mouseMoveEvent(QMouseEvent * event)
{
    if (!mouseDown) return;


}

void ToneMap::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (points.isEmpty())
        return;

    int W = p.viewport().width();
    int H = p.viewport().height();
    int N = points.count();

    QVector<QPoint> scaled;
    for (int i = 0; i < points.count(); ++i)
        scaled.append(QPoint(points[i].x() * W / 65536, points[i].y() * H / 65536));

    QVector<QPoint> pts = paintCubic(scaled);
    /*
    pts.append(QPoint(0, (points[0].y()*H) >> 16));

    int pti = 0;
    for (int i = 0; (i < W) && (pti < N-1); i += 1)
    {
        int x = (i << 16) / W;
        pts.append(QPoint(i, (value(x) * H) >> 16));
    }

    pts.append(QPoint(W-1, (points[N-1].y()*H) / 65536));
    */

    p.drawPolyline(pts);
}
