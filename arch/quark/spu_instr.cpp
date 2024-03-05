/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#include "quark/spu_instr.h"
#include "utils.h"
#include <sstream>

namespace quark {

static const std::map<SpuInstr::OpCodeType, std::string> opCodeStrMap = {
  {SpuInstr::OpCodeType::LDT,   "ldt"},
  {SpuInstr::OpCodeType::LDW,   "ldw"},
  {SpuInstr::OpCodeType::LDM,   "ldm"},
  {SpuInstr::OpCodeType::STT,   "stt"},
  {SpuInstr::OpCodeType::GEMM,  "gemm"},
  {SpuInstr::OpCodeType::TR,    "tr"},
  {SpuInstr::OpCodeType::TRA,   "tra"},
  {SpuInstr::OpCodeType::ACT,   "act"},
  {SpuInstr::OpCodeType::ACTI,  "acti"},
  {SpuInstr::OpCodeType::ACTA,  "acta"},
  {SpuInstr::OpCodeType::SORT,  "sort"},
  {SpuInstr::OpCodeType::TOPK,  "topk"},
  {SpuInstr::OpCodeType::CPY,   "cpy"},
  {SpuInstr::OpCodeType::CPYA,  "cpya"},
  {SpuInstr::OpCodeType::QNT,   "qnt"},
  {SpuInstr::OpCodeType::QNTI,  "qnti"},
  {SpuInstr::OpCodeType::DQT,   "dqt"},
  {SpuInstr::OpCodeType::DQTI,  "dqti"},
  {SpuInstr::OpCodeType::LPS,   "lps"},
  {SpuInstr::OpCodeType::LPSI,  "lpsi"},
  {SpuInstr::OpCodeType::LPE,   "lpe"},
  {SpuInstr::OpCodeType::SET,   "set"},
  {SpuInstr::OpCodeType::SETI,  "seti"},
  {SpuInstr::OpCodeType::SETIH, "setih"},
  {SpuInstr::OpCodeType::SETIL, "setil"},
  {SpuInstr::OpCodeType::AAI,   "aai"},
  {SpuInstr::OpCodeType::END,   "end"},
};

SpuInstr::SpuInstr(OpCodeType opCode) : 
    opCode(opCode), binary(4, 0) {
}

std::string SpuInstr::opCodeToString(OpCodeType opCode) {
  if(opCodeStrMap.find(opCode) != opCodeStrMap.end()) {
    return opCodeStrMap.at(opCode);
  }
  return "";
}

SpuInstr::OpCodeType SpuInstr::stringToOpCode(std::string name) {
  for(auto &it : opCodeStrMap) {
    if(it.second == name)
      return it.first;
  }
  return OpCodeType::INVALID;
}

SpuInstr::OpCodeType SpuInstr::getOpCode() {
  return opCode;
}

std::shared_ptr<spu::InstrInterface> SpuInstr::clone() {
  auto ret = create(opCode);
  ret->fields = fields;
  return ret;
}

bool SpuInstr::loadBinary(const std::vector<uint8_t>& bin) {
  auto x = *reinterpret_cast<const uint32_t *>(bin.data());
  opCode = OpCodeType((x>>26)&0x3f);
  fields = x&(((uint32_t)1<<26)-1);
  return true;
}

const std::vector<uint8_t>& SpuInstr::toBinary() {
  *reinterpret_cast<uint32_t *>(binary.data()) = 
      ((uint32_t)opCode<<26) | (fields&(((uint32_t)1<<26)-1));
  return binary;
}

bool SpuInstr::loadText(const std::string& text) {
  std::vector<std::string> v;
  split(text, v);

  if(v.size() != (setters.size()+1))
    return false;
  if((opCode = stringToOpCode(v[0])) != OpCodeType::INVALID)
    return false;

  for(size_t i = 0; i < setters.size(); i++) {
    setters[i](std::strtoul(v[i+1].c_str(), 0, 10));
  }
  return true;
}

const std::string& SpuInstr::toText() {
  std::stringstream ss;
  ss << opCodeToString(opCode);
  for(size_t i = 0; i < getters.size(); i++) {
    ss << " " << getters[i]();
  }
  text = ss.str();
  return text;
}

std::shared_ptr<SpuInstr> SpuInstr::create(OpCodeType opCode) {
  switch (opCode) {
    case OpCodeType::LDT:   return std::shared_ptr<LdtInstr>(new LdtInstr);
    case OpCodeType::LDW:   return std::shared_ptr<LdwInstr>(new LdwInstr);
    case OpCodeType::LDM:   return std::shared_ptr<LdmInstr>(new LdmInstr);
    case OpCodeType::STT:   return std::shared_ptr<SttInstr>(new SttInstr);
    case OpCodeType::GEMM:  return std::shared_ptr<GemmInstr>(new GemmInstr);
    case OpCodeType::TR:    return std::shared_ptr<TrInstr>(new TrInstr);
    case OpCodeType::TRA:   return std::shared_ptr<TraInstr>(new TraInstr);
    case OpCodeType::ACT:   return std::shared_ptr<ActInstr>(new ActInstr);
    case OpCodeType::ACTI:  return std::shared_ptr<ActiInstr>(new ActiInstr);
    case OpCodeType::ACTA:  return std::shared_ptr<ActaInstr>(new ActaInstr);
    case OpCodeType::SORT:  return std::shared_ptr<SortInstr>(new SortInstr);
    case OpCodeType::TOPK:  return std::shared_ptr<TopkInstr>(new TopkInstr);
    case OpCodeType::CPY:   return std::shared_ptr<CpyInstr>(new CpyInstr);
    case OpCodeType::CPYA:  return std::shared_ptr<CpyaInstr>(new CpyaInstr);
    case OpCodeType::QNT:   return std::shared_ptr<QntInstr>(new QntInstr);
    case OpCodeType::QNTI:  return std::shared_ptr<QntiInstr>(new QntiInstr);
    case OpCodeType::DQT:   return std::shared_ptr<DqtInstr>(new DqtInstr);
    case OpCodeType::DQTI:  return std::shared_ptr<DqtiInstr>(new DqtiInstr);
    case OpCodeType::LPS:   return std::shared_ptr<LpsInstr>(new LpsInstr);
    case OpCodeType::LPSI:  return std::shared_ptr<LpsiInstr>(new LpsiInstr);
    case OpCodeType::LPE:   return std::shared_ptr<LpeInstr>(new LpeInstr);
    case OpCodeType::SET:   return std::shared_ptr<SetInstr>(new SetInstr);
    case OpCodeType::SETI:  return std::shared_ptr<SetiInstr>(new SetiInstr);
    case OpCodeType::SETIH: return std::shared_ptr<SetihInstr>(new SetihInstr);
    case OpCodeType::SETIL: return std::shared_ptr<SetilInstr>(new SetilInstr);
    case OpCodeType::AAI:   return std::shared_ptr<AaiInstr>(new AaiInstr);
    case OpCodeType::END:   return std::shared_ptr<EndInstr>(new EndInstr);
    default:                return nullptr;
  }
}

std::shared_ptr<SpuInstr> SpuInstr::create(const std::string& text) {
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

std::shared_ptr<SpuInstr> SpuInstr::create(const std::vector<uint8_t>& bin) {
  auto x = *reinterpret_cast<const uint32_t *>(bin.data());
  auto opCode = OpCodeType((x>>26)&0x3f);
  auto ret = create(opCode);
  if (ret != nullptr)
    ret->loadBinary(bin);

  return ret;
}

} // namespace quark
