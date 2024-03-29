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
  PELS,
  COLS,
  CHLS,
  INVALID = 255,
};

const size_t kNumCore = 4;
const size_t kSizeMemPage = 8192;
const size_t kNumPe = 16;
const size_t kSizeCoreDdr = 4UL << 30;
const size_t kSizeSharedMem = 1UL << 20;
const size_t kSizePerSpuCmd = 4;
const size_t kSizePerVpuCmd = 4;
const size_t kSizePerPelsCmd = 4;
const size_t kSizePerColsCmd = 4;
const size_t kSizePerChlsCmd = 4;

const size_t kSizeSpuCmdMem = 2UL << 10;
const size_t kSizeVpuCmdMem = 4UL << 10;
const size_t kSizeDmaCmdMem = 2UL << 10;
const size_t kSizeAeCmdMem = 2UL << 10;
const size_t kSizeTeCmdMem = 2UL << 10;
const size_t kSizeSeCmdMem = 2UL << 10;
const size_t kSizePelsCmdMem = 2UL << 10;
const size_t kSizeColsCmdMem = 2UL << 10;
const size_t kSizeChlsCmdMem = 2UL << 10;

const size_t kNumVpuVectorReg = 4;
const size_t kVpuVectorRegDepth = 64;
const size_t kNumVpuScalarReg = 32;

const size_t kDmaSrcBaseAddrL = 0;
const size_t kDmaSrcBaseAddrH = 1;
const size_t kDmaSrcTsrAddrL0 = 2;
const size_t kDmaSrcTsrAddrH0 = 3;
const size_t kDmaSrcTsrAddrL1 = 4;
const size_t kDmaSrcTsrAddrH1 = 5;
const size_t kDmaSrcTsrAddrL2 = 6;
const size_t kDmaSrcTsrAddrH2 = 7;
const size_t kDmaSrcTsrAddrL3 = 8;
const size_t kDmaSrcTsrAddrH3 = 9;
const size_t kDmaSrcTsrAddrL4 = 10;
const size_t kDmaSrcTsrAddrH4 = 11;
const size_t kDmaSrcTsrAddrL5 = 12;
const size_t kDmaSrcTsrAddrH5 = 13;
const size_t kDmaSrcTsrAddrL6 = 14;
const size_t kDmaSrcTsrAddrH6 = 15;
const size_t kDmaSrcTsrAddrL7 = 16;
const size_t kDmaSrcTsrAddrH7 = 17;
const size_t kDmaDstBaseAddrL = 18;
const size_t kDmaDstBaseAddrH = 19;
const size_t kDmaDstTsrAddrL0 = 20;
const size_t kDmaDstTsrAddrH0 = 21;
const size_t kDmaDstTsrAddrL1 = 22;
const size_t kDmaDstTsrAddrH1 = 23;
const size_t kDmaDstTsrAddrL2 = 24;
const size_t kDmaDstTsrAddrH2 = 25;
const size_t kDmaDstTsrAddrL3 = 26;
const size_t kDmaDstTsrAddrH3 = 27;
const size_t kDmaDstTsrAddrL4 = 28;
const size_t kDmaDstTsrAddrH4 = 29;
const size_t kDmaDstTsrAddrL5 = 30;
const size_t kDmaDstTsrAddrH5 = 31;
const size_t kDmaDstTsrAddrL6 = 32;
const size_t kDmaDstTsrAddrH6 = 33;
const size_t kDmaDstTsrAddrL7 = 34;
const size_t kDmaDstTsrAddrH7 = 35;
const size_t kDmaSrcTsrStrideL0 = 36;
const size_t kDmaSrcTsrStrideH0 = 37;
const size_t kDmaSrcTsrStrideL1 = 38;
const size_t kDmaSrcTsrStrideH1 = 39;
const size_t kDmaSrcTsrStrideL2 = 40;
const size_t kDmaSrcTsrStrideH2 = 41;
const size_t kDmaSrcTsrStrideL3 = 42;
const size_t kDmaSrcTsrStrideH3 = 43;
const size_t kDmaSrcTsrStrideL4 = 44;
const size_t kDmaSrcTsrStrideH4 = 45;
const size_t kDmaSrcTsrStrideL5 = 46;
const size_t kDmaSrcTsrStrideH5 = 47;
const size_t kDmaSrcTsrStrideL6 = 48;
const size_t kDmaSrcTsrStrideH6 = 49;
const size_t kDmaSrcTsrStrideL7 = 50;
const size_t kDmaSrcTsrStrideH7 = 51;
const size_t kDmaDstTsrStrideL0 = 52;
const size_t kDmaDstTsrStrideH0 = 53;
const size_t kDmaDstTsrStrideL1 = 54;
const size_t kDmaDstTsrStrideH1 = 55;
const size_t kDmaDstTsrStrideL2 = 56;
const size_t kDmaDstTsrStrideH2 = 57;
const size_t kDmaDstTsrStrideL3 = 58;
const size_t kDmaDstTsrStrideH3 = 59;
const size_t kDmaDstTsrStrideL4 = 60;
const size_t kDmaDstTsrStrideH4 = 61;
const size_t kDmaDstTsrStrideL5 = 62;
const size_t kDmaDstTsrStrideH5 = 63;
const size_t kDmaDstTsrStrideL6 = 64;
const size_t kDmaDstTsrStrideH6 = 65;
const size_t kDmaDstTsrStrideL7 = 66;
const size_t kDmaDstTsrStrideH7 = 67;
const size_t kDmaSrcTsrLength = 68;
const size_t kDmaDstTsrLength = 69;
const size_t kDmaLoopCount0 = 70;
const size_t kDmaLoopCount1 = 71;
const size_t kDmaLoopCount2 = 72;
const size_t kDmaLoopCount3 = 73;
const size_t kDmaLoopCount4 = 74;
const size_t kDmaLoopCount5 = 75;
const size_t kDmaLoopCount6 = 76;
const size_t kDmaLoopCount7 = 77;

} // namespace quark

#endif // SPU_HWARDWARE_CONFIG_H
