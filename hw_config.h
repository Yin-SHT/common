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

enum class IsaOpcode {
  NOP = 0x0,
  LOADCFG,
  CONV2D,
  ACTIV,
  TOPK,
  TRANS,
  GLBCPY,
  DMAMV,
};

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
  DMA,
  SIZE = SE,
  INVALID = 255,
};

const size_t kNumCore = 4;
const size_t kSizeMemPage = 8192;
const size_t kNumCluster = 16;
const size_t kSizeCoreDdr = 4UL << 30;
const size_t kSizeClusterSharedMem = 2UL << 20;
const size_t kSizePerSpuCmd = 4;
const size_t kSizePerVpuCmd = 4;

const size_t kSizeSpuCmdMem = 2UL << 10;
const size_t kSizeVpuCmdMem = 4UL << 10;
const size_t kSizeDmaCmdMem = 2UL << 10;
const size_t kSizeAeCmdMem = 2UL << 10;
const size_t kSizeTeCmdMem = 2UL << 10;
const size_t kSizeSeCmdMem = 2UL << 10;

const size_t kNumVpuVectorReg = 4;
const size_t kVpuVectorRegDepth = 64;
const size_t kNumVpuScalarReg = 32;

} // namespace quark

#endif // SPU_HWARDWARE_CONFIG_H
