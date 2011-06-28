#include "tonemap.h"

#include <QPainter>
#include <QVector>
#include <QPoint>
#include <QDebug>
#include <cmath>

ToneMap::ToneMap(QWidget *parent) :
    QWidget(parent)
{
    points.append(QPoint(0,0));
    points.append(QPoint(16384,32768));
    points.append(QPoint(32768, 16384));
    points.append(QPoint(65535, 65535));
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

void ToneMap::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (points.isEmpty())
        return;

    int W = p.viewport().width();
    int H = p.viewport().height();
    int N = points.count();

    QVector<QPoint> pts;
    pts.append(QPoint(0, (points[0].y()*H) >> 16));

    int pti = 0;
    for (int i = 0; (i < W) && (pti < N-1); i += 4)
    {
        int x = (i << 16) / W;
        pts.append(QPoint(i, (value(x) * H) >> 16));
    }

    pts.append(QPoint(W-1, (points[N-1].y()*H) / 65536));

    p.drawPolyline(pts);
}
