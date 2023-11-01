#ifndef DATMESSAGE_HPP_INCLUDED
#define DATMESSAGE_HPP_INCLUDED

#include "message.h"

#include <QMetaType>
#include <stdint.h>


class DatMessage : public Message {
public:

    using Message::Message;
    DatMessage(char* buffer, int16_t len);

    void print_format(const char format) const;
    void print() const;
    void printSummary() const;

    void validate() const;
    uint16_t getCounter() const;
    uint8_t getSlotId() const;
    void validateSlotId() const;
    static std::string getSlotIdStr(const uint8_t slot_id);
    std::string getSlotIdStr() const;
    uint8_t getCellStackSize() const;
    float getCurrentScale() const;
    uint8_t getCurrentPrecision() const;
    float getChargeCapScale() const;
    uint8_t getChargeCapPrecision() const;
    float getDischargeCapScale() const;
    uint8_t getDischargeCapPrecision() const;
    uint8_t getChemistry() const;
    void validateChemistry() const;
    bool isChemistryEmpty() const;
    bool isChemistryNimhCd() const;
    bool isChemistryNizn() const;
    std::string getChemistryStr() const;
    uint8_t getRunningStateByte() const;
    bool getRunningStateBool() const;
    void validateRunningState() const;
    std::string getRunningStateStr() const;
    uint8_t getProgramState() const;
    void validateProgramState() const;
    bool isProgramStateEmpty() const;
    bool isProgramStateCycle() const;
    bool isProgramStateMaximize() const;
    bool isProgramStateNoSetup() const;
    bool isProgramStateTrickle() const;
    bool isProgramStateError() const;
    bool isProgramStateComplete() const;
    std::string getProgramStateStr() const;
    uint8_t getProgram() const;
    void validateProgram() const;
    bool isProgramDischarge() const;
    bool isProgramMaximize() const;
    bool isProgramError() const;
    std::string getProgramStr(const uint8_t program) const;
    std::string getProgramStr() const;
    uint8_t getStep() const;
    void validateStep() const;
    bool isStepIdle() const;
    bool isStepCharging() const;
    bool isStepDischarging() const;
    bool isStepReadyCharged() const;
    bool isStepReadyDischarged() const;
    bool isStepCoolDown() const;
    bool isStepError() const;
    std::string getStepStr() const;
    uint16_t getMinutes() const;
    std::string getTimerStr() const;
    uint16_t getVoltage() const; // in units of 0.001
    std::string getVoltageStr() const;
    uint16_t getCurrent() const; // in units of 0.001
    std::string getCurrentStr() const;
    uint32_t getChargeCap() const; // in units of 0.01
    std::string getChargeCapStr() const;
    uint32_t getDischargeCap() const; // in units of 0.01
    std::string getDischargeCapStr() const;
    uint8_t getVoltageIndex() const;
    void validateVoltageIndex() const;
    std::string getVoltageIndexStr() const;
    uint16_t getMinimumTypeMilliVoltage(const std::string info, const int16_t voltage_index_type_mv[], const size_t voltage_index_type_mv_sizeof, const uint8_t minimum_voltage_index, const uint8_t voltage_index, const uint8_t cell_stack_size) const;
    uint16_t getMaximumTypeMilliVoltage(const std::string info, const int16_t voltage_index_type_mv[], const size_t voltage_index_type_mv_sizeof, const uint8_t minimum_voltage_index, const uint8_t voltage_index, const uint8_t cell_stack_size) const;
    uint16_t getMinimumChargeMilliVoltage(const uint8_t voltage_index, const uint8_t cell_stack_size) const;
    uint16_t getMaximumChargeMilliVoltage(const uint8_t voltage_index, const uint8_t cell_stack_size) const;
    uint16_t getMinimumDischargeMilliVoltage(const uint8_t voltage_index, const uint8_t cell_stack_size) const;
    uint16_t getMaximumDischargeMilliVoltage(const uint8_t voltage_index, const uint8_t cell_stack_size) const;
    std::string getMilliVoltIntervalString(const char interval_type, const uint16_t minimum_millivolt, const uint16_t maximum_millivolt) const;
    uint8_t getTrickleStateByte() const;
    bool getTrickleStateBool() const;
    void validateTrickleState() const;
    std::string getTrickleStateStr() const;
    uint8_t getMaxCharge() const;
    void validateMaxCharge() const;
    std::string getMaxChargeStr() const;
    uint8_t getSdCardLog() const;
    void validateSdCardLog() const;
    bool isSdCardLogNone() const;
    bool isSdCardLogCompleteDischarged() const;
    bool isSdCardLogComplete() const;
    std::string getSdCardLogStr() const;
    uint8_t getProgramCompletedCount() const;
    std::string getProgramCompletedCountStr() const;
    uint8_t getPause() const;
    uint16_t getCapacity() const;
    uint8_t getDischarge() const;
    void validateDischarge(const std::string info, const uint8_t discharge) const;
    void validateDischarge() const;
    std::string getDischargeStr(const std::string info, const uint8_t discharge) const;
    std::string getDischargeStr() const;
    uint8_t getMaxDischarge() const;
    void validateMaxDischarge() const;
    std::string getMaxDischargeStr() const;
    uint8_t getStepIndex() const;
    void validateStepIndex() const;
    std::string getStepIndexStr() const;
    uint8_t getStepIndexTotal(const uint8_t program) const;
    std::string getStepIndexPrTotalStr() const;
    uint16_t getCrc() const;
};

Q_DECLARE_METATYPE(DatMessage);

#endif // DATMESSAGE_HPP_INCLUDED
