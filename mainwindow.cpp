#include "mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    this->usbWorker = new UsbWorker(this);

    connect(this->usbWorker, &UsbWorker::sendState,
            this, &MainWindow::updateState);

    this->usbWorker->start();
}

MainWindow::~MainWindow() {
}

void MainWindow::updateState(const DatMessage &msg) {
    std::cout << "new state arrived for slot " << msg.getSlot() << std::endl;
    msg.print();
}
