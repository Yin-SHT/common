/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */
#ifndef QUARK_ADDR_MAP_H
#define QUARK_ADDR_MAP_H

#include <stdint.h>

namespace quark {

struct AddrRange {
  AddrRange(size_t start, size_t end) :
      start(start), end(end) {}
  bool check(size_t addr) const {
    return (addr >= start && addr <= end);
  }
  size_t start;
  size_t end;
};

// System Memory Map
const AddrRange kSystemCsrMemMap = {0x100000000, 0x107ffffff};
const AddrRange kSystemCore0MemMap = {0x120000000, 0x12fffffff};
const AddrRange kSystemCore1MemMap = {0x130000000, 0x13fffffff};
const AddrRange kSystemCore2MemMap = {0x140000000, 0x14fffffff};
const AddrRange kSystemCore3MemMap = {0x150000000, 0x15fffffff};
const AddrRange kSystemFirmwarePart0MemMap = {0x160000000, 0x167ffffff};
const AddrRange kSystemFirmwarePart1MemMap = {0x168000000, 0x16fffffff};
const AddrRange kSystemFirmwarePart2MemMap = {0x170000000, 0x177ffffff};
const AddrRange kSystemFirmwarePart3MemMap = {0x178000000, 0x17fffffff};
const AddrRange kSystemRuntimeMemMap = {0x180000000, 0x1ffffffff};
const AddrRange kSystemTopSpmMemMap = {0xffffc0000, 0xfffffffff};
const AddrRange kSystemPcieDmaMemMap = {0x1000000000, 0x10ffffffff};
const AddrRange kSystemEthernetMac0MemMap = {0x2000000000, 0x20ffffffff};
const AddrRange kSystemEthernetMac1MemMap = {0x2100000000, 0x21ffffffff};
const AddrRange kSystemEthernetMac2MemMap = {0x2200000000, 0x22ffffffff};
const AddrRange kSystemEthernetMac3MemMap = {0x2300000000, 0x23ffffffff};
const AddrRange kSystemEthernetMac4MemMap = {0x2400000000, 0x24ffffffff};
const AddrRange kSystemEthernetMac5MemMap = {0x2500000000, 0x25ffffffff};
const AddrRange kSystemEthernetMac6MemMap = {0x2600000000, 0x26ffffffff};
const AddrRange kSystemEthernetMac7MemMap = {0x2700000000, 0x27ffffffff};
const AddrRange kSystemFrontPort0MemMap = {0x4000000000, 0x47ffffffff};
const AddrRange kSystemFrontPort1MemMap = {0x4800000000, 0x4fffffffff};
const AddrRange kSystemFrontPort2MemMap = {0x5000000000, 0x57ffffffff};
const AddrRange kSystemFrontPort3MemMap = {0x5800000000, 0x5fffffffff};
const AddrRange kSystemNwDdrRt0MemMap = {0x6000000000, 0x601effffff};
const AddrRange kSystemNwDdrFw0MemMap = {0x6020000000, 0x6027ffffff};
const AddrRange kSystemNwDdrOtherMemMap = {0x6028000000, 0x65ffffffff};
const AddrRange kSystemNeDdrRt0MemMap = {0x6800000000, 0x681effffff};
const AddrRange kSystemNeDdrFw0MemMap = {0x6820000000, 0x6827ffffff};
const AddrRange kSystemNeDdrOtherMemMap = {0x6828000000, 0x6dffffffff};
const AddrRange kSystemSwDdrRt0MemMap = {0x7000000000, 0x701effffff};
const AddrRange kSystemSwDdrFw0MemMap = {0x7020000000, 0x7027ffffff};
const AddrRange kSystemSwDdrOtherMemMap = {0x7028000000, 0x75ffffffff};
const AddrRange kSystemSeDdrRt0MemMap = {0x7800000000, 0x781effffff};
const AddrRange kSystemSeDdrFw0MemMap = {0x7820000000, 0x7827ffffff};
const AddrRange kSystemSeDdrOtherMemMap = {0x7828000000, 0x7dffffffff};

// Core Memory Map
const AddrRange kCoreVirtualCoreCsrMemMap = {0x0, 0x7ffff};
const AddrRange kCoreCoreDmac0InstrMemMap = {0x80000, 0x81fff};
const AddrRange kCoreCoreDmac1InstrMemMap = {0x82000, 0x83fff};
const AddrRange kCoreCoreDmac2InstrMemMap = {0x84000, 0x85fff};
const AddrRange kCoreIduL1BroadcastSpaceMemMap = {0xf00000, 0xffffff};
const AddrRange kCoreIduL2MemMap = {0x1000000, 0x1ffffff};
const AddrRange kCoreSmBroadcastSpaceRow0MemMap = {0x2000000, 0x21fffff};
const AddrRange kCoreSmBroadcastSpaceRow1MemMap = {0x2200000, 0x23fffff};
const AddrRange kCoreSmBroadcastSpaceRow2MemMap = {0x2400000, 0x25fffff};
const AddrRange kCoreSmBroadcastSpaceRow3MemMap = {0x2600000, 0x27fffff};
const AddrRange kCoreSmBroadcastSpaceCoreMemMap = {0x2800000, 0x29fffff};
const AddrRange kCoreLlcSpmMemMap = {0x3000000, 0x3ffffff};
const AddrRange kCorePe0MemMap = {0x8000000, 0x87fffff};
const AddrRange kCorePe1MemMap = {0x8800000, 0x8ffffff};
const AddrRange kCorePe2MemMap = {0x9000000, 0x97fffff};
const AddrRange kCorePe3MemMap = {0x9800000, 0x9ffffff};
const AddrRange kCorePe4MemMap = {0xa000000, 0xa7fffff};
const AddrRange kCorePe5MemMap = {0xa800000, 0xaffffff};
const AddrRange kCorePe6MemMap = {0xb000000, 0xb7fffff};
const AddrRange kCorePe7MemMap = {0xb800000, 0xbffffff};
const AddrRange kCorePe8MemMap = {0xc000000, 0xc7fffff};
const AddrRange kCorePe9MemMap = {0xc800000, 0xcffffff};
const AddrRange kCorePe10MemMap = {0xd000000, 0xd7fffff};
const AddrRange kCorePe11MemMap = {0xd800000, 0xdffffff};
const AddrRange kCorePe12MemMap = {0xe000000, 0xe7fffff};
const AddrRange kCorePe13MemMap = {0xe800000, 0xeffffff};
const AddrRange kCorePe14MemMap = {0xf000000, 0xf7fffff};
const AddrRange kCorePe15MemMap = {0xf800000, 0xfffffff};

// Pe Memory Map
const AddrRange kPeVirtualPeCsrMemMap = {0x0, 0x7ffff};
const AddrRange kPeSchedulerIInstrMemMap = {0x80000, 0x81fff};
const AddrRange kPeSchedulerEInstrMemMap = {0x82000, 0x83fff};
const AddrRange kPeSchedulerOInstrMemMap = {0x84000, 0x85fff};
const AddrRange kPeSpuInstrMemMap = {0x88000, 0x8ffff};
const AddrRange kPeVpuInstrMemMap = {0x90000, 0x97fff};
const AddrRange kPeAeInstrMemMap = {0x98000, 0x9ffff};
const AddrRange kPeTeInstrMemMap = {0xa0000, 0xa7fff};
const AddrRange kPeSeInstrMemMap = {0xa8000, 0xaffff};
const AddrRange kPeDmaCtrl0InstrMemMap = {0xb0000, 0xb1fff};
const AddrRange kPeDmaCtrl1InstrMemMap = {0xb2000, 0xb3fff};
const AddrRange kPeDmaCtrl2InstrMemMap = {0xb4000, 0xb5fff};
const AddrRange kPeDmaCtrl3InstrMemMap = {0xb6000, 0xb7fff};
const AddrRange kPeDmaCtrl4InstrMemMap = {0xb8000, 0xb9fff};
const AddrRange kPeDmaCtrl5InstrMemMap = {0xba000, 0xbbfff};
const AddrRange kPeVirtualCsrMemMap = {0x400000, 0x47ffff};
const AddrRange kPeTcmMemMap = {0x4c0000, 0x4fffff};
const AddrRange kPeSharedMemoryMemMap = {0x500000, 0x6fffff};

// System Registers Map
const AddrRange kSystemNwDdr0RegMap = {0x0, 0x3fffff};
const AddrRange kSystemNwDdr1RegMap = {0x400000, 0x7fffff};
const AddrRange kSystemNwDdr2RegMap = {0x800000, 0xbfffff};
const AddrRange kSystemNeDdr0RegMap = {0xc00000, 0xffffff};
const AddrRange kSystemNeDdr1RegMap = {0x1000000, 0x13fffff};
const AddrRange kSystemNeDdr2RegMap = {0x1400000, 0x17fffff};
const AddrRange kSystemSwDdr0RegMap = {0x1800000, 0x1bfffff};
const AddrRange kSystemSwDdr1RegMap = {0x1c00000, 0x1ffffff};
const AddrRange kSystemSwDdr2RegMap = {0x2000000, 0x23fffff};
const AddrRange kSystemSeDdr0RegMap = {0x2400000, 0x27fffff};
const AddrRange kSystemSeDdr1RegMap = {0x2800000, 0x2bfffff};
const AddrRange kSystemSeDdr2RegMap = {0x2c00000, 0x2ffffff};
const AddrRange kSystemPcie0RegMap = {0x3000000, 0x33fffff};
const AddrRange kSystemPcie1RegMap = {0x3400000, 0x37fffff};
const AddrRange kSystemPcie2RegMap = {0x3800000, 0x3bfffff};
const AddrRange kSystemPcie3RegMap = {0x3c00000, 0x3ffffff};
const AddrRange kSystemCore0RegMap = {0x4000000, 0x43fffff};
const AddrRange kSystemCore1RegMap = {0x4400000, 0x47fffff};
const AddrRange kSystemCore2RegMap = {0x4800000, 0x4bfffff};
const AddrRange kSystemCore3RegMap = {0x4c00000, 0x4ffffff};
const AddrRange kSystemNorthPeripheralIoRegMap = {0x5000000, 0x53fffff};
const AddrRange kSystemSouthPeripheralIoRegMap = {0x5400000, 0x57fffff};
const AddrRange kSystemRingNocApbRegMap = {0x5800000, 0x5bfffff};
const AddrRange kSystem2dMeshNocApbRegMap = {0x5c00000, 0x5ffffff};
const AddrRange kSystemNocServiceNetworkRegMap = {0x6000000, 0x63fffff};
const AddrRange kSystemClockGeneratorRegMap = {0x6400000, 0x67fffff};
const AddrRange kSystemTemperatureSensorRegMap = {0x6800000, 0x6bfffff};
const AddrRange kSystemEfuseRegMap = {0x6c00000, 0x6ffffff};
const AddrRange kSystemTopControlRegMap = {0x7000000, 0x73fffff};

// Core Registers Map
const AddrRange kCoreCoreControlRegMap = {0x0, 0xfff};
const AddrRange kCoreDdma0RegMap = {0x1000, 0x1fff};
const AddrRange kCoreDdma1RegMap = {0x2000, 0x2fff};
const AddrRange kCoreDdma2RegMap = {0x3000, 0x3fff};
const AddrRange kCoreDdma3RegMap = {0x4000, 0x4fff};
const AddrRange kCoreCdmaRegMap = {0x5000, 0x5fff};
const AddrRange kCoreCoreDmac0RegMap = {0x6000, 0x6fff};
const AddrRange kCoreCoreDmac1RegMap = {0x7000, 0x7fff};
const AddrRange kCoreCoreDmac2RegMap = {0x8000, 0x8fff};
const AddrRange kCoreCoreRvvMailboxRegMap = {0x40000, 0x40fff};
const AddrRange kCorePe0RegMap = {0x800000, 0x87ffff};
const AddrRange kCorePe1RegMap = {0x880000, 0x8fffff};
const AddrRange kCorePe2RegMap = {0x900000, 0x97ffff};
const AddrRange kCorePe3RegMap = {0x980000, 0x9fffff};
const AddrRange kCorePe4RegMap = {0xa00000, 0xa7ffff};
const AddrRange kCorePe5RegMap = {0xa80000, 0xafffff};
const AddrRange kCorePe6RegMap = {0xb00000, 0xb7ffff};
const AddrRange kCorePe7RegMap = {0xb80000, 0xbfffff};
const AddrRange kCorePe8RegMap = {0xc00000, 0xc7ffff};
const AddrRange kCorePe9RegMap = {0xc80000, 0xcfffff};
const AddrRange kCorePe10RegMap = {0xd00000, 0xd7ffff};
const AddrRange kCorePe11RegMap = {0xd80000, 0xdfffff};
const AddrRange kCorePe12RegMap = {0xe00000, 0xe7ffff};
const AddrRange kCorePe13RegMap = {0xe80000, 0xefffff};
const AddrRange kCorePe14RegMap = {0xf00000, 0xf7ffff};
const AddrRange kCorePe15RegMap = {0xf80000, 0xffffff};

// Pe Registers Map
const AddrRange kPePeRegMap = {0x0, 0xfff};
const AddrRange kPeRvvRegMap = {0x1000, 0x1fff};
const AddrRange kPeIduRegMap = {0x2000, 0x2fff};
const AddrRange kPeIcuRegMap = {0x3000, 0x3fff};
const AddrRange kPeExceptionRegMap = {0x4000, 0x4fff};
const AddrRange kPeDebugRegMap = {0x5000, 0x5fff};
const AddrRange kPeSpuRegMap = {0x6000, 0x6fff};
const AddrRange kPeVpuRegMap = {0x7000, 0x7fff};
const AddrRange kPeAeRegMap = {0x8000, 0x8fff};
const AddrRange kPeTeRegMap = {0x9000, 0x9fff};
const AddrRange kPeSeRegMap = {0xa000, 0xafff};
const AddrRange kPeDmaCtrl0RegMap = {0xb000, 0xbfff};
const AddrRange kPeDmaCtrl1RegMap = {0xc000, 0xcfff};
const AddrRange kPeDmaCtrl2RegMap = {0xd000, 0xdfff};
const AddrRange kPeDmaCtrl3RegMap = {0xe000, 0xefff};
const AddrRange kPeDmaCtrl4RegMap = {0xf000, 0xffff};
const AddrRange kPeDmaCtrl5RegMap = {0x10000, 0x10fff};
const AddrRange kPeWdmaRegMap = {0x11000, 0x11fff};
const AddrRange kPeSmRegMap = {0x12000, 0x12fff};
const AddrRange kPeRvvMailboxRegMap = {0x40000, 0x40fff};

// Top RV Address Map
const AddrRange kTopRvDebugModuleAddrMap = {0x0, 0xfff};
const AddrRange kTopRvErrorDeviceAddrMap = {0x3000, 0x3fff};
const AddrRange kTopRvTestStatusRegisterAddrMap = {0x4000, 0x4fff};
const AddrRange kTopRvBusErrorUnitAddrMap = {0x1700000, 0x1700fff};
const AddrRange kTopRvClintAddrMap = {0x2000000, 0x200ffff};
const AddrRange kTopRvComposableCacheControlAddrMap = {0x2010000, 0x2013fff};
const AddrRange kTopRvPrefetcherHardwareAddrMap = {0x2030000, 0x2037fff};
const AddrRange kTopRvSubsystemPowerManagementControllerWithinCpuAddrMap = {0x3100000, 0x3100fff};
const AddrRange kTopRvTilePowerManagementControllerWithCpuAddrMap = {0x3101000, 0x3101fff};
const AddrRange kTopRvL3CacheAddrMap = {0x8000000, 0x81fffff};
const AddrRange kTopRvZeroDeviceAddrMap = {0xa000000, 0xa1fffff};
const AddrRange kTopRvPlatformlevelInterruptControllerPlicAddrMap = {0xc000000, 0xfffffff};
const AddrRange kTopRvNexusTraceEncoderTeAddrMap = {0x10000000, 0x10000fff};
const AddrRange kTopRvFrontPortBurstBundlerAddrMap = {0x10010000, 0x10010fff};
const AddrRange kTopRvSystemPort0BurstBundlerAddrMap = {0x10020000, 0x10020fff};
const AddrRange kTopRvFrontPortOrderObliteratorAddrMap = {0x10030000, 0x10033fff};
const AddrRange kTopRvPl2ControlAddrMap = {0x10200000, 0x10203fff};
const AddrRange kTopRvPeripheralPortAddrMap = {0x20000000, 0x3fffffff};
const AddrRange kTopRvSystemPort0AddrMap = {0x100000000, 0x1ffffffff};
const AddrRange kTopRvMemoryPortAddrMap = {0x200000000, 0x11ffffffff};

// Core RV Address Map
const AddrRange kCoreRvDebugModuleAddrMap = {0x0, 0xfff};
const AddrRange kCoreRvErrorDeviceAddrMap = {0x3000, 0x3fff};
const AddrRange kCoreRvTestStatusRegisterAddrMap = {0x4000, 0x4fff};
const AddrRange kCoreRvBusErrorUnitAddrMap = {0x1700000, 0x1700fff};
const AddrRange kCoreRvClintAddrMap = {0x2000000, 0x200ffff};
const AddrRange kCoreRvComposableCacheControlAddrMap = {0x2010000, 0x2013fff};
const AddrRange kCoreRvPrefetcherHardwareAddrMap = {0x2030000, 0x2037fff};
const AddrRange kCoreRvSubsystemPowerManagementControllerWithinCpuAddrMap = {0x3100000, 0x3100fff};
const AddrRange kCoreRvTilePowerManagementControllerWithCpuAddrMap = {0x3101000, 0x3104fff};
const AddrRange kCoreRvL3CacheAddrMap = {0x8000000, 0x83fffff};
const AddrRange kCoreRvZeroDeviceAddrMap = {0xa000000, 0xa3fffff};
const AddrRange kCoreRvPlatformlevelInterruptControllerPlicAddrMap = {0xc000000, 0xfffffff};
const AddrRange kCoreRvNexusTraceEncoderTeAddrMap = {0x10000000, 0x10003fff};
const AddrRange kCoreRvFrontPortBurstBundlerAddrMap = {0x10010000, 0x10010fff};
const AddrRange kCoreRvSystemPort0BurstBundlerAddrMap = {0x10020000, 0x10020fff};
const AddrRange kCoreRvFrontPortOrderObliteratorAddrMap = {0x10030000, 0x10033fff};
const AddrRange kCoreRvSubsystemCoherentSbusAddrMap = {0x10070000, 0x10070fff};
const AddrRange kCoreRvPl2ControlAddrMap = {0x10200000, 0x1020ffff};
const AddrRange kCoreRvPeripheralPortAddrMap = {0x20000000, 0x3fffffff};
const AddrRange kCoreRvSystemPort0AddrMap = {0x40000000, 0x5fffffff};
const AddrRange kCoreRvMemoryPortAddrMap = {0x80000000, 0x607fffffff};

// Pe RV Address Map
const AddrRange kPeRvDebugModuleAddrMap = {0x0, 0xfff};
const AddrRange kPeRvErrorDeviceAddrMap = {0x3000, 0x3fff};
const AddrRange kPeRvTestStatusRegisterAddrMap = {0x4000, 0x4fff};
const AddrRange kPeRvBusErrorUnitAddrMap = {0x1700000, 0x1700fff};
const AddrRange kPeRvClintAddrMap = {0x2000000, 0x200ffff};
const AddrRange kPeRvSubsystemPowerManagementControllerWithinCpuAddrMap = {0x3100000, 0x3100fff};
const AddrRange kPeRvTilePowerManagementControllerWithCpuAddrMap = {0x3101000, 0x3101fff};
const AddrRange kPeRvPlatformlevelInterruptControllerPlicAddrMap = {0xc000000, 0xfffffff};
const AddrRange kPeRvNexusTraceEncoderTeAddrMap = {0x10000000, 0x10000fff};
const AddrRange kPeRvFrontPortBurstBundlerAddrMap = {0x10010000, 0x10010fff};
const AddrRange kPeRvSystemPortBurstBundlerAddrMap = {0x10020000, 0x10020fff};
const AddrRange kPeRvFrontPortOrderObliteratorAddrMap = {0x10030000, 0x10033fff};
const AddrRange kPeRvPl2ControlAddrMap = {0x10200000, 0x10203fff};
const AddrRange kPeRvPeripheralPortAddrMap = {0x20000000, 0x3fffffff};
const AddrRange kPeRvSystemPort0AddrMap = {0x40000000, 0x5fffffff};
const AddrRange kPeRvDataLocalStoreTodoAddrMap = {0x70000000, 0x7003ffff};
const AddrRange kPeRvMemoryPortAddrMap = {0x80000000, 0x8000ffff};
const AddrRange kPeRvPeCsrAddrMap = {0xa0000000, 0xa007ffff};
const AddrRange kPeRvTcmAddrMap = {0xa00c0000, 0xa00fffff};
const AddrRange kPeRvSharedMemoryAddrMap = {0xa0100000, 0xa02fffff};

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
  DmacSrcBaseFifoAddrIdx = 48,
  DmacDstBaseFifoAddrIdx = 49,
  DmacSrcBaseFifoAddrPop = 50,
  DmacDstBaseFifoAddrPop = 51,
};

