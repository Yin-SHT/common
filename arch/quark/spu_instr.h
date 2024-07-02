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
#include <sstream>
#include "instr_interface.h"

#ifndef QUARK_GEN_GETTER_SETTER
#define QUARK_GEN_GETTER_SETTER(field, high, low)\
  uint32_t get##field() { return (fields >> low) & (((uint32_t)1<<(high-low+1))-1); }\
  void set##field(uint32_t field) {\
    uint32_t m = (((uint32_t)1<<(high-low+1))-1)<<low; fields &= ~m; fields |= (field<<low)&m; }
#endif

#ifndef QUARK_PUSH_GETTER_SETTER
#define QUARK_PUSH_GETTER_SETTER(field) do {\
  getters.push_back([this](){ return get##field(); });\
  setters.push_back([this](uint32_t x){ set##field(x); });\
} while(0)
#endif

extern void split(const std::string &text, std::vector<std::string> &v);

namespace quark {

class SpuInstr : public spu::InstrInterface {
public:
  enum class OpCodeType {
    NOP = 0,
    LDT = 1,
    LDW = 2,
    LDM = 3,
    STT = 4,
    GEMM = 5,
    TR = 6,
    ACT = 7,
    SRT = 8,
    CMP = 23,
    TPK = 9,
    PCK = 10,
    CPY = 11,
    SND = 12,
    RCV = 13,
    LPS = 14,
    LPSI = 15,
    LPE = 16,
    MOV = 17,
    MOVP = 18,
    SET = 19,
    AAI = 20,
    AAIP = 21,
    DCFG = 22,
    END = 63,
    INVALID = 255
  };

  static const std::map<OpCodeType, std::string>& getOpCodeStrMap() {
    static const std::map<OpCodeType, std::string> opCodeStrMap = {
      {OpCodeType::NOP,  "nop"},
      {OpCodeType::LDT,  "ldt"},
      {OpCodeType::LDW,  "ldw"},
      {OpCodeType::LDM,  "ldm"},
      {OpCodeType::STT,  "stt"},
      {OpCodeType::GEMM, "gemm"},
      {OpCodeType::TR,   "tr"},
      {OpCodeType::ACT,  "act"},
      {OpCodeType::SRT,  "srt"},
      {OpCodeType::CMP,  "cmp"},
      {OpCodeType::TPK,  "tpk"},
      {OpCodeType::PCK,  "pck"},
      {OpCodeType::CPY,  "cpy"},
      {OpCodeType::SND,  "snd"},
      {OpCodeType::RCV,  "rcv"},
      {OpCodeType::LPS,  "lps"},
      {OpCodeType::LPSI, "lpsi"},
      {OpCodeType::LPE,  "lpe"},
      {OpCodeType::MOV,  "mov"},
      {OpCodeType::MOVP, "movp"},
      {OpCodeType::SET,  "set"},
      {OpCodeType::AAI,  "aai"},
      {OpCodeType::AAIP, "aaip"},
      {OpCodeType::DCFG, "dcfg"},
      {OpCodeType::END,  "end"},
    };
    return opCodeStrMap;
  }

public:
  explicit SpuInstr(OpCodeType opCode) : 
      opCode(opCode), binary(4, 0) {
  }

  virtual ~SpuInstr() = default;

  static std::string opCodeToString(OpCodeType opCode)  {
    auto &opCodeStrMap = getOpCodeStrMap();
    if (opCodeStrMap.find(opCode) != opCodeStrMap.end()) {
      return opCodeStrMap.at(opCode);
    }
    return "";
  }

  static OpCodeType stringToOpCode(std::string name) {
    auto &opCodeStrMap = getOpCodeStrMap();
    for(auto &it : opCodeStrMap) {
      if (it.second == name)
        return it.first;
    }
    return OpCodeType::INVALID;
  }

  OpCodeType getOpCode() {
    return opCode;
  }

  std::shared_ptr<spu::InstrInterface> clone() override {
    auto ret = create(opCode);
    ret->fields = fields;
    return ret;
  }

  bool loadBinary(const std::vector<uint8_t>& bin) override {
    auto x = *reinterpret_cast<const uint32_t *>(bin.data());
    opCode = OpCodeType((x>>26)&0x3f);
    fields = x&(((uint32_t)1<<26)-1);
    return true;
  }

  const std::vector<uint8_t>& toBinary() override  {
    *reinterpret_cast<uint32_t *>(binary.data()) = 
        ((uint32_t)opCode<<26) | (fields&(((uint32_t)1<<26)-1));
    return binary;
  }

  bool loadText(const std::string& text) override {
    std::vector<std::string> v;
    split(text, v);

    if (v.size() != (setters.size()+1))
      return false;
    if ((opCode = stringToOpCode(v[0])) == OpCodeType::INVALID)
      return false;

    for(size_t i = 0; i < setters.size(); i++) {
      setters[i](std::strtoul(v[i+1].c_str(), 0, 10));
    }
    return true;
  }

  const std::string& toText() override {
    std::stringstream ss;
    ss << opCodeToString(opCode);
    for(size_t i = 0; i < getters.size(); i++) {
      ss << " " << getters[i]();
    }
    text = ss.str();
    return text;
  }

  template <typename T>  
  static std::shared_ptr<T> create(const std::vector<size_t> &args) {
    std::shared_ptr<T> x(new T);
    assert(x->setters.size() == args.size());
    for (size_t i = 0; i < args.size(); ++i) {
      x->setters[i](args[i]);
    }
    return x;
  }

  static inline std::shared_ptr<SpuInstr> create(OpCodeType opCode);

  static std::shared_ptr<SpuInstr> create(const std::string& text) {
    std::vector<std::string> fields;
    split(text, fields);
    if (fields.empty())
      return nullptr;

    auto opCode = stringToOpCode(fields[0]);
    auto ret = create(opCode);
    if (ret != nullptr)
      ret->loadText(text);
    return ret;
  }

  static std::shared_ptr<SpuInstr> create(const std::vector<uint8_t>& bin) {
    auto x = *reinterpret_cast<const uint32_t *>(bin.data());
    auto opCode = OpCodeType((x>>26)&0x3f);
    auto ret = create(opCode);
    if (ret != nullptr)
      ret->loadBinary(bin);
    return ret;
  }

protected:
  OpCodeType opCode;
  uint32_t fields = 0;
  std::vector<std::function<uint32_t(void)>> getters;
  std::vector<std::function<void(uint32_t)>> setters;
  std::string text;
  std::vector<uint8_t> binary;
};

class NopInstr : public SpuInstr {
public:
  explicit NopInstr() : SpuInstr(OpCodeType::NOP) {
  }
};

class LdtInstr : public SpuInstr {
public:
  explicit LdtInstr() : SpuInstr(OpCodeType::LDT) {
    QUARK_PUSH_GETTER_SETTER(RSrcAddr);
    QUARK_PUSH_GETTER_SETTER(BDst);
    QUARK_PUSH_GETTER_SETTER(RLength);
    QUARK_PUSH_GETTER_SETTER(Layout);
    QUARK_PUSH_GETTER_SETTER(Dqt);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(AddrInc);
  }
  QUARK_GEN_GETTER_SETTER(RSrcAddr, 25, 23);
  QUARK_GEN_GETTER_SETTER(BDst, 22, 20);
  QUARK_GEN_GETTER_SETTER(RLength, 19, 17);
  QUARK_GEN_GETTER_SETTER(Layout, 7, 7);
  QUARK_GEN_GETTER_SETTER(Dqt, 6, 6);
  QUARK_GEN_GETTER_SETTER(Dtype, 5, 3);
  QUARK_GEN_GETTER_SETTER(Bank, 2, 1);
  QUARK_GEN_GETTER_SETTER(AddrInc, 0, 0);
};

class LdwInstr : public SpuInstr {
public:
  explicit LdwInstr() : SpuInstr(OpCodeType::LDW) {
    QUARK_PUSH_GETTER_SETTER(RSrcAddr);
    QUARK_PUSH_GETTER_SETTER(BDst);
    QUARK_PUSH_GETTER_SETTER(RLength);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Bankc);
    QUARK_PUSH_GETTER_SETTER(Bankk);
    QUARK_PUSH_GETTER_SETTER(AddrInc);
  }
  QUARK_GEN_GETTER_SETTER(RSrcAddr, 25, 23);
  QUARK_GEN_GETTER_SETTER(BDst, 22, 20);
  QUARK_GEN_GETTER_SETTER(RLength, 19, 17);
  QUARK_GEN_GETTER_SETTER(Dtype, 7, 5);
  QUARK_GEN_GETTER_SETTER(Bankc, 4, 3);
  QUARK_GEN_GETTER_SETTER(Bankk, 2, 1);
  QUARK_GEN_GETTER_SETTER(AddrInc, 0, 0);
};

class LdmInstr : public SpuInstr {
public:
  explicit LdmInstr() : SpuInstr(OpCodeType::LDM) {
    QUARK_PUSH_GETTER_SETTER(RSrcAddr);
    QUARK_PUSH_GETTER_SETTER(BDst);
    QUARK_PUSH_GETTER_SETTER(RLength);
    QUARK_PUSH_GETTER_SETTER(GroupElement);
    QUARK_PUSH_GETTER_SETTER(UnknownSr);
    QUARK_PUSH_GETTER_SETTER(Nnz);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(AddrInc);
  }
  QUARK_GEN_GETTER_SETTER(RSrcAddr, 25, 23);
  QUARK_GEN_GETTER_SETTER(BDst, 22, 20);
  QUARK_GEN_GETTER_SETTER(RLength, 19, 17);
  QUARK_GEN_GETTER_SETTER(GroupElement, 7, 6);
  QUARK_GEN_GETTER_SETTER(UnknownSr, 5, 5);
  QUARK_GEN_GETTER_SETTER(Nnz, 4, 3);
  QUARK_GEN_GETTER_SETTER(Bank, 2, 1);
  QUARK_GEN_GETTER_SETTER(AddrInc, 0, 0);
};

class SttInstr : public SpuInstr {
public:
  explicit SttInstr() : SpuInstr(OpCodeType::STT) {
    QUARK_PUSH_GETTER_SETTER(BSrc);
    QUARK_PUSH_GETTER_SETTER(RDstAddr);
    QUARK_PUSH_GETTER_SETTER(RLength);
    QUARK_PUSH_GETTER_SETTER(Condensed);
    QUARK_PUSH_GETTER_SETTER(Qnt);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(AddrInc);
  }
  QUARK_GEN_GETTER_SETTER(BSrc, 25, 23);
  QUARK_GEN_GETTER_SETTER(RDstAddr, 22, 20);
  QUARK_GEN_GETTER_SETTER(RLength, 19, 17);
  QUARK_GEN_GETTER_SETTER(Condensed, 8, 8);
  QUARK_GEN_GETTER_SETTER(Qnt, 7, 7);
  QUARK_GEN_GETTER_SETTER(Dtype, 6, 3);
  QUARK_GEN_GETTER_SETTER(Bank, 2, 1);
  QUARK_GEN_GETTER_SETTER(AddrInc, 0, 0);
};

class GemmInstr : public SpuInstr {
public:
  explicit GemmInstr() : SpuInstr(OpCodeType::GEMM) {
    QUARK_PUSH_GETTER_SETTER(AccSwitch);
    QUARK_PUSH_GETTER_SETTER(IDtype);
    QUARK_PUSH_GETTER_SETTER(WDtype);
    QUARK_PUSH_GETTER_SETTER(Nnzc);
    QUARK_PUSH_GETTER_SETTER(Nnzk);
  }
  QUARK_GEN_GETTER_SETTER(AccSwitch, 10, 10);
  QUARK_GEN_GETTER_SETTER(IDtype, 9, 7);
  QUARK_GEN_GETTER_SETTER(WDtype, 6, 4);
  QUARK_GEN_GETTER_SETTER(Nnzc, 3, 2);
  QUARK_GEN_GETTER_SETTER(Nnzk, 1, 0);
};

class TrInstr : public SpuInstr {
public:
  explicit TrInstr() : SpuInstr(OpCodeType::TR) {
    QUARK_PUSH_GETTER_SETTER(Mode);
    QUARK_PUSH_GETTER_SETTER(Row);
    QUARK_PUSH_GETTER_SETTER(Col);
    QUARK_PUSH_GETTER_SETTER(Elen);
  }
  QUARK_GEN_GETTER_SETTER(Mode, 6, 6);
  QUARK_GEN_GETTER_SETTER(Row, 5, 4);
  QUARK_GEN_GETTER_SETTER(Col, 3, 2);
  QUARK_GEN_GETTER_SETTER(Elen, 1, 0);
};

class ActInstr : public SpuInstr {
public:
  explicit ActInstr() : SpuInstr(OpCodeType::ACT) {
    QUARK_PUSH_GETTER_SETTER(Func);
  }
  QUARK_GEN_GETTER_SETTER(Func, 3, 0);
};

class SrtInstr : public SpuInstr {
public:
  explicit SrtInstr() : SpuInstr(OpCodeType::SRT) {
    QUARK_PUSH_GETTER_SETTER(RMask);
    QUARK_PUSH_GETTER_SETTER(RInterval);
    QUARK_PUSH_GETTER_SETTER(Bitonic);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Order);
  }
  QUARK_GEN_GETTER_SETTER(RMask, 25, 25);
  QUARK_GEN_GETTER_SETTER(RInterval, 24, 24);
  QUARK_GEN_GETTER_SETTER(Bitonic, 6, 6);
  QUARK_GEN_GETTER_SETTER(Bank, 5, 4);
  QUARK_GEN_GETTER_SETTER(Dtype, 3, 1);
  QUARK_GEN_GETTER_SETTER(Order, 0, 0);
};

class CmpInstr : public SpuInstr {
public:
  explicit CmpInstr() : SpuInstr(OpCodeType::CMP) {
    QUARK_PUSH_GETTER_SETTER(RMask);
    QUARK_PUSH_GETTER_SETTER(RInterval);
    QUARK_PUSH_GETTER_SETTER(RSeg);
    QUARK_PUSH_GETTER_SETTER(Bitonic);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Mode);
  }
  QUARK_GEN_GETTER_SETTER(RMask, 25, 25);
  QUARK_GEN_GETTER_SETTER(RInterval, 24, 24);
  QUARK_GEN_GETTER_SETTER(RSeg, 23, 23);
  QUARK_GEN_GETTER_SETTER(Bitonic, 6, 6);
  QUARK_GEN_GETTER_SETTER(Bank, 5, 4);
  QUARK_GEN_GETTER_SETTER(Dtype, 3, 1);
  QUARK_GEN_GETTER_SETTER(Mode, 0, 0);
};

class TpkInstr : public SpuInstr {
public:
  explicit TpkInstr() : SpuInstr(OpCodeType::TPK) {
    QUARK_PUSH_GETTER_SETTER(RMask);
    QUARK_PUSH_GETTER_SETTER(RInterval);
    QUARK_PUSH_GETTER_SETTER(K);
    QUARK_PUSH_GETTER_SETTER(Abs);
    QUARK_PUSH_GETTER_SETTER(Res);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Mode);
  }
  QUARK_GEN_GETTER_SETTER(RMask, 25, 25);
  QUARK_GEN_GETTER_SETTER(RInterval, 24, 24);
  QUARK_GEN_GETTER_SETTER(K, 13, 8);
  QUARK_GEN_GETTER_SETTER(Abs, 7, 7);
  QUARK_GEN_GETTER_SETTER(Res, 6, 6);
  QUARK_GEN_GETTER_SETTER(Bank, 5, 4);
  QUARK_GEN_GETTER_SETTER(Dtype, 3, 1);
  QUARK_GEN_GETTER_SETTER(Mode, 0, 0);
};

