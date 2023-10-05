#include "datmessage.h"

#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <string>

const std::string ERR                               = "ERR";
const std::string NOT_AVAILABLE                     = "N/A";
const int CHEMISTRY_EMPTY                           =  0;
const int CHEMISTRY_NIMH_CD                         =  1;
const int CHEMISTRY_NIZN                            =  2;
const int RUNNING_STATE_NOT_RUNNING                 =  0;
const int RUNNING_STATE_RUNNING                     =  1;
const int PROGRAM_STATE_EMPTY                       =  0;
const int PROGRAM_STATE_RECHARGE                    =  1;
const int PROGRAM_STATE_DISCHARGE                   =  2;
const int PROGRAM_STATE_PROCHARGE                   =  3;
const int PROGRAM_STATE_CYCLE                       =  4;
const int PROGRAM_STATE_ALIVE                       =  5;
const int PROGRAM_STATE_MAXIMIZE                    =  6;
const int PROGRAM_STATE_NO_SETUP                    =  7;
const int PROGRAM_STATE_TRICKLE                     =  8;
const int PROGRAM_STATE_ERROR                       = 10;
const int PROGRAM_STATE_COMPLETE                    = 11;
const int STEP_IDLE                                 =  0;
const int STEP_CHARGING                             =  1;
const int STEP_DISCHARGING                          =  2;
const int STEP_READY_CHARGED                        =  3;
const int STEP_READY_DISCHARGED                     =  4;
const int STEP_COOL_DOWN                            =  5;
const int STEP_ERROR                                =  6;
const int TRICKLE_STATE_NOT_RUNNING                 =  0;
const int TRICKLE_STATE_RUNNING                     =  1;
const int MAX_CHARGE_AUTO                           =  0;
const int MAX_CHARGE_3000MA                         =  6;
const int PROGRAM_STATUS_EMPTY                      =  0;
const int PROGRAM_STATUS_COMPLETE                   =  4;

const int VALIDATE_SLOT_ID_MIN                      =  0;
const int VALIDATE_SLOT_ID_MAX                      =  9;
const int VALIDATE_CHEMISTRY_MIN                    = CHEMISTRY_EMPTY;
const int VALIDATE_CHEMISTRY_MAX                    = CHEMISTRY_NIZN;
const int VALIDATE_RUNNING_STATE_MIN                = RUNNING_STATE_NOT_RUNNING;
const int VALIDATE_RUNNING_STATE_MAX                = RUNNING_STATE_RUNNING;
const int VALIDATE_PROGRAM_STATE_MIN                = PROGRAM_STATE_EMPTY;
const int VALIDATE_PROGRAM_STATE_MAX                = PROGRAM_STATE_COMPLETE;
const int VALIDATE_PROGRAM_MIN                      = PROGRAM_STATE_EMPTY;
const int VALIDATE_PROGRAM_MAX                      = PROGRAM_STATE_NO_SETUP;
const int VALIDATE_STEP_MIN                         = STEP_IDLE;
const int VALIDATE_STEP_MAX                         = STEP_ERROR;
const int VALIDATE_TRICKLE_STATE_MIN                = TRICKLE_STATE_NOT_RUNNING;
const int VALIDATE_TRICKLE_STATE_MAX                = TRICKLE_STATE_RUNNING;
const int VALIDATE_MAX_CHARGE_MIN                   = MAX_CHARGE_AUTO;
const int VALIDATE_MAX_CHARGE_MAX                   = MAX_CHARGE_3000MA;
const int VALIDATE_PROGRAM_STATUS_MIN               = PROGRAM_STATUS_EMPTY;
const int VALIDATE_PROGRAM_STATUS_MAX               = PROGRAM_STATUS_COMPLETE;
const int VALIDATE_DISCHARGE_MIN                    =  0;
const int VALIDATE_DISCHARGE_CHEMISTRY_NIMH_CD_MAX  =  6;
const int VALIDATE_DISCHARGE_CHEMISTRY_NIZN_MAX     =  4;
const int VALIDATE_STEP_INDEX_MIN                   =  0;
const int VALIDATE_STEP_INDEX_MAX                   = 20; // Program "Maximise" is unlimited

DatMessage::DatMessage(char* buffer, const int16_t len)
    : Message(buffer, len) {
    validateSlotId();
    validateChemistry();
    validateRunningState();
    validateProgramState();
    validateProgram();
    validateStep();
    validateVoltageIndex();
    validateTrickleState();
    validateMaxCharge();
    validateProgramStatus();
    validateDischarge();
    validateMaxDischarge();
    validateStepIndex();
}

