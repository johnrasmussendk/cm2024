#include "datmessage.h"

#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <string>

const std::string AUTO                              = "Auto";
const std::string EMPTY                             = "Empty";
const std::string NIMH_CD                           = "NiMH/CD";
const std::string NIZN                              = "NiZn";
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
const int SD_CARD_LOG_NONE                          =  0;
const int SD_CARD_LOG_COMPLETE                      =  4;

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
const int VALIDATE_SD_CARD_LOG_MIN                  = SD_CARD_LOG_NONE;
const int VALIDATE_SD_CARD_LOG_MAX                  = SD_CARD_LOG_COMPLETE;
const int VALIDATE_DISCHARGE_MIN                    =  0;
const int VALIDATE_DISCHARGE_CHEMISTRY_NIMH_CD_MAX  =  6;
const int VALIDATE_DISCHARGE_CHEMISTRY_NIZN_MAX     =  4;
const int VALIDATE_STEP_INDEX_MIN                   =  0;
const int VALIDATE_STEP_INDEX_MAX                   = 20; // Program "Maximise" is unlimited

// Maximal *included* value milliVolt, mV, at index:    0,    1,    2,    3,    4,    5,    6,    7
const int8_t NIMH_CD_CHARGE_VOLTAGE_INDEX_MV_MIN    =   2;
const int16_t NIMH_CD_CHARGE_VOLTAGE_INDEX_MV[]     = {-1,    0, 1375, 1390, 1405, 1420, 1530};
const int8_t NIMH_CD_DISCHARGE_VOLTAGE_INDEX_MV_MIN =   3;
const int16_t NIMH_CD_DISCHARGE_VOLTAGE_INDEX_MV[]  = {-1,   -1,    0, 1250, 1270, 1285, 1300, 1530};
// TODO NiZn
const int8_t NIZN_CHARGE_VOLTAGE_INDEX_MV_MIN       =   1;
const int16_t NIZN_CHARGE_VOLTAGE_INDEX_MV[]        = {-1,   -1,   -1};
const int8_t NIZN_DISCHARGE_VOLTAGE_INDEX_MV_MIN    =   1;
const int16_t NIZN_DISCHARGE_VOLTAGE_INDEX_MV[]     = {-1,   -1,   -1};

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
    validateSdCardLog();
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
        case '1' ... '8': {
                              slotId = format - '1';
                              break;
        }
        case 'A' ... 'B': {
                              slotId = format - 'A' + 8;
                              break;
        }
    }
    if (slotId != -1 && getSlotId() == slotId) {
        std::cout << getMinutes();
        std::cout << " " << getStepStr();
        std::cout << " " << getVoltageStr();
        std::cout << " " << getCurrentStr();
        std::cout << " " << getChargeCapStr();
        std::cout << " " << getDischargeCapStr();
        std::cout << std::endl;
    }
}

void DatMessage::print() const {
    std::cout << "counter:      "  << getCounter()                      << std::endl;
    std::cout << "slot:         "  << getSlotIdStr()                    << std::endl;
    std::cout << "chemistry:    #" << (unsigned) getChemistry()         << ", " << getChemistryStr()     << std::endl;
    std::cout << "runningState: #" << (unsigned) getRunningStateByte()  << ", " << getRunningStateStr()  << std::endl;
    std::cout << "programState: #" << (unsigned) getProgramState()      << ", " << getProgramStateStr()  << std::endl;
    std::cout << "program:      #" << (unsigned) getProgram()           << ", " << getProgramStr()       << std::endl;
    std::cout << "step:         #" << (unsigned) getStep()              << ", " << getStepStr()          << std::endl;
    std::cout << "minutes:      "  << getMinutes()                      << std::endl;
    std::cout << "voltage:      "  << getVoltageStr()                   << std::endl;
    std::cout << "current:      "  << getCurrentStr()                   << std::endl;
    std::cout << "chargeCap:    "  << getChargeCapStr()                 << std::endl;
    std::cout << "dischargeCap: "  << getDischargeCapStr()              << std::endl;
    std::cout << "voltageIndex: #" << (unsigned) getVoltageIndex()      << ", " << getVoltageIndexStr()  << std::endl;
    std::cout << "trickleState: #" << (unsigned) getTrickleStateByte()  << ", " << getTrickleStateStr()  << std::endl;
    std::cout << "maxCharge:    #" << (unsigned) getMaxCharge()         << ", " << getMaxChargeStr()     << std::endl;
    std::cout << "sdCardLog:    #" << (unsigned) getSdCardLog()         << ", " << getSdCardLogStr()     << std::endl;
    std::cout << "proComleteNo: "  << getProgramCompletedCountStr()     << std::endl;
    std::cout << "pause:        "  << (unsigned) getPause()             << std::endl;
    std::cout << "capacity:     "  << (unsigned) getCapacity()          << std::endl;
    std::cout << "discharge:    #" << (unsigned) getDischarge()         << ", " << getDischargeStr()     << std::endl;
    std::cout << "maxDischarge: #" << (unsigned) getMaxDischarge()      << ", " << getMaxDischargeStr()  << std::endl;
    std::cout << "stepIndex:    #" << (unsigned) getStepIndex()         << ", " << getStepIndexStr()     << std::endl;

    std::cout << "crc:          " << std::hex << getCrc() << std::dec << std::endl;
    std::cout << std::endl;
}

