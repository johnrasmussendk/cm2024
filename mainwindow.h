#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datmessage.h"
#include "usbworker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void updateState(const DatMessage &msg);

private:
    UsbWorker *usbWorker;

};

#endif // MAINWINDOW_H