void DatMessage::print_format(const char format) const {
    if (format == 'd') {
        printBuf();
        printSummary();
        print();
    }
    int slotId = -1;
    switch(format) {
        case '1' ... '8':   {   slotId = format - '1';
                                break;
                            }
        case 'A' ... 'B':   {   slotId = format - 'A' + 8;
                                break;
                            }
    }
    if (slotId != -1 && getSlotId() == slotId) {
        std::cout << getMinutes();
        std::cout << " " << getStepStr();
        std::cout << " " << getVoltage()/1000.0;
        std::cout << " " << getCurrent()/1000.0;
        std::cout << " " << getChargeCap()/100.0;
        std::cout << " " << getDischargeCap()/100.0;
        std::cout << std::endl;
    }
}

void DatMessage::print() const {
    std::cout << "counter:      " << this->getCounter() << std::endl;
    std::cout << "slot:         " << this->getSlotIdStr() << std::endl;
    std::cout << "chemistry:    #" << (unsigned) this->getChemistry() << ", " << this->getChemistryStr() << std::endl;
    std::cout << "runningState: #" <<            this->getRunningState() << ", " << this->getRunningStateStr() << std::endl;
    std::cout << "programState: #" << (unsigned) this->getProgramState() << ", " << this->getProgramStateStr() << std::endl;
    std::cout << "program:      #" << (unsigned) this->getProgram() << ", " << this->getProgramStr() << std::endl;
    std::cout << "step:         #" << (unsigned) this->getStep() << ", " << this->getStepStr() << std::endl;
    std::cout << "minutes:      " << this->getMinutes() << std::endl;
    std::cout << "voltage:      " << this->getVoltage()/1000.0 << std::endl;
    std::cout << "current:      " << this->getCurrent()/1000.0 << std::endl;
    std::cout << "chargeCap:    " << this->getChargeCap()/100.0 << std::endl;
    std::cout << "dischargeCap: " << this->getDischargeCap()/100.0 << std::endl;
    std::cout << "voltageIndex: #" << (unsigned) this->getVoltageIndex() << ", " << this->getVoltageIndexStr() << std::endl;
    std::cout << "trickleState: #" <<            this->getTrickleState() << ", " << this->getTrickleStateStr() << std::endl;
    std::cout << "maxCharge:    #" << (unsigned) this->getMaxCharge() << ", " << this->getMaxChargeStr() << std::endl;
    std::cout << "progStatus:   #" << (unsigned) this->getProgramStatus() << ", " << this->getProgramStatusStr() << std::endl;
    std::cout << "progStartNo:  " << this->getProgramStartCountStr() << std::endl;
    std::cout << "pause:        "  << (unsigned) this->getPause() << std::endl;
    std::cout << "capacity:     "  << (unsigned) this->getCapacity() << std::endl;
    std::cout << "discharge:    #" << (unsigned) this->getDischarge() << ", " << this->getDischargeStr() << std::endl;
    std::cout << "maxDischarge: #" << (unsigned) this->getMaxDischarge() << ", " << this->getMaxDischargeStr() << std::endl;
    std::cout << "stepIndex:    #" << (unsigned) this->getStepIndex() << ", " << this->getStepIndexStr() << std::endl;

    std::cout << "crc:          " << std::hex << this->getCrc() << std::dec << std::endl;
    std::cout << std::endl;
}

void DatMessage::printSummary() const {
    std::cout << this->getSlotIdStr() << ": " << this->getProgramStateStr() << ": " << this->getStepStr() << std::endl;
}

uint16_t DatMessage::getCounter() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[0], 2);
    return htons(temp); // sequence counter is little endian unlike other fields!
}

uint8_t DatMessage::getSlotId() const {
    return this->buffer[2];
}

void DatMessage::validateSlotId() const {
    const uint8_t slot_id = getSlotId();
    if (slot_id < VALIDATE_SLOT_ID_MIN || VALIDATE_SLOT_ID_MAX < slot_id) {
        std::cerr << "Invalid slot id: " << (unsigned) slot_id << std::endl;
    }
}

std::string DatMessage::getSlotIdStr(const uint8_t slot_id) {
    switch(slot_id) {
        case 0:
            return "Slot 1";
        case 1:
            return "Slot 2";
        case 2:
            return "Slot 3";
        case 3:
            return "Slot 4";
        case 4:
            return "Slot 5";
        case 5:
            return "Slot 6";
        case 6:
            return "Slot 7";
        case 7:
            return "Slot 8";
        case 8:
            return "Slot A";
        case 9:
            return "Slot B";
        default:
            std::cerr << "Invalid slot id: " << (unsigned) slot_id << std::endl;
            return ERR + "(" + std::to_string(slot_id) + ")";
    }
}