void DatMessage::printSummary() const {
    std::cout << getSlotIdStr() << ": " << getProgramStateStr() << ": " << getStepStr() << std::endl;
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
        case 0 ... 7: return std::string("Slot ") + (char) ('1' + slot_id);
        case 8 ... 9: return std::string("Slot ") + (char) ('A' + slot_id - 8);
        default:
            std::cerr << "Invalid slot id: " << (unsigned) slot_id << std::endl;
            return ERR + "(" + std::to_string(slot_id) + ")";
    }
}

std::string DatMessage::getSlotIdStr() const {
    return getSlotIdStr(getSlotId());
}

uint8_t DatMessage::getCellStackSize() const {
    const uint8_t slot_id = getSlotId();
    switch(slot_id) {
        case 0 ... 7: return 1;
        case 8 ... 9: return 7;
        default:
            std::cerr << "Invalid cell stack size for slot id: " << (unsigned) slot_id << std::endl;
            return -1;
    }
}

float DatMessage::getCurrentScale() const {
    const uint8_t slot_id = getSlotId();
    switch(slot_id) {
        case 0 ... 7: return 0.001;  // in units of 0.001
        case 8 ... 9: return 0.0001; // in units of 0.0001
        default:
            std::cerr << "Invalid current scale for slot id: " << (unsigned) slot_id << std::endl;
            return -1.0;
    }
}

uint8_t DatMessage::getCurrentPrecision() const {
    const uint8_t slot_id = getSlotId();
    switch(slot_id) {
        case 0 ... 7: return 3; // in units of 0.001
        case 8 ... 9: return 4; // in units of 0.0001
        default:
            std::cerr << "Invalid current precision for slot id: " << (unsigned) slot_id << std::endl;
            return 1;
    }
}

float DatMessage::getChargeCapScale() const {
    const uint8_t slot_id = getSlotId();
    switch(slot_id) {
        case 0 ... 7: return 0.01;  // in units of 0.01
        case 8 ... 9: return 0.001; // in units of 0.001
        default:
            std::cerr << "Invalid charge capacity scale for slot id: " << (unsigned) slot_id << std::endl;
            return -1.0;
    }
}

uint8_t DatMessage::getChargeCapPrecision() const {
    const uint8_t slot_id = getSlotId();
    switch(slot_id) {
        case 0 ... 7: return 2; // in units of 0.01
        case 8 ... 9: return 3; // in units of 0.001
        default:
            std::cerr << "Invalid charge capacity precision for slot id: " << (unsigned) slot_id << std::endl;
            return 1;
    }
}

float DatMessage::getDischargeCapScale() const {
    const uint8_t slot_id = getSlotId();
    switch(slot_id) {
        case 0 ... 7: return 0.01;  // in units of 0.01
        case 8 ... 9: return 0.001; // in units of 0.001
        default:
            std::cerr << "Invalid discharge capacity scale for slot id: " << (unsigned) slot_id << std::endl;
            return -1.0;
    }
}

uint8_t DatMessage::getDischargeCapPrecision() const {
    const uint8_t slot_id = getSlotId();
    switch(slot_id) {
        case 0 ... 7: return 2; // in units of 0.01
        case 8 ... 9: return 3; // in units of 0.001
        default:
            std::cerr << "Invalid discharge capacity precision for slot id: " << (unsigned) slot_id << std::endl;
            return 1;
    }
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
            return EMPTY;
        case CHEMISTRY_NIMH_CD:
            return NIMH_CD;
        case CHEMISTRY_NIZN:
            return NIZN;
        default:
            std::cerr << getSlotIdStr() << ", invalid chemistry: " << (unsigned) chemistry << std::endl;
            return ERR + "(" + std::to_string(chemistry) + ")";
    }
}

