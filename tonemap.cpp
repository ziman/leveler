#include "tonemap.h"

#include <QPainter>
#include <QList>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <QColor>
#include <cmath>

using namespace std;
using namespace cv;

static const int DOT_RADIUS = 3;
static const double GRAB_DIST = 10; // pixels

inline static double sqr(double x)
{
    return x*x;
}

void ToneMap::setHistogram(int chan, int * bins)
{
    float &hmax = histMax[chan];
    hmax = 0;
    float * histChan = hist[chan];
    for (int i = 0; i < HIST_BINS; ++i)
    {
        float cnt = *bins++;
        float val = (cnt == 0) ? 0 : logf(cnt);
        if (val > hmax)
            hmax = val;
        *histChan++ = val;
    }

    update();
}

Mat ToneMap::tonemap(const Mat & hdr)
{
    Mat result(hdr.rows, hdr.cols, CV_8UC3);

    for (int y = 0; y < hdr.rows; ++y)
    {
        const unsigned short * src = hdr.ptr<unsigned short>(y);
        unsigned char * dst = result.ptr<unsigned char>(y);
        for (int x = 0; x < 3*hdr.cols; ++x)
        {
            *dst++ = value(*src++);
        }
    }

    return result;
}

void ToneMap::reset()
{
    selectedPoint = grabbedPoint = -1;
    mouseDown = false;
    points.clear();
    points.append(QPoint(0,0));
    points.append(QPoint(65535, 255));
    for (int c = 0; c < 3; ++c)
        for (int i = 0; i < HIST_BINS; ++i)
            hist[c][i] = 0;
    histMax[0] = histMax[1] = histMax[2] = 1;
    refreshPoints();
}

void ToneMap::clearCache()
{
    for (int i = 0; i < 65536; ++i)
        cache[i] = -1;
}

ToneMap::ToneMap(QWidget *parent) :
    QWidget(parent)
{
    W = width();
    H = height();
    setMouseTracking(true);
    reset();
}

QPoint ToneMap::fromDisplay(const QPoint & p)
{
    return QPoint((p.x() * 65535) / W, ((H-1-p.y()) * 255) / H);
}

QPoint ToneMap::toDisplay(const QPoint &p)
{
    return QPoint((p.x() * W) / 65535, H-1 - (p.y() * H) / 255);
}

void ToneMap::refreshPoints()
{
    dpoints.clear();
    for (int i = 0; i < points.size(); ++i)
        dpoints.append(toDisplay(points[i]));

    curve.setControlPoints(points);
    dcurve.setControlPoints(dpoints);
    clearCache();
    emit changed();
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

void ToneMap::mouseReleaseEvent(QMouseEvent *)
{
    mouseDown = false;
    grabbedPoint = -1;
    update();
}

void ToneMap::mouseMoveEvent(QMouseEvent * event)
{
    double dist;
    int npt = nearestPoint(dpoints, event->pos(), &dist);
    QPoint pos = fromDisplay(event->pos());
    if (grabbedPoint == 0)
        pos.setX(0);
    if (grabbedPoint == points.size()-1)
        pos.setX(65535);

    if (pos.x() < 0) pos.setX(0);
    if (pos.x() > 65535) pos.setX(65535);
    if (pos.y() < 0) pos.setY(0);
    if (pos.y() > 255) pos.setY(255);

    // qDebug() << "mouse at point " << pos << ", with value " << value(pos.x()) << endl;

    if (dist >= GRAB_DIST)
        npt = -1;

    if (npt != selectedPoint)
    {
        selectedPoint = npt;
        update();
    }

    if (grabbedPoint != -1)
    {
        QPoint & pt = points[grabbedPoint];
        pt = pos;

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

void ToneMap::paintHistogram(float * bins, float hmax, QColor color, QPainter & p)
{
    int W = p.viewport().width();
    int H = p.viewport().height();
    int HH = H - 1;

    // draw the histogram
    QPolygon histPoly;
    for (int i = 0, xr = 0; i < HIST_BINS; ++i)
    {
        int xl = xr;
        xr = W * (i + 1) / HIST_BINS;
        int y = HH - lround((bins[i] * HH) / hmax);
        histPoly.append(QPoint(xl, y));
        histPoly.append(QPoint(xr, y));
    }

    histPoly.append(QPoint(W,H));
    histPoly.append(QPoint(0,H));

    p.setPen(color);
    p.drawPolygon(histPoly);
    p.setPen(Qt::black);
}

void ToneMap::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    W = p.viewport().width();
    H = p.viewport().height();

    // draw the histograms
    paintHistogram(hist[0], histMax[0], Qt::red,   p);
    paintHistogram(hist[1], histMax[1], Qt::green, p);
    paintHistogram(hist[2], histMax[2], Qt::blue,  p);

    // draw the frame
    p.drawRect(p.viewport());

    QBrush brush;
    for (int i = 0; i < dpoints.count(); ++i)
    {
        brush.setColor(grabbedPoint == i ? Qt::red : Qt::black);
        brush.setStyle((selectedPoint == i || grabbedPoint == i) ? Qt::SolidPattern : Qt::NoBrush);
        p.setBrush(brush);
        p.drawEllipse(dpoints[i], DOT_RADIUS, DOT_RADIUS);
    }

    p.drawPolyline(dcurve.getPolyline());
}
