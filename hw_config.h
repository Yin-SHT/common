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

struct DmaLinkedNode {
  uint64_t srcAddr : 42;
  uint64_t dstAddr : 42;
  uint64_t length : 32;
  uint64_t nextDescAddr : 42;
  uint64_t mode : 1;
  uint64_t pingPong : 1;
  uint64_t broadcastMask : 16;
};

const size_t kNumCore = 4;
const size_t kSizeMemPage = 8192;
const size_t kNumPe = 16;
const size_t kSizeDdr = 192UL << 30;
const size_t kSizeTcm = 256UL << 10;
const size_t kSizeSharedMem = 1UL << 20;
const size_t kSizePerSpuCmd = 4;
const size_t kSizePerVpuCmd = 4;
const size_t kSizePerPelsCmd = 4;
const size_t kSizePerColsCmd = 4;
const size_t kSizePerChlsCmd = 4;
const size_t kIduTransGranularity = 32;

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
const size_t kChipEnCtrl0 = 0;
const size_t kChipEnCtrl1 = 1;
const size_t kChipEnCtrl2 = 2;
const size_t kChipEnCtrl3 = 3;
const size_t kChipEnCtrl4 = 4;
const size_t kChipDoneIntSt0 = 8;
const size_t kChipDoneIntSt1 = 9;
const size_t kChipDoneIntSt2 = 10;
const size_t kChipDoneIntSt3 = 11;
const size_t kChipDoneIntSt4 = 12;
const size_t kChipRvvIrqSt = 16;
const size_t kChipRvvIraSt = 17;
const size_t kChlsInstrConfigL = 20;
const size_t kChlsInstrConfigH = 21;
const size_t kCols0InstrConfigL = 24;
const size_t kCols0InstrConfigH = 25;
const size_t kCols1InstrConfigL = 26;
const size_t kCols1InstrConfigH = 27;
const size_t kCols2InstrConfigL = 28;
const size_t kCols2InstrConfigH = 29;
const size_t kCols3InstrConfigL = 30;
const size_t kCols3InstrConfigH = 31;
const size_t kCore0RvPrgmConfigL = 32;
const size_t kCore0RvPrgmConfigH = 33;
const size_t kCore1RvPrgmConfigL = 34;
const size_t kCore1RvPrgmConfigH = 35;
const size_t kCore2RvPrgmConfigL = 36;
const size_t kCore2RvPrgmConfigH = 37;
const size_t kCore3RvPrgmConfigL = 38;
const size_t kCore3RvPrgmConfigH = 39;