uint8_t DatMessage::getRunningStateByte() const {
    return this->buffer[4];
}

bool DatMessage::getRunningStateBool() const {
    const uint8_t running_state = getRunningStateByte();
    switch(running_state) {
        case RUNNING_STATE_NOT_RUNNING:
            // Program not running
            return false;
        case RUNNING_STATE_RUNNING:
            // Program running
            return true;
        default:
            std::cerr << getSlotIdStr() << ", invalid running state: " << (unsigned) running_state << std::endl;
            return false;
    }
}

void DatMessage::validateRunningState() const {
    const uint8_t running_state = getRunningStateByte();
    if (running_state < VALIDATE_RUNNING_STATE_MIN || VALIDATE_RUNNING_STATE_MAX < running_state) {
        std::cerr << getSlotIdStr() << ", invalid running state: " << (unsigned) running_state << std::endl;
    }
}

std::string DatMessage::getRunningStateStr() const {
    return getRunningStateBool() ? "Program running" : "Program not running";
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
            return EMPTY;
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

uint16_t DatMessage::getCurrent() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[12], 2);
    return temp;
}

std::string DatMessage::getCurrentStr() const {
    std::ostringstream out;
    out.precision(getCurrentPrecision());
    out << std::fixed << getCurrent() * getCurrentScale();
    return out.str();
}

uint32_t DatMessage::getChargeCap() const {
    uint32_t temp = 0;
    memcpy(&temp, &this->buffer[14], 4);
    return temp;
}

std::string DatMessage::getChargeCapStr() const {
    std::ostringstream out;
    out.precision(getChargeCapPrecision());
    out << std::fixed << getChargeCap() * getChargeCapScale();
    return out.str();
}

uint32_t DatMessage::getDischargeCap() const {
    uint32_t temp = 0;
    memcpy(&temp, &this->buffer[18], 4);
    return temp;
}

