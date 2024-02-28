#ifndef __TOPK_INSTR_H
#define __TOPK_INSTR_H

#include "instr_base.h"

namespace antoum {
struct TKEInstrFields : public TKEInstrFields_t{
    TKEInstrFields(std::shared_ptr<MFPara> para=nullptr) : TKEInstrFields_t(para){
    } 
    virtual ~TKEInstrFields() = default;
    virtual std::shared_ptr<TKEInstrFields> Clone() {
        auto ret = CREATE_CLASS_DPC(TKEInstrFields_t, _para, TKEInstrFields);
        *ret = *this;
        return ret;
    }                    
    uint32_t SRC_VALUE_START_ADDR : 20;
    uint32_t SRC_INDEX_START_ADDR : 20;
    uint32_t DST_VALUE_START_ADDR : 20;
    uint32_t DST_INDEX_START_ADDR : 20;
    uint32_t N : 15;
    uint32_t K : 11;
    uint32_t READ_MASK_RPT_CYCLE : 7;
    uint32_t FUNC_MODE : 1;
    uint32_t PRECISION_MODE : 1;
    uint32_t OP_END : 1;
};

class TKEInstruction : public TKEInstruction_t {
public:
    std::string opcode;
    std::shared_ptr<TKEInstrFields> fields;

    virtual void SetFields(const std::map<std::string, uint32_t> &field_map) {
        #define SET(x) do { auto it = field_map.find(#x);  fields->x = (it != field_map.end()) ?it->second :0; } while(0)
        SET(SRC_VALUE_START_ADDR);
        SET(SRC_INDEX_START_ADDR);
        SET(DST_VALUE_START_ADDR);
        SET(DST_INDEX_START_ADDR);
        SET(N);
        SET(K);
        SET(READ_MASK_RPT_CYCLE);
        SET(FUNC_MODE);
        SET(PRECISION_MODE);
        SET(OP_END);  
        #undef SET
    }

    virtual void GetFields(std::map<std::string, uint32_t> &field_map) {
        #define GET(x) field_map[#x] = fields->x;
        GET(SRC_VALUE_START_ADDR);
        GET(SRC_INDEX_START_ADDR);
        GET(DST_VALUE_START_ADDR);
        GET(DST_INDEX_START_ADDR);
        GET(N);
        GET(K);
        GET(READ_MASK_RPT_CYCLE);
        GET(FUNC_MODE);
        GET(PRECISION_MODE);
        GET(OP_END); 
        #undef GET
    }

    TKEInstruction(std::shared_ptr<MFPara> para=nullptr) : TKEInstruction_t(para) {
        fields = CREATE_CLASS_DPC(TKEInstrFields_t,para, TKEInstrFields); 
        SetFields({});
    }
    virtual ~TKEInstruction() = default;
    virtual std::shared_ptr<TKEInstruction> Clone() {
        auto ret = CREATE_CLASS_DPC(TKEInstruction_t, _para, TKEInstruction);
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
