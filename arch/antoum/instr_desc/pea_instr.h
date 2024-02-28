#ifndef __PEA_INSTR_H
#define __PEA_INSTR_H

#include "instr_base.h"

namespace antoum {
enum PEAOpcode {
    CONV2D,
    PEA_OP_END,
};

struct PEAConfigFields{
    uint32_t EMPTY;
};

struct PEAConfiguration {
    std::string opcode;
    PEAConfigFields fields;

    void SetFields(const std::map<std::string, uint32_t> &field_map) {
        #define SET(x) do { auto it = field_map.find(#x);  fields.x = (it != field_map.end()) ?it->second :fields.x; } while(0)
        SET(EMPTY);
        #undef SET
    }

    void GetFields(std::map<std::string, uint32_t> &field_map) {
        #define GET(x) field_map[#x] = fields.x;
        GET(EMPTY);
        #undef GET
    }

    PEAConfiguration() :PEAConfiguration(std::map<std::string, uint32_t>()) {}
    PEAConfiguration(const std::map<std::string, uint32_t> &fields) { SetFields(fields); }
};

struct PEAInstrFields : public PEAInstrFields_t {
    PEAInstrFields(std::shared_ptr<MFPara> para=nullptr) : PEAInstrFields_t(para) {
    } 
    virtual ~PEAInstrFields() = default;
    virtual std::shared_ptr<PEAInstrFields> Clone() {
        auto ret = CREATE_CLASS_DPC(PEAInstrFields_t, _para, PEAInstrFields);
        *ret = *this;
        return ret;
    }       
                                           //Description                                         Total Bits
    uint32_t SRC_START_ADDR : 20;               //Starting address of the src                         0~16 MB               //                                                    0~127
    uint32_t SRC_LINE_STRIDE : 15;              //Byte granularity (multiple of 512B)                 0~2^13-1
    uint32_t SRC_SURFACE_NUM : 7;              //                                                    0~127
    uint32_t SRC_SURFACE_STRIDE : 19;           //Byte granularity (multiple of 512B)                 0~2^16-1
    uint32_t WEIGHT_BUF_START_ADDR : 15;        //Address up to 8 MB Global Weight Buffer             0~2^23
    uint32_t WEIGHT_LINE_LENGTH : 10;
    uint32_t WEIGHT_LINE_NUM : 5;
    uint32_t WEIGHT_LINE_STRIDE : 3;
    uint32_t WEIGHT_SUF_NUM : 7;
    uint32_t WEIGHT_SUF_STRIDE : 5;
    uint32_t DST_START_ADDR : 20;               //Starting address of the dst                         0~16 MB
    uint32_t DST_LINE_LENGTH : 12;              //Byte granularity                                    0~2^16-1
    uint32_t DST_LINE_STRIDE : 15;              //Byte granularity (multiple of 512B)                 0~2^13-1
    uint32_t DST_SURFACE_NUM : 7;              //                                                    0~127
    uint32_t DST_SURFACE_STRIDE : 19;           //Byte granularity (multiple of 512B)                 0~2^16-1
    uint32_t DST_MODE : 2;
    uint32_t Rq : 2;                           //Conv Filter Size, R quotient                        0~3
    uint32_t Rr : 2;                           //Conv Filter Size, R remainder, R=Rq*3+Rr            0~2
    uint32_t Sq : 2;                           //Conv Filter Size, S quotient                        0~3
    uint32_t Sr : 2;                           //Conv Filter Size, S remainder, S=Sq*3+Sr            0~2
    uint32_t Cgb : 3;                          //GLB buffer read channel number
    uint32_t Cgb_wr : 3;                       //GLB buffer write channel number                     
    uint32_t Kq : 8;                           //Output channel number quotient                      0~2^10
    uint32_t Kr : 6;                           //Output channel number remainder, K= 8(Kg*Kq+Kr)     0~7
    uint32_t Kr_bcast : 1;
    uint32_t Kg_pt : 3;
    uint32_t Cq : 8;                           //Input channel number quotient                       0~2^12
    uint32_t Cr : 3;                           //Input channel remainder C= Cgb(Ncol_ch*Cq+Cr)       0~7
    uint32_t nnz_cgb_cq : 6;
    uint32_t nnz_cgb_cr : 6;
    uint32_t FILTER_STRIDE : 1;
    uint32_t PAD_TOP_MARGIN : 3;
    uint32_t PAD_BOTTOM_MARGIN : 3;
    uint32_t PAD_LEFT_MARGIN : 3;
    uint32_t PAD_RIGHT_MARGIN : 3;
    uint32_t ZERO_PAD_TOP_EN : 1;
    uint32_t ZERO_PAD_BOTTOM_EN : 1;
    uint32_t ZERO_PAD_LEFT_EN : 1;
    uint32_t ZERO_PAD_RIGHT_EN : 1;
    uint32_t WIDTH : 12;
    uint32_t HEIGHT : 12;
    uint32_t PRECISION_MODE : 1;
    uint32_t BIAS_ADD_EN : 1;
    uint32_t ACTIVATION_EN : 1;
    uint32_t ELEMENT_OP_EN : 1;
    uint32_t ACTIVATION_FUNC : 1;
    uint32_t ACTIVATION_TABLE_START : 1;
    uint32_t BIAS_START_ADDR : 15;
    uint32_t QUANT_SCALE_START_ADDR : 15;
    uint32_t DEQUANT_SCALE_START_ADDR : 15;
    uint32_t ELEMENT_OP_SRC_START_ADDR : 20;
    uint32_t USE_PARTIAL_SUM_EN : 1;
    uint32_t GEN_FINAL_SUM_EN : 1;
    uint32_t DST_VALUE_WR_MASK : 4;
    uint32_t WEIGHT_DENSE_ENABLE: 1;
    uint32_t OP_END : 1;
    
};

class PEAInstruction : public PEAInstruction_t {
public:
    std::string opcode;
    std::shared_ptr<PEAInstrFields> fields;

