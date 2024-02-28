//=========================================================================
// Moffett AI Inc. CONFIDENTIAL
//
// 2020 - 2025 Moffett AI Inc. All Rights Reserved.
//
//NOTICE:  All information contained herein is, and remains
//the property of Moffett AI Inc. and its suppliers,
//if any.  The intellectual and technical concepts contained
//herein are proprietary to Moffett AI Inc.
//and its suppliers and may be covered by U.S. and Foreign Patents,
//patents in process, and are protected by trade secret or copyright law.
//Dissemination of this information or reproduction of this material
//is strictly forbidden unless prior written permission is obtained
//from Moffett AI Inc.
//=========================================================================
// File Name: wsparse_hw_config_fpga.h
// Author: David Xiao, Enxu Yan
// Date:  02/09/2020
//=========================================================================
#ifndef __HWARDWARE_CONFIG_H
#define __HWARDWARE_CONFIG_H

#include <map>
#include <cassert>
#include <string>
#include <cstdint>

typedef struct {
    std::string name;
    uint64_t begin_addr;
    uint64_t end_addr;
    uint64_t capacity;
    uint64_t cmd_width;
} MemMappingEntry;

typedef enum {
    MM_ATCM = 0,
    MM_BTCM,
    MM_ENGINE_REG,
    MM_ICU_REG,
    MM_TRANSPOSE_REG,
    MM_DMA_REG,
    MM_RESERVED1,
    MM_GLB_C,
    MM_GLB,
    MM_GLB_W,
    MM_GLB_Q,
    MM_GLB_S1 = MM_GLB_Q,
    MM_GLB_DQ,
    MM_GLB_S2 = MM_GLB_DQ,
    MM_GLB_BIAS,
    MM_ACT_LUT,
    MM_VPU_CMD,
    MM_DMA_CMD,
    MM_PEA_CMD,
    MM_GCPY_CMD,
    MM_TRANSPOSE_CMD,
    MM_TOPK_CMD,
    MM_DDMA_CMD,
    MM_ACT_CMD,
    MM_CORE_DDR,
    MM_RESERVED2,
    MM_RESERVED3,
    MM_SYSCFG_REG,
    MM_TZC_REG,
    MM_IPC_CA7_REG,
    MM_IPC_CR5_REG,
    MM_TIMERS_REG,
    MM_UART_REG,
    MM_PP_AFBC_REG,
    MM_RESERVED1_0,
    MM_VPU_REG,
    MM_JPEG_REG,
    MM_PP_REG,
    MM_GLB_S3,
    MM_GLB_M,
    MM_INVALID,
} MemMappingIdx;

typedef std::pair<MemMappingIdx, int64_t> GLBAddress;

