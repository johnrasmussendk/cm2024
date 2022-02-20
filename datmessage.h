#ifndef DATMESSAGE_HPP_INCLUDED
#define DATMESSAGE_HPP_INCLUDED

#include <QMetaType>
#include <stdint.h>


class DatMessage {
public:
    DatMessage();
    DatMessage(char* buffer, int16_t len);
    DatMessage(const DatMessage &other);
    ~DatMessage();

    uint16_t getSC() const;
    uint8_t getSlot() const;
    uint8_t getChemistry() const;
    uint8_t getUnknown1() const;
    uint8_t getProgramState() const;
    uint8_t getProgram() const;
    uint8_t getStep() const;
    uint16_t getMinutes() const;
    uint16_t getVoltage() const; // in units of 0.001
    uint16_t getCurrent() const; // in units of 0.001
    uint32_t getChargeCap() const; // in units of 0.01
    uint32_t getDischargeCap() const; // in units of 0.01
    uint8_t getUnknown2() const;
    uint8_t getUnknown3() const;
    uint8_t getMaxCharge() const;
    uint8_t getUnknown4() const;
    uint8_t getUnknown5() const;
    uint8_t getPause() const;
    uint16_t getCapacity() const;
    uint8_t getDischarge() const;
    uint8_t getUnknown6() const;
    uint8_t getUnknown7() const;
    uint16_t getCrc() const;

    void print() const;

    char* getBuffer() const { return buffer; };
    int getBufferLen() const { return bufferLen; };

private:
    char* buffer;
    int bufferLen;
};

Q_DECLARE_METATYPE(DatMessage);

#endif // DATMESSAGE_HPP_INCLUDED