std::string DatMessage::getDischargeCapStr() const {
    std::ostringstream out;
    out.precision(getDischargeCapPrecision());
    out << std::fixed << getDischargeCap() * getDischargeCapScale();
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
    const uint8_t cell_stack_size = getCellStackSize();
    if (isStepDischarging() || isStepReadyDischarged()) {
        switch(voltage_index) {
            case 2: if (isProgramStateComplete() || isProgramStateTrickle()) {
                        return;
                    }
            case 3 ... 7: {
                              voltage_index_matches_voltage =    getMinimumDischargeMilliVoltage(voltage_index, cell_stack_size) <= voltage  // NiMH, case 3: Lowest  seen 0.318V
                                                              && voltage <= getMaximumDischargeMilliVoltage(voltage_index, cell_stack_size); // NiMH, case 7: Highest seen 1.522V
                              break;
            }
        }
        if (isProgramStateComplete() || isProgramStateTrickle()) {
            std::cerr << getSlotIdStr() << ", voltage index should be 2 for discharging and a finished program, index: " << (unsigned) voltage_index << ": " << getVoltageIndexStr() << ", voltage: " << getVoltageStr() << std::endl;
        }
        if (!voltage_index_matches_voltage) {
            std::cerr << getSlotIdStr() << ", invalid discharge voltage index: " << (unsigned) voltage_index << ": " << getVoltageIndexStr() << ", voltage: " << getVoltageStr() << std::endl;
        }
    } else {
        switch(voltage_index) {
            case 2 ... 6: {
                              voltage_index_matches_voltage =    getMinimumChargeMilliVoltage(voltage_index, cell_stack_size) <= voltage  // NiMH, case 2: Lowest  seen 0.006V
                                                              && voltage <= getMaximumChargeMilliVoltage(voltage_index, cell_stack_size); // NiMH, case 6: Highest seen 1.523V
                              break;
            }
            case 7: if (isProgramStateComplete() || isProgramStateTrickle()) {
                        return;
            }
        }
        if (isProgramStateComplete() || isProgramStateTrickle()) {
            std::cerr << getSlotIdStr() << ", voltage index should be 7 for charging and a finished program, index: " << (unsigned) voltage_index << ": " << getVoltageIndexStr() << ", voltage: " << getVoltageStr() << std::endl;
        }
        if (!voltage_index_matches_voltage) {
            std::cerr << getSlotIdStr() << ", invalid charge voltage index: " << (unsigned) voltage_index << ": " << getVoltageIndexStr() << ", voltage: " << getVoltageStr() << std::endl;
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
        return getStepStr();
    }
    const uint8_t cell_stack_size = getCellStackSize();
    if (isStepDischarging() || isStepReadyDischarged()) {
        switch(voltage_index) {
            case 2: if (isProgramStateComplete() || isProgramStateTrickle()) {
                        return "DFinished";
                    }
            case 3 ... 7: return getMilliVoltIntervalString('D', getMinimumDischargeMilliVoltage(voltage_index, cell_stack_size),  // NiMH, case 3: Lowest  seen 0.318V
                                                                 getMaximumDischargeMilliVoltage(voltage_index, cell_stack_size)); // NiMH, case 7: Highest seen 1.522V
            default:
                std::cerr << getSlotIdStr() << ", invalid discharge voltage index: " << (unsigned) voltage_index << std::endl;
                return ERR + "(D" + std::to_string(voltage_index) + ")";
        }
    } else {
        switch(voltage_index) {
            case 2 ... 6: return getMilliVoltIntervalString('C', getMinimumChargeMilliVoltage(voltage_index, cell_stack_size),  // NiMH, case 2: Lowest  seen 0.006V
                                                                 getMaximumChargeMilliVoltage(voltage_index, cell_stack_size)); // NiMH, case 6: Highest seen 1.523V
            case 7: if (isProgramStateComplete() || isProgramStateTrickle()) {
                        return "CFinished";
                    }
            default:
                std::cerr << getSlotIdStr() << ", invalid charge voltage index: " << (unsigned) voltage_index << std::endl;
                return ERR + "(C" + std::to_string(voltage_index) + ")";
        }
    }
}

uint16_t DatMessage::getMinimumTypeMilliVoltage(const std::string info, const int16_t type_voltage_index_mv[], const size_t type_voltage_index_mv_sizeof, const uint8_t minimum_voltage_index, const uint8_t voltage_index, const uint8_t cell_stack_size) const {
    const uint8_t type_voltage_index_mv_size = type_voltage_index_mv_sizeof / sizeof(int16_t);
    if (voltage_index < minimum_voltage_index || type_voltage_index_mv_size - 1 < voltage_index) {
        std::cerr << getSlotIdStr() << ", invalid " + info + " minimum voltage index: " << (unsigned) voltage_index << ", should be from [" << (unsigned) minimum_voltage_index << ", " << (unsigned) (type_voltage_index_mv_size - 1) << "]" << std::endl;
        return -1;
    }
    const int16_t minimum_type_voltage_mv = type_voltage_index_mv[voltage_index - 1];
    if (minimum_type_voltage_mv < 0) {
        std::cerr << getSlotIdStr() << ", invalid " + info + " minimum voltage value: " << (unsigned) minimum_type_voltage_mv << " for voltage index: " << (unsigned) voltage_index << std::endl;
        return -1;
    }
    if (minimum_type_voltage_mv == 0) {
        return 0;
    }
    return cell_stack_size * minimum_type_voltage_mv + 1;
}

uint16_t DatMessage::getMaximumTypeMilliVoltage(const std::string info, const int16_t type_voltage_index_mv[], const size_t type_voltage_index_mv_sizeof, const uint8_t minimum_voltage_index, const uint8_t voltage_index, const uint8_t cell_stack_size) const {
    const uint8_t type_voltage_index_mv_size = type_voltage_index_mv_sizeof / sizeof(int16_t);
    if (voltage_index < minimum_voltage_index || type_voltage_index_mv_size - 1 < voltage_index) {
        std::cerr << getSlotIdStr() << ", invalid " + info + " maximum voltage index: " << (unsigned) voltage_index << ", should be from [" << (unsigned) minimum_voltage_index << ", " << (unsigned) (type_voltage_index_mv_size - 1) << "]" << std::endl;
    }
    const int16_t maximum_type_voltage_mv = type_voltage_index_mv[voltage_index];
    if (maximum_type_voltage_mv < 0) {
        std::cerr << getSlotIdStr() << ", invalid " + info + " maximum voltage value: " << (unsigned) maximum_type_voltage_mv << " for voltage index: " << (unsigned) voltage_index << std::endl;
        return -1;
    }
    return cell_stack_size * maximum_type_voltage_mv;
}

uint16_t DatMessage::getMinimumChargeMilliVoltage(const uint8_t voltage_index, const uint8_t cell_stack_size) const {
    if (isChemistryNimhCd()) {
        return getMinimumTypeMilliVoltage(NIMH_CD + " charge", NIMH_CD_CHARGE_VOLTAGE_INDEX_MV, sizeof(NIMH_CD_CHARGE_VOLTAGE_INDEX_MV), NIMH_CD_CHARGE_VOLTAGE_INDEX_MV_MIN, voltage_index, cell_stack_size);
    } else if (isChemistryNizn()) {
        return getMinimumTypeMilliVoltage(NIZN + " charge", NIZN_CHARGE_VOLTAGE_INDEX_MV, sizeof(NIZN_CHARGE_VOLTAGE_INDEX_MV), NIZN_CHARGE_VOLTAGE_INDEX_MV_MIN, voltage_index, cell_stack_size);
    } else {
        const uint8_t chemistry = getChemistry();
        std::cerr << getSlotIdStr() << ", invalid chemistry: " << (unsigned) chemistry << " for minimum charge mv" << std::endl;
        return 0;
    }
}

uint16_t DatMessage::getMaximumChargeMilliVoltage(const uint8_t voltage_index, const uint8_t cell_stack_size) const {
    if (isChemistryNimhCd()) {
        return getMaximumTypeMilliVoltage(NIMH_CD + " charge", NIMH_CD_CHARGE_VOLTAGE_INDEX_MV, sizeof(NIMH_CD_CHARGE_VOLTAGE_INDEX_MV), NIMH_CD_CHARGE_VOLTAGE_INDEX_MV_MIN, voltage_index, cell_stack_size);
    } else if (isChemistryNizn()) {
        return getMaximumTypeMilliVoltage(NIZN + " charge", NIZN_CHARGE_VOLTAGE_INDEX_MV, sizeof(NIZN_CHARGE_VOLTAGE_INDEX_MV), NIZN_CHARGE_VOLTAGE_INDEX_MV_MIN, voltage_index, cell_stack_size);
    } else {
        const uint8_t chemistry = getChemistry();
        std::cerr << getSlotIdStr() << ", invalid chemistry: " << (unsigned) chemistry << " for maximum charge mv" << std::endl;
        return 0;
    }
}

uint16_t DatMessage::getMinimumDischargeMilliVoltage(const uint8_t voltage_index, const uint8_t cell_stack_size) const {
    if (isChemistryNimhCd()) {
        return getMinimumTypeMilliVoltage(NIMH_CD + " discharge", NIMH_CD_DISCHARGE_VOLTAGE_INDEX_MV, sizeof(NIMH_CD_DISCHARGE_VOLTAGE_INDEX_MV), NIMH_CD_DISCHARGE_VOLTAGE_INDEX_MV_MIN, voltage_index, cell_stack_size);
    } else if (isChemistryNizn()) {
        return getMinimumTypeMilliVoltage(NIZN + " discharge", NIZN_DISCHARGE_VOLTAGE_INDEX_MV, sizeof(NIZN_DISCHARGE_VOLTAGE_INDEX_MV), NIZN_DISCHARGE_VOLTAGE_INDEX_MV_MIN, voltage_index, cell_stack_size);
    } else {
        const uint8_t chemistry = getChemistry();
        std::cerr << getSlotIdStr() << ", invalid chemistry: " << (unsigned) chemistry << " for minimum discharge mv" << std::endl;
        return 0;
    }
}

uint16_t DatMessage::getMaximumDischargeMilliVoltage(const uint8_t voltage_index, const uint8_t cell_stack_size) const {
    if (isChemistryNimhCd()) {
        return getMaximumTypeMilliVoltage(NIMH_CD + " discharge", NIMH_CD_DISCHARGE_VOLTAGE_INDEX_MV, sizeof(NIMH_CD_DISCHARGE_VOLTAGE_INDEX_MV), NIMH_CD_DISCHARGE_VOLTAGE_INDEX_MV_MIN, voltage_index, cell_stack_size);
    } else if (isChemistryNizn()) {
        return getMaximumTypeMilliVoltage(NIZN + " discharge", NIZN_DISCHARGE_VOLTAGE_INDEX_MV, sizeof(NIZN_DISCHARGE_VOLTAGE_INDEX_MV), NIZN_DISCHARGE_VOLTAGE_INDEX_MV_MIN, voltage_index, cell_stack_size);
    } else {
        const uint8_t chemistry = getChemistry();
        std::cerr << getSlotIdStr() << ", invalid chemistry: " << (unsigned) chemistry << " for maximum discharge mv" << std::endl;
        return 0;
    }
}

std::string DatMessage::getMilliVoltIntervalString(const char interval_type, const uint16_t minimum_millivolt, const uint16_t maximum_millivolt) const {
    std::ostringstream out;
    out << interval_type;
    out << '[';
    out.precision(3);
    out << std::fixed << minimum_millivolt * 0.001;
    out << ", ";
    out << std::fixed << maximum_millivolt * 0.001;
    out << ']';
    return out.str();
}

uint8_t DatMessage::getTrickleStateByte() const {
    return this->buffer[23];
}

bool DatMessage::getTrickleStateBool() const {
    const unsigned char trickle_state = getTrickleStateByte();
    switch(trickle_state) {
        case TRICKLE_STATE_NOT_RUNNING:
            // Trickle not running
            return false;
        case TRICKLE_STATE_RUNNING:
            // Trickle running
            return true;
        default:
            std::cerr << getSlotIdStr() << ", invalid trickle state: " << (unsigned) trickle_state << std::endl;
            return false;
    }
}

void DatMessage::validateTrickleState() const {
    const uint8_t trickle_state = getTrickleStateByte();
    if (trickle_state < VALIDATE_TRICKLE_STATE_MIN || VALIDATE_TRICKLE_STATE_MAX < trickle_state) {
        std::cerr << getSlotIdStr() << ", invalid trickle state: " << (unsigned) trickle_state << std::endl;
    }
    if (getTrickleStateBool() != isProgramStateTrickle()) {
        std::cerr << getSlotIdStr() << ", trickle state: " << getTrickleStateStr() << ", should match program state trickle, is: " << getProgramStateStr() << std::endl;
    }
}

std::string DatMessage::getTrickleStateStr() const {
    return getTrickleStateBool() ? "Trickle running" : "Trickle not running";
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
        case MAX_CHARGE_AUTO: return AUTO;
        case 1 ... MAX_CHARGE_3000MA : return std::to_string(max_charge * 500) + "mA";
        default:
            std::cerr << getSlotIdStr() << ", invalid max charge: " << (unsigned) max_charge << std::endl;
            return ERR + "(" + std::to_string(max_charge) + ")";
    }
}

