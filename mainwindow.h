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
    void updateConnected(bool connected);
    void updateState(const DatMessage &msg);

protected:
    void resetState();

private:
    UsbWorker *usbWorker;
    QScopedPointer<Ui::MainWindow> ui;
};

#endif // MAINWINDOW_H