// The following indices are for SET instruction of COLS.
const size_t kCoreEnCtrl0 = 0;
const size_t kCoreEnCtrl1 = 1;
const size_t kCoreEnCtrl2 = 2;
const size_t kCoreEnCtrl3 = 3;
const size_t kCoreEnCtrl4 = 4;
const size_t kCoreEnCtrl5 = 5;
const size_t kCoreDoneIntSt0 = 8;
const size_t kCoreDoneIntSt1 = 9;
const size_t kCoreDoneIntSt2 = 10;
const size_t kCoreDoneIntSt3 = 11;
const size_t kCoreDoneIntSt4 = 12;
const size_t kCoreDoneIntSt5 = 13;
const size_t kCoreRvvIrqSt = 16;
const size_t kCoreRvvIraSt = 17;
const size_t kColsInstrConfigL = 20;
const size_t kColsInstrConfigH = 21;
const size_t kPels0IInstrConfigL = 24;
const size_t kPels0IInstrConfigH = 25;
const size_t kPels0EInstrConfigL = 26;
const size_t kPels0EInstrConfigH = 27;
const size_t kPels0OInstrConfigL = 28;
const size_t kPels0OInstrConfigH = 29;
const size_t kPels1IInstrConfigL = 32;
const size_t kPels1IInstrConfigH = 33;
const size_t kPels1EInstrConfigL = 34;
const size_t kPels1EInstrConfigH = 35;
const size_t kPels1OInstrConfigL = 36;
const size_t kPels1OInstrConfigH = 37;
const size_t kPels2IInstrConfigL = 40;
const size_t kPels2IInstrConfigH = 41;
const size_t kPels2EInstrConfigL = 42;
const size_t kPels2EInstrConfigH = 43;
const size_t kPels2OInstrConfigL = 44;
const size_t kPels2OInstrConfigH = 45;
const size_t kPels3IInstrConfigL = 48;
const size_t kPels3IInstrConfigH = 49;
const size_t kPels3EInstrConfigL = 50;
const size_t kPels3EInstrConfigH = 51;
const size_t kPels3OInstrConfigL = 52;
const size_t kPels3OInstrConfigH = 53;
const size_t kPels4IInstrConfigL = 56;
const size_t kPels4IInstrConfigH = 57;
const size_t kPels4EInstrConfigL = 58;
const size_t kPels4EInstrConfigH = 59;
const size_t kPels4OInstrConfigL = 60;
const size_t kPels4OInstrConfigH = 61;
const size_t kPels5IInstrConfigL = 64;
const size_t kPels5IInstrConfigH = 65;
const size_t kPels5EInstrConfigL = 66;
const size_t kPels5EInstrConfigH = 67;
const size_t kPels5OInstrConfigL = 68;
const size_t kPels5OInstrConfigH = 69;
const size_t kPels6IInstrConfigL = 72;
const size_t kPels6IInstrConfigH = 73;
const size_t kPels6EInstrConfigL = 74;
const size_t kPels6EInstrConfigH = 75;
const size_t kPels6OInstrConfigL = 76;
const size_t kPels6OInstrConfigH = 77;
const size_t kPels7IInstrConfigL = 80;
const size_t kPels7IInstrConfigH = 81;
const size_t kPels7EInstrConfigL = 82;
const size_t kPels7EInstrConfigH = 83;
const size_t kPels7OInstrConfigL = 84;
const size_t kPels7OInstrConfigH = 85;
const size_t kPels8IInstrConfigL = 88;
const size_t kPels8IInstrConfigH = 89;
const size_t kPels8EInstrConfigL = 90;
const size_t kPels8EInstrConfigH = 91;
const size_t kPels8OInstrConfigL = 92;
const size_t kPels8OInstrConfigH = 93;
const size_t kPels9IInstrConfigL = 96;
const size_t kPels9IInstrConfigH = 97;
const size_t kPels9EInstrConfigL = 98;
const size_t kPels9EInstrConfigH = 99;
const size_t kPels9OInstrConfigL = 100;
const size_t kPels9OInstrConfigH = 101;
const size_t kPels10IInstrConfigL = 104;
const size_t kPels10IInstrConfigH = 105;
const size_t kPels10EInstrConfigL = 106;
const size_t kPels10EInstrConfigH = 107;
const size_t kPels10OInstrConfigL = 108;
const size_t kPels10OInstrConfigH = 109;
const size_t kPels11IInstrConfigL = 112;
const size_t kPels11IInstrConfigH = 113;
const size_t kPels11EInstrConfigL = 114;
const size_t kPels11EInstrConfigH = 115;
const size_t kPels11OInstrConfigL = 116;
const size_t kPels11OInstrConfigH = 117;
const size_t kPels12IInstrConfigL = 120;
const size_t kPels12IInstrConfigH = 121;
const size_t kPels12EInstrConfigL = 122;
const size_t kPels12EInstrConfigH = 123;
const size_t kPels12OInstrConfigL = 124;
const size_t kPels12OInstrConfigH = 125;
const size_t kPels13IInstrConfigL = 128;
const size_t kPels13IInstrConfigH = 129;
const size_t kPels13EInstrConfigL = 130;
const size_t kPels13EInstrConfigH = 131;
const size_t kPels13OInstrConfigL = 132;
const size_t kPels13OInstrConfigH = 133;
const size_t kPels14IInstrConfigL = 136;
const size_t kPels14IInstrConfigH = 137;
const size_t kPels14EInstrConfigL = 138;
const size_t kPels14EInstrConfigH = 139;
const size_t kPels14OInstrConfigL = 140;
const size_t kPels14OInstrConfigH = 141;
const size_t kPels15IInstrConfigL = 144;
const size_t kPels15IInstrConfigH = 145;
const size_t kPels15EInstrConfigL = 146;
const size_t kPels15EInstrConfigH = 147;
const size_t kPels15OInstrConfigL = 148;
const size_t kPels15OInstrConfigH = 149;
const size_t kPe0TcmConfigL = 152;
const size_t kPe0TcmConfigH = 153;
const size_t kPe1TcmConfigL = 154;
const size_t kPe1TcmConfigH = 155;
const size_t kPe2TcmConfigL = 156;
const size_t kPe2TcmConfigH = 157;
const size_t kPe3TcmConfigL = 158;
const size_t kPe3TcmConfigH = 159;
const size_t kPe4TcmConfigL = 160;
const size_t kPe4TcmConfigH = 161;
const size_t kPe5TcmConfigL = 162;
const size_t kPe5TcmConfigH = 163;
const size_t kPe6TcmConfigL = 164;
const size_t kPe6TcmConfigH = 165;
const size_t kPe7TcmConfigL = 166;
const size_t kPe7TcmConfigH = 167;
const size_t kPe8TcmConfigL = 168;
const size_t kPe8TcmConfigH = 169;
const size_t kPe9TcmConfigL = 170;
const size_t kPe9TcmConfigH = 171;
const size_t kPe10TcmConfigL = 172;
const size_t kPe10TcmConfigH = 173;
const size_t kPe11TcmConfigL = 174;
const size_t kPe11TcmConfigH = 175;
const size_t kPe12TcmConfigL = 176;
const size_t kPe12TcmConfigH = 177;
const size_t kPe13TcmConfigL = 178;
const size_t kPe13TcmConfigH = 179;
const size_t kPe14TcmConfigL = 180;
const size_t kPe14TcmConfigH = 181;
const size_t kPe15TcmConfigL = 182;
const size_t kPe15TcmConfigH = 183;
const size_t kIduDispatchMode = 184;