class PckInstr : public SpuInstr {
public:
  explicit PckInstr() : SpuInstr(OpCodeType::PCK) {
    QUARK_PUSH_GETTER_SETTER(RMask);
    QUARK_PUSH_GETTER_SETTER(RInterval);
    QUARK_PUSH_GETTER_SETTER(RThreshold);
    QUARK_PUSH_GETTER_SETTER(RResult);
    QUARK_PUSH_GETTER_SETTER(Res);
    QUARK_PUSH_GETTER_SETTER(Bank);
    QUARK_PUSH_GETTER_SETTER(Dtype);
    QUARK_PUSH_GETTER_SETTER(Mode);
  }
  QUARK_GEN_GETTER_SETTER(RMask, 25, 25);
  QUARK_GEN_GETTER_SETTER(RInterval, 24, 24);
  QUARK_GEN_GETTER_SETTER(RThreshold, 23, 23);
  QUARK_GEN_GETTER_SETTER(RResult, 22, 22);
  QUARK_GEN_GETTER_SETTER(Res, 6, 6);
  QUARK_GEN_GETTER_SETTER(Bank, 5, 4);
  QUARK_GEN_GETTER_SETTER(Dtype, 3, 1);
  QUARK_GEN_GETTER_SETTER(Mode, 0, 0);
};