uint8_t DatMessage::getSdCardLog() const {
    return this->buffer[25];
}

void DatMessage::validateSdCardLog() const {
    const uint8_t sd_card_log = getSdCardLog();
    if (sd_card_log < VALIDATE_SD_CARD_LOG_MIN || VALIDATE_SD_CARD_LOG_MAX < sd_card_log || 3 == sd_card_log) {
        std::cerr << getSlotIdStr() << ", invalid sd card log: " << (unsigned) sd_card_log << std::endl;
    }
    if (isSdCardLogComplete()) {
        if (isProgramStateError() && isProgramError() && isStepError()) {
            return;
        }
        if (getRunningStateBool() && isProgramStateComplete() || !getRunningStateBool() && isProgramStateTrickle()) {
            std::cerr << getSlotIdStr() << ", invalid, sd card log: " << (unsigned) sd_card_log << ", running state: " << getRunningStateStr() << ", program state: " << getProgramStateStr() << std::endl;
        }
        if (!isProgramStateComplete() && !isProgramStateTrickle()) {
            std::cerr << getSlotIdStr() << ", invalid, sd card log: " << (unsigned) sd_card_log << ", program state: " << getProgramStateStr() << std::endl;
        }
        if (!isStepReadyCharged() && !isStepReadyDischarged()) {
            std::cerr << getSlotIdStr() << ", invalid, sd card log: " << (unsigned) sd_card_log << ", step: " << getStepStr() << std::endl;
        }
    }
}

