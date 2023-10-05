#include "supmessage.h"

#include <arpa/inet.h>
#include <iostream>

uint8_t SupMessage::getSlotId() const {
    return this->buffer[9];
}

uint8_t SupMessage::getChemistry() const {
    return this->buffer[10];
}

uint8_t SupMessage::getProgram() const {
    return this->buffer[11];
}

uint8_t SupMessage::getMaxCharge() const {
    return this->buffer[12];
}

uint8_t SupMessage::getDischargeRate() const {
    return this->buffer[13];
}

uint16_t SupMessage::getCapacity() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[14], 2);
    return htons(temp);
}

uint8_t SupMessage::getSdCardStatus() const {
    return this->buffer[16];
}

uint8_t SupMessage::getCoolMinutes() const {
    return this->buffer[17];
}

uint8_t SupMessage::getSdSlotStatus() const {
    return this->buffer[21];
}

uint16_t SupMessage::getCrc() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[33], 2);
    return temp;
}

void SupMessage::print_format(const char format) const {
    if (format == 'd') {
        printBuf();
        print();
    }
}

void SupMessage::print() const {
    std::cout << "unknown SUP bytes: ";
    for(int i=0; i<9; i++) {
        printf("%02x ", (unsigned char)this->buffer[i]);
    }
    std::cout << std::endl;
    std::cout << "slotId :        " << (unsigned) getSlotId() << std::endl;
    std::cout << "chemistry:      " << (unsigned) getChemistry() << std::endl;
    std::cout << "program:        " << (unsigned) getProgram() << std::endl;
    std::cout << "max charge:     " << (unsigned) getMaxCharge() << std::endl;
    std::cout << "discharge rate: " << (unsigned) getDischargeRate() << std::endl;
    std::cout << "capacity:       " << (unsigned) getCapacity() << std::endl;
    std::cout << "sd card status: " << (unsigned) getSdCardStatus() << std::endl;
    std::cout << "cool minutes:   " << (unsigned) getCoolMinutes() << std::endl;

    std::cout << "padding:        ";
    for(int i=18; i<21; i++) {
        printf("%02x ", (unsigned char)this->buffer[i]);
    }
    std::cout << std::endl;

    std::cout << "sd slot status: " << (unsigned) getSdSlotStatus() << std::endl;

    std::cout << "unknown SUP bytes: ";
    for(int i=22; i<33; i++) {
        printf("%02x ", (unsigned char)this->buffer[i]);
    }
    std::cout << std::endl;
    std::cout << "crc:          " << std::hex << this->getCrc() << std::dec << std::endl;
    std::cout << std::endl;
}
