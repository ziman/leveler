#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <opencv.hpp>

#include <QMainWindow>
#include "tonemap.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

public slots:
    void cropChanged();
    void tonemapChanged();
    void openClicked();
    void saveAsClicked();

private:
    Ui::MainWindow *ui;
    cv::Mat hdr, hdr_pp, ldr;
};

#endif // MAINWINDOW_H