bool DatMessage::isSdCardLogNone() const {
    return getSdCardLog() == SD_CARD_LOG_NONE;
}

bool DatMessage::isSdCardLogComplete() const {
    return getSdCardLog() == SD_CARD_LOG_COMPLETE;
}

std::string DatMessage::getSdCardLogStr() const {
    const uint8_t sd_card_log = getSdCardLog();
    switch(sd_card_log) {
        case SD_CARD_LOG_NONE: return "SD Log None";
        case 1: return "SD Log Running";
        case 2: return "SD Log Create";
        case SD_CARD_LOG_COMPLETE: return "SD Log Finished";
        default:
            std::cerr << getSlotIdStr() << ", invalid sd card log: " << (unsigned) sd_card_log << std::endl;
            return ERR + "(" + std::to_string(sd_card_log) + ")";
    }
}

uint8_t DatMessage::getProgramCompletedCount() const {
    return this->buffer[26];
}

std::string DatMessage::getProgramCompletedCountStr() const {
    const int8_t program_complete_count = getProgramCompletedCount();
    return std::to_string(program_complete_count);
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
            std::cerr << getSlotIdStr() << ", invalid " + NIMH_CD + " " + info + "discharge: " << (unsigned) discharge << std::endl;
        }
    }
    if (isChemistryNizn()) {
        if (discharge < VALIDATE_DISCHARGE_MIN || VALIDATE_DISCHARGE_CHEMISTRY_NIZN_MAX < discharge) {
            std::cerr << getSlotIdStr() << ", invalid " + NIZN + " " + info + "discharge: " << (unsigned) discharge << std::endl;
        }
    }
}