std::string DatMessage::getSlotIdStr() const {
    return getSlotIdStr(getSlotId());
}

uint8_t DatMessage::getChemistry() const {
    return this->buffer[3];
}

void DatMessage::validateChemistry() const {
    const uint8_t chemistry = getChemistry();
    if (chemistry < VALIDATE_CHEMISTRY_MIN || VALIDATE_CHEMISTRY_MAX < chemistry) {
        std::cerr << getSlotIdStr() << ", invalid chemistry: " << (unsigned) chemistry << std::endl;
    }
}

bool DatMessage::isChemistryEmpty() const {
    return getChemistry() == CHEMISTRY_EMPTY;
}

bool DatMessage::isChemistryNimhCd() const {
    return getChemistry() == CHEMISTRY_NIMH_CD;
}

bool DatMessage::isChemistryNizn() const {
    return getChemistry() == CHEMISTRY_NIZN;
}

std::string DatMessage::getChemistryStr() const {
    const uint8_t chemistry = getChemistry();
    switch(chemistry) {
        case CHEMISTRY_EMPTY:
            return "Empty";
        case CHEMISTRY_NIMH_CD:
            return "NiMH/Cd";
        case CHEMISTRY_NIZN:
            return "NiZn";
        default:
            std::cerr << getSlotIdStr() << ", invalid chemistry: " << (unsigned) chemistry << std::endl;
            return ERR + "(" + std::to_string(chemistry) + ")";
    }
}

bool DatMessage::getRunningState() const {
    const unsigned char running_state_byte = buffer[4];
    switch(running_state_byte) {
        case RUNNING_STATE_NOT_RUNNING:
            // Program not running
            return false;
        case RUNNING_STATE_RUNNING:
            // Program running
            return true;
        default:
            std::cerr << getSlotIdStr() << ", invalid running state: " << (int) running_state_byte << std::endl;
            return false;
    }
}

void DatMessage::validateRunningState() const {
    const uint8_t running_state = getRunningState();
    if (running_state < VALIDATE_RUNNING_STATE_MIN || VALIDATE_RUNNING_STATE_MAX < running_state) {
        std::cerr << getSlotIdStr() << ", invalid running state: " << (unsigned) running_state << std::endl;
    }
}

std::string DatMessage::getRunningStateStr() const {
    return getRunningState() ? "Program running" : "Program not running";
}

uint8_t DatMessage::getProgramState() const {
    return this->buffer[5];
}

void DatMessage::validateProgramState() const {
    const uint8_t program_state = getProgramState();
    if (program_state < VALIDATE_PROGRAM_STATE_MIN || VALIDATE_PROGRAM_STATE_MAX < program_state || 9 == program_state) {
        std::cerr << getSlotIdStr() << ", invalid program state: " << (unsigned) program_state << std::endl;
    }
}

bool DatMessage::isProgramStateEmpty() const {
    return getProgramState() == PROGRAM_STATE_EMPTY;
}

bool DatMessage::isProgramStateCycle() const {
    return getProgramState() == PROGRAM_STATE_CYCLE;
}

bool DatMessage::isProgramStateMaximize() const {
    return getProgramState() == PROGRAM_STATE_MAXIMIZE;
}

bool DatMessage::isProgramStateNoSetup() const {
    return getProgramState() == PROGRAM_STATE_NO_SETUP;
}

bool DatMessage::isProgramStateTrickle() const {
    return getProgramState() == PROGRAM_STATE_TRICKLE;
}

bool DatMessage::isProgramStateError() const {
    return getProgramState() == PROGRAM_STATE_ERROR;
}

bool DatMessage::isProgramStateComplete() const {
    return getProgramState() == PROGRAM_STATE_COMPLETE;
}

std::string DatMessage::getProgramStateStr() const {
    return getProgramStr(getProgramState());
}

uint8_t DatMessage::getProgram() const {
    return this->buffer[6];
}

void DatMessage::validateProgram() const {
    const uint8_t program = getProgram();
    if ((program < VALIDATE_PROGRAM_MIN || VALIDATE_PROGRAM_MAX < program) && PROGRAM_STATE_ERROR != program) {
        std::cerr << getSlotIdStr() << ", invalid program: " << (unsigned) program << std::endl;
    }
}

bool DatMessage::isProgramDischarge() const {
    return getProgram() == PROGRAM_STATE_DISCHARGE;
}

bool DatMessage::isProgramMaximize() const {
    return getProgram() == PROGRAM_STATE_MAXIMIZE;
}

bool DatMessage::isProgramError() const {
    return getProgram() == PROGRAM_STATE_ERROR;
}

