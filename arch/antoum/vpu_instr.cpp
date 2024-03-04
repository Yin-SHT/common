/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#include "antoum/vpu_instr.h"
#include "utils.h"

namespace antoum {

std::vector<VpuInstrFormat> VpuInstr::instrFmtTable = {
  {{"vpu_set_load_base","base_addr"}, {{"op",(uint32_t)VpuOpcode::SET_LOAD_BASE}}},
  {{"vpu_set_load2_base","base_addr"}, {{"op",(uint32_t)VpuOpcode::SET_LOAD2_BASE}}},
  {{"vpu_set_store_base","base_addr"}, {{"op",(uint32_t)VpuOpcode::SET_STORE_BASE}}},
  {{"vpu_set_load_glb","stride","cluster_mask","bank_mask","cluster_broadcast","bank_broadcast"}, {{"op",(uint32_t)VpuOpcode::SET_LOAD_GLB}}},
  {{"vpu_set_store_glb","stride","cluster_mask","bank_mask"}, {{"op",(uint32_t)VpuOpcode::SET_STORE_GLB}}},
  {{"vpu_set_quantize","quantize"}, {{"op",(uint32_t)VpuOpcode::SET_QUANTIZE}}},
  {{"vpu_set_dequantize","dequantize"}, {{"op",(uint32_t)VpuOpcode::SET_DEQUANTIZE}}},
  {{"vpu_set_dequantize2","dequantize"}, {{"op",(uint32_t)VpuOpcode::SET_DEQUANTIZE2}}},

  {{"vpu_nop"}, {{"op",(uint32_t)VpuOpcode::V_NOP}}},
  {{"vpu_op_end"}, {{"op",(uint32_t)VpuOpcode::V_OP_END}}},
  {{"vpu_fload_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"is_bf16",1}}},
  {{"vpu_fload_upper_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"is_bf16",1},{"upper",1}}},
  {{"vpu_fload_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"is_bf16",1}}},
  {{"vpu_fload_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"is_bf16",1}}},
  {{"vpu_fload_dual_upper_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"is_bf16",1},{"upper",1}}},
  {{"vpu_fstore_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fstore_upper_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1},{"upper",1}}},
  {{"vpu_i16store_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_int16",1}}},
  {{"vpu_i16store_upper_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_int16",1},{"upper",1}}},

  {{"vpu_load_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_store_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},

  {{"vpu_uload_glb","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_const","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_CONST},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_glb","offset","rt","rs"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_ustore_glb","rt","offset"}, {{"op",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},

  {{"vpu_fadd","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"has_imm",0}}},
  {{"vpu_fsub","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"has_imm",0}}},
  {{"vpu_fmul","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"has_imm",0}}},
  {{"vpu_fmax","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"has_imm",0}}},
  {{"vpu_fmin","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"has_imm",0}}},
  {{"vpu_greater","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_GREATER},{"has_imm",0}}},
  {{"vpu_greater_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_GREATER_EQUAL},{"has_imm",0}}},
  {{"vpu_less","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_LESS},{"has_imm",0}}},
  {{"vpu_less_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_LESS_EQUAL},{"has_imm",0}}},
  {{"vpu_equal","rs","rt"}, {{"op",(uint32_t)VpuOpcode::V_EQUAL},{"has_imm",0}}},
  {{"vpu_sel","rs","rt","rd"}, {{"op",(uint32_t)VpuOpcode::V_SEL},{"has_imm",0}}},

  {{"vpu_faddi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"has_imm",1}}},
  {{"vpu_fsubi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"has_imm",1}}},
  {{"vpu_fmuli","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"has_imm",1}}},
  {{"vpu_fmaxi","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"has_imm",1}}},
  {{"vpu_fmini","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"has_imm",1}}},
  {{"vpu_fmovi","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_MOV},{"has_imm",1}}},
  {{"vpu_greateri","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_GREATER},{"has_imm",1}}},
  {{"vpu_greater_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_GREATER_EQUAL},{"has_imm",1}}},
  {{"vpu_lessi","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_LESS},{"has_imm",1}}},
  {{"vpu_less_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_LESS_EQUAL},{"has_imm",1}}},
  {{"vpu_equali","rs","imm"}, {{"op",(uint32_t)VpuOpcode::V_EQUAL},{"has_imm",1}}},
  {{"vpu_seli","rs","imm","rd"}, {{"op",(uint32_t)VpuOpcode::V_SEL},{"has_imm",1}}},
  {{"vpu_freduce_sum","rt"}, {{"op",(uint32_t)VpuOpcode::V_REDUCE_SUM}}},

  // ================= load + compute (BF16) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"vpu_fload_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"is_bf16",1}}},
  {{"vpu_fload_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"is_bf16",1}}},
  {{"vpu_fload_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"is_bf16",1}}},
  {{"vpu_fload_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"is_bf16",1}}},
  {{"vpu_fload_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"is_bf16",1}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"vpu_fload_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"is_bf16",1}}},
  {{"vpu_fload_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"is_bf16",1}}},
  {{"vpu_fload_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"is_bf16",1}}},
  {{"vpu_fload_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"is_bf16",1}}},
  {{"vpu_fload_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"is_bf16",1}}},

  // ================= compute + store (BF16) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"vpu_fadd_fstore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fsub_fstore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fmul_fstore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fmax_fstore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fmin_fstore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},

  // ================= load + compute + store (BF16) =================
  // GLB3 = GLB1 ※ GLB2
  {{"vpu_fload_dual_fadd_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fload_dual_fsub_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fload_dual_fmul_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fload_dual_fmax_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fload_dual_fmin_fstore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"vpu_fload_fadd_fstore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fload_fsub_fstore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fload_fmul_fstore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fload_fmax_fstore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},
  {{"vpu_fload_fmin_fstore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",1}}},

  // ================= load + compute (INT8) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"vpu_load_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"is_bf16",0},{"is_unsigned",0}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"vpu_load_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"is_bf16",0},{"is_unsigned",0}}},

  // ================= compute + store (INT8) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"vpu_fadd_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_fsub_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_fmul_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_fmax_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_fmin_store","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},

  // ================= load + compute + store (INT8) =================
  // GLB3 = GLB1 ※ GLB2
  {{"vpu_load_dual_fadd_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_fsub_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_fmul_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_fmax_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_dual_fmin_store","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"vpu_load_fadd_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_fsub_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_fmul_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_fmax_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
  {{"vpu_load_fmin_store","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},

  // ================= load + compute (UINT8) =================
  // 𝑟𝑓𝑑 = GLB1 ※ GLB2
  {{"vpu_uload_dual_fadd","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_fsub","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_fmul","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_fmax","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_fmin","offset","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"is_bf16",0},{"is_unsigned",1}}},
  // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
  {{"vpu_uload_fadd","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_fsub","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_fmul","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_fmax","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_fmin","offset","rs","rd"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"is_bf16",0},{"is_unsigned",1}}},

  // ================= compute + store (UINT8) =================
  // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
  {{"vpu_fadd_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_ADD},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_fsub_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_SUB},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_fmul_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MUL},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_fmax_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MAX},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_fmin_ustore","rs","rd","offset"}, {{"op",(uint32_t)VpuOpcode::V_MIN},{"op2",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},

  // ================= load + compute + store (UINT8) =================
  // GLB3 = GLB1 ※ GLB2
  {{"vpu_uload_dual_fadd_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_fsub_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_fmul_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_fmax_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_dual_fmin_ustore","offset"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_DUAL_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  // GLB3 = GLB1 ※ 𝑟𝑓𝑡
  {{"vpu_uload_fadd_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_ADD},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_fsub_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_SUB},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_fmul_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MUL},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_fmax_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MAX},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
  {{"vpu_uload_fmin_ustore","offset","rt"}, {{"op",(uint32_t)VpuOpcode::V_LOAD_GLB},{"op2",(uint32_t)VpuOpcode::V_MIN},{"op3",(uint32_t)VpuOpcode::V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},

};

VpuInstr::VpuInstr() : 
    binary(kSizePerVpuCmd, 0) {
  mapFields();
  resetFields();
}

std::shared_ptr<spu::InstrInterface> VpuInstr::clone() {
  auto ret = std::shared_ptr<VpuInstr>(new VpuInstr(*this));
  ret->mapFields();
  return ret;
}

void VpuInstr::mapFields() {
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
    {"quantize", &quantize},
    {"dequantize", &dequantize},
    {"rs", &rs},
    {"rt", &rt},
    {"rd", &rd},
    {"imm", &imm},
    {"offset", &offset},
    {"is_bf16", &isBf16},
    {"is_unsigned", &isUnsigned},
    {"has_imm", &hasImm},
    {"upper", &upper},
    {"is_int16", &isInt16},
  };
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
  CHECK(clusterBroadcast <= 3);
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
    bool unmatched = false;
    for (auto &j : i->fixedFields) {
      if ( fields.find(j.first) == fields.end() || j.second != fields[j.first]) {
        unmatched = true;
        break;
      }
    }
    if (false == unmatched && (nullptr == instrForm || 
      instrForm->fixedFields.size() < i->fixedFields.size())) {
      instrForm = i;
    }
  }

  if (nullptr != instrForm) {
    ss << instrForm->names[0];
    for(int i = 1; i < (int)instrForm->names.size(); i++) {
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
  } else if (0x80 & bin[3]) {  // vpu
    uint32_t cmdBin = ((uint32_t)bin[3]<<24) | ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
    if (0x40 & bin[3]) { // load/store
      isBf16 = 0x1 & (cmdBin >> 29);
      isUnsigned = 0x1 & (cmdBin >> 28);
      offset = 0xFFFF & cmdBin;

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

      if (0x1 & (cmdBin >> 19)) { // super acceleration
        if (0x1 & (cmdBin >> 17)) { // dual load
          op = (uint32_t)VpuOpcode::V_LOAD_DUAL_GLB;
        } else { // single load
          op = (uint32_t)VpuOpcode::V_LOAD_GLB;
          rt = 0x03 & (cmdBin >> 20);
        }
        op2 = (uint32_t)getComputeOp();
        op3 = (uint32_t)VpuOpcode::V_STORE_GLB;
      } else if (0x01 & (cmdBin >> 16)) { // secial acceleration
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
      } else {
        isInt16 = 0x1 & (cmdBin >> 25);
        upper = 0x1 & (cmdBin >> 24);
        rt = 0x03 & (cmdBin >> 20);
        if (0x01 & (cmdBin) >> 27) { // store
          op = (uint32_t)VpuOpcode::V_STORE_GLB;
        } else { // load
          if (0x01 & (cmdBin) >> 18) { // load const
            op = (uint32_t)VpuOpcode::V_LOAD_CONST;
          } else if (0x01 & (cmdBin) >> 17) { // dual load
            op = (uint32_t)VpuOpcode::V_LOAD_DUAL_GLB;
            rs = 0x03 & (cmdBin >> 22);
          } else { // single load
            op = (uint32_t)VpuOpcode::V_LOAD_GLB;
          }
        }
      }
    } else { // computation
      hasImm = 0x1 & (cmdBin >> 29);
      if (hasImm) {
        imm = 0xFFFF & cmdBin;
      } else {
        rt = 0x7 & (cmdBin >> 15);
      }
      rs = 0x7 & (cmdBin >> 18);
      rd = 0x7 & (cmdBin >> 21);

      switch (0x1F & (cmdBin >> 24)) {
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
      }
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
        clusterBroadcast = 0x3 & (label >> 7);
        bankBroadcast = 0x7 & (label >> 4);
        break;
      case 3:
        op = (uint32_t)VpuOpcode::SET_DEQUANTIZE;
        dequantize = 0xFFFF & label;
        break;
      case 4:
        op = (uint32_t)VpuOpcode::SET_LOAD2_BASE;
        baseAddr = label;
        break;
      case 6:
        op = (uint32_t)VpuOpcode::SET_DEQUANTIZE2;
        dequantize = 0xFFFF & label;
        break;
      case 7:
        op = (uint32_t)VpuOpcode::SET_STORE_BASE;
        baseAddr = label;
        break;
      case 8:
        op = (uint32_t)VpuOpcode::SET_STORE_GLB;
        stride = 0x7 & (label >> 21);
        clusterMask = 0xFF & (label >> 13);
        bankMask = 0xF & (label >> 9);
        break;
      case 9:
        op = (uint32_t)VpuOpcode::SET_QUANTIZE;
        quantize = 0xFFFF & label;
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
  } else if (VpuOpcode(op) >= VpuOpcode::V_LOAD_DUAL_GLB) {  // vpu
    uint32_t cmdBin = 0x80000000;
    if (VpuOpcode(op) == VpuOpcode::V_LOAD_DUAL_GLB || 
        VpuOpcode(op) == VpuOpcode::V_LOAD_GLB ||
        VpuOpcode(op) == VpuOpcode::V_LOAD_CONST || 
        VpuOpcode(op) == VpuOpcode::V_STORE_GLB ||
        VpuOpcode(op2) == VpuOpcode::V_STORE_GLB) { // load/store
      cmdBin |= 0x40000000;
      cmdBin |= (0x1 & (uint32_t)isBf16) << 29;
      cmdBin |= (0x1 & (uint32_t)isUnsigned) << 28;
      cmdBin |= 0xFFFF & (uint32_t)offset;

      auto getComputeOp = [&] (VpuOpcode opcode) -> uint32_t {
        switch (opcode) {
          case VpuOpcode::V_ADD:  return (0x7 & 0) << 24;
          case VpuOpcode::V_SUB:  return (0x7 & 1) << 24;
          case VpuOpcode::V_MUL:  return (0x7 & 2) << 24;
          case VpuOpcode::V_MAX:  return (0x7 & 3) << 24;
          case VpuOpcode::V_MIN:  return (0x7 & 4) << 24;
          default:                return 0;
        }
      };

      if (VpuOpcode(op) != VpuOpcode::V_INVALID && 
          VpuOpcode(op3) != VpuOpcode::V_INVALID) { // super acceleration
        cmdBin |= 0x1 << 19;
        cmdBin |= getComputeOp(VpuOpcode(op2));

        if (VpuOpcode(op) == VpuOpcode::V_LOAD_DUAL_GLB) { // dual load
          cmdBin |= 0x1 << 17;
        } else { // single load
          cmdBin |= (0x03 & (uint32_t)rt) << 20;
        }
      } else if (VpuOpcode(op) != VpuOpcode::V_INVALID && 
                 VpuOpcode(op2) != VpuOpcode::V_INVALID) { // secial acceleration
        cmdBin |= 0x1 << 16;
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
        cmdBin |= (0x01 & (uint32_t)isInt16) << 25;
        cmdBin |= (0x01 & (uint32_t)upper) << 24;
        cmdBin |= (0x03 & (uint32_t)rt) << 20;

        if (VpuOpcode(op) == VpuOpcode::V_STORE_GLB) { // store
          cmdBin |= 0x1 << 27;
        } else { // load
          if (VpuOpcode(op) == VpuOpcode::V_LOAD_CONST) { // load const
            cmdBin |= 0x1 << 18;
          } else if (VpuOpcode(op) == VpuOpcode::V_LOAD_DUAL_GLB) { // dual load
            cmdBin |= 0x1 << 17;
            cmdBin |= (0x03 & (uint32_t)rs) << 22;
          }
        }
      }
    } else { // computation
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
        label |= (0x3 & (uint32_t)clusterBroadcast) << 7;
        label |= (0x7 & (uint32_t)bankBroadcast) << 4;
        break;
      case VpuOpcode::SET_DEQUANTIZE:
        bin[3] |= 3;
        label |= 0xFFFF & (uint32_t)dequantize;
        break;
      case VpuOpcode::SET_LOAD2_BASE:
        bin[3] |= 4;
        label = (uint32_t)baseAddr;
        break;
      case VpuOpcode::SET_DEQUANTIZE2:
        bin[3] |= 6;
        label |= 0xFFFF & (uint32_t)dequantize;
        break;
      case VpuOpcode::SET_STORE_BASE:
        bin[3] |= 7;
        label = (uint32_t)baseAddr;
        break;
      case VpuOpcode::SET_STORE_GLB:
        bin[3] |= 8;
        label |= (0x7 & (uint32_t)stride) << 21;
        label |= (0xFF & (uint32_t)clusterMask) << 13;
        label |= (0xF & (uint32_t)bankMask) << 9;
        break;
      case VpuOpcode::SET_QUANTIZE:
        bin[3] |= 9;
        label |= 0xFFFF & (uint32_t)quantize;
        break;
      default:
        break;
    }
    bin[0] = 0xFF & (label >> 0);
    bin[1] = 0xFF & (label >> 8);
    bin[2] = 0xFF & (label >> 16);
  }
  return binary;
}

} // namespace antoum
