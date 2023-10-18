#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <chrono>
#include <iomanip>
#include <iostream>

const unsigned char COMMAND_CANCEL_SLOT = 0x02;
const unsigned char COMMAND_LCD_BRIGHTNESS = 0x03;
const unsigned char COMMAND_RELEASE_SD_CARD = 0x04;

MainWindow::MainWindow(const bool dump_output, const char format, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    this->usbWorker = new UsbWorker(nullptr, dump_output, format, this);
    lcd_brightness = 0;           // Default, do not change on init
    charge_limit_percentage = -1; // Default, do not change on init
    slotToCancel = -1;
    for (int slotId=0; slotId<10; slotId++) {
        remaining_time_for_slot_running[slotId] = false;
    }
    connect(this->usbWorker, &UsbWorker::sendDat, this, &MainWindow::updateDatState);
    connect(this->usbWorker, &UsbWorker::sendSup, this, &MainWindow::updateSupState);
    connect(this->usbWorker, &UsbWorker::sendConnected, this, &MainWindow::updateConnected);
    this->usbWorker->start();

    this->ui->setupUi(this);

    //connect(this->ui->actionConnect, &QAction::triggered, this->usbWorker, QOverload<>::of(&UsbWorker::connectUsb));
    //connect(this->ui->actionDisconnect, &QAction::triggered, this->usbWorker, QOverload<>::of(&UsbWorker::disconnectUsb));
    connect(this->ui->btn_connect, &QPushButton::clicked,
            this->usbWorker, QOverload<>::of(&UsbWorker::toggleConnect));
    connect(ui->release_sd_card, &QPushButton::clicked, this, &MainWindow::releaseSdCard);

    updateConnected(false);
}

MainWindow::~MainWindow() {
}

void MainWindow::updateConnected(bool connected) {
    if(connected == true) {
        ui->btn_connect->setText("CONNECTED");
        ui->btn_connect->setPalette(QPalette(Qt::green));
    } else {
        ui->btn_connect->setText("DISCONNECTED");
        ui->btn_connect->setPalette(QPalette(Qt::red));
        resetState();
    }
}

void MainWindow::updateSupState(const SupMessage &msg) {
    updateLcdBrightness();
    updateChargeLimit();
    updateSlotToCancel();
}

