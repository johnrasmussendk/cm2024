#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <chrono>
#include "datmessage.h"
#include "usbworker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const bool dump_output, const char format, QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void updateConnected(bool connected);
    void updateSupState(const SupMessage &msg);
    void updateDatState(const DatMessage &msg);
    void releaseSdCard();

private:
    void resetState();
    void updateLcdBrightness();
    void updateChargeLimit();
    void updateSlotToCancel();
    std::string updateAndGetRemainingTimeStringForSlot(const DatMessage &msg);
    std::string getRemainingTimeStringForSlot(const int slotId, const uint32_t charge_cap_limit, const uint32_t charge_cap, const uint32_t discharge_cap);
    void setChargeLimitPercentageForQWidget(QWidget *qWidget, const bool enabled);

    UsbWorker *usbWorker;
    QScopedPointer<Ui::MainWindow> ui;
    int lcd_brightness;
    int charge_limit_percentage;
    int slotToCancel;
    bool remaining_time_for_slot_running[10];
    std::chrono::time_point<std::chrono::system_clock> remaining_time_for_slot_time_start[10];
    std::chrono::time_point<std::chrono::system_clock> remaining_time_for_slot_time_current[10];
    uint32_t remaining_time_for_slot_current_current[10];
    uint32_t remaining_time_for_slot_charge_cap_start[10];
    uint32_t remaining_time_for_slot_charge_cap_current[10];
};

#endif // MAINWINDOW_H
