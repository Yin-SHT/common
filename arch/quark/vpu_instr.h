/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef QAUARK_VPU_INSTR_H
#define QAUARK_VPU_INSTR_H

#include <stdint.h>
#include <memory>
#include <map>
#include "instr_interface.h"
#include "antoum/vpu_instr.h"

namespace quark {

enum class VpuOpcode {
  V_INVALID,
  V_NOP,
  V_OP_END,
  SET_LOAD_BASE,
  SET_LOAD2_BASE,
  SET_STORE_BASE,
  SET_LOAD_GLB,
  SET_STORE_GLB,
  SET_QUANTIZE,
  SET_DEQUANTIZE,
  SET_DEQUANTIZE2,
  SET_FS,
  SET_CMASK_HIGH,
  V_LOAD_DUAL_GLB,
  V_LOAD_GLB,
  V_LOAD_CONST,
  V_STORE_GLB,
  V_ADD,
  V_SUB,
  V_MUL,
  V_MIN,
  V_MAX,
  V_MOV,
  V_REDUCE_SUM,
  V_GREATER,
  V_GREATER_EQUAL,
  V_LESS,
  V_LESS_EQUAL,
  V_EQUAL,
  V_SEL,
  V_LUI,
  V_LI,
  
  S_ADDI,
  S_ADDIU,
  S_SUBI,
  S_ANDI,
  S_ORI,
  S_XORI,
  S_SGTI,
  S_SLTI,
  S_SEQI,
  S_SGEI,
  S_SLEI,
  S_SRAI,
  S_SRLI,
  S_SLLI,
  S_LUI,
  S_ADD,
  S_ADDU,
  S_SUB,
  S_MUL,
  S_MULH,
  S_MULHU,
  S_MIN,
  S_MAX,
  S_MINU,
  S_MAXU,
  S_AND,
  S_OR,
  S_XOR,
  S_SGT,
  S_SLT,
  S_SEQ,
  S_SGE,
  S_SLE,
  S_CMPSEL,
  S_SRA,
  S_SRL,
  S_SLL,
  S_JUMP,
  S_JAL,
  S_JR,
  S_BNE,
  S_BEQ,
  S_BLT,
  S_BLTU,
  S_BGE,
  S_BGEU,
  S_SETCFG,
  S_SETRF,
};

enum class ScalarSetCfg {
   S_SET_LOAD_STRIDE = 0,
   S_SET_LOAD_CLUSTER_MSK,
   S_SET_LOAD_BANK_MSK,
   S_SET_LOAD_CLUSTER_BRD,
   S_SET_LOAD_BANK_BRD,
   S_SET_LOAD_BASE0,
   S_SET_LOAD_DQUAN0,
   S_SET_LOAD_BASE1,
   S_SET_LOAD_DQUAN1,
   S_SET_STORE_BASE,
   S_SET_STORE_STRIDE,
   S_SET_STORE_CLUSTER_MSK,
   S_SET_STORE_BANK_MSK,
   S_SET_STORE_QUAN,
   S_SET_LOAD_CHANN_BRD,
   S_SET_BF16_OR_FP16,
};

struct VpuInstrFormat {
  std::vector<std::string> names;
  std::map<std::string, uint32_t> fixedFields;
};

class VpuInstr : public spu::InstrInterface {
public:
  VpuInstr();
  std::shared_ptr<spu::InstrInterface> clone() override;
  bool loadText(const std::string& text) override;
  const std::string& toText() override;
  bool loadBinary(const std::vector<uint8_t>& bin) override;
  const std::vector<uint8_t>& toBinary() override;

protected:
  void resetFields();
  void mapFields();
  void setFields(const std::map<std::string, uint32_t>& fields);
  void getFields(std::map<std::string, uint32_t>& fields);
  std::map<std::string, std::vector<VpuInstrFormat*>>& getNameInstrMap();
  std::map<VpuOpcode, std::vector<VpuInstrFormat*>>& getOpInstrMap();
  bool loadText(const std::string&name, const std::vector<int> &args);
  bool check();

protected:
  static std::vector<VpuInstrFormat> instrFmtTable;
  std::map<std::string, uint32_t *> fieldMap;
  std::string text;
  std::vector<uint8_t> binary;

public:
  uint32_t op;
  uint32_t op2;
  uint32_t op3;
  uint32_t baseAddr;
  uint32_t stride;
  uint32_t clusterMask;
  uint32_t bankMask;
  uint32_t clusterBroadcast;
  uint32_t bankBroadcast;
  uint32_t channelBroadcast;
  uint32_t quantize;
  uint32_t dequantize;
  uint32_t rs;
  uint32_t rt;
  uint32_t rd;
  uint32_t imm;
  uint32_t offset;
  uint32_t hasImm;
  uint32_t upper;
  uint32_t dtype;
  uint32_t alu;
  uint32_t spRd0;
  uint32_t spRd1;
  uint32_t srcNum;
  uint32_t rfSel;
  uint32_t fsSel;
  uint32_t consecutiveReg;
};

} // namespace quark 

#endif // QAUARK_VPU_INSTR_H
