#ifndef __VPU_INSTR_QUARK_H
#define __VPU_INSTR_QUARK_H

#include "antoum/instr_desc/vpu_instr.h"

namespace quark {
class VPUInstrFields : public antoum::VPUInstrFields {
public:
    VPUInstrFields(std::shared_ptr<MFPara> para) : antoum::VPUInstrFields(para) {
    }
    virtual ~VPUInstrFields() = default;

    virtual std::shared_ptr<antoum::VPUInstrFields> Clone() {
        auto ret = CREATE_CLASS_DPC(VPUInstrFields_t, _para, VPUInstrFields);
        *ret = *this;
        //return ret;
        return std::dynamic_pointer_cast<antoum::VPUInstrFields>(ret); 
    }

    uint32_t op;
    uint32_t op2;
    uint32_t op3;

    uint32_t base_addr : 24;
    uint32_t stride : 3;
    uint32_t cluster_mask : 16;
    uint32_t bank_mask : 4;
    uint32_t cluster_broadcast : 3;
    uint32_t bank_broadcast : 3;
    uint32_t channel_broadcast : 2;
    uint32_t quantize : 16;
    uint32_t dequantize : 24;
    //uint32_t dequantize;

    uint32_t rs : 5;
    uint32_t rt : 5;
    uint32_t rd : 5;
    //uint32_t imm : 16;
    //uint32_t offset : 16;
    //uint32_t has_imm : 1;
    //uint32_t upper : 1;
    uint32_t dtype;
    uint32_t alu;

    uint32_t sp_rd0 : 5;
    uint32_t sp_rd1 : 5;
    uint32_t src_num : 1;
    uint32_t rf_sel : 3;

    uint32_t fs_sel  : 8;                       /////add by v2.0
    uint32_t consecutive_reg : 1;

};

class VPUInstruction : public antoum::VPUInstruction {
public:
    VPUInstruction(std::shared_ptr<MFPara> para=nullptr) : antoum::VPUInstruction(para) {
        fields = std::dynamic_pointer_cast<quark::VPUInstrFields>(antoum::VPUInstruction::fields);
        SetFields({});
    }
    virtual ~VPUInstruction() = default;
    std::shared_ptr<quark::VPUInstrFields> fields;
    virtual void SetFields(const std::map<std::string, uint32_t> &field_map) {
        #define SET(x) do { auto it = field_map.find(#x);  fields->x = (it != field_map.end()) ?it->second :0; } while(0)
        SET(op);
        SET(op2);
        SET(op3);
        SET(base_addr);
        SET(stride);
        SET(cluster_mask);
        SET(bank_mask);
        SET(cluster_broadcast);
        SET(bank_broadcast);
        SET(channel_broadcast);
        SET(quantize);
        SET(dequantize);
        SET(rs);
        SET(rt);
        SET(rd);
        SET(imm);
        SET(offset);
        SET(has_imm);
        SET(upper);
        SET(dtype);
        SET(alu);
        SET(sp_rd0);
        SET(sp_rd1);
        SET(src_num);
        SET(rf_sel);
        SET(fs_sel);
        SET(consecutive_reg);
        #undef SET

        //opcode = VPUOpcode(fields2->op);
        //opcode2 = VPUOpcode(fields2->op2);
        //opcode3 = VPUOpcode(fields2->op3);
        opcode = VPUOpcode(fields->op);
        opcode2 = VPUOpcode(fields->op2);
        opcode3 = VPUOpcode(fields->op3);
    }

    virtual void GetFields(std::map<std::string, uint32_t> &field_map) {
        //auto fields2 = std::dynamic_pointer_cast<VPUInstrFieldsV2_0>(this->fields);
        //fields2->op = opcode;
        //fields2->op2 = opcode2;
        //fields2->op3 = opcode3;

        //#define GET(x) field_map[#x] = fields2->x;
        fields->op = opcode;
        fields->op2 = opcode2;
        fields->op3 = opcode3;

        #define GET(x) field_map[#x] = fields->x;
        GET(op);
        GET(op2);
        GET(op3);
        GET(base_addr);
        GET(stride);
        GET(cluster_mask);
        GET(bank_mask);
        GET(cluster_broadcast);
        GET(bank_broadcast);
        GET(channel_broadcast);
        GET(quantize);
        GET(dequantize);
        GET(rs);
        GET(rt);
        GET(rd);
        GET(imm);
        GET(offset);
        GET(has_imm);
        GET(upper);
        GET(dtype);
        GET(alu);
        GET(sp_rd0);
        GET(sp_rd1);
        GET(src_num);
        GET(rf_sel);
        GET(fs_sel);
        GET(consecutive_reg);
        #undef GET
    }

    virtual std::shared_ptr<antoum::VPUInstruction> Clone() {
        auto ret = CREATE_CLASS_DPC(VPUInstruction_t, _para, VPUInstruction);
        ret->opcode = this->opcode;
        ret->opcode2 = this->opcode2;
        ret->opcode3 = this->opcode3;
        ret->fields = std::dynamic_pointer_cast<quark::VPUInstrFields>(this->fields->Clone());
        return std::dynamic_pointer_cast<antoum::VPUInstruction>(ret);
    }

    virtual void DumpFields(std::map<std::string, uint32_t> &field_map) {
        GetFields(field_map);
    }
};


struct VPUOperation : public antoum::VPUOperation {
    public:
        VPUOperation(std::shared_ptr<MFPara> para=nullptr) : antoum::VPUOperation(para) {
            fields = std::dynamic_pointer_cast<quark::VPUInstrFields>(antoum::VPUOperation::fields);
        }
        virtual ~VPUOperation() = default;

        virtual std::shared_ptr<antoum::VPUOperation> Clone() {
            auto ret = CREATE_CLASS_DPC(VPUOperation_t, _para, VPUOperation);
            ret->opcode = this->opcode;
            ret->fields = std::dynamic_pointer_cast<quark::VPUInstrFields>(this->fields->Clone());
            ret->latency = this->latency;
            ret->params = this->params;
            //return ret;
            return std::dynamic_pointer_cast<antoum::VPUOperation>(ret);
        }

        std::shared_ptr<quark::VPUInstrFields> fields;
        //VPUInstrFieldsV2_0 fields;
};

}

#endif
