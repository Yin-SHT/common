/*
 * Copyright (c) 2018-2024 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef QUARK_SPU_ISA_H
#define QUARK_SPU_ISA_H

#include "quark/spu_instr.h"
#include "utils.h"

namespace quark {

class SpuIsa {
public:
  struct LoopBinding {
    uint64_t start = 0;
    uint64_t count = 0;
  };

public:
  explicit SpuIsa(EngineType engine);
  virtual ~SpuIsa() = default;
  virtual std::vector<std::shared_ptr<SpuInstr>>& getInstrs();
  virtual void push(std::shared_ptr<SpuInstr> instr);
  virtual size_t evalCycles();
  virtual bool loopExists();

  void nop() {
    instrs.push_back(SpuInstr::create<NopInstr>({}));
  }
  void ldt(size_t rSrcAddr, size_t bDst, size_t rLength, size_t layout, size_t dqt, size_t dtype, size_t bank, size_t addrInc) {
    instrs.push_back(SpuInstr::create<LdtInstr>({rSrcAddr, bDst, rLength, layout, dqt, dtype, bank, addrInc}));
  }
  void ldw(size_t rSrcAddr, size_t bDst, size_t rLength, size_t dtype, size_t bankc, size_t bankk, size_t addrInc) {
    instrs.push_back(SpuInstr::create<LdwInstr>({rSrcAddr, bDst, rLength, dtype, bankc, bankk, addrInc}));
  }
  void ldm(size_t rSrcAddr, size_t bDst, size_t rLength, size_t groupElement, size_t unknownSr, size_t nnz, size_t bank, size_t addrInc) {
    instrs.push_back(SpuInstr::create<LdmInstr>({rSrcAddr, bDst, rLength, groupElement, unknownSr, nnz, bank, addrInc}));
  }
  void stt(size_t bSrc, size_t rDstAddr, size_t rLength, size_t condensed, size_t qnt, size_t dtype, size_t bank, size_t addrInc) {
    instrs.push_back(SpuInstr::create<SttInstr>({bSrc, rDstAddr, rLength, condensed, qnt, dtype, bank, addrInc}));
  }
  void gemm(size_t scaling, size_t accSwitch, size_t iDtype, size_t wDtype, size_t nnzc, size_t nnzk) {
    instrs.push_back(SpuInstr::create<GemmInstr>({scaling, accSwitch, iDtype, wDtype, nnzc, nnzk}));
  }
  void tr(size_t mode, size_t row, size_t col, size_t elen) {
    instrs.push_back(SpuInstr::create<TrInstr>({mode, row, col, elen}));
  }
  void act(size_t func) {
    instrs.push_back(SpuInstr::create<ActInstr>({func}));
  }
  void srt(size_t rMask, size_t rInterval, size_t bitonic, size_t bank, size_t dtype, size_t order) {
    instrs.push_back(SpuInstr::create<SrtInstr>({rMask, rInterval, bitonic, bank, dtype, order}));
  }
  void cmp(size_t rMask, size_t rInterval, size_t rSeg, size_t bitonic, size_t bank, size_t dtype, size_t mode) {
    instrs.push_back(SpuInstr::create<CmpInstr>({rMask, rInterval, rSeg, bitonic, bank, dtype, mode}));
  }
  void tpk(size_t rMask, size_t rInterval, size_t k, size_t abs, size_t res, size_t bank, size_t dtype, size_t mode) {
    instrs.push_back(SpuInstr::create<TpkInstr>({rMask, rInterval, k, abs, res, bank, dtype, mode}));
  }
  void pck(size_t rMask, size_t rInterval, size_t rThreshold, size_t rResult, size_t res, size_t bank, size_t dtype, size_t mode) {
    instrs.push_back(SpuInstr::create<PckInstr>({rMask, rInterval, rThreshold, rResult, res, bank, dtype, mode}));
  }
  void cpy(size_t surIdx, size_t lineIdx, size_t lenIdx, size_t lowPrecision, size_t srcDynamic, size_t dstDynamic, size_t bank, size_t dtype, size_t extendSqueeze, size_t validLen, size_t formatConvert, size_t rConvertStride, size_t direction) {
    instrs.push_back(SpuInstr::create<CpyInstr>({surIdx, lineIdx, lenIdx, lowPrecision, srcDynamic, dstDynamic, bank, dtype, extendSqueeze, validLen, formatConvert, rConvertStride, direction}));
  }
  void snd(size_t surIdx, size_t lineIdx, size_t lenIdx, size_t peId) {
    instrs.push_back(SpuInstr::create<SndInstr>({surIdx, lineIdx, lenIdx, peId}));
  }
  void rcv(size_t rLength, size_t peId) {
    instrs.push_back(SpuInstr::create<RcvInstr>({rLength, peId}));
  }
  void lps(size_t loopId, size_t rCount) {
    instrs.push_back(SpuInstr::create<LpsInstr>({loopId, rCount}));
  }
  void lpsi(size_t loopId, size_t count) {
    instrs.push_back(SpuInstr::create<LpsiInstr>({loopId, count}));
  }
  void lpe(size_t loopId) {
    instrs.push_back(SpuInstr::create<LpeInstr>({loopId}));
  }
  void mov(size_t rDst, size_t rSrc) {
    instrs.push_back(SpuInstr::create<MovInstr>({rDst, rSrc}));
  }
  void movp(size_t srcId, size_t dstId, size_t addrMovMask) {
    instrs.push_back(SpuInstr::create<MovpInstr>({srcId, dstId, addrMovMask}));
  }
  void set(size_t rDst, size_t offset, size_t imm) {
    instrs.push_back(SpuInstr::create<SetInstr>({rDst, offset, imm}));
  }
  void aai(size_t rAddr, size_t rStride) {
    instrs.push_back(SpuInstr::create<AaiInstr>({rAddr, rStride}));
  }
  void aaip(size_t loopId, size_t addrIncMask) {
    instrs.push_back(SpuInstr::create<AaipInstr>({loopId, addrIncMask}));
  }
  void dcfg() {
    instrs.push_back(SpuInstr::create<DcfgInstr>({}));
  }
  void smx() {
    instrs.push_back(SpuInstr::create<SmxInstr>({}));
  }
  void gat(size_t nnz, size_t lpt, size_t bank, size_t dtype, size_t mode) {
    instrs.push_back(SpuInstr::create<GatInstr>({nnz, lpt, bank, dtype, mode}));
  }
  void sct(size_t nnz, size_t lpt, size_t bank, size_t dtype, size_t mode) {
    instrs.push_back(SpuInstr::create<SctInstr>({nnz, lpt, bank, dtype, mode}));
  }
  void end(size_t addressAlign) {
    instrs.push_back(SpuInstr::create<EndInstr>({addressAlign}));
  }

  void set(size_t rDst, size_t imm);
  void ldt(size_t rSrcAddr, size_t bDst, size_t rLength, size_t dqt, size_t dtype, size_t bank, size_t addrInc) {
    ldt(rSrcAddr, bDst, rLength, 0/*layout*/, dqt, dtype, bank, addrInc);
  }
  void srt(size_t rMask, size_t rInterval, size_t bank, size_t dtype, size_t order) {
    srt(rMask, rInterval, 0/*bitonic*/, bank, dtype, order);
  }
  void gemm(size_t accSwitch, size_t iDtype, size_t wDtype, size_t nnzc, size_t nnzk) {
    gemm(0/*scaling*/, accSwitch, iDtype, wDtype, nnzc, nnzk);
  }
  void end() {
    end(0);
  }

protected:
  uint64_t *getInnerRegister(size_t index);

protected:
  EngineType engine = EngineType::INVALID;
  std::vector<std::shared_ptr<SpuInstr>> instrs;
  uint64_t srcTsrLength = 0;
  uint64_t dstTsrLength = 0;
  std::vector<uint64_t> loopCounts;
  std::vector<LoopBinding> loopBindings;
};

} // namespace quark

#endif // QUARK_SPU_ISA_H
