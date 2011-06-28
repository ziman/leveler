#ifndef TONEMAP_H
#define TONEMAP_H

#include <QWidget>
#include <QList>
#include <QPoint>

class ToneMap : public QWidget
{
Q_OBJECT
public:
    explicit ToneMap(QWidget *parent = 0);
    int value(int x);

protected:
    void paintEvent(QPaintEvent *);

signals:

public slots:

private:
    QList<QPoint> points;

};

#endif // TONEMAP_H
