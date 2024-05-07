/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */
#ifndef QUARK_PELS_INSTR_H
#define QUARK_PELS_INSTR_H

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
namespace pels {

class PelsInstr : public spu::InstrInterface {
public:
  enum class OpCodeType {
    SET = 19,
    TMS = 55,
    TME = 56,
    ENB = 57,
    WFI = 58,
    LCK = 59,
    ULCK = 60,
    CALL = 61,
    STR = 62,
    END = 63,
    INVALID = 255
  };
  enum class EnableType {
    Engine = 0,
    RVV = 1,
    Instr = 2,
    TCM = 3,
  };
  enum class InterruptType {
    Engine = 0,
    RVV = 1,
    Instr = 2,
    TCM = 3,
    Unlock = 4,
    RVV_Com = 5,
  };

  static const std::map<OpCodeType, std::string>& getOpCodeStrMap() {
    static const std::map<OpCodeType, std::string> opCodeStrMap = {
      {OpCodeType::SET,  "set"},
      {OpCodeType::TMS,  "tms"},
      {OpCodeType::TME,  "tme"},
      {OpCodeType::ENB,  "enb"},
      {OpCodeType::WFI,  "wfi"},
      {OpCodeType::LCK,  "lck"},
      {OpCodeType::ULCK, "ulck"},
      {OpCodeType::CALL, "call"},
      {OpCodeType::STR,  "str"},
      {OpCodeType::END,  "end"},
    };
    return opCodeStrMap;
  }

public:
  explicit PelsInstr(OpCodeType opCode) : 
      opCode(opCode), binary(4, 0) {
  }

  virtual ~PelsInstr() = default;

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

  static inline std::shared_ptr<PelsInstr> create(OpCodeType opCode);