void MainWindow::updateDatState(const DatMessage &msg) {
    // Label
    const bool msgIsProgramStateCycle = msg.isProgramStateCycle();
    // Program State
    const std::string running_string = msg.getRunningStateBool() ? "+" : "-";
    const std::string program_state_string = msg.isProgramStateEmpty() ? ("#" + msg.getProgramCompletedCountStr()) : (running_string + msg.getProgramStateStr());
    const QString program_state_q_string = QString::fromStdString(program_state_string);
    // Step
    std::string step_index_string = "";
    if (!msg.isProgramStateEmpty() && !msg.isProgramStateNoSetup() && !msg.isProgramStateComplete()) {
        step_index_string = "  (" + msg.getStepIndexPrTotalStr() + ")";
    }
    const std::string step_state_string = msg.getStepStr() + step_index_string;
    const QString step_state_q_string = QString::fromStdString(step_state_string);
    // C-cap
    const std::string remaining_time_string = updateAndGetRemainingTimeStringForSlot(msg);
    const std::string ccap_state_string = msg.getChargeCapStr() + remaining_time_string;
    const QString ccap_state_q_string = QString::fromStdString(ccap_state_string);
    // Current
    std::string max_current_state = "";
    if (msg.isStepCharging()) {
        max_current_state = " / " + msg.getMaxChargeStr();
    } else if (msg.isStepDischarging()) {
        max_current_state = " / " + msg.getMaxDischargeStr();
    }
    const std::string current_state_string = msg.getCurrentStr() + max_current_state;
    const QString current_state_q_string = QString::fromStdString(current_state_string);
    // Update gui
    setChargeLimitPercentageForQWidget(ui->label_charge_limit_percentage, -1 != charge_limit_percentage);
    switch(msg.getSlotId()) {
        case 0:
            setChargeLimitPercentageForQWidget(this->ui->ldl_s1_name, msgIsProgramStateCycle);
            this->ui->lbl_s1_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s1_program_state->setText(program_state_q_string);
            this->ui->lbl_s1_step_state->setText(step_state_q_string);
            this->ui->lbl_s1_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_s1_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s1_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s1_current_state->setText(current_state_q_string);
            this->ui->lbl_s1_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 1:
            setChargeLimitPercentageForQWidget(this->ui->ldl_s2_name, msgIsProgramStateCycle);
            this->ui->lbl_s2_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s2_program_state->setText(program_state_q_string);
            this->ui->lbl_s2_step_state->setText(step_state_q_string);
            this->ui->lbl_s2_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_s2_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s2_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s2_current_state->setText(current_state_q_string);
            this->ui->lbl_s2_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 2:
            setChargeLimitPercentageForQWidget(this->ui->ldl_s3_name, msgIsProgramStateCycle);
            this->ui->lbl_s3_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s3_program_state->setText(program_state_q_string);
            this->ui->lbl_s3_step_state->setText(step_state_q_string);
            this->ui->lbl_s3_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_s3_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s3_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s3_current_state->setText(current_state_q_string);
            this->ui->lbl_s3_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 3:
            setChargeLimitPercentageForQWidget(this->ui->ldl_s4_name, msgIsProgramStateCycle);
            this->ui->lbl_s4_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s4_program_state->setText(program_state_q_string);
            this->ui->lbl_s4_step_state->setText(step_state_q_string);
            this->ui->lbl_s4_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_s4_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s4_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s4_current_state->setText(current_state_q_string);
            this->ui->lbl_s4_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 4:
            setChargeLimitPercentageForQWidget(this->ui->ldl_s5_name, msgIsProgramStateCycle);
            this->ui->lbl_s5_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s5_program_state->setText(program_state_q_string);
            this->ui->lbl_s5_step_state->setText(step_state_q_string);
            this->ui->lbl_s5_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_s5_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s5_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s5_current_state->setText(current_state_q_string);
            this->ui->lbl_s5_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 5:
            setChargeLimitPercentageForQWidget(this->ui->ldl_s6_name, msgIsProgramStateCycle);
            this->ui->lbl_s6_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s6_program_state->setText(program_state_q_string);
            this->ui->lbl_s6_step_state->setText(step_state_q_string);
            this->ui->lbl_s6_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_s6_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s6_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s6_current_state->setText(current_state_q_string);
            this->ui->lbl_s6_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 6:
            setChargeLimitPercentageForQWidget(this->ui->ldl_s7_name, msgIsProgramStateCycle);
            this->ui->lbl_s7_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s7_program_state->setText(program_state_q_string);
            this->ui->lbl_s7_step_state->setText(step_state_q_string);
            this->ui->lbl_s7_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_s7_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s7_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s7_current_state->setText(current_state_q_string);
            this->ui->lbl_s7_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 7:
            setChargeLimitPercentageForQWidget(this->ui->ldl_s8_name, msgIsProgramStateCycle);
            this->ui->lbl_s8_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s8_program_state->setText(program_state_q_string);
            this->ui->lbl_s8_step_state->setText(step_state_q_string);
            this->ui->lbl_s8_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_s8_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s8_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s8_current_state->setText(current_state_q_string);
            this->ui->lbl_s8_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 8:
            setChargeLimitPercentageForQWidget(this->ui->ldl_sA_name, msgIsProgramStateCycle);
            this->ui->lbl_sA_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_sA_program_state->setText(program_state_q_string);
            this->ui->lbl_sA_step_state->setText(step_state_q_string);
            this->ui->lbl_sA_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_sA_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_sA_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_sA_current_state->setText(current_state_q_string);
            this->ui->lbl_sA_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 9:
            setChargeLimitPercentageForQWidget(this->ui->ldl_sB_name, msgIsProgramStateCycle);
            this->ui->lbl_sB_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_sB_program_state->setText(program_state_q_string);
            this->ui->lbl_sB_step_state->setText(step_state_q_string);
            this->ui->lbl_sB_ccap_state->setText(ccap_state_q_string);
            this->ui->lbl_sB_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_sB_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_sB_current_state->setText(current_state_q_string);
            this->ui->lbl_sB_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        default:
            std::cerr << "Unsupported Slot" << std::endl;
    }

}

void MainWindow::releaseSdCard() {
    this->usbWorker->writeCommand(COMMAND_RELEASE_SD_CARD, (unsigned char) 0);
}

/**
 * Reset all UI elements to initial state
 */
