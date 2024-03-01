/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#include "vpu_instr.h"
#include "utils.h"

namespace quark {

std::vector<VpuInstrFormat> VpuInstr::instrFmtTable = {
  {{"vpu_set_load_base","base_addr"}, {{"op",(uint32_t)VpuOpcode::SET_LOAD_BASE}}},
  {{"vpu_set_load2_base","base_addr"}, {{"op",(uint32_t)VpuOpcode::SET_LOAD2_BASE}}},
  {{"vpu_set_store_base","base_addr"}, {{"op",(uint32_t)VpuOpcode::SET_STORE_BASE}}},
  {{"vpu_set_load_glb","stride","cluster_mask","bank_mask","cluster_broadcast","bank_broadcast","channel_broadcast"}, {{"op",(uint32_t)VpuOpcode::SET_LOAD_GLB}}},
  {{"vpu_set_store_glb","stride","cluster_mask","bank_mask"}, {{"op",(uint32_t)VpuOpcode::SET_STORE_GLB}}},
  {{"vpu_set_quantize","quantize"}, {{"op",(uint32_t)VpuOpcode::SET_QUANTIZE}}},
  {{"vpu_set_dequantize","dequantize"}, {{"op",(uint32_t)VpuOpcode::SET_DEQUANTIZE}}},
  {{"vpu_set_dequantize2","dequantize"}, {{"op",(uint32_t)VpuOpcode::SET_DEQUANTIZE2}}},
  {{"vpu_set_fs","fs_sel"}, {{"op",(uint32_t)VpuOpcode::SET_FS}}},
  {{"vpu_set_cmask_high","cluster_mask"}, {{"op",(uint32_t)VpuOpcode::SET_CMASK_HIGH}}},
  {{"vpu_nop"}, {{"op",(uint32_t)VpuOpcode::V_NOP}}},
  {{"vpu_op_end"}, {{"op",(uint32_t)VpuOpcode::V_OP_END}}},


  // ================= [BF16_ALU] load (INT8) =================
  {{"vpu_load_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  //{{"vpu_load_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load (UINT8) =================
  {{"vpu_uload_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  //{{"vpu_uload_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load (BF16) =================
  {{"vpu_fload_dual_glb","offset","rt","rs","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_upper_glb","offset","rt","rs","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"upper",1},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_glb","offset","rt","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_upper_glb","offset","rt","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"upper",1},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  //{{"vpu_fload_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  //{{"vpu_fload_upper_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"upper",1},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load (FP8_43) =================
  {{"vpu_e43load_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  //{{"vpu_e43load_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},


  // ================= [FP32_ALU] load (INT8) =================
  {{"v32_i8load_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  //{{"v32_i8load_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load (UINT8) =================
  {{"v32_u8load_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  //{{"v32_u8load_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load (BF16) =================
  {{"v32_f16load_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  //{{"v32_f16load_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load (FP32) =================
  {{"v32_f32load_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  //{{"v32_f32load_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},


  // ================= [BF16_ALU] store (INT8) =================
  {{"vpu_store_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] store (UINT8) =================
  {{"vpu_ustore_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] store (BF16) =================
  {{"vpu_fstore_glb","rt","offset","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fstore_upper_glb","rt","offset","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"upper",1},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] store (FP8_43) =================
  {{"vpu_e43store_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},


  // ================= [FP32_ALU] store (INT8) =================
  {{"v32_i8store_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] store (UINT8) =================
  {{"v32_u8store_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] store (BF16) =================
  {{"v32_f16store_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] store (FP32) =================
  {{"v32_f32store_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},


  // ================= [BF16_ALU] compute =================
  {{"vpu_fadd","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fsub","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmul","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmax","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmin","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmov","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_MOV},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_greater","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_GREATER},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_greater_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_GREATER_EQUAL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_less","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_LESS},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_less_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_LESS_EQUAL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_EQUAL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_sel","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_SEL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // compute with immediate
  {{"vpu_faddi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fsubi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmuli","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmaxi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmini","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmovi","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MOV},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_greateri","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_GREATER},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_greater_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_GREATER_EQUAL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_lessi","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_LESS},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_less_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_LESS_EQUAL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_EQUAL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_seli","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_SEL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_freduce_sum","rt"}, {{"op",(uint32_t)VpuOpcode::V_REDUCE_SUM},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},


  // ================= [FP32_ALU] compute =================
  {{"v32_fadd","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fsub","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmul","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmax","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmin","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_greater","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_GREATER},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_greater_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_GREATER_EQUAL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_less","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_LESS},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_less_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_LESS_EQUAL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_EQUAL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_sel","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_SEL},{"has_imm",0},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // compute with immediate
  {{"v32_faddi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fsubi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmuli","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmaxi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmini","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmovi","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MOV},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_greateri","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_GREATER},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_greater_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_GREATER_EQUAL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_lessi","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_LESS},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_less_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_LESS_EQUAL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_EQUAL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_seli","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_SEL},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_lui","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_LUI},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_li","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_LI},{"has_imm",1},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},


  // ================= [BF16_ALU] load + compute (INT8) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"vpu_load_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"vpu_load_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load + compute (UINT8) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"vpu_uload_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"vpu_uload_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load + compute (BF16) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"vpu_fload_dual_fadd","offset","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_fsub","offset","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_fmul","offset","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_fmax","offset","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_fmin","offset","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"vpu_fload_fadd","offset","rs","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_fsub","offset","rs","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_fmul","offset","rs","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_fmax","offset","rs","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_fmin","offset","rs","rd","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load + compute (FP8_43) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"vpu_e43load_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"vpu_e43load_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},


  // ================= [FP32_ALU] load + compute (INT8) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"v32_i8load_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"v32_i8load_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load + compute (UINT8) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"v32_u8load_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"v32_u8load_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load + compute (BF16) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"v32_f16load_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"v32_f16load_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load + compute (FP32) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"v32_f32load_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"v32_f32load_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},


  // ================= [BF16_ALU] compute + store (INT8) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"vpu_fadd_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fsub_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmul_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmax_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmin_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] compute + store (UINT8) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"vpu_fadd_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fsub_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmul_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmax_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmin_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] compute + store (BF16) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"vpu_fadd_fstore","rs","rd","offset","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fsub_fstore","rs","rd","offset","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmul_fstore","rs","rd","offset","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmax_fstore","rs","rd","offset","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmin_fstore","rs","rd","offset","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] compute + store (FP8_43) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"vpu_fadd_e43store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fsub_e43store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmul_e43store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmax_e43store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fmin_e43store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},


  // ================= [FP32_ALU] compute + store (INT8) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"v32_fadd_i8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fsub_i8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmul_i8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmax_i8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmin_i8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] compute + store (UINT8) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"v32_fadd_u8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fsub_u8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmul_u8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmax_u8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmin_u8store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] compute + store (BF16) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"v32_fadd_f16store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fsub_f16store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmul_f16store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmax_f16store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmin_f16store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] compute + store (FP32) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"v32_fadd_f32store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fsub_f32store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmul_f32store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmax_f32store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_fmin_f32store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},


  // ================= [BF16_ALU] load + compute + store (INT8) =================
  // GLB3 = GLB1 ※ GLB2
  {{"vpu_load_dual_fadd_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_dual_fsub_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_dual_fmul_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_dual_fmax_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_dual_fmin_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"vpu_load_fadd_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_fsub_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_fmul_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_fmax_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_load_fmin_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load + compute + store (UINT8) =================
  // GLB3 = GLB1 ※ GLB2
  {{"vpu_uload_dual_fadd_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_dual_fsub_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_dual_fmul_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_dual_fmax_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_dual_fmin_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"vpu_uload_fadd_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_fsub_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_fmul_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_fmax_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_uload_fmin_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load + compute + store (BF16) =================
  // GLB3 = GLB1 ※ GLB2
  {{"vpu_fload_dual_fadd_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_fsub_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_fmul_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_fmax_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_dual_fmin_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"vpu_fload_fadd_fstore","offset","rt","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_fsub_fstore","offset","rt","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_fmul_fstore","offset","rt","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_fmax_fstore","offset","rt","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_fload_fmin_fstore","offset","rt","consecutive_reg"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // ================= [BF16_ALU] load + compute + store (FP8_43) =================
  // GLB3 = GLB1 ※ GLB2
  {{"vpu_e43load_dual_fadd_e43store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_dual_fsub_e43store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_dual_fmul_e43store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_dual_fmax_e43store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_dual_fmin_e43store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"vpu_e43load_fadd_e43store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_fsub_e43store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_fmul_e43store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_fmax_e43store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},
  {{"vpu_e43load_fmin_e43store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::BF16_ALU}}},


  // ================= [FP32_ALU] load + compute + store (INT8) =================
  // GLB3 = GLB1 ※ GLB2
  {{"v32_i8load_dual_fadd_i8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_dual_fsub_i8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_dual_fmul_i8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_dual_fmax_i8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_dual_fmin_i8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"v32_i8load_fadd_i8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_fsub_i8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_fmul_i8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_fmax_i8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_i8load_fmin_i8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_INT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load + compute + store (UINT8) =================
  // GLB3 = GLB1 ※ GLB2
  {{"v32_u8load_dual_fadd_u8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_dual_fsub_u8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_dual_fmul_u8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_dual_fmax_u8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_dual_fmin_u8store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"v32_u8load_fadd_u8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_fsub_u8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_fmul_u8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_fmax_u8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_u8load_fmin_u8store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_UINT8},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load + compute + store (BF16) =================
  // GLB3 = GLB1 ※ GLB2
  {{"v32_f16load_dual_fadd_f16store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_dual_fsub_f16store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_dual_fmul_f16store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_dual_fmax_f16store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_dual_fmin_f16store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"v32_f16load_fadd_f16store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_fsub_f16store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_fmul_f16store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_fmax_f16store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f16load_fmin_f16store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_BF16},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // ================= [FP32_ALU] load + compute + store (FP32) =================
  // GLB3 = GLB1 ※ GLB2
  {{"v32_f32load_dual_fadd_f32store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_dual_fsub_f32store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_dual_fmul_f32store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_dual_fmax_f32store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_dual_fmin_f32store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"v32_f32load_fadd_f32store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_fsub_f32store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_fmul_f32store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_fmax_f32store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},
  {{"v32_f32load_fmin_f32store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"dtype",(uint32_t)VpuDataType::VPU_FP32},{"alu",(uint32_t)VpuAluType::FP32_ALU}}},


  // ================= [Scalar_ALU] =================
  // rt = rs ※ imm
  {{"s_addi","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_ADDI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_addiu","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_ADDIU},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_subi","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SUBI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_andi","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_ANDI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_ori","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_ORI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_xori","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_XORI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_sgti","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SGTI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_slti","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SLTI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_seqi","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SEQI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_sgei","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SGEI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_slei","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SLEI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_srai","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SRAI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_srli","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SRLI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_slli","rs","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_SLLI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_lui","imm","rt"}, {{"op",(uint32_t)VpuOpcode::S_LUI},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"has_imm",1},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  // rd = rs ※ rt
  {{"s_add","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_ADD},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_addu","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_ADDU},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_sub","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SUB},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_mul","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_MUL},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_mulh","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_MULH},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_mulhu","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_MULHU},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_min","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_MIN},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_max","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_MAX},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_minu","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_MINU},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_maxu","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_MAXU},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_and","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_AND},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_or","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_OR},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_xor","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_XOR},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_sgt","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SGT},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_slt","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SLT},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_seq","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SEQ},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_sge","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SGE},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_sle","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SLE},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_cmpsel","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_CMPSEL},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_sra","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SRA},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_srl","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SRL},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_sll","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::S_SLL},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_jump","offset"}, {{"op",(uint32_t)VpuOpcode::S_JUMP},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_jal","offset"}, {{"op",(uint32_t)VpuOpcode::S_JAL},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_jr","rs"}, {{"op",(uint32_t)VpuOpcode::S_JR},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_bne","rs","rt","offset"}, {{"op",(uint32_t)VpuOpcode::S_BNE},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_beq","rs","rt","offset"}, {{"op",(uint32_t)VpuOpcode::S_BEQ},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_blt","rs","rt","offset"}, {{"op",(uint32_t)VpuOpcode::S_BLT},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_bltu","rs","rt","offset"}, {{"op",(uint32_t)VpuOpcode::S_BLTU},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_bge","rs","rt","offset"}, {{"op",(uint32_t)VpuOpcode::S_BGE},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_bgeu","rs","rt","offset"}, {{"op",(uint32_t)VpuOpcode::S_BGEU},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_setcfg","rs","rt","sp_rd0","sp_rd1","src_num"}, {{"op",(uint32_t)VpuOpcode::S_SETCFG},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},
  {{"s_setrf","rf_sel"}, {{"op",(uint32_t)VpuOpcode::S_SETRF},{"dtype",(uint32_t)VpuDataType::VPU_INT32},{"alu",(uint32_t)VpuAluType::SCALAR_ALU}}},

};

VpuInstr::VpuInstr() : 
    binary(MemMapping.at(MM_VPU_CMD).cmd_width, 0) {
  fieldMap = {
    {"op", &op},
    {"op2", &op2},
    {"op3", &op3},
    {"base_addr", &baseAddr},
    {"stride", &stride},
    {"cluster_mask", &clusterMask},
    {"bank_mask", &bankMask},
    {"cluster_broadcast", &clusterBroadcast},
    {"bank_broadcast", &bankBroadcast},
    {"channel_broadcast", &channelBroadcast},
    {"quantize", &quantize},
    {"dequantize", &dequantize},
    {"rs", &rs},
    {"rt", &rt},
    {"rd", &rd},
    {"imm", &imm},
    {"offset", &offset},
    {"has_imm", &hasImm},
    {"upper", &upper},
    {"dtype", &dtype},
    {"alu", &alu},
    {"sp_rd0", &spRd0},
    {"sp_rd1", &spRd1},
    {"src_num", &srcNum},
    {"rf_sel", &rfSel},
    {"fs_sel", &fsSel},
    {"consecutive_reg", &consecutiveReg},
  };
  resetFields();
}

std::shared_ptr<spu::InstrInterface> VpuInstr::clone() {
  return std::shared_ptr<VpuInstr>(new VpuInstr(*this));
}

void VpuInstr::resetFields() {
  for (auto &it : fieldMap) {
    *it.second = 0;
  }
}

void VpuInstr::setFields(const std::map<std::string, uint32_t>& fields) {
  for (auto &it : fields) {
    CHECK(fieldMap.find(it.first) != fieldMap.end());
    *fieldMap[it.first] = it.second;
  }
}

void VpuInstr::getFields(std::map<std::string, uint32_t>& fields) {
  for (auto &it : fieldMap) {
    fields[it.first] = *it.second;
  }
}

std::map<std::string, std::vector<VpuInstrFormat*>>& VpuInstr::getNameInstrMap() {
  static std::mutex mtx;
  static bool staticInit = false;
  static std::map<std::string, std::vector<VpuInstrFormat*>> nameInstrMap;

  if (staticInit == false) {
    mtx.lock();
    if (staticInit == false) {
      for (auto &i : instrFmtTable) {
        nameInstrMap[i.names[0]].push_back(&i);
      }
      staticInit = true;
    }
    mtx.unlock();
  }
  return nameInstrMap;
}

std::map<VpuOpcode, std::vector<VpuInstrFormat*>>& VpuInstr::getOpInstrMap() {
  static std::mutex mtx;
  static bool staticInit = false;
  static std::map<VpuOpcode, std::vector<VpuInstrFormat*>> opInstrMap;

  if (staticInit == false) {
    mtx.lock();
    if (staticInit == false) {
      for (auto &i : instrFmtTable) {
        opInstrMap[VpuOpcode(i.fixedFields["op"])].push_back(&i);
      }
      staticInit = true;
    }
    mtx.unlock();
  }
  return opInstrMap;
}

bool VpuInstr::check() {
  CHECK(clusterBroadcast <= 4);
  CHECK(bankBroadcast <= 6);
  CHECK(rs <= 3);
  CHECK(rt <= 3);
  CHECK(rd <= 3);
  return true;
}

bool VpuInstr::loadText(const std::string&name, const std::vector<int> &args) {
  VpuInstrFormat *instrForm = nullptr;
  auto &nameInstrMap = getNameInstrMap();
  for (auto &i : nameInstrMap[name]) {
    if (i->names.size() == (args.size()+1)) {
      instrForm = i;
      break;
    }
  }
  if (nullptr == instrForm)
    return false;

  std::map<std::string, uint32_t> fields(instrForm->fixedFields);
  for (int i = 0; i < (int)args.size(); i++) {
    fields[instrForm->names[i+1]] = args[i];
  }
  resetFields();
  setFields(fields);
  return check();
}

bool VpuInstr::loadText(const std::string& text) {
  std::vector<std::string> v;
  std::vector<int> args;
  split(text, v);

  for (int i = 1; i < (int)v.size(); i++) {
    args.emplace_back(std::stoi(v[i]));
  }
  return loadText(v[0], args);
}

const std::string& VpuInstr::toText() {
  std::stringstream ss;
  VpuInstrFormat *instrForm = nullptr;
  auto &opInstrMap = getOpInstrMap();

  std::map<std::string, uint32_t> fields;
  getFields(fields);

  for (auto &i : opInstrMap[VpuOpcode(fields["op"])]) {
    bool flag = true;
    if (flag && VpuOpcode(fields["op2"]) != VpuOpcode::V_INVALID && i->fixedFields.find("op2") == i->fixedFields.end()) {
      flag = false;
    } else if (flag && VpuOpcode(fields["op3"]) != VpuOpcode::V_INVALID && i->fixedFields.find("op3") == i->fixedFields.end()) {
      flag = false;
    } else if (flag) {
      for (auto &j : i->fixedFields) {
        if ( fields.find(j.first) == fields.end() || j.second != fields[j.first]) {
          flag = false;
          break;
        }
      }
    }
    if (flag && (nullptr == instrForm || instrForm->fixedFields.size() < i->fixedFields.size())) {
      instrForm = i;
    }
  }

  if (nullptr != instrForm) {
    ss << instrForm->names[0];
    for (int i = 1; i < (int)instrForm->names.size(); i++) {
      ss << " ";
      ss << fields[instrForm->names[i]];
    }
  }

  text = ss.str();
  return text;
}

bool VpuInstr::loadBinary(const std::vector<uint8_t>& bin) {
  assert(bin.size() == binary.size());
  resetFields();

  if (0x00 == bin[3]) {
    op = (uint32_t)VpuOpcode::V_NOP;
  } else if (0xFF == bin[3]) {
    op = (uint32_t)VpuOpcode::V_OP_END;
  } else if (0x80 & bin[3]) {  // VPU M-type/R-type/I-type
    uint32_t cmdBin = ((uint32_t)bin[3]<<24) | ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
    if (0x40 & bin[3]) {   // VPU M-type
      offset = 0xFFFF & cmdBin;
      consecutiveReg = 0x1 & (cmdBin>>18);

      auto getComputeOp = [&] () {
        switch (0x7 & (cmdBin >> 24)) {
          case 0:   return VpuOpcode::V_ADD;
          case 1:   return VpuOpcode::V_SUB;
          case 2:   return VpuOpcode::V_MUL;
          case 3:   return VpuOpcode::V_MAX;
          case 4:   return VpuOpcode::V_MIN;
          default:  return VpuOpcode::V_INVALID;
        }
      };

      switch (0x3 & (cmdBin>>28)) {
        case 0: dtype = (uint32_t)VpuDataType::VPU_INT8;   break;
        case 1: dtype = (uint32_t)VpuDataType::VPU_UINT8;  break;
        case 2: dtype = (uint32_t)VpuDataType::VPU_BF16;   break;
        case 3: dtype = (uint32_t)VpuDataType::VPU_FP32;   break;
        default:
          break;
      }

      if ((0x1&(cmdBin>>19)) && (0x1&(cmdBin>>16))==0) { // super acceleration
        alu = (0x1 & (cmdBin>>27)) ?(uint32_t)VpuAluType::FP32_ALU :(uint32_t)VpuAluType::BF16_ALU;

        if (0x1 & (cmdBin >> 17)) { // dual load
          op = (uint32_t)VpuOpcode::V_LOAD_DUAL_GLB;
        } else { // single load
          op = (uint32_t)VpuOpcode::V_LOAD_GLB;
          rt = 0x03 & (cmdBin >> 20); // rs, select register file index , rt
        }
        op2 = (uint32_t)getComputeOp();
        op3 = (uint32_t)VpuOpcode::V_STORE_GLB;
      } else if (0x1&(cmdBin>>16)) { // special acceleration
        alu = (0x1 & (cmdBin>>19)) ?(uint32_t)VpuAluType::FP32_ALU :(uint32_t)VpuAluType::BF16_ALU;
        rd = 0x03 & (cmdBin >> 22);            
        if (0x01 & (cmdBin) >> 27) { // store
          op = (uint32_t)getComputeOp();
          op2 = (uint32_t)VpuOpcode::V_STORE_GLB;
          rs = 0x03 & (cmdBin >> 20);
        } else { // load
          if (0x1 & (cmdBin >> 17)) { // dual load
            op = (uint32_t)VpuOpcode::V_LOAD_DUAL_GLB;
          } else { // single load
            op = (uint32_t)VpuOpcode::V_LOAD_GLB;
            rs = 0x03 & (cmdBin >> 20);
          }
          op2 = (uint32_t)getComputeOp();
        }
      } else { // normal load/store
        alu = (0x1 & (cmdBin>>26)) ?(uint32_t)VpuAluType::FP32_ALU :(uint32_t)VpuAluType::BF16_ALU;     ////
        upper = 0x1 & (cmdBin>>24); //bf16 upper
        rt = 0x03 & (cmdBin>>20); // rs, select register file index , rt
        if (0x01 & (cmdBin>>27)) { // store
          op = (uint32_t)VpuOpcode::V_STORE_GLB;
        } else { // load
          if (0x01 & (cmdBin>>17)) { // dual load
            op = (uint32_t)VpuOpcode::V_LOAD_DUAL_GLB;
            rs = 0x03 & (cmdBin >> 22);
          } else { // single load
            op = (uint32_t)VpuOpcode::V_LOAD_GLB;
          }
        }
      }
    } else { // VPU R-type/I-type
      alu = (0x1 & (cmdBin>>28)) ?(uint32_t)VpuAluType::FP32_ALU :(uint32_t)VpuAluType::BF16_ALU;
      dtype = (0x1 & (cmdBin>>28)) ?(uint32_t)VpuDataType::VPU_FP32 : (uint32_t)VpuDataType::VPU_BF16;

      hasImm = 0x1 & (cmdBin >> 29);
      if (hasImm) {
        imm = 0xFFFF & cmdBin;
      } else {
        rt = 0x7 & (cmdBin >> 15);
      }
      rs = 0x7 & (cmdBin >> 18);
      rd = 0x7 & (cmdBin >> 21);

      switch (0xF & (cmdBin >> 24)) {
        case 0x00: op = (uint32_t)VpuOpcode::V_ADD;            break;
        case 0x01: op = (uint32_t)VpuOpcode::V_SUB;            break;
        case 0x02: op = (uint32_t)VpuOpcode::V_MUL;            break;
        case 0x03: op = (uint32_t)VpuOpcode::V_MAX;            break;
        case 0x04: op = (uint32_t)VpuOpcode::V_MIN;            break;
        case 0x05: op = (uint32_t)VpuOpcode::V_MOV;            break;
        case 0x06: op = (uint32_t)VpuOpcode::V_REDUCE_SUM;     break;
        case 0x07: op = (uint32_t)VpuOpcode::V_GREATER;        break;
        case 0x08: op = (uint32_t)VpuOpcode::V_EQUAL;          break;
        case 0x09: op = (uint32_t)VpuOpcode::V_LESS;           break;
        case 0x0A: op = (uint32_t)VpuOpcode::V_GREATER_EQUAL;  break;
        case 0x0B: op = (uint32_t)VpuOpcode::V_LESS_EQUAL;     break;
        case 0x0C: op = (uint32_t)VpuOpcode::V_SEL;            break;
        case 0x0D: op = (uint32_t)VpuOpcode::V_LUI;            break;
        case 0x0E: op = (uint32_t)VpuOpcode::V_LI;             break;
        default:
          break;
      }
    }
  } else if (0x40 & bin[3]) { // scalar
    uint32_t cmdBin = ((uint32_t)bin[3]<<24) | ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
    alu = (uint32_t)VpuAluType::SCALAR_ALU;
    dtype = (uint32_t)VpuDataType::VPU_INT32;

    uint32_t funct = 0x3F & (cmdBin>>24);
    if (funct <= 0xE) {
      op = (uint32_t)VpuOpcode::S_ADDI + funct;
      rs = 0xF & (cmdBin>>20);
      rt = 0xF & (cmdBin>>16);
      imm = 0xFFFF & cmdBin;
      hasImm = true;
    } else if (funct >= 0x11 && funct <= 0x26) {
      op = (uint32_t)VpuOpcode::S_ADD + funct - 0x11;
      rs = 0xF & (cmdBin>>20);
      rt = 0xF & (cmdBin>>16);
      rd = 0xF & (cmdBin>>12);
    } else if (funct >= 0x29 && funct <= 0x31) {
      op = (uint32_t)VpuOpcode::S_JUMP + funct - 0x29;
      rs = 0xF & (cmdBin>>20);
      rt = 0xF & (cmdBin>>16);
      offset = 0xFFFF & cmdBin;
    } else if (funct == 0x34) {
      op = (uint32_t)VpuOpcode::S_SETCFG;
      rs = 0xF & (cmdBin>>20);
      rt = 0xF & (cmdBin>>16);
      spRd0 = 0x1F & (cmdBin>>11);
      spRd1 = 0x1F & (cmdBin>>6);
      srcNum = 0x1 & (cmdBin>>5);
    } else if (funct == 0x35) {
      op = (uint32_t)VpuOpcode::S_SETRF;
      rfSel = 0x7 & cmdBin;
    }
  } else { // configurable register
    uint32_t label = ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
    switch (0x7F & bin[3]) {
    case 1:
      op = (uint32_t)VpuOpcode::SET_LOAD_BASE;
      baseAddr = label;
      break;
    case 2:
      op = (uint32_t)VpuOpcode::SET_LOAD_GLB;
      stride = 0x7 & (label >> 21);
      clusterMask = 0xFF & (label >> 13);
      bankMask = 0xF & (label >> 9);
      clusterBroadcast = 0x7 & (label >> 6);
      bankBroadcast = 0x7 & (label >> 3);
      channelBroadcast = 0x3 & (label >> 1);
      break;
    case 3:
      op = (uint32_t)VpuOpcode::SET_DEQUANTIZE;
      dequantize = 0xFFFFFF & label;
      break;
    case 4:
      op = (uint32_t)VpuOpcode::SET_LOAD2_BASE;
      baseAddr = label;
      break;
    case 6:
      op = (uint32_t)VpuOpcode::SET_DEQUANTIZE2;
      dequantize = 0xFFFFFF & label;
      break;
    case 7:
      op = (uint32_t)VpuOpcode::SET_STORE_BASE;
      baseAddr = label;
      break;
    case 8:
      op = (uint32_t)VpuOpcode::SET_STORE_GLB;
      stride = 0x7 & (label >> 21);
      clusterMask = 0xFFFF & (label >> 5);
      bankMask = 0xF & (label >> 1);
      break;
    case 9:
      op = (uint32_t)VpuOpcode::SET_QUANTIZE;
      quantize = 0xFFFF & label;
      break;
    case 10:
      op = (uint32_t)VpuOpcode::SET_FS;
      fsSel = 0xFF & label;
      break;
    case 11:
      op = (uint32_t)VpuOpcode::SET_CMASK_HIGH;
      clusterMask = 0xFF & (label >> 16);
      break;
    default:
      break;
    }
  }
  return check();
}

const std::vector<uint8_t>& VpuInstr::toBinary() {
  auto &bin = binary;
  std::fill(bin.begin(), bin.end(), 0);

  if (VpuOpcode(op) == VpuOpcode::V_NOP) {
    bin[3] = 0x00;
  } else if (VpuOpcode(op) == VpuOpcode::V_OP_END) {
    bin[3] = 0xFF;
  } else if (VpuOpcode(op) >= VpuOpcode::S_ADDI) {  // scalar
    uint32_t cmdBin = 0x40000000;
    if (VpuOpcode(op) <= VpuOpcode::S_LUI) {
      cmdBin |= (0x3F&(op-(uint32_t)VpuOpcode::S_ADDI)) << 24;
      cmdBin |= (0xF&rs) << 20;
      cmdBin |= (0xF&rt) << 16;
      cmdBin |= (0xFFFF&imm) << 0;
    } else if (VpuOpcode(op) <= VpuOpcode::S_SLL) {
      cmdBin |= (0x3F&(0x11+op-(uint32_t)VpuOpcode::S_ADD)) << 24;
      cmdBin |= (0xF&rs) << 20;
      cmdBin |= (0xF&rt) << 16;
      cmdBin |= (0xF&rd) << 12;
    } else if (VpuOpcode(op) <= VpuOpcode::S_BGEU) {
      cmdBin |= (0x3F&(0x29+op-(uint32_t)VpuOpcode::S_JUMP)) << 24;
      if (VpuOpcode(op) >= VpuOpcode::S_JR) {
        cmdBin |= (0xF&rs) << 20;
      }
      if (VpuOpcode(op) >= VpuOpcode::S_BNE) {
        cmdBin |= (0xF&rt) << 16;
      }
      if (VpuOpcode(op) != VpuOpcode::S_JR) {
        cmdBin |= (0xFFFF&offset) << 0;
      }
    } else if (VpuOpcode(op) == VpuOpcode::S_SETCFG) {
      cmdBin |= (0x3F&(0x34)) << 24;
      cmdBin |= (0xF&rs) << 20;
      cmdBin |= (0xF&rt) << 16;
      cmdBin |= (0x1F&spRd0) << 11;
      cmdBin |= (0x1F&spRd1) << 6;
      cmdBin |= (0x1&srcNum) << 5;
    } else if (VpuOpcode(op) == VpuOpcode::S_SETRF) {
      cmdBin |= (0x3F&(0x35)) << 24;
      cmdBin |= (0x7&rfSel);
    }

    bin[0] = 0xFF & (cmdBin >> 0);
    bin[1] = 0xFF & (cmdBin >> 8);
    bin[2] = 0xFF & (cmdBin >> 16);
    bin[3] = 0xFF & (cmdBin >> 24);
  } else if (VpuOpcode(op) >= VpuOpcode::V_LOAD_DUAL_GLB) {  // vpu
    uint32_t cmdBin = 0x80000000;
    if (VpuOpcode(op) == VpuOpcode::V_LOAD_DUAL_GLB || VpuOpcode(op) == VpuOpcode::V_LOAD_GLB ||
      VpuOpcode(op) == VpuOpcode::V_LOAD_CONST || VpuOpcode(op) == VpuOpcode::V_STORE_GLB ||
      VpuOpcode(op2) == VpuOpcode::V_STORE_GLB) { // load/store
      cmdBin |= 0x40000000;
      cmdBin |= 0xFFFF & (uint32_t)offset;
      cmdBin |= (0x01 & (uint32_t)consecutiveReg) << 18;

      auto getComputeOp = [&] (VpuOpcode opcode) -> uint32_t {
        switch (opcode) {
        case VpuOpcode::V_ADD: return (0x7 & 0) << 24;
        case VpuOpcode::V_SUB: return (0x7 & 1) << 24;
        case VpuOpcode::V_MUL: return (0x7 & 2) << 24;
        case VpuOpcode::V_MAX: return (0x7 & 3) << 24;
        case VpuOpcode::V_MIN: return (0x7 & 4) << 24;
        default:    return 0;
        }
      };

      switch (VpuDataType(dtype)) {
        case VpuDataType::VPU_INT8:  cmdBin |= (0x3 & 0) << 28;  break;
        case VpuDataType::VPU_UINT8: cmdBin |= (0x3 & 1) << 28;  break;
        case VpuDataType::VPU_BF16:  cmdBin |= (0x3 & 2) << 28;  break;
        case VpuDataType::VPU_FP32:  cmdBin |= (0x3 & 3) << 28;  break;
        default:
          break;
      }

      if (VpuOpcode(op) != VpuOpcode::V_INVALID && VpuOpcode(op3) != VpuOpcode::V_INVALID) { // super acceleration
        cmdBin |= 0x1 << 19;
        cmdBin |= (0x01 & (uint32_t)((VpuAluType(alu)==VpuAluType::FP32_ALU) ?1 :0)) << 27;
        cmdBin |= getComputeOp(VpuOpcode(op2));

        if (VpuOpcode(op) == VpuOpcode::V_LOAD_DUAL_GLB) { // dual load
          cmdBin |= 0x1 << 17;
        } else { // single load
          cmdBin |= (0x03 & (uint32_t)rt) << 20;
        }
      } else if (VpuOpcode(op) != VpuOpcode::V_INVALID && VpuOpcode(op2) != VpuOpcode::V_INVALID) { // secial acceleration
        cmdBin |= 0x1 << 16;
        cmdBin |= (0x01 & (uint32_t)((VpuAluType(alu)==VpuAluType::FP32_ALU) ?1 :0)) << 19;
        cmdBin |= (0x03 & (uint32_t)rd) << 22;

        if (VpuOpcode(op2) == VpuOpcode::V_STORE_GLB) { // store
          cmdBin |= 0x1 << 27;
          cmdBin |= getComputeOp(VpuOpcode(op));
          cmdBin |= (0x03 & (uint32_t)rs) << 20;
        } else { // load
          if (VpuOpcode(op) == VpuOpcode::V_LOAD_DUAL_GLB) { // dual load
            cmdBin |= 0x1 << 17;
          } else { // single load
            cmdBin |= (0x03 & (uint32_t)rs) << 20;
          }
          cmdBin |= getComputeOp(VpuOpcode(op2));
        }
      } else {
        cmdBin |= (0x01 & (uint32_t)((VpuAluType(alu)==VpuAluType::FP32_ALU) ?1 :0)) << 26;
        cmdBin |= (0x01 & (uint32_t)upper) << 24;
        cmdBin |= (0x03 & (uint32_t)rt) << 20;

        if (VpuOpcode(op) == VpuOpcode::V_STORE_GLB) { // store
          cmdBin |= 0x1 << 27;
          //cmdBin |= (0x01 & (uint32_t)((dtype==VPU_INT16) ?1 :0)) << 25;
        } else { // load
          if (VpuOpcode(op) == VpuOpcode::V_LOAD_DUAL_GLB) { // dual load
            cmdBin |= 0x1 << 17;
            cmdBin |= (0x03 & (uint32_t)rs) << 22;
          }
        }
      }
    } else { // computation
      cmdBin |= (0x1 & (uint32_t)((VpuAluType(alu)==VpuAluType::FP32_ALU) ?1 :0)) << 28;
      cmdBin |= (0x1 & (uint32_t)hasImm) << 29;
      if (hasImm) {
        cmdBin |= 0xFFFF & (uint32_t)imm;
      } else {
        cmdBin |= (0x7 & (uint32_t)rt) << 15;
      }
      cmdBin |= (0x7 & (uint32_t)rs) << 18;
      cmdBin |= (0x7 & (uint32_t)rd) << 21;

      switch (VpuOpcode(op)) {
        case VpuOpcode::V_ADD:            cmdBin |= 0x00 << 24; break;
        case VpuOpcode::V_SUB:            cmdBin |= 0x01 << 24; break;
        case VpuOpcode::V_MUL:            cmdBin |= 0x02 << 24; break;
        case VpuOpcode::V_MAX:            cmdBin |= 0x03 << 24; break;
        case VpuOpcode::V_MIN:            cmdBin |= 0x04 << 24; break;
        case VpuOpcode::V_MOV:            cmdBin |= 0x05 << 24; break;
        case VpuOpcode::V_REDUCE_SUM:     cmdBin |= 0x06 << 24; break;
        case VpuOpcode::V_GREATER:        cmdBin |= 0x07 << 24; break;
        case VpuOpcode::V_EQUAL:          cmdBin |= 0x08 << 24; break;
        case VpuOpcode::V_LESS:           cmdBin |= 0x09 << 24; break;
        case VpuOpcode::V_GREATER_EQUAL:  cmdBin |= 0x0A << 24; break;
        case VpuOpcode::V_LESS_EQUAL:     cmdBin |= 0x0B << 24; break;
        case VpuOpcode::V_SEL:            cmdBin |= 0x0C << 24; break;
        case VpuOpcode::V_LUI:            cmdBin |= 0x0D << 24; break;
        case VpuOpcode::V_LI:             cmdBin |= 0x0E << 24; break;
        default:
          break;
      }
    }
    bin[0] = 0xFF & (cmdBin >> 0);
    bin[1] = 0xFF & (cmdBin >> 8);
    bin[2] = 0xFF & (cmdBin >> 16);
    bin[3] = 0xFF & (cmdBin >> 24);
  } else { // configurable register
    uint32_t label = 0;
    switch (VpuOpcode(op)) {
      case VpuOpcode::SET_LOAD_BASE:
        bin[3] |= 1;
        label = (uint32_t)baseAddr;
        break;
      case VpuOpcode::SET_LOAD_GLB:
        bin[3] |= 2;
        label |= (0x7 & (uint32_t)stride) << 21;
        label |= (0xFF & (uint32_t)clusterMask) << 13;
        label |= (0xF & (uint32_t)bankMask) << 9;
        label |= (0x7 & (uint32_t)clusterBroadcast) << 6;
        label |= (0x7 & (uint32_t)bankBroadcast) << 3;
        label |= (0x3 & (uint32_t)channelBroadcast) << 1;
        break;
      case VpuOpcode::SET_DEQUANTIZE:
        bin[3] |= 3;
        label |= 0xFFFFFF & (uint32_t)dequantize;
        break;
      case VpuOpcode::SET_LOAD2_BASE:
        bin[3] |= 4;
        label = (uint32_t)baseAddr;
        break;
      case VpuOpcode::SET_DEQUANTIZE2:
        bin[3] |= 6;
        label |= 0xFFFFFF & (uint32_t)dequantize;
        break;
      case VpuOpcode::SET_STORE_BASE:
        bin[3] |= 7;
        label = (uint32_t)baseAddr;
        break;
      case VpuOpcode::SET_STORE_GLB:
        bin[3] |= 8;
        label |= (0x7 & (uint32_t)stride) << 21;
        label |= (0xFFFF & (uint32_t)clusterMask) << 5;
        label |= (0xF & (uint32_t)bankMask) << 1;
        break;
      case VpuOpcode::SET_QUANTIZE:
        bin[3] |= 9;
        label |= 0xFFFF & (uint32_t)quantize;
        break;
      case VpuOpcode::SET_FS:
        bin[3] |= 10;
        label |= 0xFF & (uint32_t)fsSel;
        break;
      case VpuOpcode::SET_CMASK_HIGH:
        bin[3] |= 11;
        label |= (0xFF & (uint32_t)clusterMask) << 16;
        break;
      default:
        break;
    }
    bin[0] = 0xFF & (label >> 0);
    bin[1] = 0xFF & (label >> 8);
    bin[2] = 0xFF & (label >> 16);
  }
  return bin;
}

} // namespace quark
