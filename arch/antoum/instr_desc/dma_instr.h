#ifndef __DMA_INSTR_H
#define __DMA_INSTR_H

#include "instr_base.h"


enum DMASrcDstMode {
    DMAMV_DDR_TO_GLB = 0,
    DMAMV_GLB_TO_DDR,
    DMAMV_DDR_TO_DDR,
    DMAMV_GLB_TO_DTCM,
    DMAMV_DTCM_TO_GLB,
    DMAMV_DDR_TO_DTCM,
    DMAMV_DTCM_TO_DDR,
    DMAMV_DDR_TO_CMD,
    DMAMV_DDR_TO_CONST,
    DMAMV_DDR_TO_GLBW,
    DMAMV_DTCM_TO_CMD,
    DMAMV_DTCM_TO_CONST,
    DMAMV_DTCM_TO_GLBW,
};

const uint32_t DMA_MAX_LINE_LENGTH = (1 << 14) * 4;
const uint32_t DMA_MAX_LINE_NUMBER = (1 << 11);
const uint32_t DMA_MAX_SURFACE_NUMBER = (1 << 7);
const uint32_t DMA_MAX_LINE_STRIDE = ((1 << 17) - 1) * 4;
const uint32_t DMA_MAX_SURFACE_STRIDE = ((1 << 21) - 1) * 4;

namespace antoum {
    
class DMAInstruction : public DMAInstruction_t {
public:
    uint64_t OP_CODE;
    uint64_t SRC_START_ADDR;
    uint64_t SRC_LINE_LENGTH;
    uint64_t SRC_LINE_NUMBER;
    uint64_t SRC_LINE_STRIDE;
    uint64_t SRC_SURFACE_NUMBER;
    uint64_t SRC_SURFACE_STRIDE;
    uint64_t SRC_DST_MODE;
    uint64_t CHANNEL_EXT_EN;
    uint64_t TENSOR_FORMAT_CONVERT_EN;
    uint64_t CONST_BROADCAST_DUP_NUM;
    uint64_t TENSOR_Cgb;
    uint64_t DST_START_ADDR;
    uint64_t DST_LINE_LENGTH;
    uint64_t DST_LINE_NUMBER;
    uint64_t DST_LINE_STRIDE;
    uint64_t DST_SURFACE_NUMBER;
    uint64_t DST_SURFACE_STRIDE;
    uint64_t OP_END;

    virtual void SetFields(const std::map<std::string, uint32_t> &field_map) {
        #define SET(x) do { auto it = field_map.find(#x);  x = (it != field_map.end()) ?it->second :x; } while(0)
        SET(SRC_START_ADDR);
        SET(SRC_LINE_LENGTH);
        SET(SRC_LINE_NUMBER);
        SET(SRC_LINE_STRIDE);
        SET(SRC_SURFACE_NUMBER);
        SET(SRC_SURFACE_STRIDE);
        SET(SRC_DST_MODE);
        SET(CHANNEL_EXT_EN);
        SET(TENSOR_FORMAT_CONVERT_EN);
        SET(CONST_BROADCAST_DUP_NUM);
        SET(TENSOR_Cgb);
        SET(DST_START_ADDR);
        SET(DST_LINE_LENGTH);
        SET(DST_LINE_NUMBER);
        SET(DST_LINE_STRIDE);
        SET(DST_SURFACE_NUMBER);
        SET(DST_SURFACE_STRIDE);
        SET(OP_END);
        #undef SET
    }

    virtual void GetFields(std::map<std::string, uint32_t> &field_map) {
        #define GET(x) field_map[#x] = x;
        GET(SRC_START_ADDR);
        GET(SRC_LINE_LENGTH);
        GET(SRC_LINE_NUMBER);
        GET(SRC_LINE_STRIDE);
        GET(SRC_SURFACE_NUMBER);
        GET(SRC_SURFACE_STRIDE);
        GET(SRC_DST_MODE);
        GET(CHANNEL_EXT_EN);
        GET(TENSOR_FORMAT_CONVERT_EN);
        GET(CONST_BROADCAST_DUP_NUM);
        GET(TENSOR_Cgb);
        GET(DST_START_ADDR);
        GET(DST_LINE_LENGTH);
        GET(DST_LINE_NUMBER);
        GET(DST_LINE_STRIDE);
        GET(DST_SURFACE_NUMBER);
        GET(DST_SURFACE_STRIDE);
        GET(OP_END);
        #undef GET
    }

    DMAInstruction(std::shared_ptr<MFPara> para=nullptr) : DMAInstruction_t(para) {
        OP_CODE = 0;
        SRC_START_ADDR = 0;
        SRC_LINE_LENGTH = 0;
        SRC_LINE_NUMBER = 0;
        SRC_LINE_STRIDE = 0;
        SRC_SURFACE_NUMBER = 0;
        SRC_SURFACE_STRIDE = 0;
        SRC_DST_MODE = 0;
        CHANNEL_EXT_EN = 0;
        TENSOR_FORMAT_CONVERT_EN = 0;
        CONST_BROADCAST_DUP_NUM = 0;
        TENSOR_Cgb = 0;
        DST_START_ADDR = 0;
        DST_LINE_LENGTH = 0;
        DST_LINE_NUMBER = 0;
        DST_LINE_STRIDE = 0;
        DST_SURFACE_NUMBER = 0;
        DST_SURFACE_STRIDE = 0;
        OP_END = 0;
    }
    virtual ~DMAInstruction() = default;

    virtual std::shared_ptr<DMAInstruction> Clone() {
        auto ret = CREATE_CLASS_DPC(DMAInstruction_t, _para, DMAInstruction);
        *ret = *this;
        return ret;
    }

    virtual void DumpFields(std::map<std::string, uint32_t> &field_map) {
        GetFields(field_map);
    }
};
}

#endif