std::string DatMessage::getProgramStr(const uint8_t program) const {
    switch(program) {
        case PROGRAM_STATE_EMPTY:
            return "Empty";
        case PROGRAM_STATE_RECHARGE:
            return "Recharge";
        case PROGRAM_STATE_DISCHARGE:
            return "Discharge";
        case PROGRAM_STATE_PROCHARGE:
            return "Procharge";
        case PROGRAM_STATE_CYCLE:
            return "Cycle";
        case PROGRAM_STATE_ALIVE:
            return "Alive";
        case PROGRAM_STATE_MAXIMIZE:
            return "Maximize";
        case PROGRAM_STATE_NO_SETUP:
            return "No setup";
        case PROGRAM_STATE_TRICKLE:
            return "Trickle";
        //case 9:
        //    return "Unknown(9)";
        case PROGRAM_STATE_ERROR:
            return "Error";
        case PROGRAM_STATE_COMPLETE:
            return "Complete";
        default:
            std::cerr << getSlotIdStr() << ", invalid program(state): " << (unsigned) program << std::endl;
            return ERR + "(" + std::to_string(program) + ")";
    }
}

std::string DatMessage::getProgramStr() const {
    return getProgramStr(getProgram());
}

uint8_t DatMessage::getStep() const {
    return this->buffer[7];
}

void DatMessage::validateStep() const {
    const uint8_t step = getStep();
    if (step < VALIDATE_STEP_MIN || VALIDATE_STEP_MAX < step) {
        std::cerr << getSlotIdStr() << ", invalid step: " << (unsigned) step << std::endl;
    }
    if (isStepReadyDischarged()) {
        if (isProgramStateComplete() && isProgramDischarge()) {
            return;
        }
        std::cerr << getSlotIdStr() << ", invalid step: " << getStepStr() << ", program state: " << getProgramStateStr() << ", program: " << getProgramStr() << std::endl;
    }
    if (isStepReadyCharged()) {
        if ((isProgramStateComplete() || isProgramStateTrickle()) && !isProgramDischarge()) {
            return;
        }
        std::cerr << getSlotIdStr() << ", invalid step: " << getStepStr() << ", program state: " << getProgramStateStr() << ", program: " << getProgramStr() << std::endl;
    }
}

bool DatMessage::isStepIdle() const {
    return getStep() == STEP_IDLE;
}

bool DatMessage::isStepCharging() const {
    return getStep() == STEP_CHARGING;
}

bool DatMessage::isStepDischarging() const {
    return getStep() == STEP_DISCHARGING;
}

bool DatMessage::isStepReadyCharged() const {
    return getStep() == STEP_READY_CHARGED;
}

bool DatMessage::isStepReadyDischarged() const {
    return getStep() == STEP_READY_DISCHARGED;
}

bool DatMessage::isStepCoolDown() const {
    return getStep() == STEP_COOL_DOWN;
}

bool DatMessage::isStepError() const {
    return getStep() == STEP_ERROR;
}

std::string DatMessage::getStepStr() const {
    const uint8_t step = getStep();
    switch(step) {
        case STEP_IDLE:
            return "Idle";
        case STEP_CHARGING:
            return "Charging";
        case STEP_DISCHARGING:
            return "Discharging";
        case STEP_READY_CHARGED:
            return "Ready-Charged";
        case STEP_READY_DISCHARGED:
            return "Ready-Discharged";
        case STEP_COOL_DOWN:
            return "Cool Down";
        case STEP_ERROR:
            return "Error";
        default:
            std::cerr << getSlotIdStr() << ", invalid step state: " << (unsigned) step << std::endl;
            return ERR + "(" + std::to_string(step) + ")";
    }
}

uint16_t DatMessage::getMinutes() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[8], 2);
    return temp;
}

std::string DatMessage::getTimerStr() const {
    uint16_t hours = getMinutes()/60;
    uint16_t minutes = getMinutes()%60;
    return std::to_string(hours) + std::string(":") + (minutes < 10 ? "0" : "") + std::to_string(minutes);
}

// in units of 0.001
uint16_t DatMessage::getVoltage() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[10], 2);
    return temp;
}

std::string DatMessage::getVoltageStr() const {
    std::ostringstream out;
    out.precision(3);
    out << std::fixed << getVoltage() * 0.001;
    return out.str();
}

// in units of 0.001
uint16_t DatMessage::getCurrent() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[12], 2);
    return temp;
}

std::string DatMessage::getCurrentStr() const {
    std::ostringstream out;
    out.precision(3);
    out << std::fixed << getCurrent() * 0.001;
    return out.str();
}