    virtual void SetFields(const std::map<std::string, uint32_t> &field_map) {
        #define SET(x) do { auto it = field_map.find(#x);  fields->x = (it != field_map.end()) ?it->second :fields->x; } while(0)
        SET(SRC_START_ADDR);
        SET(SRC_LINE_STRIDE);
        SET(SRC_SURFACE_NUM);
        SET(SRC_SURFACE_STRIDE);
        SET(WEIGHT_BUF_START_ADDR);
        SET(WEIGHT_LINE_LENGTH);
        SET(WEIGHT_LINE_NUM);
        SET(WEIGHT_LINE_STRIDE);
        SET(WEIGHT_SUF_NUM);
        SET(WEIGHT_SUF_STRIDE);
        SET(DST_START_ADDR);
        SET(DST_LINE_LENGTH);
        SET(DST_LINE_STRIDE);
        SET(DST_SURFACE_NUM);
        SET(DST_SURFACE_STRIDE);
        SET(DST_MODE);
        SET(Rq);
        SET(Rr);
        SET(Sq);
        SET(Sr);
        SET(Cgb);
        SET(Cgb_wr);
        SET(Kq);
        SET(Kr);
        SET(Kr_bcast);
        SET(Kg_pt);
        SET(Cq);
        SET(Cr);
        SET(nnz_cgb_cq);
        SET(nnz_cgb_cr);
        SET(FILTER_STRIDE);
        SET(PAD_TOP_MARGIN);
        SET(PAD_BOTTOM_MARGIN);
        SET(PAD_LEFT_MARGIN);
        SET(PAD_RIGHT_MARGIN);
        SET(ZERO_PAD_TOP_EN);
        SET(ZERO_PAD_BOTTOM_EN);
        SET(ZERO_PAD_LEFT_EN);
        SET(ZERO_PAD_RIGHT_EN);
        SET(WIDTH);
        SET(HEIGHT);
        SET(PRECISION_MODE);
        SET(BIAS_ADD_EN);
        SET(ACTIVATION_EN);
        SET(ELEMENT_OP_EN);
        SET(ACTIVATION_FUNC);
        SET(ACTIVATION_TABLE_START);
        SET(BIAS_START_ADDR);
        SET(QUANT_SCALE_START_ADDR);
        SET(DEQUANT_SCALE_START_ADDR);       
        SET(ELEMENT_OP_SRC_START_ADDR);
        SET(USE_PARTIAL_SUM_EN);
        SET(GEN_FINAL_SUM_EN);
        SET(DST_VALUE_WR_MASK);
        SET(WEIGHT_DENSE_ENABLE);
        SET(OP_END);
        #undef SET
    }