// The following indices are for SET instruction of PELS.
const size_t kPeEnCtrl0 = 0;
const size_t kPeEnCtrl1 = 1;
const size_t kPeEnCtrl2 = 2;
const size_t kPeEnCtrl3 = 3;
const size_t kPeDoneIntSt0 = 4;
const size_t kPeDoneIntSt1 = 5;
const size_t kPeDoneIntSt2 = 6;
const size_t kPeDoneIntSt3 = 7;
const size_t kPeRvvIrqSt = 8;
const size_t kPeRvvIraSt = 9;
const size_t kPeIoBufferSt = 12;
const size_t kPeDmacPriority = 13;
const size_t kPelsIInstrConfigL = 16;
const size_t kPelsIInstrConfigH = 17;
const size_t kPelsEInstrConfigL = 18;
const size_t kPelsEInstrConfigH = 19;
const size_t kPelsOInstrConfigL = 20;
const size_t kPelsOInstrConfigH = 21;
const size_t kSpuInstrConfigL = 24;
const size_t kSpuInstrConfigH = 25;
const size_t kVpuInstrConfigL = 26;
const size_t kVpuInstrConfigH = 27;
const size_t kAeInstrConfigL = 28;
const size_t kAeInstrConfigH = 29;
const size_t kTeInstrConfigL = 30;
const size_t kTeInstrConfigH = 31;
const size_t kSeInstrConfigL = 32;
const size_t kSeInstrConfigH = 33;
const size_t kDma0InstrConfigL = 34;
const size_t kDma0InstrConfigH = 35;
const size_t kDma1InstrConfigL = 36;
const size_t kDma1InstrConfigH = 37;
const size_t kDma2InstrConfigL = 38;
const size_t kDma2InstrConfigH = 39;
const size_t kDma3InstrConfigL = 40;
const size_t kDma3InstrConfigH = 41;
const size_t kDma4InstrConfigL = 42;
const size_t kDma4InstrConfigH = 43;
const size_t kDma5InstrConfigL = 44;
const size_t kDma5InstrConfigH = 45;