const std::map<MemMappingIdx, MemMappingEntry> MemMapping = {
    { MM_ATCM,          { "ATCM",           0,          0xFFFFF,        128UL*1024,             (uint64_t)-1 }},
    { MM_BTCM,          { "BTCM",           0x100000,   0x1FFFFF,       256UL*1024,             (uint64_t)-1 }},
    { MM_ICU_REG,       { "ICU_REG",        0x200000,   0x200FFF,       1UL*1024,               (uint64_t)-1 }},
    { MM_TRANSPOSE_REG, { "TRANS_REG",      0x201000,   0x201FFF,       1UL*1024,               (uint64_t)-1 }},
    { MM_ENGINE_REG,    { "ENGINE_REG",     0x202000,   0x221FFF,       4UL*1024,               (uint64_t)-1 }},
    { MM_DMA_REG,       { "DMA_REG",        0x222000,   0x241FFF,       2UL*1024,               (uint64_t)-1 }},
    { MM_SYSCFG_REG,    { "SYSCFG_REG",     0x242000,   0x242FFF,       4UL*1024,               (uint64_t)-1 }},
    { MM_TZC_REG,       { "TZC_REG",        0x243000,   0x243FFF,       4UL*1024,               (uint64_t)-1 }},
    { MM_IPC_CA7_REG,   { "IPC_CA7_REG",    0x244000,   0x244FFF,       4UL*1024,               (uint64_t)-1 }},
    { MM_IPC_CR5_REG,   { "IPC_CR5_REG",    0x245000,   0x245FFF,       4UL*1024,               (uint64_t)-1 }},
    { MM_TIMERS_REG,    { "TIMERS_REG",     0x246000,   0x246FFF,       4UL*1024,               (uint64_t)-1 }},
    { MM_UART_REG,      { "UART_REG",       0x247000,   0x247FFF,       4UL*1024,               (uint64_t)-1 }},
    { MM_PP_AFBC_REG,   { "PP_AFBC_REG",    0x248000,   0x248FFF,       4UL*1024,               (uint64_t)-1 }},
    { MM_RESERVED1_0,   { "RESERVED1_0",    0x249000,   0x251FFF,       36UL*1024,              (uint64_t)-1 }},
    { MM_VPU_REG,       { "VPU_REG",        0x252000,   0x261FFF,       64UL*1024,              (uint64_t)-1 }},
    { MM_JPEG_REG,      { "JPEG_REG",       0x262000,   0x271FFF,       64UL*1024,              (uint64_t)-1 }},
    { MM_PP_REG,        { "PP_REG",         0x272000,   0x281FFF,       64UL*1024,              (uint64_t)-1 }},
    { MM_RESERVED1,     { "RESERVED1",      0x282000,   0x3FFFFF,       1528UL*1024,            (uint64_t)-1 }},
    { MM_GLB,           { "GLB",            0x400000,   0x13FFFFF,      16UL*1024*1024,         64           }},
    { MM_GLB_W,         { "GLB_W",          0x1400000,  0x1BFFFFF,      (4UL*1024+512)*1024,    144          }},
    { MM_GLB_Q,         { "GLB_Q",          0x1C00000,  0x1C1FFFF,      64UL*1024,              128          }},
    { MM_GLB_DQ,        { "GLB_DQ",         0x1C20000,  0x1C3FFFF,      64UL*1024,              128          }},
    { MM_GLB_BIAS,      { "GLB_BIAS",       0x1C40000,  0x1C7FFFF,      128UL*1024,             256          }},
    { MM_ACT_LUT,       { "ACT_LUT",        0x1C80000,  0x1CFFFFF,      128UL*1024,             64           }},
    { MM_GLB_C,         { "GLB_C",          0x1D00000,  0x1D7FFFF,      128UL*1024,             512          }},
    { MM_VPU_CMD,       { "VPU_CMD",        0x1D80000,  0x1E7FFFF,      256UL*1024,             4            }},
    { MM_DMA_CMD,       { "DMA_CMD",        0x1E80000,  0x1E9FFFF,      64UL*1024,              32           }},
    { MM_PEA_CMD,       { "PEA_CMD",        0x1EA0000,  0x1EBFFFF,      32UL*1024,              48           }},
    { MM_GCPY_CMD,      { "GCPY_CMD",       0x1EC0000,  0x1EDFFFF,      16UL*1024,              32           }},
    { MM_TRANSPOSE_CMD, { "TRANS_CMD",      0x1EE0000,  0x1EFFFFF,      1UL*1024,               8            }},
    { MM_TOPK_CMD,      { "TOPK_CMD",       0x1F00000,  0x1F1FFFF,      1UL*1024,               16           }},
    { MM_DDMA_CMD,      { "DDMA_CMD",       0x1F20000,  0x1F3FFFF,      16UL*1024,              32           }},
    { MM_ACT_CMD,       { "ACT_CMD",        0x1F40000,  0x1F5FFFF,      1UL*1024,               32           }},
    { MM_RESERVED2,     { "RESERVED2",      0x1F60000,  0x1FFFFFF,      640UL*1024,             (uint64_t)-1 }},
    { MM_RESERVED3,     { "RESERVED3",      0x2000000,  0x3FFFFFFF,     992UL*1024*1024,        (uint64_t)-1 }},
    { MM_CORE_DDR,      { "DDR",            0x40000000, 0x13FFFFFFF,    4UL*1024*1024*1024,     16           }},
};

#define CORE_NUM 4
// #define CORE_NUM ((::GetHwVersion()==HW_VER1_5||::GetHwVersion()==HW_VER2_0) ?8 :4)
#define DMA_BANDWITH_TO_DDR GetConfig()->PERF.DDR_BANDWIDTH //(16)
#define DMA_EXTRA_LATENCY (100)
#define DDMA_EXTRA_LATENCY (100)
#define DMA_TRANSFER_LENGTH (8192)