    virtual void GetFields(std::map<std::string, uint32_t> &field_map) {
        #define GET(x) field_map[#x] = fields->x;
        GET(SRC_START_ADDR);
        GET(SRC_LINE_STRIDE);
        GET(SRC_SURFACE_NUM);
        GET(SRC_SURFACE_STRIDE);
        GET(WEIGHT_BUF_START_ADDR);
        GET(WEIGHT_LINE_LENGTH);
        GET(WEIGHT_LINE_NUM);
        GET(WEIGHT_LINE_STRIDE);
        GET(WEIGHT_SUF_NUM);
        GET(WEIGHT_SUF_STRIDE);
        GET(DST_START_ADDR);
        GET(DST_LINE_LENGTH);
        GET(DST_LINE_STRIDE);
        GET(DST_SURFACE_NUM);
        GET(DST_SURFACE_STRIDE);
        GET(DST_MODE);
        GET(Rq);
        GET(Rr);
        GET(Sq);
        GET(Sr);
        GET(Cgb);
        GET(Cgb_wr);
        GET(Kq);
        GET(Kr);
        GET(Kr_bcast);
        GET(Kg_pt);
        GET(Cq);
        GET(Cr);
        GET(nnz_cgb_cq);
        GET(nnz_cgb_cr);
        GET(FILTER_STRIDE);
        GET(PAD_TOP_MARGIN);
        GET(PAD_BOTTOM_MARGIN);
        GET(PAD_LEFT_MARGIN);
        GET(PAD_RIGHT_MARGIN);
        GET(ZERO_PAD_TOP_EN);
        GET(ZERO_PAD_BOTTOM_EN);
        GET(ZERO_PAD_LEFT_EN);
        GET(ZERO_PAD_RIGHT_EN);
        GET(WIDTH);
        GET(HEIGHT);
        GET(PRECISION_MODE);
        GET(BIAS_ADD_EN);
        GET(ACTIVATION_EN);
        GET(ELEMENT_OP_EN);
        GET(ACTIVATION_FUNC);
        GET(ACTIVATION_TABLE_START);
        GET(BIAS_START_ADDR);
        GET(QUANT_SCALE_START_ADDR);
        GET(DEQUANT_SCALE_START_ADDR);       
        GET(ELEMENT_OP_SRC_START_ADDR);
        GET(USE_PARTIAL_SUM_EN);
        GET(GEN_FINAL_SUM_EN);
        GET(DST_VALUE_WR_MASK);
        GET(WEIGHT_DENSE_ENABLE);
        GET(OP_END);
        #undef GET
    }

    PEAInstruction(std::shared_ptr<MFPara> para=nullptr) : PEAInstruction_t(para) {
        fields = CREATE_CLASS_DPC(PEAInstrFields_t, para, PEAInstrFields);
        SetFields({});
    }
    virtual ~PEAInstruction() = default;
    virtual std::shared_ptr<PEAInstruction> Clone() {
        auto ret = CREATE_CLASS_DPC(PEAInstruction_t, _para, PEAInstruction);
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