// The following indices are for SET instruction of SPU.
const size_t kSpuSrcTsrAddr0 = 0;
const size_t kSpuSrcTsrAddr1 = 1;
const size_t kSpuSrcTsrAddr2 = 2;
const size_t kSpuSrcTsrAddr3 = 3;
const size_t kSpuSrcTsrAddr4 = 4;
const size_t kSpuSrcTsrAddr5 = 5;
const size_t kSpuSrcTsrAddr6 = 6;
const size_t kSpuSrcTsrAddr7 = 7;
const size_t kSpuSrcWgtAddr0 = 8;
const size_t kSpuSrcWgtAddr1 = 9;
const size_t kSpuSrcWgtAddr2 = 10;
const size_t kSpuSrcWgtAddr3 = 11;
const size_t kSpuSrcWgtAddr4 = 12;
const size_t kSpuSrcWgtAddr5 = 13;
const size_t kSpuSrcWgtAddr6 = 14;
const size_t kSpuSrcWgtAddr7 = 15;
const size_t kSpuSrcMiAddr0 = 16;
const size_t kSpuSrcMiAddr1 = 17;
const size_t kSpuSrcMiAddr2 = 18;
const size_t kSpuSrcMiAddr3 = 19;
const size_t kSpuSrcMiAddr4 = 20;
const size_t kSpuSrcMiAddr5 = 21;
const size_t kSpuSrcMiAddr6 = 22;
const size_t kSpuSrcMiAddr7 = 23;
const size_t kSpuSrcMoAddr0 = 24;
const size_t kSpuSrcMoAddr1 = 25;
const size_t kSpuSrcMoAddr2 = 26;
const size_t kSpuSrcMoAddr3 = 27;
const size_t kSpuSrcMoAddr4 = 28;
const size_t kSpuSrcMoAddr5 = 29;
const size_t kSpuSrcMoAddr6 = 30;
const size_t kSpuSrcMoAddr7 = 31;
const size_t kSpuSrcQntAddr0 = 32;
const size_t kSpuSrcQntAddr1 = 33;
const size_t kSpuSrcQntAddr2 = 34;
const size_t kSpuSrcQntAddr3 = 35;
const size_t kSpuSrcQntAddr4 = 36;
const size_t kSpuSrcQntAddr5 = 37;
const size_t kSpuSrcQntAddr6 = 38;
const size_t kSpuSrcQntAddr7 = 39;
const size_t kSpuSrcAccAddr0 = 40;
const size_t kSpuSrcAccAddr1 = 41;
const size_t kSpuSrcAccAddr2 = 42;
const size_t kSpuSrcAccAddr3 = 43;
const size_t kSpuSrcAccAddr4 = 44;
const size_t kSpuSrcAccAddr5 = 45;
const size_t kSpuSrcAccAddr6 = 46;
const size_t kSpuSrcAccAddr7 = 47;
const size_t kSpuDstTsrAddr0 = 48;
const size_t kSpuDstTsrAddr1 = 49;
const size_t kSpuDstTsrAddr2 = 50;
const size_t kSpuDstTsrAddr3 = 51;
const size_t kSpuDstTsrAddr4 = 52;
const size_t kSpuDstTsrAddr5 = 53;
const size_t kSpuDstTsrAddr6 = 54;
const size_t kSpuDstTsrAddr7 = 55;
const size_t kSpuSrcTsrStride0 = 56;
const size_t kSpuSrcTsrStride1 = 57;
const size_t kSpuSrcTsrStride2 = 58;
const size_t kSpuSrcTsrStride3 = 59;
const size_t kSpuSrcTsrStride4 = 60;
const size_t kSpuSrcTsrStride5 = 61;
const size_t kSpuSrcTsrStride6 = 62;
const size_t kSpuSrcTsrStride7 = 63;
const size_t kSpuSrcWgtStride0 = 64;
const size_t kSpuSrcWgtStride1 = 65;
const size_t kSpuSrcWgtStride2 = 66;
const size_t kSpuSrcWgtStride3 = 67;
const size_t kSpuSrcWgtStride4 = 68;
const size_t kSpuSrcWgtStride5 = 69;
const size_t kSpuSrcWgtStride6 = 70;
const size_t kSpuSrcWgtStride7 = 71;
const size_t kSpuSrcMiStride0 = 72;
const size_t kSpuSrcMiStride1 = 73;
const size_t kSpuSrcMiStride2 = 74;
const size_t kSpuSrcMiStride3 = 75;
const size_t kSpuSrcMiStride4 = 76;
const size_t kSpuSrcMiStride5 = 77;
const size_t kSpuSrcMiStride6 = 78;
const size_t kSpuSrcMiStride7 = 79;
const size_t kSpuSrcMoStride0 = 80;
const size_t kSpuSrcMoStride1 = 81;
const size_t kSpuSrcMoStride2 = 82;
const size_t kSpuSrcMoStride3 = 83;
const size_t kSpuSrcMoStride4 = 84;
const size_t kSpuSrcMoStride5 = 85;
const size_t kSpuSrcMoStride6 = 86;
const size_t kSpuSrcMoStride7 = 87;
const size_t kSpuSrcQntStride0 = 88;
const size_t kSpuSrcQntStride1 = 89;
const size_t kSpuSrcQntStride2 = 90;
const size_t kSpuSrcQntStride3 = 91;
const size_t kSpuSrcQntStride4 = 92;
const size_t kSpuSrcQntStride5 = 93;
const size_t kSpuSrcQntStride6 = 94;
const size_t kSpuSrcQntStride7 = 95;
const size_t kSpuSrcAccStride0 = 96;
const size_t kSpuSrcAccStride1 = 97;
const size_t kSpuSrcAccStride2 = 98;
const size_t kSpuSrcAccStride3 = 99;
const size_t kSpuSrcAccStride4 = 100;
const size_t kSpuSrcAccStride5 = 101;
const size_t kSpuSrcAccStride6 = 102;
const size_t kSpuSrcAccStride7 = 103;
const size_t kSpuDstTsrStride0 = 104;
const size_t kSpuDstTsrStride1 = 105;
const size_t kSpuDstTsrStride2 = 106;
const size_t kSpuDstTsrStride3 = 107;
const size_t kSpuDstTsrStride4 = 108;
const size_t kSpuDstTsrStride5 = 109;
const size_t kSpuDstTsrStride6 = 110;
const size_t kSpuDstTsrStride7 = 111;
const size_t kSpuSrcTsrLength0 = 112;
const size_t kSpuSrcTsrLength1 = 113;
const size_t kSpuSrcWgtLength = 114;
const size_t kSpuSrcMiLength0 = 115;
const size_t kSpuSrcMiLength1 = 116;
const size_t kSpuSrcMoLength0 = 117;
const size_t kSpuSrcMoLength1 = 118;
const size_t kSpuSrcQntLength = 119;
const size_t kSpuSrcAccLength0 = 120;
const size_t kSpuSrcAccLength1 = 121;
const size_t kSpuDstTsrLength0 = 122;
const size_t kSpuDstTsrLength1 = 123;
const size_t kSpuDstTsrLength2 = 124;
const size_t kSpuDstTsrLength3 = 125;

