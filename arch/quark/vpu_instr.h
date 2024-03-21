/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */
#ifndef QUARK_VPU_INSTR_H
#define QUARK_VPU_INSTR_H

#include <stdint.h>
#include <assert.h>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <sstream>
#include "instr_interface.h"

#ifndef VPU_GEN_GETTER_SETTER
#define VPU_GEN_GETTER_SETTER(field, high, low)\
  uint32_t get##field() { return (fields >> low) & (((uint32_t)1<<(high-low+1))-1); }\
  void set##field(uint32_t field) {\
    n##field = field;\
    uint32_t m = (((uint32_t)1<<(high-low+1))-1)<<low;\
    fields = (fields&(~m)) | (field<<low)&m; }
#endif

#ifndef VPU_PUSH_GETTER_SETTER
#define VPU_PUSH_GETTER_SETTER(field) do {\
  getters.push_back([this](){ return get##field(); });\
  setters.push_back([this](uint32_t x){ set##field(x); });\
} while(0)
#endif

extern void split(const std::string &text, std::vector<std::string> &v);

namespace quark {

enum class AluType {
  BF16_ALU = 0,
  SCALAR_ALU,
  FP32_ALU,
  INT32_ALU,
  FP16_ALU,
};

enum class DataMode {
  TENSOR = 0,
  WEIGHT_DENSE,
  WEIGHT_VALUE,
  WEIGHT_INDEX,
};


struct OpCodeItem {
  std::string name;
  uint32_t mask;
  uint32_t value;
};

class VpuInstr : public spu::InstrInterface {
public:
  enum class OpCodeType {
    VPU_SET_LOAD_BASE = 0,
    VPU_SET_LOAD2_BASE = 1,
    VPU_SET_STORE_BASE = 2,
    VPU_SET_LOAD_GLB = 3,
    VPU_SET_STORE_GLB = 4,
    VPU_SET_QUANTIZE = 5,
    VPU_SET_DEQUANTIZE = 6,
    VPU_SET_DEQUANTIZE2 = 7,
    VPU_SET_FS = 8,
    VPU_SET_CMASK_HIGH = 9,
    VPU_NOP = 10,
    VPU_OP_END = 11,
    VPU_LOAD_DUAL_GLB = 12,
    VPU_LOAD_GLB = 13,
    VPU_ULOAD_DUAL_GLB = 14,
    VPU_ULOAD_GLB = 15,
    VPU_FLOAD_DUAL_GLB = 16,
    VPU_FLOAD_DUAL_UPPER_GLB = 17,
    VPU_FLOAD_GLB = 18,
    VPU_FLOAD_UPPER_GLB = 19,
    VPU_E43LOAD_DUAL_GLB = 20,
    VPU_E43LOAD_GLB = 21,
    V32_I8LOAD_DUAL_GLB = 22,
    V32_I8LOAD_GLB = 23,
    V32_U8LOAD_DUAL_GLB = 24,
    V32_U8LOAD_GLB = 25,
    V32_F16LOAD_DUAL_GLB = 26,
    V32_F16LOAD_GLB = 27,
    V32_F32LOAD_DUAL_GLB = 28,
    V32_F32LOAD_GLB = 29,
    VPU_STORE_GLB = 30,
    VPU_USTORE_GLB = 31,
    VPU_FSTORE_GLB = 32,
    VPU_FSTORE_UPPER_GLB = 33,
    VPU_E43STORE_GLB = 34,
    V32_I8STORE_GLB = 35,
    V32_U8STORE_GLB = 36,
    V32_F16STORE_GLB = 37,
    V32_F32STORE_GLB = 38,
    VPU_FADD = 39,
    VPU_FSUB = 40,
    VPU_FMUL = 41,
    VPU_FMAX = 42,
    VPU_FMIN = 43,
    VPU_FMOV = 44,
    VPU_GREATER = 45,
    VPU_GREATER_EQUAL = 46,
    VPU_LESS = 47,
    VPU_LESS_EQUAL = 48,
    VPU_EQUAL = 49,
    VPU_SEL = 50,
    VPU_FADDI = 51,
    VPU_FSUBI = 52,
    VPU_FMULI = 53,
    VPU_FMAXI = 54,
    VPU_FMINI = 55,
    VPU_FMOVI = 56,
    VPU_GREATERI = 57,
    VPU_GREATER_EQUALI = 58,
    VPU_LESSI = 59,
    VPU_LESS_EQUALI = 60,
    VPU_EQUALI = 61,
    VPU_SELI = 62,
    VPU_FREDUCE_SUM = 63,
    V32_FADD = 64,
    V32_FSUB = 65,
    V32_FMUL = 66,
    V32_FMAX = 67,
    V32_FMIN = 68,
    V32_GREATER = 69,
    V32_GREATER_EQUAL = 70,
    V32_LESS = 71,
    V32_LESS_EQUAL = 72,
    V32_EQUAL = 73,
    V32_SEL = 74,
    V32_FADDI = 75,
    V32_FSUBI = 76,
    V32_FMULI = 77,
    V32_FMAXI = 78,
    V32_FMINI = 79,
    V32_FMOVI = 80,
    V32_GREATERI = 81,
    V32_GREATER_EQUALI = 82,
    V32_LESSI = 83,
    V32_LESS_EQUALI = 84,
    V32_EQUALI = 85,
    V32_SELI = 86,
    V32_LUI = 87,
    V32_LI = 88,
    VPU_LOAD_DUAL_FADD = 89,
    VPU_LOAD_DUAL_FSUB = 90,
    VPU_LOAD_DUAL_FMUL = 91,
    VPU_LOAD_DUAL_FMAX = 92,
    VPU_LOAD_DUAL_FMIN = 93,
    VPU_LOAD_FADD = 94,
    VPU_LOAD_FSUB = 95,
    VPU_LOAD_FMUL = 96,
    VPU_LOAD_FMAX = 97,
    VPU_LOAD_FMIN = 98,
    VPU_ULOAD_DUAL_FADD = 99,
    VPU_ULOAD_DUAL_FSUB = 100,
    VPU_ULOAD_DUAL_FMUL = 101,
    VPU_ULOAD_DUAL_FMAX = 102,
    VPU_ULOAD_DUAL_FMIN = 103,
    VPU_ULOAD_FADD = 104,
    VPU_ULOAD_FSUB = 105,
    VPU_ULOAD_FMUL = 106,
    VPU_ULOAD_FMAX = 107,
    VPU_ULOAD_FMIN = 108,
    VPU_FLOAD_DUAL_FADD = 109,
    VPU_FLOAD_DUAL_FSUB = 110,
    VPU_FLOAD_DUAL_FMUL = 111,
    VPU_FLOAD_DUAL_FMAX = 112,
    VPU_FLOAD_DUAL_FMIN = 113,
    VPU_FLOAD_FADD = 114,
    VPU_FLOAD_FSUB = 115,
    VPU_FLOAD_FMUL = 116,
    VPU_FLOAD_FMAX = 117,
    VPU_FLOAD_FMIN = 118,
    VPU_E43LOAD_DUAL_FADD = 119,
    VPU_E43LOAD_DUAL_FSUB = 120,
    VPU_E43LOAD_DUAL_FMUL = 121,
    VPU_E43LOAD_DUAL_FMAX = 122,
    VPU_E43LOAD_DUAL_FMIN = 123,
    VPU_E43LOAD_FADD = 124,
    VPU_E43LOAD_FSUB = 125,
    VPU_E43LOAD_FMUL = 126,
    VPU_E43LOAD_FMAX = 127,
    VPU_E43LOAD_FMIN = 128,
    V32_I8LOAD_DUAL_FADD = 129,
    V32_I8LOAD_DUAL_FSUB = 130,
    V32_I8LOAD_DUAL_FMUL = 131,
    V32_I8LOAD_DUAL_FMAX = 132,
    V32_I8LOAD_DUAL_FMIN = 133,
    V32_I8LOAD_FADD = 134,
    V32_I8LOAD_FSUB = 135,
    V32_I8LOAD_FMUL = 136,
    V32_I8LOAD_FMAX = 137,
    V32_I8LOAD_FMIN = 138,
    V32_U8LOAD_DUAL_FADD = 139,
    V32_U8LOAD_DUAL_FSUB = 140,
    V32_U8LOAD_DUAL_FMUL = 141,
    V32_U8LOAD_DUAL_FMAX = 142,
    V32_U8LOAD_DUAL_FMIN = 143,
    V32_U8LOAD_FADD = 144,
    V32_U8LOAD_FSUB = 145,
    V32_U8LOAD_FMUL = 146,
    V32_U8LOAD_FMAX = 147,
    V32_U8LOAD_FMIN = 148,
    V32_F16LOAD_DUAL_FADD = 149,
    V32_F16LOAD_DUAL_FSUB = 150,
    V32_F16LOAD_DUAL_FMUL = 151,
    V32_F16LOAD_DUAL_FMAX = 152,
    V32_F16LOAD_DUAL_FMIN = 153,
    V32_F16LOAD_FADD = 154,
    V32_F16LOAD_FSUB = 155,
    V32_F16LOAD_FMUL = 156,
    V32_F16LOAD_FMAX = 157,
    V32_F16LOAD_FMIN = 158,
    V32_F32LOAD_DUAL_FADD = 159,
    V32_F32LOAD_DUAL_FSUB = 160,
    V32_F32LOAD_DUAL_FMUL = 161,
    V32_F32LOAD_DUAL_FMAX = 162,
    V32_F32LOAD_DUAL_FMIN = 163,
    V32_F32LOAD_FADD = 164,
    V32_F32LOAD_FSUB = 165,
    V32_F32LOAD_FMUL = 166,
    V32_F32LOAD_FMAX = 167,
    V32_F32LOAD_FMIN = 168,
    VPU_FADD_STORE = 169,
    VPU_FSUB_STORE = 170,
    VPU_FMUL_STORE = 171,
    VPU_FMAX_STORE = 172,
    VPU_FMIN_STORE = 173,
    VPU_FADD_USTORE = 174,
    VPU_FSUB_USTORE = 175,
    VPU_FMUL_USTORE = 176,
    VPU_FMAX_USTORE = 177,
    VPU_FMIN_USTORE = 178,
    VPU_FADD_FSTORE = 179,
    VPU_FSUB_FSTORE = 180,
    VPU_FMUL_FSTORE = 181,
    VPU_FMAX_FSTORE = 182,
    VPU_FMIN_FSTORE = 183,
    VPU_FADD_E43STORE = 184,
    VPU_FSUB_E43STORE = 185,
    VPU_FMUL_E43STORE = 186,
    VPU_FMAX_E43STORE = 187,
    VPU_FMIN_E43STORE = 188,
    V32_FADD_I8STORE = 189,
    V32_FSUB_I8STORE = 190,
    V32_FMUL_I8STORE = 191,
    V32_FMAX_I8STORE = 192,
    V32_FMIN_I8STORE = 193,
    V32_FADD_U8STORE = 194,
    V32_FSUB_U8STORE = 195,
    V32_FMUL_U8STORE = 196,
    V32_FMAX_U8STORE = 197,
    V32_FMIN_U8STORE = 198,
    V32_FADD_F16STORE = 199,
    V32_FSUB_F16STORE = 200,
    V32_FMUL_F16STORE = 201,
    V32_FMAX_F16STORE = 202,
    V32_FMIN_F16STORE = 203,
    V32_FADD_F32STORE = 204,
    V32_FSUB_F32STORE = 205,
    V32_FMUL_F32STORE = 206,
    V32_FMAX_F32STORE = 207,
    V32_FMIN_F32STORE = 208,
    VPU_LOAD_DUAL_FADD_STORE = 209,
    VPU_LOAD_DUAL_FSUB_STORE = 210,
    VPU_LOAD_DUAL_FMUL_STORE = 211,
    VPU_LOAD_DUAL_FMAX_STORE = 212,
    VPU_LOAD_DUAL_FMIN_STORE = 213,
    VPU_LOAD_FADD_STORE = 214,
    VPU_LOAD_FSUB_STORE = 215,
    VPU_LOAD_FMUL_STORE = 216,
    VPU_LOAD_FMAX_STORE = 217,
    VPU_LOAD_FMIN_STORE = 218,
    VPU_ULOAD_DUAL_FADD_USTORE = 219,
    VPU_ULOAD_DUAL_FSUB_USTORE = 220,
    VPU_ULOAD_DUAL_FMUL_USTORE = 221,
    VPU_ULOAD_DUAL_FMAX_USTORE = 222,
    VPU_ULOAD_DUAL_FMIN_USTORE = 223,
    VPU_ULOAD_FADD_USTORE = 224,
    VPU_ULOAD_FSUB_USTORE = 225,
    VPU_ULOAD_FMUL_USTORE = 226,
    VPU_ULOAD_FMAX_USTORE = 227,
    VPU_ULOAD_FMIN_USTORE = 228,
    VPU_FLOAD_DUAL_FADD_FSTORE = 229,
    VPU_FLOAD_DUAL_FSUB_FSTORE = 230,
    VPU_FLOAD_DUAL_FMUL_FSTORE = 231,
    VPU_FLOAD_DUAL_FMAX_FSTORE = 232,
    VPU_FLOAD_DUAL_FMIN_FSTORE = 233,
    VPU_FLOAD_FADD_FSTORE = 234,
    VPU_FLOAD_FSUB_FSTORE = 235,
    VPU_FLOAD_FMUL_FSTORE = 236,
    VPU_FLOAD_FMAX_FSTORE = 237,
    VPU_FLOAD_FMIN_FSTORE = 238,
    VPU_E43LOAD_DUAL_FADD_E43STORE = 239,
    VPU_E43LOAD_DUAL_FSUB_E43STORE = 240,
    VPU_E43LOAD_DUAL_FMUL_E43STORE = 241,
    VPU_E43LOAD_DUAL_FMAX_E43STORE = 242,
    VPU_E43LOAD_DUAL_FMIN_E43STORE = 243,
    VPU_E43LOAD_FADD_E43STORE = 244,
    VPU_E43LOAD_FSUB_E43STORE = 245,
    VPU_E43LOAD_FMUL_E43STORE = 246,
    VPU_E43LOAD_FMAX_E43STORE = 247,
    VPU_E43LOAD_FMIN_E43STORE = 248,
    V32_I8LOAD_DUAL_FADD_I8STORE = 249,
    V32_I8LOAD_DUAL_FSUB_I8STORE = 250,
    V32_I8LOAD_DUAL_FMUL_I8STORE = 251,
    V32_I8LOAD_DUAL_FMAX_I8STORE = 252,
    V32_I8LOAD_DUAL_FMIN_I8STORE = 253,
    V32_I8LOAD_FADD_I8STORE = 254,
    V32_I8LOAD_FSUB_I8STORE = 255,
    V32_I8LOAD_FMUL_I8STORE = 256,
    V32_I8LOAD_FMAX_I8STORE = 257,
    V32_I8LOAD_FMIN_I8STORE = 258,
    V32_U8LOAD_DUAL_FADD_U8STORE = 259,
    V32_U8LOAD_DUAL_FSUB_U8STORE = 260,
    V32_U8LOAD_DUAL_FMUL_U8STORE = 261,
    V32_U8LOAD_DUAL_FMAX_U8STORE = 262,
    V32_U8LOAD_DUAL_FMIN_U8STORE = 263,
    V32_U8LOAD_FADD_U8STORE = 264,
    V32_U8LOAD_FSUB_U8STORE = 265,
    V32_U8LOAD_FMUL_U8STORE = 266,
    V32_U8LOAD_FMAX_U8STORE = 267,
    V32_U8LOAD_FMIN_U8STORE = 268,
    V32_F16LOAD_DUAL_FADD_F16STORE = 269,
    V32_F16LOAD_DUAL_FSUB_F16STORE = 270,
    V32_F16LOAD_DUAL_FMUL_F16STORE = 271,
    V32_F16LOAD_DUAL_FMAX_F16STORE = 272,
    V32_F16LOAD_DUAL_FMIN_F16STORE = 273,
    V32_F16LOAD_FADD_F16STORE = 274,
    V32_F16LOAD_FSUB_F16STORE = 275,
    V32_F16LOAD_FMUL_F16STORE = 276,
    V32_F16LOAD_FMAX_F16STORE = 277,
    V32_F16LOAD_FMIN_F16STORE = 278,
    V32_F32LOAD_DUAL_FADD_F32STORE = 279,
    V32_F32LOAD_DUAL_FSUB_F32STORE = 280,
    V32_F32LOAD_DUAL_FMUL_F32STORE = 281,
    V32_F32LOAD_DUAL_FMAX_F32STORE = 282,
    V32_F32LOAD_DUAL_FMIN_F32STORE = 283,
    V32_F32LOAD_FADD_F32STORE = 284,
    V32_F32LOAD_FSUB_F32STORE = 285,
    V32_F32LOAD_FMUL_F32STORE = 286,
    V32_F32LOAD_FMAX_F32STORE = 287,
    V32_F32LOAD_FMIN_F32STORE = 288,
    S_ADDI = 289,
    S_ADDIU = 290,
    S_SUBI = 291,
    S_ANDI = 292,
    S_ORI = 293,
    S_XORI = 294,
    S_SGTI = 295,
    S_SLTI = 296,
    S_SEQI = 297,
    S_SGEI = 298,
    S_SLEI = 299,
    S_SRAI = 300,
    S_SRLI = 301,
    S_SLLI = 302,
    S_LUI = 303,
    S_ADD = 304,
    S_ADDU = 305,
    S_SUB = 306,
    S_MUL = 307,
    S_MULH = 308,
    S_MULHU = 309,
    S_MIN = 310,
    S_MAX = 311,
    S_MINU = 312,
    S_MAXU = 313,
    S_AND = 314,
    S_OR = 315,
    S_XOR = 316,
    S_SGT = 317,
    S_SLT = 318,
    S_SEQ = 319,
    S_SGE = 320,
    S_SLE = 321,
    S_CMPSEL = 322,
    S_SRA = 323,
    S_SRL = 324,
    S_SLL = 325,
    S_JUMP = 326,
    S_JAL = 327,
    S_JR = 328,
    S_BNE = 329,
    S_BEQ = 330,
    S_BLT = 331,
    S_BLTU = 332,
    S_BGE = 333,
    S_BGEU = 334,
    S_SETCFG = 335,
    S_SETRF = 336,
    INVALID = 65536
  };

  static const std::map<OpCodeType, OpCodeItem>& getOpCodeMap() {
    static const std::map<OpCodeType, OpCodeItem> opCodeMap = {
      {OpCodeType::VPU_SET_LOAD_BASE,              {"vpu_set_load_base",              0xff000000, 0x01000000}},
      {OpCodeType::VPU_SET_LOAD2_BASE,             {"vpu_set_load2_base",             0xff000000, 0x04000000}},
      {OpCodeType::VPU_SET_STORE_BASE,             {"vpu_set_store_base",             0xff000000, 0x07000000}},
      {OpCodeType::VPU_SET_LOAD_GLB,               {"vpu_set_load_glb",               0xff000001, 0x02000000}},
      {OpCodeType::VPU_SET_STORE_GLB,              {"vpu_set_store_glb",              0xff000001, 0x08000000}},
      {OpCodeType::VPU_SET_QUANTIZE,               {"vpu_set_quantize",               0xffff0000, 0x09000000}},
      {OpCodeType::VPU_SET_DEQUANTIZE,             {"vpu_set_dequantize",             0xff000000, 0x03000000}},
      {OpCodeType::VPU_SET_DEQUANTIZE2,            {"vpu_set_dequantize2",            0xff000000, 0x06000000}},
      {OpCodeType::VPU_SET_FS,                     {"vpu_set_fs",                     0xffffff00, 0x0a000000}},
      {OpCodeType::VPU_SET_CMASK_HIGH,             {"vpu_set_cmask_high",             0xff00ffff, 0x0b000000}},
      {OpCodeType::VPU_NOP,                        {"vpu_nop",                        0xffffffff, 0x00000000}},
      {OpCodeType::VPU_OP_END,                     {"vpu_op_end",                     0xffffffff, 0xff000000}},
      {OpCodeType::VPU_LOAD_DUAL_GLB,              {"vpu_load_dual_glb",              0xff0f0000, 0xc0020000}},
      {OpCodeType::VPU_LOAD_GLB,                   {"vpu_load_glb",                   0xffcf0000, 0xc0000000}},
      {OpCodeType::VPU_ULOAD_DUAL_GLB,             {"vpu_uload_dual_glb",             0xff0f0000, 0xd0020000}},
      {OpCodeType::VPU_ULOAD_GLB,                  {"vpu_uload_glb",                  0xffcf0000, 0xd0000000}},
      {OpCodeType::VPU_FLOAD_DUAL_GLB,             {"vpu_fload_dual_glb",             0xff0b0000, 0xe0020000}},
      {OpCodeType::VPU_FLOAD_DUAL_UPPER_GLB,       {"vpu_fload_dual_upper_glb",       0xff0b0000, 0xe1020000}},
      {OpCodeType::VPU_FLOAD_GLB,                  {"vpu_fload_glb",                  0xffcb0000, 0xe0000000}},
      {OpCodeType::VPU_FLOAD_UPPER_GLB,            {"vpu_fload_upper_glb",            0xffcb0000, 0xe1000000}},
      {OpCodeType::VPU_E43LOAD_DUAL_GLB,           {"vpu_e43load_dual_glb",           0xff0f0000, 0xf0020000}},
      {OpCodeType::VPU_E43LOAD_GLB,                {"vpu_e43load_glb",                0xffcf0000, 0xf0000000}},
      {OpCodeType::V32_I8LOAD_DUAL_GLB,            {"v32_i8load_dual_glb",            0xff0f0000, 0xc4020000}},
      {OpCodeType::V32_I8LOAD_GLB,                 {"v32_i8load_glb",                 0xffcf0000, 0xc4000000}},
      {OpCodeType::V32_U8LOAD_DUAL_GLB,            {"v32_u8load_dual_glb",            0xff0f0000, 0xd4020000}},
      {OpCodeType::V32_U8LOAD_GLB,                 {"v32_u8load_glb",                 0xffcf0000, 0xd4000000}},
      {OpCodeType::V32_F16LOAD_DUAL_GLB,           {"v32_f16load_dual_glb",           0xff0f0000, 0xe4020000}},
      {OpCodeType::V32_F16LOAD_GLB,                {"v32_f16load_glb",                0xffcf0000, 0xe4000000}},
      {OpCodeType::V32_F32LOAD_DUAL_GLB,           {"v32_f32load_dual_glb",           0xff0f0000, 0xf4020000}},
      {OpCodeType::V32_F32LOAD_GLB,                {"v32_f32load_glb",                0xffcf0000, 0xf4000000}},
      {OpCodeType::VPU_STORE_GLB,                  {"vpu_store_glb",                  0xffcf0000, 0xc8000000}},
      {OpCodeType::VPU_USTORE_GLB,                 {"vpu_ustore_glb",                 0xffcf0000, 0xd8000000}},
      {OpCodeType::VPU_FSTORE_GLB,                 {"vpu_fstore_glb",                 0xffcb0000, 0xe8000000}},
      {OpCodeType::VPU_FSTORE_UPPER_GLB,           {"vpu_fstore_upper_glb",           0xffcb0000, 0xe9000000}},
      {OpCodeType::VPU_E43STORE_GLB,               {"vpu_e43store_glb",               0xffcf0000, 0xf8000000}},
      {OpCodeType::V32_I8STORE_GLB,                {"v32_i8store_glb",                0xffcf0000, 0xcc000000}},
      {OpCodeType::V32_U8STORE_GLB,                {"v32_u8store_glb",                0xffcf0000, 0xdc000000}},
      {OpCodeType::V32_F16STORE_GLB,               {"v32_f16store_glb",               0xffcf0000, 0xec000000}},
      {OpCodeType::V32_F32STORE_GLB,               {"v32_f32store_glb",               0xffcf0000, 0xfc000000}},
      {OpCodeType::VPU_FADD,                       {"vpu_fadd",                       0xff007fff, 0x80000000}},
      {OpCodeType::VPU_FSUB,                       {"vpu_fsub",                       0xff007fff, 0x81000000}},
      {OpCodeType::VPU_FMUL,                       {"vpu_fmul",                       0xff007fff, 0x82000000}},
      {OpCodeType::VPU_FMAX,                       {"vpu_fmax",                       0xff007fff, 0x83000000}},
      {OpCodeType::VPU_FMIN,                       {"vpu_fmin",                       0xff007fff, 0x84000000}},
      {OpCodeType::VPU_FMOV,                       {"vpu_fmov",                       0xff03ffff, 0x85000000}},
      {OpCodeType::VPU_GREATER,                    {"vpu_greater",                    0xffe07fff, 0x87000000}},
      {OpCodeType::VPU_GREATER_EQUAL,              {"vpu_greater_equal",              0xffe07fff, 0x8a000000}},
      {OpCodeType::VPU_LESS,                       {"vpu_less",                       0xffe07fff, 0x89000000}},
      {OpCodeType::VPU_LESS_EQUAL,                 {"vpu_less_equal",                 0xffe07fff, 0x8b000000}},
      {OpCodeType::VPU_EQUAL,                      {"vpu_equal",                      0xffe07fff, 0x88000000}},
      {OpCodeType::VPU_SEL,                        {"vpu_sel",                        0xff007fff, 0x8c000000}},
      {OpCodeType::VPU_FADDI,                      {"vpu_faddi",                      0xff030000, 0xa0000000}},
      {OpCodeType::VPU_FSUBI,                      {"vpu_fsubi",                      0xff030000, 0xa1000000}},
      {OpCodeType::VPU_FMULI,                      {"vpu_fmuli",                      0xff030000, 0xa2000000}},
      {OpCodeType::VPU_FMAXI,                      {"vpu_fmaxi",                      0xff030000, 0xa3000000}},
      {OpCodeType::VPU_FMINI,                      {"vpu_fmini",                      0xff030000, 0xa4000000}},
      {OpCodeType::VPU_FMOVI,                      {"vpu_fmovi",                      0xff1f0000, 0xa5000000}},
      {OpCodeType::VPU_GREATERI,                   {"vpu_greateri",                   0xffe30000, 0xa7000000}},
      {OpCodeType::VPU_GREATER_EQUALI,             {"vpu_greater_equali",             0xffe30000, 0xaa000000}},
      {OpCodeType::VPU_LESSI,                      {"vpu_lessi",                      0xffe30000, 0xa9000000}},
      {OpCodeType::VPU_LESS_EQUALI,                {"vpu_less_equali",                0xffe30000, 0xab000000}},
      {OpCodeType::VPU_EQUALI,                     {"vpu_equali",                     0xffe30000, 0xa8000000}},
      {OpCodeType::VPU_SELI,                       {"vpu_seli",                       0xff030000, 0xac000000}},
      {OpCodeType::VPU_FREDUCE_SUM,                {"vpu_freduce_sum",                0xfffc7fff, 0x86000000}},
      {OpCodeType::V32_FADD,                       {"v32_fadd",                       0xff007fff, 0x90000000}},
      {OpCodeType::V32_FSUB,                       {"v32_fsub",                       0xff007fff, 0x91000000}},
      {OpCodeType::V32_FMUL,                       {"v32_fmul",                       0xff007fff, 0x92000000}},
      {OpCodeType::V32_FMAX,                       {"v32_fmax",                       0xff007fff, 0x93000000}},
      {OpCodeType::V32_FMIN,                       {"v32_fmin",                       0xff007fff, 0x94000000}},
      {OpCodeType::V32_GREATER,                    {"v32_greater",                    0xffe07fff, 0x97000000}},
      {OpCodeType::V32_GREATER_EQUAL,              {"v32_greater_equal",              0xffe07fff, 0x9a000000}},
      {OpCodeType::V32_LESS,                       {"v32_less",                       0xffe07fff, 0x99000000}},
      {OpCodeType::V32_LESS_EQUAL,                 {"v32_less_equal",                 0xffe07fff, 0x9b000000}},
      {OpCodeType::V32_EQUAL,                      {"v32_equal",                      0xffe07fff, 0x98000000}},
      {OpCodeType::V32_SEL,                        {"v32_sel",                        0xff007fff, 0x9c000000}},
      {OpCodeType::V32_FADDI,                      {"v32_faddi",                      0xff030000, 0xb0000000}},
      {OpCodeType::V32_FSUBI,                      {"v32_fsubi",                      0xff030000, 0xb1000000}},
      {OpCodeType::V32_FMULI,                      {"v32_fmuli",                      0xff030000, 0xb2000000}},
      {OpCodeType::V32_FMAXI,                      {"v32_fmaxi",                      0xff030000, 0xb3000000}},
      {OpCodeType::V32_FMINI,                      {"v32_fmini",                      0xff030000, 0xb4000000}},
      {OpCodeType::V32_FMOVI,                      {"v32_fmovi",                      0xff1f0000, 0xb5000000}},
      {OpCodeType::V32_GREATERI,                   {"v32_greateri",                   0xffe30000, 0xb7000000}},
      {OpCodeType::V32_GREATER_EQUALI,             {"v32_greater_equali",             0xffe30000, 0xba000000}},
      {OpCodeType::V32_LESSI,                      {"v32_lessi",                      0xffe30000, 0xb9000000}},
      {OpCodeType::V32_LESS_EQUALI,                {"v32_less_equali",                0xffe30000, 0xbb000000}},
      {OpCodeType::V32_EQUALI,                     {"v32_equali",                     0xffe30000, 0xb8000000}},
      {OpCodeType::V32_SELI,                       {"v32_seli",                       0xff030000, 0xbc000000}},
      {OpCodeType::V32_LUI,                        {"v32_lui",                        0xff1f0000, 0xbd000000}},
      {OpCodeType::V32_LI,                         {"v32_li",                         0xff1f0000, 0xbe000000}},
      {OpCodeType::VPU_LOAD_DUAL_FADD,             {"vpu_load_dual_fadd",             0xff3f0000, 0xc0030000}},
      {OpCodeType::VPU_LOAD_DUAL_FSUB,             {"vpu_load_dual_fsub",             0xff3f0000, 0xc1030000}},
      {OpCodeType::VPU_LOAD_DUAL_FMUL,             {"vpu_load_dual_fmul",             0xff3f0000, 0xc2030000}},
      {OpCodeType::VPU_LOAD_DUAL_FMAX,             {"vpu_load_dual_fmax",             0xff3f0000, 0xc3030000}},
      {OpCodeType::VPU_LOAD_DUAL_FMIN,             {"vpu_load_dual_fmin",             0xff3f0000, 0xc4030000}},
      {OpCodeType::VPU_LOAD_FADD,                  {"vpu_load_fadd",                  0xff0f0000, 0xc0010000}},
      {OpCodeType::VPU_LOAD_FSUB,                  {"vpu_load_fsub",                  0xff0f0000, 0xc1010000}},
      {OpCodeType::VPU_LOAD_FMUL,                  {"vpu_load_fmul",                  0xff0f0000, 0xc2010000}},
      {OpCodeType::VPU_LOAD_FMAX,                  {"vpu_load_fmax",                  0xff0f0000, 0xc3010000}},
      {OpCodeType::VPU_LOAD_FMIN,                  {"vpu_load_fmin",                  0xff0f0000, 0xc4010000}},
      {OpCodeType::VPU_ULOAD_DUAL_FADD,            {"vpu_uload_dual_fadd",            0xff3f0000, 0xd0030000}},
      {OpCodeType::VPU_ULOAD_DUAL_FSUB,            {"vpu_uload_dual_fsub",            0xff3f0000, 0xd1030000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMUL,            {"vpu_uload_dual_fmul",            0xff3f0000, 0xd2030000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMAX,            {"vpu_uload_dual_fmax",            0xff3f0000, 0xd3030000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMIN,            {"vpu_uload_dual_fmin",            0xff3f0000, 0xd4030000}},
      {OpCodeType::VPU_ULOAD_FADD,                 {"vpu_uload_fadd",                 0xff0f0000, 0xd0010000}},
      {OpCodeType::VPU_ULOAD_FSUB,                 {"vpu_uload_fsub",                 0xff0f0000, 0xd1010000}},
      {OpCodeType::VPU_ULOAD_FMUL,                 {"vpu_uload_fmul",                 0xff0f0000, 0xd2010000}},
      {OpCodeType::VPU_ULOAD_FMAX,                 {"vpu_uload_fmax",                 0xff0f0000, 0xd3010000}},
      {OpCodeType::VPU_ULOAD_FMIN,                 {"vpu_uload_fmin",                 0xff0f0000, 0xd4010000}},
      {OpCodeType::VPU_FLOAD_DUAL_FADD,            {"vpu_fload_dual_fadd",            0xff3b0000, 0xe0030000}},
      {OpCodeType::VPU_FLOAD_DUAL_FSUB,            {"vpu_fload_dual_fsub",            0xff3b0000, 0xe1030000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMUL,            {"vpu_fload_dual_fmul",            0xff3b0000, 0xe2030000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMAX,            {"vpu_fload_dual_fmax",            0xff3b0000, 0xe3030000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMIN,            {"vpu_fload_dual_fmin",            0xff3b0000, 0xe4030000}},
      {OpCodeType::VPU_FLOAD_FADD,                 {"vpu_fload_fadd",                 0xff0b0000, 0xe0010000}},
      {OpCodeType::VPU_FLOAD_FSUB,                 {"vpu_fload_fsub",                 0xff0b0000, 0xe1010000}},
      {OpCodeType::VPU_FLOAD_FMUL,                 {"vpu_fload_fmul",                 0xff0b0000, 0xe2010000}},
      {OpCodeType::VPU_FLOAD_FMAX,                 {"vpu_fload_fmax",                 0xff0b0000, 0xe3010000}},
      {OpCodeType::VPU_FLOAD_FMIN,                 {"vpu_fload_fmin",                 0xff0b0000, 0xe4010000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FADD,          {"vpu_e43load_dual_fadd",          0xff3f0000, 0xf0030000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FSUB,          {"vpu_e43load_dual_fsub",          0xff3f0000, 0xf1030000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FMUL,          {"vpu_e43load_dual_fmul",          0xff3f0000, 0xf2030000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FMAX,          {"vpu_e43load_dual_fmax",          0xff3f0000, 0xf3030000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FMIN,          {"vpu_e43load_dual_fmin",          0xff3f0000, 0xf4030000}},
      {OpCodeType::VPU_E43LOAD_FADD,               {"vpu_e43load_fadd",               0xff0f0000, 0xf0010000}},
      {OpCodeType::VPU_E43LOAD_FSUB,               {"vpu_e43load_fsub",               0xff0f0000, 0xf1010000}},
      {OpCodeType::VPU_E43LOAD_FMUL,               {"vpu_e43load_fmul",               0xff0f0000, 0xf2010000}},
      {OpCodeType::VPU_E43LOAD_FMAX,               {"vpu_e43load_fmax",               0xff0f0000, 0xf3010000}},
      {OpCodeType::VPU_E43LOAD_FMIN,               {"vpu_e43load_fmin",               0xff0f0000, 0xf4010000}},
      {OpCodeType::V32_I8LOAD_DUAL_FADD,           {"v32_i8load_dual_fadd",           0xff3f0000, 0xc00b0000}},
      {OpCodeType::V32_I8LOAD_DUAL_FSUB,           {"v32_i8load_dual_fsub",           0xff3f0000, 0xc10b0000}},
      {OpCodeType::V32_I8LOAD_DUAL_FMUL,           {"v32_i8load_dual_fmul",           0xff3f0000, 0xc20b0000}},
      {OpCodeType::V32_I8LOAD_DUAL_FMAX,           {"v32_i8load_dual_fmax",           0xff3f0000, 0xc30b0000}},
      {OpCodeType::V32_I8LOAD_DUAL_FMIN,           {"v32_i8load_dual_fmin",           0xff3f0000, 0xc40b0000}},
      {OpCodeType::V32_I8LOAD_FADD,                {"v32_i8load_fadd",                0xff0f0000, 0xc0090000}},
      {OpCodeType::V32_I8LOAD_FSUB,                {"v32_i8load_fsub",                0xff0f0000, 0xc1090000}},
      {OpCodeType::V32_I8LOAD_FMUL,                {"v32_i8load_fmul",                0xff0f0000, 0xc2090000}},
      {OpCodeType::V32_I8LOAD_FMAX,                {"v32_i8load_fmax",                0xff0f0000, 0xc3090000}},
      {OpCodeType::V32_I8LOAD_FMIN,                {"v32_i8load_fmin",                0xff0f0000, 0xc4090000}},
      {OpCodeType::V32_U8LOAD_DUAL_FADD,           {"v32_u8load_dual_fadd",           0xff3f0000, 0xd00b0000}},
      {OpCodeType::V32_U8LOAD_DUAL_FSUB,           {"v32_u8load_dual_fsub",           0xff3f0000, 0xd10b0000}},
      {OpCodeType::V32_U8LOAD_DUAL_FMUL,           {"v32_u8load_dual_fmul",           0xff3f0000, 0xd20b0000}},
      {OpCodeType::V32_U8LOAD_DUAL_FMAX,           {"v32_u8load_dual_fmax",           0xff3f0000, 0xd30b0000}},
      {OpCodeType::V32_U8LOAD_DUAL_FMIN,           {"v32_u8load_dual_fmin",           0xff3f0000, 0xd40b0000}},
      {OpCodeType::V32_U8LOAD_FADD,                {"v32_u8load_fadd",                0xff0f0000, 0xd0090000}},
      {OpCodeType::V32_U8LOAD_FSUB,                {"v32_u8load_fsub",                0xff0f0000, 0xd1090000}},
      {OpCodeType::V32_U8LOAD_FMUL,                {"v32_u8load_fmul",                0xff0f0000, 0xd2090000}},
      {OpCodeType::V32_U8LOAD_FMAX,                {"v32_u8load_fmax",                0xff0f0000, 0xd3090000}},
      {OpCodeType::V32_U8LOAD_FMIN,                {"v32_u8load_fmin",                0xff0f0000, 0xd4090000}},
      {OpCodeType::V32_F16LOAD_DUAL_FADD,          {"v32_f16load_dual_fadd",          0xff3f0000, 0xe00b0000}},
      {OpCodeType::V32_F16LOAD_DUAL_FSUB,          {"v32_f16load_dual_fsub",          0xff3f0000, 0xe10b0000}},
      {OpCodeType::V32_F16LOAD_DUAL_FMUL,          {"v32_f16load_dual_fmul",          0xff3f0000, 0xe20b0000}},
      {OpCodeType::V32_F16LOAD_DUAL_FMAX,          {"v32_f16load_dual_fmax",          0xff3f0000, 0xe30b0000}},
      {OpCodeType::V32_F16LOAD_DUAL_FMIN,          {"v32_f16load_dual_fmin",          0xff3f0000, 0xe40b0000}},
      {OpCodeType::V32_F16LOAD_FADD,               {"v32_f16load_fadd",               0xff0f0000, 0xe0090000}},
      {OpCodeType::V32_F16LOAD_FSUB,               {"v32_f16load_fsub",               0xff0f0000, 0xe1090000}},
      {OpCodeType::V32_F16LOAD_FMUL,               {"v32_f16load_fmul",               0xff0f0000, 0xe2090000}},
      {OpCodeType::V32_F16LOAD_FMAX,               {"v32_f16load_fmax",               0xff0f0000, 0xe3090000}},
      {OpCodeType::V32_F16LOAD_FMIN,               {"v32_f16load_fmin",               0xff0f0000, 0xe4090000}},
      {OpCodeType::V32_F32LOAD_DUAL_FADD,          {"v32_f32load_dual_fadd",          0xff3f0000, 0xf00b0000}},
      {OpCodeType::V32_F32LOAD_DUAL_FSUB,          {"v32_f32load_dual_fsub",          0xff3f0000, 0xf10b0000}},
      {OpCodeType::V32_F32LOAD_DUAL_FMUL,          {"v32_f32load_dual_fmul",          0xff3f0000, 0xf20b0000}},
      {OpCodeType::V32_F32LOAD_DUAL_FMAX,          {"v32_f32load_dual_fmax",          0xff3f0000, 0xf30b0000}},
      {OpCodeType::V32_F32LOAD_DUAL_FMIN,          {"v32_f32load_dual_fmin",          0xff3f0000, 0xf40b0000}},
      {OpCodeType::V32_F32LOAD_FADD,               {"v32_f32load_fadd",               0xff0f0000, 0xf0090000}},
      {OpCodeType::V32_F32LOAD_FSUB,               {"v32_f32load_fsub",               0xff0f0000, 0xf1090000}},
      {OpCodeType::V32_F32LOAD_FMUL,               {"v32_f32load_fmul",               0xff0f0000, 0xf2090000}},
      {OpCodeType::V32_F32LOAD_FMAX,               {"v32_f32load_fmax",               0xff0f0000, 0xf3090000}},
      {OpCodeType::V32_F32LOAD_FMIN,               {"v32_f32load_fmin",               0xff0f0000, 0xf4090000}},
      {OpCodeType::VPU_FADD_STORE,                 {"vpu_fadd_store",                 0xff0f0000, 0xc8010000}},
      {OpCodeType::VPU_FSUB_STORE,                 {"vpu_fsub_store",                 0xff0f0000, 0xc9010000}},
      {OpCodeType::VPU_FMUL_STORE,                 {"vpu_fmul_store",                 0xff0f0000, 0xca010000}},
      {OpCodeType::VPU_FMAX_STORE,                 {"vpu_fmax_store",                 0xff0f0000, 0xcb010000}},
      {OpCodeType::VPU_FMIN_STORE,                 {"vpu_fmin_store",                 0xff0f0000, 0xcc010000}},
      {OpCodeType::VPU_FADD_USTORE,                {"vpu_fadd_ustore",                0xff0f0000, 0xd8010000}},
      {OpCodeType::VPU_FSUB_USTORE,                {"vpu_fsub_ustore",                0xff0f0000, 0xd9010000}},
      {OpCodeType::VPU_FMUL_USTORE,                {"vpu_fmul_ustore",                0xff0f0000, 0xda010000}},
      {OpCodeType::VPU_FMAX_USTORE,                {"vpu_fmax_ustore",                0xff0f0000, 0xdb010000}},
      {OpCodeType::VPU_FMIN_USTORE,                {"vpu_fmin_ustore",                0xff0f0000, 0xdc010000}},
      {OpCodeType::VPU_FADD_FSTORE,                {"vpu_fadd_fstore",                0xff0b0000, 0xe8010000}},
      {OpCodeType::VPU_FSUB_FSTORE,                {"vpu_fsub_fstore",                0xff0b0000, 0xe9010000}},
      {OpCodeType::VPU_FMUL_FSTORE,                {"vpu_fmul_fstore",                0xff0b0000, 0xea010000}},
      {OpCodeType::VPU_FMAX_FSTORE,                {"vpu_fmax_fstore",                0xff0b0000, 0xeb010000}},
      {OpCodeType::VPU_FMIN_FSTORE,                {"vpu_fmin_fstore",                0xff0b0000, 0xec010000}},
      {OpCodeType::VPU_FADD_E43STORE,              {"vpu_fadd_e43store",              0xff0f0000, 0xf8010000}},
      {OpCodeType::VPU_FSUB_E43STORE,              {"vpu_fsub_e43store",              0xff0f0000, 0xf9010000}},
      {OpCodeType::VPU_FMUL_E43STORE,              {"vpu_fmul_e43store",              0xff0f0000, 0xfa010000}},
      {OpCodeType::VPU_FMAX_E43STORE,              {"vpu_fmax_e43store",              0xff0f0000, 0xfb010000}},
      {OpCodeType::VPU_FMIN_E43STORE,              {"vpu_fmin_e43store",              0xff0f0000, 0xfc010000}},
      {OpCodeType::V32_FADD_I8STORE,               {"v32_fadd_i8store",               0xff0f0000, 0xc8090000}},
      {OpCodeType::V32_FSUB_I8STORE,               {"v32_fsub_i8store",               0xff0f0000, 0xc9090000}},
      {OpCodeType::V32_FMUL_I8STORE,               {"v32_fmul_i8store",               0xff0f0000, 0xca090000}},
      {OpCodeType::V32_FMAX_I8STORE,               {"v32_fmax_i8store",               0xff0f0000, 0xcb090000}},
      {OpCodeType::V32_FMIN_I8STORE,               {"v32_fmin_i8store",               0xff0f0000, 0xcc090000}},
      {OpCodeType::V32_FADD_U8STORE,               {"v32_fadd_u8store",               0xff0f0000, 0xd8090000}},
      {OpCodeType::V32_FSUB_U8STORE,               {"v32_fsub_u8store",               0xff0f0000, 0xd9090000}},
      {OpCodeType::V32_FMUL_U8STORE,               {"v32_fmul_u8store",               0xff0f0000, 0xda090000}},
      {OpCodeType::V32_FMAX_U8STORE,               {"v32_fmax_u8store",               0xff0f0000, 0xdb090000}},
      {OpCodeType::V32_FMIN_U8STORE,               {"v32_fmin_u8store",               0xff0f0000, 0xdc090000}},
      {OpCodeType::V32_FADD_F16STORE,              {"v32_fadd_f16store",              0xff0f0000, 0xe8090000}},
      {OpCodeType::V32_FSUB_F16STORE,              {"v32_fsub_f16store",              0xff0f0000, 0xe9090000}},
      {OpCodeType::V32_FMUL_F16STORE,              {"v32_fmul_f16store",              0xff0f0000, 0xea090000}},
      {OpCodeType::V32_FMAX_F16STORE,              {"v32_fmax_f16store",              0xff0f0000, 0xeb090000}},
      {OpCodeType::V32_FMIN_F16STORE,              {"v32_fmin_f16store",              0xff0f0000, 0xec090000}},
      {OpCodeType::V32_FADD_F32STORE,              {"v32_fadd_f32store",              0xff0f0000, 0xf8090000}},
      {OpCodeType::V32_FSUB_F32STORE,              {"v32_fsub_f32store",              0xff0f0000, 0xf9090000}},
      {OpCodeType::V32_FMUL_F32STORE,              {"v32_fmul_f32store",              0xff0f0000, 0xfa090000}},
      {OpCodeType::V32_FMAX_F32STORE,              {"v32_fmax_f32store",              0xff0f0000, 0xfb090000}},
      {OpCodeType::V32_FMIN_F32STORE,              {"v32_fmin_f32store",              0xff0f0000, 0xfc090000}},
      {OpCodeType::VPU_LOAD_DUAL_FADD_STORE,       {"vpu_load_dual_fadd_store",       0xffff0000, 0xc00a0000}},
      {OpCodeType::VPU_LOAD_DUAL_FSUB_STORE,       {"vpu_load_dual_fsub_store",       0xffff0000, 0xc10a0000}},
      {OpCodeType::VPU_LOAD_DUAL_FMUL_STORE,       {"vpu_load_dual_fmul_store",       0xffff0000, 0xc20a0000}},
      {OpCodeType::VPU_LOAD_DUAL_FMAX_STORE,       {"vpu_load_dual_fmax_store",       0xffff0000, 0xc30a0000}},
      {OpCodeType::VPU_LOAD_DUAL_FMIN_STORE,       {"vpu_load_dual_fmin_store",       0xffff0000, 0xc40a0000}},
      {OpCodeType::VPU_LOAD_FADD_STORE,            {"vpu_load_fadd_store",            0xffcf0000, 0xc0080000}},
      {OpCodeType::VPU_LOAD_FSUB_STORE,            {"vpu_load_fsub_store",            0xffcf0000, 0xc1080000}},
      {OpCodeType::VPU_LOAD_FMUL_STORE,            {"vpu_load_fmul_store",            0xffcf0000, 0xc2080000}},
      {OpCodeType::VPU_LOAD_FMAX_STORE,            {"vpu_load_fmax_store",            0xffcf0000, 0xc3080000}},
      {OpCodeType::VPU_LOAD_FMIN_STORE,            {"vpu_load_fmin_store",            0xffcf0000, 0xc4080000}},
      {OpCodeType::VPU_ULOAD_DUAL_FADD_USTORE,     {"vpu_uload_dual_fadd_ustore",     0xffff0000, 0xd00a0000}},
      {OpCodeType::VPU_ULOAD_DUAL_FSUB_USTORE,     {"vpu_uload_dual_fsub_ustore",     0xffff0000, 0xd10a0000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMUL_USTORE,     {"vpu_uload_dual_fmul_ustore",     0xffff0000, 0xd20a0000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMAX_USTORE,     {"vpu_uload_dual_fmax_ustore",     0xffff0000, 0xd30a0000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMIN_USTORE,     {"vpu_uload_dual_fmin_ustore",     0xffff0000, 0xd40a0000}},
      {OpCodeType::VPU_ULOAD_FADD_USTORE,          {"vpu_uload_fadd_ustore",          0xffcf0000, 0xd0080000}},
      {OpCodeType::VPU_ULOAD_FSUB_USTORE,          {"vpu_uload_fsub_ustore",          0xffcf0000, 0xd1080000}},
      {OpCodeType::VPU_ULOAD_FMUL_USTORE,          {"vpu_uload_fmul_ustore",          0xffcf0000, 0xd2080000}},
      {OpCodeType::VPU_ULOAD_FMAX_USTORE,          {"vpu_uload_fmax_ustore",          0xffcf0000, 0xd3080000}},
      {OpCodeType::VPU_ULOAD_FMIN_USTORE,          {"vpu_uload_fmin_ustore",          0xffcf0000, 0xd4080000}},
      {OpCodeType::VPU_FLOAD_DUAL_FADD_FSTORE,     {"vpu_fload_dual_fadd_fstore",     0xffff0000, 0xe00a0000}},
      {OpCodeType::VPU_FLOAD_DUAL_FSUB_FSTORE,     {"vpu_fload_dual_fsub_fstore",     0xffff0000, 0xe10a0000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMUL_FSTORE,     {"vpu_fload_dual_fmul_fstore",     0xffff0000, 0xe20a0000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMAX_FSTORE,     {"vpu_fload_dual_fmax_fstore",     0xffff0000, 0xe30a0000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMIN_FSTORE,     {"vpu_fload_dual_fmin_fstore",     0xffff0000, 0xe40a0000}},
      {OpCodeType::VPU_FLOAD_FADD_FSTORE,          {"vpu_fload_fadd_fstore",          0xffcb0000, 0xe0080000}},
      {OpCodeType::VPU_FLOAD_FSUB_FSTORE,          {"vpu_fload_fsub_fstore",          0xffcb0000, 0xe1080000}},
      {OpCodeType::VPU_FLOAD_FMUL_FSTORE,          {"vpu_fload_fmul_fstore",          0xffcb0000, 0xe2080000}},
      {OpCodeType::VPU_FLOAD_FMAX_FSTORE,          {"vpu_fload_fmax_fstore",          0xffcb0000, 0xe3080000}},
      {OpCodeType::VPU_FLOAD_FMIN_FSTORE,          {"vpu_fload_fmin_fstore",          0xffcb0000, 0xe4080000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FADD_E43STORE, {"vpu_e43load_dual_fadd_e43store", 0xffff0000, 0xf00a0000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FSUB_E43STORE, {"vpu_e43load_dual_fsub_e43store", 0xffff0000, 0xf10a0000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FMUL_E43STORE, {"vpu_e43load_dual_fmul_e43store", 0xffff0000, 0xf20a0000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FMAX_E43STORE, {"vpu_e43load_dual_fmax_e43store", 0xffff0000, 0xf30a0000}},
      {OpCodeType::VPU_E43LOAD_DUAL_FMIN_E43STORE, {"vpu_e43load_dual_fmin_e43store", 0xffff0000, 0xf40a0000}},
      {OpCodeType::VPU_E43LOAD_FADD_E43STORE,      {"vpu_e43load_fadd_e43store",      0xffcf0000, 0xf0080000}},
      {OpCodeType::VPU_E43LOAD_FSUB_E43STORE,      {"vpu_e43load_fsub_e43store",      0xffcf0000, 0xf1080000}},
      {OpCodeType::VPU_E43LOAD_FMUL_E43STORE,      {"vpu_e43load_fmul_e43store",      0xffcf0000, 0xf2080000}},
      {OpCodeType::VPU_E43LOAD_FMAX_E43STORE,      {"vpu_e43load_fmax_e43store",      0xffcf0000, 0xf3080000}},
      {OpCodeType::VPU_E43LOAD_FMIN_E43STORE,      {"vpu_e43load_fmin_e43store",      0xffcf0000, 0xf4080000}},
      {OpCodeType::V32_I8LOAD_DUAL_FADD_I8STORE,   {"v32_i8load_dual_fadd_i8store",   0xffff0000, 0xc80a0000}},
      {OpCodeType::V32_I8LOAD_DUAL_FSUB_I8STORE,   {"v32_i8load_dual_fsub_i8store",   0xffff0000, 0xc90a0000}},
      {OpCodeType::V32_I8LOAD_DUAL_FMUL_I8STORE,   {"v32_i8load_dual_fmul_i8store",   0xffff0000, 0xca0a0000}},
      {OpCodeType::V32_I8LOAD_DUAL_FMAX_I8STORE,   {"v32_i8load_dual_fmax_i8store",   0xffff0000, 0xcb0a0000}},
      {OpCodeType::V32_I8LOAD_DUAL_FMIN_I8STORE,   {"v32_i8load_dual_fmin_i8store",   0xffff0000, 0xcc0a0000}},
      {OpCodeType::V32_I8LOAD_FADD_I8STORE,        {"v32_i8load_fadd_i8store",        0xffcf0000, 0xc8080000}},
      {OpCodeType::V32_I8LOAD_FSUB_I8STORE,        {"v32_i8load_fsub_i8store",        0xffcf0000, 0xc9080000}},
      {OpCodeType::V32_I8LOAD_FMUL_I8STORE,        {"v32_i8load_fmul_i8store",        0xffcf0000, 0xca080000}},
      {OpCodeType::V32_I8LOAD_FMAX_I8STORE,        {"v32_i8load_fmax_i8store",        0xffcf0000, 0xcb080000}},
      {OpCodeType::V32_I8LOAD_FMIN_I8STORE,        {"v32_i8load_fmin_i8store",        0xffcf0000, 0xcc080000}},
      {OpCodeType::V32_U8LOAD_DUAL_FADD_U8STORE,   {"v32_u8load_dual_fadd_u8store",   0xffff0000, 0xd80a0000}},
      {OpCodeType::V32_U8LOAD_DUAL_FSUB_U8STORE,   {"v32_u8load_dual_fsub_u8store",   0xffff0000, 0xd90a0000}},
      {OpCodeType::V32_U8LOAD_DUAL_FMUL_U8STORE,   {"v32_u8load_dual_fmul_u8store",   0xffff0000, 0xda0a0000}},
      {OpCodeType::V32_U8LOAD_DUAL_FMAX_U8STORE,   {"v32_u8load_dual_fmax_u8store",   0xffff0000, 0xdb0a0000}},
      {OpCodeType::V32_U8LOAD_DUAL_FMIN_U8STORE,   {"v32_u8load_dual_fmin_u8store",   0xffff0000, 0xdc0a0000}},
      {OpCodeType::V32_U8LOAD_FADD_U8STORE,        {"v32_u8load_fadd_u8store",        0xffcf0000, 0xd8080000}},
      {OpCodeType::V32_U8LOAD_FSUB_U8STORE,        {"v32_u8load_fsub_u8store",        0xffcf0000, 0xd9080000}},
      {OpCodeType::V32_U8LOAD_FMUL_U8STORE,        {"v32_u8load_fmul_u8store",        0xffcf0000, 0xda080000}},
      {OpCodeType::V32_U8LOAD_FMAX_U8STORE,        {"v32_u8load_fmax_u8store",        0xffcf0000, 0xdb080000}},
      {OpCodeType::V32_U8LOAD_FMIN_U8STORE,        {"v32_u8load_fmin_u8store",        0xffcf0000, 0xdc080000}},
      {OpCodeType::V32_F16LOAD_DUAL_FADD_F16STORE, {"v32_f16load_dual_fadd_f16store", 0xffff0000, 0xe80a0000}},
      {OpCodeType::V32_F16LOAD_DUAL_FSUB_F16STORE, {"v32_f16load_dual_fsub_f16store", 0xffff0000, 0xe90a0000}},
      {OpCodeType::V32_F16LOAD_DUAL_FMUL_F16STORE, {"v32_f16load_dual_fmul_f16store", 0xffff0000, 0xea0a0000}},
      {OpCodeType::V32_F16LOAD_DUAL_FMAX_F16STORE, {"v32_f16load_dual_fmax_f16store", 0xffff0000, 0xeb0a0000}},
      {OpCodeType::V32_F16LOAD_DUAL_FMIN_F16STORE, {"v32_f16load_dual_fmin_f16store", 0xffff0000, 0xec0a0000}},
      {OpCodeType::V32_F16LOAD_FADD_F16STORE,      {"v32_f16load_fadd_f16store",      0xffcf0000, 0xe8080000}},
      {OpCodeType::V32_F16LOAD_FSUB_F16STORE,      {"v32_f16load_fsub_f16store",      0xffcf0000, 0xe9080000}},
      {OpCodeType::V32_F16LOAD_FMUL_F16STORE,      {"v32_f16load_fmul_f16store",      0xffcf0000, 0xea080000}},
      {OpCodeType::V32_F16LOAD_FMAX_F16STORE,      {"v32_f16load_fmax_f16store",      0xffcf0000, 0xeb080000}},
      {OpCodeType::V32_F16LOAD_FMIN_F16STORE,      {"v32_f16load_fmin_f16store",      0xffcf0000, 0xec080000}},
      {OpCodeType::V32_F32LOAD_DUAL_FADD_F32STORE, {"v32_f32load_dual_fadd_f32store", 0xffff0000, 0xf80a0000}},
      {OpCodeType::V32_F32LOAD_DUAL_FSUB_F32STORE, {"v32_f32load_dual_fsub_f32store", 0xffff0000, 0xf90a0000}},
      {OpCodeType::V32_F32LOAD_DUAL_FMUL_F32STORE, {"v32_f32load_dual_fmul_f32store", 0xffff0000, 0xfa0a0000}},
      {OpCodeType::V32_F32LOAD_DUAL_FMAX_F32STORE, {"v32_f32load_dual_fmax_f32store", 0xffff0000, 0xfb0a0000}},
      {OpCodeType::V32_F32LOAD_DUAL_FMIN_F32STORE, {"v32_f32load_dual_fmin_f32store", 0xffff0000, 0xfc0a0000}},
      {OpCodeType::V32_F32LOAD_FADD_F32STORE,      {"v32_f32load_fadd_f32store",      0xffcf0000, 0xf8080000}},
      {OpCodeType::V32_F32LOAD_FSUB_F32STORE,      {"v32_f32load_fsub_f32store",      0xffcf0000, 0xf9080000}},
      {OpCodeType::V32_F32LOAD_FMUL_F32STORE,      {"v32_f32load_fmul_f32store",      0xffcf0000, 0xfa080000}},
      {OpCodeType::V32_F32LOAD_FMAX_F32STORE,      {"v32_f32load_fmax_f32store",      0xffcf0000, 0xfb080000}},
      {OpCodeType::V32_F32LOAD_FMIN_F32STORE,      {"v32_f32load_fmin_f32store",      0xffcf0000, 0xfc080000}},
      {OpCodeType::S_ADDI,                         {"s_addi",                         0xff000000, 0x40000000}},
      {OpCodeType::S_ADDIU,                        {"s_addiu",                        0xff000000, 0x41000000}},
      {OpCodeType::S_SUBI,                         {"s_subi",                         0xff000000, 0x42000000}},
      {OpCodeType::S_ANDI,                         {"s_andi",                         0xff000000, 0x43000000}},
      {OpCodeType::S_ORI,                          {"s_ori",                          0xff000000, 0x44000000}},
      {OpCodeType::S_XORI,                         {"s_xori",                         0xff000000, 0x45000000}},
      {OpCodeType::S_SGTI,                         {"s_sgti",                         0xff000000, 0x46000000}},
      {OpCodeType::S_SLTI,                         {"s_slti",                         0xff000000, 0x47000000}},
      {OpCodeType::S_SEQI,                         {"s_seqi",                         0xff000000, 0x48000000}},
      {OpCodeType::S_SGEI,                         {"s_sgei",                         0xff000000, 0x49000000}},
      {OpCodeType::S_SLEI,                         {"s_slei",                         0xff000000, 0x4a000000}},
      {OpCodeType::S_SRAI,                         {"s_srai",                         0xff000000, 0x4b000000}},
      {OpCodeType::S_SRLI,                         {"s_srli",                         0xff000000, 0x4c000000}},
      {OpCodeType::S_SLLI,                         {"s_slli",                         0xff000000, 0x4d000000}},
      {OpCodeType::S_LUI,                          {"s_lui",                          0xfff00000, 0x4e000000}},
      {OpCodeType::S_ADD,                          {"s_add",                          0xff000fff, 0x51000000}},
      {OpCodeType::S_ADDU,                         {"s_addu",                         0xff000fff, 0x52000000}},
      {OpCodeType::S_SUB,                          {"s_sub",                          0xff000fff, 0x53000000}},
      {OpCodeType::S_MUL,                          {"s_mul",                          0xff000fff, 0x54000000}},
      {OpCodeType::S_MULH,                         {"s_mulh",                         0xff000fff, 0x55000000}},
      {OpCodeType::S_MULHU,                        {"s_mulhu",                        0xff000fff, 0x56000000}},
      {OpCodeType::S_MIN,                          {"s_min",                          0xff000fff, 0x57000000}},
      {OpCodeType::S_MAX,                          {"s_max",                          0xff000fff, 0x58000000}},
      {OpCodeType::S_MINU,                         {"s_minu",                         0xff000fff, 0x59000000}},
      {OpCodeType::S_MAXU,                         {"s_maxu",                         0xff000fff, 0x5a000000}},
      {OpCodeType::S_AND,                          {"s_and",                          0xff000fff, 0x5b000000}},
      {OpCodeType::S_OR,                           {"s_or",                           0xff000fff, 0x5c000000}},
      {OpCodeType::S_XOR,                          {"s_xor",                          0xff000fff, 0x5d000000}},
      {OpCodeType::S_SGT,                          {"s_sgt",                          0xff000fff, 0x5e000000}},
      {OpCodeType::S_SLT,                          {"s_slt",                          0xff000fff, 0x5f000000}},
      {OpCodeType::S_SEQ,                          {"s_seq",                          0xff000fff, 0x60000000}},
      {OpCodeType::S_SGE,                          {"s_sge",                          0xff000fff, 0x61000000}},
      {OpCodeType::S_SLE,                          {"s_sle",                          0xff000fff, 0x62000000}},
      {OpCodeType::S_CMPSEL,                       {"s_cmpsel",                       0xff000fff, 0x63000000}},
      {OpCodeType::S_SRA,                          {"s_sra",                          0xff000fff, 0x64000000}},
      {OpCodeType::S_SRL,                          {"s_srl",                          0xff000fff, 0x65000000}},
      {OpCodeType::S_SLL,                          {"s_sll",                          0xff000fff, 0x66000000}},
      {OpCodeType::S_JUMP,                         {"s_jump",                         0xffff0000, 0x69000000}},
      {OpCodeType::S_JAL,                          {"s_jal",                          0xffff0000, 0x6a000000}},
      {OpCodeType::S_JR,                           {"s_jr",                           0xff0fffff, 0x6b000000}},
      {OpCodeType::S_BNE,                          {"s_bne",                          0xff000000, 0x6c000000}},
      {OpCodeType::S_BEQ,                          {"s_beq",                          0xff000000, 0x6d000000}},
      {OpCodeType::S_BLT,                          {"s_blt",                          0xff000000, 0x6e000000}},
      {OpCodeType::S_BLTU,                         {"s_bltu",                         0xff000000, 0x6f000000}},
      {OpCodeType::S_BGE,                          {"s_bge",                          0xff000000, 0x70000000}},
      {OpCodeType::S_BGEU,                         {"s_bgeu",                         0xff000000, 0x71000000}},
      {OpCodeType::S_SETCFG,                       {"s_setcfg",                       0xff00001f, 0x74000000}},
      {OpCodeType::S_SETRF,                        {"s_setrf",                        0xfffffff8, 0x75000000}},
    };
    return opCodeMap;
  }

public:
  explicit VpuInstr(OpCodeType opCode) : 
      opCode(opCode), binary(4, 0) {
  }

  virtual ~VpuInstr() = default;

  static std::string opCodeToString(OpCodeType opCode)  {
    auto &opCodeMap = getOpCodeMap();
    if (opCodeMap.find(opCode) != opCodeMap.end())
      return opCodeMap.at(opCode).name;
    return "";
  }

  static OpCodeType stringToOpCode(std::string name) {
    auto &opCodeMap = getOpCodeMap();
    for (auto &it : opCodeMap)
      if (it.second.name == name)
        return it.first;
    return OpCodeType::INVALID;
  }

  OpCodeType getOpCode() {
    return opCode;
  }

  std::shared_ptr<spu::InstrInterface> clone() override {
    auto ret = create(opCode);
    ret->fields = fields;
    for(size_t i = 0; i < setters.size(); i++)
      ret->setters[i](getters[i]());
    return ret;
  }

  bool loadBinary(const std::vector<uint8_t>& bin) override {
    auto x = *reinterpret_cast<const uint32_t *>(bin.data());
    auto &item = getOpCodeMap().at(opCode);
    fields = item.value | (x & (~item.mask));
    for(size_t i = 0; i < setters.size(); i++)
      setters[i](getters[i]());
    return true;
  }

  const std::vector<uint8_t>& toBinary() override  {
    auto &item = getOpCodeMap().at(opCode);
    *reinterpret_cast<uint32_t *>(binary.data()) = 
        item.value | (fields & (~item.mask));
    return binary;
  }

  bool loadText(const std::string& text) override {
    std::vector<std::string> v;
    split(text, v);

    if (v.size() != (setters.size()+1))
      return false;
    if ((opCode = stringToOpCode(v[0])) == OpCodeType::INVALID)
      return false;

    for (size_t i = 0; i < setters.size(); i++)
      setters[i](std::strtoul(v[i+1].c_str(), 0, 10));
    return true;
  }

  const std::string& toText() override {
    std::stringstream ss;
    ss << opCodeToString(opCode);
    for (size_t i = 0; i < getters.size(); i++)
      ss << " " << getters[i]();
    text = ss.str();
    return text;
  }

  template <typename T>  
  static std::shared_ptr<T> create(const std::vector<size_t> &args) {
    std::shared_ptr<T> x(new T);
    assert(x->setters.size() == args.size());
    for (size_t i = 0; i < args.size(); ++i)
      x->setters[i](args[i]);
    return x;
  }

  static inline std::shared_ptr<VpuInstr> create(OpCodeType opCode, const std::vector<size_t> &args={});

  static std::shared_ptr<VpuInstr> create(const std::string& text) {
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

  static std::shared_ptr<VpuInstr> create(const std::vector<uint8_t>& bin) {
    auto x = *reinterpret_cast<const uint32_t *>(bin.data());
    for (auto &it : getOpCodeMap())
      if ((x & it.second.mask) == it.second.value) {
        auto ret = create(it.first);
        ret->loadBinary(bin);
        return ret;
      }
    return nullptr;
  }

protected:
  OpCodeType opCode;
  uint32_t fields = 0;
  std::vector<std::function<uint32_t(void)>> getters;
  std::vector<std::function<void(uint32_t)>> setters;
  std::string text;
  std::vector<uint8_t> binary;

public:
  uint32_t nAlu = 0;
  uint32_t nBankBroadcast = 0;
  uint32_t nBankMask = 0;
  uint32_t nBaseAddr = 0;
  uint32_t nChannelBroadcast = 0;
  uint32_t nClusterBroadcast = 0;
  uint32_t nClusterMask = 0;
  uint32_t nConsecutiveReg = 0;
  uint32_t nDequantize = 0;
  uint32_t nDtype = 0;
  uint32_t nFsSel = 0;
  uint32_t nHasImm = 0;
  uint32_t nImm = 0;
  uint32_t nOffset = 0;
  uint32_t nQuantize = 0;
  uint32_t nRd = 0;
  uint32_t nRfSel = 0;
  uint32_t nRs = 0;
  uint32_t nRt = 0;
  uint32_t nSpRd0 = 0;
  uint32_t nSpRd1 = 0;
  uint32_t nSrcNum = 0;
  uint32_t nStride = 0;
  uint32_t nUpper = 0;
};

class DualLoadComputeStoreInstr {
public:
  DualLoadComputeStoreInstr(VpuInstr::OpCodeType load, VpuInstr::OpCodeType compute, VpuInstr::OpCodeType store) : 
      load(load), compute(compute), store(store) {}
  virtual ~DualLoadComputeStoreInstr() = default;
  VpuInstr::OpCodeType load;
  VpuInstr::OpCodeType compute;
  VpuInstr::OpCodeType store;
};

class LoadComputeStoreInstr {
public:
  LoadComputeStoreInstr(VpuInstr::OpCodeType load, VpuInstr::OpCodeType compute, VpuInstr::OpCodeType store) : 
      load(load), compute(compute), store(store) {}
  virtual ~LoadComputeStoreInstr() = default;
  VpuInstr::OpCodeType load;
  VpuInstr::OpCodeType compute;
  VpuInstr::OpCodeType store;
};

class DualLoadComputeInstr {
public:
  DualLoadComputeInstr(VpuInstr::OpCodeType load, VpuInstr::OpCodeType compute) : 
      load(load), compute(compute) {}
  virtual ~DualLoadComputeInstr() = default;
  VpuInstr::OpCodeType load;
  VpuInstr::OpCodeType compute;
};

class LoadComputeInstr {
public:
  LoadComputeInstr(VpuInstr::OpCodeType load, VpuInstr::OpCodeType compute) : 
      load(load), compute(compute) {}
  virtual ~LoadComputeInstr() = default;
  VpuInstr::OpCodeType load;
  VpuInstr::OpCodeType compute;
};

class ComputeStoreInstr {
public:
  ComputeStoreInstr(VpuInstr::OpCodeType compute, VpuInstr::OpCodeType store) : 
      compute(compute), store(store) {}
  virtual ~ComputeStoreInstr() = default;
  VpuInstr::OpCodeType compute;
  VpuInstr::OpCodeType store;
};

class ScalarInstr {
public:
  virtual ~ScalarInstr() = default;
};

class VectorInstr {
public:
  virtual ~VectorInstr() = default;
};

class DualLoadInstr {
public:
  virtual ~DualLoadInstr() = default;
};

class LoadInstr {
public:
  virtual ~LoadInstr() = default;
};

class StoreInstr {
public:
  virtual ~StoreInstr() = default;
};

class VpuSetLoadBaseInstr : public VpuInstr {
public:
  explicit VpuSetLoadBaseInstr() : VpuInstr(OpCodeType::VPU_SET_LOAD_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class VpuSetLoad2BaseInstr : public VpuInstr {
public:
  explicit VpuSetLoad2BaseInstr() : VpuInstr(OpCodeType::VPU_SET_LOAD2_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class VpuSetStoreBaseInstr : public VpuInstr {
public:
  explicit VpuSetStoreBaseInstr() : VpuInstr(OpCodeType::VPU_SET_STORE_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class VpuSetLoadGlbInstr : public VpuInstr {
public:
  explicit VpuSetLoadGlbInstr() : VpuInstr(OpCodeType::VPU_SET_LOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Stride);
    VPU_PUSH_GETTER_SETTER(ClusterMask);
    VPU_PUSH_GETTER_SETTER(BankMask);
    VPU_PUSH_GETTER_SETTER(ClusterBroadcast);
    VPU_PUSH_GETTER_SETTER(BankBroadcast);
    VPU_PUSH_GETTER_SETTER(ChannelBroadcast);
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 13);
  VPU_GEN_GETTER_SETTER(BankMask, 12, 9);
  VPU_GEN_GETTER_SETTER(ClusterBroadcast, 8, 6);
  VPU_GEN_GETTER_SETTER(BankBroadcast, 5, 3);
  VPU_GEN_GETTER_SETTER(ChannelBroadcast, 2, 1);
};

class VpuSetStoreGlbInstr : public VpuInstr {
public:
  explicit VpuSetStoreGlbInstr() : VpuInstr(OpCodeType::VPU_SET_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Stride);
    VPU_PUSH_GETTER_SETTER(ClusterMask);
    VPU_PUSH_GETTER_SETTER(BankMask);
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 5);
  VPU_GEN_GETTER_SETTER(BankMask, 4, 1);
};

class VpuSetQuantizeInstr : public VpuInstr {
public:
  explicit VpuSetQuantizeInstr() : VpuInstr(OpCodeType::VPU_SET_QUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Quantize);
  }
  VPU_GEN_GETTER_SETTER(Quantize, 15, 0);
};

class VpuSetDequantizeInstr : public VpuInstr {
public:
  explicit VpuSetDequantizeInstr() : VpuInstr(OpCodeType::VPU_SET_DEQUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 23, 0);
};

class VpuSetDequantize2Instr : public VpuInstr {
public:
  explicit VpuSetDequantize2Instr() : VpuInstr(OpCodeType::VPU_SET_DEQUANTIZE2) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 23, 0);
};

class VpuSetFsInstr : public VpuInstr {
public:
  explicit VpuSetFsInstr() : VpuInstr(OpCodeType::VPU_SET_FS) {
    VPU_PUSH_GETTER_SETTER(FsSel);
  }
  VPU_GEN_GETTER_SETTER(FsSel, 7, 0);
};

class VpuSetCmaskHighInstr : public VpuInstr {
public:
  explicit VpuSetCmaskHighInstr() : VpuInstr(OpCodeType::VPU_SET_CMASK_HIGH) {
    VPU_PUSH_GETTER_SETTER(ClusterMask);
  }
  VPU_GEN_GETTER_SETTER(ClusterMask, 23, 16);
};

class VpuNopInstr : public VpuInstr {
public:
  explicit VpuNopInstr() : VpuInstr(OpCodeType::VPU_NOP) {
  }
};

class VpuOpEndInstr : public VpuInstr {
public:
  explicit VpuOpEndInstr() : VpuInstr(OpCodeType::VPU_OP_END) {
  }
};

class VpuLoadDualGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit VpuLoadDualGlbInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class VpuLoadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuLoadGlbInstr() : VpuInstr(OpCodeType::VPU_LOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadDualGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit VpuUloadDualGlbInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class VpuUloadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuUloadGlbInstr() : VpuInstr(OpCodeType::VPU_ULOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuFloadDualGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit VpuFloadDualGlbInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadDualUpperGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit VpuFloadDualUpperGlbInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_UPPER_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
    nUpper = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuFloadGlbInstr() : VpuInstr(OpCodeType::VPU_FLOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadUpperGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuFloadUpperGlbInstr() : VpuInstr(OpCodeType::VPU_FLOAD_UPPER_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
    nUpper = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuE43loadDualGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit VpuE43loadDualGlbInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class VpuE43loadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuE43loadGlbInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32I8loadDualGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit V32I8loadDualGlbInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class V32I8loadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit V32I8loadGlbInstr() : VpuInstr(OpCodeType::V32_I8LOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32U8loadDualGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit V32U8loadDualGlbInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class V32U8loadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit V32U8loadGlbInstr() : VpuInstr(OpCodeType::V32_U8LOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F16loadDualGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit V32F16loadDualGlbInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class V32F16loadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit V32F16loadGlbInstr() : VpuInstr(OpCodeType::V32_F16LOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F32loadDualGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit V32F32loadDualGlbInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class V32F32loadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit V32F32loadGlbInstr() : VpuInstr(OpCodeType::V32_F32LOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuStoreGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuStoreGlbInstr() : VpuInstr(OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUstoreGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuUstoreGlbInstr() : VpuInstr(OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFstoreGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuFstoreGlbInstr() : VpuInstr(OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFstoreUpperGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuFstoreUpperGlbInstr() : VpuInstr(OpCodeType::VPU_FSTORE_UPPER_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
    nUpper = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuE43storeGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuE43storeGlbInstr() : VpuInstr(OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32I8storeGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit V32I8storeGlbInstr() : VpuInstr(OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32U8storeGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit V32U8storeGlbInstr() : VpuInstr(OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F16storeGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit V32F16storeGlbInstr() : VpuInstr(OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F32storeGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit V32F32storeGlbInstr() : VpuInstr(OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFaddInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFaddInstr() : VpuInstr(OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFsubInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFsubInstr() : VpuInstr(OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFmulInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFmulInstr() : VpuInstr(OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFmaxInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFmaxInstr() : VpuInstr(OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFminInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFminInstr() : VpuInstr(OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFmovInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFmovInstr() : VpuInstr(OpCodeType::VPU_FMOV) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuGreaterInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuGreaterInstr() : VpuInstr(OpCodeType::VPU_GREATER) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuGreaterEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuGreaterEqualInstr() : VpuInstr(OpCodeType::VPU_GREATER_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuLessInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuLessInstr() : VpuInstr(OpCodeType::VPU_LESS) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuLessEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuLessEqualInstr() : VpuInstr(OpCodeType::VPU_LESS_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuEqualInstr() : VpuInstr(OpCodeType::VPU_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuSelInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuSelInstr() : VpuInstr(OpCodeType::VPU_SEL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFaddiInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFaddiInstr() : VpuInstr(OpCodeType::VPU_FADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFsubiInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFsubiInstr() : VpuInstr(OpCodeType::VPU_FSUBI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFmuliInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFmuliInstr() : VpuInstr(OpCodeType::VPU_FMULI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFmaxiInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFmaxiInstr() : VpuInstr(OpCodeType::VPU_FMAXI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFminiInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFminiInstr() : VpuInstr(OpCodeType::VPU_FMINI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFmoviInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFmoviInstr() : VpuInstr(OpCodeType::VPU_FMOVI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuGreateriInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuGreateriInstr() : VpuInstr(OpCodeType::VPU_GREATERI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class VpuGreaterEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuGreaterEqualiInstr() : VpuInstr(OpCodeType::VPU_GREATER_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class VpuLessiInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuLessiInstr() : VpuInstr(OpCodeType::VPU_LESSI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class VpuLessEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuLessEqualiInstr() : VpuInstr(OpCodeType::VPU_LESS_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class VpuEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuEqualiInstr() : VpuInstr(OpCodeType::VPU_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class VpuSeliInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuSeliInstr() : VpuInstr(OpCodeType::VPU_SELI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFreduceSumInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFreduceSumInstr() : VpuInstr(OpCodeType::VPU_FREDUCE_SUM) {
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class V32FaddInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FaddInstr() : VpuInstr(OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FsubInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FsubInstr() : VpuInstr(OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FmulInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FmulInstr() : VpuInstr(OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FmaxInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FmaxInstr() : VpuInstr(OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FminInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FminInstr() : VpuInstr(OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32GreaterInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32GreaterInstr() : VpuInstr(OpCodeType::V32_GREATER) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class V32GreaterEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32GreaterEqualInstr() : VpuInstr(OpCodeType::V32_GREATER_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class V32LessInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32LessInstr() : VpuInstr(OpCodeType::V32_LESS) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class V32LessEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32LessEqualInstr() : VpuInstr(OpCodeType::V32_LESS_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class V32EqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32EqualInstr() : VpuInstr(OpCodeType::V32_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class V32SelInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32SelInstr() : VpuInstr(OpCodeType::V32_SEL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FaddiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FaddiInstr() : VpuInstr(OpCodeType::V32_FADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FsubiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FsubiInstr() : VpuInstr(OpCodeType::V32_FSUBI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FmuliInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FmuliInstr() : VpuInstr(OpCodeType::V32_FMULI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FmaxiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FmaxiInstr() : VpuInstr(OpCodeType::V32_FMAXI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FminiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FminiInstr() : VpuInstr(OpCodeType::V32_FMINI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32FmoviInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32FmoviInstr() : VpuInstr(OpCodeType::V32_FMOVI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32GreateriInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32GreateriInstr() : VpuInstr(OpCodeType::V32_GREATERI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class V32GreaterEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32GreaterEqualiInstr() : VpuInstr(OpCodeType::V32_GREATER_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class V32LessiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32LessiInstr() : VpuInstr(OpCodeType::V32_LESSI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class V32LessEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32LessEqualiInstr() : VpuInstr(OpCodeType::V32_LESS_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class V32EqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32EqualiInstr() : VpuInstr(OpCodeType::V32_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class V32SeliInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32SeliInstr() : VpuInstr(OpCodeType::V32_SELI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32LuiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32LuiInstr() : VpuInstr(OpCodeType::V32_LUI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class V32LiInstr : public VpuInstr, public VectorInstr {
public:
  explicit V32LiInstr() : VpuInstr(OpCodeType::V32_LI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuLoadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuLoadDualFaddInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuLoadDualFsubInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuLoadDualFmulInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuLoadDualFmaxInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuLoadDualFminInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuLoadFaddInstr() : VpuInstr(OpCodeType::VPU_LOAD_FADD), LoadComputeInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuLoadFsubInstr() : VpuInstr(OpCodeType::VPU_LOAD_FSUB), LoadComputeInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuLoadFmulInstr() : VpuInstr(OpCodeType::VPU_LOAD_FMUL), LoadComputeInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuLoadFmaxInstr() : VpuInstr(OpCodeType::VPU_LOAD_FMAX), LoadComputeInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuLoadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuLoadFminInstr() : VpuInstr(OpCodeType::VPU_LOAD_FMIN), LoadComputeInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFaddInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFsubInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFmulInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFmaxInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFminInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuUloadFaddInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FADD), LoadComputeInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuUloadFsubInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FSUB), LoadComputeInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuUloadFmulInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMUL), LoadComputeInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuUloadFmaxInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMAX), LoadComputeInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuUloadFminInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMIN), LoadComputeInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFaddInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFsubInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFmulInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFmaxInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFminInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFaddInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FADD), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFsubInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FSUB), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFmulInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMUL), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFmaxInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMAX), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFminInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMIN), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuE43loadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuE43loadDualFaddInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuE43loadDualFsubInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuE43loadDualFmulInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuE43loadDualFmaxInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuE43loadDualFminInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuE43loadFaddInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FADD), LoadComputeInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuE43loadFsubInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FSUB), LoadComputeInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuE43loadFmulInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FMUL), LoadComputeInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuE43loadFmaxInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FMAX), LoadComputeInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuE43loadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuE43loadFminInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FMIN), LoadComputeInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32I8loadDualFaddInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32I8loadDualFsubInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32I8loadDualFmulInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32I8loadDualFmaxInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32I8loadDualFminInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32I8loadFaddInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FADD), LoadComputeInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32I8loadFsubInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FSUB), LoadComputeInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32I8loadFmulInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FMUL), LoadComputeInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32I8loadFmaxInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FMAX), LoadComputeInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32I8loadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32I8loadFminInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FMIN), LoadComputeInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32U8loadDualFaddInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32U8loadDualFsubInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32U8loadDualFmulInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32U8loadDualFmaxInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32U8loadDualFminInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32U8loadFaddInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FADD), LoadComputeInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32U8loadFsubInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FSUB), LoadComputeInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32U8loadFmulInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FMUL), LoadComputeInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32U8loadFmaxInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FMAX), LoadComputeInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32U8loadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32U8loadFminInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FMIN), LoadComputeInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F16loadDualFaddInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F16loadDualFsubInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F16loadDualFmulInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F16loadDualFmaxInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F16loadDualFminInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F16loadFaddInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FADD), LoadComputeInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F16loadFsubInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FSUB), LoadComputeInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F16loadFmulInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FMUL), LoadComputeInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F16loadFmaxInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FMAX), LoadComputeInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F16loadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F16loadFminInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FMIN), LoadComputeInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F32loadDualFaddInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F32loadDualFsubInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F32loadDualFmulInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F32loadDualFmaxInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit V32F32loadDualFminInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F32loadFaddInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FADD), LoadComputeInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F32loadFsubInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FSUB), LoadComputeInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F32loadFmulInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FMUL), LoadComputeInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F32loadFmaxInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FMAX), LoadComputeInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class V32F32loadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit V32F32loadFminInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FMIN), LoadComputeInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFaddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFaddStoreInstr() : VpuInstr(OpCodeType::VPU_FADD_STORE), ComputeStoreInstr(OpCodeType::VPU_FADD, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFsubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFsubStoreInstr() : VpuInstr(OpCodeType::VPU_FSUB_STORE), ComputeStoreInstr(OpCodeType::VPU_FSUB, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFmulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmulStoreInstr() : VpuInstr(OpCodeType::VPU_FMUL_STORE), ComputeStoreInstr(OpCodeType::VPU_FMUL, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFmaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmaxStoreInstr() : VpuInstr(OpCodeType::VPU_FMAX_STORE), ComputeStoreInstr(OpCodeType::VPU_FMAX, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFminStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFminStoreInstr() : VpuInstr(OpCodeType::VPU_FMIN_STORE), ComputeStoreInstr(OpCodeType::VPU_FMIN, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFaddUstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFaddUstoreInstr() : VpuInstr(OpCodeType::VPU_FADD_USTORE), ComputeStoreInstr(OpCodeType::VPU_FADD, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFsubUstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFsubUstoreInstr() : VpuInstr(OpCodeType::VPU_FSUB_USTORE), ComputeStoreInstr(OpCodeType::VPU_FSUB, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFmulUstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmulUstoreInstr() : VpuInstr(OpCodeType::VPU_FMUL_USTORE), ComputeStoreInstr(OpCodeType::VPU_FMUL, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFmaxUstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmaxUstoreInstr() : VpuInstr(OpCodeType::VPU_FMAX_USTORE), ComputeStoreInstr(OpCodeType::VPU_FMAX, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFminUstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFminUstoreInstr() : VpuInstr(OpCodeType::VPU_FMIN_USTORE), ComputeStoreInstr(OpCodeType::VPU_FMIN, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFaddFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFaddFstoreInstr() : VpuInstr(OpCodeType::VPU_FADD_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FADD, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFsubFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFsubFstoreInstr() : VpuInstr(OpCodeType::VPU_FSUB_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FSUB, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFmulFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmulFstoreInstr() : VpuInstr(OpCodeType::VPU_FMUL_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FMUL, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFmaxFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmaxFstoreInstr() : VpuInstr(OpCodeType::VPU_FMAX_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FMAX, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFminFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFminFstoreInstr() : VpuInstr(OpCodeType::VPU_FMIN_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FMIN, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFaddE43storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFaddE43storeInstr() : VpuInstr(OpCodeType::VPU_FADD_E43STORE), ComputeStoreInstr(OpCodeType::VPU_FADD, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFsubE43storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFsubE43storeInstr() : VpuInstr(OpCodeType::VPU_FSUB_E43STORE), ComputeStoreInstr(OpCodeType::VPU_FSUB, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFmulE43storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmulE43storeInstr() : VpuInstr(OpCodeType::VPU_FMUL_E43STORE), ComputeStoreInstr(OpCodeType::VPU_FMUL, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFmaxE43storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmaxE43storeInstr() : VpuInstr(OpCodeType::VPU_FMAX_E43STORE), ComputeStoreInstr(OpCodeType::VPU_FMAX, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFminE43storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFminE43storeInstr() : VpuInstr(OpCodeType::VPU_FMIN_E43STORE), ComputeStoreInstr(OpCodeType::VPU_FMIN, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FaddI8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FaddI8storeInstr() : VpuInstr(OpCodeType::V32_FADD_I8STORE), ComputeStoreInstr(OpCodeType::V32_FADD, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FsubI8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FsubI8storeInstr() : VpuInstr(OpCodeType::V32_FSUB_I8STORE), ComputeStoreInstr(OpCodeType::V32_FSUB, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FmulI8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FmulI8storeInstr() : VpuInstr(OpCodeType::V32_FMUL_I8STORE), ComputeStoreInstr(OpCodeType::V32_FMUL, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FmaxI8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FmaxI8storeInstr() : VpuInstr(OpCodeType::V32_FMAX_I8STORE), ComputeStoreInstr(OpCodeType::V32_FMAX, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FminI8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FminI8storeInstr() : VpuInstr(OpCodeType::V32_FMIN_I8STORE), ComputeStoreInstr(OpCodeType::V32_FMIN, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FaddU8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FaddU8storeInstr() : VpuInstr(OpCodeType::V32_FADD_U8STORE), ComputeStoreInstr(OpCodeType::V32_FADD, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FsubU8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FsubU8storeInstr() : VpuInstr(OpCodeType::V32_FSUB_U8STORE), ComputeStoreInstr(OpCodeType::V32_FSUB, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FmulU8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FmulU8storeInstr() : VpuInstr(OpCodeType::V32_FMUL_U8STORE), ComputeStoreInstr(OpCodeType::V32_FMUL, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FmaxU8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FmaxU8storeInstr() : VpuInstr(OpCodeType::V32_FMAX_U8STORE), ComputeStoreInstr(OpCodeType::V32_FMAX, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FminU8storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FminU8storeInstr() : VpuInstr(OpCodeType::V32_FMIN_U8STORE), ComputeStoreInstr(OpCodeType::V32_FMIN, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FaddF16storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FaddF16storeInstr() : VpuInstr(OpCodeType::V32_FADD_F16STORE), ComputeStoreInstr(OpCodeType::V32_FADD, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FsubF16storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FsubF16storeInstr() : VpuInstr(OpCodeType::V32_FSUB_F16STORE), ComputeStoreInstr(OpCodeType::V32_FSUB, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FmulF16storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FmulF16storeInstr() : VpuInstr(OpCodeType::V32_FMUL_F16STORE), ComputeStoreInstr(OpCodeType::V32_FMUL, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FmaxF16storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FmaxF16storeInstr() : VpuInstr(OpCodeType::V32_FMAX_F16STORE), ComputeStoreInstr(OpCodeType::V32_FMAX, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FminF16storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FminF16storeInstr() : VpuInstr(OpCodeType::V32_FMIN_F16STORE), ComputeStoreInstr(OpCodeType::V32_FMIN, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FaddF32storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FaddF32storeInstr() : VpuInstr(OpCodeType::V32_FADD_F32STORE), ComputeStoreInstr(OpCodeType::V32_FADD, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FsubF32storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FsubF32storeInstr() : VpuInstr(OpCodeType::V32_FSUB_F32STORE), ComputeStoreInstr(OpCodeType::V32_FSUB, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FmulF32storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FmulF32storeInstr() : VpuInstr(OpCodeType::V32_FMUL_F32STORE), ComputeStoreInstr(OpCodeType::V32_FMUL, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FmaxF32storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FmaxF32storeInstr() : VpuInstr(OpCodeType::V32_FMAX_F32STORE), ComputeStoreInstr(OpCodeType::V32_FMAX, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32FminF32storeInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit V32FminF32storeInstr() : VpuInstr(OpCodeType::V32_FMIN_F32STORE), ComputeStoreInstr(OpCodeType::V32_FMIN, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuLoadDualFaddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuLoadDualFaddStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FADD_STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuLoadDualFsubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuLoadDualFsubStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FSUB_STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuLoadDualFmulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuLoadDualFmulStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FMUL_STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuLoadDualFmaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuLoadDualFmaxStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FMAX_STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuLoadDualFminStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuLoadDualFminStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_DUAL_FMIN_STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_LOAD_DUAL_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuLoadFaddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuLoadFaddStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_FADD_STORE), LoadComputeStoreInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuLoadFsubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuLoadFsubStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_FSUB_STORE), LoadComputeStoreInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuLoadFmulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuLoadFmulStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_FMUL_STORE), LoadComputeStoreInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuLoadFmaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuLoadFmaxStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_FMAX_STORE), LoadComputeStoreInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuLoadFminStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuLoadFminStoreInstr() : VpuInstr(OpCodeType::VPU_LOAD_FMIN_STORE), LoadComputeStoreInstr(OpCodeType::VPU_LOAD_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadDualFaddUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFaddUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FADD_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFsubUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFsubUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FSUB_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFmulUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFmulUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMUL_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFmaxUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFmaxUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMAX_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFminUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFminUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMIN_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadFaddUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFaddUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FADD_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadFsubUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFsubUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FSUB_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadFmulUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFmulUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMUL_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadFmaxUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFmaxUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMAX_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadFminUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFminUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMIN_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuFloadDualFaddFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFaddFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FADD_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFsubFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFsubFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FSUB_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFmulFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFmulFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMUL_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFmaxFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFmaxFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMAX_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFminFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFminFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMIN_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadFaddFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFaddFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FADD_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFsubFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFsubFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FSUB_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFmulFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFmulFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMUL_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFmaxFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFmaxFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMAX_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuFloadFminFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFminFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMIN_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(ConsecutiveReg);
    nDtype = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(ConsecutiveReg, 18, 18);
};

class VpuE43loadDualFaddE43storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuE43loadDualFaddE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FADD_E43STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuE43loadDualFsubE43storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuE43loadDualFsubE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FSUB_E43STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuE43loadDualFmulE43storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuE43loadDualFmulE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FMUL_E43STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuE43loadDualFmaxE43storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuE43loadDualFmaxE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FMAX_E43STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuE43loadDualFminE43storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuE43loadDualFminE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_DUAL_FMIN_E43STORE), DualLoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_DUAL_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuE43loadFaddE43storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuE43loadFaddE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FADD_E43STORE), LoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuE43loadFsubE43storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuE43loadFsubE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FSUB_E43STORE), LoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuE43loadFmulE43storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuE43loadFmulE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FMUL_E43STORE), LoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuE43loadFmaxE43storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuE43loadFmaxE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FMAX_E43STORE), LoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuE43loadFminE43storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuE43loadFminE43storeInstr() : VpuInstr(OpCodeType::VPU_E43LOAD_FMIN_E43STORE), LoadComputeStoreInstr(OpCodeType::VPU_E43LOAD_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_E43STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32I8loadDualFaddI8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32I8loadDualFaddI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FADD_I8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FADD, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32I8loadDualFsubI8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32I8loadDualFsubI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FSUB_I8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FSUB, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32I8loadDualFmulI8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32I8loadDualFmulI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FMUL_I8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FMUL, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32I8loadDualFmaxI8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32I8loadDualFmaxI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FMAX_I8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FMAX, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32I8loadDualFminI8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32I8loadDualFminI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_DUAL_FMIN_I8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_I8LOAD_DUAL_GLB, OpCodeType::V32_FMIN, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32I8loadFaddI8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32I8loadFaddI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FADD_I8STORE), LoadComputeStoreInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FADD, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32I8loadFsubI8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32I8loadFsubI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FSUB_I8STORE), LoadComputeStoreInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FSUB, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32I8loadFmulI8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32I8loadFmulI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FMUL_I8STORE), LoadComputeStoreInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FMUL, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32I8loadFmaxI8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32I8loadFmaxI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FMAX_I8STORE), LoadComputeStoreInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FMAX, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32I8loadFminI8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32I8loadFminI8storeInstr() : VpuInstr(OpCodeType::V32_I8LOAD_FMIN_I8STORE), LoadComputeStoreInstr(OpCodeType::V32_I8LOAD_GLB, OpCodeType::V32_FMIN, OpCodeType::V32_I8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32U8loadDualFaddU8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32U8loadDualFaddU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FADD_U8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FADD, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32U8loadDualFsubU8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32U8loadDualFsubU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FSUB_U8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FSUB, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32U8loadDualFmulU8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32U8loadDualFmulU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FMUL_U8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FMUL, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32U8loadDualFmaxU8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32U8loadDualFmaxU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FMAX_U8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FMAX, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32U8loadDualFminU8storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32U8loadDualFminU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_DUAL_FMIN_U8STORE), DualLoadComputeStoreInstr(OpCodeType::V32_U8LOAD_DUAL_GLB, OpCodeType::V32_FMIN, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32U8loadFaddU8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32U8loadFaddU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FADD_U8STORE), LoadComputeStoreInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FADD, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32U8loadFsubU8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32U8loadFsubU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FSUB_U8STORE), LoadComputeStoreInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FSUB, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32U8loadFmulU8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32U8loadFmulU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FMUL_U8STORE), LoadComputeStoreInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FMUL, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32U8loadFmaxU8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32U8loadFmaxU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FMAX_U8STORE), LoadComputeStoreInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FMAX, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32U8loadFminU8storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32U8loadFminU8storeInstr() : VpuInstr(OpCodeType::V32_U8LOAD_FMIN_U8STORE), LoadComputeStoreInstr(OpCodeType::V32_U8LOAD_GLB, OpCodeType::V32_FMIN, OpCodeType::V32_U8STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F16loadDualFaddF16storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F16loadDualFaddF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FADD_F16STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FADD, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F16loadDualFsubF16storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F16loadDualFsubF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FSUB_F16STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FSUB, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F16loadDualFmulF16storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F16loadDualFmulF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FMUL_F16STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FMUL, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F16loadDualFmaxF16storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F16loadDualFmaxF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FMAX_F16STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FMAX, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F16loadDualFminF16storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F16loadDualFminF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_DUAL_FMIN_F16STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F16LOAD_DUAL_GLB, OpCodeType::V32_FMIN, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F16loadFaddF16storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F16loadFaddF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FADD_F16STORE), LoadComputeStoreInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FADD, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F16loadFsubF16storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F16loadFsubF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FSUB_F16STORE), LoadComputeStoreInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FSUB, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F16loadFmulF16storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F16loadFmulF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FMUL_F16STORE), LoadComputeStoreInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FMUL, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F16loadFmaxF16storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F16loadFmaxF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FMAX_F16STORE), LoadComputeStoreInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FMAX, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F16loadFminF16storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F16loadFminF16storeInstr() : VpuInstr(OpCodeType::V32_F16LOAD_FMIN_F16STORE), LoadComputeStoreInstr(OpCodeType::V32_F16LOAD_GLB, OpCodeType::V32_FMIN, OpCodeType::V32_F16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F32loadDualFaddF32storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F32loadDualFaddF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FADD_F32STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FADD, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F32loadDualFsubF32storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F32loadDualFsubF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FSUB_F32STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FSUB, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F32loadDualFmulF32storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F32loadDualFmulF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FMUL_F32STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FMUL, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F32loadDualFmaxF32storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F32loadDualFmaxF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FMAX_F32STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FMAX, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F32loadDualFminF32storeInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit V32F32loadDualFminF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_DUAL_FMIN_F32STORE), DualLoadComputeStoreInstr(OpCodeType::V32_F32LOAD_DUAL_GLB, OpCodeType::V32_FMIN, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class V32F32loadFaddF32storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F32loadFaddF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FADD_F32STORE), LoadComputeStoreInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FADD, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F32loadFsubF32storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F32loadFsubF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FSUB_F32STORE), LoadComputeStoreInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FSUB, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F32loadFmulF32storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F32loadFmulF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FMUL_F32STORE), LoadComputeStoreInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FMUL, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F32loadFmaxF32storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F32loadFmaxF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FMAX_F32STORE), LoadComputeStoreInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FMAX, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class V32F32loadFminF32storeInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit V32F32loadFminF32storeInstr() : VpuInstr(OpCodeType::V32_F32LOAD_FMIN_F32STORE), LoadComputeStoreInstr(OpCodeType::V32_F32LOAD_GLB, OpCodeType::V32_FMIN, OpCodeType::V32_F32STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class SAddiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SAddiInstr() : VpuInstr(OpCodeType::S_ADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SAddiuInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SAddiuInstr() : VpuInstr(OpCodeType::S_ADDIU) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSubiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSubiInstr() : VpuInstr(OpCodeType::S_SUBI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SAndiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SAndiInstr() : VpuInstr(OpCodeType::S_ANDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SOriInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SOriInstr() : VpuInstr(OpCodeType::S_ORI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SXoriInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SXoriInstr() : VpuInstr(OpCodeType::S_XORI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSgtiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSgtiInstr() : VpuInstr(OpCodeType::S_SGTI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSltiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSltiInstr() : VpuInstr(OpCodeType::S_SLTI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSeqiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSeqiInstr() : VpuInstr(OpCodeType::S_SEQI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSgeiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSgeiInstr() : VpuInstr(OpCodeType::S_SGEI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSleiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSleiInstr() : VpuInstr(OpCodeType::S_SLEI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSraiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSraiInstr() : VpuInstr(OpCodeType::S_SRAI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSrliInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSrliInstr() : VpuInstr(OpCodeType::S_SRLI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SSlliInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSlliInstr() : VpuInstr(OpCodeType::S_SLLI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SLuiInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SLuiInstr() : VpuInstr(OpCodeType::S_LUI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nHasImm = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
};

class SAddInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SAddInstr() : VpuInstr(OpCodeType::S_ADD) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SAdduInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SAdduInstr() : VpuInstr(OpCodeType::S_ADDU) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSubInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSubInstr() : VpuInstr(OpCodeType::S_SUB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SMulInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SMulInstr() : VpuInstr(OpCodeType::S_MUL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SMulhInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SMulhInstr() : VpuInstr(OpCodeType::S_MULH) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SMulhuInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SMulhuInstr() : VpuInstr(OpCodeType::S_MULHU) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SMinInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SMinInstr() : VpuInstr(OpCodeType::S_MIN) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SMaxInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SMaxInstr() : VpuInstr(OpCodeType::S_MAX) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SMinuInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SMinuInstr() : VpuInstr(OpCodeType::S_MINU) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SMaxuInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SMaxuInstr() : VpuInstr(OpCodeType::S_MAXU) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SAndInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SAndInstr() : VpuInstr(OpCodeType::S_AND) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SOrInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SOrInstr() : VpuInstr(OpCodeType::S_OR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SXorInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SXorInstr() : VpuInstr(OpCodeType::S_XOR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSgtInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSgtInstr() : VpuInstr(OpCodeType::S_SGT) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSltInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSltInstr() : VpuInstr(OpCodeType::S_SLT) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSeqInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSeqInstr() : VpuInstr(OpCodeType::S_SEQ) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSgeInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSgeInstr() : VpuInstr(OpCodeType::S_SGE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSleInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSleInstr() : VpuInstr(OpCodeType::S_SLE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SCmpselInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SCmpselInstr() : VpuInstr(OpCodeType::S_CMPSEL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSraInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSraInstr() : VpuInstr(OpCodeType::S_SRA) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSrlInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSrlInstr() : VpuInstr(OpCodeType::S_SRL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SSllInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSllInstr() : VpuInstr(OpCodeType::S_SLL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SJumpInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SJumpInstr() : VpuInstr(OpCodeType::S_JUMP) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SJalInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SJalInstr() : VpuInstr(OpCodeType::S_JAL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SJrInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SJrInstr() : VpuInstr(OpCodeType::S_JR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
};

class SBneInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SBneInstr() : VpuInstr(OpCodeType::S_BNE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SBeqInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SBeqInstr() : VpuInstr(OpCodeType::S_BEQ) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SBltInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SBltInstr() : VpuInstr(OpCodeType::S_BLT) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SBltuInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SBltuInstr() : VpuInstr(OpCodeType::S_BLTU) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SBgeInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SBgeInstr() : VpuInstr(OpCodeType::S_BGE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SBgeuInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SBgeuInstr() : VpuInstr(OpCodeType::S_BGEU) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SSetcfgInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSetcfgInstr() : VpuInstr(OpCodeType::S_SETCFG) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(SpRd0);
    VPU_PUSH_GETTER_SETTER(SpRd1);
    VPU_PUSH_GETTER_SETTER(SrcNum);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(SpRd0, 15, 11);
  VPU_GEN_GETTER_SETTER(SpRd1, 10, 6);
  VPU_GEN_GETTER_SETTER(SrcNum, 5, 5);
};

class SSetrfInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SSetrfInstr() : VpuInstr(OpCodeType::S_SETRF) {
    VPU_PUSH_GETTER_SETTER(RfSel);
    nDtype = 3;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(RfSel, 2, 0);
};


std::shared_ptr<VpuInstr> VpuInstr::create(OpCodeType opCode, const std::vector<size_t> &args) {
  std::shared_ptr<VpuInstr> ret = nullptr;
  switch (opCode) {
    case OpCodeType::VPU_SET_LOAD_BASE:              ret = std::shared_ptr<VpuSetLoadBaseInstr>(new VpuSetLoadBaseInstr); break;
    case OpCodeType::VPU_SET_LOAD2_BASE:             ret = std::shared_ptr<VpuSetLoad2BaseInstr>(new VpuSetLoad2BaseInstr); break;
    case OpCodeType::VPU_SET_STORE_BASE:             ret = std::shared_ptr<VpuSetStoreBaseInstr>(new VpuSetStoreBaseInstr); break;
    case OpCodeType::VPU_SET_LOAD_GLB:               ret = std::shared_ptr<VpuSetLoadGlbInstr>(new VpuSetLoadGlbInstr); break;
    case OpCodeType::VPU_SET_STORE_GLB:              ret = std::shared_ptr<VpuSetStoreGlbInstr>(new VpuSetStoreGlbInstr); break;
    case OpCodeType::VPU_SET_QUANTIZE:               ret = std::shared_ptr<VpuSetQuantizeInstr>(new VpuSetQuantizeInstr); break;
    case OpCodeType::VPU_SET_DEQUANTIZE:             ret = std::shared_ptr<VpuSetDequantizeInstr>(new VpuSetDequantizeInstr); break;
    case OpCodeType::VPU_SET_DEQUANTIZE2:            ret = std::shared_ptr<VpuSetDequantize2Instr>(new VpuSetDequantize2Instr); break;
    case OpCodeType::VPU_SET_FS:                     ret = std::shared_ptr<VpuSetFsInstr>(new VpuSetFsInstr); break;
    case OpCodeType::VPU_SET_CMASK_HIGH:             ret = std::shared_ptr<VpuSetCmaskHighInstr>(new VpuSetCmaskHighInstr); break;
    case OpCodeType::VPU_NOP:                        ret = std::shared_ptr<VpuNopInstr>(new VpuNopInstr); break;
    case OpCodeType::VPU_OP_END:                     ret = std::shared_ptr<VpuOpEndInstr>(new VpuOpEndInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_GLB:              ret = std::shared_ptr<VpuLoadDualGlbInstr>(new VpuLoadDualGlbInstr); break;
    case OpCodeType::VPU_LOAD_GLB:                   ret = std::shared_ptr<VpuLoadGlbInstr>(new VpuLoadGlbInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_GLB:             ret = std::shared_ptr<VpuUloadDualGlbInstr>(new VpuUloadDualGlbInstr); break;
    case OpCodeType::VPU_ULOAD_GLB:                  ret = std::shared_ptr<VpuUloadGlbInstr>(new VpuUloadGlbInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_GLB:             ret = std::shared_ptr<VpuFloadDualGlbInstr>(new VpuFloadDualGlbInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_UPPER_GLB:       ret = std::shared_ptr<VpuFloadDualUpperGlbInstr>(new VpuFloadDualUpperGlbInstr); break;
    case OpCodeType::VPU_FLOAD_GLB:                  ret = std::shared_ptr<VpuFloadGlbInstr>(new VpuFloadGlbInstr); break;
    case OpCodeType::VPU_FLOAD_UPPER_GLB:            ret = std::shared_ptr<VpuFloadUpperGlbInstr>(new VpuFloadUpperGlbInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_GLB:           ret = std::shared_ptr<VpuE43loadDualGlbInstr>(new VpuE43loadDualGlbInstr); break;
    case OpCodeType::VPU_E43LOAD_GLB:                ret = std::shared_ptr<VpuE43loadGlbInstr>(new VpuE43loadGlbInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_GLB:            ret = std::shared_ptr<V32I8loadDualGlbInstr>(new V32I8loadDualGlbInstr); break;
    case OpCodeType::V32_I8LOAD_GLB:                 ret = std::shared_ptr<V32I8loadGlbInstr>(new V32I8loadGlbInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_GLB:            ret = std::shared_ptr<V32U8loadDualGlbInstr>(new V32U8loadDualGlbInstr); break;
    case OpCodeType::V32_U8LOAD_GLB:                 ret = std::shared_ptr<V32U8loadGlbInstr>(new V32U8loadGlbInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_GLB:           ret = std::shared_ptr<V32F16loadDualGlbInstr>(new V32F16loadDualGlbInstr); break;
    case OpCodeType::V32_F16LOAD_GLB:                ret = std::shared_ptr<V32F16loadGlbInstr>(new V32F16loadGlbInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_GLB:           ret = std::shared_ptr<V32F32loadDualGlbInstr>(new V32F32loadDualGlbInstr); break;
    case OpCodeType::V32_F32LOAD_GLB:                ret = std::shared_ptr<V32F32loadGlbInstr>(new V32F32loadGlbInstr); break;
    case OpCodeType::VPU_STORE_GLB:                  ret = std::shared_ptr<VpuStoreGlbInstr>(new VpuStoreGlbInstr); break;
    case OpCodeType::VPU_USTORE_GLB:                 ret = std::shared_ptr<VpuUstoreGlbInstr>(new VpuUstoreGlbInstr); break;
    case OpCodeType::VPU_FSTORE_GLB:                 ret = std::shared_ptr<VpuFstoreGlbInstr>(new VpuFstoreGlbInstr); break;
    case OpCodeType::VPU_FSTORE_UPPER_GLB:           ret = std::shared_ptr<VpuFstoreUpperGlbInstr>(new VpuFstoreUpperGlbInstr); break;
    case OpCodeType::VPU_E43STORE_GLB:               ret = std::shared_ptr<VpuE43storeGlbInstr>(new VpuE43storeGlbInstr); break;
    case OpCodeType::V32_I8STORE_GLB:                ret = std::shared_ptr<V32I8storeGlbInstr>(new V32I8storeGlbInstr); break;
    case OpCodeType::V32_U8STORE_GLB:                ret = std::shared_ptr<V32U8storeGlbInstr>(new V32U8storeGlbInstr); break;
    case OpCodeType::V32_F16STORE_GLB:               ret = std::shared_ptr<V32F16storeGlbInstr>(new V32F16storeGlbInstr); break;
    case OpCodeType::V32_F32STORE_GLB:               ret = std::shared_ptr<V32F32storeGlbInstr>(new V32F32storeGlbInstr); break;
    case OpCodeType::VPU_FADD:                       ret = std::shared_ptr<VpuFaddInstr>(new VpuFaddInstr); break;
    case OpCodeType::VPU_FSUB:                       ret = std::shared_ptr<VpuFsubInstr>(new VpuFsubInstr); break;
    case OpCodeType::VPU_FMUL:                       ret = std::shared_ptr<VpuFmulInstr>(new VpuFmulInstr); break;
    case OpCodeType::VPU_FMAX:                       ret = std::shared_ptr<VpuFmaxInstr>(new VpuFmaxInstr); break;
    case OpCodeType::VPU_FMIN:                       ret = std::shared_ptr<VpuFminInstr>(new VpuFminInstr); break;
    case OpCodeType::VPU_FMOV:                       ret = std::shared_ptr<VpuFmovInstr>(new VpuFmovInstr); break;
    case OpCodeType::VPU_GREATER:                    ret = std::shared_ptr<VpuGreaterInstr>(new VpuGreaterInstr); break;
    case OpCodeType::VPU_GREATER_EQUAL:              ret = std::shared_ptr<VpuGreaterEqualInstr>(new VpuGreaterEqualInstr); break;
    case OpCodeType::VPU_LESS:                       ret = std::shared_ptr<VpuLessInstr>(new VpuLessInstr); break;
    case OpCodeType::VPU_LESS_EQUAL:                 ret = std::shared_ptr<VpuLessEqualInstr>(new VpuLessEqualInstr); break;
    case OpCodeType::VPU_EQUAL:                      ret = std::shared_ptr<VpuEqualInstr>(new VpuEqualInstr); break;
    case OpCodeType::VPU_SEL:                        ret = std::shared_ptr<VpuSelInstr>(new VpuSelInstr); break;
    case OpCodeType::VPU_FADDI:                      ret = std::shared_ptr<VpuFaddiInstr>(new VpuFaddiInstr); break;
    case OpCodeType::VPU_FSUBI:                      ret = std::shared_ptr<VpuFsubiInstr>(new VpuFsubiInstr); break;
    case OpCodeType::VPU_FMULI:                      ret = std::shared_ptr<VpuFmuliInstr>(new VpuFmuliInstr); break;
    case OpCodeType::VPU_FMAXI:                      ret = std::shared_ptr<VpuFmaxiInstr>(new VpuFmaxiInstr); break;
    case OpCodeType::VPU_FMINI:                      ret = std::shared_ptr<VpuFminiInstr>(new VpuFminiInstr); break;
    case OpCodeType::VPU_FMOVI:                      ret = std::shared_ptr<VpuFmoviInstr>(new VpuFmoviInstr); break;
    case OpCodeType::VPU_GREATERI:                   ret = std::shared_ptr<VpuGreateriInstr>(new VpuGreateriInstr); break;
    case OpCodeType::VPU_GREATER_EQUALI:             ret = std::shared_ptr<VpuGreaterEqualiInstr>(new VpuGreaterEqualiInstr); break;
    case OpCodeType::VPU_LESSI:                      ret = std::shared_ptr<VpuLessiInstr>(new VpuLessiInstr); break;
    case OpCodeType::VPU_LESS_EQUALI:                ret = std::shared_ptr<VpuLessEqualiInstr>(new VpuLessEqualiInstr); break;
    case OpCodeType::VPU_EQUALI:                     ret = std::shared_ptr<VpuEqualiInstr>(new VpuEqualiInstr); break;
    case OpCodeType::VPU_SELI:                       ret = std::shared_ptr<VpuSeliInstr>(new VpuSeliInstr); break;
    case OpCodeType::VPU_FREDUCE_SUM:                ret = std::shared_ptr<VpuFreduceSumInstr>(new VpuFreduceSumInstr); break;
    case OpCodeType::V32_FADD:                       ret = std::shared_ptr<V32FaddInstr>(new V32FaddInstr); break;
    case OpCodeType::V32_FSUB:                       ret = std::shared_ptr<V32FsubInstr>(new V32FsubInstr); break;
    case OpCodeType::V32_FMUL:                       ret = std::shared_ptr<V32FmulInstr>(new V32FmulInstr); break;
    case OpCodeType::V32_FMAX:                       ret = std::shared_ptr<V32FmaxInstr>(new V32FmaxInstr); break;
    case OpCodeType::V32_FMIN:                       ret = std::shared_ptr<V32FminInstr>(new V32FminInstr); break;
    case OpCodeType::V32_GREATER:                    ret = std::shared_ptr<V32GreaterInstr>(new V32GreaterInstr); break;
    case OpCodeType::V32_GREATER_EQUAL:              ret = std::shared_ptr<V32GreaterEqualInstr>(new V32GreaterEqualInstr); break;
    case OpCodeType::V32_LESS:                       ret = std::shared_ptr<V32LessInstr>(new V32LessInstr); break;
    case OpCodeType::V32_LESS_EQUAL:                 ret = std::shared_ptr<V32LessEqualInstr>(new V32LessEqualInstr); break;
    case OpCodeType::V32_EQUAL:                      ret = std::shared_ptr<V32EqualInstr>(new V32EqualInstr); break;
    case OpCodeType::V32_SEL:                        ret = std::shared_ptr<V32SelInstr>(new V32SelInstr); break;
    case OpCodeType::V32_FADDI:                      ret = std::shared_ptr<V32FaddiInstr>(new V32FaddiInstr); break;
    case OpCodeType::V32_FSUBI:                      ret = std::shared_ptr<V32FsubiInstr>(new V32FsubiInstr); break;
    case OpCodeType::V32_FMULI:                      ret = std::shared_ptr<V32FmuliInstr>(new V32FmuliInstr); break;
    case OpCodeType::V32_FMAXI:                      ret = std::shared_ptr<V32FmaxiInstr>(new V32FmaxiInstr); break;
    case OpCodeType::V32_FMINI:                      ret = std::shared_ptr<V32FminiInstr>(new V32FminiInstr); break;
    case OpCodeType::V32_FMOVI:                      ret = std::shared_ptr<V32FmoviInstr>(new V32FmoviInstr); break;
    case OpCodeType::V32_GREATERI:                   ret = std::shared_ptr<V32GreateriInstr>(new V32GreateriInstr); break;
    case OpCodeType::V32_GREATER_EQUALI:             ret = std::shared_ptr<V32GreaterEqualiInstr>(new V32GreaterEqualiInstr); break;
    case OpCodeType::V32_LESSI:                      ret = std::shared_ptr<V32LessiInstr>(new V32LessiInstr); break;
    case OpCodeType::V32_LESS_EQUALI:                ret = std::shared_ptr<V32LessEqualiInstr>(new V32LessEqualiInstr); break;
    case OpCodeType::V32_EQUALI:                     ret = std::shared_ptr<V32EqualiInstr>(new V32EqualiInstr); break;
    case OpCodeType::V32_SELI:                       ret = std::shared_ptr<V32SeliInstr>(new V32SeliInstr); break;
    case OpCodeType::V32_LUI:                        ret = std::shared_ptr<V32LuiInstr>(new V32LuiInstr); break;
    case OpCodeType::V32_LI:                         ret = std::shared_ptr<V32LiInstr>(new V32LiInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FADD:             ret = std::shared_ptr<VpuLoadDualFaddInstr>(new VpuLoadDualFaddInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FSUB:             ret = std::shared_ptr<VpuLoadDualFsubInstr>(new VpuLoadDualFsubInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMUL:             ret = std::shared_ptr<VpuLoadDualFmulInstr>(new VpuLoadDualFmulInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMAX:             ret = std::shared_ptr<VpuLoadDualFmaxInstr>(new VpuLoadDualFmaxInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMIN:             ret = std::shared_ptr<VpuLoadDualFminInstr>(new VpuLoadDualFminInstr); break;
    case OpCodeType::VPU_LOAD_FADD:                  ret = std::shared_ptr<VpuLoadFaddInstr>(new VpuLoadFaddInstr); break;
    case OpCodeType::VPU_LOAD_FSUB:                  ret = std::shared_ptr<VpuLoadFsubInstr>(new VpuLoadFsubInstr); break;
    case OpCodeType::VPU_LOAD_FMUL:                  ret = std::shared_ptr<VpuLoadFmulInstr>(new VpuLoadFmulInstr); break;
    case OpCodeType::VPU_LOAD_FMAX:                  ret = std::shared_ptr<VpuLoadFmaxInstr>(new VpuLoadFmaxInstr); break;
    case OpCodeType::VPU_LOAD_FMIN:                  ret = std::shared_ptr<VpuLoadFminInstr>(new VpuLoadFminInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FADD:            ret = std::shared_ptr<VpuUloadDualFaddInstr>(new VpuUloadDualFaddInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FSUB:            ret = std::shared_ptr<VpuUloadDualFsubInstr>(new VpuUloadDualFsubInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMUL:            ret = std::shared_ptr<VpuUloadDualFmulInstr>(new VpuUloadDualFmulInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMAX:            ret = std::shared_ptr<VpuUloadDualFmaxInstr>(new VpuUloadDualFmaxInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMIN:            ret = std::shared_ptr<VpuUloadDualFminInstr>(new VpuUloadDualFminInstr); break;
    case OpCodeType::VPU_ULOAD_FADD:                 ret = std::shared_ptr<VpuUloadFaddInstr>(new VpuUloadFaddInstr); break;
    case OpCodeType::VPU_ULOAD_FSUB:                 ret = std::shared_ptr<VpuUloadFsubInstr>(new VpuUloadFsubInstr); break;
    case OpCodeType::VPU_ULOAD_FMUL:                 ret = std::shared_ptr<VpuUloadFmulInstr>(new VpuUloadFmulInstr); break;
    case OpCodeType::VPU_ULOAD_FMAX:                 ret = std::shared_ptr<VpuUloadFmaxInstr>(new VpuUloadFmaxInstr); break;
    case OpCodeType::VPU_ULOAD_FMIN:                 ret = std::shared_ptr<VpuUloadFminInstr>(new VpuUloadFminInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FADD:            ret = std::shared_ptr<VpuFloadDualFaddInstr>(new VpuFloadDualFaddInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FSUB:            ret = std::shared_ptr<VpuFloadDualFsubInstr>(new VpuFloadDualFsubInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMUL:            ret = std::shared_ptr<VpuFloadDualFmulInstr>(new VpuFloadDualFmulInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMAX:            ret = std::shared_ptr<VpuFloadDualFmaxInstr>(new VpuFloadDualFmaxInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMIN:            ret = std::shared_ptr<VpuFloadDualFminInstr>(new VpuFloadDualFminInstr); break;
    case OpCodeType::VPU_FLOAD_FADD:                 ret = std::shared_ptr<VpuFloadFaddInstr>(new VpuFloadFaddInstr); break;
    case OpCodeType::VPU_FLOAD_FSUB:                 ret = std::shared_ptr<VpuFloadFsubInstr>(new VpuFloadFsubInstr); break;
    case OpCodeType::VPU_FLOAD_FMUL:                 ret = std::shared_ptr<VpuFloadFmulInstr>(new VpuFloadFmulInstr); break;
    case OpCodeType::VPU_FLOAD_FMAX:                 ret = std::shared_ptr<VpuFloadFmaxInstr>(new VpuFloadFmaxInstr); break;
    case OpCodeType::VPU_FLOAD_FMIN:                 ret = std::shared_ptr<VpuFloadFminInstr>(new VpuFloadFminInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FADD:          ret = std::shared_ptr<VpuE43loadDualFaddInstr>(new VpuE43loadDualFaddInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FSUB:          ret = std::shared_ptr<VpuE43loadDualFsubInstr>(new VpuE43loadDualFsubInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FMUL:          ret = std::shared_ptr<VpuE43loadDualFmulInstr>(new VpuE43loadDualFmulInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FMAX:          ret = std::shared_ptr<VpuE43loadDualFmaxInstr>(new VpuE43loadDualFmaxInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FMIN:          ret = std::shared_ptr<VpuE43loadDualFminInstr>(new VpuE43loadDualFminInstr); break;
    case OpCodeType::VPU_E43LOAD_FADD:               ret = std::shared_ptr<VpuE43loadFaddInstr>(new VpuE43loadFaddInstr); break;
    case OpCodeType::VPU_E43LOAD_FSUB:               ret = std::shared_ptr<VpuE43loadFsubInstr>(new VpuE43loadFsubInstr); break;
    case OpCodeType::VPU_E43LOAD_FMUL:               ret = std::shared_ptr<VpuE43loadFmulInstr>(new VpuE43loadFmulInstr); break;
    case OpCodeType::VPU_E43LOAD_FMAX:               ret = std::shared_ptr<VpuE43loadFmaxInstr>(new VpuE43loadFmaxInstr); break;
    case OpCodeType::VPU_E43LOAD_FMIN:               ret = std::shared_ptr<VpuE43loadFminInstr>(new VpuE43loadFminInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FADD:           ret = std::shared_ptr<V32I8loadDualFaddInstr>(new V32I8loadDualFaddInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FSUB:           ret = std::shared_ptr<V32I8loadDualFsubInstr>(new V32I8loadDualFsubInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FMUL:           ret = std::shared_ptr<V32I8loadDualFmulInstr>(new V32I8loadDualFmulInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FMAX:           ret = std::shared_ptr<V32I8loadDualFmaxInstr>(new V32I8loadDualFmaxInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FMIN:           ret = std::shared_ptr<V32I8loadDualFminInstr>(new V32I8loadDualFminInstr); break;
    case OpCodeType::V32_I8LOAD_FADD:                ret = std::shared_ptr<V32I8loadFaddInstr>(new V32I8loadFaddInstr); break;
    case OpCodeType::V32_I8LOAD_FSUB:                ret = std::shared_ptr<V32I8loadFsubInstr>(new V32I8loadFsubInstr); break;
    case OpCodeType::V32_I8LOAD_FMUL:                ret = std::shared_ptr<V32I8loadFmulInstr>(new V32I8loadFmulInstr); break;
    case OpCodeType::V32_I8LOAD_FMAX:                ret = std::shared_ptr<V32I8loadFmaxInstr>(new V32I8loadFmaxInstr); break;
    case OpCodeType::V32_I8LOAD_FMIN:                ret = std::shared_ptr<V32I8loadFminInstr>(new V32I8loadFminInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FADD:           ret = std::shared_ptr<V32U8loadDualFaddInstr>(new V32U8loadDualFaddInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FSUB:           ret = std::shared_ptr<V32U8loadDualFsubInstr>(new V32U8loadDualFsubInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FMUL:           ret = std::shared_ptr<V32U8loadDualFmulInstr>(new V32U8loadDualFmulInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FMAX:           ret = std::shared_ptr<V32U8loadDualFmaxInstr>(new V32U8loadDualFmaxInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FMIN:           ret = std::shared_ptr<V32U8loadDualFminInstr>(new V32U8loadDualFminInstr); break;
    case OpCodeType::V32_U8LOAD_FADD:                ret = std::shared_ptr<V32U8loadFaddInstr>(new V32U8loadFaddInstr); break;
    case OpCodeType::V32_U8LOAD_FSUB:                ret = std::shared_ptr<V32U8loadFsubInstr>(new V32U8loadFsubInstr); break;
    case OpCodeType::V32_U8LOAD_FMUL:                ret = std::shared_ptr<V32U8loadFmulInstr>(new V32U8loadFmulInstr); break;
    case OpCodeType::V32_U8LOAD_FMAX:                ret = std::shared_ptr<V32U8loadFmaxInstr>(new V32U8loadFmaxInstr); break;
    case OpCodeType::V32_U8LOAD_FMIN:                ret = std::shared_ptr<V32U8loadFminInstr>(new V32U8loadFminInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FADD:          ret = std::shared_ptr<V32F16loadDualFaddInstr>(new V32F16loadDualFaddInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FSUB:          ret = std::shared_ptr<V32F16loadDualFsubInstr>(new V32F16loadDualFsubInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FMUL:          ret = std::shared_ptr<V32F16loadDualFmulInstr>(new V32F16loadDualFmulInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FMAX:          ret = std::shared_ptr<V32F16loadDualFmaxInstr>(new V32F16loadDualFmaxInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FMIN:          ret = std::shared_ptr<V32F16loadDualFminInstr>(new V32F16loadDualFminInstr); break;
    case OpCodeType::V32_F16LOAD_FADD:               ret = std::shared_ptr<V32F16loadFaddInstr>(new V32F16loadFaddInstr); break;
    case OpCodeType::V32_F16LOAD_FSUB:               ret = std::shared_ptr<V32F16loadFsubInstr>(new V32F16loadFsubInstr); break;
    case OpCodeType::V32_F16LOAD_FMUL:               ret = std::shared_ptr<V32F16loadFmulInstr>(new V32F16loadFmulInstr); break;
    case OpCodeType::V32_F16LOAD_FMAX:               ret = std::shared_ptr<V32F16loadFmaxInstr>(new V32F16loadFmaxInstr); break;
    case OpCodeType::V32_F16LOAD_FMIN:               ret = std::shared_ptr<V32F16loadFminInstr>(new V32F16loadFminInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FADD:          ret = std::shared_ptr<V32F32loadDualFaddInstr>(new V32F32loadDualFaddInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FSUB:          ret = std::shared_ptr<V32F32loadDualFsubInstr>(new V32F32loadDualFsubInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FMUL:          ret = std::shared_ptr<V32F32loadDualFmulInstr>(new V32F32loadDualFmulInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FMAX:          ret = std::shared_ptr<V32F32loadDualFmaxInstr>(new V32F32loadDualFmaxInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FMIN:          ret = std::shared_ptr<V32F32loadDualFminInstr>(new V32F32loadDualFminInstr); break;
    case OpCodeType::V32_F32LOAD_FADD:               ret = std::shared_ptr<V32F32loadFaddInstr>(new V32F32loadFaddInstr); break;
    case OpCodeType::V32_F32LOAD_FSUB:               ret = std::shared_ptr<V32F32loadFsubInstr>(new V32F32loadFsubInstr); break;
    case OpCodeType::V32_F32LOAD_FMUL:               ret = std::shared_ptr<V32F32loadFmulInstr>(new V32F32loadFmulInstr); break;
    case OpCodeType::V32_F32LOAD_FMAX:               ret = std::shared_ptr<V32F32loadFmaxInstr>(new V32F32loadFmaxInstr); break;
    case OpCodeType::V32_F32LOAD_FMIN:               ret = std::shared_ptr<V32F32loadFminInstr>(new V32F32loadFminInstr); break;
    case OpCodeType::VPU_FADD_STORE:                 ret = std::shared_ptr<VpuFaddStoreInstr>(new VpuFaddStoreInstr); break;
    case OpCodeType::VPU_FSUB_STORE:                 ret = std::shared_ptr<VpuFsubStoreInstr>(new VpuFsubStoreInstr); break;
    case OpCodeType::VPU_FMUL_STORE:                 ret = std::shared_ptr<VpuFmulStoreInstr>(new VpuFmulStoreInstr); break;
    case OpCodeType::VPU_FMAX_STORE:                 ret = std::shared_ptr<VpuFmaxStoreInstr>(new VpuFmaxStoreInstr); break;
    case OpCodeType::VPU_FMIN_STORE:                 ret = std::shared_ptr<VpuFminStoreInstr>(new VpuFminStoreInstr); break;
    case OpCodeType::VPU_FADD_USTORE:                ret = std::shared_ptr<VpuFaddUstoreInstr>(new VpuFaddUstoreInstr); break;
    case OpCodeType::VPU_FSUB_USTORE:                ret = std::shared_ptr<VpuFsubUstoreInstr>(new VpuFsubUstoreInstr); break;
    case OpCodeType::VPU_FMUL_USTORE:                ret = std::shared_ptr<VpuFmulUstoreInstr>(new VpuFmulUstoreInstr); break;
    case OpCodeType::VPU_FMAX_USTORE:                ret = std::shared_ptr<VpuFmaxUstoreInstr>(new VpuFmaxUstoreInstr); break;
    case OpCodeType::VPU_FMIN_USTORE:                ret = std::shared_ptr<VpuFminUstoreInstr>(new VpuFminUstoreInstr); break;
    case OpCodeType::VPU_FADD_FSTORE:                ret = std::shared_ptr<VpuFaddFstoreInstr>(new VpuFaddFstoreInstr); break;
    case OpCodeType::VPU_FSUB_FSTORE:                ret = std::shared_ptr<VpuFsubFstoreInstr>(new VpuFsubFstoreInstr); break;
    case OpCodeType::VPU_FMUL_FSTORE:                ret = std::shared_ptr<VpuFmulFstoreInstr>(new VpuFmulFstoreInstr); break;
    case OpCodeType::VPU_FMAX_FSTORE:                ret = std::shared_ptr<VpuFmaxFstoreInstr>(new VpuFmaxFstoreInstr); break;
    case OpCodeType::VPU_FMIN_FSTORE:                ret = std::shared_ptr<VpuFminFstoreInstr>(new VpuFminFstoreInstr); break;
    case OpCodeType::VPU_FADD_E43STORE:              ret = std::shared_ptr<VpuFaddE43storeInstr>(new VpuFaddE43storeInstr); break;
    case OpCodeType::VPU_FSUB_E43STORE:              ret = std::shared_ptr<VpuFsubE43storeInstr>(new VpuFsubE43storeInstr); break;
    case OpCodeType::VPU_FMUL_E43STORE:              ret = std::shared_ptr<VpuFmulE43storeInstr>(new VpuFmulE43storeInstr); break;
    case OpCodeType::VPU_FMAX_E43STORE:              ret = std::shared_ptr<VpuFmaxE43storeInstr>(new VpuFmaxE43storeInstr); break;
    case OpCodeType::VPU_FMIN_E43STORE:              ret = std::shared_ptr<VpuFminE43storeInstr>(new VpuFminE43storeInstr); break;
    case OpCodeType::V32_FADD_I8STORE:               ret = std::shared_ptr<V32FaddI8storeInstr>(new V32FaddI8storeInstr); break;
    case OpCodeType::V32_FSUB_I8STORE:               ret = std::shared_ptr<V32FsubI8storeInstr>(new V32FsubI8storeInstr); break;
    case OpCodeType::V32_FMUL_I8STORE:               ret = std::shared_ptr<V32FmulI8storeInstr>(new V32FmulI8storeInstr); break;
    case OpCodeType::V32_FMAX_I8STORE:               ret = std::shared_ptr<V32FmaxI8storeInstr>(new V32FmaxI8storeInstr); break;
    case OpCodeType::V32_FMIN_I8STORE:               ret = std::shared_ptr<V32FminI8storeInstr>(new V32FminI8storeInstr); break;
    case OpCodeType::V32_FADD_U8STORE:               ret = std::shared_ptr<V32FaddU8storeInstr>(new V32FaddU8storeInstr); break;
    case OpCodeType::V32_FSUB_U8STORE:               ret = std::shared_ptr<V32FsubU8storeInstr>(new V32FsubU8storeInstr); break;
    case OpCodeType::V32_FMUL_U8STORE:               ret = std::shared_ptr<V32FmulU8storeInstr>(new V32FmulU8storeInstr); break;
    case OpCodeType::V32_FMAX_U8STORE:               ret = std::shared_ptr<V32FmaxU8storeInstr>(new V32FmaxU8storeInstr); break;
    case OpCodeType::V32_FMIN_U8STORE:               ret = std::shared_ptr<V32FminU8storeInstr>(new V32FminU8storeInstr); break;
    case OpCodeType::V32_FADD_F16STORE:              ret = std::shared_ptr<V32FaddF16storeInstr>(new V32FaddF16storeInstr); break;
    case OpCodeType::V32_FSUB_F16STORE:              ret = std::shared_ptr<V32FsubF16storeInstr>(new V32FsubF16storeInstr); break;
    case OpCodeType::V32_FMUL_F16STORE:              ret = std::shared_ptr<V32FmulF16storeInstr>(new V32FmulF16storeInstr); break;
    case OpCodeType::V32_FMAX_F16STORE:              ret = std::shared_ptr<V32FmaxF16storeInstr>(new V32FmaxF16storeInstr); break;
    case OpCodeType::V32_FMIN_F16STORE:              ret = std::shared_ptr<V32FminF16storeInstr>(new V32FminF16storeInstr); break;
    case OpCodeType::V32_FADD_F32STORE:              ret = std::shared_ptr<V32FaddF32storeInstr>(new V32FaddF32storeInstr); break;
    case OpCodeType::V32_FSUB_F32STORE:              ret = std::shared_ptr<V32FsubF32storeInstr>(new V32FsubF32storeInstr); break;
    case OpCodeType::V32_FMUL_F32STORE:              ret = std::shared_ptr<V32FmulF32storeInstr>(new V32FmulF32storeInstr); break;
    case OpCodeType::V32_FMAX_F32STORE:              ret = std::shared_ptr<V32FmaxF32storeInstr>(new V32FmaxF32storeInstr); break;
    case OpCodeType::V32_FMIN_F32STORE:              ret = std::shared_ptr<V32FminF32storeInstr>(new V32FminF32storeInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FADD_STORE:       ret = std::shared_ptr<VpuLoadDualFaddStoreInstr>(new VpuLoadDualFaddStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FSUB_STORE:       ret = std::shared_ptr<VpuLoadDualFsubStoreInstr>(new VpuLoadDualFsubStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMUL_STORE:       ret = std::shared_ptr<VpuLoadDualFmulStoreInstr>(new VpuLoadDualFmulStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMAX_STORE:       ret = std::shared_ptr<VpuLoadDualFmaxStoreInstr>(new VpuLoadDualFmaxStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMIN_STORE:       ret = std::shared_ptr<VpuLoadDualFminStoreInstr>(new VpuLoadDualFminStoreInstr); break;
    case OpCodeType::VPU_LOAD_FADD_STORE:            ret = std::shared_ptr<VpuLoadFaddStoreInstr>(new VpuLoadFaddStoreInstr); break;
    case OpCodeType::VPU_LOAD_FSUB_STORE:            ret = std::shared_ptr<VpuLoadFsubStoreInstr>(new VpuLoadFsubStoreInstr); break;
    case OpCodeType::VPU_LOAD_FMUL_STORE:            ret = std::shared_ptr<VpuLoadFmulStoreInstr>(new VpuLoadFmulStoreInstr); break;
    case OpCodeType::VPU_LOAD_FMAX_STORE:            ret = std::shared_ptr<VpuLoadFmaxStoreInstr>(new VpuLoadFmaxStoreInstr); break;
    case OpCodeType::VPU_LOAD_FMIN_STORE:            ret = std::shared_ptr<VpuLoadFminStoreInstr>(new VpuLoadFminStoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FADD_USTORE:     ret = std::shared_ptr<VpuUloadDualFaddUstoreInstr>(new VpuUloadDualFaddUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FSUB_USTORE:     ret = std::shared_ptr<VpuUloadDualFsubUstoreInstr>(new VpuUloadDualFsubUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMUL_USTORE:     ret = std::shared_ptr<VpuUloadDualFmulUstoreInstr>(new VpuUloadDualFmulUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMAX_USTORE:     ret = std::shared_ptr<VpuUloadDualFmaxUstoreInstr>(new VpuUloadDualFmaxUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMIN_USTORE:     ret = std::shared_ptr<VpuUloadDualFminUstoreInstr>(new VpuUloadDualFminUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FADD_USTORE:          ret = std::shared_ptr<VpuUloadFaddUstoreInstr>(new VpuUloadFaddUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FSUB_USTORE:          ret = std::shared_ptr<VpuUloadFsubUstoreInstr>(new VpuUloadFsubUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FMUL_USTORE:          ret = std::shared_ptr<VpuUloadFmulUstoreInstr>(new VpuUloadFmulUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FMAX_USTORE:          ret = std::shared_ptr<VpuUloadFmaxUstoreInstr>(new VpuUloadFmaxUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FMIN_USTORE:          ret = std::shared_ptr<VpuUloadFminUstoreInstr>(new VpuUloadFminUstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FADD_FSTORE:     ret = std::shared_ptr<VpuFloadDualFaddFstoreInstr>(new VpuFloadDualFaddFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FSUB_FSTORE:     ret = std::shared_ptr<VpuFloadDualFsubFstoreInstr>(new VpuFloadDualFsubFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMUL_FSTORE:     ret = std::shared_ptr<VpuFloadDualFmulFstoreInstr>(new VpuFloadDualFmulFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMAX_FSTORE:     ret = std::shared_ptr<VpuFloadDualFmaxFstoreInstr>(new VpuFloadDualFmaxFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMIN_FSTORE:     ret = std::shared_ptr<VpuFloadDualFminFstoreInstr>(new VpuFloadDualFminFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FADD_FSTORE:          ret = std::shared_ptr<VpuFloadFaddFstoreInstr>(new VpuFloadFaddFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FSUB_FSTORE:          ret = std::shared_ptr<VpuFloadFsubFstoreInstr>(new VpuFloadFsubFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FMUL_FSTORE:          ret = std::shared_ptr<VpuFloadFmulFstoreInstr>(new VpuFloadFmulFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FMAX_FSTORE:          ret = std::shared_ptr<VpuFloadFmaxFstoreInstr>(new VpuFloadFmaxFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FMIN_FSTORE:          ret = std::shared_ptr<VpuFloadFminFstoreInstr>(new VpuFloadFminFstoreInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FADD_E43STORE: ret = std::shared_ptr<VpuE43loadDualFaddE43storeInstr>(new VpuE43loadDualFaddE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FSUB_E43STORE: ret = std::shared_ptr<VpuE43loadDualFsubE43storeInstr>(new VpuE43loadDualFsubE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FMUL_E43STORE: ret = std::shared_ptr<VpuE43loadDualFmulE43storeInstr>(new VpuE43loadDualFmulE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FMAX_E43STORE: ret = std::shared_ptr<VpuE43loadDualFmaxE43storeInstr>(new VpuE43loadDualFmaxE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_DUAL_FMIN_E43STORE: ret = std::shared_ptr<VpuE43loadDualFminE43storeInstr>(new VpuE43loadDualFminE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_FADD_E43STORE:      ret = std::shared_ptr<VpuE43loadFaddE43storeInstr>(new VpuE43loadFaddE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_FSUB_E43STORE:      ret = std::shared_ptr<VpuE43loadFsubE43storeInstr>(new VpuE43loadFsubE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_FMUL_E43STORE:      ret = std::shared_ptr<VpuE43loadFmulE43storeInstr>(new VpuE43loadFmulE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_FMAX_E43STORE:      ret = std::shared_ptr<VpuE43loadFmaxE43storeInstr>(new VpuE43loadFmaxE43storeInstr); break;
    case OpCodeType::VPU_E43LOAD_FMIN_E43STORE:      ret = std::shared_ptr<VpuE43loadFminE43storeInstr>(new VpuE43loadFminE43storeInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FADD_I8STORE:   ret = std::shared_ptr<V32I8loadDualFaddI8storeInstr>(new V32I8loadDualFaddI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FSUB_I8STORE:   ret = std::shared_ptr<V32I8loadDualFsubI8storeInstr>(new V32I8loadDualFsubI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FMUL_I8STORE:   ret = std::shared_ptr<V32I8loadDualFmulI8storeInstr>(new V32I8loadDualFmulI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FMAX_I8STORE:   ret = std::shared_ptr<V32I8loadDualFmaxI8storeInstr>(new V32I8loadDualFmaxI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_DUAL_FMIN_I8STORE:   ret = std::shared_ptr<V32I8loadDualFminI8storeInstr>(new V32I8loadDualFminI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_FADD_I8STORE:        ret = std::shared_ptr<V32I8loadFaddI8storeInstr>(new V32I8loadFaddI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_FSUB_I8STORE:        ret = std::shared_ptr<V32I8loadFsubI8storeInstr>(new V32I8loadFsubI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_FMUL_I8STORE:        ret = std::shared_ptr<V32I8loadFmulI8storeInstr>(new V32I8loadFmulI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_FMAX_I8STORE:        ret = std::shared_ptr<V32I8loadFmaxI8storeInstr>(new V32I8loadFmaxI8storeInstr); break;
    case OpCodeType::V32_I8LOAD_FMIN_I8STORE:        ret = std::shared_ptr<V32I8loadFminI8storeInstr>(new V32I8loadFminI8storeInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FADD_U8STORE:   ret = std::shared_ptr<V32U8loadDualFaddU8storeInstr>(new V32U8loadDualFaddU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FSUB_U8STORE:   ret = std::shared_ptr<V32U8loadDualFsubU8storeInstr>(new V32U8loadDualFsubU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FMUL_U8STORE:   ret = std::shared_ptr<V32U8loadDualFmulU8storeInstr>(new V32U8loadDualFmulU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FMAX_U8STORE:   ret = std::shared_ptr<V32U8loadDualFmaxU8storeInstr>(new V32U8loadDualFmaxU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_DUAL_FMIN_U8STORE:   ret = std::shared_ptr<V32U8loadDualFminU8storeInstr>(new V32U8loadDualFminU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_FADD_U8STORE:        ret = std::shared_ptr<V32U8loadFaddU8storeInstr>(new V32U8loadFaddU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_FSUB_U8STORE:        ret = std::shared_ptr<V32U8loadFsubU8storeInstr>(new V32U8loadFsubU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_FMUL_U8STORE:        ret = std::shared_ptr<V32U8loadFmulU8storeInstr>(new V32U8loadFmulU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_FMAX_U8STORE:        ret = std::shared_ptr<V32U8loadFmaxU8storeInstr>(new V32U8loadFmaxU8storeInstr); break;
    case OpCodeType::V32_U8LOAD_FMIN_U8STORE:        ret = std::shared_ptr<V32U8loadFminU8storeInstr>(new V32U8loadFminU8storeInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FADD_F16STORE: ret = std::shared_ptr<V32F16loadDualFaddF16storeInstr>(new V32F16loadDualFaddF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FSUB_F16STORE: ret = std::shared_ptr<V32F16loadDualFsubF16storeInstr>(new V32F16loadDualFsubF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FMUL_F16STORE: ret = std::shared_ptr<V32F16loadDualFmulF16storeInstr>(new V32F16loadDualFmulF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FMAX_F16STORE: ret = std::shared_ptr<V32F16loadDualFmaxF16storeInstr>(new V32F16loadDualFmaxF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_DUAL_FMIN_F16STORE: ret = std::shared_ptr<V32F16loadDualFminF16storeInstr>(new V32F16loadDualFminF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_FADD_F16STORE:      ret = std::shared_ptr<V32F16loadFaddF16storeInstr>(new V32F16loadFaddF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_FSUB_F16STORE:      ret = std::shared_ptr<V32F16loadFsubF16storeInstr>(new V32F16loadFsubF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_FMUL_F16STORE:      ret = std::shared_ptr<V32F16loadFmulF16storeInstr>(new V32F16loadFmulF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_FMAX_F16STORE:      ret = std::shared_ptr<V32F16loadFmaxF16storeInstr>(new V32F16loadFmaxF16storeInstr); break;
    case OpCodeType::V32_F16LOAD_FMIN_F16STORE:      ret = std::shared_ptr<V32F16loadFminF16storeInstr>(new V32F16loadFminF16storeInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FADD_F32STORE: ret = std::shared_ptr<V32F32loadDualFaddF32storeInstr>(new V32F32loadDualFaddF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FSUB_F32STORE: ret = std::shared_ptr<V32F32loadDualFsubF32storeInstr>(new V32F32loadDualFsubF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FMUL_F32STORE: ret = std::shared_ptr<V32F32loadDualFmulF32storeInstr>(new V32F32loadDualFmulF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FMAX_F32STORE: ret = std::shared_ptr<V32F32loadDualFmaxF32storeInstr>(new V32F32loadDualFmaxF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_DUAL_FMIN_F32STORE: ret = std::shared_ptr<V32F32loadDualFminF32storeInstr>(new V32F32loadDualFminF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_FADD_F32STORE:      ret = std::shared_ptr<V32F32loadFaddF32storeInstr>(new V32F32loadFaddF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_FSUB_F32STORE:      ret = std::shared_ptr<V32F32loadFsubF32storeInstr>(new V32F32loadFsubF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_FMUL_F32STORE:      ret = std::shared_ptr<V32F32loadFmulF32storeInstr>(new V32F32loadFmulF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_FMAX_F32STORE:      ret = std::shared_ptr<V32F32loadFmaxF32storeInstr>(new V32F32loadFmaxF32storeInstr); break;
    case OpCodeType::V32_F32LOAD_FMIN_F32STORE:      ret = std::shared_ptr<V32F32loadFminF32storeInstr>(new V32F32loadFminF32storeInstr); break;
    case OpCodeType::S_ADDI:                         ret = std::shared_ptr<SAddiInstr>(new SAddiInstr); break;
    case OpCodeType::S_ADDIU:                        ret = std::shared_ptr<SAddiuInstr>(new SAddiuInstr); break;
    case OpCodeType::S_SUBI:                         ret = std::shared_ptr<SSubiInstr>(new SSubiInstr); break;
    case OpCodeType::S_ANDI:                         ret = std::shared_ptr<SAndiInstr>(new SAndiInstr); break;
    case OpCodeType::S_ORI:                          ret = std::shared_ptr<SOriInstr>(new SOriInstr); break;
    case OpCodeType::S_XORI:                         ret = std::shared_ptr<SXoriInstr>(new SXoriInstr); break;
    case OpCodeType::S_SGTI:                         ret = std::shared_ptr<SSgtiInstr>(new SSgtiInstr); break;
    case OpCodeType::S_SLTI:                         ret = std::shared_ptr<SSltiInstr>(new SSltiInstr); break;
    case OpCodeType::S_SEQI:                         ret = std::shared_ptr<SSeqiInstr>(new SSeqiInstr); break;
    case OpCodeType::S_SGEI:                         ret = std::shared_ptr<SSgeiInstr>(new SSgeiInstr); break;
    case OpCodeType::S_SLEI:                         ret = std::shared_ptr<SSleiInstr>(new SSleiInstr); break;
    case OpCodeType::S_SRAI:                         ret = std::shared_ptr<SSraiInstr>(new SSraiInstr); break;
    case OpCodeType::S_SRLI:                         ret = std::shared_ptr<SSrliInstr>(new SSrliInstr); break;
    case OpCodeType::S_SLLI:                         ret = std::shared_ptr<SSlliInstr>(new SSlliInstr); break;
    case OpCodeType::S_LUI:                          ret = std::shared_ptr<SLuiInstr>(new SLuiInstr); break;
    case OpCodeType::S_ADD:                          ret = std::shared_ptr<SAddInstr>(new SAddInstr); break;
    case OpCodeType::S_ADDU:                         ret = std::shared_ptr<SAdduInstr>(new SAdduInstr); break;
    case OpCodeType::S_SUB:                          ret = std::shared_ptr<SSubInstr>(new SSubInstr); break;
    case OpCodeType::S_MUL:                          ret = std::shared_ptr<SMulInstr>(new SMulInstr); break;
    case OpCodeType::S_MULH:                         ret = std::shared_ptr<SMulhInstr>(new SMulhInstr); break;
    case OpCodeType::S_MULHU:                        ret = std::shared_ptr<SMulhuInstr>(new SMulhuInstr); break;
    case OpCodeType::S_MIN:                          ret = std::shared_ptr<SMinInstr>(new SMinInstr); break;
    case OpCodeType::S_MAX:                          ret = std::shared_ptr<SMaxInstr>(new SMaxInstr); break;
    case OpCodeType::S_MINU:                         ret = std::shared_ptr<SMinuInstr>(new SMinuInstr); break;
    case OpCodeType::S_MAXU:                         ret = std::shared_ptr<SMaxuInstr>(new SMaxuInstr); break;
    case OpCodeType::S_AND:                          ret = std::shared_ptr<SAndInstr>(new SAndInstr); break;
    case OpCodeType::S_OR:                           ret = std::shared_ptr<SOrInstr>(new SOrInstr); break;
    case OpCodeType::S_XOR:                          ret = std::shared_ptr<SXorInstr>(new SXorInstr); break;
    case OpCodeType::S_SGT:                          ret = std::shared_ptr<SSgtInstr>(new SSgtInstr); break;
    case OpCodeType::S_SLT:                          ret = std::shared_ptr<SSltInstr>(new SSltInstr); break;
    case OpCodeType::S_SEQ:                          ret = std::shared_ptr<SSeqInstr>(new SSeqInstr); break;
    case OpCodeType::S_SGE:                          ret = std::shared_ptr<SSgeInstr>(new SSgeInstr); break;
    case OpCodeType::S_SLE:                          ret = std::shared_ptr<SSleInstr>(new SSleInstr); break;
    case OpCodeType::S_CMPSEL:                       ret = std::shared_ptr<SCmpselInstr>(new SCmpselInstr); break;
    case OpCodeType::S_SRA:                          ret = std::shared_ptr<SSraInstr>(new SSraInstr); break;
    case OpCodeType::S_SRL:                          ret = std::shared_ptr<SSrlInstr>(new SSrlInstr); break;
    case OpCodeType::S_SLL:                          ret = std::shared_ptr<SSllInstr>(new SSllInstr); break;
    case OpCodeType::S_JUMP:                         ret = std::shared_ptr<SJumpInstr>(new SJumpInstr); break;
    case OpCodeType::S_JAL:                          ret = std::shared_ptr<SJalInstr>(new SJalInstr); break;
    case OpCodeType::S_JR:                           ret = std::shared_ptr<SJrInstr>(new SJrInstr); break;
    case OpCodeType::S_BNE:                          ret = std::shared_ptr<SBneInstr>(new SBneInstr); break;
    case OpCodeType::S_BEQ:                          ret = std::shared_ptr<SBeqInstr>(new SBeqInstr); break;
    case OpCodeType::S_BLT:                          ret = std::shared_ptr<SBltInstr>(new SBltInstr); break;
    case OpCodeType::S_BLTU:                         ret = std::shared_ptr<SBltuInstr>(new SBltuInstr); break;
    case OpCodeType::S_BGE:                          ret = std::shared_ptr<SBgeInstr>(new SBgeInstr); break;
    case OpCodeType::S_BGEU:                         ret = std::shared_ptr<SBgeuInstr>(new SBgeuInstr); break;
    case OpCodeType::S_SETCFG:                       ret = std::shared_ptr<SSetcfgInstr>(new SSetcfgInstr); break;
    case OpCodeType::S_SETRF:                        ret = std::shared_ptr<SSetrfInstr>(new SSetrfInstr); break;
    default: break;
  }
  for (size_t i = 0; i < args.size(); ++i)
    ret->setters[i](args[i]);
  return ret;
}

} // namespace quark

#endif // QUARK_VPU_INSTR_H
