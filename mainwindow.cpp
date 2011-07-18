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
    tonemapChanged();
}

void MainWindow::tonemapChanged()
{
    ldr = ui->toneMap->tonemap(hdr);
    QImage img(ldr.data, ldr.cols, ldr.rows, 3*ldr.cols, QImage::Format_RGB888);
    ui->picture->setPixmap(QPixmap::fromImage(img));
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