// The following indices are for SET instruction of VPU.
const size_t kVpuLdStride = 0;
const size_t kVpuLdPortMask = 1;
const size_t kVpuLdPortBcast = 2;
const size_t kVpuLdBankMask = 3;
const size_t kVpuLdBankBcast = 4;
const size_t kVpuLdChBcast = 5;
const size_t kVpuLdBaseAddr0 = 6;
const size_t kVpuLdDqtFactor0 = 7;
const size_t kVpuLdBaseAddr1 = 8;
const size_t kVpuLdDqtFactor1 = 9;
const size_t kVpuStStride = 10;
const size_t kVpuStPortMask = 11;
const size_t kVpuStBankMask = 12;
const size_t kVpuStBaseAddr = 13;
const size_t kVpuStQntFactor = 14;
const size_t kVpuFormatSelect = 15;

// The following indices are for SET instruction of AE.
const size_t kAeSrcTsrAddr0 = 0;
const size_t kAeSrcTsrAddr1 = 1;
const size_t kAeSrcTsrAddr2 = 2;
const size_t kAeSrcTsrAddr3 = 3;
const size_t kAeSrcTsrAddr4 = 4;
const size_t kAeSrcTsrAddr5 = 5;
const size_t kAeSrcTsrAddr6 = 6;
const size_t kAeSrcTsrAddr7 = 7;
const size_t kAeDstTsrAddr0 = 8;
const size_t kAeDstTsrAddr1 = 9;
const size_t kAeDstTsrAddr2 = 10;
const size_t kAeDstTsrAddr3 = 11;
const size_t kAeDstTsrAddr4 = 12;
const size_t kAeDstTsrAddr5 = 13;
const size_t kAeDstTsrAddr6 = 14;
const size_t kAeDstTsrAddr7 = 15;
const size_t kAeSrcTsrStride0 = 16;
const size_t kAeSrcTsrStride1 = 17;
const size_t kAeSrcTsrStride2 = 18;
const size_t kAeSrcTsrStride3 = 19;
const size_t kAeSrcTsrStride4 = 20;
const size_t kAeSrcTsrStride5 = 21;
const size_t kAeSrcTsrStride6 = 22;
const size_t kAeSrcTsrStride7 = 23;
const size_t kAeDstTsrStride0 = 24;
const size_t kAeDstTsrStride1 = 25;
const size_t kAeDstTsrStride2 = 26;
const size_t kAeDstTsrStride3 = 27;
const size_t kAeDstTsrStride4 = 28;
const size_t kAeDstTsrStride5 = 29;
const size_t kAeDstTsrStride6 = 30;
const size_t kAeDstTsrStride7 = 31;
const size_t kAeSrcTsrLength = 32;
const size_t kAeDstTsrLength = 33;
const size_t kAeCoeffA = 34;
const size_t kAeCoeffB = 35;
const size_t kAeCoeffC = 36;
const size_t kAePValue = 37;
const size_t kAeQuantFactor = 38;
const size_t kAeDequantFactor = 39;

