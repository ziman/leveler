#include "tonemap.h"

#include <QPainter>
#include <QList>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <QColor>
#include <cmath>

static const int DOT_RADIUS = 3;
static const double GRAB_DIST = 10; // pixels

inline static double sqr(double x)
{
    return x*x;
}

ToneMap::ToneMap(QWidget *parent) :
    QWidget(parent)
{
    mouseDown = false;
    W = width();
    H = height();
    selectedPoint = grabbedPoint = -1;

    points.append(QPoint(0,0));
    points.append(QPoint(65535, 65535));
    refreshPoints();

    setMouseTracking(true);
}

QPoint ToneMap::fromDisplay(const QPoint & p)
{
    return QPoint((p.x() * 65536) / W, ((H-1-p.y()) * 65536) / H);
}

QPoint ToneMap::toDisplay(const QPoint &p)
{
    return QPoint((p.x() * W) / 65536, H-1 - (p.y() * H) / 65536);
}

void ToneMap::refreshPoints()
{
    dpoints.clear();
    for (int i = 0; i < points.size(); ++i)
        dpoints.append(toDisplay(points[i]));

    curve.setControlPoints(points);
    dcurve.setControlPoints(dpoints);
}

int ToneMap::nearestPoint(const QList<QPoint> & pts, QPoint p, double * dist)
{
    double bdist = 1.0e10;
    int best = 0;
    for (int i = 0; i < pts.count(); ++i)
    {
        double d = sqrt(sqr(p.x() - pts[i].x()) + sqr(p.y() - pts[i].y()));
        if (d < bdist)
        {
            bdist = d;
            best = i;
        }
    }

    if (dist) *dist = bdist;
    return best;
}

int ToneMap::value(int x)
{
   return x;
}

void ToneMap::resizeEvent(QResizeEvent * event)
{
    W = event->size().width();
    H = event->size().height();
    refreshPoints();
}

void ToneMap::mousePressEvent(QMouseEvent * event)
{
    mouseDown = true;
    grabbedPoint = selectedPoint;

    if (grabbedPoint == -1)
    {
        QPoint p = fromDisplay(event->pos());
        int i = 0;
        while (points[i].x() < p.x()) ++i;
        points.insert(i, p);
        grabbedPoint = i;
        refreshPoints();
    }

    mouseMoveEvent(event);
    update();
}

void ToneMap::mouseReleaseEvent(QMouseEvent * event)
{
    mouseDown = false;
    grabbedPoint = -1;
    update();
}

void ToneMap::mouseMoveEvent(QMouseEvent * event)
{
    double dist;
    int pt = nearestPoint(dpoints, event->pos(), &dist);

    if (dist >= GRAB_DIST)
        pt = -1;

    if (pt != selectedPoint)
    {
        selectedPoint = pt;
        update();
    }

    if (grabbedPoint != -1)
    {
        QPoint & pt = points[grabbedPoint];
        pt = fromDisplay(event->pos());
        if (grabbedPoint == 0)
            pt.setX(0);
        if (grabbedPoint == points.size()-1)
            pt.setX(65535);

        if (pt.x() < 0) pt.setX(0);
        if (pt.x() > 65535) pt.setX(65535);
        if (pt.y() < 0) pt.setY(0);
        if (pt.y() > 65535) pt.setY(65535);

        if (grabbedPoint < points.count()-1 && points[grabbedPoint+1].x() <= pt.x())
        {
            points.removeAt(grabbedPoint+1);
        }

        if (grabbedPoint > 0 && points[grabbedPoint-1].x() >= pt.x())
        {
            --grabbedPoint;
            points.removeAt(grabbedPoint);
        }

        refreshPoints();
        update();
    }
}

void ToneMap::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    W = p.viewport().width();
    H = p.viewport().height();

    // draw the frame
    p.drawRect(p.viewport());

    QBrush brush(mouseDown ? Qt::red : Qt::black);
    for (int i = 0; i < dpoints.count(); ++i)
    {
        brush.setStyle(selectedPoint == i ? Qt::SolidPattern : Qt::NoBrush);
        p.setBrush(brush);
        p.drawEllipse(dpoints[i], DOT_RADIUS, DOT_RADIUS);
    }

    p.drawPolyline(dcurve.getPolyline());
}
