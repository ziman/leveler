#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cvtiff.h"

#include <QFileDialog>

using namespace std;
using namespace cv;

void MainWindow::openClicked()
{
    QString fname = QFileDialog::getOpenFileName(this, "Open a 16-bit TIFF file", ".", "TIFF files (*.tif *.tiff);;All files (*.*)");
    if (fname.isNull())
        return;

    hdr = cvtiffLoad16(fname.toStdString());
    tonemapChanged();
}

void MainWindow::tonemapChanged()
{
    ldr = ui->toneMap->tonemap(hdr);
    QImage img = QImage(ldr.data, ldr.cols, ldr.rows, 3*ldr.cols, QImage::Format_RGB888);
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