// The following indices are for SET instruction of TE.
const size_t kTeSrcTsrAddr0 = 0;
const size_t kTeSrcTsrAddr1 = 1;
const size_t kTeSrcTsrAddr2 = 2;
const size_t kTeSrcTsrAddr3 = 3;
const size_t kTeSrcTsrAddr4 = 4;
const size_t kTeSrcTsrAddr5 = 5;
const size_t kTeSrcTsrAddr6 = 6;
const size_t kTeSrcTsrAddr7 = 7;
const size_t kTeDstTsrAddr0 = 8;
const size_t kTeDstTsrAddr1 = 9;
const size_t kTeDstTsrAddr2 = 10;
const size_t kTeDstTsrAddr3 = 11;
const size_t kTeDstTsrAddr4 = 12;
const size_t kTeDstTsrAddr5 = 13;
const size_t kTeDstTsrAddr6 = 14;
const size_t kTeDstTsrAddr7 = 15;
const size_t kTeSrcTsrStride0 = 16;
const size_t kTeSrcTsrStride1 = 17;
const size_t kTeSrcTsrStride2 = 18;
const size_t kTeSrcTsrStride3 = 19;
const size_t kTeSrcTsrStride4 = 20;
const size_t kTeSrcTsrStride5 = 21;
const size_t kTeSrcTsrStride6 = 22;
const size_t kTeSrcTsrStride7 = 23;
const size_t kTeDstTsrStride0 = 24;
const size_t kTeDstTsrStride1 = 25;
const size_t kTeDstTsrStride2 = 26;
const size_t kTeDstTsrStride3 = 27;
const size_t kTeDstTsrStride4 = 28;
const size_t kTeDstTsrStride5 = 29;
const size_t kTeDstTsrStride6 = 30;
const size_t kTeDstTsrStride7 = 31;
const size_t kTeSrcTsrLength = 32;
const size_t kTeDstTsrLength = 33;

