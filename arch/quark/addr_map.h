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

#ifdef __cplusplus
struct AddrRange {
  AddrRange(size_t start, size_t end) :
      start(start), end(end) {}
  bool check(size_t addr) const {
    return (addr >= start && addr <= end);
  }
  size_t start;
  size_t end;
};
#else
typedef struct {
  size_t start;
  size_t end;
} AddrRange;
#endif

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
const AddrRange kCoreExceptionRegMap = {0x9000, 0x9fff};
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
  SpuSrcAcpAddr0 = 16,
  SpuSrcAcpAddr1 = 17,
  SpuSrcAcpAddr2 = 18,
  SpuSrcAcpAddr3 = 19,
  SpuSrcAcpAddr4 = 20,
  SpuSrcAcpAddr5 = 21,
  SpuSrcAcpAddr6 = 22,
  SpuSrcAcpAddr7 = 23,
  SpuSrcMiAddr0 = 24,
  SpuSrcMiAddr1 = 25,
  SpuSrcMiAddr2 = 26,
  SpuSrcMiAddr3 = 27,
  SpuSrcMiAddr4 = 28,
  SpuSrcMiAddr5 = 29,
  SpuSrcMiAddr6 = 30,
  SpuSrcMiAddr7 = 31,
  SpuSrcScale0Addr0 = 32,
  SpuSrcScale0Addr1 = 33,
  SpuSrcScale0Addr2 = 34,
  SpuSrcScale0Addr3 = 35,
  SpuSrcScale0Addr4 = 36,
  SpuSrcScale0Addr5 = 37,
  SpuSrcScale0Addr6 = 38,
  SpuSrcScale0Addr7 = 39,
  SpuSrcScale1Addr0 = 40,
  SpuSrcScale1Addr1 = 41,
  SpuSrcScale1Addr2 = 42,
  SpuSrcScale1Addr3 = 43,
  SpuSrcScale1Addr4 = 44,
  SpuSrcScale1Addr5 = 45,
  SpuSrcScale1Addr6 = 46,
  SpuSrcScale1Addr7 = 47,
  SpuSrcScale2Addr0 = 48,
  SpuSrcScale2Addr1 = 49,
  SpuSrcScale2Addr2 = 50,
  SpuSrcScale2Addr3 = 51,
  SpuSrcScale2Addr4 = 52,
  SpuSrcScale2Addr5 = 53,
  SpuSrcScale2Addr6 = 54,
  SpuSrcScale2Addr7 = 55,
  SpuSrcAccAddr0 = 56,
  SpuSrcAccAddr1 = 57,
  SpuSrcAccAddr2 = 58,
  SpuSrcAccAddr3 = 59,
  SpuSrcAccAddr4 = 60,
  SpuSrcAccAddr5 = 61,
  SpuSrcAccAddr6 = 62,
  SpuSrcAccAddr7 = 63,
  SpuDstTsrAddr0 = 64,
  SpuDstTsrAddr1 = 65,
  SpuDstTsrAddr2 = 66,
  SpuDstTsrAddr3 = 67,
  SpuDstTsrAddr4 = 68,
  SpuDstTsrAddr5 = 69,
  SpuDstTsrAddr6 = 70,
  SpuDstTsrAddr7 = 71,
  SpuSrcTsrStride0 = 72,
  SpuSrcTsrStride1 = 73,
  SpuSrcTsrStride2 = 74,
  SpuSrcTsrStride3 = 75,
  SpuSrcTsrStride4 = 76,
  SpuSrcTsrStride5 = 77,
  SpuSrcTsrStride6 = 78,
  SpuSrcTsrStride7 = 79,
  SpuSrcWgtStride0 = 80,
  SpuSrcWgtStride1 = 81,
  SpuSrcWgtStride2 = 82,
  SpuSrcWgtStride3 = 83,
  SpuSrcWgtStride4 = 84,
  SpuSrcWgtStride5 = 85,
  SpuSrcWgtStride6 = 86,
  SpuSrcWgtStride7 = 87,
  SpuSrcAcpStride0 = 88,
  SpuSrcAcpStride1 = 89,
  SpuSrcAcpStride2 = 90,
  SpuSrcAcpStride3 = 91,
  SpuSrcAcpStride4 = 92,
  SpuSrcAcpStride5 = 93,
  SpuSrcAcpStride6 = 94,
  SpuSrcAcpStride7 = 95,
  SpuSrcMiStride0 = 96,
  SpuSrcMiStride1 = 97,
  SpuSrcMiStride2 = 98,
  SpuSrcMiStride3 = 99,
  SpuSrcMiStride4 = 100,
  SpuSrcMiStride5 = 101,
  SpuSrcMiStride6 = 102,
  SpuSrcMiStride7 = 103,
  SpuSrcScale0Stride0 = 104,
  SpuSrcScale0Stride1 = 105,
  SpuSrcScale0Stride2 = 106,
  SpuSrcScale0Stride3 = 107,
  SpuSrcScale0Stride4 = 108,
  SpuSrcScale0Stride5 = 109,
  SpuSrcScale0Stride6 = 110,
  SpuSrcScale0Stride7 = 111,
  SpuSrcScale1Stride0 = 112,
  SpuSrcScale1Stride1 = 113,
  SpuSrcScale1Stride2 = 114,
  SpuSrcScale1Stride3 = 115,
  SpuSrcScale1Stride4 = 116,
  SpuSrcScale1Stride5 = 117,
  SpuSrcScale1Stride6 = 118,
  SpuSrcScale1Stride7 = 119,
  SpuSrcScale2Stride0 = 120,
  SpuSrcScale2Stride1 = 121,
  SpuSrcScale2Stride2 = 122,
  SpuSrcScale2Stride3 = 123,
  SpuSrcScale2Stride4 = 124,
  SpuSrcScale2Stride5 = 125,
  SpuSrcScale2Stride6 = 126,
  SpuSrcScale2Stride7 = 127,
  SpuSrcAccStride0 = 128,
  SpuSrcAccStride1 = 129,
  SpuSrcAccStride2 = 130,
  SpuSrcAccStride3 = 131,
  SpuSrcAccStride4 = 132,
  SpuSrcAccStride5 = 133,
  SpuSrcAccStride6 = 134,
  SpuSrcAccStride7 = 135,
  SpuDstTsrStride0 = 136,
  SpuDstTsrStride1 = 137,
  SpuDstTsrStride2 = 138,
  SpuDstTsrStride3 = 139,
  SpuDstTsrStride4 = 140,
  SpuDstTsrStride5 = 141,
  SpuDstTsrStride6 = 142,
  SpuDstTsrStride7 = 143,
  SpuSrcTsrLength0 = 144,
  SpuSrcTsrLength1 = 145,
  SpuSrcWgtLength = 146,
  SpuSrcAcpLength0 = 147,
  SpuSrcAcpLength1 = 148,
  SpuSrcMiLength0 = 149,
  SpuSrcMiLength1 = 150,
  SpuSrcScale0Length = 151,
  SpuSrcScale1Length0 = 152,
  SpuSrcScale1Length1 = 153,
  SpuSrcScale2Length0 = 154,
  SpuSrcScale2Length1 = 155,
  SpuSrcBiasLength = 156,
  SpuSrcAccLength0 = 157,
  SpuSrcAccLength1 = 158,
  SpuDstTsrLength0 = 159,
  SpuDstTsrLength1 = 160,
  SpuDstTsrLength2 = 161,
  SpuDstTsrLength3 = 162,
  SpuSrcTsrBankStride = 163,
  SpuSrcWgtBankStride = 164,
  SpuSrcAcpBankStride = 165,
  SpuSrcMiBankStride = 166,
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
  SeSrcTsr0Addr0 = 0,
  SeSrcTsr0Addr1 = 1,
  SeSrcTsr0Addr2 = 2,
  SeSrcTsr0Addr3 = 3,
  SeSrcTsr0Addr4 = 4,
  SeSrcTsr0Addr5 = 5,
  SeSrcTsr0Addr6 = 6,
  SeSrcTsr0Addr7 = 7,
  SeSrcTsr1Addr0 = 8,
  SeSrcTsr1Addr1 = 9,
  SeSrcTsr1Addr2 = 10,
  SeSrcTsr1Addr3 = 11,
  SeSrcTsr1Addr4 = 12,
  SeSrcTsr1Addr5 = 13,
  SeSrcTsr1Addr6 = 14,
  SeSrcTsr1Addr7 = 15,
  SeDstTsr0Addr0 = 16,
  SeDstTsr0Addr1 = 17,
  SeDstTsr0Addr2 = 18,
  SeDstTsr0Addr3 = 19,
  SeDstTsr0Addr4 = 20,
  SeDstTsr0Addr5 = 21,
  SeDstTsr0Addr6 = 22,
  SeDstTsr0Addr7 = 23,
  SeDstTsr1Addr0 = 24,
  SeDstTsr1Addr1 = 25,
  SeDstTsr1Addr2 = 26,
  SeDstTsr1Addr3 = 27,
  SeDstTsr1Addr4 = 28,
  SeDstTsr1Addr5 = 29,
  SeDstTsr1Addr6 = 30,
  SeDstTsr1Addr7 = 31,
  SeSrcTsr0Stride0 = 32,
  SeSrcTsr0Stride1 = 33,
  SeSrcTsr0Stride2 = 34,
  SeSrcTsr0Stride3 = 35,
  SeSrcTsr0Stride4 = 36,
  SeSrcTsr0Stride5 = 37,
  SeSrcTsr0Stride6 = 38,
  SeSrcTsr0Stride7 = 39,
  SeSrcTsr1Stride0 = 40,
  SeSrcTsr1Stride1 = 41,
  SeSrcTsr1Stride2 = 42,
  SeSrcTsr1Stride3 = 43,
  SeSrcTsr1Stride4 = 44,
  SeSrcTsr1Stride5 = 45,
  SeSrcTsr1Stride6 = 46,
  SeSrcTsr1Stride7 = 47,
  SeDstTsr0Stride0 = 48,
  SeDstTsr0Stride1 = 49,
  SeDstTsr0Stride2 = 50,
  SeDstTsr0Stride3 = 51,
  SeDstTsr0Stride4 = 52,
  SeDstTsr0Stride5 = 53,
  SeDstTsr0Stride6 = 54,
  SeDstTsr0Stride7 = 55,
  SeDstTsr1Stride0 = 56,
  SeDstTsr1Stride1 = 57,
  SeDstTsr1Stride2 = 58,
  SeDstTsr1Stride3 = 59,
  SeDstTsr1Stride4 = 60,
  SeDstTsr1Stride5 = 61,
  SeDstTsr1Stride6 = 62,
  SeDstTsr1Stride7 = 63,
  SeSrcTsr0Length = 64,
  SeSrcTsr1Length = 65,
  SeDstTsr0Length = 66,
  SeDstTsr1Length = 67,
  SeScaleConst = 68,
  SeSrc0BankStride = 69,
  SeSrc1BankStride = 70,
  SeDst0BankStride = 71,
  SeDst1BankStride = 72,
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
  DmacLengthRemainder = 52,
  DmacNarrowAlign = 53,
  DmacC2cDcid = 54,
  DmacC2cOption = 55,
  DmacC2cUmsgL = 56,
  DmacC2cUmsgH = 57,
  DmacC2cCfgSt = 58,
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
  CodmacConvertStride = 42,
  CodmacSrcBaseFifoAddrIdx = 43,
  CodmacDstBaseFifoAddrIdx = 44,
  CodmacSrcBaseFifoAddrPop = 45,
  CodmacDstBaseFifoAddrPop = 46,
  CodmacLengthRemainder = 47,
  CodmacNarrowAlign = 48,
  CodmacC2cDcid = 49,
  CodmacC2cOption = 50,
  CodmacC2cUmsgL = 51,
  CodmacC2cUmsgH = 52,
  CodmacC2cCfgSt = 53,
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
  PE_DONE_INT_ST0 = 32,
  PE_DONE_INT_ST1 = 36,
  PE_DONE_INT_ST2 = 40,
  PE_DONE_INT_ST3 = 44,
  PE_DONE_INT_ST4 = 48,
  WAIT_PE_IDLE_CFG = 64,
  WAIT_PE_BOUNDARY_CFG = 68,
  PE_RST_TIME_CFG = 72,
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
  SPU_SRC_ACP_ADDR0 = 24,
  SPU_SRC_MI_ADDR0 = 28,
  SPU_SRC_SCALE0_ADDR0 = 32,
  SPU_SRC_SCALE1_ADDR0 = 36,
  SPU_SRC_SCALE2_ADDR0 = 40,
  SPU_SRC_ACC_ADDR0 = 44,
  SPU_DST_TSR_ADDR0 = 48,
  SPU_SRC_TSR_STRIDE0 = 64,
  SPU_SRC_TSR_STRIDE1 = 68,
  SPU_SRC_TSR_STRIDE2 = 72,
  SPU_SRC_TSR_STRIDE3 = 76,
  SPU_SRC_TSR_STRIDE4 = 80,
  SPU_SRC_TSR_STRIDE5 = 84,
  SPU_SRC_TSR_STRIDE6 = 88,
  SPU_SRC_TSR_STRIDE7 = 92,
  SPU_SRC_WGT_STRIDE0 = 96,
  SPU_SRC_WGT_STRIDE1 = 100,
  SPU_SRC_WGT_STRIDE2 = 104,
  SPU_SRC_WGT_STRIDE3 = 108,
  SPU_SRC_WGT_STRIDE4 = 112,
  SPU_SRC_WGT_STRIDE5 = 116,
  SPU_SRC_WGT_STRIDE6 = 120,
  SPU_SRC_WGT_STRIDE7 = 124,
  SPU_SRC_ACP_STRIDE0 = 128,
  SPU_SRC_ACP_STRIDE1 = 132,
  SPU_SRC_ACP_STRIDE2 = 136,
  SPU_SRC_ACP_STRIDE3 = 140,
  SPU_SRC_ACP_STRIDE4 = 144,
  SPU_SRC_ACP_STRIDE5 = 148,
  SPU_SRC_ACP_STRIDE6 = 152,
  SPU_SRC_ACP_STRIDE7 = 156,
  SPU_SRC_MI_STRIDE0 = 160,
  SPU_SRC_MI_STRIDE1 = 164,
  SPU_SRC_MI_STRIDE2 = 168,
  SPU_SRC_MI_STRIDE3 = 172,
  SPU_SRC_MI_STRIDE4 = 176,
  SPU_SRC_MI_STRIDE5 = 180,
  SPU_SRC_MI_STRIDE6 = 184,
  SPU_SRC_MI_STRIDE7 = 188,
  SPU_SRC_SCALE0_STRIDE0 = 192,
  SPU_SRC_SCALE0_STRIDE1 = 196,
  SPU_SRC_SCALE0_STRIDE2 = 200,
  SPU_SRC_SCALE0_STRIDE3 = 204,
  SPU_SRC_SCALE0_STRIDE4 = 208,
  SPU_SRC_SCALE0_STRIDE5 = 212,
  SPU_SRC_SCALE0_STRIDE6 = 216,
  SPU_SRC_SCALE0_STRIDE7 = 220,
  SPU_SRC_SCALE1_STRIDE0 = 224,
  SPU_SRC_SCALE1_STRIDE1 = 228,
  SPU_SRC_SCALE1_STRIDE2 = 232,
  SPU_SRC_SCALE1_STRIDE3 = 236,
  SPU_SRC_SCALE1_STRIDE4 = 240,
  SPU_SRC_SCALE1_STRIDE5 = 244,
  SPU_SRC_SCALE1_STRIDE6 = 248,
  SPU_SRC_SCALE1_STRIDE7 = 252,
  SPU_SRC_SCALE2_STRIDE0 = 256,
  SPU_SRC_SCALE2_STRIDE1 = 260,
  SPU_SRC_SCALE2_STRIDE2 = 264,
  SPU_SRC_SCALE2_STRIDE3 = 268,
  SPU_SRC_SCALE2_STRIDE4 = 272,
  SPU_SRC_SCALE2_STRIDE5 = 276,
  SPU_SRC_SCALE2_STRIDE6 = 280,
  SPU_SRC_SCALE2_STRIDE7 = 284,
  SPU_SRC_ACC_STRIDE0 = 288,
  SPU_SRC_ACC_STRIDE1 = 292,
  SPU_SRC_ACC_STRIDE2 = 296,
  SPU_SRC_ACC_STRIDE3 = 300,
  SPU_SRC_ACC_STRIDE4 = 304,
  SPU_SRC_ACC_STRIDE5 = 308,
  SPU_SRC_ACC_STRIDE6 = 312,
  SPU_SRC_ACC_STRIDE7 = 316,
  SPU_DST_TSR_STRIDE0 = 320,
  SPU_DST_TSR_STRIDE1 = 324,
  SPU_DST_TSR_STRIDE2 = 328,
  SPU_DST_TSR_STRIDE3 = 332,
  SPU_DST_TSR_STRIDE4 = 336,
  SPU_DST_TSR_STRIDE5 = 340,
  SPU_DST_TSR_STRIDE6 = 344,
  SPU_DST_TSR_STRIDE7 = 348,
  SPU_SRC_TSR_LENGTH0 = 352,
  SPU_SRC_TSR_LENGTH1 = 356,
  SPU_SRC_WGT_LENGTH = 360,
  SPU_SRC_ACP_LENGTH0 = 368,
  SPU_SRC_ACP_LENGTH1 = 372,
  SPU_SRC_MI_LENGTH0 = 376,
  SPU_SRC_MI_LENGTH1 = 380,
  SPU_SRC_SCALE0_LENGTH = 384,
  SPU_SRC_SCALE1_LENGTH0 = 392,
  SPU_SRC_SCALE1_LENGTH1 = 396,
  SPU_SRC_SCALE2_LENGTH0 = 400,
  SPU_SRC_SCALE2_LENGTH1 = 404,
  SPU_SRC_BIAS_LENGTH = 408,
  SPU_DST_TSR_LENGTH0 = 432,
  SPU_DST_TSR_LENGTH1 = 436,
  SPU_DST_TSR_LENGTH2 = 440,
  SPU_DST_TSR_LENGTH3 = 444,
  SPU_SRC_TSR_BANK_STRIDE = 448,
  SPU_SRC_WGT_BANK_STRIDE = 452,
  SPU_SRC_ACP_BANK_STRIDE = 456,
  SPU_SRC_MI_BANK_STRIDE = 460,
  SPU_LOOP_COUNT0 = 464,
  SPU_LOOP_COUNT1 = 468,
  SPU_LOOP_COUNT2 = 472,
  SPU_LOOP_COUNT3 = 476,
  SPU_LOOP_COUNT4 = 480,
  SPU_LOOP_COUNT5 = 484,
  SPU_LOOP_COUNT6 = 488,
  SPU_LOOP_COUNT7 = 492,
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
  SE_SRC_TSR0_ADDR0 = 16,
  SE_SRC_TSR1_ADDR0 = 20,
  SE_DST_TSR0_ADDR0 = 24,
  SE_DST_TSR1_ADDR0 = 28,
  SE_SRC_TSR0_STRIDE0 = 32,
  SE_SRC_TSR0_STRIDE1 = 36,
  SE_SRC_TSR0_STRIDE2 = 40,
  SE_SRC_TSR0_STRIDE3 = 44,
  SE_SRC_TSR0_STRIDE4 = 48,
  SE_SRC_TSR0_STRIDE5 = 52,
  SE_SRC_TSR0_STRIDE6 = 56,
  SE_SRC_TSR0_STRIDE7 = 60,
  SE_SRC_TSR1_STRIDE0 = 64,
  SE_SRC_TSR1_STRIDE1 = 68,
  SE_SRC_TSR1_STRIDE2 = 72,
  SE_SRC_TSR1_STRIDE3 = 76,
  SE_SRC_TSR1_STRIDE4 = 80,
  SE_SRC_TSR1_STRIDE5 = 84,
  SE_SRC_TSR1_STRIDE6 = 88,
  SE_SRC_TSR1_STRIDE7 = 92,
  SE_DST_TSR0_STRIDE0 = 96,
  SE_DST_TSR0_STRIDE1 = 100,
  SE_DST_TSR0_STRIDE2 = 104,
  SE_DST_TSR0_STRIDE3 = 108,
  SE_DST_TSR0_STRIDE4 = 112,
  SE_DST_TSR0_STRIDE5 = 116,
  SE_DST_TSR0_STRIDE6 = 120,
  SE_DST_TSR0_STRIDE7 = 124,
  SE_DST_TSR1_STRIDE0 = 128,
  SE_DST_TSR1_STRIDE1 = 132,
  SE_DST_TSR1_STRIDE2 = 136,
  SE_DST_TSR1_STRIDE3 = 140,
  SE_DST_TSR1_STRIDE4 = 144,
  SE_DST_TSR1_STRIDE5 = 148,
  SE_DST_TSR1_STRIDE6 = 152,
  SE_DST_TSR1_STRIDE7 = 156,
  SE_SRC_TSR0_LENGTH = 160,
  SE_SRC_TSR1_LENGTH = 164,
  SE_DST_TSR0_LENGTH = 168,
  SE_DST_TSR1_LENGTH = 172,
  SE_SCALE_CONST = 176,
  SE_SRC_TSR0_BANK_STRIDE = 192,
  SE_SRC_TSR1_BANK_STRIDE = 196,
  SE_DST_TSR0_BANK_STRIDE = 200,
  SE_DST_TSR1_BANK_STRIDE = 204,
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
  DMAC_NARROW_ALIGN = 8,
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
  DMAC_LENGTH_REMAINDER = 156,
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
  DMAC_C2C_DCID = 336,
  DMAC_C2C_OPTION = 340,
  DMAC_C2C_UMSG_L = 344,
  DMAC_C2C_UMSG_H = 348,
  DMAC_C2C_CFG_ST = 352,
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
  CODMAC_NARROW_ALIGN = 8,
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
  CODMAC_LENGTH_REMAINDER = 156,
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