void MainWindow::resetState() {

    this->ui->lbl_s1_chem_state->setText("-");
    this->ui->lbl_s1_program_state->setText("-");
    this->ui->lbl_s1_step_state->setText("-");
    this->ui->lbl_s1_ccap_state->setText("-");
    this->ui->lbl_s1_dcap_state->setText("-");
    this->ui->lbl_s1_voltage_state->setText("-");
    this->ui->lbl_s1_current_state->setText("-");
    this->ui->lbl_s1_timer_state->setText("-");

    this->ui->lbl_s2_chem_state->setText("-");
    this->ui->lbl_s2_program_state->setText("-");
    this->ui->lbl_s2_step_state->setText("-");
    this->ui->lbl_s2_ccap_state->setText("-");
    this->ui->lbl_s2_dcap_state->setText("-");
    this->ui->lbl_s2_voltage_state->setText("-");
    this->ui->lbl_s2_current_state->setText("-");
    this->ui->lbl_s2_timer_state->setText("-");

    this->ui->lbl_s3_chem_state->setText("-");
    this->ui->lbl_s3_program_state->setText("-");
    this->ui->lbl_s3_step_state->setText("-");
    this->ui->lbl_s3_ccap_state->setText("-");
    this->ui->lbl_s3_dcap_state->setText("-");
    this->ui->lbl_s3_voltage_state->setText("-");
    this->ui->lbl_s3_current_state->setText("-");
    this->ui->lbl_s3_timer_state->setText("-");

    this->ui->lbl_s4_chem_state->setText("-");
    this->ui->lbl_s4_program_state->setText("-");
    this->ui->lbl_s4_step_state->setText("-");
    this->ui->lbl_s4_ccap_state->setText("-");
    this->ui->lbl_s4_dcap_state->setText("-");
    this->ui->lbl_s4_voltage_state->setText("-");
    this->ui->lbl_s4_current_state->setText("-");
    this->ui->lbl_s4_timer_state->setText("-");

    this->ui->lbl_s5_chem_state->setText("-");
    this->ui->lbl_s5_program_state->setText("-");
    this->ui->lbl_s5_step_state->setText("-");
    this->ui->lbl_s5_ccap_state->setText("-");
    this->ui->lbl_s5_dcap_state->setText("-");
    this->ui->lbl_s5_voltage_state->setText("-");
    this->ui->lbl_s5_current_state->setText("-");
    this->ui->lbl_s5_timer_state->setText("-");

    this->ui->lbl_s6_chem_state->setText("-");
    this->ui->lbl_s6_program_state->setText("-");
    this->ui->lbl_s6_step_state->setText("-");
    this->ui->lbl_s6_ccap_state->setText("-");
    this->ui->lbl_s6_dcap_state->setText("-");
    this->ui->lbl_s6_voltage_state->setText("-");
    this->ui->lbl_s6_current_state->setText("-");
    this->ui->lbl_s6_timer_state->setText("-");

    this->ui->lbl_s7_chem_state->setText("-");
    this->ui->lbl_s7_program_state->setText("-");
    this->ui->lbl_s7_step_state->setText("-");
    this->ui->lbl_s7_ccap_state->setText("-");
    this->ui->lbl_s7_dcap_state->setText("-");
    this->ui->lbl_s7_voltage_state->setText("-");
    this->ui->lbl_s7_current_state->setText("-");
    this->ui->lbl_s7_timer_state->setText("-");

    this->ui->lbl_s8_chem_state->setText("-");
    this->ui->lbl_s8_program_state->setText("-");
    this->ui->lbl_s8_step_state->setText("-");
    this->ui->lbl_s8_ccap_state->setText("-");
    this->ui->lbl_s8_dcap_state->setText("-");
    this->ui->lbl_s8_voltage_state->setText("-");
    this->ui->lbl_s8_current_state->setText("-");
    this->ui->lbl_s8_timer_state->setText("-");

    this->ui->lbl_sA_chem_state->setText("-");
    this->ui->lbl_sA_program_state->setText("-");
    this->ui->lbl_sA_step_state->setText("-");
    this->ui->lbl_sA_ccap_state->setText("-");
    this->ui->lbl_sA_dcap_state->setText("-");
    this->ui->lbl_sA_voltage_state->setText("-");
    this->ui->lbl_sA_current_state->setText("-");
    this->ui->lbl_sA_timer_state->setText("-");

    this->ui->lbl_sB_chem_state->setText("-");
    this->ui->lbl_sB_program_state->setText("-");
    this->ui->lbl_sB_step_state->setText("-");
    this->ui->lbl_sB_ccap_state->setText("-");
    this->ui->lbl_sB_dcap_state->setText("-");
    this->ui->lbl_sB_voltage_state->setText("-");
    this->ui->lbl_sB_current_state->setText("-");
    this->ui->lbl_sB_timer_state->setText("-");
}

