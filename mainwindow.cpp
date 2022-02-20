#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    this->usbWorker = new UsbWorker(this);
    this->usbWorker->start();
}

MainWindow::~MainWindow() {
}