// in units of 0.01
uint32_t DatMessage::getChargeCap() const {
    uint32_t temp = 0;
    memcpy(&temp, &this->buffer[14], 4);
    return temp;
}

std::string DatMessage::getChargeCapStr() const {
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << getChargeCap() * 0.01;
    return out.str();
}

// in units of 0.01
uint32_t DatMessage::getDischargeCap() const {
    uint32_t temp = 0;
    memcpy(&temp, &this->buffer[18], 4);
    return temp;
}

std::string DatMessage::getDischargeCapStr() const {
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << getDischargeCap() * 0.01;
    return out.str();
}

uint8_t DatMessage::getVoltageIndex() const {
    return this->buffer[22];
}

void DatMessage::validateVoltageIndex() const {
    const uint8_t voltage_index = getVoltageIndex();
    const uint16_t voltage = getVoltage();
    if (isProgramStateEmpty() && voltage_index == 0x20 && voltage == 0) {
        return;
    }
    if (isProgramStateNoSetup() && voltage_index == 0x20) {
        return;
    }
    if (isProgramStateError() && voltage_index == 0x09) {
        return;
    }
    if (isProgramStateMaximize() && isStepCoolDown()) {
        return;
    }
    bool voltage_index_matches_voltage = false;
    if (isStepDischarging() || isStepReadyDischarged()) {
        switch(voltage_index) {
            case 2: if (isProgramStateComplete() || isProgramStateTrickle()) {
                        return;
                    }
            case 3: {   voltage_index_matches_voltage =    0 <= voltage && voltage <= 1250; // Lowest seen 0.318V
                        break;
                    }
            case 4: {   voltage_index_matches_voltage = 1251 <= voltage && voltage <= 1270;
                        break;
                    }
            case 5: {   voltage_index_matches_voltage = 1271 <= voltage && voltage <= 1285;
                        break;
                    }
            case 6: {   voltage_index_matches_voltage = 1286 <= voltage && voltage <= 1300;
                        break;
                    }
            case 7: {   voltage_index_matches_voltage = 1301 <= voltage && voltage <= 1510;  // Higest seen 1.492V
                        break;
                    }
        }
        if (isProgramStateComplete() || isProgramStateTrickle()) {
            std::cerr << getSlotIdStr() << ", voltage index should be 2 for discharging and a finished program, index: " << (int) voltage_index << ": " << getVoltageIndexStr() << ", voltage: " << getVoltageStr() << std::endl;
        }
        if (!voltage_index_matches_voltage) {
            std::cerr << getSlotIdStr() << ", invalid voltage discharge index: " << (int) voltage_index << ": " << getVoltageIndexStr() << ", voltage: " << getVoltageStr() << std::endl;
        }
    } else {
        switch(voltage_index) {
            case 2: {   voltage_index_matches_voltage =    0 <= voltage && voltage <= 1375;  // Lowest seen 0.006V
                        break;
                    }
            case 3: {   voltage_index_matches_voltage = 1376 <= voltage && voltage <= 1390;
                        break;
                    }
            case 4: {   voltage_index_matches_voltage = 1391 <= voltage && voltage <= 1405;
                        break;
                    }
            case 5: {   voltage_index_matches_voltage = 1406 <= voltage && voltage <= 1420;
                        break;
                    }
            case 6: {   voltage_index_matches_voltage = 1421 <= voltage && voltage <= 1510;  // Higest seen 1.501V
                        break;
                    }
            case 7: if (isProgramStateComplete() || isProgramStateTrickle()) {
                        return;
                    }
        }
        if (isProgramStateComplete() || isProgramStateTrickle()) {
            std::cerr << getSlotIdStr() << ", voltage index should be 7 for charging and a finished program, index: " << (int) voltage_index << ": " << getVoltageIndexStr() << ", voltage: " << getVoltageStr() << std::endl;
        }
        if (!voltage_index_matches_voltage) {
            std::cerr << getSlotIdStr() << ", invalid voltage charge index: " << (int) voltage_index << ": " << getVoltageIndexStr() << ", voltage: " << getVoltageStr() << std::endl;
        }
    }
}

