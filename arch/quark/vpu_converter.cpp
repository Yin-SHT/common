#include "quark/vpu_converter.h"
#include "quark/vpu_instr.h"

namespace quark {
static std::vector<VPUInstrFormat> g_vpu_instr_table_quark = {
    {{"vpu_set_load_base","base_addr"}, {{"op",SET_LOAD_BASE}}},
    {{"vpu_set_load2_base","base_addr"}, {{"op",SET_LOAD2_BASE}}},
    {{"vpu_set_store_base","base_addr"}, {{"op",SET_STORE_BASE}}},
    {{"vpu_set_load_glb","stride","cluster_mask","bank_mask","cluster_broadcast","bank_broadcast","channel_broadcast"}, {{"op",SET_LOAD_GLB}}},
    {{"vpu_set_store_glb","stride","cluster_mask","bank_mask"}, {{"op",SET_STORE_GLB}}},
    {{"vpu_set_quantize","quantize"}, {{"op",SET_QUANTIZE}}},
    {{"vpu_set_dequantize","dequantize"}, {{"op",SET_DEQUANTIZE}}},
    {{"vpu_set_dequantize2","dequantize"}, {{"op",SET_DEQUANTIZE2}}},
    {{"vpu_set_fs","fs_sel"}, {{"op",SET_FS}}},
    {{"vpu_set_cmask_high","cluster_mask"}, {{"op",SET_CMASK_HIGH}}},
    {{"vpu_nop"}, {{"op",V_NOP}}},
    {{"vpu_op_end"}, {{"op",V_OP_END}}},


    // ================= [BF16_ALU] load (INT8) =================
    {{"vpu_load_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    //{{"vpu_load_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load (UINT8) =================
    {{"vpu_uload_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    //{{"vpu_uload_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load (BF16) =================
    {{"vpu_fload_dual_glb","offset","rt","rs","consecutive_reg"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_upper_glb","offset","rt","rs","consecutive_reg"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_BF16},{"upper",1},{"alu",BF16_ALU}}},
    {{"vpu_fload_glb","offset","rt","consecutive_reg"}, {{"op",V_LOAD_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_upper_glb","offset","rt","consecutive_reg"}, {{"op",V_LOAD_GLB},{"dtype",VPU_BF16},{"upper",1},{"alu",BF16_ALU}}},
    //{{"vpu_fload_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    //{{"vpu_fload_upper_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_BF16},{"upper",1},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load (FP8_43) =================
    {{"vpu_e43load_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    //{{"vpu_e43load_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},


    // ================= [FP32_ALU] load (INT8) =================
    {{"v32_i8load_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    //{{"v32_i8load_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load (UINT8) =================
    {{"v32_u8load_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    //{{"v32_u8load_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load (BF16) =================
    {{"v32_f16load_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    //{{"v32_f16load_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load (FP32) =================
    {{"v32_f32load_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    //{{"v32_f32load_const","offset","rt"}, {{"op",V_LOAD_CONST},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},


    // ================= [BF16_ALU] store (INT8) =================
    {{"vpu_store_glb","rt","offset"}, {{"op",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] store (UINT8) =================
    {{"vpu_ustore_glb","rt","offset"}, {{"op",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] store (BF16) =================
    {{"vpu_fstore_glb","rt","offset","consecutive_reg"}, {{"op",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fstore_upper_glb","rt","offset","consecutive_reg"}, {{"op",V_STORE_GLB},{"dtype",VPU_BF16},{"upper",1},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] store (FP8_43) =================
    {{"vpu_e43store_glb","rt","offset"}, {{"op",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},


    // ================= [FP32_ALU] store (INT8) =================
    {{"v32_i8store_glb","rt","offset"}, {{"op",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] store (UINT8) =================
    {{"v32_u8store_glb","rt","offset"}, {{"op",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] store (BF16) =================
    {{"v32_f16store_glb","rt","offset"}, {{"op",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] store (FP32) =================
    {{"v32_f32store_glb","rt","offset"}, {{"op",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},


    // ================= [BF16_ALU] compute =================
    {{"vpu_fadd","rs","rt","rd"}, {{"op",V_ADD},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fsub","rs","rt","rd"}, {{"op",V_SUB},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmul","rs","rt","rd"}, {{"op",V_MUL},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmax","rs","rt","rd"}, {{"op",V_MAX},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmin","rs","rt","rd"}, {{"op",V_MIN},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmov","rs","rd"}, {{"op",V_MOV},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_greater","rs","rt"}, {{"op",V_GREATER},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_greater_equal","rs","rt"}, {{"op",V_GREATER_EQUAL},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_less","rs","rt"}, {{"op",V_LESS},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_less_equal","rs","rt"}, {{"op",V_LESS_EQUAL},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_equal","rs","rt"}, {{"op",V_EQUAL},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_sel","rs","rt","rd"}, {{"op",V_SEL},{"has_imm",0},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    // compute with immediate
    {{"vpu_faddi","rs","imm","rd"}, {{"op",V_ADD},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fsubi","rs","imm","rd"}, {{"op",V_SUB},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmuli","rs","imm","rd"}, {{"op",V_MUL},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmaxi","rs","imm","rd"}, {{"op",V_MAX},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmini","rs","imm","rd"}, {{"op",V_MIN},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmovi","imm","rd"}, {{"op",V_MOV},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_greateri","rs","imm"}, {{"op",V_GREATER},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_greater_equali","rs","imm"}, {{"op",V_GREATER_EQUAL},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_lessi","rs","imm"}, {{"op",V_LESS},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_less_equali","rs","imm"}, {{"op",V_LESS_EQUAL},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_equali","rs","imm"}, {{"op",V_EQUAL},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_seli","rs","imm","rd"}, {{"op",V_SEL},{"has_imm",1},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_freduce_sum","rt"}, {{"op",V_REDUCE_SUM},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},


    // ================= [FP32_ALU] compute =================
    {{"v32_fadd","rs","rt","rd"}, {{"op",V_ADD},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fsub","rs","rt","rd"}, {{"op",V_SUB},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmul","rs","rt","rd"}, {{"op",V_MUL},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmax","rs","rt","rd"}, {{"op",V_MAX},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmin","rs","rt","rd"}, {{"op",V_MIN},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_greater","rs","rt"}, {{"op",V_GREATER},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_greater_equal","rs","rt"}, {{"op",V_GREATER_EQUAL},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_less","rs","rt"}, {{"op",V_LESS},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_less_equal","rs","rt"}, {{"op",V_LESS_EQUAL},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_equal","rs","rt"}, {{"op",V_EQUAL},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_sel","rs","rt","rd"}, {{"op",V_SEL},{"has_imm",0},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    // compute with immediate
    {{"v32_faddi","rs","imm","rd"}, {{"op",V_ADD},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fsubi","rs","imm","rd"}, {{"op",V_SUB},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmuli","rs","imm","rd"}, {{"op",V_MUL},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmaxi","rs","imm","rd"}, {{"op",V_MAX},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmini","rs","imm","rd"}, {{"op",V_MIN},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmovi","imm","rd"}, {{"op",V_MOV},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_greateri","rs","imm"}, {{"op",V_GREATER},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_greater_equali","rs","imm"}, {{"op",V_GREATER_EQUAL},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_lessi","rs","imm"}, {{"op",V_LESS},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_less_equali","rs","imm"}, {{"op",V_LESS_EQUAL},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_equali","rs","imm"}, {{"op",V_EQUAL},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_seli","rs","imm","rd"}, {{"op",V_SEL},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_lui","imm","rd"}, {{"op",V_LUI},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_li","imm","rd"}, {{"op",V_LI},{"has_imm",1},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},


    // ================= [BF16_ALU] load + compute (INT8) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"vpu_load_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"vpu_load_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load + compute (UINT8) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"vpu_uload_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"vpu_uload_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load + compute (BF16) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"vpu_fload_dual_fadd","offset","rd","consecutive_reg"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_fsub","offset","rd","consecutive_reg"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_fmul","offset","rd","consecutive_reg"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_fmax","offset","rd","consecutive_reg"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_fmin","offset","rd","consecutive_reg"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"vpu_fload_fadd","offset","rs","rd","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_fsub","offset","rs","rd","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_fmul","offset","rs","rd","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_fmax","offset","rs","rd","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_fmin","offset","rs","rd","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load + compute (FP8_43) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"vpu_e43load_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"vpu_e43load_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},


    // ================= [FP32_ALU] load + compute (INT8) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"v32_i8load_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"v32_i8load_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load + compute (UINT8) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"v32_u8load_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"v32_u8load_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load + compute (BF16) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"v32_f16load_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"v32_f16load_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load + compute (FP32) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"v32_f32load_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"v32_f32load_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},


    // ================= [BF16_ALU] compute + store (INT8) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"vpu_fadd_store","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_fsub_store","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_fmul_store","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_fmax_store","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_fmin_store","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] compute + store (UINT8) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"vpu_fadd_ustore","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_fsub_ustore","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_fmul_ustore","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_fmax_ustore","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_fmin_ustore","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] compute + store (BF16) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"vpu_fadd_fstore","rs","rd","offset","consecutive_reg"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fsub_fstore","rs","rd","offset","consecutive_reg"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmul_fstore","rs","rd","offset","consecutive_reg"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmax_fstore","rs","rd","offset","consecutive_reg"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fmin_fstore","rs","rd","offset","consecutive_reg"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] compute + store (FP8_43) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"vpu_fadd_e43store","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_fsub_e43store","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_fmul_e43store","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_fmax_e43store","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_fmin_e43store","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},


    // ================= [FP32_ALU] compute + store (INT8) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"v32_fadd_i8store","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_fsub_i8store","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_fmul_i8store","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_fmax_i8store","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_fmin_i8store","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] compute + store (UINT8) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"v32_fadd_u8store","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_fsub_u8store","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_fmul_u8store","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_fmax_u8store","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_fmin_u8store","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] compute + store (BF16) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"v32_fadd_f16store","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_fsub_f16store","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_fmul_f16store","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_fmax_f16store","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_fmin_f16store","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] compute + store (FP32) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"v32_fadd_f32store","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fsub_f32store","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmul_f32store","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmax_f32store","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_fmin_f32store","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},


    // ================= [BF16_ALU] load + compute + store (INT8) =================
    // GLB3 = GLB1 ※ GLB2
    {{"vpu_load_dual_fadd_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_dual_fsub_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_dual_fmul_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_dual_fmax_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_dual_fmin_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"vpu_load_fadd_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_fsub_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_fmul_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_fmax_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    {{"vpu_load_fmin_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load + compute + store (UINT8) =================
    // GLB3 = GLB1 ※ GLB2
    {{"vpu_uload_dual_fadd_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_dual_fsub_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_dual_fmul_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_dual_fmax_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_dual_fmin_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"vpu_uload_fadd_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_fsub_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_fmul_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_fmax_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    {{"vpu_uload_fmin_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load + compute + store (BF16) =================
    // GLB3 = GLB1 ※ GLB2
    {{"vpu_fload_dual_fadd_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_fsub_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_fmul_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_fmax_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_dual_fmin_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"vpu_fload_fadd_fstore","offset","rt","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_fsub_fstore","offset","rt","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_fmul_fstore","offset","rt","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_fmax_fstore","offset","rt","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    {{"vpu_fload_fmin_fstore","offset","rt","consecutive_reg"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",BF16_ALU}}},
    // ================= [BF16_ALU] load + compute + store (FP8_43) =================
    // GLB3 = GLB1 ※ GLB2
    {{"vpu_e43load_dual_fadd_e43store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_dual_fsub_e43store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_dual_fmul_e43store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_dual_fmax_e43store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_dual_fmin_e43store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"vpu_e43load_fadd_e43store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_fsub_e43store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_fmul_e43store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_fmax_e43store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},
    {{"vpu_e43load_fmin_e43store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",BF16_ALU}}},


    // ================= [FP32_ALU] load + compute + store (INT8) =================
    // GLB3 = GLB1 ※ GLB2
    {{"v32_i8load_dual_fadd_i8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_dual_fsub_i8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_dual_fmul_i8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_dual_fmax_i8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_dual_fmin_i8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"v32_i8load_fadd_i8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_fsub_i8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_fmul_i8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_fmax_i8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    {{"v32_i8load_fmin_i8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_INT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load + compute + store (UINT8) =================
    // GLB3 = GLB1 ※ GLB2
    {{"v32_u8load_dual_fadd_u8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_dual_fsub_u8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_dual_fmul_u8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_dual_fmax_u8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_dual_fmin_u8store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"v32_u8load_fadd_u8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_fsub_u8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_fmul_u8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_fmax_u8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    {{"v32_u8load_fmin_u8store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_UINT8},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load + compute + store (BF16) =================
    // GLB3 = GLB1 ※ GLB2
    {{"v32_f16load_dual_fadd_f16store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_dual_fsub_f16store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_dual_fmul_f16store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_dual_fmax_f16store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_dual_fmin_f16store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"v32_f16load_fadd_f16store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_fsub_f16store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_fmul_f16store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_fmax_f16store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    {{"v32_f16load_fmin_f16store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_BF16},{"alu",FP32_ALU}}},
    // ================= [FP32_ALU] load + compute + store (FP32) =================
    // GLB3 = GLB1 ※ GLB2
    {{"v32_f32load_dual_fadd_f32store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_dual_fsub_f32store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_dual_fmul_f32store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_dual_fmax_f32store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_dual_fmin_f32store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"v32_f32load_fadd_f32store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_fsub_f32store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_fmul_f32store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_fmax_f32store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},
    {{"v32_f32load_fmin_f32store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"dtype",VPU_FP32},{"alu",FP32_ALU}}},


    // ================= [Scalar_ALU] =================
    // rt = rs ※ imm
    {{"s_addi","rs","imm","rt"}, {{"op",S_ADDI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_addiu","rs","imm","rt"}, {{"op",S_ADDIU},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_subi","rs","imm","rt"}, {{"op",S_SUBI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_andi","rs","imm","rt"}, {{"op",S_ANDI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_ori","rs","imm","rt"}, {{"op",S_ORI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_xori","rs","imm","rt"}, {{"op",S_XORI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_sgti","rs","imm","rt"}, {{"op",S_SGTI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_slti","rs","imm","rt"}, {{"op",S_SLTI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_seqi","rs","imm","rt"}, {{"op",S_SEQI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_sgei","rs","imm","rt"}, {{"op",S_SGEI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_slei","rs","imm","rt"}, {{"op",S_SLEI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_srai","rs","imm","rt"}, {{"op",S_SRAI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_srli","rs","imm","rt"}, {{"op",S_SRLI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_slli","rs","imm","rt"}, {{"op",S_SLLI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    {{"s_lui","imm","rt"}, {{"op",S_LUI},{"dtype",VPU_INT32},{"has_imm",1},{"alu",SCALAR_ALU}}},
    // rd = rs ※ rt
    {{"s_add","rs","rt","rd"}, {{"op",S_ADD},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_addu","rs","rt","rd"}, {{"op",S_ADDU},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_sub","rs","rt","rd"}, {{"op",S_SUB},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_mul","rs","rt","rd"}, {{"op",S_MUL},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_mulh","rs","rt","rd"}, {{"op",S_MULH},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_mulhu","rs","rt","rd"}, {{"op",S_MULHU},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_min","rs","rt","rd"}, {{"op",S_MIN},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_max","rs","rt","rd"}, {{"op",S_MAX},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_minu","rs","rt","rd"}, {{"op",S_MINU},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_maxu","rs","rt","rd"}, {{"op",S_MAXU},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_and","rs","rt","rd"}, {{"op",S_AND},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_or","rs","rt","rd"}, {{"op",S_OR},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_xor","rs","rt","rd"}, {{"op",S_XOR},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_sgt","rs","rt","rd"}, {{"op",S_SGT},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_slt","rs","rt","rd"}, {{"op",S_SLT},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_seq","rs","rt","rd"}, {{"op",S_SEQ},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_sge","rs","rt","rd"}, {{"op",S_SGE},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_sle","rs","rt","rd"}, {{"op",S_SLE},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_cmpsel","rs","rt","rd"}, {{"op",S_CMPSEL},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_sra","rs","rt","rd"}, {{"op",S_SRA},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_srl","rs","rt","rd"}, {{"op",S_SRL},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_sll","rs","rt","rd"}, {{"op",S_SLL},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_jump","offset"}, {{"op",S_JUMP},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_jal","offset"}, {{"op",S_JAL},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_jr","rs"}, {{"op",S_JR},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_bne","rs","rt","offset"}, {{"op",S_BNE},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_beq","rs","rt","offset"}, {{"op",S_BEQ},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_blt","rs","rt","offset"}, {{"op",S_BLT},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_bltu","rs","rt","offset"}, {{"op",S_BLTU},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_bge","rs","rt","offset"}, {{"op",S_BGE},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_bgeu","rs","rt","offset"}, {{"op",S_BGEU},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_setcfg","rs","rt","sp_rd0","sp_rd1","src_num"}, {{"op",S_SETCFG},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},
    {{"s_setrf","rf_sel"}, {{"op",S_SETRF},{"dtype",VPU_INT32},{"alu",SCALAR_ALU}}},

};

VPUConverter::VPUConverter(std::shared_ptr<MFPara> para) : antoum::VPUConverter(para) {
}

std::map<std::string, std::vector<VPUInstrFormat*>> *VPUConverter::GetNameInstrMap() {
    static std::mutex mtx;
    static bool static_init = false;
    static std::map<std::string, std::vector<VPUInstrFormat*>> name_instr_map;

    if(static_init == false) {
        mtx.lock();
        if(static_init == false) {
            for(auto &i : g_vpu_instr_table_quark) {
                name_instr_map[i.names[0]].push_back(&i);
            }
            static_init = true;
        }
        mtx.unlock();
    }
    return &name_instr_map;
}

std::map<VPUOpcode, std::vector<VPUInstrFormat*>> *VPUConverter::GetOpInstrMap() {
    static std::mutex mtx;
    static bool static_init = false;
    static std::map<VPUOpcode, std::vector<VPUInstrFormat*>> op_instr_map;

    if(static_init == false) {
        mtx.lock();
        if(static_init == false) {
            for(auto &i : g_vpu_instr_table_quark) {
                op_instr_map[VPUOpcode(i.fixed_fields["op"])].push_back(&i);
            }
            static_init = true;
        }
        mtx.unlock();
    }
    return &op_instr_map;
}

bool VPUConverter::Check() {
    return antoum::VPUConverter::Check();
}

ConverterBase* VPUConverter::LoadText(std::string text) {
    return antoum::VPUConverter::LoadText(text);
}

std::string VPUConverter::ToText() {
    std::stringstream ss;
    VPUInstrFormat *instr_form = nullptr;
    auto &op_instr_map = *GetOpInstrMap();

    std::map<std::string, uint32_t> fields;
    std::dynamic_pointer_cast<quark::VPUInstruction>(_instr)->GetFields(fields);

    for(auto &i : op_instr_map[VPUOpcode(fields["op"])]) {
        bool flag = true;
        if(flag && VPUOpcode(fields["op2"]) != V_INVALID && i->fixed_fields.find("op2") == i->fixed_fields.end()) {
            flag = false;
        } else if(flag && VPUOpcode(fields["op3"]) != V_INVALID && i->fixed_fields.find("op3") == i->fixed_fields.end()) {
            flag = false;
        } else if(flag) {
            for(auto &j : i->fixed_fields) {
                if( fields.find(j.first) == fields.end() || j.second != fields[j.first]) {
                    flag = false;
                    break;
                }
            }
        }
        if(flag && (NULL == instr_form || instr_form->fixed_fields.size() < i->fixed_fields.size())) {
            instr_form = i;
        }
    }

    if(nullptr != instr_form) {
        ss << instr_form->names[0];
        for(int i = 1; i < (int)instr_form->names.size(); i++) {
            ss << " ";
            ss << fields[instr_form->names[i]];
        }
    }

    return ss.str();
}

ConverterBase* VPUConverter::LoadBinary(const std::vector<uint8_t> &bin) {
    CHECK(bin.size() == MemMapping.at(MM_VPU_CMD).cmd_width);
    auto instr = std::dynamic_pointer_cast<quark::VPUInstruction>(_instr);
    auto fields = std::dynamic_pointer_cast<quark::VPUInstrFields>(instr->fields);
    instr->SetFields({});

    if(0x00 == bin[3]) {
        fields->op = V_NOP;
    }
    else if(0xFF == bin[3]) {
        fields->op = V_OP_END;
    }
    else if(0x80 & bin[3]) {  // VPU M-type/R-type/I-type
        uint32_t cmd_bin = ((uint32_t)bin[3]<<24) | ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
        if(0x40 & bin[3]) {   // VPU M-type
            fields->offset = 0xFFFF & cmd_bin;
            fields->consecutive_reg = 0x1 & (cmd_bin>>18);

            auto get_compute_op = [&] () {
                switch(0x7 & (cmd_bin >> 24)) {
                case 0: return V_ADD;
                case 1: return V_SUB;
                case 2: return V_MUL;
                case 3: return V_MAX;
                case 4: return V_MIN;
                }
                return V_INVALID;
            };

            switch(0x3 & (cmd_bin>>28)) {
            case 0: fields->dtype = VPU_INT8;   break;
            case 1: fields->dtype = VPU_UINT8;  break;
            case 2: fields->dtype = VPU_BF16;   break;
            case 3: fields->dtype = VPU_FP32;   break;
            default:                            break;
            }

            if((0x1&(cmd_bin>>19)) && (0x1&(cmd_bin>>16))==0) { // super acceleration
                fields->alu = (0x1 & (cmd_bin>>27)) ?FP32_ALU :BF16_ALU;

                if(0x1 & (cmd_bin >> 17)) {                     // dual load
                    fields->op = V_LOAD_DUAL_GLB;
                }
                else {                                          // single load
                    fields->op = V_LOAD_GLB;
                    fields->rt = 0x03 & (cmd_bin >> 20);        // rs, select register file index , rt
                }
                fields->op2 = get_compute_op();
                fields->op3 = V_STORE_GLB;
            }
            else if(0x1&(cmd_bin>>16)) {                        // special acceleration
                fields->alu = (0x1 & (cmd_bin>>19)) ?FP32_ALU :BF16_ALU;
                fields->rd = 0x03 & (cmd_bin >> 22);            
                if(0x01 & (cmd_bin) >> 27) {                    // store
                    fields->op = get_compute_op();
                    fields->op2 = V_STORE_GLB;
                    fields->rs = 0x03 & (cmd_bin >> 20);
                }
                else {                                          // load
                    if(0x1 & (cmd_bin >> 17)) {                 // dual load
                        fields->op = V_LOAD_DUAL_GLB;
                    }
                    else {                                      // single load
                        fields->op = V_LOAD_GLB;
                        fields->rs = 0x03 & (cmd_bin >> 20);
                    }
                    fields->op2 = get_compute_op();
                }
            }
            else {                                              // normal load/store
                fields->alu = (0x1 & (cmd_bin>>26)) ?FP32_ALU :BF16_ALU;     ////
                fields->upper = 0x1 & (cmd_bin>>24);            //bf16 upper
                fields->rt = 0x03 & (cmd_bin>>20);              // rs, select register file index , rt
                if(0x01 & (cmd_bin>>27)) {                      // store
                    fields->op = V_STORE_GLB;
                }
                else {                                         // load
                    if(0x01 & (cmd_bin>>17)) {            // dual load
                        fields->op = V_LOAD_DUAL_GLB;
                        fields->rs = 0x03 & (cmd_bin >> 22);
                    }
                    else {                                     // single load
                        fields->op = V_LOAD_GLB;
                    }
                }
            }
        }
        else {                // VPU R-type/I-type
            fields->alu = (0x1 & (cmd_bin>>28)) ?FP32_ALU :BF16_ALU;
            fields->dtype = (0x1 & (cmd_bin>>28)) ?VPU_FP32 : VPU_BF16;

            fields->has_imm = 0x1 & (cmd_bin >> 29);
            if(fields->has_imm) {
                fields->imm = 0xFFFF & cmd_bin;
            }
            else {
                fields->rt = 0x7 & (cmd_bin >> 15);
            }
            fields->rs = 0x7 & (cmd_bin >> 18);
            fields->rd = 0x7 & (cmd_bin >> 21);

            switch(0xF & (cmd_bin >> 24)) {
            case 0x00: fields->op = V_ADD;            break;
            case 0x01: fields->op = V_SUB;            break;
            case 0x02: fields->op = V_MUL;            break;
            case 0x03: fields->op = V_MAX;            break;
            case 0x04: fields->op = V_MIN;            break;
            case 0x05: fields->op = V_MOV;            break;
            case 0x06: fields->op = V_REDUCE_SUM;     break;
            case 0x07: fields->op = V_GREATER;        break;
            case 0x08: fields->op = V_EQUAL;          break;
            case 0x09: fields->op = V_LESS;           break;
            case 0x0A: fields->op = V_GREATER_EQUAL;  break;
            case 0x0B: fields->op = V_LESS_EQUAL;     break;
            case 0x0C: fields->op = V_SEL;            break;
            case 0x0D: fields->op = V_LUI;            break;
            case 0x0E: fields->op = V_LI;             break;
            }
        }
    }
    else if(0x40 & bin[3]) {  // scalar
        uint32_t cmd_bin = ((uint32_t)bin[3]<<24) | ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
        fields->alu = SCALAR_ALU;
        fields->dtype = VPU_INT32;

        uint32_t funct = 0x3F & (cmd_bin>>24);
        if(funct <= 0xE) {
            fields->op = S_ADDI + funct;
            fields->rs = 0xF & (cmd_bin>>20);
            fields->rt = 0xF & (cmd_bin>>16);
            fields->imm = 0xFFFF & cmd_bin;
            fields->has_imm = true;
        }
        else if(funct >= 0x11 && funct <= 0x26) {
            fields->op = S_ADD + funct - 0x11;
            fields->rs = 0xF & (cmd_bin>>20);
            fields->rt = 0xF & (cmd_bin>>16);
            fields->rd = 0xF & (cmd_bin>>12);
        }
        else if(funct >= 0x29 && funct <= 0x31) {
            fields->op = S_JUMP + funct - 0x29;
            fields->rs = 0xF & (cmd_bin>>20);
            fields->rt = 0xF & (cmd_bin>>16);
            fields->offset = 0xFFFF & cmd_bin;
        }
        else if(funct == 0x34) {
            fields->op = S_SETCFG;
            fields->rs = 0xF & (cmd_bin>>20);
            fields->rt = 0xF & (cmd_bin>>16);
            fields->sp_rd0 = 0x1F & (cmd_bin>>11);
            fields->sp_rd1 = 0x1F & (cmd_bin>>6);
            fields->src_num = 0x1 & (cmd_bin>>5);
        }
        else if(funct == 0x35) {
            fields->op = S_SETRF;
            fields->rf_sel = 0x7 & cmd_bin;
        }
    }
    else { // configurable register
        uint32_t label = ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
        switch(0x7F & bin[3]) {
        case 1: {
            fields->op = SET_LOAD_BASE;
            fields->base_addr = label;
        }
        break;
        case 2: {
            fields->op = SET_LOAD_GLB;
            fields->stride = 0x7 & (label >> 21);
            fields->cluster_mask = 0xFF & (label >> 13);
            fields->bank_mask = 0xF & (label >> 9);
            fields->cluster_broadcast = 0x7 & (label >> 6);
            fields->bank_broadcast = 0x7 & (label >> 3);
            fields->channel_broadcast = 0x3 & (label >> 1);
        }
        break;
        case 3: {
            fields->op = SET_DEQUANTIZE;
            fields->dequantize = 0xFFFFFF & label;
        }
        break;
        case 4: {
            fields->op = SET_LOAD2_BASE;
            fields->base_addr = label;
        }
        break;
        case 6: {
            fields->op = SET_DEQUANTIZE2;
            fields->dequantize = 0xFFFFFF & label;
        }
        break;
        case 7: {
            fields->op = SET_STORE_BASE;
            fields->base_addr = label;
        }
        break;
        case 8: {
            fields->op = SET_STORE_GLB;
            fields->stride = 0x7 & (label >> 21);
            fields->cluster_mask = 0xFFFF & (label >> 5);
            fields->bank_mask = 0xF & (label >> 1);
        }
        break;
        case 9: {
            fields->op = SET_QUANTIZE;
            fields->quantize = 0xFFFF & label;
        }
        break;
        case 10: {
            fields->op = SET_FS;
            fields->fs_sel = 0xFF & label;
        }
        break;
        case 11: {
            fields->op = SET_CMASK_HIGH;
            fields->cluster_mask = 0xFF & (label >> 16);
        }
        break;
        }
    }

    instr->opcode = VPUOpcode(fields->op);
    instr->opcode2 = VPUOpcode(fields->op2);
    instr->opcode3 = VPUOpcode(fields->op3);
    return this;
}

std::vector<uint8_t> VPUConverter::ToBinary() {
    VPUBinary_t bin(MemMapping.at(MM_VPU_CMD).cmd_width, 0);
    auto instr = std::dynamic_pointer_cast<quark::VPUInstruction>(_instr);
    auto fields = std::dynamic_pointer_cast<quark::VPUInstrFields>(instr->fields);
    
    fields->op = instr->opcode;
    fields->op2 = instr->opcode2;
    fields->op3 = instr->opcode3;

    if(fields->op == V_NOP) {
        bin[3] = 0x00;
    }
    else if(fields->op == V_OP_END) {
        bin[3] = 0xFF;
    }
    else if(fields->op >= S_ADDI) {  // scalar
        uint32_t cmd_bin = 0x40000000;
        if(fields->op <= S_LUI) {
            cmd_bin |= (0x3F&(fields->op-S_ADDI)) << 24;
            cmd_bin |= (0xF&fields->rs) << 20;
            cmd_bin |= (0xF&fields->rt) << 16;
            cmd_bin |= (0xFFFF&fields->imm) << 0;
        }
        else if(fields->op <= S_SLL) {
            cmd_bin |= (0x3F&(0x11+fields->op-S_ADD)) << 24;
            cmd_bin |= (0xF&fields->rs) << 20;
            cmd_bin |= (0xF&fields->rt) << 16;
            cmd_bin |= (0xF&fields->rd) << 12;
        }
        else if(fields->op <= S_BGEU) {
            cmd_bin |= (0x3F&(0x29+fields->op-S_JUMP)) << 24;
            if(fields->op >= S_JR) {
                cmd_bin |= (0xF&fields->rs) << 20;
            }
            if(fields->op >= S_BNE) {
                cmd_bin |= (0xF&fields->rt) << 16;
            }
            if(fields->op != S_JR) {
                cmd_bin |= (0xFFFF&fields->offset) << 0;
            }
        }
        else if(fields->op == S_SETCFG) {
            cmd_bin |= (0x3F&(0x34)) << 24;
            cmd_bin |= (0xF&fields->rs) << 20;
            cmd_bin |= (0xF&fields->rt) << 16;
            cmd_bin |= (0x1F&fields->sp_rd0) << 11;
            cmd_bin |= (0x1F&fields->sp_rd1) << 6;
            cmd_bin |= (0x1&fields->src_num) << 5;
        }
        else if(fields->op == S_SETRF) {
            cmd_bin |= (0x3F&(0x35)) << 24;
            cmd_bin |= (0x7&fields->rf_sel);
        }

        bin[0] = 0xFF & (cmd_bin >> 0);
        bin[1] = 0xFF & (cmd_bin >> 8);
        bin[2] = 0xFF & (cmd_bin >> 16);
        bin[3] = 0xFF & (cmd_bin >> 24);
    }
    else if(fields->op >= V_LOAD_DUAL_GLB) {  // vpu
        uint32_t cmd_bin = 0x80000000;
        if(fields->op == V_LOAD_DUAL_GLB || fields->op == V_LOAD_GLB ||
            fields->op == V_LOAD_CONST || fields->op == V_STORE_GLB ||
            fields->op2 == V_STORE_GLB) { // load/store
            cmd_bin |= 0x40000000;
            cmd_bin |= 0xFFFF & (uint32_t)fields->offset;
            cmd_bin |= (0x01 & (uint32_t)fields->consecutive_reg) << 18;

            auto get_compute_op = [&] (VPUOpcode opcode) -> uint32_t {
                switch(opcode) {
                case V_ADD: return (0x7 & 0) << 24;
                case V_SUB: return (0x7 & 1) << 24;
                case V_MUL: return (0x7 & 2) << 24;
                case V_MAX: return (0x7 & 3) << 24;
                case V_MIN: return (0x7 & 4) << 24;
                default:    return 0;
                }
            };

            switch (fields->dtype)
            {
            case VPU_INT8:  cmd_bin |= (0x3 & 0) << 28;  break;
            case VPU_UINT8: cmd_bin |= (0x3 & 1) << 28;  break;
            case VPU_BF16:  cmd_bin |= (0x3 & 2) << 28;  break;
            case VPU_FP32:  cmd_bin |= (0x3 & 3) << 28;  break;
            default: break;
            }

            if(fields->op != V_INVALID && fields->op3 != V_INVALID) { // super acceleration
                cmd_bin |= 0x1 << 19;
                cmd_bin |= (0x01 & (uint32_t)((fields->alu==FP32_ALU) ?1 :0)) << 27;
                cmd_bin |= get_compute_op(VPUOpcode(fields->op2));

                if(fields->op == V_LOAD_DUAL_GLB) { // dual load
                    cmd_bin |= 0x1 << 17;
                }
                else { // single load
                    cmd_bin |= (0x03 & (uint32_t)fields->rt) << 20;
                }
            }
            else if(fields->op != V_INVALID && fields->op2 != V_INVALID) { // secial acceleration
                cmd_bin |= 0x1 << 16;
                cmd_bin |= (0x01 & (uint32_t)((fields->alu==FP32_ALU) ?1 :0)) << 19;
                cmd_bin |= (0x03 & (uint32_t)fields->rd) << 22;
    
                if(fields->op2 == V_STORE_GLB) { // store
                    cmd_bin |= 0x1 << 27;
                    cmd_bin |= get_compute_op(VPUOpcode(fields->op));
                    cmd_bin |= (0x03 & (uint32_t)fields->rs) << 20;
                }
                else { // load
                    if(fields->op == V_LOAD_DUAL_GLB) { // dual load
                       cmd_bin |= 0x1 << 17;
                    }
                    else { // single load
                        cmd_bin |= (0x03 & (uint32_t)fields->rs) << 20;
                    }
                    cmd_bin |= get_compute_op(VPUOpcode(fields->op2));
                }
            }
            else {
                cmd_bin |= (0x01 & (uint32_t)((fields->alu==FP32_ALU) ?1 :0)) << 26;
                cmd_bin |= (0x01 & (uint32_t)fields->upper) << 24;
                cmd_bin |= (0x03 & (uint32_t)fields->rt) << 20;

                if(fields->op == V_STORE_GLB) { // store
                    cmd_bin |= 0x1 << 27;
                    //cmd_bin |= (0x01 & (uint32_t)((fields->dtype==VPU_INT16) ?1 :0)) << 25;
                }
                else { // load
                    if(fields->op == V_LOAD_DUAL_GLB) { // dual load
                        cmd_bin |= 0x1 << 17;
                        cmd_bin |= (0x03 & (uint32_t)fields->rs) << 22;
                    }
                }
            }
        }
        else { // computation
            cmd_bin |= (0x1 & (uint32_t)((fields->alu==FP32_ALU) ?1 :0)) << 28;
            cmd_bin |= (0x1 & (uint32_t)fields->has_imm) << 29;
            if(fields->has_imm) {
                cmd_bin |= 0xFFFF & (uint32_t)fields->imm;
            }
            else {
                cmd_bin |= (0x7 & (uint32_t)fields->rt) << 15;
            }
            cmd_bin |= (0x7 & (uint32_t)fields->rs) << 18;
            cmd_bin |= (0x7 & (uint32_t)fields->rd) << 21;

            switch(fields->op) {
            case V_ADD:            cmd_bin |= 0x00 << 24; break;
            case V_SUB:            cmd_bin |= 0x01 << 24; break;
            case V_MUL:            cmd_bin |= 0x02 << 24; break;
            case V_MAX:            cmd_bin |= 0x03 << 24; break;
            case V_MIN:            cmd_bin |= 0x04 << 24; break;
            case V_MOV:            cmd_bin |= 0x05 << 24; break;
            case V_REDUCE_SUM:     cmd_bin |= 0x06 << 24; break;
            case V_GREATER:        cmd_bin |= 0x07 << 24; break;
            case V_EQUAL:          cmd_bin |= 0x08 << 24; break;
            case V_LESS:           cmd_bin |= 0x09 << 24; break;
            case V_GREATER_EQUAL:  cmd_bin |= 0x0A << 24; break;
            case V_LESS_EQUAL:     cmd_bin |= 0x0B << 24; break;
            case V_SEL:            cmd_bin |= 0x0C << 24; break;
            case V_LUI:            cmd_bin |= 0x0D << 24; break;
            case V_LI:             cmd_bin |= 0x0E << 24; break;
            }
        }
        bin[0] = 0xFF & (cmd_bin >> 0);
        bin[1] = 0xFF & (cmd_bin >> 8);
        bin[2] = 0xFF & (cmd_bin >> 16);
        bin[3] = 0xFF & (cmd_bin >> 24);
    }
    else { // configurable register
        uint32_t label = 0;
        switch(fields->op) {
        case SET_LOAD_BASE: {
            bin[3] |= 1;
            label = (uint32_t)fields->base_addr;
        }
        break;
        case SET_LOAD_GLB: {
            bin[3] |= 2;
            label |= (0x7 & (uint32_t)fields->stride) << 21;
            label |= (0xFF & (uint32_t)fields->cluster_mask) << 13;
            label |= (0xF & (uint32_t)fields->bank_mask) << 9;
            label |= (0x7 & (uint32_t)fields->cluster_broadcast) << 6;
            label |= (0x7 & (uint32_t)fields->bank_broadcast) << 3;
            label |= (0x3 & (uint32_t)fields->channel_broadcast) << 1;
        }
        break;
        case SET_DEQUANTIZE: {
            bin[3] |= 3;
            label |= 0xFFFFFF & (uint32_t)fields->dequantize;
        }
        break;
        case SET_LOAD2_BASE: {
            bin[3] |= 4;
            label = (uint32_t)fields->base_addr;
        }
        break;
        case SET_DEQUANTIZE2: {
            bin[3] |= 6;
            label |= 0xFFFFFF & (uint32_t)fields->dequantize;
        }
        break;
        case SET_STORE_BASE: {
            bin[3] |= 7;
            label = (uint32_t)fields->base_addr;
        }
        break;
        case SET_STORE_GLB: {
            bin[3] |= 8;
            label |= (0x7 & (uint32_t)fields->stride) << 21;
            label |= (0xFFFF & (uint32_t)fields->cluster_mask) << 5;
            label |= (0xF & (uint32_t)fields->bank_mask) << 1;
        }
        break;
        case SET_QUANTIZE: {
            bin[3] |= 9;
            label |= 0xFFFF & (uint32_t)fields->quantize;
        }
        break;
        case SET_FS: {
            bin[3] |= 10;
            label |= 0xFF & (uint32_t)fields->fs_sel;
        }
        break;
        case SET_CMASK_HIGH: {
            bin[3] |= 11;
            label |= (0xFF & (uint32_t)fields->cluster_mask) << 16;
        }
        break;
        }
        bin[0] = 0xFF & (label >> 0);
        bin[1] = 0xFF & (label >> 8);
        bin[2] = 0xFF & (label >> 16);
    }

    return bin;
}
}