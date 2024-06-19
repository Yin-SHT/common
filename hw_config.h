/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef SPU_HWARDWARE_CONFIG_H
#define SPU_HWARDWARE_CONFIG_H

#include <map>
#include <cassert>
#include <string>
#include <cstdint>

namespace antoum {

enum class MemoryType {
  ATCM = 0,
  BTCM,
  GLB_C,
  GLB,
  GLB_W,
  GLB_Q,
  GLB_DQ,
  GLB_BIAS,
  ACT_LUT,
  CORE_DDR,
  GCPY_CMD, 
  PEA_CMD, 
  VPU_CMD, 
  TRANSPOSE_CMD, 
  TOPK_CMD, 
  ACTIV_CMD,
};

enum class EngineType {
  DMA = 0, 
  GCPY, 
  PEA, 
  VPU, 
  TRANSPOSE, 
  TOPK, 
  ACTIV,
  DMA1,
  DMA2,
  DMA3,
  DMA4,
  DMA5,
  DMA6,
  DMA7,
  SIZE = DMA7,
  INVALID = 255,
};

const size_t kRegionMode = 3;
const size_t kGlbBankSize = 8;
const bool kAssertGlbConflict = true;
const size_t kFp8Format = 0;

const size_t kNumCore = 4;
const size_t kSizeMemPage = 8192;
const size_t kIduTransGranularity = 16;

const size_t kSizeAtcmMem = 128UL*1024;
const size_t kSizeBtcmMem = 128UL*1024;
const size_t kSizeTranposeRegMem = 1UL*1024;
const size_t kSizeGlbMem = 16UL*1024*1024;
const size_t kSizeGlbWMem = (4UL*1024+512)*1024;
const size_t kSizeGlbQMem = 64UL*1024;
const size_t kSizeGlbDqMem = 64UL*1024;
const size_t kSizeGlbBiasMem = 128UL*1024;
const size_t kSizeActLutMem = 128UL*1024;
const size_t kSizeGlbCMem = 128UL*1024;
const size_t kSizeVpuCmdMem = 256UL*1024;
const size_t kSizePeaCmdMem = 32UL*1024;
const size_t kSizeGcpyCmdMem = 16UL*1024;
const size_t kSizeTransposeCmdMem = 1UL*1024;
const size_t kSizeTopkCmdMem = 1UL*1024;
const size_t kSizeActCmdMem = 1UL*1024;
const size_t kSizeDma0CmdMem = 16UL*1024;
const size_t kSizeDma1CmdMem = 16UL*1024;
const size_t kSizeDma2CmdMem = 1UL*1024;
const size_t kSizeDma3CmdMem = 1UL*1024;
const size_t kSizeDma4CmdMem = 1UL*1024;
const size_t kSizeDma5CmdMem = 8UL*1024;
const size_t kSizeDma6CmdMem = 8UL*1024;
const size_t kSizeDma7CmdMem = 8UL*1024;
const size_t kSizeCoreDdr = 4UL << 30;

const size_t kAtcmMemAddr = 0;
const size_t kBtcmMemAddr = 0x100000;
const size_t kSizeTranposeRegAddr = 0x201000;
const size_t kGlbMemAddr = 0x400000;
const size_t kGlbWMemAddr = 0x1400000;
const size_t kGlbQMemAddr = 0x1C00000;
const size_t kGlbDqMemAddr = 0x1C20000;
const size_t kGlbBiasMemAddr = 0x1C40000;
const size_t kActLutMemAddr = 0x1C80000;
const size_t kGlbCMemAddr = 0x1D00000;
const size_t kVpuCmdMemAddr = 0x1D80000;
const size_t kPeaCmdMemAddr = 0x1EA0000;
const size_t kGcpyCmdMemAddr = 0x1EC0000;
const size_t kTransposeCmdMemAddr = 0x1EE0000;
const size_t kTopkCmdMemAddr = 0x1F00000;
const size_t kActCmdMemAddr = 0x1F40000;
const size_t kCoreDdrAddr = 0x40000000;

const size_t kSizePerDmaCmd = 32;
const size_t kSizePerVpuCmd = 4;
const size_t kSizePerPeaCmd = 48;
const size_t kSizePerGcpyCmd = 32;
const size_t kSizePerTransposeCmd = 8;
const size_t kSizePerTopkCmd = 16;
const size_t kSizePerActCmd = 32;

const size_t kNumVpuInnerReg = 4;

const float kDmaBandwidthToDdr = 12;
const size_t kDmaExtraLatency = 100;
const size_t kDmaTransferLength = 8192;

} // namespace antoum

