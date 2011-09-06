#ifndef TONEMAP_H
#define TONEMAP_H

#include <opencv.hpp>

#include <QWidget>
#include <QList>
#include <QPoint>

#include "linearcurve.h"

#define HIST_BINS 256

#define CLAMP(x) (x < 0 ? 0 : (x > 255 ? 255 : x))
class ToneMap : public QWidget
{
Q_OBJECT
public:
    explicit ToneMap(QWidget *parent = 0);

    inline int value(int x)
    {
        if (cache[x] == -1)
            return cache[x] = lround(CLAMP(curve.value(x)));
        else
            return cache[x];
    }

    cv::Mat tonemap(const cv::Mat & hdr);
    void setHistogram(int chan, int * bins);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void resizeEvent (QResizeEvent * event);

signals:
    void changed();

public slots:
    void reset();

private:
    QList<QPoint> points;  ///< mapping points
    QList<QPoint> dpoints; ///< displayed points
    LinearCurve curve, dcurve;
    bool mouseDown;
    int W, H;
    int selectedPoint, grabbedPoint;
    int cache[65536];
    int hist[3][HIST_BINS];
    int histMax[3];

    static int nearestPoint(const QList<QPoint> & pts, QPoint p, double * bestDist = 0);
    void refreshPoints();
    void clearCache();

    void paintHistogram(int * bins, int hmax, QColor color, QPainter & p);

    QPoint fromDisplay(const QPoint & p);
    QPoint toDisplay(const QPoint & p);
};

#endif // TONEMAP_H
