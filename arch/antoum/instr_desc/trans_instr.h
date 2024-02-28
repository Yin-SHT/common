#ifndef __TRANS_INSTR_H
#define __TRANS_INSTR_H

#include "instr_base.h"

namespace antoum {
struct TEInstrFields : public TEInstrFields_t {
    TEInstrFields(std::shared_ptr<MFPara> para=nullptr) : TEInstrFields_t(para){

    } 
    virtual ~TEInstrFields() = default;
    virtual std::shared_ptr<TEInstrFields> Clone() {
        auto ret = CREATE_CLASS_DPC(TEInstrFields_t, _para, TEInstrFields);
        *ret = *this;
        return ret;
    }                 
    uint32_t SRC_START_ADDR : 20;
    uint32_t DST_START_ADDR : 20;
    uint32_t MODE : 2;
    uint32_t CONFIG_IDX : 3;
    uint32_t OP_END : 1;
};

class TEInstruction : public TEInstruction_t {
public:
    std::string opcode;
    std::shared_ptr<TEInstrFields> fields;

    virtual void SetFields(const std::map<std::string, uint32_t> &field_map) {
        #define SET(x) do { auto it = field_map.find(#x);  fields->x = (it != field_map.end()) ?it->second :fields->x; } while(0)
        SET(SRC_START_ADDR);
        SET(DST_START_ADDR);
        SET(MODE);
        SET(CONFIG_IDX);
        SET(OP_END);
        #undef SET
    }

    virtual void GetFields(std::map<std::string, uint32_t> &field_map) {
        #define GET(x) field_map[#x] = fields->x;
        GET(SRC_START_ADDR);
        GET(DST_START_ADDR);
        GET(MODE);
        GET(CONFIG_IDX);
        GET(OP_END);
        #undef GET
    }

    TEInstruction(std::shared_ptr<MFPara> para=nullptr) : TEInstruction_t(para) {
        fields = CREATE_CLASS_DPC(TEInstrFields_t,para,TEInstrFields);
        SetFields({});
    }
    virtual ~TEInstruction() = default;
    virtual std::shared_ptr<TEInstruction> Clone() {
        auto ret = CREATE_CLASS_DPC(TEInstruction_t, _para, TEInstruction);
        ret->opcode = this->opcode;
        ret->fields = this->fields->Clone();
        return ret;
    }
    virtual void DumpFields(std::map<std::string, uint32_t> &field_map) {
        GetFields(field_map);
    }
};

struct TEConfigFields: public TEConfigFields_t {
    TEConfigFields(std::shared_ptr<MFPara> para=nullptr) : TEConfigFields_t(para) {
    }
    virtual ~TEConfigFields() = default;
    virtual std::shared_ptr<TEConfigFields> Clone() {
        auto ret = CREATE_CLASS_DPC(TEConfigFields_t, _para, TEConfigFields);
        *ret = *this;
        return ret;
    }   
    uint32_t SRC_STRIDE_0;
    uint32_t SRC_STRIDE_1;
    uint32_t SRC_STRIDE_2;
    uint32_t SRC_STRIDE_3;
    uint32_t SRC_STRIDE_4;
    uint32_t SRC_STEPBACK_STRIDE_0;
    uint32_t SRC_STEPBACK_STRIDE_1;
    uint32_t SRC_STEPBACK_STRIDE_2;
    uint32_t SRC_STEPBACK_STRIDE_3;
    uint32_t SRC_STEPBACK_STRIDE_4;
    uint32_t DST_STRIDE_0;
    uint32_t DST_STRIDE_1;
    uint32_t DST_STRIDE_2;
    uint32_t DST_STRIDE_3;
    uint32_t DST_STRIDE_4;
    uint32_t DST_STEPBACK_STRIDE_0;
    uint32_t DST_STEPBACK_STRIDE_1;
    uint32_t DST_STEPBACK_STRIDE_2;
    uint32_t DST_STEPBACK_STRIDE_3;
    uint32_t DST_STEPBACK_STRIDE_4;
    uint32_t WRITE_AXIS_SEGMENTS;
    uint32_t READ_AXIS_SEGMENTS;
    uint32_t WRITE_STRIDE;
    uint32_t READ_STRIDE;
    uint32_t GROUP_NUM;
    uint32_t CONTINUOUS_UNIT_SEGMENTS;
};

struct TEConfiguration: public TEConfiguration_t {
    std::string opcode = "NOP";
    std::shared_ptr<TEConfigFields> fields;
    void SetFields(const std::map<std::string, uint32_t> &field_map) {
        #define SET(x) do { auto it = field_map.find(#x);  fields->x = (it != field_map.end()) ?it->second :fields->x; } while(0)
        SET(SRC_STRIDE_0);
        SET(SRC_STRIDE_1);
        SET(SRC_STRIDE_2);
        SET(SRC_STRIDE_3);
        SET(SRC_STRIDE_4);
        SET(SRC_STEPBACK_STRIDE_0);
        SET(SRC_STEPBACK_STRIDE_1);
        SET(SRC_STEPBACK_STRIDE_2);
        SET(SRC_STEPBACK_STRIDE_3);
        SET(SRC_STEPBACK_STRIDE_4);
        SET(DST_STRIDE_0);
        SET(DST_STRIDE_1);
        SET(DST_STRIDE_2);
        SET(DST_STRIDE_3);
        SET(DST_STRIDE_4);
        SET(DST_STEPBACK_STRIDE_0);
        SET(DST_STEPBACK_STRIDE_1);
        SET(DST_STEPBACK_STRIDE_2);
        SET(DST_STEPBACK_STRIDE_3);
        SET(DST_STEPBACK_STRIDE_4);
        SET(WRITE_AXIS_SEGMENTS);
        SET(READ_AXIS_SEGMENTS);
        SET(WRITE_STRIDE);
        SET(READ_STRIDE);
        SET(GROUP_NUM);
        SET(CONTINUOUS_UNIT_SEGMENTS);
        #undef SET
    }