void DatMessage::validateDischarge() const {
    const uint8_t discharge = getDischarge();
    validateDischarge("", discharge);
    if (isStepIdle()) {
        const uint8_t expected_discharge = 0;
        if (discharge != expected_discharge) {
            std::cerr << getSlotIdStr() << ", invalid discharge: " << (unsigned) discharge << ". While idle, discharge: " << (unsigned) expected_discharge << " is expected, minutes: " << getMinutes() << std::endl;
        }
    }
    if (isStepReadyDischarged()) {
        const uint8_t expected_discharge = getSlotId() < 8 ? 1 : 0;
        if (discharge != expected_discharge) {
            std::cerr << getSlotIdStr() << ", invalid discharge: " << (unsigned) discharge << ". While ready-discharged, discharge: " << (unsigned) expected_discharge << " is expected, minutes: " << getMinutes() << std::endl;
        }
    }
    if (isStepDischarging()) {
        const uint8_t max_discharge = getMaxDischarge();
        if (discharge != max_discharge) {
            std::cerr << getSlotIdStr() << ", invalid discharge: " << (unsigned) discharge << ". While discharging max discharge: " << (unsigned) max_discharge << " is expected, minutes: " << getMinutes() << std::endl;
        }
    }
}

std::string DatMessage::getDischargeStr(const std::string info, const uint8_t discharge) const {
    if (isChemistryEmpty()) {
        return EMPTY;
    }
    if (isChemistryNimhCd()) {
        // NiMH/Cd
        switch(discharge) {
            case 0: return AUTO;
            case 1 ... 6 : return std::to_string(discharge * 125) + "mA";
            default:
                std::cerr << getSlotIdStr() << ", invalid " + NIMH_CD + " " + info + "discharge: " << (unsigned) discharge << std::endl;
                return ERR + "(" + NIMH_CD + std::to_string(discharge) + ")";
        }
    }
    if (isChemistryNizn()) {
        // NiZn
        switch(discharge) {
            case 1 ... 4 : return std::to_string(discharge * 150) + "mA";
            default:
                std::cerr << getSlotIdStr() << ", invalid " + NIZN + " " + info + "discharge: " << (unsigned) discharge << std::endl;
                return ERR + "(" + NIZN + std::to_string(discharge) + ")";
        }
    }
    const uint8_t chemistry = getChemistry();
    std::cerr << getSlotIdStr() << ", invalid chemistry: " << (unsigned) chemistry << std::endl;
    return ERR + "(" + std::to_string(chemistry) + "," + std::to_string(discharge) + ")";
}

std::string DatMessage::getDischargeStr() const {
    const uint8_t discharge = getDischarge();
    return getDischargeStr("", discharge);
}

uint8_t DatMessage::getMaxDischarge() const {
    return this->buffer[31];
}

void DatMessage::validateMaxDischarge() const {
    validateDischarge("maximum ", getMaxDischarge());
}

std::string DatMessage::getMaxDischargeStr() const {
    const uint8_t max_discharge = getMaxDischarge();
    return getDischargeStr("maximum ", max_discharge);
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