// The following indices are for SET instruction of SE.
const size_t kSeSrcValAddr0 = 0;
const size_t kSeSrcValAddr1 = 1;
const size_t kSeSrcValAddr2 = 2;
const size_t kSeSrcValAddr3 = 3;
const size_t kSeSrcValAddr4 = 4;
const size_t kSeSrcValAddr5 = 5;
const size_t kSeSrcValAddr6 = 6;
const size_t kSeSrcValAddr7 = 7;
const size_t kSeSrcIdxAddr0 = 8;
const size_t kSeSrcIdxAddr1 = 9;
const size_t kSeSrcIdxAddr2 = 10;
const size_t kSeSrcIdxAddr3 = 11;
const size_t kSeSrcIdxAddr4 = 12;
const size_t kSeSrcIdxAddr5 = 13;
const size_t kSeSrcIdxAddr6 = 14;
const size_t kSeSrcIdxAddr7 = 15;
const size_t kSeDstValAddr0 = 16;
const size_t kSeDstValAddr1 = 17;
const size_t kSeDstValAddr2 = 18;
const size_t kSeDstValAddr3 = 19;
const size_t kSeDstValAddr4 = 20;
const size_t kSeDstValAddr5 = 21;
const size_t kSeDstValAddr6 = 22;
const size_t kSeDstValAddr7 = 23;
const size_t kSeDstIdxAddr0 = 24;
const size_t kSeDstIdxAddr1 = 25;
const size_t kSeDstIdxAddr2 = 26;
const size_t kSeDstIdxAddr3 = 27;
const size_t kSeDstIdxAddr4 = 28;
const size_t kSeDstIdxAddr5 = 29;
const size_t kSeDstIdxAddr6 = 30;
const size_t kSeDstIdxAddr7 = 31;
const size_t kSeSrcValStride0 = 32;
const size_t kSeSrcValStride1 = 33;
const size_t kSeSrcValStride2 = 34;
const size_t kSeSrcValStride3 = 35;
const size_t kSeSrcValStride4 = 36;
const size_t kSeSrcValStride5 = 37;
const size_t kSeSrcValStride6 = 38;
const size_t kSeSrcValStride7 = 39;
const size_t kSeSrcIdxStride0 = 40;
const size_t kSeSrcIdxStride1 = 41;
const size_t kSeSrcIdxStride2 = 42;
const size_t kSeSrcIdxStride3 = 43;
const size_t kSeSrcIdxStride4 = 44;
const size_t kSeSrcIdxStride5 = 45;
const size_t kSeSrcIdxStride6 = 46;
const size_t kSeSrcIdxStride7 = 47;
const size_t kSeDstValStride0 = 48;
const size_t kSeDstValStride1 = 49;
const size_t kSeDstValStride2 = 50;
const size_t kSeDstValStride3 = 51;
const size_t kSeDstValStride4 = 52;
const size_t kSeDstValStride5 = 53;
const size_t kSeDstValStride6 = 54;
const size_t kSeDstValStride7 = 55;
const size_t kSeDstIdxStride0 = 56;
const size_t kSeDstIdxStride1 = 57;
const size_t kSeDstIdxStride2 = 58;
const size_t kSeDstIdxStride3 = 59;
const size_t kSeDstIdxStride4 = 60;
const size_t kSeDstIdxStride5 = 61;
const size_t kSeDstIdxStride6 = 62;
const size_t kSeDstIdxStride7 = 63;
const size_t kSeSrcValLength = 64;
const size_t kSeSrcIdxLength = 65;
const size_t kSeDstValLength = 66;
const size_t kSeDstIdxLength = 67;
const size_t kSeThreshold = 68;
const size_t kSeReadMaskL = 69;
const size_t kSeReadMaskH = 70;
const size_t kSeMaskInterval = 71;
const size_t kSeWriteCount = 72;