#define ASIC_DDR_SIZE (MemMapping.at(MM_CORE_DDR).capacity)
#define ASIC_DDR_BASE_ADDR (MemMapping.at(MM_CORE_DDR).begin_addr)

#define ASIC_GLB_SIZE (MemMapping.at(MM_GLB).capacity)
#define ASIC_GLB_BASE_ADDR (MemMapping.at(MM_GLB).begin_addr)

#define ASIC_GLB_W_SIZE (MemMapping.at(MM_GLB_W).capacity)
#define ASIC_GLB_W_BASE_ADDR (MemMapping.at(MM_GLB_W).begin_addr)

#define ASIC_GLB_C_SIZE ((MemMapping.find(MM_GLB_C)!=MemMapping.end()) ?MemMapping.at(MM_GLB_C).capacity :0)
#define ASIC_GLB_C_BASE_ADDR ((MemMapping.find(MM_GLB_C)!=MemMapping.end()) ?MemMapping.at(MM_GLB_C).begin_addr :0)

#define ASIC_GLB_M_SIZE ((MemMapping.find(MM_GLB_M)!=MemMapping.end()) ?MemMapping.at(MM_GLB_M).capacity :0)
#define ASIC_GLB_M_BASE_ADDR ((MemMapping.find(MM_GLB_M)!=MemMapping.end()) ?MemMapping.at(MM_GLB_M).begin_addr :0)

#define ASIC_GLB_BIAS_SIZE (MemMapping.at(MM_GLB_BIAS).capacity)
#define ASIC_GLB_BIAS_BASE_ADDR (MemMapping.at(MM_GLB_BIAS).begin_addr)

#define ASIC_GLB_Q_SIZE (MemMapping.at(MM_GLB_Q).capacity)
#define ASIC_GLB_Q_BASE_ADDR (MemMapping.at(MM_GLB_Q).begin_addr)

#define ASIC_GLB_DQ_SIZE (MemMapping.at(MM_GLB_DQ).capacity)
#define ASIC_GLB_DQ_BASE_ADDR (MemMapping.at(MM_GLB_DQ).begin_addr)

#define ASIC_GLB_S3_SIZE ((MemMapping.find(MM_GLB_S3)!=MemMapping.end()) ?MemMapping.at(MM_GLB_S3).capacity :0)
#define ASIC_GLB_S3_BASE_ADDR ((MemMapping.find(MM_GLB_S3)!=MemMapping.end()) ?MemMapping.at(MM_GLB_S3).begin_addr :0)

#define ASIC_ACT_LUT_SIZE (MemMapping.at(MM_ACT_LUT).capacity)
#define ASIC_ACT_LUT_BASE_ADDR (MemMapping.at(MM_ACT_LUT).begin_addr)

#define ASIC_DDMA_REG_BASE_ADDR (MemMapping.at(MM_ENGINE_REG).begin_addr + 0x300)
#define ASIC_TOPK_REG_BASE_ADDR (MemMapping.at(MM_ENGINE_REG).begin_addr + 0x100)

#define ASIC_TRANSPOSE_REG_BASE_ADDR (MemMapping.at(MM_TRANSPOSE_REG).begin_addr)
#define ASIC_TRANSPOSE_REG_SIZE (26*8)

//#define ASIC_DMA_CMD_SIZE (MemMapping.at(MM_DMA_CMD).capacity)
#define ASIC_DMA_CMD_WIDTH (MemMapping.at(MM_DMA_CMD).cmd_width)
#define ASIC_DMA_CMD_BASE_ADDR (MemMapping.at(MM_DMA_CMD).begin_addr)

#define ASIC_DMA_CMD_SIZE (16UL*1024)
#define ASIC_DMA1_CMD_SIZE (16UL*1024)
#define ASIC_DMA2_CMD_SIZE (1UL*1024)
#define ASIC_DMA3_CMD_SIZE (1UL*1024)
#define ASIC_DMA4_CMD_SIZE (1UL*1024)
#define ASIC_DMA5_CMD_SIZE (8UL*1024)
#define ASIC_DMA6_CMD_SIZE (8UL*1024)
#define ASIC_DMA7_CMD_SIZE (8UL*1024)

