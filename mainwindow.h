#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datmessage.h"
#include "usbworker.h"

namespace Ui {
class MainWindow;
}

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
    QScopedPointer<Ui::MainWindow> ui;
};

#endif // MAINWINDOW_H
