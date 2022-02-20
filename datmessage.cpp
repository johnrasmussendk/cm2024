#include "datmessage.h"
#include <iostream>
#include <string.h>
#include <arpa/inet.h>

DatMessage::DatMessage(char* buffer, int16_t len) {
    this->buffer = new char[len];
    memcpy(this->buffer, buffer, len);
}

DatMessage::~DatMessage() {
    delete [] this->buffer;
}


void DatMessage::print()
{
    std::cout << "sc:           " << this->getSC() << std::endl;
    std::cout << "slot:         " << (unsigned)this->getSlot() << std::endl;
    std::cout << "chemistry:    " << (unsigned)this->getChemistry() << std::endl;
    std::cout << "unknown1:     " << std::hex << (unsigned)this->getUnknown1() << std::dec << std::endl;
    std::cout << "programState: " << (unsigned)this->getProgramState() << std::endl;
    std::cout << "program:      " << (unsigned)this->getProgram() << std::endl;
    std::cout << "step:         " << (unsigned)this->getStep() << std::endl;
    std::cout << "minutes:      " << this->getMinutes() << std::endl;
    std::cout << "voltage:      " << this->getVoltage()/1000.0 << std::endl;
    std::cout << "current:      " << this->getCurrent()/1000.0 << std::endl;
    std::cout << "ccap:         " << this->getChargeCap()/100.0 << std::endl;
    std::cout << "dcap:         " << this->getDischargeCap()/100.0 << std::endl;
    std::cout << "unknown2:     " << std::hex << (unsigned)this->getUnknown2() << std::dec << std::endl;
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

uint16_t DatMessage::getSC() {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[0], 2);
    return htons(temp); // sequence counter is little endian unlike other fields!
}

uint8_t DatMessage::getSlot() {
    return this->buffer[2];
}

uint8_t DatMessage::getChemistry() {
    return this->buffer[3];
}

uint8_t DatMessage::getUnknown1() {
    return this->buffer[4];
}

uint8_t DatMessage::getProgramState() {
    return this->buffer[5];
}

uint8_t DatMessage::getProgram() {
    return this->buffer[6];
}

uint8_t DatMessage::getStep() {
    return this->buffer[7];
}

uint16_t DatMessage::getMinutes() {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[8], 2);
    return temp;
}

// in units of 0.001
uint16_t DatMessage::getVoltage() {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[10], 2);
    return temp;
}

// in units of 0.001
uint16_t DatMessage::getCurrent() {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[12], 2);
    return temp;
}

// in units of 0.01
uint32_t DatMessage::getChargeCap() {
    uint32_t temp = 0;
    memcpy(&temp, &this->buffer[14], 4);
    return temp;
}

// in units of 0.01
uint32_t DatMessage::getDischargeCap() {
    uint32_t temp = 0;
    memcpy(&temp, &this->buffer[18], 4);
    return temp;
}

uint8_t DatMessage::getUnknown2() {
    return this->buffer[22];
}

uint8_t DatMessage::getUnknown3() {
    return this->buffer[23];
}

uint8_t DatMessage::getMaxCharge() {
    return this->buffer[24];
}

uint8_t DatMessage::getUnknown4() {
    return this->buffer[25];
}

uint8_t DatMessage::getUnknown5() {
    return this->buffer[26];
}

uint8_t DatMessage::getPause() {
    return this->buffer[27];
}

uint16_t DatMessage::getCapacity() {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[28], 2);
    return temp;
}

uint8_t DatMessage::getDischarge() {
    return this->buffer[30];
}

uint8_t DatMessage::getUnknown6() {
    return this->buffer[31];
}

uint8_t DatMessage::getUnknown7() {
    return this->buffer[32];
}

uint16_t DatMessage::getCrc() {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[33], 2);
    return temp;
}

