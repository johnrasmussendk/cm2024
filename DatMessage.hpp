#ifndef DATMESSAGE_HPP_INCLUDED
#define DATMESSAGE_HPP_INCLUDED

#include <stdint.h>

class DatMessage {
public:
    DatMessage(char* buffer, int16_t len);
    ~DatMessage();

    uint16_t getSC();
    uint8_t getSlot();
    uint8_t getChemistry();
    uint8_t getUnknown1();
    uint8_t getProgramState();
    uint8_t getProgram();
    uint8_t getStep();
    uint16_t getMinutes();
    uint16_t getVoltage(); // in units of 0.001
    uint16_t getCurrent(); // in units of 0.001
    uint32_t getChargeCap(); // in units of 0.01
    uint32_t getDischargeCap(); // in units of 0.01
    uint8_t getUnknown2();
    uint8_t getUnknown3();
    uint8_t getMaxCharge();
    uint8_t getUnknown4();
    uint8_t getUnknown5();
    uint8_t getPause();
    uint16_t getCapacity();
    uint8_t getDischarge();
    uint8_t getUnknown6();
    uint8_t getUnknown7();
    uint16_t getCrc();

    /*
    void parse(char *message);
    */
    void print();

private:
    /*
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
    */

    char* buffer;
};

#endif // DATMESSAGE_HPP_INCLUDED
