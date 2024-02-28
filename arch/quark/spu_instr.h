/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef QUARK_SPU_INSTR_H
#define QUARK_SPU_INSTR_H

#include <stdint.h>
#include <assert.h>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>

namespace quark {

#define QUARK_GEN_GETTER_SETTER(field, high, low) \
  uint32_t get##field() { return (fields >> low) & (((uint32_t)1<<(high-low+1))-1); } \
  void set##field(uint32_t field) { \
    uint32_t m = (((uint32_t)1<<(high-low+1))-1)<<low; fields &= ~m; fields |= (field<<low)&m; }

#define QUARK_PUSH_GETTER_SETTER(field) do {\
  getters.push_back([this](){ return get##field(); });\
  setters.push_back([this](uint32_t x){ set##field(x); });\
} while(0)

class SpuInstr {
public:
  enum class OpCodeType { LDT = 0, LDW, LDM, STT, GEMM, TR, TRA, ACT, 
    ACTI, ACTA, SORT, TOPK, CPY, CPYA, QNT, QNTI, DQT, DQTI, LPS, 
    LPSI, LPE, SET, SETI, SETIH, SETIL, AAI, END, INVALID = 255 };

public:
  explicit SpuInstr(OpCodeType opCode) 
    : opCode(opCode) {}
  virtual ~SpuInstr() = default;

  static std::string opCodeToString(OpCodeType opCode);
  static OpCodeType stringToOpCode(std::string name);

  OpCodeType getOpCode();
  bool loadBinary(uint32_t binary);
  uint32_t toBinary();
  bool loadText(std::string text);
  std::string toText();

  template <typename T>  
  static std::shared_ptr<T> create(const std::vector<size_t> &args) {
    std::shared_ptr<T> x(new T);
    assert(x->setters.size() == args.size());
    for (size_t i = 0; i < args.size(); ++i) {
      x->setters[i](args[i]);
    }
    return x;
  }

protected:
  OpCodeType opCode;
  uint32_t fields = 0;
  std::vector<std::function<uint32_t(void)>> getters;
  std::vector<std::function<void(uint32_t)>> setters;
};

class LdtInstr : public SpuInstr {
public:
  explicit LdtInstr() : SpuInstr(OpCodeType::LDT) {
    QUARK_PUSH_GETTER_SETTER(RSrcAddr);
    QUARK_PUSH_GETTER_SETTER(Length);
    QUARK_PUSH_GETTER_SETTER(Condensed);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(SparseRate);
    QUARK_PUSH_GETTER_SETTER(Dtype);
  }
  QUARK_GEN_GETTER_SETTER(RSrcAddr, 25, 23);
  QUARK_GEN_GETTER_SETTER(Length, 22, 13);
  QUARK_GEN_GETTER_SETTER(Condensed, 12, 12);
  QUARK_GEN_GETTER_SETTER(Bank, 11, 10);
  QUARK_GEN_GETTER_SETTER(SparseRate, 9, 8);
  QUARK_GEN_GETTER_SETTER(Dtype, 6, 4);
};

class LdwInstr : public SpuInstr {
public:
  explicit LdwInstr() : SpuInstr(OpCodeType::LDW) {
    QUARK_PUSH_GETTER_SETTER(RSrcAddr);
    QUARK_PUSH_GETTER_SETTER(Length);
    QUARK_PUSH_GETTER_SETTER(Bankc);
    QUARK_PUSH_GETTER_SETTER(Bankk);
    QUARK_PUSH_GETTER_SETTER(Dtype);
  }
  QUARK_GEN_GETTER_SETTER(RSrcAddr, 25, 20);
  QUARK_GEN_GETTER_SETTER(Length, 19, 10);
  QUARK_GEN_GETTER_SETTER(Bankc, 9, 8);
  QUARK_GEN_GETTER_SETTER(Bankk, 7, 6);
  QUARK_GEN_GETTER_SETTER(Dtype, 5, 2);
};

class LdmInstr : public SpuInstr {
public:
  explicit LdmInstr() : SpuInstr(OpCodeType::LDM) {
    QUARK_PUSH_GETTER_SETTER(RSrcAddr);
    QUARK_PUSH_GETTER_SETTER(Length);
    QUARK_PUSH_GETTER_SETTER(DstRegion);
    QUARK_PUSH_GETTER_SETTER(GroupElement);
    QUARK_PUSH_GETTER_SETTER(SparseRate);
  }
  QUARK_GEN_GETTER_SETTER(RSrcAddr, 25, 23);
  QUARK_GEN_GETTER_SETTER(Length, 22, 13);
  QUARK_GEN_GETTER_SETTER(DstRegion, 12, 11);
  QUARK_GEN_GETTER_SETTER(GroupElement, 10, 9);
  QUARK_GEN_GETTER_SETTER(SparseRate, 8, 6);
};

class SttInstr : public SpuInstr {
public:
  explicit SttInstr() : SpuInstr(OpCodeType::STT) {
    QUARK_PUSH_GETTER_SETTER(SrcRegion);
    QUARK_PUSH_GETTER_SETTER(RDstAddr);
    QUARK_PUSH_GETTER_SETTER(Length);
    QUARK_PUSH_GETTER_SETTER(Condensed);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(SparseRate);
  }
  QUARK_GEN_GETTER_SETTER(SrcRegion, 25, 23);
  QUARK_GEN_GETTER_SETTER(RDstAddr, 22, 20);
  QUARK_GEN_GETTER_SETTER(Length, 19, 10);
  QUARK_GEN_GETTER_SETTER(Condensed, 9, 9);
  QUARK_GEN_GETTER_SETTER(Bank, 8, 7);
  QUARK_GEN_GETTER_SETTER(SparseRate, 6, 4);
};

class GemmInstr : public SpuInstr {
public:
  explicit GemmInstr() : SpuInstr(OpCodeType::GEMM) {
    QUARK_PUSH_GETTER_SETTER(IDtype);
    QUARK_PUSH_GETTER_SETTER(WDtype);
  }
  QUARK_GEN_GETTER_SETTER(IDtype, 25, 23);
  QUARK_GEN_GETTER_SETTER(WDtype, 22, 20);
};

class TrInstr : public SpuInstr {
public:
  explicit TrInstr() : SpuInstr(OpCodeType::TR) {
    QUARK_PUSH_GETTER_SETTER(Row);
    QUARK_PUSH_GETTER_SETTER(Col);
    QUARK_PUSH_GETTER_SETTER(Elen);
  }
  QUARK_GEN_GETTER_SETTER(Row, 25, 24);
  QUARK_GEN_GETTER_SETTER(Col, 23, 22);
  QUARK_GEN_GETTER_SETTER(Elen, 21, 10);
};

class TraInstr : public SpuInstr {
public:
  explicit TraInstr() : SpuInstr(OpCodeType::TRA) {
    QUARK_PUSH_GETTER_SETTER(Row);
    QUARK_PUSH_GETTER_SETTER(Col);
    QUARK_PUSH_GETTER_SETTER(Elen);
    QUARK_PUSH_GETTER_SETTER(RSrcAddr);
    QUARK_PUSH_GETTER_SETTER(RDstAddr);
  }
  QUARK_GEN_GETTER_SETTER(Row, 25, 24);
  QUARK_GEN_GETTER_SETTER(Col, 23, 22);
  QUARK_GEN_GETTER_SETTER(Elen, 21, 10);
  QUARK_GEN_GETTER_SETTER(RSrcAddr, 19, 17);
  QUARK_GEN_GETTER_SETTER(RDstAddr, 16, 14);
};

class ActInstr : public SpuInstr {
public:
  explicit ActInstr() : SpuInstr(OpCodeType::ACT) {
    QUARK_PUSH_GETTER_SETTER(Func);
  }
  QUARK_GEN_GETTER_SETTER(Func, 25, 22);
};

class ActiInstr : public SpuInstr {
public:
  explicit ActiInstr() : SpuInstr(OpCodeType::ACTI) {
    QUARK_PUSH_GETTER_SETTER(Func);
    QUARK_PUSH_GETTER_SETTER(Imm);
  }
  QUARK_GEN_GETTER_SETTER(Func, 25, 22);
  QUARK_GEN_GETTER_SETTER(Imm, 21, 5);
};

class ActaInstr : public SpuInstr {
public:
  explicit ActaInstr() : SpuInstr(OpCodeType::ACTA) {
    QUARK_PUSH_GETTER_SETTER(Func);
  }
  QUARK_GEN_GETTER_SETTER(Func, 25, 22);
};

class SortInstr : public SpuInstr {
public:
  explicit SortInstr() : SpuInstr(OpCodeType::SORT) {
    QUARK_PUSH_GETTER_SETTER(Length);
  }
  QUARK_GEN_GETTER_SETTER(Length, 25, 10);
};

class TopkInstr : public SpuInstr {
public:
  explicit TopkInstr() : SpuInstr(OpCodeType::TOPK) {
  }
};

class CpyInstr : public SpuInstr {
public:
  explicit CpyInstr() : SpuInstr(OpCodeType::CPY) {
    QUARK_PUSH_GETTER_SETTER(RSrcAddr);
    QUARK_PUSH_GETTER_SETTER(RDstAddr);
    QUARK_PUSH_GETTER_SETTER(RLength);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Extend);
    QUARK_PUSH_GETTER_SETTER(Squeeze);
    QUARK_PUSH_GETTER_SETTER(ValidLen);
    QUARK_PUSH_GETTER_SETTER(FormatConvert);
    QUARK_PUSH_GETTER_SETTER(RConvertStride);
    QUARK_PUSH_GETTER_SETTER(NocPath);
  }
  QUARK_GEN_GETTER_SETTER(RSrcAddr, 25, 23);
  QUARK_GEN_GETTER_SETTER(RDstAddr, 22, 20);
  QUARK_GEN_GETTER_SETTER(RLength, 19, 17);
  QUARK_GEN_GETTER_SETTER(Bank, 16, 15);
  QUARK_GEN_GETTER_SETTER(Dtype, 14, 12);
  QUARK_GEN_GETTER_SETTER(Extend, 11, 11);
  QUARK_GEN_GETTER_SETTER(Squeeze, 10, 10);
  QUARK_GEN_GETTER_SETTER(ValidLen, 9, 6);
  QUARK_GEN_GETTER_SETTER(FormatConvert, 5, 5);
  QUARK_GEN_GETTER_SETTER(RConvertStride, 4, 2);
  QUARK_GEN_GETTER_SETTER(NocPath, 1, 1);
};

class CpyaInstr : public SpuInstr {
public:
  explicit CpyaInstr() : SpuInstr(OpCodeType::CPYA) {
    QUARK_PUSH_GETTER_SETTER(SurIdx);
    QUARK_PUSH_GETTER_SETTER(LineIdx);
    QUARK_PUSH_GETTER_SETTER(RLength);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Extend);
    QUARK_PUSH_GETTER_SETTER(Squeeze);
    QUARK_PUSH_GETTER_SETTER(ValidLen);
    QUARK_PUSH_GETTER_SETTER(FormatConvert);
    QUARK_PUSH_GETTER_SETTER(RConvertStride);
    QUARK_PUSH_GETTER_SETTER(NocPath);
  }
  QUARK_GEN_GETTER_SETTER(SurIdx, 25, 23);
  QUARK_GEN_GETTER_SETTER(LineIdx, 22, 20);
  QUARK_GEN_GETTER_SETTER(RLength, 19, 17);
  QUARK_GEN_GETTER_SETTER(Bank, 16, 15);
  QUARK_GEN_GETTER_SETTER(Dtype, 14, 12);
  QUARK_GEN_GETTER_SETTER(Extend, 11, 11);
  QUARK_GEN_GETTER_SETTER(Squeeze, 10, 10);
  QUARK_GEN_GETTER_SETTER(ValidLen, 9, 6);
  QUARK_GEN_GETTER_SETTER(FormatConvert, 5, 5);
  QUARK_GEN_GETTER_SETTER(RConvertStride, 4, 2);
  QUARK_GEN_GETTER_SETTER(NocPath, 1, 1);
};

class QntInstr : public SpuInstr {
public:
  explicit QntInstr() : SpuInstr(OpCodeType::QNT) {
    QUARK_PUSH_GETTER_SETTER(IDtype);
    QUARK_PUSH_GETTER_SETTER(ODtype);
    QUARK_PUSH_GETTER_SETTER(FactorAddr);
  }
  QUARK_GEN_GETTER_SETTER(IDtype, 25, 23);
  QUARK_GEN_GETTER_SETTER(ODtype, 22, 20);
  QUARK_GEN_GETTER_SETTER(FactorAddr, 19, 16);
};

class QntiInstr : public SpuInstr {
public:
  explicit QntiInstr() : SpuInstr(OpCodeType::QNTI) {
    QUARK_PUSH_GETTER_SETTER(IDtype);
    QUARK_PUSH_GETTER_SETTER(ODtype);
    QUARK_PUSH_GETTER_SETTER(Factor);
  }
  QUARK_GEN_GETTER_SETTER(IDtype, 25, 23);
  QUARK_GEN_GETTER_SETTER(ODtype, 22, 20);
  QUARK_GEN_GETTER_SETTER(Factor, 19, 16);
};

class DqtInstr : public SpuInstr {
public:
  explicit DqtInstr() : SpuInstr(OpCodeType::DQT) {
    QUARK_PUSH_GETTER_SETTER(IDtype);
    QUARK_PUSH_GETTER_SETTER(ODtype);
    QUARK_PUSH_GETTER_SETTER(FactorAddr);
  }
  QUARK_GEN_GETTER_SETTER(IDtype, 25, 23);
  QUARK_GEN_GETTER_SETTER(ODtype, 22, 20);
  QUARK_GEN_GETTER_SETTER(FactorAddr, 19, 16);
};

class DqtiInstr : public SpuInstr {
public:
  explicit DqtiInstr() : SpuInstr(OpCodeType::DQTI) {
    QUARK_PUSH_GETTER_SETTER(IDtype);
    QUARK_PUSH_GETTER_SETTER(ODtype);
    QUARK_PUSH_GETTER_SETTER(Factor);
  }
  QUARK_GEN_GETTER_SETTER(IDtype, 25, 23);
  QUARK_GEN_GETTER_SETTER(ODtype, 22, 20);
  QUARK_GEN_GETTER_SETTER(Factor, 19, 16);
};

class LpsInstr : public SpuInstr {
public:
  explicit LpsInstr() : SpuInstr(OpCodeType::LPS) {
    QUARK_PUSH_GETTER_SETTER(LoopId);
    QUARK_PUSH_GETTER_SETTER(RCount);
  }
  QUARK_GEN_GETTER_SETTER(LoopId, 25, 23);
  QUARK_GEN_GETTER_SETTER(RCount, 22, 20);
};

class LpsiInstr : public SpuInstr {
public:
  explicit LpsiInstr() : SpuInstr(OpCodeType::LPSI) {
    QUARK_PUSH_GETTER_SETTER(LoopId);
    QUARK_PUSH_GETTER_SETTER(Count);
  }
  QUARK_GEN_GETTER_SETTER(LoopId, 25, 20);
  QUARK_GEN_GETTER_SETTER(Count, 19, 10);
};

class LpeInstr : public SpuInstr {
public:
  explicit LpeInstr() : SpuInstr(OpCodeType::LPE) {
    QUARK_PUSH_GETTER_SETTER(LoopId);
  }
  QUARK_GEN_GETTER_SETTER(LoopId, 25, 23);
};

class SetInstr : public SpuInstr {
public:
  explicit SetInstr() : SpuInstr(OpCodeType::SET) {
    QUARK_PUSH_GETTER_SETTER(RDst);
    QUARK_PUSH_GETTER_SETTER(RSrc);
  }
  QUARK_GEN_GETTER_SETTER(RDst, 25, 23);
  QUARK_GEN_GETTER_SETTER(RSrc, 22, 20);
};

class SetiInstr : public SpuInstr {
public:
  explicit SetiInstr() : SpuInstr(OpCodeType::SETI) {
    QUARK_PUSH_GETTER_SETTER(RDst);
    QUARK_PUSH_GETTER_SETTER(Imm);
  }
  QUARK_GEN_GETTER_SETTER(RDst, 25, 23);
  QUARK_GEN_GETTER_SETTER(Imm, 15, 0);
};

class SetihInstr : public SpuInstr {
public:
  explicit SetihInstr() : SpuInstr(OpCodeType::SETIH) {
    QUARK_PUSH_GETTER_SETTER(RDst);
    QUARK_PUSH_GETTER_SETTER(Imm);
  }
  QUARK_GEN_GETTER_SETTER(RDst, 25, 23);
  QUARK_GEN_GETTER_SETTER(Imm, 15, 0);
};

class SetilInstr : public SpuInstr {
public:
  explicit SetilInstr() : SpuInstr(OpCodeType::SETIL) {
    QUARK_PUSH_GETTER_SETTER(RDst);
    QUARK_PUSH_GETTER_SETTER(Imm);
  }
  QUARK_GEN_GETTER_SETTER(RDst, 25, 23);
  QUARK_GEN_GETTER_SETTER(Imm, 15, 0);
};

class AaiInstr : public SpuInstr {
public:
  explicit AaiInstr() : SpuInstr(OpCodeType::AAI) {
    QUARK_PUSH_GETTER_SETTER(RAddr);
    QUARK_PUSH_GETTER_SETTER(RStride);
  }
  QUARK_GEN_GETTER_SETTER(RAddr, 25, 23);
  QUARK_GEN_GETTER_SETTER(RStride, 22, 20);
};

class EndInstr : public SpuInstr {
public:
  explicit EndInstr() : SpuInstr(OpCodeType::END) {
  }
};

}  // namespace quark

#endif // QUARK_SPU_INSTR_H