std::string DatMessage::getVoltageIndexStr() const {
    const uint8_t voltage_index = getVoltageIndex();
    const uint16_t voltage = getVoltage();
    if (isProgramStateEmpty() && voltage_index == 0x20 && voltage == 0) {
        return getProgramStateStr();
    }
    if (isProgramStateNoSetup() && voltage_index == 0x20) {
        return getProgramStateStr();
    }
    if (isProgramStateError() && voltage_index == 0x09) {
        return getProgramStateStr();
    }
    if (isProgramStateMaximize() && isStepCoolDown()) {
        return "Cool-down";
    }
    if (isStepDischarging() || isStepReadyDischarged()) {
        switch(voltage_index) {
            case 2: if (isProgramStateComplete() || isProgramStateTrickle()) {
                        return "DFinished";
                    }
            case 3: return "D[0.000, 1.250]"; // Lowest seen 0.318V
            case 4: return "D[1.251, 1.270]";
            case 5: return "D[1.271, 1.285]";
            case 6: return "D[1.286, 1.300]";
            case 7: return "D[1.301, 1.510]";  // Higest seen 1.492V
            default:
                std::cerr << getSlotIdStr() << ", invalid voltage discharge index: " << (int) voltage_index << std::endl;
                return ERR + "(D" + std::to_string(voltage_index) + ")";
        }
    } else {
        switch(voltage_index) {
            case 2: return "C[0.000, 1.375]";  // Lowest seen 0.006V
            case 3: return "C[1.376, 1.390]";
            case 4: return "C[1.391, 1.405]";
            case 5: return "C[1.406, 1.420]";
            case 6: return "C[1.421, 1.510]";  // Higest seen 1.501V
            case 7: if (isProgramStateComplete() || isProgramStateTrickle()) {
                        return "CFinished";
                    }
            default:
                std::cerr << getSlotIdStr() << ", invalid voltage charge index: " << (int) voltage_index << std::endl;
                return ERR + "(C" + std::to_string(voltage_index) + ")";
        }
    }
}

bool DatMessage::getTrickleState() const {
    const unsigned char trickle_state_byte = buffer[23];
    switch(trickle_state_byte) {
        case TRICKLE_STATE_NOT_RUNNING:
            // Trickle not running
            return false;
        case TRICKLE_STATE_RUNNING:
            // Trickle running
            return true;
        default:
            std::cerr << getSlotIdStr() << ", invalid trickle state: " << (int) trickle_state_byte << std::endl;
            return false;
    }
}

void DatMessage::validateTrickleState() const {
    const uint8_t trickle_state = getTrickleState();
    if (trickle_state < VALIDATE_TRICKLE_STATE_MIN || VALIDATE_TRICKLE_STATE_MAX < trickle_state) {
        std::cerr << getSlotIdStr() << ", invalid trickle state: " << (unsigned) trickle_state << std::endl;
    }
    if (getTrickleState() != isProgramStateTrickle()) {
        std::cerr << getSlotIdStr() << ", trickle state: " << getTrickleStateStr() << ", should match program state trickle, is: " << getProgramStateStr() << std::endl;
    }
}

std::string DatMessage::getTrickleStateStr() const {
    return getTrickleState() ? "Trickle running" : "Trickle not running";
}

uint8_t DatMessage::getMaxCharge() const {
    return this->buffer[24];
}

void DatMessage::validateMaxCharge() const {
    const uint8_t max_charge = getMaxCharge();
    if (max_charge < VALIDATE_MAX_CHARGE_MIN || VALIDATE_MAX_CHARGE_MAX < max_charge) {
        std::cerr << getSlotIdStr() << ", invalid max charge: " << (unsigned) max_charge << std::endl;
    }
}

std::string DatMessage::getMaxChargeStr() const {
    const uint8_t max_charge = getMaxCharge();
    switch(max_charge) {
        case MAX_CHARGE_AUTO: return "Auto";
        case 1: return "500mA";
        case 2: return "1000mA";
        case 3: return "1500mA";
        case 4: return "2000mA";
        case 5: return "2500mA";
        case MAX_CHARGE_3000MA: return "3000mA";
        default:
            std::cerr << getSlotIdStr() << ", invalid max charge: " << (int) max_charge << std::endl;
            return ERR + "(" + std::to_string(max_charge) + ")";
    }
}

uint8_t DatMessage::getProgramStatus() const {
    return this->buffer[25];
}

