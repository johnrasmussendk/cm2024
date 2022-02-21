#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    this->usbWorker = new UsbWorker(this);

    connect(this->usbWorker, &UsbWorker::sendState,
            this, &MainWindow::updateState);

    this->usbWorker->start();

    this->ui->setupUi(this);
}

MainWindow::~MainWindow() {
}

void MainWindow::updateState(const DatMessage &msg) {
    //std::cout << "new state arrived for slot " << (unsigned)msg.getSlot() << std::endl;
    msg.printBuf();
    //msg.printSummary();
    //msg.print();
    switch(msg.getSlot()) {
        case 0:
            this->ui->lbl_s1_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s1_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_s1_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_s1_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_s1_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s1_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s1_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_s1_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 1:
            this->ui->lbl_s2_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s2_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_s2_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_s2_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_s2_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s2_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s2_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_s2_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 2:
            this->ui->lbl_s3_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s3_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_s3_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_s3_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_s3_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s3_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s3_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_s3_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 3:
            this->ui->lbl_s4_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s4_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_s4_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_s4_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_s4_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s4_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s4_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_s4_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 4:
            this->ui->lbl_s5_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s5_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_s5_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_s5_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_s5_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s5_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s5_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_s5_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 5:
            this->ui->lbl_s6_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s6_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_s6_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_s6_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_s6_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s6_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s6_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_s6_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 6:
            this->ui->lbl_s7_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s7_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_s7_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_s7_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_s7_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s7_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s7_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_s7_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 7:
            this->ui->lbl_s8_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_s8_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_s8_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_s8_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_s8_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_s8_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_s8_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_s8_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 8:
            this->ui->lbl_sA_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_sA_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_sA_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_sA_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_sA_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_sA_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_sA_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_sA_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        case 9:
            this->ui->lbl_sB_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
            this->ui->lbl_sB_program_state->setText(QString::fromStdString(msg.getProgramStateStr()));
            this->ui->lbl_sB_step_state->setText(QString::fromStdString(msg.getStepStr()));
            this->ui->lbl_sB_ccap_state->setText(QString::fromStdString(msg.getChargeCapStr()));
            this->ui->lbl_sB_dcap_state->setText(QString::fromStdString(msg.getDischargeCapStr()));
            this->ui->lbl_sB_voltage_state->setText(QString::fromStdString(msg.getVoltageStr()));
            this->ui->lbl_sB_current_state->setText(QString::fromStdString(msg.getCurrentStr()));
            this->ui->lbl_sB_timer_state->setText(QString::fromStdString(msg.getTimerStr()));
            break;
        //case 1:
        //    this->ui->lbl_s1_chem_state->setText(QString::fromStdString(msg.getChemistryStr()));
        //    break;
        default:
            std::cout << "Unsupported Slot" << std::endl;
    }
}