  static std::shared_ptr<PelsInstr> create(const std::string& text) {
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

  static std::shared_ptr<PelsInstr> create(const std::vector<uint8_t>& bin) {
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

class SetInstr : public PelsInstr {
public:
  explicit SetInstr() : PelsInstr(OpCodeType::SET) {
    QUARK_PUSH_GETTER_SETTER(RDst);
    QUARK_PUSH_GETTER_SETTER(Offset);
    QUARK_PUSH_GETTER_SETTER(Imm);
  }
  QUARK_GEN_GETTER_SETTER(RDst, 25, 18);
  QUARK_GEN_GETTER_SETTER(Offset, 17, 16);
  QUARK_GEN_GETTER_SETTER(Imm, 15, 0);
};

class TmsInstr : public PelsInstr {
public:
  explicit TmsInstr() : PelsInstr(OpCodeType::TMS) {
    QUARK_PUSH_GETTER_SETTER(EventId);
  }
  QUARK_GEN_GETTER_SETTER(EventId, 3, 0);
};

class TmeInstr : public PelsInstr {
public:
  explicit TmeInstr() : PelsInstr(OpCodeType::TME) {
    QUARK_PUSH_GETTER_SETTER(EventId);
  }
  QUARK_GEN_GETTER_SETTER(EventId, 3, 0);
};

class EnbInstr : public PelsInstr {
public:
  explicit EnbInstr() : PelsInstr(OpCodeType::ENB) {
    QUARK_PUSH_GETTER_SETTER(EnType);
    QUARK_PUSH_GETTER_SETTER(Dma5);
    QUARK_PUSH_GETTER_SETTER(Dma4);
    QUARK_PUSH_GETTER_SETTER(Dma3);
    QUARK_PUSH_GETTER_SETTER(Dma2);
    QUARK_PUSH_GETTER_SETTER(Dma1);
    QUARK_PUSH_GETTER_SETTER(Dma0);
    QUARK_PUSH_GETTER_SETTER(Ae);
    QUARK_PUSH_GETTER_SETTER(Se);
    QUARK_PUSH_GETTER_SETTER(Te);
    QUARK_PUSH_GETTER_SETTER(Vpu);
    QUARK_PUSH_GETTER_SETTER(Spu);
  }
  QUARK_GEN_GETTER_SETTER(EnType, 25, 23);
  QUARK_GEN_GETTER_SETTER(Dma5, 10, 10);
  QUARK_GEN_GETTER_SETTER(Dma4, 9, 9);
  QUARK_GEN_GETTER_SETTER(Dma3, 8, 8);
  QUARK_GEN_GETTER_SETTER(Dma2, 7, 7);
  QUARK_GEN_GETTER_SETTER(Dma1, 6, 6);
  QUARK_GEN_GETTER_SETTER(Dma0, 5, 5);
  QUARK_GEN_GETTER_SETTER(Ae, 4, 4);
  QUARK_GEN_GETTER_SETTER(Se, 3, 3);
  QUARK_GEN_GETTER_SETTER(Te, 2, 2);
  QUARK_GEN_GETTER_SETTER(Vpu, 1, 1);
  QUARK_GEN_GETTER_SETTER(Spu, 0, 0);
};

class WfiInstr : public PelsInstr {
public:
  explicit WfiInstr() : PelsInstr(OpCodeType::WFI) {
    QUARK_PUSH_GETTER_SETTER(IrType);
    QUARK_PUSH_GETTER_SETTER(Dma5);
    QUARK_PUSH_GETTER_SETTER(Dma4);
    QUARK_PUSH_GETTER_SETTER(Dma3);
    QUARK_PUSH_GETTER_SETTER(Dma2);
    QUARK_PUSH_GETTER_SETTER(Dma1);
    QUARK_PUSH_GETTER_SETTER(Dma0);
    QUARK_PUSH_GETTER_SETTER(Ae);
    QUARK_PUSH_GETTER_SETTER(Se);
    QUARK_PUSH_GETTER_SETTER(Te);
    QUARK_PUSH_GETTER_SETTER(Vpu);
    QUARK_PUSH_GETTER_SETTER(Spu);
  }
  QUARK_GEN_GETTER_SETTER(IrType, 25, 23);
  QUARK_GEN_GETTER_SETTER(OutBuf, 1, 1);
  QUARK_GEN_GETTER_SETTER(InBuf, 0, 0);
  QUARK_GEN_GETTER_SETTER(Rvv3, 3, 3);
  QUARK_GEN_GETTER_SETTER(Rvv2, 2, 2);
  QUARK_GEN_GETTER_SETTER(Rvv1, 1, 1);
  QUARK_GEN_GETTER_SETTER(Rvv0, 0, 0);
  QUARK_GEN_GETTER_SETTER(Dma5, 10, 10);
  QUARK_GEN_GETTER_SETTER(Dma4, 9, 9);
  QUARK_GEN_GETTER_SETTER(Dma3, 8, 8);
  QUARK_GEN_GETTER_SETTER(Dma2, 7, 7);
  QUARK_GEN_GETTER_SETTER(Dma1, 6, 6);
  QUARK_GEN_GETTER_SETTER(Dma0, 5, 5);
  QUARK_GEN_GETTER_SETTER(Ae, 4, 4);
  QUARK_GEN_GETTER_SETTER(Se, 3, 3);
  QUARK_GEN_GETTER_SETTER(Te, 2, 2);
  QUARK_GEN_GETTER_SETTER(Vpu, 1, 1);
  QUARK_GEN_GETTER_SETTER(Spu, 0, 0);
};

class LckInstr : public PelsInstr {
public:
  explicit LckInstr() : PelsInstr(OpCodeType::LCK) {
    QUARK_PUSH_GETTER_SETTER(Region);
  }
  QUARK_GEN_GETTER_SETTER(Region, 0, 0);
};

class UlckInstr : public PelsInstr {
public:
  explicit UlckInstr() : PelsInstr(OpCodeType::ULCK) {
    QUARK_PUSH_GETTER_SETTER(Region);
  }
  QUARK_GEN_GETTER_SETTER(Region, 0, 0);
};

class CallInstr : public PelsInstr {
public:
  explicit CallInstr() : PelsInstr(OpCodeType::CALL) {
    QUARK_PUSH_GETTER_SETTER(Com);
    QUARK_PUSH_GETTER_SETTER(Func);
  }
  QUARK_GEN_GETTER_SETTER(Com, 25, 24);
  QUARK_GEN_GETTER_SETTER(Func, 15, 0);
};

class StrInstr : public PelsInstr {
public:
  explicit StrInstr() : PelsInstr(OpCodeType::STR) {
  }
};

class EndInstr : public PelsInstr {
public:
  explicit EndInstr() : PelsInstr(OpCodeType::END) {
  }
};


std::shared_ptr<PelsInstr> PelsInstr::create(OpCodeType opCode) {
  switch (opCode) {
    case OpCodeType::SET:  return std::shared_ptr<SetInstr>(new SetInstr);
    case OpCodeType::TMS:  return std::shared_ptr<TmsInstr>(new TmsInstr);
    case OpCodeType::TME:  return std::shared_ptr<TmeInstr>(new TmeInstr);
    case OpCodeType::ENB:  return std::shared_ptr<EnbInstr>(new EnbInstr);
    case OpCodeType::WFI:  return std::shared_ptr<WfiInstr>(new WfiInstr);
    case OpCodeType::LCK:  return std::shared_ptr<LckInstr>(new LckInstr);
    case OpCodeType::ULCK: return std::shared_ptr<UlckInstr>(new UlckInstr);
    case OpCodeType::CALL: return std::shared_ptr<CallInstr>(new CallInstr);
    case OpCodeType::STR:  return std::shared_ptr<StrInstr>(new StrInstr);
    case OpCodeType::END:  return std::shared_ptr<EndInstr>(new EndInstr);
    default: return nullptr;
  }
}

} // namespace pels
} // namespace quark

#endif // QUARK_PELS_INSTR_H