void DatMessage::validateProgramStatus() const {
    const uint8_t program_status = getProgramStatus();
    if (program_status < VALIDATE_PROGRAM_STATUS_MIN || VALIDATE_PROGRAM_STATUS_MAX < program_status || 3 == program_status) {
        std::cerr << getSlotIdStr() << ", invalid program status: " << (unsigned) program_status << std::endl;
    }
    if (isProgramStatusEmpty()) {
        if (getRunningState()) {
            std::cerr << getSlotIdStr() << ", invalid, program status: " << (unsigned) program_status << ", running state is running" << std::endl;
        }
        if (!isProgramStateEmpty() && !isProgramStateNoSetup()) {
            std::cerr << getSlotIdStr() << ", invalid, program status: " << (unsigned) program_status << ", program state: " << getProgramStateStr() << std::endl;
        }
        if (!isStepIdle()) {
            std::cerr << getSlotIdStr() << ", invalid, program status: " << (unsigned) program_status << ", step: " << getStepStr() << std::endl;
        }
    }
    if (isProgramStatusComplete()) {
        if (isProgramStateError() && isProgramError() && isStepError()) {
            return;
        }
        if (getRunningState() && isProgramStateComplete() || !getRunningState() && isProgramStateTrickle()) {
            std::cerr << getSlotIdStr() << ", invalid, program status: " << (unsigned) program_status << ", running state: " << getRunningStateStr() << ", program state: " << getProgramStateStr() << std::endl;
        }
        if (!isProgramStateComplete() && !isProgramStateTrickle()) {
            std::cerr << getSlotIdStr() << ", invalid, program status: " << (unsigned) program_status << ", program state: " << getProgramStateStr() << std::endl;
        }
        if (!isStepReadyCharged() && !isStepReadyDischarged()) {
            std::cerr << getSlotIdStr() << ", invalid, program status: " << (unsigned) program_status << ", step: " << getStepStr() << std::endl;
        }
    }
}

bool DatMessage::isProgramStatusEmpty() const {
    return getProgramStatus() == PROGRAM_STATUS_EMPTY;
}

bool DatMessage::isProgramStatusComplete() const {
    return getProgramStatus() == PROGRAM_STATUS_COMPLETE;
}

std::string DatMessage::getProgramStatusStr() const {
    const uint8_t program_status = getProgramStatus();
    switch(program_status) {
        case PROGRAM_STATUS_EMPTY: return "Empty";
        case 1: return "Running";
        case 2: return "Determinating";
        case PROGRAM_STATUS_COMPLETE: return "Finished";
        default:
            std::cerr << getSlotIdStr() << ", invalid program status: " << (int) program_status << std::endl;
            return ERR + "(" + std::to_string(program_status) + ")";
    }
}

uint8_t DatMessage::getProgramStartCount() const {
    return this->buffer[26];
}

std::string DatMessage::getProgramStartCountStr() const {
    const int8_t program_start_count = getProgramStartCount();
    return std::to_string(program_start_count);
}

uint8_t DatMessage::getPause() const {
    return this->buffer[27];
}

uint16_t DatMessage::getCapacity() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[28], 2);
    return temp;
}

uint8_t DatMessage::getDischarge() const {
    return this->buffer[30];
}

void DatMessage::validateDischarge(const std::string info, const uint8_t discharge) const {
    if (isChemistryNimhCd()) {
        if (discharge < VALIDATE_DISCHARGE_MIN || VALIDATE_DISCHARGE_CHEMISTRY_NIMH_CD_MAX < discharge) {
            std::cerr << getSlotIdStr() << ", invalid " + info + "discharge: " << (unsigned) discharge << std::endl;
        }
    }
    if (isChemistryNizn()) {
        if (discharge < VALIDATE_DISCHARGE_MIN || VALIDATE_DISCHARGE_CHEMISTRY_NIZN_MAX < discharge) {
            std::cerr << getSlotIdStr() << ", invalid " + info + "discharge: " << (unsigned) discharge << std::endl;
        }
    }
}

void DatMessage::validateDischarge() const {
    validateDischarge("", getDischarge());
}

std::string DatMessage::getDischargeStr(const uint8_t chemistry, const uint8_t discharge) const {
    if (isChemistryEmpty()) {
        return "Empty";
    }
    if (isChemistryNimhCd()) {
        // NiMH/Cd
        const uint8_t max_discharge = discharge;
        switch(discharge) {
            case 1: return "125mA";
            case 2: return "250mA";
            case 3: return "375mA";
            case 4: return "500mA";
            case 5: return "625mA";
            case 6: return "750mA";
            default:
                std::cerr << getSlotIdStr() << ", invalid NiMH/CD max discharge: " << (int) max_discharge << std::endl;
                return ERR + "(NiMH/CD" + std::to_string(discharge) + ")";
        }
    }
    if (isChemistryNizn()) {
        // NiZn
        const uint8_t discharge = discharge;
        switch(discharge) {
            case 1: return "150mA";
            case 2: return "300mA";
            case 3: return "450mA";
            case 4: return "600mA";
            default:
                std::cerr << getSlotIdStr() << ", invalid NiZn discharge: " << (int) discharge << std::endl;
                return ERR + "(NiZn" + std::to_string(discharge) + ")";
        }
    }
    std::cerr << getSlotIdStr() << ", invalid chemistry: " << (int) chemistry << std::endl;
    return ERR + "(" + std::to_string(chemistry) + "," + std::to_string(discharge) + ")";
}

