#include "datmessage.h"

#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <string>


void DatMessage::print() const
{
    std::cout << "counter:      " << this->getCounter() << std::endl;
    std::cout << "slot:         " << this->getSlotIdStr() << std::endl;
    std::cout << "chemistry:    " << this->getChemistryStr() << std::endl;
    std::cout << "unknown1:     " << std::hex << (unsigned)this->getUnknown1() << std::dec << std::endl;
    std::cout << "programState: " << this->getProgramStateStr() << std::endl;
    std::cout << "program:      " << this->getProgramStr() << std::endl;
    std::cout << "step:         " << this->getStepStr() << std::endl;
    std::cout << "minutes:      " << this->getMinutes() << std::endl;
    std::cout << "voltage:      " << this->getVoltage()/1000.0 << std::endl;
    std::cout << "current:      " << this->getCurrent()/1000.0 << std::endl;
    std::cout << "ccap:         " << this->getChargeCap()/100.0 << std::endl;
    std::cout << "dcap:         " << this->getDischargeCap()/100.0 << std::endl;
//     std::cout << "unknown2:     " << std::hex << (unsigned)this->getUnknown2() << std::dec << std::endl;
    std::cout << "unknown3:     " << std::hex << (unsigned)this->getUnknown3() << std::dec << std::endl;
    std::cout << "maxCharge:    " << (unsigned)this->getMaxCharge() << std::endl;
    std::cout << "unknown4:     " << std::hex << (unsigned)this->getUnknown4() << std::dec << std::endl;
    std::cout << "unknown5:     " << std::hex << (unsigned)this->getUnknown5() << std::dec << std::endl;
    std::cout << "pause:        " << (unsigned)this->getPause() << std::endl;
    std::cout << "capacity:     " << this->getCapacity() << std::endl;
    std::cout << "discharge:    " << (unsigned)this->getDischarge() << std::endl;
    std::cout << "unknown6:     " << std::hex << (unsigned)this->getUnknown6() << std::dec << std::endl;
    std::cout << "unknown7:     " << std::hex << (unsigned)this->getUnknown7() << std::dec << std::endl;
    std::cout << "crc:          " << std::hex << this->getCrc() << std::dec << std::endl;
}

void DatMessage::printSummary() const
{
    std::cout << (unsigned)this->getSlotId() << ": " << this->getProgramStateStr() << ": " << this->getStepStr() << std::endl;
}

uint16_t DatMessage::getCounter() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[0], 2);
    return htons(temp); // sequence counter is little endian unlike other fields!
}

uint8_t DatMessage::getSlotId() const {
    return this->buffer[2];
}

std::string DatMessage::getSlotIdStr() const {
    switch(getSlotId()) {
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
            std::cout << "unknown slot number: " << (unsigned)getSlotId() << std::endl;
            return "Unknown";
    }
}

uint8_t DatMessage::getChemistry() const {
    return this->buffer[3];
}

std::string DatMessage::getChemistryStr() const {
    switch(getChemistry()) {
        case 0:
            // the slot is empty
            return "-";
        case 1:
            return "NiHM/Cd";
        case 2:
            return "NiZn";
        default:
            std::cout << "unknown chemistry state: " << (unsigned)getChemistry() << std::endl;
            return "Unknown";
    }
}

uint8_t DatMessage::getUnknown1() const {
    return this->buffer[4];
}

uint8_t DatMessage::getProgramState() const {
    return this->buffer[5];
}

std::string DatMessage::getProgramStateStr() const {
    return programAsString(getProgramState());
}

uint8_t DatMessage::getProgram() const {
    return this->buffer[6];
}

std::string DatMessage::getProgramStr() const {
    return programAsString(getProgram());
}

std::string DatMessage::programAsString(uint8_t program) const {
    switch(program) {
        case 0:
            // the slot is empty
            return "-";
        case 1:
            return "Recharge";
        case 2:
            return "Discharge";
        case 3:
            return "Pro-charge";
        case 4:
            return "Cycle";
        case 5:
            return "Alive";
        case 6:
            return "Maximize";
        case 7:
            return "No setup";
        //case 8:
        //    return "Unknown(8)";
        //case 9:
        //    return "Unknown(9)";
        case 10:
            return "Error";
        case 11:
            return "Complete";
        default:
            std::cout << "unknown program state: " << (unsigned)program << std::endl;
            return "Unknown";
    }
}


uint8_t DatMessage::getStep() const {
    return this->buffer[7];
}

std::string DatMessage::getStepStr() const {
     switch(getStep()) {
        case 0:
            return "Idle";
        case 1:
            return "Charging";
        case 2:
            return "Discharging";
        case 3:
            return "Ready";
        case 4:
            return "Unknown";
        case 5:
            return "Cool Down";
        case 6:
            return "Error";
        default:
            std::cout << "unknown step state: " << (unsigned)getStep() << std::endl;
            return "Unknown";
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
    return std::to_string(hours) + std::string(":") + std::to_string(minutes);
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

uint8_t DatMessage::getUnknown2() const {
    return this->buffer[22];
}

uint8_t DatMessage::getUnknown3() const {
    return this->buffer[23];
}

uint8_t DatMessage::getMaxCharge() const {
    return this->buffer[24];
}

uint8_t DatMessage::getUnknown4() const {
    return this->buffer[25];
}

uint8_t DatMessage::getUnknown5() const {
    return this->buffer[26];
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

uint8_t DatMessage::getUnknown6() const {
    return this->buffer[31];
}

uint8_t DatMessage::getUnknown7() const {
    return this->buffer[32];
}

uint16_t DatMessage::getCrc() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[33], 2);
    return temp;
}
