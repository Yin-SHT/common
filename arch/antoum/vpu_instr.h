/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef ANTOUM_INSTR_INTERFACE_H
#define ANTOUM_INSTR_INTERFACE_H

#include <stdint.h>
#include <memory>
#include <map>
#include "instr_interface.h"

enum class VpuAluType {
  BF16_ALU = 0,
  SCALAR_ALU,
  FP32_ALU,
  INT32_ALU,
  FP16_ALU,
};

enum class VpuDataType {
  VPU_INT8 = 0,
  VPU_UINT8,
  VPU_BF16,
  VPU_INT16,
  VPU_UINT16,
  VPU_FP32,
  VPU_INT32,
  VPU_FP8_52,
  VPU_FP8_43,
  VPU_FP16,
};

enum class VpuDataMode {
  DATA_MODE_TENSOR = 0,
  DATA_MODE_WEIGHT_DENSE,
  DATA_MODE_WEIGHT_VALUE,
  DATA_MODE_WEIGHT_INDEX,
};

namespace antoum {
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
  uint32_t quantize;
  uint32_t dequantize;
  uint32_t rs;
  uint32_t rt;
  uint32_t rd;
  uint32_t imm;
  uint32_t offset;
  uint32_t isBf16;
  uint32_t isUnsigned;
  uint32_t hasImm;
  uint32_t upper;
  uint32_t isInt16;
};

} // namespace antoum

#endif // ANTOUM_INSTR_INTERFACE_H
