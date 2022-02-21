#ifndef SUPMESSAGE_HPP_INCLUDED
#define SUPMESSAGE_HPP_INCLUDED

#include "message.h"
#include <QMetaType>

class SupMessage : public Message {

    using Message::Message;

    uint8_t getUnknown1() const;
    uint8_t getUnknown2() const;
    uint8_t getUnknown3() const;
    uint8_t getUnknown4() const;
    uint8_t getUnknown5() const;
    uint8_t getUnknown6() const;
    uint8_t getUnknown7() const;
    uint16_t getCounter() const;
    uint8_t getSlotId() const;
    uint8_t getChemistry() const;
    uint8_t getProgram() const;
    uint8_t getMaxCharge() const;
    uint8_t getDischargeRate() const;
    uint16_t getCapacity() const;
    uint8_t getSdCardStatus() const;
    uint8_t getCoolMinutes() const;
    uint8_t getUnknown8() const;
    uint8_t getUnknown9() const;
    uint8_t getUnknown10() const;
    uint8_t getSdSlotStatus() const;
    uint8_t getUnknown11() const;
    uint8_t getUnknown12() const;
    uint8_t getUnknown13() const;
    uint8_t getUnknown14() const;
    uint8_t getUnknown15() const;
    uint8_t getUnknown16() const;
    uint8_t getUnknown17() const;
    uint8_t getUnknown18() const;
    uint8_t getUnknown19() const;
    uint8_t getUnknown20() const;
    uint8_t getUnknown21() const;
    uint16_t getCrc() const;

};

Q_DECLARE_METATYPE(SupMessage);

#endif // SUPMESSAGE_HPP_INCLUDED
