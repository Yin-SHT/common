#ifndef __SPU_ENGINE_UTILS_QUARK_H
#define __SPU_ENGINE_UTILS_QUARK_H

#include "defs.h"
#include "addr_map.h"

namespace quark {

#define ASIC_SPU_L1_INPUT_CACHE_SIZE 8192 // 64W*64C*2B
#define ASIC_SPU_L1_MASK_CACHE_SIZE 1024 // 2*64W*8B
#define ASIC_SPU_L1_WEIGHT_CACHE_SIZE 8192 // 64C*64K*2B
#define ASIC_SPU_L1_ACC_PAGE_SIZE 16384 // 64W*64K*4B
#define ASIC_SPU_GLB_RD_PORT1_BW 1024
#define ASIC_SPU_GLB_RD_PORT2_BW 256
#define ASIC_SPU_GLB_WR_BW 512
#define ASIC_SPU_REG_WIDTH 4 // byte
#define ASIC_SPU_REG_WORD_SIZE 2 // byte
#define ASIC_SPU_REG_DEPTH 256
#define ASIC_SPU_REG_LATENCY 1
#define ASIC_SPU_L1_MASK_CACHE_K_OFFSET (ASIC_SPU_L1_MASK_CACHE_SIZE/2) // 512B
#define ASIC_SPU_ADDR_GRAN 16
#define ASIC_SPU_LENGTH_GRAN 16
#define ASIC_SPU_STRIDE_GRAN 16
#define ASIC_SPU_C_PROCS_NUM 16
#define ASIC_SPU_K_PROCS_NUM 16
#define ASIC_SPU_P_PROCS_NUM 8
#define ASIC_SPU_DEFAULT_BANK_SIZE 64
#define ASIC_SPU_INSTR_PIPELINE_NUM 5
#define ASIC_SPU_QNT_MODE 0 // 0:K dimension, 1:C*K dimension

const std::map<std::string,std::map<std::string,uint32_t>> spuRegIdxMapping = { // TBD
    {"ADDRESS", {
                {"SRC_TSR",     SpuSetIndexMap::SpuSrcTsrAddr0},
                {"SRC_WGT",     SpuSetIndexMap::SpuSrcWgtAddr0},
                {"SRC_ACP",     SpuSetIndexMap::SpuSrcAcpAddr0},
                {"SRC_MI",      SpuSetIndexMap::SpuSrcMiAddr0},
                {"SRC_MO",      SpuSetIndexMap::SpuSrcMoAddr0},
                {"SRC_SCALE_0", SpuSetIndexMap::SpuSrcScale0Addr0},
                {"SRC_SCALE_1", SpuSetIndexMap::SpuSrcScale1Addr0},
                {"SRC_SCALE_2", SpuSetIndexMap::SpuSrcScale2Addr0},
                {"SRC_ACC",     SpuSetIndexMap::SpuSrcAccAddr0},
                {"DST_TSR",     SpuSetIndexMap::SpuDstTsrAddr0}
                }}, // type10 x loop8
    {"STRIDE",  {
                {"SRC_TSR",     SpuSetIndexMap::SpuSrcTsrStride0},
                {"SRC_WGT",     SpuSetIndexMap::SpuSrcWgtStride0},
                {"SRC_ACP",     SpuSetIndexMap::SpuSrcAcpStride0},
                {"SRC_MI",      SpuSetIndexMap::SpuSrcMiStride0},
                {"SRC_MO",      SpuSetIndexMap::SpuSrcMoStride0},
                {"SRC_SCALE_0", SpuSetIndexMap::SpuSrcScale0Stride0},
                {"SRC_SCALE_1", SpuSetIndexMap::SpuSrcScale1Stride0},
                {"SRC_SCALE_2", SpuSetIndexMap::SpuSrcScale2Stride0},
                {"SRC_ACC",     SpuSetIndexMap::SpuSrcAccStride0},
                {"DST_TSR",     SpuSetIndexMap::SpuDstTsrStride0}
                }}, // type10 x loop8
    {"LENGTH",  {
                {"SRC_TSR",     SpuSetIndexMap::SpuSrcTsrLength0},
                {"SRC_WGT",     SpuSetIndexMap::SpuSrcWgtLength},
                {"SRC_ACP",     SpuSetIndexMap::SpuSrcAcpLength0},
                {"SRC_MI",      SpuSetIndexMap::SpuSrcMiLength0},
                {"SRC_MO",      SpuSetIndexMap::SpuSrcMoLength0},
                {"SRC_SCALE_0", SpuSetIndexMap::SpuSrcScale0Length},
                {"SRC_SCALE_1", SpuSetIndexMap::SpuSrcScale1Length0},
                {"SRC_SCALE_2", SpuSetIndexMap::SpuSrcScale2Length0},
                {"SRC_BIAS",    SpuSetIndexMap::SpuSrcBiasLength},
                {"SRC_ACC",     SpuSetIndexMap::SpuSrcAccLength0},
                {"DST_TSR",     SpuSetIndexMap::SpuDstTsrLength0}
                }},  // type11 x 2
    {"BANK_STRIDE", {
                {"SRC_TSR",     SpuSetIndexMap::SpuSrcTsrBankStride},
                {"SRC_WGT",     SpuSetIndexMap::SpuSrcWgtBankStride},
                {"SRC_ACP",     SpuSetIndexMap::SpuSrcAcpBankStride},
                {"SRC_MI",      SpuSetIndexMap::SpuSrcMiBankStride}
                }},
    {"CTRL",    {
                {"LOOP_COUNT",  SpuSetIndexMap::258},
                }}
};

const std::map<std::string, uint32_t> spuRegAddrMaskEncode = {
    {"SRC_TSR",     7},
    {"SRC_WGT",     6},
    {"SRC_MI",      5},
    {"SRC_MO",      4},
    {"SRC_SCALE_0", 3},
    {"SRC_ACC",     2},
    {"DST_TSR",     1},
    {"NOT_DEFINE",  0}
};

const std::map<uint32_t, std::string> spuRegAddrMaskDecode = {
    {7,    "SRC_TSR"},
    {6,    "SRC_WGT"},
    {5,    "SRC_MI"},
    {4,    "SRC_MO"},
    {3,    "SRC_SCALE_0"},
    {2,    "SRC_ACC"},
    {1,    "DST_TSR"},
    {0,    "NOT_DEFINE"}
};

const std::map<DataType,uint32_t> spuDtypeEncode = {
    {DTYPE_INT8,    0},
    {DTYPE_FP8_52,  1},
    {DTYPE_FP8_43,  2},
    {DTYPE_BF16,    3},
    {DTYPE_FP16,    4},
    {DTYPE_FP32,    5},
    {DTYPE_INT32,   6},
    {DTYPE_INT4,    7},
    {DTYPE_INT2,    8}
};

const std::map<uint32_t,DataType> spuDtypeDecode = {
    {0, DTYPE_INT8  },
    {1, DTYPE_FP8_52},
    {2, DTYPE_FP8_43},
    {3, DTYPE_BF16  },
    {4, DTYPE_FP16  },
    {5, DTYPE_FP32  },
    {6, DTYPE_INT32 },
    {7, DTYPE_INT4  },
    {8, DTYPE_INT2  }
};

const std::map<uint32_t,uint32_t> spuBankEncode = {
    {16, 0},
    {32, 1},
    {64, 2}
};

const std::map<uint32_t,uint32_t> spuBankDecode = {
    {0, 16},
    {1, 32},
    {2, 64}
};

const std::map<uint32_t,uint32_t> spuMaskBankEncode = {
    {1, 0},
    {2, 1},
    {4, 2},
    {8, 3}
};

const std::map<uint32_t,uint32_t> spuMaskBankDecode = {
    {0, 1},
    {1, 2},
    {2, 4},
    {3, 8}
};

const std::map<uint32_t,uint32_t> spuNNZEncode = {
    {16, 0},
    {32, 1},
    {48, 2},
    {64, 3}
};

const std::map<uint32_t,uint32_t> spuNNZDecode = {
    {0, 16},
    {1, 32},
    {2, 48},
    {3, 64}
};

const std::map<uint32_t,uint32_t> spuLDMGroupElementEncode = {
    {1, 0},
    {2, 1},
    {4, 2}
};

const std::map<uint32_t,uint32_t> spuLDMGroupElementDecode = {
    {0, 1},
    {1, 2},
    {2, 4}
};

} // namespace quark
#endif // __SPU_ENGINE_UTILS_QUARK_H
