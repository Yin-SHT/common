#ifndef __VPU_INSTR_H
#define __VPU_INSTR_H

#include "instr_base.h"


enum VpuAluType {
    BF16_ALU = 0,
    SCALAR_ALU,
    FP32_ALU,
    INT32_ALU,
    FP16_ALU,
};

enum VpuDataType {
    VPU_INT8 = 0,
    VPU_UINT8,
    VPU_BF16,
    VPU_INT16,
    VPU_UINT16,
    VPU_FP32,
    VPU_INT32,
    VPU_FP8_52,
    VPU_FP8_43,
    VPU_FP16,
};

enum VPUOpcode {
    V_INVALID,
    V_NOP,
    V_OP_END,
    SET_LOAD_BASE,
    SET_LOAD2_BASE,
    SET_STORE_BASE,
    SET_LOAD_GLB,
    SET_STORE_GLB,
    SET_QUANTIZE,
    SET_DEQUANTIZE,
    SET_DEQUANTIZE2,
    SET_FS,
    SET_CMASK_HIGH,
    V_LOAD_DUAL_GLB,
    V_LOAD_GLB,
    V_LOAD_CONST,
    V_STORE_GLB,
    V_ADD,
    V_SUB,
    V_MUL,
    V_MIN,
    V_MAX,
    V_MOV,
    V_REDUCE_SUM,
    V_GREATER,
    V_GREATER_EQUAL,
    V_LESS,
    V_LESS_EQUAL,
    V_EQUAL,
    V_SEL,
    V_LUI,
    V_LI,

    S_ADDI,
    S_ADDIU,
    S_SUBI,
    S_ANDI,
    S_ORI,
    S_XORI,
    S_SGTI,
    S_SLTI,
    S_SEQI,
    S_SGEI,
    S_SLEI,
    S_SRAI,
    S_SRLI,
    S_SLLI,
    S_LUI,
    S_ADD,
    S_ADDU,
    S_SUB,
    S_MUL,
    S_MULH,
    S_MULHU,
    S_MIN,
    S_MAX,
    S_MINU,
    S_MAXU,
    S_AND,
    S_OR,
    S_XOR,
    S_SGT,
    S_SLT,
    S_SEQ,
    S_SGE,
    S_SLE,
    S_CMPSEL,
    S_SRA,
    S_SRL,
    S_SLL,
    S_JUMP,
    S_JAL,
    S_JR,
    S_BNE,
    S_BEQ,
    S_BLT,
    S_BLTU,
    S_BGE,
    S_BGEU,
    S_SETCFG,
    S_SETRF,
};

enum ScalarSetCfg {
    S_SET_LOAD_STRIDE = 0,
    S_SET_LOAD_CLUSTER_MSK,
    S_SET_LOAD_BANK_MSK,
    S_SET_LOAD_CLUSTER_BRD,
    S_SET_LOAD_BANK_BRD,
    S_SET_LOAD_BASE0,
    S_SET_LOAD_DQUAN0,
    S_SET_LOAD_BASE1,
    S_SET_LOAD_DQUAN1,
    S_SET_STORE_BASE,
    S_SET_STORE_STRIDE,
    S_SET_STORE_CLUSTER_MSK,
    S_SET_STORE_BANK_MSK,
    S_SET_STORE_QUAN,
    S_SET_LOAD_CHANN_BRD,
    S_SET_BF16_OR_FP16,
};

enum VpuDataMode {
    DATA_MODE_TENSOR = 0,
    DATA_MODE_WEIGHT_DENSE,
    DATA_MODE_WEIGHT_VALUE,
    DATA_MODE_WEIGHT_INDEX,
};

namespace antoum {
class VPUInstrFields : public VPUInstrFields_t {
public:
    VPUInstrFields(std::shared_ptr<MFPara> para) : VPUInstrFields_t(para) {
    }
    virtual ~VPUInstrFields() = default;

    virtual std::shared_ptr<VPUInstrFields> Clone() {
        auto ret = CREATE_CLASS_DPC(VPUInstrFields_t, _para, VPUInstrFields);
        *ret = *this;
        return ret;
    }

    uint32_t op;
    uint32_t op2;
    uint32_t op3;
    
    uint32_t base_addr : 24;
    uint32_t stride : 3;
    uint32_t cluster_mask : 8;
    uint32_t bank_mask : 4;
    uint32_t cluster_broadcast : 2;
    uint32_t bank_broadcast : 3;
    uint32_t quantize : 16;
    uint32_t dequantize : 16;

    uint32_t rs : 3;
    uint32_t rt : 3;
    uint32_t rd : 3;
    uint32_t imm : 16;
    uint32_t offset : 16;
    uint32_t is_bf16 : 1;
    uint32_t is_unsigned : 1;
    uint32_t has_imm : 1;
    uint32_t upper : 1;
    uint32_t is_int16 : 1;
};

class VPUInstruction : public VPUInstruction_t {
public:
    VPUOpcode opcode;
    VPUOpcode opcode2;
    VPUOpcode opcode3;
    std::shared_ptr<VPUInstrFields> fields;

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
        SET(quantize);
        SET(dequantize);
        SET(rs);
        SET(rt);
        SET(rd);
        SET(imm);
        SET(offset);
        SET(is_bf16);
        SET(is_unsigned);
        SET(has_imm);
        SET(upper);
        SET(is_int16);
        #undef SET

        opcode = VPUOpcode(fields->op);
        opcode2 = VPUOpcode(fields->op2);
        opcode3 = VPUOpcode(fields->op3);
    }

    virtual void GetFields(std::map<std::string, uint32_t> &field_map) {
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
        GET(quantize);
        GET(dequantize);
        GET(rs);
        GET(rt);
        GET(rd);
        GET(imm);
        GET(offset);
        GET(is_bf16);
        GET(is_unsigned);
        GET(has_imm);
        GET(upper);
        GET(is_int16);
        #undef GET
    }

    VPUInstruction(std::shared_ptr<MFPara> para) : VPUInstruction_t(para) {
        fields = CREATE_CLASS_DPC(VPUInstrFields_t, _para, VPUInstrFields);
        SetFields({});
    }
    virtual ~VPUInstruction() = default;

    virtual std::shared_ptr<VPUInstruction> Clone() {
        auto ret = CREATE_CLASS_DPC(VPUInstruction_t, _para, VPUInstruction);
        ret->opcode = this->opcode;
        ret->opcode2 = this->opcode2;
        ret->opcode3 = this->opcode3;
        ret->fields = this->fields->Clone();
        return ret;
    }

    virtual void DumpFields(std::map<std::string, uint32_t> &field_map) {
        GetFields(field_map);
    }
};

struct VPUOperation : public VPUOperation_t {
public:
    VPUOperation(std::shared_ptr<MFPara> para) : VPUOperation_t(para) {
    }
    virtual ~VPUOperation() = default;
    virtual std::shared_ptr<VPUOperation> Clone() {
        auto ret = CREATE_CLASS_DPC(VPUOperation_t, _para, VPUOperation);
        ret->opcode = this->opcode;
        ret->fields = this->fields->Clone();
        ret->latency = this->latency;
        ret->params = this->params;
        return ret;
    }

    VPUOpcode opcode;
    std::shared_ptr<VPUInstrFields> fields;
    int latency;
    std::map<std::string, std::shared_ptr<void>> params;
};
}

#endif