// The following indices are for SET instruction of DMA.
const size_t kDmacSrcTsrAddrL0 = 0;
const size_t kDmacSrcTsrAddrH0 = 1;
const size_t kDmacSrcTsrAddrL1 = 2;
const size_t kDmacSrcTsrAddrH1 = 3;
const size_t kDmacSrcTsrAddrL2 = 4;
const size_t kDmacSrcTsrAddrH2 = 5;
const size_t kDmacSrcTsrAddrL3 = 6;
const size_t kDmacSrcTsrAddrH3 = 7;
const size_t kDmacSrcTsrAddrL4 = 8;
const size_t kDmacSrcTsrAddrH4 = 9;
const size_t kDmacSrcTsrAddrL5 = 10;
const size_t kDmacSrcTsrAddrH5 = 11;
const size_t kDmacSrcTsrAddrL6 = 12;
const size_t kDmacSrcTsrAddrH6 = 13;
const size_t kDmacSrcTsrAddrL7 = 14;
const size_t kDmacSrcTsrAddrH7 = 15;
const size_t kDmacDstTsrAddrL0 = 16;
const size_t kDmacDstTsrAddrH0 = 17;
const size_t kDmacDstTsrAddrL1 = 18;
const size_t kDmacDstTsrAddrH1 = 19;
const size_t kDmacDstTsrAddrL2 = 20;
const size_t kDmacDstTsrAddrH2 = 21;
const size_t kDmacDstTsrAddrL3 = 22;
const size_t kDmacDstTsrAddrH3 = 23;
const size_t kDmacDstTsrAddrL4 = 24;
const size_t kDmacDstTsrAddrH4 = 25;
const size_t kDmacDstTsrAddrL5 = 26;
const size_t kDmacDstTsrAddrH5 = 27;
const size_t kDmacDstTsrAddrL6 = 28;
const size_t kDmacDstTsrAddrH6 = 29;
const size_t kDmacDstTsrAddrL7 = 30;
const size_t kDmacDstTsrAddrH7 = 31;
const size_t kDmacSrcTsrStrideL0 = 32;
const size_t kDmacSrcTsrStrideH0 = 33;
const size_t kDmacSrcTsrStrideL1 = 34;
const size_t kDmacSrcTsrStrideH1 = 35;
const size_t kDmacSrcTsrStrideL2 = 36;
const size_t kDmacSrcTsrStrideH2 = 37;
const size_t kDmacSrcTsrStrideL3 = 38;
const size_t kDmacSrcTsrStrideH3 = 39;
const size_t kDmacSrcTsrStrideL4 = 40;
const size_t kDmacSrcTsrStrideH4 = 41;
const size_t kDmacSrcTsrStrideL5 = 42;
const size_t kDmacSrcTsrStrideH5 = 43;
const size_t kDmacSrcTsrStrideL6 = 44;
const size_t kDmacSrcTsrStrideH6 = 45;
const size_t kDmacSrcTsrStrideL7 = 46;
const size_t kDmacSrcTsrStrideH7 = 47;
const size_t kDmacDstTsrStrideL0 = 48;
const size_t kDmacDstTsrStrideH0 = 49;
const size_t kDmacDstTsrStrideL1 = 50;
const size_t kDmacDstTsrStrideH1 = 51;
const size_t kDmacDstTsrStrideL2 = 52;
const size_t kDmacDstTsrStrideH2 = 53;
const size_t kDmacDstTsrStrideL3 = 54;
const size_t kDmacDstTsrStrideH3 = 55;
const size_t kDmacDstTsrStrideL4 = 56;
const size_t kDmacDstTsrStrideH4 = 57;
const size_t kDmacDstTsrStrideL5 = 58;
const size_t kDmacDstTsrStrideH5 = 59;
const size_t kDmacDstTsrStrideL6 = 60;
const size_t kDmacDstTsrStrideH6 = 61;
const size_t kDmacDstTsrStrideL7 = 62;
const size_t kDmacDstTsrStrideH7 = 63;
const size_t kDmacSrcTsrLength = 64;
const size_t kDmacDstTsrLength = 65;
const size_t kDmacLoopCount0 = 66;
const size_t kDmacLoopCount1 = 67;
const size_t kDmacLoopCount2 = 68;
const size_t kDmacLoopCount3 = 69;
const size_t kDmacLoopCount4 = 70;
const size_t kDmacLoopCount5 = 71;
const size_t kDmacLoopCount6 = 72;
const size_t kDmacLoopCount7 = 73;
const size_t kDmacConvertStrideL = 74;
const size_t kDmacConvertStrideH = 75;

} // namespace quark

#endif // SPU_HWARDWARE_CONFIG_H