std::string DatMessage::getDischargeStr() const {
    const uint8_t chemistry = getChemistry();
    const uint8_t discharge = getDischarge();
    return getDischargeStr(chemistry, discharge);
}

uint8_t DatMessage::getMaxDischarge() const {
    return this->buffer[31];
}

void DatMessage::validateMaxDischarge() const {
    validateDischarge("max ", getMaxDischarge());
}

std::string DatMessage::getMaxDischargeStr() const {
    const uint8_t chemistry = getChemistry();
    const uint8_t max_discharge = getMaxDischarge();
    return getDischargeStr(chemistry, max_discharge);
}

uint8_t DatMessage::getStepIndex() const {
    return this->buffer[32];
}

void DatMessage::validateStepIndex() const {
    const uint8_t step_index = getStepIndex();
    if (step_index < VALIDATE_STEP_INDEX_MIN || VALIDATE_STEP_INDEX_MAX < step_index) {
        std::cerr << getSlotIdStr() << ", invalid step index: " << (unsigned) step_index << ", Running Program \"Maximise\"? The upper limit has to be changed." << std::endl;
    }
    if (isProgramStateEmpty()) {
        if (step_index != 0) {
            std::cerr << getSlotIdStr() << ", invalid step index: " << (unsigned) step_index << ", expected 0 for empty program." << std::endl;
        }
    } else if (isProgramStateError()) {
        if (step_index != 0) {
            std::cerr << getSlotIdStr() << ", invalid step index: " << (unsigned) step_index << ", expected 0 for error program." << std::endl;
        }
    } else if (isProgramStateNoSetup()) {
        if (step_index != 0) {
            std::cerr << getSlotIdStr() << ", invalid step index: " << (unsigned) step_index << ", expected 0 for no setup program." << std::endl;
        }
    } else {
        const uint8_t program = getProgram();
        const uint8_t step_index_total = getStepIndexTotal(program);
        if (!isProgramMaximize() && (isProgramStateComplete() || isProgramStateTrickle())) {
            if (step_index != step_index_total) {
                std::cerr << getSlotIdStr() << ", invalid step index for finished program, index: " << (unsigned) step_index << ", step index max: " << (unsigned) step_index_total << ", program: " << (unsigned) program << std::endl;
            }
        } else {
            if (step_index_total <= step_index) {
                std::cerr << getSlotIdStr() << ", invalid step index for running program, index: " << (unsigned) step_index << ", step index max: " << (unsigned) step_index_total << ", program: " << (unsigned) program << std::endl;
            }
        }
    }
}

std::string DatMessage::getStepIndexStr() const {
    const int8_t step_index = getStepIndex();
    if (VALIDATE_STEP_INDEX_MIN <= step_index && step_index <= VALIDATE_STEP_INDEX_MAX) {
        return std::to_string(step_index + 1);
    }
    std::cerr << getSlotIdStr() << ", invalid step index: " << step_index << std::endl;
    return ERR + "(" + std::to_string(step_index) + ")";
}

uint8_t DatMessage::getStepIndexTotal(const uint8_t program) const {
    switch(program) {
        case PROGRAM_STATE_RECHARGE:
            return 1;
        case PROGRAM_STATE_DISCHARGE:
            return 1;
        case PROGRAM_STATE_PROCHARGE:
            return 2;
        case PROGRAM_STATE_CYCLE:
            return 3;
        case PROGRAM_STATE_ALIVE:
            return 5;
        case PROGRAM_STATE_MAXIMIZE:
            return VALIDATE_STEP_INDEX_MAX;
    }
    std::cerr << getSlotIdStr() << ", invalid program index for step index max, program: " << (unsigned) program << std::endl;
    return 0;
}

std::string DatMessage::getStepIndexPrTotalStr() const {
    const uint8_t program = getProgram();
    if (PROGRAM_STATE_RECHARGE <= program && program <= PROGRAM_STATE_ALIVE) {
        const uint8_t step_index_total = getStepIndexTotal(program);
        return getStepIndexStr() + "/" + std::to_string(step_index_total);
    }
    if (isProgramStateMaximize()) {
        return getStepIndexStr() + "/X";
    }
    return getStepIndexStr();
}

uint16_t DatMessage::getCrc() const {
    uint16_t temp = 0;
    const unsigned char high_byte = (unsigned char) this->buffer[33];
    const unsigned char low_byte = (unsigned char) this->buffer[34];
    temp = high_byte;
    temp = temp << 8;
    temp = temp + low_byte;
    return temp;
}
