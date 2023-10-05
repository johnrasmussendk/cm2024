#ifndef SUPMESSAGE_HPP_INCLUDED
#define SUPMESSAGE_HPP_INCLUDED

#include "message.h"
#include <QMetaType>

class SupMessage : public Message {

    using Message::Message;

public:
    uint8_t getSlotId() const;
    uint8_t getChemistry() const;
    uint8_t getProgram() const;
    uint8_t getMaxCharge() const;
    uint8_t getDischargeRate() const;
    uint16_t getCapacity() const;
    uint8_t getSdCardStatus() const;
    uint8_t getCoolMinutes() const;
    uint8_t getSdSlotStatus() const;
    uint16_t getCrc() const;

    void print_format(const char format) const;
    void print() const;
};

Q_DECLARE_METATYPE(SupMessage);

#endif // SUPMESSAGE_HPP_INCLUDED