    void GetFields(std::map<std::string, uint32_t> &field_map) {
        #define GET(x) field_map[#x] = fields->x;
        GET(SRC_STRIDE_0);
        GET(SRC_STRIDE_1);
        GET(SRC_STRIDE_2);
        GET(SRC_STRIDE_3);
        GET(SRC_STRIDE_4);
        GET(SRC_STEPBACK_STRIDE_0);
        GET(SRC_STEPBACK_STRIDE_1);
        GET(SRC_STEPBACK_STRIDE_2);
        GET(SRC_STEPBACK_STRIDE_3);
        GET(SRC_STEPBACK_STRIDE_4);
        GET(DST_STRIDE_0);
        GET(DST_STRIDE_1);
        GET(DST_STRIDE_2);
        GET(DST_STRIDE_3);
        GET(DST_STRIDE_4);
        GET(DST_STEPBACK_STRIDE_0);
        GET(DST_STEPBACK_STRIDE_1);
        GET(DST_STEPBACK_STRIDE_2);
        GET(DST_STEPBACK_STRIDE_3);
        GET(DST_STEPBACK_STRIDE_4);
        GET(WRITE_AXIS_SEGMENTS);
        GET(READ_AXIS_SEGMENTS);
        GET(WRITE_STRIDE);
        GET(READ_STRIDE);
        GET(GROUP_NUM);
        GET(CONTINUOUS_UNIT_SEGMENTS);
        #undef GET
    }

    TEConfiguration(std::shared_ptr<MFPara> para=nullptr) :TEConfiguration(std::map<std::string, uint32_t>(),para) {
        SetFields({});
    }
    virtual ~TEConfiguration() = default;
    TEConfiguration(const std::map<std::string, uint32_t> &init_fields,std::shared_ptr<MFPara> para=nullptr):TEConfiguration_t(para) {
        fields = CREATE_CLASS_DPC(TEConfigFields_t, para, TEConfigFields);
        SetFields(init_fields); }
    virtual std::shared_ptr<TEConfiguration> Clone() {
        auto ret = CREATE_CLASS_DPC(TEConfiguration_t, _para, TEConfiguration);
        ret->opcode = opcode;
        ret->fields = fields->Clone();
        return ret;
    }
};

}

#endif
