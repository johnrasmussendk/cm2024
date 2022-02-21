#include "supmessage.h"

#include <arpa/inet.h>

uint8_t SupMessage::getUnknown1() const {
    return this->buffer[0];
}

uint8_t SupMessage::getUnknown2() const {
    return this->buffer[1];
}

uint8_t SupMessage::getUnknown3() const {
    return this->buffer[2];
}

uint8_t SupMessage::getUnknown4() const {
    return this->buffer[3];
}

uint8_t SupMessage::getUnknown5() const {
    return this->buffer[4];
}

uint8_t SupMessage::getUnknown6() const {
    return this->buffer[5];
}

uint8_t SupMessage::getUnknown7() const {
    return this->buffer[6];
}

uint16_t SupMessage::getCounter() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[7], 2);
    return htons(temp); // sequence counter is little endian unlike other fields!
}

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

uint8_t SupMessage::getUnknown8() const {
    return this->buffer[18];
}

uint8_t SupMessage::getUnknown9() const {
    return this->buffer[19];
}

uint8_t SupMessage::getUnknown10() const {
    return this->buffer[20];
}

uint8_t SupMessage::getSdSlotStatus() const {
    return this->buffer[21];
}

uint8_t SupMessage::getUnknown11() const {
    return this->buffer[22];
}

uint8_t SupMessage::getUnknown12() const {
    return this->buffer[23];
}

uint8_t SupMessage::getUnknown13() const {
    return this->buffer[24];
}

uint8_t SupMessage::getUnknown14() const {
    return this->buffer[25];
}

uint8_t SupMessage::getUnknown15() const {
    return this->buffer[26];
}

uint8_t SupMessage::getUnknown16() const {
    return this->buffer[27];
}

uint8_t SupMessage::getUnknown17() const {
    return this->buffer[28];
}

uint8_t SupMessage::getUnknown18() const {
    return this->buffer[29];
}

uint8_t SupMessage::getUnknown19() const {
    return this->buffer[30];
}

uint8_t SupMessage::getUnknown20() const {
    return this->buffer[31];
}

uint8_t SupMessage::getUnknown21() const {
    return this->buffer[32];
}

uint16_t SupMessage::getCrc() const {
    uint16_t temp = 0;
    memcpy(&temp, &this->buffer[33], 2);
    return temp;
}
