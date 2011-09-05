#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cvtiff.h"

#include <QFileDialog>

using namespace std;
using namespace cv;

void MainWindow::saveAsClicked()
{
    QString fname = QFileDialog::getSaveFileName(this, "Save a 8-bit image", ".",
        "JPEG files (*.jpg);;PNG files (*.png);;All files (*.*)"
    );
    if (fname.isNull())
        return;

    QImage img(ldr.data, ldr.cols, ldr.rows, 3*ldr.cols, QImage::Format_RGB888);
    img.save(fname);
}

void MainWindow::openClicked()
{
    QString fname = QFileDialog::getOpenFileName(this, "Open a 16-bit TIFF file", ".",
        "TIFF files (*.tif *.tiff);;All files (*.*)"
    );
    if (fname.isNull())
        return;

    hdr = cvtiffLoad16(fname.toStdString());
    cropChanged();
}

void MainWindow::tonemapChanged()
{
    if (hdr_pp.empty())
        return;

    ldr = ui->toneMap->tonemap(hdr_pp);
    QImage img(ldr.data, ldr.cols, ldr.rows, 3*ldr.cols, QImage::Format_RGB888);
    ui->picture->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::cropChanged()
{
    if (hdr.empty())
        return;

    int l = ui->scaleHLo->value() * hdr.cols / 100;
    int r = ui->scaleHHi->value() * hdr.cols / 100;
    int t = ui->scaleVLo->value() * hdr.rows / 100;
    int b = ui->scaleVHi->value() * hdr.rows / 100;

    if (t >= b)
    {
        if (b > 0)
            t = b - 1;
        else
            b = t + 1;
    }

    if (l >= r)
    {
        if (r > 0)
            l = r - 1;
        else
            r = l + 1;
    }

    hdr_pp = Mat(hdr, cvRect(l, t, r-l, b-t));

    tonemapChanged();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->toneMap, SIGNAL(changed()), SLOT(tonemapChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