// The following indices are for SET instruction of CODMA.
enum class CodmaSetIndexMap {
  CodmacSrcTsrAddr0 = 0,
  CodmacSrcTsrAddr1 = 1,
  CodmacSrcTsrAddr2 = 2,
  CodmacSrcTsrAddr3 = 3,
  CodmacSrcTsrAddr4 = 4,
  CodmacSrcTsrAddr5 = 5,
  CodmacSrcTsrAddr6 = 6,
  CodmacSrcTsrAddr7 = 7,
  CodmacDstTsrAddr0 = 8,
  CodmacDstTsrAddr1 = 9,
  CodmacDstTsrAddr2 = 10,
  CodmacDstTsrAddr3 = 11,
  CodmacDstTsrAddr4 = 12,
  CodmacDstTsrAddr5 = 13,
  CodmacDstTsrAddr6 = 14,
  CodmacDstTsrAddr7 = 15,
  CodmacSrcTsrStride0 = 16,
  CodmacSrcTsrStride1 = 17,
  CodmacSrcTsrStride2 = 18,
  CodmacSrcTsrStride3 = 19,
  CodmacSrcTsrStride4 = 20,
  CodmacSrcTsrStride5 = 21,
  CodmacSrcTsrStride6 = 22,
  CodmacSrcTsrStride7 = 23,
  CodmacDstTsrStride0 = 24,
  CodmacDstTsrStride1 = 25,
  CodmacDstTsrStride2 = 26,
  CodmacDstTsrStride3 = 27,
  CodmacDstTsrStride4 = 28,
  CodmacDstTsrStride5 = 29,
  CodmacDstTsrStride6 = 30,
  CodmacDstTsrStride7 = 31,
  CodmacSrcTsrLength = 32,
  CodmacDstTsrLength = 33,
  CodmacLoopCount0 = 34,
  CodmacLoopCount1 = 35,
  CodmacLoopCount2 = 36,
  CodmacLoopCount3 = 37,
  CodmacLoopCount4 = 38,
  CodmacLoopCount5 = 39,
  CodmacLoopCount6 = 40,
  CodmacLoopCount7 = 41,
  CodmacConvertStride = 42,
  CodmacSrcBaseFifoAddrIdx = 43,
  CodmacDstBaseFifoAddrIdx = 44,
  CodmacSrcBaseFifoAddrPop = 45,
  CodmacDstBaseFifoAddrPop = 46,
};

} // namespace quark

#endif // QUARK_ADDR_MAP_H