namespace quark {

enum class EngineType {
  SPU = 0, 
  VPU, 
  AE, 
  TE, 
  SE,
  DMA0,
  DMA1,
  DMA2,
  DMA3,
  DMA4,
  DMA5,
  SIZE,
  PELS_I,
  PELS_E,
  PELS_O,
  COLS,
  CHLS,
  INVALID = 255,
};

enum class MemoryType {
  UNKNOWN = 0,
  DDR,
  SHARED_MEM,
  LLC_SPM,
};

struct DmaLinkedNode {
  uint64_t srcAddr : 40;
  uint64_t reserved : 24;
  uint64_t dstAddr : 40;
  uint64_t reserved2 : 24;
  uint64_t length : 12;
  uint64_t reserved3 : 52;
  uint64_t signature : 32;
  uint64_t reserved4 : 1;
  uint64_t pingPong : 1;
  uint64_t broadcastMask : 16;
  uint64_t reserved5 : 13;
  uint64_t end : 1;
};

const size_t kDmaLinkedNodeSignature = 0b10011011000000101001000011011110;
const size_t kNumCore = 4;
const size_t kSizeMemPage = 8192;
const size_t kNumPe = 16;
const size_t kSizeDdr = 192UL << 30;
const size_t kSizeTcm = 256UL << 10;
const size_t kSizeSharedMem = 1UL << 20;
const size_t kSizeLlcSpm = 16UL << 20;
const size_t kSizePerSpuCmd = 4;
const size_t kSizePerVpuCmd = 4;
const size_t kSizePerPelsCmd = 4;
const size_t kSizePerColsCmd = 4;
const size_t kSizePerChlsCmd = 4;
const size_t kIduTransGranularity = 64;

// Sizes of command memmory of each Engine
const size_t kSizeSpuCmdMem = 2UL << 10;
const size_t kSizeVpuCmdMem = 4UL << 10;
const size_t kSizeDmaCmdMem = 2UL << 10;
const size_t kSizeAeCmdMem = 2UL << 10;
const size_t kSizeTeCmdMem = 2UL << 10;
const size_t kSizeSeCmdMem = 2UL << 10;
const size_t kSizePelsCmdMem = 2UL << 10;
const size_t kSizeColsCmdMem = 2UL << 10;
const size_t kSizeChlsCmdMem = 2UL << 10;

// VPU-Engine inner definitions
const size_t kNumVpuVectorReg = 4;
const size_t kVpuVectorRegDepth = 64;
const size_t kNumVpuScalarReg = 32;

// RVV address mapping
const size_t kCoreRvvSysStartAddr = 0x000040000000; const size_t kCoreRvvSysEndAddr   = 0x00005FFFFFFF;  
const size_t kCoreRvvDdrStartAddr = 0x008000000000; const size_t kCoreRvvDdrEndAddr   = 0x017FFFFFFFFF;
const size_t kPeRvvSysStartAddr   = 0x000040000000; const size_t kPeRvvSysEndAddr     = 0x00005FFFFFFF;
const size_t kPeRvvDdrStartAddr   = 0x008000000000; const size_t kPeRvvDdrEndAddr     = 0x017FFFFFFFFF;

// Chip-Level address mapping
const size_t kChipRegStartAddr    = 0x00000000;     const size_t kChipRegEndAddr      = 0x000FFFFF;
const size_t kCore0StartAddr      = 0x40000000;     const size_t kCore0EndAddr        = 0x4FFFFFFF;
const size_t kCore1StartAddr      = 0x50000000;     const size_t kCore1EndAddr        = 0x5FFFFFFF;
const size_t kCore2StartAddr      = 0x60000000;     const size_t kCore2EndAddr        = 0x6FFFFFFF;
const size_t kCore3StartAddr      = 0x70000000;     const size_t kCore3EndAddr        = 0x7FFFFFFF;
const size_t kDdrStartAddr        = 0x008000000000; const size_t kDdrEndAddr          = 0x017FFFFFFFFF;

// Core-Level address mapping
const size_t kCoreRegStartAddr    = 0x00000000;     const size_t kCoreRegEndAddr      = 0x00000FFF;
const size_t kDdmaRegStartAddr    = 0x00001000;     const size_t kDdmaRegEndAddr      = 0x00001FFF;
const size_t kCdmaRegStartAddr    = 0x00002000;     const size_t kCdmaRegEndAddr      = 0x00002FFF;
const size_t kIduL2StartAddr      = 0x00100000;     const size_t kIduL2EndAddr        = 0x010FFFFF;
const size_t kIduL1StartAddr      = 0x01100000;     const size_t kIduL1EndAddr        = 0x011FFFFF;
const size_t kSmBrdStartAddr      = 0x01200000;     const size_t kSmBrdEndAddr        = 0x013FFFFF;
const size_t kLlcSpmStartAddr     = 0x01400000;     const size_t kLlcSpmEndAddr       = 0x023FFFFF;
const size_t kPe0StartAddr        = 0x08000000;     const size_t kPe0EndAddr          = 0x087FFFFF;
const size_t kPe1StartAddr        = 0x08800000;     const size_t kPe1EndAddr          = 0x08FFFFFF;
const size_t kPe2StartAddr        = 0x09000000;     const size_t kPe2EndAddr          = 0x097FFFFF;
const size_t kPe3StartAddr        = 0x09800000;     const size_t kPe3EndAddr          = 0x09FFFFFF;
const size_t kPe4StartAddr        = 0x0A000000;     const size_t kPe4EndAddr          = 0x0A7FFFFF;
const size_t kPe5StartAddr        = 0x0A800000;     const size_t kPe5EndAddr          = 0x0AFFFFFF;
const size_t kPe6StartAddr        = 0x0B000000;     const size_t kPe6EndAddr          = 0x0B7FFFFF;
const size_t kPe7StartAddr        = 0x0B800000;     const size_t kPe7EndAddr          = 0x0BFFFFFF;
const size_t kPe8StartAddr        = 0x0C000000;     const size_t kPe8EndAddr          = 0x0C7FFFFF;
const size_t kPe9StartAddr        = 0x0C800000;     const size_t kPe9EndAddr          = 0x0CFFFFFF;
const size_t kPe10StartAddr       = 0x0D000000;     const size_t kPe10EndAddr         = 0x0D7FFFFF;
const size_t kPe11StartAddr       = 0x0D800000;     const size_t kPe11EndAddr         = 0x0DFFFFFF;
const size_t kPe12StartAddr       = 0x0E000000;     const size_t kPe12EndAddr         = 0x0E7FFFFF;
const size_t kPe13StartAddr       = 0x0E800000;     const size_t kPe13EndAddr         = 0x0EFFFFFF;
const size_t kPe14StartAddr       = 0x0F000000;     const size_t kPe14EndAddr         = 0x0F8FFFFF;
const size_t kPe15StartAddr       = 0x0F800000;     const size_t kPe15EndAddr         = 0x0FFFFFFF;

// PE-Level address mapping
const size_t kPeRegStartAddr      = 0x00000000;     const size_t kPeRegEndAddr        = 0x00000FFF;
const size_t kRvvRegStartAddr     = 0x00001000;     const size_t kRvvRegEndAddr       = 0x00001FFF;
const size_t kIduRegStartAddr     = 0x00002000;     const size_t kIduRegEndAddr       = 0x00002FFF;
const size_t kIcuRegStartAddr     = 0x00003000;     const size_t kIcuRegEndAddr       = 0x00003FFF;
const size_t kExcepRegStartAddr   = 0x00004000;     const size_t kExcepRegEndAddr     = 0x00004FFF;
const size_t kDebugRegStartAddr   = 0x00005000;     const size_t kDebugRegEndAddr     = 0x00005FFF;
const size_t kSpuRegStartAddr     = 0x00006000;     const size_t kSpuRegEndAddr       = 0x00006FFF;
const size_t kVpuRegStartAddr     = 0x00007000;     const size_t kVpuRegEndAddr       = 0x00007FFF;
const size_t kAeRegStartAddr      = 0x00008000;     const size_t kAeRegEndAddr        = 0x00008FFF;
const size_t kTeRegStartAddr      = 0x00009000;     const size_t kTeRegEndAddr        = 0x00009FFF;
const size_t kSeRegStartAddr      = 0x0000A000;     const size_t kSeRegEndAddr        = 0x0000AFFF;
const size_t kDma0RegStartAddr    = 0x0000B000;     const size_t kDma0RegEndAddr      = 0x0000BFFF;
const size_t kDma1RegStartAddr    = 0x0000C000;     const size_t kDma1RegEndAddr      = 0x0000CFFF;
const size_t kDma2RegStartAddr    = 0x0000D000;     const size_t kDma2RegEndAddr      = 0x0000DFFF;
const size_t kDma3RegStartAddr    = 0x0000E000;     const size_t kDma3RegEndAddr      = 0x0000EFFF;
const size_t kDma4RegStartAddr    = 0x0000F000;     const size_t kDma4RegEndAddr      = 0x0000FFFF;
const size_t kDma5RegStartAddr    = 0x00010000;     const size_t kDma5RegEndAddr      = 0x00010FFF;
const size_t kWdmaRegStartAddr    = 0x00011000;     const size_t kWdmaRegEndAddr      = 0x00011FFF;
const size_t kRvvMailBoxStartAddr = 0x00012000;     const size_t kRvvMailBoxEndAddr   = 0x00012FFF;
const size_t kPelsIInstrStartAddr = 0x00080000;     const size_t kPelsIInstrEndAddr   = 0x00081FFF;
const size_t kPelsEInstrStartAddr = 0x00082000;     const size_t kPelsEInstrEndAddr   = 0x00083FFF;
const size_t kPelsOInstrStartAddr = 0x00084000;     const size_t kPelsOInstrEndAddr   = 0x00085FFF;
const size_t kSpuInstrStartAddr   = 0x00088000;     const size_t kSpuInstrEndAddr     = 0x0008FFFF;
const size_t kVpuInstrStartAddr   = 0x00090000;     const size_t kVpuInstrEndAddr     = 0x00097FFF;
const size_t kAeInstrStartAddr    = 0x00098000;     const size_t kAeInstrEndAddr      = 0x0009FFFF;
const size_t kTeInstrStartAddr    = 0x000A0000;     const size_t kTeInstrEndAddr      = 0x000A7FFF;
const size_t kSeInstrStartAddr    = 0x000A8000;     const size_t kSeInstrEndAddr      = 0x000AFFFF;
const size_t kDma0InstrStartAddr  = 0x000B0000;     const size_t kDma0InstrEndAddr    = 0x000B1FFF;
const size_t kDma1InstrStartAddr  = 0x000B2000;     const size_t kDma1InstrEndAddr    = 0x000B3FFF;
const size_t kDma2InstrStartAddr  = 0x000B4000;     const size_t kDma2InstrEndAddr    = 0x000B5FFF;
const size_t kDma3InstrStartAddr  = 0x000B6000;     const size_t kDma3InstrEndAddr    = 0x000B7FFF;
const size_t kDma4InstrStartAddr  = 0x000B8000;     const size_t kDma4InstrEndAddr    = 0x000B9FFF;
const size_t kDma5InstrStartAddr  = 0x000BA000;     const size_t kDma5InstrEndAddr    = 0x000BBFFF;
const size_t kTcmStartAddr        = 0x00100000;     const size_t kTcmEndAddr          = 0x001FFFFF;
const size_t kSharedMemStartAddr  = 0x00200000;     const size_t kSharedMemEndAddr    = 0x003FFFFF;

// The following indices are for SET instruction of CHLS.
enum class ChlsSetIndexMap {
  ChipEnCtrl0 = 0,
  ChipEnCtrl1 = 1,
  ChipEnCtrl2 = 2,
  ChipEnCtrl3 = 3,
  ChipEnCtrl4 = 4,
  ChipDoneIntSt0 = 8,
  ChipDoneIntSt1 = 9,
  ChipDoneIntSt2 = 10,
  ChipDoneIntSt3 = 11,
  ChipDoneIntSt4 = 12,
  ChipRvvIrqSt = 16,
  ChipRvvIraSt = 17,
  ChlsInstrConfigL = 20,
  ChlsInstrConfigH = 21,
  Cols0InstrConfigL = 24,
  Cols0InstrConfigH = 25,
  Cols1InstrConfigL = 26,
  Cols1InstrConfigH = 27,
  Cols2InstrConfigL = 28,
  Cols2InstrConfigH = 29,
  Cols3InstrConfigL = 30,
  Cols3InstrConfigH = 31,
  Core0RvPrgmConfigL = 32,
  Core0RvPrgmConfigH = 33,
  Core1RvPrgmConfigL = 34,
  Core1RvPrgmConfigH = 35,
  Core2RvPrgmConfigL = 36,
  Core2RvPrgmConfigH = 37,
  Core3RvPrgmConfigL = 38,
  Core3RvPrgmConfigH = 39,
};

// The following indices are for SET instruction of COLS.
enum class ColsSetIndexMap {
  CoreEnCtrl0 = 0,
  CoreEnCtrl1 = 1,
  CoreEnCtrl2 = 2,
  CoreEnCtrl3 = 3,
  CoreEnCtrl4 = 4,
  CoreEnCtrl5 = 5,
  CoreDoneIntSt0 = 8,
  CoreDoneIntSt1 = 9,
  CoreDoneIntSt2 = 10,
  CoreDoneIntSt3 = 11,
  CoreDoneIntSt4 = 12,
  CoreDoneIntSt5 = 13,
  CoreRvvIrqSt = 16,
  CoreRvvIraSt = 17,
  ColsInstrConfigL = 20,
  ColsInstrConfigH = 21,
  Pels0IInstrConfigL = 24,
  Pels0IInstrConfigH = 25,
  Pels0EInstrConfigL = 26,
  Pels0EInstrConfigH = 27,
  Pels0OInstrConfigL = 28,
  Pels0OInstrConfigH = 29,
  Pels1IInstrConfigL = 32,
  Pels1IInstrConfigH = 33,
  Pels1EInstrConfigL = 34,
  Pels1EInstrConfigH = 35,
  Pels1OInstrConfigL = 36,
  Pels1OInstrConfigH = 37,
  Pels2IInstrConfigL = 40,
  Pels2IInstrConfigH = 41,
  Pels2EInstrConfigL = 42,
  Pels2EInstrConfigH = 43,
  Pels2OInstrConfigL = 44,
  Pels2OInstrConfigH = 45,
  Pels3IInstrConfigL = 48,
  Pels3IInstrConfigH = 49,
  Pels3EInstrConfigL = 50,
  Pels3EInstrConfigH = 51,
  Pels3OInstrConfigL = 52,
  Pels3OInstrConfigH = 53,
  Pels4IInstrConfigL = 56,
  Pels4IInstrConfigH = 57,
  Pels4EInstrConfigL = 58,
  Pels4EInstrConfigH = 59,
  Pels4OInstrConfigL = 60,
  Pels4OInstrConfigH = 61,
  Pels5IInstrConfigL = 64,
  Pels5IInstrConfigH = 65,
  Pels5EInstrConfigL = 66,
  Pels5EInstrConfigH = 67,
  Pels5OInstrConfigL = 68,
  Pels5OInstrConfigH = 69,
  Pels6IInstrConfigL = 72,
  Pels6IInstrConfigH = 73,
  Pels6EInstrConfigL = 74,
  Pels6EInstrConfigH = 75,
  Pels6OInstrConfigL = 76,
  Pels6OInstrConfigH = 77,
  Pels7IInstrConfigL = 80,
  Pels7IInstrConfigH = 81,
  Pels7EInstrConfigL = 82,
  Pels7EInstrConfigH = 83,
  Pels7OInstrConfigL = 84,
  Pels7OInstrConfigH = 85,
  Pels8IInstrConfigL = 88,
  Pels8IInstrConfigH = 89,
  Pels8EInstrConfigL = 90,
  Pels8EInstrConfigH = 91,
  Pels8OInstrConfigL = 92,
  Pels8OInstrConfigH = 93,
  Pels9IInstrConfigL = 96,
  Pels9IInstrConfigH = 97,
  Pels9EInstrConfigL = 98,
  Pels9EInstrConfigH = 99,
  Pels9OInstrConfigL = 100,
  Pels9OInstrConfigH = 101,
  Pels10IInstrConfigL = 104,
  Pels10IInstrConfigH = 105,
  Pels10EInstrConfigL = 106,
  Pels10EInstrConfigH = 107,
  Pels10OInstrConfigL = 108,
  Pels10OInstrConfigH = 109,
  Pels11IInstrConfigL = 112,
  Pels11IInstrConfigH = 113,
  Pels11EInstrConfigL = 114,
  Pels11EInstrConfigH = 115,
  Pels11OInstrConfigL = 116,
  Pels11OInstrConfigH = 117,
  Pels12IInstrConfigL = 120,
  Pels12IInstrConfigH = 121,
  Pels12EInstrConfigL = 122,
  Pels12EInstrConfigH = 123,
  Pels12OInstrConfigL = 124,
  Pels12OInstrConfigH = 125,
  Pels13IInstrConfigL = 128,
  Pels13IInstrConfigH = 129,
  Pels13EInstrConfigL = 130,
  Pels13EInstrConfigH = 131,
  Pels13OInstrConfigL = 132,
  Pels13OInstrConfigH = 133,
  Pels14IInstrConfigL = 136,
  Pels14IInstrConfigH = 137,
  Pels14EInstrConfigL = 138,
  Pels14EInstrConfigH = 139,
  Pels14OInstrConfigL = 140,
  Pels14OInstrConfigH = 141,
  Pels15IInstrConfigL = 144,
  Pels15IInstrConfigH = 145,
  Pels15EInstrConfigL = 146,
  Pels15EInstrConfigH = 147,
  Pels15OInstrConfigL = 148,
  Pels15OInstrConfigH = 149,
  Pe0TcmConfigL = 152,
  Pe0TcmConfigH = 153,
  Pe1TcmConfigL = 154,
  Pe1TcmConfigH = 155,
  Pe2TcmConfigL = 156,
  Pe2TcmConfigH = 157,
  Pe3TcmConfigL = 158,
  Pe3TcmConfigH = 159,
  Pe4TcmConfigL = 160,
  Pe4TcmConfigH = 161,
  Pe5TcmConfigL = 162,
  Pe5TcmConfigH = 163,
  Pe6TcmConfigL = 164,
  Pe6TcmConfigH = 165,
  Pe7TcmConfigL = 166,
  Pe7TcmConfigH = 167,
  Pe8TcmConfigL = 168,
  Pe8TcmConfigH = 169,
  Pe9TcmConfigL = 170,
  Pe9TcmConfigH = 171,
  Pe10TcmConfigL = 172,
  Pe10TcmConfigH = 173,
  Pe11TcmConfigL = 174,
  Pe11TcmConfigH = 175,
  Pe12TcmConfigL = 176,
  Pe12TcmConfigH = 177,
  Pe13TcmConfigL = 178,
  Pe13TcmConfigH = 179,
  Pe14TcmConfigL = 180,
  Pe14TcmConfigH = 181,
  Pe15TcmConfigL = 182,
  Pe15TcmConfigH = 183,
  IduDispatchMode = 184,
};

// The following indices are for SET instruction of PELS.
enum class PelsSetIndexMap {
  PeEnCtrl0 = 0,
  PeEnCtrl1 = 1,
  PeEnCtrl2 = 2,
  PeEnCtrl3 = 3,
  PeDoneIntSt0 = 4,
  PeDoneIntSt1 = 5,
  PeDoneIntSt2 = 6,
  PeDoneIntSt3 = 7,
  PeRvvIrqSt = 8,
  PeRvvIraSt = 9,
  PeIoBufferSt = 12,
  PeDmacPriority = 13,
  PeDdmaDoneIntSt = 14,
  PeWdmaDoneIntSt = 15,
  PelsIInstrConfigL = 16,
  PelsIInstrConfigH = 17,
  PelsEInstrConfigL = 18,
  PelsEInstrConfigH = 19,
  PelsOInstrConfigL = 20,
  PelsOInstrConfigH = 21,
  SpuInstrConfigL = 24,
  SpuInstrConfigH = 25,
  VpuInstrConfigL = 26,
  VpuInstrConfigH = 27,
  AeInstrConfigL = 28,
  AeInstrConfigH = 29,
  TeInstrConfigL = 30,
  TeInstrConfigH = 31,
  SeInstrConfigL = 32,
  SeInstrConfigH = 33,
  Dma0InstrConfigL = 34,
  Dma0InstrConfigH = 35,
  Dma1InstrConfigL = 36,
  Dma1InstrConfigH = 37,
  Dma2InstrConfigL = 38,
  Dma2InstrConfigH = 39,
  Dma3InstrConfigL = 40,
  Dma3InstrConfigH = 41,
  Dma4InstrConfigL = 42,
  Dma4InstrConfigH = 43,
  Dma5InstrConfigL = 44,
  Dma5InstrConfigH = 45,
};

// The following indices are for SET instruction of SPU.
enum class SpuSetIndexMap {
  SpuSrcTsrAddr0 = 0,
  SpuSrcTsrAddr1 = 1,
  SpuSrcTsrAddr2 = 2,
  SpuSrcTsrAddr3 = 3,
  SpuSrcTsrAddr4 = 4,
  SpuSrcTsrAddr5 = 5,
  SpuSrcTsrAddr6 = 6,
  SpuSrcTsrAddr7 = 7,
  SpuSrcWgtAddr0 = 8,
  SpuSrcWgtAddr1 = 9,
  SpuSrcWgtAddr2 = 10,
  SpuSrcWgtAddr3 = 11,
  SpuSrcWgtAddr4 = 12,
  SpuSrcWgtAddr5 = 13,
  SpuSrcWgtAddr6 = 14,
  SpuSrcWgtAddr7 = 15,
  SpuSrcMiAddr0 = 16,
  SpuSrcMiAddr1 = 17,
  SpuSrcMiAddr2 = 18,
  SpuSrcMiAddr3 = 19,
  SpuSrcMiAddr4 = 20,
  SpuSrcMiAddr5 = 21,
  SpuSrcMiAddr6 = 22,
  SpuSrcMiAddr7 = 23,
  SpuSrcMoAddr0 = 24,
  SpuSrcMoAddr1 = 25,
  SpuSrcMoAddr2 = 26,
  SpuSrcMoAddr3 = 27,
  SpuSrcMoAddr4 = 28,
  SpuSrcMoAddr5 = 29,
  SpuSrcMoAddr6 = 30,
  SpuSrcMoAddr7 = 31,
  SpuSrcQntAddr0 = 32,
  SpuSrcQntAddr1 = 33,
  SpuSrcQntAddr2 = 34,
  SpuSrcQntAddr3 = 35,
  SpuSrcQntAddr4 = 36,
  SpuSrcQntAddr5 = 37,
  SpuSrcQntAddr6 = 38,
  SpuSrcQntAddr7 = 39,
  SpuSrcAccAddr0 = 40,
  SpuSrcAccAddr1 = 41,
  SpuSrcAccAddr2 = 42,
  SpuSrcAccAddr3 = 43,
  SpuSrcAccAddr4 = 44,
  SpuSrcAccAddr5 = 45,
  SpuSrcAccAddr6 = 46,
  SpuSrcAccAddr7 = 47,
  SpuDstTsrAddr0 = 48,
  SpuDstTsrAddr1 = 49,
  SpuDstTsrAddr2 = 50,
  SpuDstTsrAddr3 = 51,
  SpuDstTsrAddr4 = 52,
  SpuDstTsrAddr5 = 53,
  SpuDstTsrAddr6 = 54,
  SpuDstTsrAddr7 = 55,
  SpuSrcTsrStride0 = 56,
  SpuSrcTsrStride1 = 57,
  SpuSrcTsrStride2 = 58,
  SpuSrcTsrStride3 = 59,
  SpuSrcTsrStride4 = 60,
  SpuSrcTsrStride5 = 61,
  SpuSrcTsrStride6 = 62,
  SpuSrcTsrStride7 = 63,
  SpuSrcWgtStride0 = 64,
  SpuSrcWgtStride1 = 65,
  SpuSrcWgtStride2 = 66,
  SpuSrcWgtStride3 = 67,
  SpuSrcWgtStride4 = 68,
  SpuSrcWgtStride5 = 69,
  SpuSrcWgtStride6 = 70,
  SpuSrcWgtStride7 = 71,
  SpuSrcMiStride0 = 72,
  SpuSrcMiStride1 = 73,
  SpuSrcMiStride2 = 74,
  SpuSrcMiStride3 = 75,
  SpuSrcMiStride4 = 76,
  SpuSrcMiStride5 = 77,
  SpuSrcMiStride6 = 78,
  SpuSrcMiStride7 = 79,
  SpuSrcMoStride0 = 80,
  SpuSrcMoStride1 = 81,
  SpuSrcMoStride2 = 82,
  SpuSrcMoStride3 = 83,
  SpuSrcMoStride4 = 84,
  SpuSrcMoStride5 = 85,
  SpuSrcMoStride6 = 86,
  SpuSrcMoStride7 = 87,
  SpuSrcQntStride0 = 88,
  SpuSrcQntStride1 = 89,
  SpuSrcQntStride2 = 90,
  SpuSrcQntStride3 = 91,
  SpuSrcQntStride4 = 92,
  SpuSrcQntStride5 = 93,
  SpuSrcQntStride6 = 94,
  SpuSrcQntStride7 = 95,
  SpuSrcAccStride0 = 96,
  SpuSrcAccStride1 = 97,
  SpuSrcAccStride2 = 98,
  SpuSrcAccStride3 = 99,
  SpuSrcAccStride4 = 100,
  SpuSrcAccStride5 = 101,
  SpuSrcAccStride6 = 102,
  SpuSrcAccStride7 = 103,
  SpuDstTsrStride0 = 104,
  SpuDstTsrStride1 = 105,
  SpuDstTsrStride2 = 106,
  SpuDstTsrStride3 = 107,
  SpuDstTsrStride4 = 108,
  SpuDstTsrStride5 = 109,
  SpuDstTsrStride6 = 110,
  SpuDstTsrStride7 = 111,
  SpuSrcTsrLength0 = 112,
  SpuSrcTsrLength1 = 113,
  SpuSrcWgtLength = 114,
  SpuSrcMiLength0 = 115,
  SpuSrcMiLength1 = 116,
  SpuSrcMoLength0 = 117,
  SpuSrcMoLength1 = 118,
  SpuSrcQntLength = 119,
  SpuSrcAccLength0 = 120,
  SpuSrcAccLength1 = 121,
  SpuDstTsrLength0 = 122,
  SpuDstTsrLength1 = 123,
  SpuDstTsrLength2 = 124,
  SpuDstTsrLength3 = 125,
};

// The following indices are for SET instruction of VPU.
enum class VpuSetIndexMap {
  VpuLdStride = 0,
  VpuLdPortMask = 1,
  VpuLdPortBcast = 2,
  VpuLdBankMask = 3,
  VpuLdBankBcast = 4,
  VpuLdChBcast = 5,
  VpuLdBaseAddr0 = 6,
  VpuLdDqtFactor0 = 7,
  VpuLdBaseAddr1 = 8,
  VpuLdDqtFactor1 = 9,
  VpuStStride = 10,
  VpuStPortMask = 11,
  VpuStBankMask = 12,
  VpuStBaseAddr = 13,
  VpuStQntFactor = 14,
  VpuFormatSelect = 15,
};

// The following indices are for SET instruction of AE.
enum class AeSetIndexMap {
  AeSrcTsrAddr0 = 0,
  AeSrcTsrAddr1 = 1,
  AeSrcTsrAddr2 = 2,
  AeSrcTsrAddr3 = 3,
  AeSrcTsrAddr4 = 4,
  AeSrcTsrAddr5 = 5,
  AeSrcTsrAddr6 = 6,
  AeSrcTsrAddr7 = 7,
  AeDstTsrAddr0 = 8,
  AeDstTsrAddr1 = 9,
  AeDstTsrAddr2 = 10,
  AeDstTsrAddr3 = 11,
  AeDstTsrAddr4 = 12,
  AeDstTsrAddr5 = 13,
  AeDstTsrAddr6 = 14,
  AeDstTsrAddr7 = 15,
  AeSrcTsrStride0 = 16,
  AeSrcTsrStride1 = 17,
  AeSrcTsrStride2 = 18,
  AeSrcTsrStride3 = 19,
  AeSrcTsrStride4 = 20,
  AeSrcTsrStride5 = 21,
  AeSrcTsrStride6 = 22,
  AeSrcTsrStride7 = 23,
  AeDstTsrStride0 = 24,
  AeDstTsrStride1 = 25,
  AeDstTsrStride2 = 26,
  AeDstTsrStride3 = 27,
  AeDstTsrStride4 = 28,
  AeDstTsrStride5 = 29,
  AeDstTsrStride6 = 30,
  AeDstTsrStride7 = 31,
  AeSrcTsrLength = 32,
  AeDstTsrLength = 33,
  AeCoeffA = 34,
  AeCoeffB = 35,
  AeCoeffC = 36,
  AePValue = 37,
  AeQuantFactor = 38,
  AeDequantFactor = 39,
};

// The following indices are for SET instruction of TE.
enum class TeSetIndexMap {
  TeSrcTsrAddr0 = 0,
  TeSrcTsrAddr1 = 1,
  TeSrcTsrAddr2 = 2,
  TeSrcTsrAddr3 = 3,
  TeSrcTsrAddr4 = 4,
  TeSrcTsrAddr5 = 5,
  TeSrcTsrAddr6 = 6,
  TeSrcTsrAddr7 = 7,
  TeDstTsrAddr0 = 8,
  TeDstTsrAddr1 = 9,
  TeDstTsrAddr2 = 10,
  TeDstTsrAddr3 = 11,
  TeDstTsrAddr4 = 12,
  TeDstTsrAddr5 = 13,
  TeDstTsrAddr6 = 14,
  TeDstTsrAddr7 = 15,
  TeSrcTsrStride0 = 16,
  TeSrcTsrStride1 = 17,
  TeSrcTsrStride2 = 18,
  TeSrcTsrStride3 = 19,
  TeSrcTsrStride4 = 20,
  TeSrcTsrStride5 = 21,
  TeSrcTsrStride6 = 22,
  TeSrcTsrStride7 = 23,
  TeDstTsrStride0 = 24,
  TeDstTsrStride1 = 25,
  TeDstTsrStride2 = 26,
  TeDstTsrStride3 = 27,
  TeDstTsrStride4 = 28,
  TeDstTsrStride5 = 29,
  TeDstTsrStride6 = 30,
  TeDstTsrStride7 = 31,
  TeSrcTsrLength = 32,
  TeDstTsrLength = 33,
};

// The following indices are for SET instruction of SE.
enum class SeSetIndexMap {
  SeSrcValAddr0 = 0,
  SeSrcValAddr1 = 1,
  SeSrcValAddr2 = 2,
  SeSrcValAddr3 = 3,
  SeSrcValAddr4 = 4,
  SeSrcValAddr5 = 5,
  SeSrcValAddr6 = 6,
  SeSrcValAddr7 = 7,
  SeSrcIdxAddr0 = 8,
  SeSrcIdxAddr1 = 9,
  SeSrcIdxAddr2 = 10,
  SeSrcIdxAddr3 = 11,
  SeSrcIdxAddr4 = 12,
  SeSrcIdxAddr5 = 13,
  SeSrcIdxAddr6 = 14,
  SeSrcIdxAddr7 = 15,
  SeDstValAddr0 = 16,
  SeDstValAddr1 = 17,
  SeDstValAddr2 = 18,
  SeDstValAddr3 = 19,
  SeDstValAddr4 = 20,
  SeDstValAddr5 = 21,
  SeDstValAddr6 = 22,
  SeDstValAddr7 = 23,
  SeDstIdxAddr0 = 24,
  SeDstIdxAddr1 = 25,
  SeDstIdxAddr2 = 26,
  SeDstIdxAddr3 = 27,
  SeDstIdxAddr4 = 28,
  SeDstIdxAddr5 = 29,
  SeDstIdxAddr6 = 30,
  SeDstIdxAddr7 = 31,
  SeSrcValStride0 = 32,
  SeSrcValStride1 = 33,
  SeSrcValStride2 = 34,
  SeSrcValStride3 = 35,
  SeSrcValStride4 = 36,
  SeSrcValStride5 = 37,
  SeSrcValStride6 = 38,
  SeSrcValStride7 = 39,
  SeSrcIdxStride0 = 40,
  SeSrcIdxStride1 = 41,
  SeSrcIdxStride2 = 42,
  SeSrcIdxStride3 = 43,
  SeSrcIdxStride4 = 44,
  SeSrcIdxStride5 = 45,
  SeSrcIdxStride6 = 46,
  SeSrcIdxStride7 = 47,
  SeDstValStride0 = 48,
  SeDstValStride1 = 49,
  SeDstValStride2 = 50,
  SeDstValStride3 = 51,
  SeDstValStride4 = 52,
  SeDstValStride5 = 53,
  SeDstValStride6 = 54,
  SeDstValStride7 = 55,
  SeDstIdxStride0 = 56,
  SeDstIdxStride1 = 57,
  SeDstIdxStride2 = 58,
  SeDstIdxStride3 = 59,
  SeDstIdxStride4 = 60,
  SeDstIdxStride5 = 61,
  SeDstIdxStride6 = 62,
  SeDstIdxStride7 = 63,
  SeSrcValLength = 64,
  SeSrcIdxLength = 65,
  SeDstValLength = 66,
  SeDstIdxLength = 67,
  SeThreshold = 68,
  SeReadMaskL = 69,
  SeReadMaskH = 70,
  SeMaskInterval = 71,
  SeWriteCount = 72,
};

// The following indices are for SET instruction of DMA.
enum class DmaSetIndexMap {
  DmacSrcTsrAddr0 = 0,
  DmacSrcTsrAddr1 = 1,
  DmacSrcTsrAddr2 = 2,
  DmacSrcTsrAddr3 = 3,
  DmacSrcTsrAddr4 = 4,
  DmacSrcTsrAddr5 = 5,
  DmacSrcTsrAddr6 = 6,
  DmacSrcTsrAddr7 = 7,
  DmacDstTsrAddr0 = 8,
  DmacDstTsrAddr1 = 9,
  DmacDstTsrAddr2 = 10,
  DmacDstTsrAddr3 = 11,
  DmacDstTsrAddr4 = 12,
  DmacDstTsrAddr5 = 13,
  DmacDstTsrAddr6 = 14,
  DmacDstTsrAddr7 = 15,
  DmacSrcTsrStride0 = 16,
  DmacSrcTsrStride1 = 17,
  DmacSrcTsrStride2 = 18,
  DmacSrcTsrStride3 = 19,
  DmacSrcTsrStride4 = 20,
  DmacSrcTsrStride5 = 21,
  DmacSrcTsrStride6 = 22,
  DmacSrcTsrStride7 = 23,
  DmacDstTsrStride0 = 24,
  DmacDstTsrStride1 = 25,
  DmacDstTsrStride2 = 26,
  DmacDstTsrStride3 = 27,
  DmacDstTsrStride4 = 28,
  DmacDstTsrStride5 = 29,
  DmacDstTsrStride6 = 30,
  DmacDstTsrStride7 = 31,
  DmacSrcTsrLength = 32,
  DmacDstTsrLength = 33,
  DmacLoopCount0 = 34,
  DmacLoopCount1 = 35,
  DmacLoopCount2 = 36,
  DmacLoopCount3 = 37,
  DmacLoopCount4 = 38,
  DmacLoopCount5 = 39,
  DmacLoopCount6 = 40,
  DmacLoopCount7 = 41,
  DmacConvertStride = 42,
  DmacDynIdxMode = 43,
  DmacDynIdxStAddr = 44,
  DmacDynIdxLen = 45,
  DmacDynSrcSegLen = 46,
  DmacDynDstSegLen = 47,
};

} // namespace quark

#endif // SPU_HWARDWARE_CONFIG_H
