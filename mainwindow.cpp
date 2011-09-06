#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cvtiff.h"

#include <QFileDialog>
#include <stdint.h>

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
    if (hdr_wb.empty())
        return;

    ldr = ui->toneMap->tonemap(hdr_wb);
    QImage img(ldr.data, ldr.cols, ldr.rows, 3*ldr.cols, QImage::Format_RGB888);
    ui->picture->setPixmap(QPixmap::fromImage(img));
}

static void calcHistogram(const Mat & chan, int * hist)
{
    for (int i = 0; i < HIST_BINS; ++i)
        hist[i] = 0;

    for (int y = 0; y < chan.rows; ++y)
    {
        const uint16_t * row = chan.ptr<uint16_t>(y);
        const uint16_t * end = row + chan.cols;
        while (row < end) {
            ++hist[((int) *row) * HIST_BINS / 65536];
            ++row;
        }
    }
}

void MainWindow::wbChanged()
{
    QSlider * sliders[] = {ui->cbRed, ui->cbGreen, ui->cbBlue};
    Mat chans[3];
    split(hdr_cropped, chans);

    for (int i = 0; i < 3; ++i)
    {
        double c = ((double) sliders[i]->value()) / sliders[i]->maximum(); // result is in 0.0 .. 1.0
        c = 2*c; // normalize to 0.0 .. 2.0
        c = c*c; // quadratic

        chans[i] *= c;
    }

    if (ui->cbGrayscale->checkState() == Qt::Checked)
    {
        chans[0] = chans[1] = chans[2] = 0.333 * chans[0] + 0.333 * chans[1] + 0.333 * chans[2];
    }

    int hist[3][HIST_BINS];
    for (int i = 0; i < 3; ++i)
    {
        calcHistogram(chans[i], hist[i]);
        ui->toneMap->setHistogram(i, hist[i]);
    }

    merge(chans, 3, hdr_wb);

    tonemapChanged();
}

void MainWindow::cropChanged()
{
    if (hdr.empty())
        return;

    int l = ui->scaleHLo->value() * hdr.cols / 256;
    int r = ui->scaleHHi->value() * hdr.cols / 256;
    int t = ui->scaleVLo->value() * hdr.rows / 256;
    int b = ui->scaleVHi->value() * hdr.rows / 256;

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

    hdr_cropped = Mat(hdr, cvRect(l, t, r-l, b-t));

    wbChanged();
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
