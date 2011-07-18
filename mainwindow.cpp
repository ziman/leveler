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

    Mat img = cvtiffLoad16(fname.toStdString());
    namedWindow("preview", CV_WINDOW_NORMAL);
    imshow("preview", img);
    cvtiffSave16("saved.tiff", img);
}

void MainWindow::tonemapChanged()
{

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