#define ASIC_GCPY_CMD_SIZE (MemMapping.at(MM_GCPY_CMD).capacity)
#define ASIC_GCPY_CMD_WIDTH (MemMapping.at(MM_GCPY_CMD).cmd_width)
#define ASIC_GCPY_CMD_BASE_ADDR (MemMapping.at(MM_GCPY_CMD).begin_addr)

#define ASIC_PEA_CMD_SIZE (MemMapping.at(MM_PEA_CMD).capacity)
#define ASIC_PEA_CMD_WIDTH (MemMapping.at(MM_PEA_CMD).cmd_width)
#define ASIC_PEA_CMD_BASE_ADDR (MemMapping.at(MM_PEA_CMD).begin_addr)

#define ASIC_VPU_CMD_SIZE (MemMapping.at(MM_VPU_CMD).capacity)
#define ASIC_VPU_CMD_WIDTH (MemMapping.at(MM_VPU_CMD).cmd_width)
#define ASIC_VPU_CMD_BASE_ADDR (MemMapping.at(MM_VPU_CMD).begin_addr)

#define ASIC_TRANSPOSE_CMD_SIZE (MemMapping.at(MM_TRANSPOSE_CMD).capacity)
#define ASIC_TRANSPOSE_CMD_WIDTH (MemMapping.at(MM_TRANSPOSE_CMD).cmd_width)
#define ASIC_TRANSPOSE_CMD_BASE_ADDR (MemMapping.at(MM_TRANSPOSE_CMD).begin_addr)

#define ASIC_TOPK_CMD_SIZE (MemMapping.at(MM_TOPK_CMD).capacity)
#define ASIC_TOPK_CMD_WIDTH (MemMapping.at(MM_TOPK_CMD).cmd_width)
#define ASIC_TOPK_CMD_BASE_ADDR (MemMapping.at(MM_TOPK_CMD).begin_addr)

#define ASIC_ACT_CMD_SIZE (MemMapping.at(MM_ACT_CMD).capacity)
#define ASIC_ACT_CMD_WIDTH (MemMapping.at(MM_ACT_CMD).cmd_width)
#define ASIC_ACT_CMD_BASE_ADDR (MemMapping.at(MM_ACT_CMD).begin_addr)

#define ASIC_DDMA_CMD_SIZE (MemMapping.at(MM_DDMA_CMD).capacity)
#define ASIC_DDMA_CMD_WIDTH (MemMapping.at(MM_DDMA_CMD).cmd_width)
#define ASIC_DDMA_CMD_BASE_ADDR (MemMapping.at(MM_DDMA_CMD).begin_addr)

// due to 512 bytes / cycle, we are doing only 8 not 16 width of the panel


#define FUSED_ENGINE_HANDLE_SIZE 512

#define ASIC_PANEL_GLOB_MEM_SIZE 100000000
#define ASIC_PANEL_PARS_MEM_SIZE 100000000
#define ASIC_PANEL_CONST_BUF_SIZE (2048*8*(4+2+2))
#define ASIC_PANEL_GLB_BANK_SIZE (64*4)
#define ASIC_PANEL_GLB_BANK_BW (16)
#define ASIC_PANEL_GLB_TOTAL_BW (ASIC_PANEL_GLB_BANK_SIZE*ASIC_PANEL_GLB_BANK_BW)

#define ASIC_DATA_GLB_BASE (0)
#define ASIC_WEIGHT_GLB_BASE ((ASIC_DATA_GLB_BASE) + (ASIC_PANEL_GLOB_MEM_SIZE))
#define ASIC_CONST_GLB_BASE ((ASIC_WEIGHT_GLB_BASE) + (ASIC_PANEL_PARS_MEM_SIZE))

#define ASIC_PE_NUM_MUL 8
//64 out of 8 random lookup is expensive, hence only 32 (though it would be nice for sparsity to have >32)
#define ASIC_PE_CIN 32
#define ASIC_PE_COUT 8
#define ASIC_PE_MEM_SIZE ((ASIC_PE_CIN + ASIC_PE_COUT) * sizeof(float))
#define ASIC_PE_CLUSTER_NUM 8



