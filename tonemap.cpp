#include "tonemap.h"

#include <QPainter>
#include <QList>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <QColor>
#include <cmath>

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
    selectedPoint = -1;

    points.append(QPoint(0,0));
    points.append(QPoint(16384,32768));
    points.append(QPoint(32768, 16384));
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

void ToneMap::mousePressEvent(QMouseEvent * event)
{
    mouseDown = true;
    mouseMoveEvent(event);
}

void ToneMap::mouseReleaseEvent(QMouseEvent * event)
{
    mouseDown = false;
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
        repaint();
    }

    qDebug() << "Move at " << event->pos() << endl;
    qDebug() << " nearest: " << pt << ", with dist = " << dist << endl;
}

void ToneMap::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (points.isEmpty())
        return;

    W = p.viewport().width();
    H = p.viewport().height();

    p.setBrush(QBrush(Qt::black));
    for (int i = 0; i < dpoints.count(); ++i)
    {
        if (selectedPoint == i)
            p.setBrush(Qt::SolidPattern);
        else
            p.setBrush(Qt::NoBrush);
        p.drawEllipse(dpoints[i], 3, 3);
    }

    p.drawPolyline(dcurve.getPolyline());
}