class CpyInstr : public SpuInstr {
public:
  explicit CpyInstr() : SpuInstr(OpCodeType::CPY) {
    QUARK_PUSH_GETTER_SETTER(SurIdx);
    QUARK_PUSH_GETTER_SETTER(LineIdx);
    QUARK_PUSH_GETTER_SETTER(LenIdx);
    QUARK_PUSH_GETTER_SETTER(Direction);
    QUARK_PUSH_GETTER_SETTER(SrcDynamic);
    QUARK_PUSH_GETTER_SETTER(DstDynamic);
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
  QUARK_GEN_GETTER_SETTER(LenIdx, 19, 19);
  QUARK_GEN_GETTER_SETTER(Direction, 16, 16);
  QUARK_GEN_GETTER_SETTER(SrcDynamic, 15, 15);
  QUARK_GEN_GETTER_SETTER(DstDynamic, 14, 14);
  QUARK_GEN_GETTER_SETTER(Bank, 13, 12);
  QUARK_GEN_GETTER_SETTER(Dtype, 11, 9);
  QUARK_GEN_GETTER_SETTER(Extend, 8, 8);
  QUARK_GEN_GETTER_SETTER(Squeeze, 7, 7);
  QUARK_GEN_GETTER_SETTER(ValidLen, 6, 3);
  QUARK_GEN_GETTER_SETTER(FormatConvert, 2, 2);
  QUARK_GEN_GETTER_SETTER(RConvertStride, 1, 1);
  QUARK_GEN_GETTER_SETTER(NocPath, 0, 0);
};

class SndInstr : public SpuInstr {
public:
  explicit SndInstr() : SpuInstr(OpCodeType::SND) {
    QUARK_PUSH_GETTER_SETTER(SurIdx);
    QUARK_PUSH_GETTER_SETTER(LineIdx);
    QUARK_PUSH_GETTER_SETTER(LenIdx);
    QUARK_PUSH_GETTER_SETTER(PeId);
  }
  QUARK_GEN_GETTER_SETTER(SurIdx, 25, 23);
  QUARK_GEN_GETTER_SETTER(LineIdx, 22, 20);
  QUARK_GEN_GETTER_SETTER(LenIdx, 19, 19);
  QUARK_GEN_GETTER_SETTER(PeId, 6, 0);
};

class RcvInstr : public SpuInstr {
public:
  explicit RcvInstr() : SpuInstr(OpCodeType::RCV) {
    QUARK_PUSH_GETTER_SETTER(PeId);
  }
  QUARK_GEN_GETTER_SETTER(PeId, 6, 0);
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
  QUARK_GEN_GETTER_SETTER(LoopId, 25, 23);
  QUARK_GEN_GETTER_SETTER(Count, 22, 7);
};

class LpeInstr : public SpuInstr {
public:
  explicit LpeInstr() : SpuInstr(OpCodeType::LPE) {
    QUARK_PUSH_GETTER_SETTER(LoopId);
  }
  QUARK_GEN_GETTER_SETTER(LoopId, 25, 23);
};

class MovInstr : public SpuInstr {
public:
  explicit MovInstr() : SpuInstr(OpCodeType::MOV) {
    QUARK_PUSH_GETTER_SETTER(RDst);
    QUARK_PUSH_GETTER_SETTER(RSrc);
  }
  QUARK_GEN_GETTER_SETTER(RDst, 25, 18);
  QUARK_GEN_GETTER_SETTER(RSrc, 17, 10);
};

class MovpInstr : public SpuInstr {
public:
  explicit MovpInstr() : SpuInstr(OpCodeType::MOVP) {
    QUARK_PUSH_GETTER_SETTER(SrcId);
    QUARK_PUSH_GETTER_SETTER(DstId);
    QUARK_PUSH_GETTER_SETTER(AddrMovMask);
  }
  QUARK_GEN_GETTER_SETTER(SrcId, 25, 23);
  QUARK_GEN_GETTER_SETTER(DstId, 22, 20);
  QUARK_GEN_GETTER_SETTER(AddrMovMask, 19, 12);
};

class SetInstr : public SpuInstr {
public:
  explicit SetInstr() : SpuInstr(OpCodeType::SET) {
    QUARK_PUSH_GETTER_SETTER(RDst);
    QUARK_PUSH_GETTER_SETTER(Offset);
    QUARK_PUSH_GETTER_SETTER(Imm);
  }
  QUARK_GEN_GETTER_SETTER(RDst, 25, 18);
  QUARK_GEN_GETTER_SETTER(Offset, 17, 16);
  QUARK_GEN_GETTER_SETTER(Imm, 15, 0);
};

class AaiInstr : public SpuInstr {
public:
  explicit AaiInstr() : SpuInstr(OpCodeType::AAI) {
    QUARK_PUSH_GETTER_SETTER(RAddr);
    QUARK_PUSH_GETTER_SETTER(RStride);
  }
  QUARK_GEN_GETTER_SETTER(RAddr, 25, 18);
  QUARK_GEN_GETTER_SETTER(RStride, 17, 10);
};

class AaipInstr : public SpuInstr {
public:
  explicit AaipInstr() : SpuInstr(OpCodeType::AAIP) {
    QUARK_PUSH_GETTER_SETTER(LoopId);
    QUARK_PUSH_GETTER_SETTER(AddrIncMask);
  }
  QUARK_GEN_GETTER_SETTER(LoopId, 25, 23);
  QUARK_GEN_GETTER_SETTER(AddrIncMask, 22, 15);
};

class DcfgInstr : public SpuInstr {
public:
  explicit DcfgInstr() : SpuInstr(OpCodeType::DCFG) {
  }
};

class EndInstr : public SpuInstr {
public:
  explicit EndInstr() : SpuInstr(OpCodeType::END) {
    QUARK_PUSH_GETTER_SETTER(AddressAlign);
  }
  QUARK_GEN_GETTER_SETTER(AddressAlign, 0, 0);
};


std::shared_ptr<SpuInstr> SpuInstr::create(OpCodeType opCode) {
  switch (opCode) {
    case OpCodeType::NOP:  return std::shared_ptr<NopInstr>(new NopInstr);
    case OpCodeType::LDT:  return std::shared_ptr<LdtInstr>(new LdtInstr);
    case OpCodeType::LDW:  return std::shared_ptr<LdwInstr>(new LdwInstr);
    case OpCodeType::LDM:  return std::shared_ptr<LdmInstr>(new LdmInstr);
    case OpCodeType::STT:  return std::shared_ptr<SttInstr>(new SttInstr);
    case OpCodeType::GEMM: return std::shared_ptr<GemmInstr>(new GemmInstr);
    case OpCodeType::TR:   return std::shared_ptr<TrInstr>(new TrInstr);
    case OpCodeType::ACT:  return std::shared_ptr<ActInstr>(new ActInstr);
    case OpCodeType::SRT:  return std::shared_ptr<SrtInstr>(new SrtInstr);
    case OpCodeType::CMP:  return std::shared_ptr<CmpInstr>(new CmpInstr);
    case OpCodeType::TPK:  return std::shared_ptr<TpkInstr>(new TpkInstr);
    case OpCodeType::PCK:  return std::shared_ptr<PckInstr>(new PckInstr);
    case OpCodeType::CPY:  return std::shared_ptr<CpyInstr>(new CpyInstr);
    case OpCodeType::SND:  return std::shared_ptr<SndInstr>(new SndInstr);
    case OpCodeType::RCV:  return std::shared_ptr<RcvInstr>(new RcvInstr);
    case OpCodeType::LPS:  return std::shared_ptr<LpsInstr>(new LpsInstr);
    case OpCodeType::LPSI: return std::shared_ptr<LpsiInstr>(new LpsiInstr);
    case OpCodeType::LPE:  return std::shared_ptr<LpeInstr>(new LpeInstr);
    case OpCodeType::MOV:  return std::shared_ptr<MovInstr>(new MovInstr);
    case OpCodeType::MOVP: return std::shared_ptr<MovpInstr>(new MovpInstr);
    case OpCodeType::SET:  return std::shared_ptr<SetInstr>(new SetInstr);
    case OpCodeType::AAI:  return std::shared_ptr<AaiInstr>(new AaiInstr);
    case OpCodeType::AAIP: return std::shared_ptr<AaipInstr>(new AaipInstr);
    case OpCodeType::DCFG: return std::shared_ptr<DcfgInstr>(new DcfgInstr);
    case OpCodeType::END:  return std::shared_ptr<EndInstr>(new EndInstr);
    default: return nullptr;
  }
}

} // namespace quark

#endif // QUARK_SPU_INSTR_H
