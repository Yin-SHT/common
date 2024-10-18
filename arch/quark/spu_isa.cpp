/*
 * Copyright (c) 2018-2024 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#include "spu_isa.h"

namespace quark {

SpuIsa::SpuIsa(EngineType engine) : 
    engine(engine), loopCounts(8, 0), loopBindings(8) {
}

std::vector<std::shared_ptr<SpuInstr>>& SpuIsa::getInstrs() {
  while ((instrs.size() % (kIduTransGranularity/kSizePerSpuCmd)) != 0) {
    auto pos = instrs.end() - (std::dynamic_pointer_cast<EndInstr>(instrs.back()) ?1 :0);
    instrs.insert(pos, SpuInstr::create<NopInstr>({}));
  }
  return instrs;
}

void SpuIsa::push(std::shared_ptr<SpuInstr> instr) {
  instrs.push_back(instr);
}

size_t SpuIsa::evalCycles() {
  size_t cycles = 0;

  for (size_t pos = 0; pos < instrs.size(); ) {
    auto instr = instrs[pos++];
    if (auto x = std::dynamic_pointer_cast<SetInstr>(instr)) {
      auto reg = getInnerRegister(x->getRDst());
      if (reg) {
        size_t leftShift = x->getOffset()*16;
        *reg &= ~((uint64_t)0xffff << leftShift);
        *reg |= ((uint64_t)x->getImm()&0xffff) << leftShift;
      }
    } else if (auto x = std::dynamic_pointer_cast<AaiInstr>(instr)) {
      auto rAddr = getInnerRegister(x->getRAddr());
      auto rStride = getInnerRegister(x->getRStride());
      if (rAddr && rStride)
        *rAddr += *rStride;
    } else if (auto x = std::dynamic_pointer_cast<LpsInstr>(instr)) {
      loopBindings[x->getLoopId()].start = pos;
      loopBindings[x->getLoopId()].count = loopCounts[x->getRCount()];
    } else if (auto x = std::dynamic_pointer_cast<LpsiInstr>(instr)) {
      loopBindings[x->getLoopId()].start = pos;
      loopBindings[x->getLoopId()].count = x->getCount();
    } else if (auto x = std::dynamic_pointer_cast<LpeInstr>(instr)) {
      auto &loopCount = loopBindings[x->getLoopId()].count;
      if (loopCount > 0)
        loopCount -= 1;
      if (loopCount > 0)
        pos = loopBindings[x->getLoopId()].start;
    } else if (auto x = std::dynamic_pointer_cast<CpyInstr>(instr)) {
      cycles += loopCounts[x->getSurIdx()] * loopCounts[x->getLineIdx()] *
          std::max(srcTsrLength+1, dstTsrLength+1) / kIduTransGranularity;
    } else if (auto x = std::dynamic_pointer_cast<SndInstr>(instr)) {
      cycles += loopCounts[x->getSurIdx()] * loopCounts[x->getLineIdx()] *
          std::max(srcTsrLength+1, dstTsrLength+1) / kIduTransGranularity;
    }
    cycles++;
  }
  return cycles;
}

void SpuIsa::set(size_t rDst, size_t imm) {
  set(rDst, 0, (imm>>0)&0xffff);
  set(rDst, 1, (imm>>16)&0xffff);
  set(rDst, 2, (imm>>32)&0xffff);
  set(rDst, 3, (imm>>48)&0xffff);
}

//void SpuIsa::cpy(size_t rSrc, size_t rDst, size_t rLen, size_t nGlbElem, 
//    DataType dtype, bool extend, bool squeeze, size_t nDdrElem, 
//  bool formatConvert, size_t rStride, size_t nocPath) {
//  size_t dtype2;
//  switch(dtype) {
//  case DTYPE_BF16:  dtype2 = 1;   break;
//  case DTYPE_FP16:  dtype2 = 2;   break;
//  case DTYPE_FP32:  dtype2 = 3;   break;
//  default:          dtype2 = 0;   break;
//  }
//  size_t validLen = nDdrElem*dTypeWidthMapping.at(dtype);
//  instrs.push_back(SpuInstr::create<CpyInstr>({
//      rSrc, rDst, rLen, nGlbElem, dtype2, extend, squeeze, 
//      validLen, formatConvert, rStride, nocPath}));
//}

bool SpuIsa::loopExists() {
  for (auto &it : instrs) {
    if (std::dynamic_pointer_cast<LpeInstr>(it))
      return true;
  }
  return false;
}

uint64_t *SpuIsa::getInnerRegister(size_t index) {
  switch (engine) {
    case EngineType::DMA0:
    case EngineType::DMA1:
    case EngineType::DMA2:
    case EngineType::DMA3:
    case EngineType::DMA4:
    case EngineType::DMA5:
      switch (DmaSetIndexMap(index)) {
        case DmaSetIndexMap::DmacSrcTsrLength:  return &srcTsrLength;
        case DmaSetIndexMap::DmacDstTsrLength:  return &dstTsrLength;
        case DmaSetIndexMap::DmacLoopCount0:    return &loopCounts[0];
        case DmaSetIndexMap::DmacLoopCount1:    return &loopCounts[1];
        //case DmaSetIndexMap::DmacLoopCount2:    return &loopCounts[2];
        //case DmaSetIndexMap::DmacLoopCount3:    return &loopCounts[3];
        //case DmaSetIndexMap::DmacLoopCount4:    return &loopCounts[4];
        //case DmaSetIndexMap::DmacLoopCount5:    return &loopCounts[5];
        //case DmaSetIndexMap::DmacLoopCount6:    return &loopCounts[6];
        //case DmaSetIndexMap::DmacLoopCount7:    return &loopCounts[7];
        default:
          break;
      }
      break;
    default:
      break;
  }
  return nullptr;
}

} // namespace quark
