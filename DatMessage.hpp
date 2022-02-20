#ifndef DATMESSAGE_HPP_INCLUDED
#define DATMESSAGE_HPP_INCLUDED

#include <stdint.h>

class DatMessage {
public:
    DatMessage();
    void parse(char *message);
    void print();

private:
    uint16_t sc;
    uint8_t slot;
    uint8_t chemistry;
    uint8_t unknown1;
    uint8_t programState;
    uint8_t step;
    uint16_t minutes;
    uint16_t voltage; // in units of 0.001
    uint16_t current; // in units of 0.001
    uint32_t ccap;    // in units of 0.01
    uint32_t dcap;    // in units of 0.01
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t maxCharge;
    uint8_t unknown4;
    uint8_t unknown5;
    uint8_t pause;
    uint16_t capacity;
    uint8_t discharge;
    uint8_t unknown6;
    uint8_t unknown7;
    uint16_t crc;
};

#endif // DATMESSAGE_HPP_INCLUDED
