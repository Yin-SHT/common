/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#include "spu_instr.h"
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
  return std::shared_ptr<SpuInstr>(new SpuInstr(opCode));
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

} // namespace quark
