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

#ifdef __cplusplus
namespace quark {
#endif

struct AddrRange {
#ifdef __cplusplus
  AddrRange(size_t start, size_t end) :
      start(start), end(end) {}
  bool check(size_t addr) const {
    return (addr >= start && addr <= end);
  }
#endif
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
const AddrRange kCorePe0RegMap = {0x200000, 0x21ffff};
const AddrRange kCorePe1RegMap = {0x220000, 0x23ffff};
const AddrRange kCorePe2RegMap = {0x240000, 0x25ffff};
const AddrRange kCorePe3RegMap = {0x260000, 0x27ffff};
const AddrRange kCorePe4RegMap = {0x280000, 0x29ffff};
const AddrRange kCorePe5RegMap = {0x2a0000, 0x2bffff};
const AddrRange kCorePe6RegMap = {0x2c0000, 0x2dffff};
const AddrRange kCorePe7RegMap = {0x2e0000, 0x2fffff};
const AddrRange kCorePe8RegMap = {0x300000, 0x31ffff};
const AddrRange kCorePe9RegMap = {0x320000, 0x33ffff};
const AddrRange kCorePe10RegMap = {0x340000, 0x35ffff};
const AddrRange kCorePe11RegMap = {0x360000, 0x37ffff};
const AddrRange kCorePe12RegMap = {0x380000, 0x39ffff};
const AddrRange kCorePe13RegMap = {0x3a0000, 0x3bffff};
const AddrRange kCorePe14RegMap = {0x3c0000, 0x3dffff};
const AddrRange kCorePe15RegMap = {0x3e0000, 0x3fffff};

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
const AddrRange kPeRvvMailboxRegMap = {0x1c000, 0x1cfff};

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
#ifdef __cplusplus
enum class SpuSetIndexMap {
#else
enum SpuSetIndexMap {
#endif
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
#ifdef __cplusplus
enum class VpuSetIndexMap {
#else
enum VpuSetIndexMap {
#endif
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
#ifdef __cplusplus
enum class AeSetIndexMap {
#else
enum AeSetIndexMap {
#endif
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
#ifdef __cplusplus
enum class TeSetIndexMap {
#else
enum TeSetIndexMap {
#endif
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
#ifdef __cplusplus
enum class SeSetIndexMap {
#else
enum SeSetIndexMap {
#endif
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
#ifdef __cplusplus
enum class DmaSetIndexMap {
#else
enum DmaSetIndexMap {
#endif
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
#ifdef __cplusplus
enum class CodmaSetIndexMap {
#else
enum CodmaSetIndexMap {
#endif
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

#ifdef __cplusplus
enum class PeRegisterDef {
#else
enum PeRegisterDef {
#endif
  PE_EN_CTRL0 = 0,
  PE_EN_CTRL1 = 4,
  PE_EN_CTRL2 = 8,
  PE_EN_CTRL3 = 12,
  PE_EN_CTRL4 = 16,
  PE_DONE_INT_ST0 = 48,
  PE_DONE_INT_ST1 = 52,
  PE_DONE_INT_ST2 = 56,
  PE_DONE_INT_ST3 = 60,
  PE_DONE_INT_ST4 = 64,
  PE_IO_BUFFER_ST = 80,
  PE_DMAC_PRIORITY = 84,
  PE_DDMA_DONE_INT_ST = 88,
  PE_WDMA_DONE_INT_ST = 92,
  PE_CALL_INSTR0 = 96,
  PE_CALL_INSTR1 = 100,
  PE_CALL_INSTR2 = 104,
  PE_CALL_INSTR3 = 108,
  PELS_I_CUR_EXEC_INSTR = 112,
  PELS_E_CUR_EXEC_INSTR = 116,
  PELS_O_CUR_EXEC_INSTR = 120,
  PELS_I_LOOP_COUNT = 128,
  PELS_E_LOOP_COUNT = 132,
  PELS_O_LOOP_COUNT = 136,
  CORE_PHY_LOGIC_ID_TABLE = 144,
  CORE0_PE_PHY_LOGIC_ID_TABLE_L = 160,
  CORE0_PE_PHY_LOGIC_ID_TABLE_H = 164,
  CORE1_PE_PHY_LOGIC_ID_TABLE_L = 168,
  CORE1_PE_PHY_LOGIC_ID_TABLE_H = 172,
  CORE2_PE_PHY_LOGIC_ID_TABLE_L = 176,
  CORE2_PE_PHY_LOGIC_ID_TABLE_H = 180,
  CORE3_PE_PHY_LOGIC_ID_TABLE_L = 184,
  CORE3_PE_PHY_LOGIC_ID_TABLE_H = 188,
  PE_PARITY_CHECK = 192,
  PE_MEM_CFG_L = 200,
  PE_MEM_CFG_H = 204,
};

#ifdef __cplusplus
enum class RvvRegisterDef {
#else
enum RvvRegisterDef {
#endif
  PE_RVV_IRQ_ST = 0,
  PE_RVV_IRA_ST = 4,
};

#ifdef __cplusplus
enum class IduRegisterDef {
#else
enum IduRegisterDef {
#endif
  PE_TCM_CONFIG_L = 0,
  PE_TCM_CONFIG_H = 4,
  PELS_I_INSTR_CONFIG_L = 8,
  PELS_I_INSTR_CONFIG_H = 12,
  PELS_E_INSTR_CONFIG_L = 16,
  PELS_E_INSTR_CONFIG_H = 20,
  PELS_O_INSTR_CONFIG_L = 24,
  PELS_O_INSTR_CONFIG_H = 28,
  SPU_INSTR_CONFIG_L = 32,
  SPU_INSTR_CONFIG_H = 36,
  VPU_INSTR_CONFIG_L = 40,
  VPU_INSTR_CONFIG_H = 44,
  AE_INSTR_CONFIG_L = 48,
  AE_INSTR_CONFIG_H = 52,
  TE_INSTR_CONFIG_L = 56,
  TE_INSTR_CONFIG_H = 60,
  SE_INSTR_CONFIG_L = 64,
  SE_INSTR_CONFIG_H = 68,
  DMA0_INSTR_CONFIG_L = 72,
  DMA0_INSTR_CONFIG_H = 76,
  DMA1_INSTR_CONFIG_L = 80,
  DMA1_INSTR_CONFIG_H = 84,
  DMA2_INSTR_CONFIG_L = 88,
  DMA2_INSTR_CONFIG_H = 92,
  DMA3_INSTR_CONFIG_L = 96,
  DMA3_INSTR_CONFIG_H = 100,
  DMA4_INSTR_CONFIG_L = 104,
  DMA4_INSTR_CONFIG_H = 108,
  DMA5_INSTR_CONFIG_L = 112,
  DMA5_INSTR_CONFIG_H = 116,
};

#ifdef __cplusplus
enum class SpuRegisterDef {
#else
enum SpuRegisterDef {
#endif
  SPU_SET = 0,
  SPU_STAT = 4,
  SPU_SRC_TSR_ADDR0 = 16,
  SPU_SRC_WGT_ADDR0 = 20,
  SPU_SRC_MI_ADDR0 = 24,
  SPU_SRC_MO_ADDR0 = 28,
  SPU_SRC_QNT_ADDR0 = 32,
  SPU_SRC_ACC_ADDR0 = 36,
  SPU_DST_TSR_ADDR0 = 40,
  SPU_SRC_TSR_STRIDE0 = 48,
  SPU_SRC_TSR_STRIDE1 = 52,
  SPU_SRC_TSR_STRIDE2 = 56,
  SPU_SRC_TSR_STRIDE3 = 60,
  SPU_SRC_TSR_STRIDE4 = 64,
  SPU_SRC_TSR_STRIDE5 = 68,
  SPU_SRC_TSR_STRIDE6 = 72,
  SPU_SRC_TSR_STRIDE7 = 76,
  SPU_SRC_WGT_STRIDE0 = 80,
  SPU_SRC_WGT_STRIDE1 = 84,
  SPU_SRC_WGT_STRIDE2 = 88,
  SPU_SRC_WGT_STRIDE3 = 92,
  SPU_SRC_WGT_STRIDE4 = 96,
  SPU_SRC_WGT_STRIDE5 = 100,
  SPU_SRC_WGT_STRIDE6 = 104,
  SPU_SRC_WGT_STRIDE7 = 108,
  SPU_SRC_MI_STRIDE0 = 112,
  SPU_SRC_MI_STRIDE1 = 116,
  SPU_SRC_MI_STRIDE2 = 120,
  SPU_SRC_MI_STRIDE3 = 124,
  SPU_SRC_MI_STRIDE4 = 128,
  SPU_SRC_MI_STRIDE5 = 132,
  SPU_SRC_MI_STRIDE6 = 136,
  SPU_SRC_MI_STRIDE7 = 140,
  SPU_SRC_MO_STRIDE0 = 144,
  SPU_SRC_MO_STRIDE1 = 148,
  SPU_SRC_MO_STRIDE2 = 152,
  SPU_SRC_MO_STRIDE3 = 156,
  SPU_SRC_MO_STRIDE4 = 160,
  SPU_SRC_MO_STRIDE5 = 164,
  SPU_SRC_MO_STRIDE6 = 168,
  SPU_SRC_MO_STRIDE7 = 172,
  SPU_SRC_QNT_STRIDE0 = 176,
  SPU_SRC_QNT_STRIDE1 = 180,
  SPU_SRC_QNT_STRIDE2 = 184,
  SPU_SRC_QNT_STRIDE3 = 188,
  SPU_SRC_QNT_STRIDE4 = 192,
  SPU_SRC_QNT_STRIDE5 = 196,
  SPU_SRC_QNT_STRIDE6 = 200,
  SPU_SRC_QNT_STRIDE7 = 204,
  SPU_SRC_ACC_STRIDE0 = 208,
  SPU_SRC_ACC_STRIDE1 = 212,
  SPU_SRC_ACC_STRIDE2 = 216,
  SPU_SRC_ACC_STRIDE3 = 220,
  SPU_SRC_ACC_STRIDE4 = 224,
  SPU_SRC_ACC_STRIDE5 = 228,
  SPU_SRC_ACC_STRIDE6 = 232,
  SPU_SRC_ACC_STRIDE7 = 236,
  SPU_DST_TSR_STRIDE0 = 240,
  SPU_DST_TSR_STRIDE1 = 244,
  SPU_DST_TSR_STRIDE2 = 248,
  SPU_DST_TSR_STRIDE3 = 252,
  SPU_DST_TSR_STRIDE4 = 256,
  SPU_DST_TSR_STRIDE5 = 260,
  SPU_DST_TSR_STRIDE6 = 264,
  SPU_DST_TSR_STRIDE7 = 268,
  SPU_SRC_TSR_LENGTH0 = 272,
  SPU_SRC_TSR_LENGTH1 = 276,
  SPU_SRC_WGT_LENGTH = 280,
  SPU_SRC_MI_LENGTH0 = 288,
  SPU_SRC_MI_LENGTH1 = 292,
  SPU_SRC_MO_LENGTH0 = 296,
  SPU_SRC_MO_LENGTH1 = 300,
  SPU_SRC_QNT_LENGTH = 304,
  SPU_SRC_ACC_LENGTH0 = 312,
  SPU_SRC_ACC_LENGTH1 = 316,
  SPU_DST_TSR_LENGTH0 = 320,
  SPU_DST_TSR_LENGTH1 = 324,
  SPU_DST_TSR_LENGTH2 = 328,
  SPU_DST_TSR_LENGTH3 = 332,
  SPU_LOOP_COUNT0 = 336,
  SPU_LOOP_COUNT1 = 340,
  SPU_LOOP_COUNT2 = 344,
  SPU_LOOP_COUNT3 = 348,
  SPU_LOOP_COUNT4 = 352,
  SPU_LOOP_COUNT5 = 356,
  SPU_LOOP_COUNT6 = 360,
  SPU_LOOP_COUNT7 = 364,
};

#ifdef __cplusplus
enum class VpuRegisterDef {
#else
enum VpuRegisterDef {
#endif
  VPU_SET = 0,
  VPU_STAT = 4,
  VPU_GPR0 = 16,
  VPU_GPR1 = 20,
  VPU_GPR2 = 24,
  VPU_GPR3 = 28,
  VPU_GPR4 = 32,
  VPU_GPR5 = 36,
  VPU_GPR6 = 40,
  VPU_GPR7 = 44,
  VPU_GPR8 = 48,
  VPU_GPR9 = 52,
  VPU_GPR10 = 56,
  VPU_GPR11 = 60,
  VPU_GPR12 = 64,
  VPU_GPR13 = 68,
  VPU_GPR14 = 72,
  VPU_GPR15 = 76,
  VPU_GPR16 = 80,
  VPU_GPR17 = 84,
  VPU_GPR18 = 88,
  VPU_GPR19 = 92,
  VPU_GPR20 = 96,
  VPU_GPR21 = 100,
  VPU_GPR22 = 104,
  VPU_GPR23 = 108,
  VPU_GPR24 = 112,
  VPU_GPR25 = 116,
  VPU_GPR26 = 120,
  VPU_GPR27 = 124,
  VPU_GPR28 = 128,
  VPU_GPR29 = 132,
  VPU_GPR30 = 136,
  VPU_GPR31 = 140,
};

#ifdef __cplusplus
enum class AeRegisterDef {
#else
enum AeRegisterDef {
#endif
  AE_SET = 0,
  AE_STAT = 4,
  AE_SRC_TSR_ADDR0 = 16,
  AE_DST_TSR_ADDR0 = 24,
  AE_SRC_TSR_STRIDE0 = 32,
  AE_SRC_TSR_STRIDE1 = 36,
  AE_SRC_TSR_STRIDE2 = 40,
  AE_SRC_TSR_STRIDE3 = 44,
  AE_SRC_TSR_STRIDE4 = 48,
  AE_SRC_TSR_STRIDE5 = 52,
  AE_SRC_TSR_STRIDE6 = 56,
  AE_SRC_TSR_STRIDE7 = 60,
  AE_DST_TSR_STRIDE0 = 64,
  AE_DST_TSR_STRIDE1 = 68,
  AE_DST_TSR_STRIDE2 = 72,
  AE_DST_TSR_STRIDE3 = 76,
  AE_DST_TSR_STRIDE4 = 80,
  AE_DST_TSR_STRIDE5 = 84,
  AE_DST_TSR_STRIDE6 = 88,
  AE_DST_TSR_STRIDE7 = 92,
  AE_SRC_TSR_LENGTH = 96,
  AE_DST_TSR_LENGTH = 104,
  AE_COEFF_A = 112,
  AE_COEFF_B = 116,
  AE_COEFF_C = 120,
  AE_LOOP_COUNT0 = 128,
  AE_LOOP_COUNT1 = 132,
  AE_LOOP_COUNT2 = 136,
  AE_LOOP_COUNT3 = 140,
  AE_LOOP_COUNT4 = 144,
  AE_LOOP_COUNT5 = 148,
  AE_LOOP_COUNT6 = 152,
  AE_LOOP_COUNT7 = 156,
};

#ifdef __cplusplus
enum class TeRegisterDef {
#else
enum TeRegisterDef {
#endif
  TE_SET = 0,
  TE_STAT = 4,
  TE_SRC_TSR_ADDR0 = 16,
  TE_DST_TSR_ADDR0 = 24,
  TE_SRC_TSR_STRIDE0 = 32,
  TE_SRC_TSR_STRIDE1 = 36,
  TE_SRC_TSR_STRIDE2 = 40,
  TE_SRC_TSR_STRIDE3 = 44,
  TE_SRC_TSR_STRIDE4 = 48,
  TE_SRC_TSR_STRIDE5 = 52,
  TE_SRC_TSR_STRIDE6 = 56,
  TE_SRC_TSR_STRIDE7 = 60,
  TE_DST_TSR_STRIDE0 = 64,
  TE_DST_TSR_STRIDE1 = 68,
  TE_DST_TSR_STRIDE2 = 72,
  TE_DST_TSR_STRIDE3 = 76,
  TE_DST_TSR_STRIDE4 = 80,
  TE_DST_TSR_STRIDE5 = 84,
  TE_DST_TSR_STRIDE6 = 88,
  TE_DST_TSR_STRIDE7 = 92,
  TE_SRC_TSR_LENGTH = 96,
  TE_DST_TSR_LENGTH = 104,
  TE_LOOP_COUNT0 = 112,
  TE_LOOP_COUNT1 = 116,
  TE_LOOP_COUNT2 = 120,
  TE_LOOP_COUNT3 = 124,
  TE_LOOP_COUNT4 = 128,
  TE_LOOP_COUNT5 = 132,
  TE_LOOP_COUNT6 = 136,
  TE_LOOP_COUNT7 = 140,
};

#ifdef __cplusplus
enum class SeRegisterDef {
#else
enum SeRegisterDef {
#endif
  SE_SET = 0,
  SE_STAT = 4,
  SE_SRC_VAL_ADDR0 = 16,
  SE_SRC_IDX_ADDR0 = 20,
  SE_DST_VAL_ADDR0 = 24,
  SE_DST_IDX_ADDR0 = 28,
  SE_SRC_VAL_STRIDE0 = 32,
  SE_SRC_VAL_STRIDE1 = 36,
  SE_SRC_VAL_STRIDE2 = 40,
  SE_SRC_VAL_STRIDE3 = 44,
  SE_SRC_VAL_STRIDE4 = 48,
  SE_SRC_VAL_STRIDE5 = 52,
  SE_SRC_VAL_STRIDE6 = 56,
  SE_SRC_VAL_STRIDE7 = 60,
  SE_SRC_IDX_STRIDE0 = 64,
  SE_SRC_IDX_STRIDE1 = 68,
  SE_SRC_IDX_STRIDE2 = 72,
  SE_SRC_IDX_STRIDE3 = 76,
  SE_SRC_IDX_STRIDE4 = 80,
  SE_SRC_IDX_STRIDE5 = 84,
  SE_SRC_IDX_STRIDE6 = 88,
  SE_SRC_IDX_STRIDE7 = 92,
  SE_DST_VAL_STRIDE0 = 96,
  SE_DST_VAL_STRIDE1 = 100,
  SE_DST_VAL_STRIDE2 = 104,
  SE_DST_VAL_STRIDE3 = 108,
  SE_DST_VAL_STRIDE4 = 112,
  SE_DST_VAL_STRIDE5 = 116,
  SE_DST_VAL_STRIDE6 = 120,
  SE_DST_VAL_STRIDE7 = 124,
  SE_DST_IDX_STRIDE0 = 128,
  SE_DST_IDX_STRIDE1 = 132,
  SE_DST_IDX_STRIDE2 = 136,
  SE_DST_IDX_STRIDE3 = 140,
  SE_DST_IDX_STRIDE4 = 144,
  SE_DST_IDX_STRIDE5 = 148,
  SE_DST_IDX_STRIDE6 = 152,
  SE_DST_IDX_STRIDE7 = 156,
  SE_SRC_VAL_LENGTH = 160,
  SE_SRC_IDX_LENGTH = 164,
  SE_DST_VAL_LENGTH = 168,
  SE_DST_IDX_LENGTH = 172,
  SE_THRESHOLD = 176,
  SE_READ_MASK_L = 180,
  SE_READ_MASK_H = 184,
  SE_MASK_INTERVAL = 188,
  SE_WRITE_COUNT = 192,
  SE_LOOP_COUNT0 = 208,
  SE_LOOP_COUNT1 = 212,
  SE_LOOP_COUNT2 = 216,
  SE_LOOP_COUNT3 = 220,
  SE_LOOP_COUNT4 = 224,
  SE_LOOP_COUNT5 = 228,
  SE_LOOP_COUNT6 = 232,
  SE_LOOP_COUNT7 = 236,
};

#ifdef __cplusplus
enum class DmaRegisterDef {
#else
enum DmaRegisterDef {
#endif
  DMAC_SET = 0,
  DMAC_STAT = 4,
  DMAC_SRC_TSR_STRIDE_L0 = 16,
  DMAC_SRC_TSR_STRIDE_H0 = 20,
  DMAC_SRC_TSR_STRIDE_L1 = 24,
  DMAC_SRC_TSR_STRIDE_H1 = 28,
  DMAC_SRC_TSR_STRIDE_L2 = 32,
  DMAC_SRC_TSR_STRIDE_H2 = 36,
  DMAC_SRC_TSR_STRIDE_L3 = 40,
  DMAC_SRC_TSR_STRIDE_H3 = 44,
  DMAC_SRC_TSR_STRIDE_L4 = 48,
  DMAC_SRC_TSR_STRIDE_H4 = 52,
  DMAC_SRC_TSR_STRIDE_L5 = 56,
  DMAC_SRC_TSR_STRIDE_H5 = 60,
  DMAC_SRC_TSR_STRIDE_L6 = 64,
  DMAC_SRC_TSR_STRIDE_H6 = 68,
  DMAC_SRC_TSR_STRIDE_L7 = 72,
  DMAC_SRC_TSR_STRIDE_H7 = 76,
  DMAC_DST_TSR_STRIDE_L0 = 80,
  DMAC_DST_TSR_STRIDE_H0 = 84,
  DMAC_DST_TSR_STRIDE_L1 = 88,
  DMAC_DST_TSR_STRIDE_H1 = 92,
  DMAC_DST_TSR_STRIDE_L2 = 96,
  DMAC_DST_TSR_STRIDE_H2 = 100,
  DMAC_DST_TSR_STRIDE_L3 = 104,
  DMAC_DST_TSR_STRIDE_H3 = 108,
  DMAC_DST_TSR_STRIDE_L4 = 112,
  DMAC_DST_TSR_STRIDE_H4 = 116,
  DMAC_DST_TSR_STRIDE_L5 = 120,
  DMAC_DST_TSR_STRIDE_H5 = 124,
  DMAC_DST_TSR_STRIDE_L6 = 128,
  DMAC_DST_TSR_STRIDE_H6 = 132,
  DMAC_DST_TSR_STRIDE_L7 = 136,
  DMAC_DST_TSR_STRIDE_H7 = 140,
  DMAC_SRC_TSR_LENGTH = 144,
  DMAC_DST_TSR_LENGTH = 152,
  DMAC_LOOP_COUNT0 = 160,
  DMAC_LOOP_COUNT1 = 164,
  DMAC_LOOP_COUNT2 = 168,
  DMAC_LOOP_COUNT3 = 172,
  DMAC_LOOP_COUNT4 = 176,
  DMAC_LOOP_COUNT5 = 180,
  DMAC_LOOP_COUNT6 = 184,
  DMAC_LOOP_COUNT7 = 188,
  DMAC_CONVERT_STRIDE_L = 192,
  DMAC_CONVERT_STRIDE_H = 196,
  DMAC_WORK_SIZE = 224,
  DMAC_SRC_BASE_ADDR_L = 240,
  DMAC_SRC_BASE_ADDR_H = 244,
  DMAC_DST_BASE_ADDR_L = 248,
  DMAC_DST_BASE_ADDR_H = 252,
  DMAC_SRC_BASE_FIFO_L = 256,
  DMAC_SRC_BASE_FIFO_H = 260,
  DMAC_DST_BASE_FIFO_L = 264,
  DMAC_DST_BASE_FIFO_H = 268,
  DMAC_BASE_FIFO_PUSH_EN = 272,
  DMAC_BASE_FIFO_EN = 276,
  DMAC_SRC_BASE_FIFO_SIZE = 280,
  DMAC_DST_BASE_FIFO_SIZE = 284,
  DMAC_DYN_IDX_MODE = 288,
  DMAC_DYN_IDX_ST_ADDR = 292,
  DMAC_DYN_IDX_LEN = 296,
  DMAC_DYN_SRC_SEG_LEN = 304,
  DMAC_DYN_DST_SEG_LEN = 308,
  DMAC_SRC_BASE_FIFO_ADDR_IDX = 320,
  DMAC_DST_BASE_FIFO_ADDR_IDX = 324,
  DMAC_SRC_BASE_FIFO_ADDR_POP = 328,
  DMAC_DST_BASE_FIFO_ADDR_POP = 332,
};

#ifdef __cplusplus
enum class CoreRegisterDef {
#else
enum CoreRegisterDef {
#endif
  CORE_EN_CTRL0 = 0,
  CORE_EN_CTRL1 = 4,
  CORE_EN_CTRL2 = 8,
  CORE_EN_CTRL3 = 12,
  CORE_EN_CTRL4 = 16,
  CORE_EN_CTRL5 = 20,
  CORE_DONE_INT_ST0 = 32,
  CORE_DONE_INT_ST1 = 36,
  CORE_DONE_INT_ST2 = 40,
  CORE_DONE_INT_ST3 = 44,
  CORE_DONE_INT_ST4 = 48,
  CORE_DONE_INT_ST5 = 52,
  CORE_RVV_IRQ_ST = 64,
  CORE_RVV_IRA_ST = 68,
  COLS_INSTR_CONFIG_L = 80,
  COLS_INSTR_CONFIG_H = 84,
  PELS0_I_INSTR_CONFIG_L = 96,
  PELS0_I_INSTR_CONFIG_H = 100,
  PELS0_E_INSTR_CONFIG_L = 104,
  PELS0_E_INSTR_CONFIG_H = 108,
  PELS0_O_INSTR_CONFIG_L = 112,
  PELS0_O_INSTR_CONFIG_H = 116,
  PELS1_I_INSTR_CONFIG_L = 128,
  PELS1_I_INSTR_CONFIG_H = 132,
  PELS1_E_INSTR_CONFIG_L = 136,
  PELS1_E_INSTR_CONFIG_H = 140,
  PELS1_O_INSTR_CONFIG_L = 144,
  PELS1_O_INSTR_CONFIG_H = 148,
  PELS2_I_INSTR_CONFIG_L = 160,
  PELS2_I_INSTR_CONFIG_H = 164,
  PELS2_E_INSTR_CONFIG_L = 168,
  PELS2_E_INSTR_CONFIG_H = 172,
  PELS2_O_INSTR_CONFIG_L = 176,
  PELS2_O_INSTR_CONFIG_H = 180,
  PELS3_I_INSTR_CONFIG_L = 192,
  PELS3_I_INSTR_CONFIG_H = 196,
  PELS3_E_INSTR_CONFIG_L = 200,
  PELS3_E_INSTR_CONFIG_H = 204,
  PELS3_O_INSTR_CONFIG_L = 208,
  PELS3_O_INSTR_CONFIG_H = 212,
  PELS4_I_INSTR_CONFIG_L = 224,
  PELS4_I_INSTR_CONFIG_H = 228,
  PELS4_E_INSTR_CONFIG_L = 232,
  PELS4_E_INSTR_CONFIG_H = 236,
  PELS4_O_INSTR_CONFIG_L = 240,
  PELS4_O_INSTR_CONFIG_H = 244,
  PELS5_I_INSTR_CONFIG_L = 256,
  PELS5_I_INSTR_CONFIG_H = 260,
  PELS5_E_INSTR_CONFIG_L = 264,
  PELS5_E_INSTR_CONFIG_H = 268,
  PELS5_O_INSTR_CONFIG_L = 272,
  PELS5_O_INSTR_CONFIG_H = 276,
  PELS6_I_INSTR_CONFIG_L = 288,
  PELS6_I_INSTR_CONFIG_H = 292,
  PELS6_E_INSTR_CONFIG_L = 296,
  PELS6_E_INSTR_CONFIG_H = 300,
  PELS6_O_INSTR_CONFIG_L = 304,
  PELS6_O_INSTR_CONFIG_H = 308,
  PELS7_I_INSTR_CONFIG_L = 320,
  PELS7_I_INSTR_CONFIG_H = 324,
  PELS7_E_INSTR_CONFIG_L = 328,
  PELS7_E_INSTR_CONFIG_H = 332,
  PELS7_O_INSTR_CONFIG_L = 336,
  PELS7_O_INSTR_CONFIG_H = 340,
  PELS8_I_INSTR_CONFIG_L = 352,
  PELS8_I_INSTR_CONFIG_H = 356,
  PELS8_E_INSTR_CONFIG_L = 360,
  PELS8_E_INSTR_CONFIG_H = 364,
  PELS8_O_INSTR_CONFIG_L = 368,
  PELS8_O_INSTR_CONFIG_H = 372,
  PELS9_I_INSTR_CONFIG_L = 384,
  PELS9_I_INSTR_CONFIG_H = 388,
  PELS9_E_INSTR_CONFIG_L = 392,
  PELS9_E_INSTR_CONFIG_H = 396,
  PELS9_O_INSTR_CONFIG_L = 400,
  PELS9_O_INSTR_CONFIG_H = 404,
  PELS10_I_INSTR_CONFIG_L = 416,
  PELS10_I_INSTR_CONFIG_H = 420,
  PELS10_E_INSTR_CONFIG_L = 424,
  PELS10_E_INSTR_CONFIG_H = 428,
  PELS10_O_INSTR_CONFIG_L = 432,
  PELS10_O_INSTR_CONFIG_H = 436,
  PELS11_I_INSTR_CONFIG_L = 448,
  PELS11_I_INSTR_CONFIG_H = 452,
  PELS11_E_INSTR_CONFIG_L = 456,
  PELS11_E_INSTR_CONFIG_H = 460,
  PELS11_O_INSTR_CONFIG_L = 464,
  PELS11_O_INSTR_CONFIG_H = 468,
  PELS12_I_INSTR_CONFIG_L = 480,
  PELS12_I_INSTR_CONFIG_H = 484,
  PELS12_E_INSTR_CONFIG_L = 488,
  PELS12_E_INSTR_CONFIG_H = 492,
  PELS12_O_INSTR_CONFIG_L = 496,
  PELS12_O_INSTR_CONFIG_H = 500,
  PELS13_I_INSTR_CONFIG_L = 512,
  PELS13_I_INSTR_CONFIG_H = 516,
  PELS13_E_INSTR_CONFIG_L = 520,
  PELS13_E_INSTR_CONFIG_H = 524,
  PELS13_O_INSTR_CONFIG_L = 528,
  PELS13_O_INSTR_CONFIG_H = 532,
  PELS14_I_INSTR_CONFIG_L = 544,
  PELS14_I_INSTR_CONFIG_H = 548,
  PELS14_E_INSTR_CONFIG_L = 552,
  PELS14_E_INSTR_CONFIG_H = 556,
  PELS14_O_INSTR_CONFIG_L = 560,
  PELS14_O_INSTR_CONFIG_H = 564,
  PELS15_I_INSTR_CONFIG_L = 576,
  PELS15_I_INSTR_CONFIG_H = 580,
  PELS15_E_INSTR_CONFIG_L = 584,
  PELS15_E_INSTR_CONFIG_H = 588,
  PELS15_O_INSTR_CONFIG_L = 592,
  PELS15_O_INSTR_CONFIG_H = 596,
  PE0_TCM_CONFIG_L = 608,
  PE0_TCM_CONFIG_H = 612,
  PE1_TCM_CONFIG_L = 616,
  PE1_TCM_CONFIG_H = 620,
  PE2_TCM_CONFIG_L = 624,
  PE2_TCM_CONFIG_H = 628,
  PE3_TCM_CONFIG_L = 632,
  PE3_TCM_CONFIG_H = 636,
  PE4_TCM_CONFIG_L = 640,
  PE4_TCM_CONFIG_H = 644,
  PE5_TCM_CONFIG_L = 648,
  PE5_TCM_CONFIG_H = 652,
  PE6_TCM_CONFIG_L = 656,
  PE6_TCM_CONFIG_H = 660,
  PE7_TCM_CONFIG_L = 664,
  PE7_TCM_CONFIG_H = 668,
  PE8_TCM_CONFIG_L = 672,
  PE8_TCM_CONFIG_H = 676,
  PE9_TCM_CONFIG_L = 680,
  PE9_TCM_CONFIG_H = 684,
  PE10_TCM_CONFIG_L = 688,
  PE10_TCM_CONFIG_H = 692,
  PE11_TCM_CONFIG_L = 696,
  PE11_TCM_CONFIG_H = 700,
  PE12_TCM_CONFIG_L = 704,
  PE12_TCM_CONFIG_H = 708,
  PE13_TCM_CONFIG_L = 712,
  PE13_TCM_CONFIG_H = 716,
  PE14_TCM_CONFIG_L = 720,
  PE14_TCM_CONFIG_H = 724,
  PE15_TCM_CONFIG_L = 728,
  PE15_TCM_CONFIG_H = 732,
  CODMAC0_INSTR_CONFIG_L = 736,
  CODMAC0_INSTR_CONFIG_H = 740,
  CODMAC1_INSTR_CONFIG_L = 744,
  CODMAC1_INSTR_CONFIG_H = 748,
  CODMAC2_INSTR_CONFIG_L = 752,
  CODMAC2_INSTR_CONFIG_H = 756,
  IDU_DISPATCH_MODE = 768,
  CORE_PARITY_CHECK = 784,
  CORE_MEM_CFG_L = 792,
  CORE_MEM_CFG_H = 796,
};

#ifdef __cplusplus
enum class CodmaRegisterDef {
#else
enum CodmaRegisterDef {
#endif
  CODMAC_SET = 0,
  CODMAC_STAT = 4,
  CODMAC_SRC_TSR_STRIDE_L0 = 16,
  CODMAC_SRC_TSR_STRIDE_H0 = 20,
  CODMAC_SRC_TSR_STRIDE_L1 = 24,
  CODMAC_SRC_TSR_STRIDE_H1 = 28,
  CODMAC_SRC_TSR_STRIDE_L2 = 32,
  CODMAC_SRC_TSR_STRIDE_H2 = 36,
  CODMAC_SRC_TSR_STRIDE_L3 = 40,
  CODMAC_SRC_TSR_STRIDE_H3 = 44,
  CODMAC_SRC_TSR_STRIDE_L4 = 48,
  CODMAC_SRC_TSR_STRIDE_H4 = 52,
  CODMAC_SRC_TSR_STRIDE_L5 = 56,
  CODMAC_SRC_TSR_STRIDE_H5 = 60,
  CODMAC_SRC_TSR_STRIDE_L6 = 64,
  CODMAC_SRC_TSR_STRIDE_H6 = 68,
  CODMAC_SRC_TSR_STRIDE_L7 = 72,
  CODMAC_SRC_TSR_STRIDE_H7 = 76,
  CODMAC_DST_TSR_STRIDE_L0 = 80,
  CODMAC_DST_TSR_STRIDE_H0 = 84,
  CODMAC_DST_TSR_STRIDE_L1 = 88,
  CODMAC_DST_TSR_STRIDE_H1 = 92,
  CODMAC_DST_TSR_STRIDE_L2 = 96,
  CODMAC_DST_TSR_STRIDE_H2 = 100,
  CODMAC_DST_TSR_STRIDE_L3 = 104,
  CODMAC_DST_TSR_STRIDE_H3 = 108,
  CODMAC_DST_TSR_STRIDE_L4 = 112,
  CODMAC_DST_TSR_STRIDE_H4 = 116,
  CODMAC_DST_TSR_STRIDE_L5 = 120,
  CODMAC_DST_TSR_STRIDE_H5 = 124,
  CODMAC_DST_TSR_STRIDE_L6 = 128,
  CODMAC_DST_TSR_STRIDE_H6 = 132,
  CODMAC_DST_TSR_STRIDE_L7 = 136,
  CODMAC_DST_TSR_STRIDE_H7 = 140,
  CODMAC_SRC_TSR_LENGTH = 144,
  CODMAC_DST_TSR_LENGTH = 152,
  CODMAC_LOOP_COUNT0 = 160,
  CODMAC_LOOP_COUNT1 = 164,
  CODMAC_LOOP_COUNT2 = 168,
  CODMAC_LOOP_COUNT3 = 172,
  CODMAC_LOOP_COUNT4 = 176,
  CODMAC_LOOP_COUNT5 = 180,
  CODMAC_LOOP_COUNT6 = 184,
  CODMAC_LOOP_COUNT7 = 188,
  CODMAC_CONVERT_STRIDE_L = 192,
  CODMAC_CONVERT_STRIDE_H = 196,
  CODMAC_WORK_SIZE = 224,
  CODMAC_SRC_BASE_ADDR_L = 240,
  CODMAC_SRC_BASE_ADDR_H = 244,
  CODMAC_DST_BASE_ADDR_L = 248,
  CODMAC_DST_BASE_ADDR_H = 252,
  CODMAC_SRC_BASE_FIFO_L = 256,
  CODMAC_SRC_BASE_FIFO_H = 260,
  CODMAC_DST_BASE_FIFO_L = 264,
  CODMAC_DST_BASE_FIFO_H = 268,
  CODMAC_BASE_FIFO_PUSH_EN = 272,
  CODMAC_BASE_FIFO_EN = 276,
  CODMAC_SRC_BASE_FIFO_SIZE = 280,
  CODMAC_DST_BASE_FIFO_SIZE = 284,
  CODMAC_SRC_BASE_FIFO_ADDR_IDX = 320,
  CODMAC_DST_BASE_FIFO_ADDR_IDX = 324,
  CODMAC_SRC_BASE_FIFO_ADDR_POP = 328,
  CODMAC_DST_BASE_FIFO_ADDR_POP = 332,
};

#ifdef __cplusplus
enum class ChipRegisterDef {
#else
enum ChipRegisterDef {
#endif
  CHIP_EN_CTRL0 = 0,
  CHIP_EN_CTRL1 = 4,
  CHIP_EN_CTRL2 = 8,
  CHIP_EN_CTRL3 = 12,
  CHIP_EN_CTRL4 = 16,
  CHIP_DONE_INT_ST0 = 32,
  CHIP_DONE_INT_ST1 = 36,
  CHIP_DONE_INT_ST2 = 40,
  CHIP_DONE_INT_ST3 = 44,
  CHIP_DONE_INT_ST4 = 48,
  CHIP_RVV_IRQ_ST = 64,
  CHIP_RVV_IRA_ST = 68,
  CHLS_INSTR_CONFIG_L = 80,
  CHLS_INSTR_CONFIG_H = 84,
  COLS0_INSTR_CONFIG_L = 96,
  COLS0_INSTR_CONFIG_H = 100,
  COLS1_INSTR_CONFIG_L = 104,
  COLS1_INSTR_CONFIG_H = 108,
  COLS2_INSTR_CONFIG_L = 112,
  COLS2_INSTR_CONFIG_H = 116,
  COLS3_INSTR_CONFIG_L = 120,
  COLS3_INSTR_CONFIG_H = 124,
  CORE0_RV_PRGM_CONFIG_L = 128,
  CORE0_RV_PRGM_CONFIG_H = 132,
  CORE1_RV_PRGM_CONFIG_L = 136,
  CORE1_RV_PRGM_CONFIG_H = 140,
  CORE2_RV_PRGM_CONFIG_L = 144,
  CORE2_RV_PRGM_CONFIG_H = 148,
  CORE3_RV_PRGM_CONFIG_L = 152,
  CORE3_RV_PRGM_CONFIG_H = 156,
  CHIP_PARITY_CHECK = 160,
  CHIP_MEM_CFG_L = 168,
  CHIP_MEM_CFG_H = 172,
};

#ifdef __cplusplus
} // namespace quark
#endif

#endif // QUARK_ADDR_MAP_H
