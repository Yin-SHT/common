/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */
#ifndef ANTOUM_ANTOUM_INSTR_H
#define ANTOUM_ANTOUM_INSTR_H

#include <stdint.h>
#include <assert.h>
#include <string>
#include <cstring>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <sstream>
#include "instr_interface.h"

#ifndef ANTOUM_GEN_GETTER_SETTER
#define ANTOUM_GEN_GETTER_SETTER(field, descs)\
  uint64_t get##field() { return getField(descs); }\
  void set##field(uint64_t field) { setField(descs, field); }
#endif

#ifndef ANTOUM_PUSH_GETTER_SETTER
#define ANTOUM_PUSH_GETTER_SETTER(field) do {\
  getters.push_back([this](){ return get##field(); });\
  setters.push_back([this](uint64_t x){ set##field(x); });\
} while(0)
#endif

extern void split(const std::string &text, std::vector<std::string> &v);

namespace antoum {

class AntoumInstr : public spu::InstrInterface {
public:
  enum class OpCodeType {
    CONV2D = 2,
    ACTIVATE = 3,
    TOPK = 4,
    TRANS = 5,
    GLBCPY = 6,
    DMAMV = 7,
    INVALID = 255
  };

  struct FieldDesc {
    uint64_t qindex;
    uint64_t high;
    uint64_t low;
  };

  static const std::map<OpCodeType, std::string>& getOpCodeStrMap() {
    static const std::map<OpCodeType, std::string> opCodeStrMap = {
      {OpCodeType::CONV2D,   "conv2d"},
      {OpCodeType::ACTIVATE, "activate"},
      {OpCodeType::TOPK,     "topk"},
      {OpCodeType::TRANS,    "trans"},
      {OpCodeType::GLBCPY,   "glbcpy"},
      {OpCodeType::DMAMV,    "dmamv"},
    };
    return opCodeStrMap;
  }

public:
  explicit AntoumInstr(OpCodeType opCode, size_t qsize) : 
      opCode(opCode), fields(qsize, 0), binary(qsize*8, 0) {
  }

  virtual ~AntoumInstr() = default;

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
    opCode = OpCodeType(bin[7]);
    memcpy(fields.data(), bin.data(), bin.size());
    return true;
  }

  const std::vector<uint8_t>& toBinary() override  {
    memcpy(binary.data(), fields.data(), binary.size());
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

  static inline std::shared_ptr<AntoumInstr> create(OpCodeType opCode);

  static std::shared_ptr<AntoumInstr> create(const std::string& text) {
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

  static std::shared_ptr<AntoumInstr> create(const std::vector<uint8_t>& bin) {
    auto opCode = OpCodeType(bin[7]);
    auto ret = create(opCode);
    if (ret != nullptr)
      ret->loadBinary(bin);
    return ret;
  }

protected:
  uint64_t getField(const std::vector<FieldDesc>& descs) {
    uint64_t field = 0;
    for (auto &it : descs) {
      uint64_t width = it.high-it.low+1;
      field <<= width;
      field |= (fields[it.qindex] >> it.low) & (((uint64_t)1<<width)-1);
    }
    return field;
  }

  void setField(const std::vector<FieldDesc>& descs, uint64_t field) {
    for (auto &it : descs) {
      uint64_t width = it.high-it.low+1;
      uint64_t m = (((uint64_t)1<<width)-1)<<it.low;
      fields[it.qindex] &= ~m; 
      fields[it.qindex] |= (field<<it.low)&m;
      field >>= width;
    }
  }

protected:
  OpCodeType opCode;
  std::vector<uint64_t> fields;
  std::vector<std::function<uint64_t(void)>> getters;
  std::vector<std::function<void(uint64_t)>> setters;
  std::string text;
  std::vector<uint8_t> binary;
};

class Conv2dInstr : public AntoumInstr {
public:
  explicit Conv2dInstr() : AntoumInstr(OpCodeType::CONV2D, 6) {
    ANTOUM_PUSH_GETTER_SETTER(SrcStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineStride);
    ANTOUM_PUSH_GETTER_SETTER(SrcSurfaceNum);
    ANTOUM_PUSH_GETTER_SETTER(SrcSurfaceStride);
    ANTOUM_PUSH_GETTER_SETTER(WeightBufStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(WeightLineLength);
    ANTOUM_PUSH_GETTER_SETTER(WeightLineNum);
    ANTOUM_PUSH_GETTER_SETTER(WeightLineStride);
    ANTOUM_PUSH_GETTER_SETTER(WeightSufNum);
    ANTOUM_PUSH_GETTER_SETTER(WeightSufStride);
    ANTOUM_PUSH_GETTER_SETTER(DstStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(DstLineLength);
    ANTOUM_PUSH_GETTER_SETTER(DstLineStride);
    ANTOUM_PUSH_GETTER_SETTER(DstSurfaceNum);
    ANTOUM_PUSH_GETTER_SETTER(DstSurfaceStride);
    ANTOUM_PUSH_GETTER_SETTER(DstMode);
    ANTOUM_PUSH_GETTER_SETTER(Rq);
    ANTOUM_PUSH_GETTER_SETTER(Rr);
    ANTOUM_PUSH_GETTER_SETTER(Sq);
    ANTOUM_PUSH_GETTER_SETTER(Sr);
    ANTOUM_PUSH_GETTER_SETTER(Cgb);
    ANTOUM_PUSH_GETTER_SETTER(CgbWr);
    ANTOUM_PUSH_GETTER_SETTER(Kq);
    ANTOUM_PUSH_GETTER_SETTER(Kr);
    ANTOUM_PUSH_GETTER_SETTER(KrBcast);
    ANTOUM_PUSH_GETTER_SETTER(KgPt);
    ANTOUM_PUSH_GETTER_SETTER(Cq);
    ANTOUM_PUSH_GETTER_SETTER(Cr);
    ANTOUM_PUSH_GETTER_SETTER(NnzCgbCq);
    ANTOUM_PUSH_GETTER_SETTER(NnzCgbCr);
    ANTOUM_PUSH_GETTER_SETTER(FilterStride);
    ANTOUM_PUSH_GETTER_SETTER(PadTopMargin);
    ANTOUM_PUSH_GETTER_SETTER(PadBottomMargin);
    ANTOUM_PUSH_GETTER_SETTER(PadLeftMargin);
    ANTOUM_PUSH_GETTER_SETTER(PadRightMargin);
    ANTOUM_PUSH_GETTER_SETTER(ZeroPadTopEn);
    ANTOUM_PUSH_GETTER_SETTER(ZeroPadBottomEn);
    ANTOUM_PUSH_GETTER_SETTER(ZeroPadLeftEn);
    ANTOUM_PUSH_GETTER_SETTER(ZeroPadRightEn);
    ANTOUM_PUSH_GETTER_SETTER(Width);
    ANTOUM_PUSH_GETTER_SETTER(Height);
    ANTOUM_PUSH_GETTER_SETTER(PrecisionMode);
    ANTOUM_PUSH_GETTER_SETTER(BiasAddEn);
    ANTOUM_PUSH_GETTER_SETTER(ActivationEn);
    ANTOUM_PUSH_GETTER_SETTER(ElementOpEn);
    ANTOUM_PUSH_GETTER_SETTER(ActivationFunc);
    ANTOUM_PUSH_GETTER_SETTER(ActivationTableStart);
    ANTOUM_PUSH_GETTER_SETTER(BiasStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(QuantScaleStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(DequantScaleStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(ElementOpSrcStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(UsePartialSumEn);
    ANTOUM_PUSH_GETTER_SETTER(GenFinalSumEn);
    ANTOUM_PUSH_GETTER_SETTER(DstValueWrValid);
    ANTOUM_PUSH_GETTER_SETTER(WeightDenseEnable);
    ANTOUM_PUSH_GETTER_SETTER(OpEnd);
  }
  ANTOUM_GEN_GETTER_SETTER(SrcStartAddr, std::vector<FieldDesc>({{0,55,36}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineStride, std::vector<FieldDesc>({{1,43,29}}));
  ANTOUM_GEN_GETTER_SETTER(SrcSurfaceNum, std::vector<FieldDesc>({{1,28,22}}));
  ANTOUM_GEN_GETTER_SETTER(SrcSurfaceStride, std::vector<FieldDesc>({{1,21,3}}));
  ANTOUM_GEN_GETTER_SETTER(WeightBufStartAddr, std::vector<FieldDesc>({{3,48,34}}));
  ANTOUM_GEN_GETTER_SETTER(WeightLineLength, std::vector<FieldDesc>({{3,33,24}}));
  ANTOUM_GEN_GETTER_SETTER(WeightLineNum, std::vector<FieldDesc>({{3,23,19}}));
  ANTOUM_GEN_GETTER_SETTER(WeightLineStride, std::vector<FieldDesc>({{3,18,16}}));
  ANTOUM_GEN_GETTER_SETTER(WeightSufNum, std::vector<FieldDesc>({{3,15,9}}));
  ANTOUM_GEN_GETTER_SETTER(WeightSufStride, std::vector<FieldDesc>({{3,8,4}}));
  ANTOUM_GEN_GETTER_SETTER(DstStartAddr, std::vector<FieldDesc>({{0,35,16}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineLength, std::vector<FieldDesc>({{5,63,52}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineStride, std::vector<FieldDesc>({{5,44,30}}));
  ANTOUM_GEN_GETTER_SETTER(DstSurfaceNum, std::vector<FieldDesc>({{5,29,23}}));
  ANTOUM_GEN_GETTER_SETTER(DstSurfaceStride, std::vector<FieldDesc>({{5,22,4}}));
  ANTOUM_GEN_GETTER_SETTER(DstMode, std::vector<FieldDesc>({{0,13,12}}));
  ANTOUM_GEN_GETTER_SETTER(Rq, std::vector<FieldDesc>({{2,57,56}}));
  ANTOUM_GEN_GETTER_SETTER(Rr, std::vector<FieldDesc>({{2,55,54}}));
  ANTOUM_GEN_GETTER_SETTER(Sq, std::vector<FieldDesc>({{2,53,52}}));
  ANTOUM_GEN_GETTER_SETTER(Sr, std::vector<FieldDesc>({{2,51,50}}));
  ANTOUM_GEN_GETTER_SETTER(Cgb, std::vector<FieldDesc>({{2,63,61}}));
  ANTOUM_GEN_GETTER_SETTER(CgbWr, std::vector<FieldDesc>({{2,60,58}}));
  ANTOUM_GEN_GETTER_SETTER(Kq, std::vector<FieldDesc>({{2,49,42}}));
  ANTOUM_GEN_GETTER_SETTER(Kr, std::vector<FieldDesc>({{2,38,33}}));
  ANTOUM_GEN_GETTER_SETTER(KrBcast, std::vector<FieldDesc>({{2,39,39}}));
  ANTOUM_GEN_GETTER_SETTER(KgPt, std::vector<FieldDesc>({{2,32,30}}));
  ANTOUM_GEN_GETTER_SETTER(Cq, std::vector<FieldDesc>({{2,29,22}}));
  ANTOUM_GEN_GETTER_SETTER(Cr, std::vector<FieldDesc>({{2,18,16}}));
  ANTOUM_GEN_GETTER_SETTER(NnzCgbCq, std::vector<FieldDesc>({{4,24,19}}));
  ANTOUM_GEN_GETTER_SETTER(NnzCgbCr, std::vector<FieldDesc>({{4,18,13}}));
  ANTOUM_GEN_GETTER_SETTER(FilterStride, std::vector<FieldDesc>({{0,14,14}}));
  ANTOUM_GEN_GETTER_SETTER(PadTopMargin, std::vector<FieldDesc>({{4,12,10}}));
  ANTOUM_GEN_GETTER_SETTER(PadBottomMargin, std::vector<FieldDesc>({{4,9,7}}));
  ANTOUM_GEN_GETTER_SETTER(PadLeftMargin, std::vector<FieldDesc>({{4,6,4}}));
  ANTOUM_GEN_GETTER_SETTER(PadRightMargin, std::vector<FieldDesc>({{4,3,1}}));
  ANTOUM_GEN_GETTER_SETTER(ZeroPadTopEn, std::vector<FieldDesc>({{0,11,11}}));
  ANTOUM_GEN_GETTER_SETTER(ZeroPadBottomEn, std::vector<FieldDesc>({{0,10,10}}));
  ANTOUM_GEN_GETTER_SETTER(ZeroPadLeftEn, std::vector<FieldDesc>({{0,9,9}}));
  ANTOUM_GEN_GETTER_SETTER(ZeroPadRightEn, std::vector<FieldDesc>({{0,8,8}}));
  ANTOUM_GEN_GETTER_SETTER(Width, std::vector<FieldDesc>({{4,36,25}}));
  ANTOUM_GEN_GETTER_SETTER(Height, std::vector<FieldDesc>({{4,48,37}}));
  ANTOUM_GEN_GETTER_SETTER(PrecisionMode, std::vector<FieldDesc>({{0,5,5}}));
  ANTOUM_GEN_GETTER_SETTER(BiasAddEn, std::vector<FieldDesc>({{0,4,4}}));
  ANTOUM_GEN_GETTER_SETTER(ActivationEn, std::vector<FieldDesc>({{0,3,3}}));
  ANTOUM_GEN_GETTER_SETTER(ElementOpEn, std::vector<FieldDesc>({{0,2,2}}));
  ANTOUM_GEN_GETTER_SETTER(ActivationFunc, std::vector<FieldDesc>({{0,1,1}}));
  ANTOUM_GEN_GETTER_SETTER(ActivationTableStart, std::vector<FieldDesc>({{3,3,3}}));
  ANTOUM_GEN_GETTER_SETTER(BiasStartAddr, std::vector<FieldDesc>({{4,63,49}}));
  ANTOUM_GEN_GETTER_SETTER(QuantScaleStartAddr, std::vector<FieldDesc>({{2,15,1}}));
  ANTOUM_GEN_GETTER_SETTER(DequantScaleStartAddr, std::vector<FieldDesc>({{3,63,49}}));
  ANTOUM_GEN_GETTER_SETTER(ElementOpSrcStartAddr, std::vector<FieldDesc>({{1,63,44}}));
  ANTOUM_GEN_GETTER_SETTER(UsePartialSumEn, std::vector<FieldDesc>({{0,6,6}}));
  ANTOUM_GEN_GETTER_SETTER(GenFinalSumEn, std::vector<FieldDesc>({{0,7,7}}));
  ANTOUM_GEN_GETTER_SETTER(DstValueWrValid, std::vector<FieldDesc>({{5,3,0}}));
  ANTOUM_GEN_GETTER_SETTER(WeightDenseEnable, std::vector<FieldDesc>({{0,15,15}}));
  ANTOUM_GEN_GETTER_SETTER(OpEnd, std::vector<FieldDesc>({{0,0,0}}));
};

class ActivateInstr : public AntoumInstr {
public:
  explicit ActivateInstr() : AntoumInstr(OpCodeType::ACTIVATE, 4) {
    ANTOUM_PUSH_GETTER_SETTER(SrcStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineLength);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineNum);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineStride);
    ANTOUM_PUSH_GETTER_SETTER(SrcSurfaceNum);
    ANTOUM_PUSH_GETTER_SETTER(SrcSurfaceStride);
    ANTOUM_PUSH_GETTER_SETTER(DstStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(DstLineLength);
    ANTOUM_PUSH_GETTER_SETTER(DstLineNum);
    ANTOUM_PUSH_GETTER_SETTER(DstLineStride);
    ANTOUM_PUSH_GETTER_SETTER(DstSurfaceNum);
    ANTOUM_PUSH_GETTER_SETTER(DstSurfaceStride);
    ANTOUM_PUSH_GETTER_SETTER(PValue);
    ANTOUM_PUSH_GETTER_SETTER(ActivationFunc);
    ANTOUM_PUSH_GETTER_SETTER(OpEnd);
  }
  ANTOUM_GEN_GETTER_SETTER(SrcStartAddr, std::vector<FieldDesc>({{0,55,36}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineLength, std::vector<FieldDesc>({{1,63,52}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineNum, std::vector<FieldDesc>({{1,51,41}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineStride, std::vector<FieldDesc>({{1,40,26}}));
  ANTOUM_GEN_GETTER_SETTER(SrcSurfaceNum, std::vector<FieldDesc>({{1,25,19}}));
  ANTOUM_GEN_GETTER_SETTER(SrcSurfaceStride, std::vector<FieldDesc>({{1,18,0}}));
  ANTOUM_GEN_GETTER_SETTER(DstStartAddr, std::vector<FieldDesc>({{0,35,16}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineLength, std::vector<FieldDesc>({{2,63,52}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineNum, std::vector<FieldDesc>({{2,51,41}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineStride, std::vector<FieldDesc>({{2,40,26}}));
  ANTOUM_GEN_GETTER_SETTER(DstSurfaceNum, std::vector<FieldDesc>({{2,25,19}}));
  ANTOUM_GEN_GETTER_SETTER(DstSurfaceStride, std::vector<FieldDesc>({{2,18,0}}));
  ANTOUM_GEN_GETTER_SETTER(PValue, std::vector<FieldDesc>({{3,63,48}}));
  ANTOUM_GEN_GETTER_SETTER(ActivationFunc, std::vector<FieldDesc>({{0,7,4}}));
  ANTOUM_GEN_GETTER_SETTER(OpEnd, std::vector<FieldDesc>({{0,0,0}}));
};

class TopkInstr : public AntoumInstr {
public:
  explicit TopkInstr() : AntoumInstr(OpCodeType::TOPK, 2) {
    ANTOUM_PUSH_GETTER_SETTER(SrcValueStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(SrcIndexStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(DstValueStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(DstIndexStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(N);
    ANTOUM_PUSH_GETTER_SETTER(K);
    ANTOUM_PUSH_GETTER_SETTER(ReadMaskRptCycle);
    ANTOUM_PUSH_GETTER_SETTER(FuncMode);
    ANTOUM_PUSH_GETTER_SETTER(PrecisionMode);
    ANTOUM_PUSH_GETTER_SETTER(OpEnd);
  }
  ANTOUM_GEN_GETTER_SETTER(SrcValueStartAddr, std::vector<FieldDesc>({{1,63,44}}));
  ANTOUM_GEN_GETTER_SETTER(SrcIndexStartAddr, std::vector<FieldDesc>({{0,55,36}}));
  ANTOUM_GEN_GETTER_SETTER(DstValueStartAddr, std::vector<FieldDesc>({{1,23,4}}));
  ANTOUM_GEN_GETTER_SETTER(DstIndexStartAddr, std::vector<FieldDesc>({{1,43,24}}));
  ANTOUM_GEN_GETTER_SETTER(N, std::vector<FieldDesc>({{0,35,21}}));
  ANTOUM_GEN_GETTER_SETTER(K, std::vector<FieldDesc>({{0,20,10}}));
  ANTOUM_GEN_GETTER_SETTER(ReadMaskRptCycle, std::vector<FieldDesc>({{0,9,3}}));
  ANTOUM_GEN_GETTER_SETTER(FuncMode, std::vector<FieldDesc>({{0,2,2}}));
  ANTOUM_GEN_GETTER_SETTER(PrecisionMode, std::vector<FieldDesc>({{0,1,1}}));
  ANTOUM_GEN_GETTER_SETTER(OpEnd, std::vector<FieldDesc>({{0,0,0}}));
};

class TransInstr : public AntoumInstr {
public:
  explicit TransInstr() : AntoumInstr(OpCodeType::TRANS, 1) {
    ANTOUM_PUSH_GETTER_SETTER(SrcStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(DstStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(Mode);
    ANTOUM_PUSH_GETTER_SETTER(ConfigIdx);
    ANTOUM_PUSH_GETTER_SETTER(OpEnd);
  }
  ANTOUM_GEN_GETTER_SETTER(SrcStartAddr, std::vector<FieldDesc>({{0,55,36}}));
  ANTOUM_GEN_GETTER_SETTER(DstStartAddr, std::vector<FieldDesc>({{0,35,16}}));
  ANTOUM_GEN_GETTER_SETTER(Mode, std::vector<FieldDesc>({{0,2,1}}));
  ANTOUM_GEN_GETTER_SETTER(ConfigIdx, std::vector<FieldDesc>({{0,5,3}}));
  ANTOUM_GEN_GETTER_SETTER(OpEnd, std::vector<FieldDesc>({{0,0,0}}));
};

class GlbcpyInstr : public AntoumInstr {
public:
  explicit GlbcpyInstr() : AntoumInstr(OpCodeType::GLBCPY, 4) {
    ANTOUM_PUSH_GETTER_SETTER(SrcCoreId);
    ANTOUM_PUSH_GETTER_SETTER(SrcStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineLength);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineNum);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineStride);
    ANTOUM_PUSH_GETTER_SETTER(SrcSurfaceNum);
    ANTOUM_PUSH_GETTER_SETTER(SrcSurfaceStride);
    ANTOUM_PUSH_GETTER_SETTER(DstCoreId);
    ANTOUM_PUSH_GETTER_SETTER(DstStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(DstLineLength);
    ANTOUM_PUSH_GETTER_SETTER(DstLineNum);
    ANTOUM_PUSH_GETTER_SETTER(DstLineStride);
    ANTOUM_PUSH_GETTER_SETTER(DstSurfaceNum);
    ANTOUM_PUSH_GETTER_SETTER(DstSurfaceStride);
    ANTOUM_PUSH_GETTER_SETTER(PrecisionMode);
    ANTOUM_PUSH_GETTER_SETTER(FuncOp);
    ANTOUM_PUSH_GETTER_SETTER(Sync);
    ANTOUM_PUSH_GETTER_SETTER(OpEnd);
  }
  ANTOUM_GEN_GETTER_SETTER(SrcCoreId, std::vector<FieldDesc>({{0,8,7}}));
  ANTOUM_GEN_GETTER_SETTER(SrcStartAddr, std::vector<FieldDesc>({{0,55,35}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineLength, std::vector<FieldDesc>({{1,63,52}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineNum, std::vector<FieldDesc>({{1,51,41}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineStride, std::vector<FieldDesc>({{1,40,26}}));
  ANTOUM_GEN_GETTER_SETTER(SrcSurfaceNum, std::vector<FieldDesc>({{1,25,19}}));
  ANTOUM_GEN_GETTER_SETTER(SrcSurfaceStride, std::vector<FieldDesc>({{1,18,0}}));
  ANTOUM_GEN_GETTER_SETTER(DstCoreId, std::vector<FieldDesc>({{0,6,5}}));
  ANTOUM_GEN_GETTER_SETTER(DstStartAddr, std::vector<FieldDesc>({{0,34,14}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineLength, std::vector<FieldDesc>({{2,63,52}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineNum, std::vector<FieldDesc>({{2,51,41}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineStride, std::vector<FieldDesc>({{2,40,26}}));
  ANTOUM_GEN_GETTER_SETTER(DstSurfaceNum, std::vector<FieldDesc>({{2,25,19}}));
  ANTOUM_GEN_GETTER_SETTER(DstSurfaceStride, std::vector<FieldDesc>({{2,18,0}}));
  ANTOUM_GEN_GETTER_SETTER(PrecisionMode, std::vector<FieldDesc>({{0,2,2}}));
  ANTOUM_GEN_GETTER_SETTER(FuncOp, std::vector<FieldDesc>({{0,4,3}}));
  ANTOUM_GEN_GETTER_SETTER(Sync, std::vector<FieldDesc>({{0,1,1}}));
  ANTOUM_GEN_GETTER_SETTER(OpEnd, std::vector<FieldDesc>({{0,0,0}}));
};

class DmamvInstr : public AntoumInstr {
public:
  explicit DmamvInstr() : AntoumInstr(OpCodeType::DMAMV, 4) {
    ANTOUM_PUSH_GETTER_SETTER(SrcStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineLength);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineNum);
    ANTOUM_PUSH_GETTER_SETTER(SrcLineStride);
    ANTOUM_PUSH_GETTER_SETTER(SrcSurfaceNum);
    ANTOUM_PUSH_GETTER_SETTER(SrcSurfaceStride);
    ANTOUM_PUSH_GETTER_SETTER(SrcDstMode);
    ANTOUM_PUSH_GETTER_SETTER(ChannelExtEn);
    ANTOUM_PUSH_GETTER_SETTER(TensorFormatConvertEn);
    ANTOUM_PUSH_GETTER_SETTER(ConstBroadcastDupNum);
    ANTOUM_PUSH_GETTER_SETTER(TensorCgb);
    ANTOUM_PUSH_GETTER_SETTER(DstStartAddr);
    ANTOUM_PUSH_GETTER_SETTER(DstLineLength);
    ANTOUM_PUSH_GETTER_SETTER(DstLineNum);
    ANTOUM_PUSH_GETTER_SETTER(DstLineStride);
    ANTOUM_PUSH_GETTER_SETTER(DstSurfaceNum);
    ANTOUM_PUSH_GETTER_SETTER(DstSurfaceStride);
    ANTOUM_PUSH_GETTER_SETTER(OpEnd);
  }
  ANTOUM_GEN_GETTER_SETTER(SrcStartAddr, std::vector<FieldDesc>({{0,55,24}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineLength, std::vector<FieldDesc>({{0,23,10}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineNum, std::vector<FieldDesc>({{1,63,53}}));
  ANTOUM_GEN_GETTER_SETTER(SrcLineStride, std::vector<FieldDesc>({{1,52,36}}));
  ANTOUM_GEN_GETTER_SETTER(SrcSurfaceNum, std::vector<FieldDesc>({{1,35,29}}));
  ANTOUM_GEN_GETTER_SETTER(SrcSurfaceStride, std::vector<FieldDesc>({{1,28,8}}));
  ANTOUM_GEN_GETTER_SETTER(SrcDstMode, std::vector<FieldDesc>({{1,3,0}}));
  ANTOUM_GEN_GETTER_SETTER(ChannelExtEn, std::vector<FieldDesc>({{0,5,5}}));
  ANTOUM_GEN_GETTER_SETTER(TensorFormatConvertEn, std::vector<FieldDesc>({{0,4,4}}));
  ANTOUM_GEN_GETTER_SETTER(ConstBroadcastDupNum, std::vector<FieldDesc>({{0,3,1}}));
  ANTOUM_GEN_GETTER_SETTER(TensorCgb, std::vector<FieldDesc>({{0,9,7}}));
  ANTOUM_GEN_GETTER_SETTER(DstStartAddr, std::vector<FieldDesc>({{2,63,32}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineLength, std::vector<FieldDesc>({{2,31,18}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineNum, std::vector<FieldDesc>({{3,63,53}}));
  ANTOUM_GEN_GETTER_SETTER(DstLineStride, std::vector<FieldDesc>({{3,52,36}}));
  ANTOUM_GEN_GETTER_SETTER(DstSurfaceNum, std::vector<FieldDesc>({{3,35,29}}));
  ANTOUM_GEN_GETTER_SETTER(DstSurfaceStride, std::vector<FieldDesc>({{3,28,8}}));
  ANTOUM_GEN_GETTER_SETTER(OpEnd, std::vector<FieldDesc>({{0,0,0}}));
};


std::shared_ptr<AntoumInstr> AntoumInstr::create(OpCodeType opCode) {
  switch (opCode) {
    case OpCodeType::CONV2D:   return std::shared_ptr<Conv2dInstr>(new Conv2dInstr);
    case OpCodeType::ACTIVATE: return std::shared_ptr<ActivateInstr>(new ActivateInstr);
    case OpCodeType::TOPK:     return std::shared_ptr<TopkInstr>(new TopkInstr);
    case OpCodeType::TRANS:    return std::shared_ptr<TransInstr>(new TransInstr);
    case OpCodeType::GLBCPY:   return std::shared_ptr<GlbcpyInstr>(new GlbcpyInstr);
    case OpCodeType::DMAMV:    return std::shared_ptr<DmamvInstr>(new DmamvInstr);
    default: return nullptr;
  }
}

} // namespace antoum

#endif // ANTOUM_ANTOUM_INSTR_H
