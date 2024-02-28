#ifndef __ACT_INSTR_H
#define __ACT_INSTR_H

#include "instr_base.h"

namespace antoum {
struct AEInstrFields : public AEInstrFields_t{
    AEInstrFields(std::shared_ptr<MFPara> para=nullptr) : AEInstrFields_t(para){
    } 
    virtual ~AEInstrFields() = default;
    virtual std::shared_ptr<AEInstrFields> Clone() {
        auto ret = CREATE_CLASS_DPC(AEInstrFields_t, _para, AEInstrFields);
        *ret = *this;
        return ret;
    }                     
    uint32_t SRC_START_ADDR : 20;
    uint32_t SRC_LINE_LENGTH : 12;
    uint32_t SRC_LINE_NUM : 11;
    uint32_t SRC_LINE_STRIDE : 15;
    uint32_t SRC_SURFACE_NUM : 7;
    uint32_t SRC_SURFACE_STRIDE: 19;
    uint32_t DST_START_ADDR : 20;
    uint32_t DST_LINE_LENGTH : 12;
    uint32_t DST_LINE_NUM : 11;
    uint32_t DST_LINE_STRIDE : 15;
    uint32_t DST_SURFACE_NUM : 7;
    uint32_t DST_SURFACE_STRIDE : 19;
    uint32_t P_VALUE : 16;
    uint32_t ACTIVATION_FUNC : 4;
    uint32_t OP_END : 1;
};

class AEInstruction : public AEInstruction_t {
public:
    std::string opcode;
    std::shared_ptr<AEInstrFields> fields;

    virtual void SetFields(const std::map<std::string, uint32_t> &field_map) {
        #define SET(x) do { auto it = field_map.find(#x);  fields->x = (it != field_map.end()) ?it->second :fields->x; } while(0)
        SET(SRC_START_ADDR);
        SET(SRC_LINE_LENGTH);
        SET(SRC_LINE_NUM);
        SET(SRC_LINE_STRIDE);
        SET(SRC_SURFACE_NUM);
        SET(SRC_SURFACE_STRIDE);
        SET(DST_START_ADDR);
        SET(DST_LINE_LENGTH);
        SET(DST_LINE_NUM);
        SET(DST_LINE_STRIDE);
        SET(DST_SURFACE_NUM);
        SET(DST_SURFACE_STRIDE);
        SET(P_VALUE);
        SET(ACTIVATION_FUNC);
        SET(OP_END);
        #undef SET
    }

    virtual void GetFields(std::map<std::string, uint32_t> &field_map) {
        #define GET(x) field_map[#x] = fields->x;
        GET(SRC_START_ADDR);
        GET(SRC_LINE_LENGTH);
        GET(SRC_LINE_NUM);
        GET(SRC_LINE_STRIDE);
        GET(SRC_SURFACE_NUM);
        GET(SRC_SURFACE_STRIDE);
        GET(DST_START_ADDR);
        GET(DST_LINE_LENGTH);
        GET(DST_LINE_NUM);
        GET(DST_LINE_STRIDE);
        GET(DST_SURFACE_NUM);
        GET(DST_SURFACE_STRIDE);
        GET(P_VALUE);
        GET(ACTIVATION_FUNC);
        GET(OP_END);
        #undef GET
    }

    AEInstruction(std::shared_ptr<MFPara> para=nullptr) : AEInstruction_t(para) {
        fields = CREATE_CLASS_DPC(AEInstrFields_t, para, AEInstrFields);
        SetFields({});
    }
    virtual ~AEInstruction() = default;
    virtual std::shared_ptr<AEInstruction> Clone() {
        auto ret = CREATE_CLASS_DPC(AEInstruction_t, _para, AEInstruction);
        ret->opcode = this->opcode;
        ret->fields = this->fields->Clone();
        return ret;
    }

    virtual void DumpFields(std::map<std::string, uint32_t> &field_map) {
        GetFields(field_map);
    }
};
}

#endif