void MainWindow::updateLcdBrightness() {
    // LCD Brightness
    std::string lcd_brightness_string = ui->comboBox->currentText().toStdString();
    int lcd_brightness = -1;
    try {
        if (lcd_brightness_string.compare("Auto-dim") == 0) {
            lcd_brightness = 9;
        } else {
            lcd_brightness = stoi(lcd_brightness_string);
        }
    }
    catch(std::exception &err) {
        std::cerr << "Cannot convert LCD brightness to integer: "<< lcd_brightness_string << std::endl;
    }
    if (lcd_brightness != -1) {
        if (lcd_brightness != this->lcd_brightness) {
            this->lcd_brightness = lcd_brightness;
            this->usbWorker->writeCommand(COMMAND_LCD_BRIGHTNESS, (unsigned char) lcd_brightness);
        }
    }
}

void MainWindow::updateChargeLimit() {
    // Charge Limit
    int charge_limit_percentage = -1;
    std::string charge_limit_percentage_string = ui->charge_limit_percentage->text().toStdString();
    try {
        charge_limit_percentage = stoi(charge_limit_percentage_string);
        if (!(0 <= charge_limit_percentage && charge_limit_percentage <= 100)) {
            // std::cerr << "Charge limit percentage should be in range [0,100]: "<< charge_limit_percentage << std::endl;
            charge_limit_percentage = -1;
        }
    }
    catch(std::exception &err) {
        if (!charge_limit_percentage_string.empty()) {
            // std::cerr << "Cannot convert charge limit percentage to integer: "<< charge_limit_percentage_string << std::endl;
        }
    }
    if (charge_limit_percentage != this->charge_limit_percentage) {
        this->charge_limit_percentage = charge_limit_percentage;
    }
}

void MainWindow::updateSlotToCancel() {
    // Charge limit is reached. Stop charge. Cancel slot
    if (slotToCancel != -1) {
        std::cout << "Do stop charge/cancel " << DatMessage::getSlotIdStr(slotToCancel) << std::endl;
        this->usbWorker->writeCommand(COMMAND_CANCEL_SLOT, (unsigned char) slotToCancel);
    }
    slotToCancel = -1;
}

std::string MainWindow::updateAndGetRemainingTimeStringForSlot(const DatMessage &msg) {
    const uint8_t slotId = msg.getSlotId();
    std::string remaining_time_string_for_slot = "";

    if (msg.isProgramStateCycle() && msg.isStepCharging()) {
        const uint32_t current = msg.getCurrent();
        const uint32_t discharge_cap = msg.getDischargeCap();
        const uint32_t charge_cap = msg.getChargeCap();
        const uint8_t slotId = msg.getSlotId();
        if (0 <= slotId && slotId < 10) {
            // Program is "Cycle"
            // and, Step is "Charging"
            // and, SlotId is valid
            const bool remaining_time_for_slot_running_last = remaining_time_for_slot_running[slotId];
            // Initialise
            if (!remaining_time_for_slot_running_last) {
                // Just started. Do initialisation
                remaining_time_for_slot_running[slotId] = true;
                remaining_time_for_slot_time_start[slotId] = std::chrono::system_clock::now();
                remaining_time_for_slot_time_current[slotId] = remaining_time_for_slot_time_start[slotId];
                remaining_time_for_slot_current_current[slotId] = current;
                remaining_time_for_slot_charge_cap_start[slotId] = charge_cap;
                remaining_time_for_slot_charge_cap_current[slotId] = charge_cap;
            }
            // Re-initialise time values
            if (remaining_time_for_slot_running_last) {
                if (remaining_time_for_slot_charge_cap_start[slotId] == charge_cap) {
                    // Charge_cap has not changed from the initialised charge_cap.
                    // Assume the initialization started ONE minute ago. The charge_cap does not change for ONE minute.
                    // When the charge_cap no longer matches then the time stamp is ONE minute old, and no more changes are done to the time stamp.
                    const std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
                    remaining_time_for_slot_time_current[slotId] = time_now;
                    remaining_time_for_slot_time_start[slotId] = time_now - std::chrono::minutes(1);
                }
            }
            // Estimate Remaining Charge Time
            if (-1 != charge_limit_percentage && discharge_cap != 0) {
                // The Charge Limit Percentage is valid
                // And, the Discharge capacity is non-zero. That is the "Cycle" program has charged the battery fully. And the battery has been discharged fully with that value. Now, do the final charge of the battery.
                // Set charge_cap_limit
                const uint32_t charge_cap_limit = discharge_cap * charge_limit_percentage / 100;
                // Set last values for time stamp, current and charge_cap.
                const std::chrono::time_point<std::chrono::system_clock> remaining_time_for_slot_time_last = remaining_time_for_slot_time_current[slotId];
                const uint32_t remaining_time_for_slot_current_last = remaining_time_for_slot_current_current[slotId];
                const uint32_t remaining_time_for_slot_charge_cap_last = remaining_time_for_slot_charge_cap_current[slotId];
                if (remaining_time_for_slot_charge_cap_last != charge_cap) {
                    // Charge cap has changed since last. Do updates.
                    const std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();
                    // Set current values for time stamp, current and charge_cap.
                    remaining_time_for_slot_time_current[slotId] = time_now;
                    remaining_time_for_slot_current_current[slotId] = current;
                    remaining_time_for_slot_charge_cap_current[slotId] = charge_cap;
                    // Test if the current has changed significant
                    bool current_has_changed_significant = false;
                    if (current != 0) {
                        const uint32_t current_diff = std::abs(((int32_t) remaining_time_for_slot_current_last) - ((int32_t) current));
                        // Current has changed more than 10% since last value
                        current_has_changed_significant = 10 <= (current_diff * 100 / current);
                    }
                    // The current has changed significant
                    if (current_has_changed_significant) {
                        // The current has changed significant to the last current value
                        // The current start values for time stamp and charge_cap are no longer valid as the current has changed too much.
                        // Reinitialise start values for time stamp and charge_cap.
                        // The start value for time stamp is ONE minute ago to match the start value for charge_cap.
                        remaining_time_for_slot_time_start[slotId] = time_now - std::chrono::minutes(1);
                        // The current value for charge_cap is ONE minute old. That become the new start value for charge_cap.
                        remaining_time_for_slot_charge_cap_start[slotId] = remaining_time_for_slot_charge_cap_last;
                    }
                    if (slotToCancel == -1 && charge_cap_limit < charge_cap) {
                        std::cout << "Request stop charge/cancel " << msg.getSlotIdStr();
                        std::cout << ", Charge limit percentage: " << charge_limit_percentage;
                        std::cout << std::endl;
                        msg.print();
                        slotToCancel = msg.getSlotId();
                    }
                }
                // Set Remaining Time
                remaining_time_string_for_slot = getRemainingTimeStringForSlot(slotId, charge_cap_limit, charge_cap, discharge_cap);
            }
        } else {
            std::cerr << "Invalid slotId: " << (int) slotId << std::endl;
        }
    }

    if (!msg.isProgramStateCycle() || !msg.isStepCharging()) {
        const uint8_t slotId = msg.getSlotId();
        if (0 <= slotId && slotId < 10) {
            remaining_time_for_slot_running[slotId] = false;
        }
    }
    return remaining_time_string_for_slot;
}