#define ASIC_VPU_REG_SIZE (4)
#define ASIC_VPU_INT8_ELEM (512)
#define ASIC_VPU_BF16_ELEM (ASIC_VPU_INT8_ELEM/2)
#define ASIC_VPU_CLUSTER_SIZE (8)
#define ASIC_VPU_CLUSTER_INT8_LEN (64)
#define ASIC_VPU_CLUSTER_BF16_LEN (ASIC_VPU_CLUSTER_INT8_LEN/2)

#define ASIC_VPU_LOAD_LATENCY (5)
#define ASIC_VPU_STORE_LATENCY (24)
#define ASIC_VPU_REGISTER_LATENCY (5)
#define ASIC_VPU_REDUCESUM_LATENCY (8)

#define ASIC_TE_CACHE_CAPACITY 4096
#define ASIC_TE_BANDWIDTH 512

// PE_Array 
// compiler
#define ASIC_GLBW_CLUSTER_SIZE (144UL*1024)
#define ASIC_GLBW_REGION_SIZE (1152UL*1024)
#define ASIC_GLBW_CLUSTER_NUM 8
#define ASIC_GLBW_REGION_NUM 4

// cmodel
#define PEA_DEFAULT_CGB 64
#define GLB_CLUSTER_BW 128
#define GLBW_CLUSTER_BW ((::GetHwVersion()==HW_VER2_0) ? 256 : ((::GetHwVersion()==HW_VER1_5)? 288 : 144)) // doubled for 2xTOPs in v1p5:change to 2x144; compressed to 2x128 in v2p0
#define GLBW_CLUSTER_CAPACITY ((::GetHwVersion()==HW_VER2_0) ? (128*1024) : (144*1024)) // compressed from 144KB to 128KB in v2p0
#define GLBW_CLUSTER_NUM 8
#define L1_ACC_DEPTH 8

#define CLUSTER_ROW 8
#define CLUSTER_COL 9
#define NUM_CLUSTER 8
#define L1_INPUT_CACHE_BW 1024
#define L1_WEIGHT_CACHE_BW ((::GetHwVersion()==HW_VER2_0) ? 2048 : ((::GetHwVersion()==HW_VER1_5)? 2304 : 1152)) // doubled for 2xTOPs in v1p5, change to 128B aligned in v2p0
#define L1_ACC_CACHE_BW 1536
#define L1_INPUT_CACHE_CAPACITY (320*1024) //320KB
#define L1_WEIGHT_CACHE_CAPACITY ((::GetHwVersion()==HW_VER2_0) ? (128*1024) : (144*1024)) // 144KB, no ping-pong, compressed to 128KB in v2p0
#define L1_ACC_CACHE_CAPACITY ((::GetHwVersion()==HW_VER1_5||::GetHwVersion()==HW_VER2_0) ? (24*1024) : (12*1024)) //12KB, double for v1p5 2xTOPs
#define L1_ACC_CACHE_ELEMENT_CAPACITY (L1_ACC_CACHE_CAPACITY/3)// double for v1p5 2xTOPs
#define L1_INPUT_CACHE_CLUSTER_BW ((L1_INPUT_CACHE_BW)/(NUM_CLUSTER))
#define L1_WEIGHT_CACHE_CLUSTER_BW ((L1_WEIGHT_CACHE_BW)/(NUM_CLUSTER))
#define L1_INPUT_CACHE_CLUSTER_CAPACITY ((L1_INPUT_CACHE_CAPACITY)/(NUM_CLUSTER))
#define L1_WEIGHT_CACHE_CLUSTER_CAPACITY ((L1_WEIGHT_CACHE_CAPACITY)/(NUM_CLUSTER))

// pe
#define ASIC_IBUF_CAPACITY 64
#define ASIC_WBUF_CAPACITY (::GetHwVersion()==HW_VER1_5||::GetHwVersion()==HW_VER2_0 ?32:16) // doubled for 2xTOPs. nnz(2) * pe_cout(int8:8, bf16:4) * datawidth(int8:2B, bf16:4B)

// debugging recommendations
// W - 3
// H - 3
// NUM_MUL - 1
// CIN - 1
// COUT - 1

#endif