std::string MainWindow::getRemainingTimeStringForSlot(const int slotId, const uint32_t charge_cap_limit, const uint32_t charge_cap, const uint32_t discharge_cap) {
    std::string remaining_time_string_for_slot = "";
    const uint32_t timed_charge_cap = std::max((uint32_t)0, charge_cap - remaining_time_for_slot_charge_cap_start[slotId]);
    if (timed_charge_cap != 0) {
        const std::chrono::time_point<std::chrono::system_clock> time_start = remaining_time_for_slot_time_start[slotId];
        const std::chrono::time_point<std::chrono::system_clock> time_current = remaining_time_for_slot_time_current[slotId];
        const uint32_t time_start_to_time_current_seconds = ((std::chrono::duration<double>) (time_current-time_start)).count();
        const uint32_t time_start_to_time_current_minutes = time_start_to_time_current_seconds / 60;
        const uint32_t remaining_charge_cap = charge_cap_limit - charge_cap;
        const uint32_t remaining_minutes = 1 + (time_start_to_time_current_minutes * remaining_charge_cap) / timed_charge_cap;
        const uint32_t hour_count = std::min((uint32_t) 999, remaining_minutes / 60);
        const uint32_t minute_count = remaining_minutes % 60;
        char time_text_buf[9]; // ", xxx:xx"
        const char format[] = ", %d:%.2d";
        const int size_s = std::snprintf(nullptr, 0, format, hour_count, minute_count);
        std::sprintf(time_text_buf, format, hour_count, minute_count);
        remaining_time_string_for_slot = std::string(time_text_buf, size_s);
    }
    return remaining_time_string_for_slot;
}

void MainWindow::setChargeLimitPercentageForQWidget(QWidget *qWidget, const bool enabled) {
    if (charge_limit_percentage != -1 && enabled) {
        qWidget->setAutoFillBackground(true);
        qWidget->setPalette(QPalette(Qt::green));
    } else {
        qWidget->setAutoFillBackground(true);
        qWidget->setPalette(style()->standardPalette());
    }
}
