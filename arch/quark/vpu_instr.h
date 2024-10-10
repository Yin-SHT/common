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
    uint32_t m = (((uint32_t)1<<(high-low+1))-1)<<low;\
    fields = (fields&(~m)) | ((field<<low)&m);\
    VpuInstr::set##field(field); }
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
  SCALAR_ALU = 0,
  FP32_ALU,
  INT32_ALU,
  FP64_ALU,
  INT64_ALU,
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
    SET_LOAD_BASE = 0,
    SET_LOAD2_BASE = 1,
    SET_STORE_BASE = 2,
    SET_LOAD_GLB = 3,
    SET_STORE_GLB = 4,
    SET_QUANTIZE = 5,
    SET_DEQUANTIZE = 6,
    SET_DEQUANTIZE2 = 7,
    SET_FS = 8,
    SET_AAI = 9,
    VPU_NOP = 10,
    VPU_END = 11,
    I8_DLOAD = 12,
    I8_LOAD = 13,
    U8_DLOAD = 14,
    U8_LOAD = 15,
    F16_DLOAD = 16,
    F16_LOAD = 17,
    F32_DLOAD = 18,
    F32_LOAD = 19,
    I32_DLOAD = 20,
    I32_LOAD = 21,
    I8_STORE = 22,
    U8_STORE = 23,
    F16_STORE = 24,
    F32_STORE = 25,
    I32_STORE = 26,
    F32_ADD = 27,
    F32_SUB = 28,
    F32_MUL = 29,
    F32_MAX = 30,
    F32_MIN = 31,
    F32_GREATER = 32,
    F32_GREATER_EQUAL = 33,
    F32_LESS = 34,
    F32_LESS_EQUAL = 35,
    F32_EQUAL = 36,
    F32_SEL = 37,
    I32_AND = 38,
    I32_OR = 39,
    I32_NOT = 40,
    I32_XOR = 41,
    I32_XNOR = 42,
    BF16_ADDI = 43,
    BF16_SUBI = 44,
    BF16_MULI = 45,
    BF16_MAXI = 46,
    BF16_MINI = 47,
    BF16_MOVI = 48,
    BF16_GREATERI = 49,
    BF16_GREATER_EQUALI = 50,
    BF16_LESSI = 51,
    BF16_LESS_EQUALI = 52,
    BF16_EQUALI = 53,
    BF16_SELI = 54,
    F16_ADDI = 55,
    F16_SUBI = 56,
    F16_MULI = 57,
    F16_MAXI = 58,
    F16_MINI = 59,
    F16_MOVI = 60,
    F16_GREATERI = 61,
    F16_GREATER_EQUALI = 62,
    F16_LESSI = 63,
    F16_LESS_EQUALI = 64,
    F16_EQUALI = 65,
    F16_SELI = 66,
    I16_ADDI = 67,
    I16_SUBI = 68,
    I16_MULI = 69,
    I16_MAXI = 70,
    I16_MINI = 71,
    I16_MOVI = 72,
    I16_GREATERI = 73,
    I16_GREATER_EQUALI = 74,
    I16_LESSI = 75,
    I16_LESS_EQUALI = 76,
    I16_EQUALI = 77,
    I16_SELI = 78,
    F32_LUI = 79,
    F32_LI = 80,
    I32_ANDI = 81,
    I32_ORI = 82,
    I32_NOTI = 83,
    I32_XORI = 84,
    I32_XNORI = 85,
    I32_SHIFTL = 86,
    I32_SHIFTR = 87,
    I32_SHIFTLI = 88,
    I32_SHIFTRI = 89,
    F64_ADD = 90,
    F64_SUB = 91,
    F64_MUL = 92,
    F64_MAX = 93,
    F64_MIN = 94,
    F64_GREATER = 95,
    F64_GREATER_EQUAL = 96,
    F64_LESS = 97,
    F64_LESS_EQUAL = 98,
    F64_EQUAL = 99,
    F64_SEL = 100,
    I64_SHIFTL = 101,
    I64_SHIFTR = 102,
    I64_SHIFTLI = 103,
    I64_SHIFTRI = 104,
    I8_DLOAD_ADD = 105,
    I8_DLOAD_SUB = 106,
    I8_DLOAD_MUL = 107,
    I8_DLOAD_MAX = 108,
    I8_DLOAD_MIN = 109,
    I8_LOAD_ADD = 110,
    I8_LOAD_SUB = 111,
    I8_LOAD_MUL = 112,
    I8_LOAD_MAX = 113,
    I8_LOAD_MIN = 114,
    U8_DLOAD_ADD = 115,
    U8_DLOAD_SUB = 116,
    U8_DLOAD_MUL = 117,
    U8_DLOAD_MAX = 118,
    U8_DLOAD_MIN = 119,
    U8_LOAD_ADD = 120,
    U8_LOAD_SUB = 121,
    U8_LOAD_MUL = 122,
    U8_LOAD_MAX = 123,
    U8_LOAD_MIN = 124,
    F16_DLOAD_ADD = 125,
    F16_DLOAD_SUB = 126,
    F16_DLOAD_MUL = 127,
    F16_DLOAD_MAX = 128,
    F16_DLOAD_MIN = 129,
    F16_LOAD_ADD = 130,
    F16_LOAD_SUB = 131,
    F16_LOAD_MUL = 132,
    F16_LOAD_MAX = 133,
    F16_LOAD_MIN = 134,
    F32_DLOAD_ADD = 135,
    F32_DLOAD_SUB = 136,
    F32_DLOAD_MUL = 137,
    F32_DLOAD_MAX = 138,
    F32_DLOAD_MIN = 139,
    F32_LOAD_ADD = 140,
    F32_LOAD_SUB = 141,
    F32_LOAD_MUL = 142,
    F32_LOAD_MAX = 143,
    F32_LOAD_MIN = 144,
    I32_DLOAD_ADD = 145,
    I32_DLOAD_SUB = 146,
    I32_DLOAD_MUL = 147,
    I32_DLOAD_MAX = 148,
    I32_DLOAD_MIN = 149,
    I32_DLOAD_AND = 150,
    I32_DLOAD_OR = 151,
    I32_DLOAD_NOT = 152,
    I32_DLOAD_XOR = 153,
    I32_DLOAD_XNOR = 154,
    I32_LOAD_ADD = 155,
    I32_LOAD_SUB = 156,
    I32_LOAD_MUL = 157,
    I32_LOAD_MAX = 158,
    I32_LOAD_MIN = 159,
    I32_LOAD_AND = 160,
    I32_LOAD_OR = 161,
    I32_LOAD_NOT = 162,
    I32_LOAD_XOR = 163,
    I32_LOAD_XNOR = 164,
    I8_ADD_STORE = 165,
    I8_SUB_STORE = 166,
    I8_MUL_STORE = 167,
    I8_MAX_STORE = 168,
    I8_MIN_STORE = 169,
    U8_ADD_STORE = 170,
    U8_SUB_STORE = 171,
    U8_MUL_STORE = 172,
    U8_MAX_STORE = 173,
    U8_MIN_STORE = 174,
    F16_ADD_STORE = 175,
    F16_SUB_STORE = 176,
    F16_MUL_STORE = 177,
    F16_MAX_STORE = 178,
    F16_MIN_STORE = 179,
    F32_ADD_STORE = 180,
    F32_SUB_STORE = 181,
    F32_MUL_STORE = 182,
    F32_MAX_STORE = 183,
    F32_MIN_STORE = 184,
    I32_ADD_STORE = 185,
    I32_SUB_STORE = 186,
    I32_MUL_STORE = 187,
    I32_MAX_STORE = 188,
    I32_MIN_STORE = 189,
    I32_AND_STORE = 190,
    I32_OR_STORE = 191,
    I32_NOT_STORE = 192,
    I32_XOR_STORE = 193,
    I32_XNOR_STORE = 194,
    I8_DLOAD_ADD_STORE = 195,
    I8_DLOAD_SUB_STORE = 196,
    I8_DLOAD_MUL_STORE = 197,
    I8_DLOAD_MAX_STORE = 198,
    I8_DLOAD_MIN_STORE = 199,
    I8_LOAD_ADD_STORE = 200,
    I8_LOAD_SUB_STORE = 201,
    I8_LOAD_MUL_STORE = 202,
    I8_LOAD_MAX_STORE = 203,
    I8_LOAD_MIN_STORE = 204,
    U8_DLOAD_ADD_STORE = 205,
    U8_DLOAD_SUB_STORE = 206,
    U8_DLOAD_MUL_STORE = 207,
    U8_DLOAD_MAX_STORE = 208,
    U8_DLOAD_MIN_STORE = 209,
    U8_LOAD_ADD_STORE = 210,
    U8_LOAD_SUB_STORE = 211,
    U8_LOAD_MUL_STORE = 212,
    U8_LOAD_MAX_STORE = 213,
    U8_LOAD_MIN_STORE = 214,
    F16_DLOAD_ADD_STORE = 215,
    F16_DLOAD_SUB_STORE = 216,
    F16_DLOAD_MUL_STORE = 217,
    F16_DLOAD_MAX_STORE = 218,
    F16_DLOAD_MIN_STORE = 219,
    F16_LOAD_ADD_STORE = 220,
    F16_LOAD_SUB_STORE = 221,
    F16_LOAD_MUL_STORE = 222,
    F16_LOAD_MAX_STORE = 223,
    F16_LOAD_MIN_STORE = 224,
    F32_DLOAD_ADD_STORE = 225,
    F32_DLOAD_SUB_STORE = 226,
    F32_DLOAD_MUL_STORE = 227,
    F32_DLOAD_MAX_STORE = 228,
    F32_DLOAD_MIN_STORE = 229,
    F32_LOAD_ADD_STORE = 230,
    F32_LOAD_SUB_STORE = 231,
    F32_LOAD_MUL_STORE = 232,
    F32_LOAD_MAX_STORE = 233,
    F32_LOAD_MIN_STORE = 234,
    I32_DLOAD_ADD_STORE = 235,
    I32_DLOAD_SUB_STORE = 236,
    I32_DLOAD_MUL_STORE = 237,
    I32_DLOAD_MAX_STORE = 238,
    I32_DLOAD_MIN_STORE = 239,
    I32_DLOAD_AND_STORE = 240,
    I32_DLOAD_OR_STORE = 241,
    I32_DLOAD_NOT_STORE = 242,
    I32_DLOAD_XOR_STORE = 243,
    I32_DLOAD_XNOR_STORE = 244,
    I32_LOAD_ADD_STORE = 245,
    I32_LOAD_SUB_STORE = 246,
    I32_LOAD_MUL_STORE = 247,
    I32_LOAD_MAX_STORE = 248,
    I32_LOAD_MIN_STORE = 249,
    I32_LOAD_AND_STORE = 250,
    I32_LOAD_OR_STORE = 251,
    I32_LOAD_NOT_STORE = 252,
    I32_LOAD_XOR_STORE = 253,
    I32_LOAD_XNOR_STORE = 254,
    S_ADDI = 255,
    S_ADDIU = 256,
    S_SUBI = 257,
    S_ANDI = 258,
    S_ORI = 259,
    S_XORI = 260,
    S_SGTI = 261,
    S_SLTI = 262,
    S_SEQI = 263,
    S_SGEI = 264,
    S_SLEI = 265,
    S_SRAI = 266,
    S_SRLI = 267,
    S_SLLI = 268,
    S_LUI = 269,
    S_ADD = 270,
    S_ADDU = 271,
    S_SUB = 272,
    S_MUL = 273,
    S_MULH = 274,
    S_MULHU = 275,
    S_MIN = 276,
    S_MAX = 277,
    S_MINU = 278,
    S_MAXU = 279,
    S_AND = 280,
    S_OR = 281,
    S_XOR = 282,
    S_SGT = 283,
    S_SLT = 284,
    S_SEQ = 285,
    S_SGE = 286,
    S_SLE = 287,
    S_CMPSEL = 288,
    S_SRA = 289,
    S_SRL = 290,
    S_SLL = 291,
    S_JUMP = 292,
    S_JAL = 293,
    S_JR = 294,
    S_BNE = 295,
    S_BEQ = 296,
    S_BLT = 297,
    S_BLTU = 298,
    S_BGE = 299,
    S_BGEU = 300,
    S_SETCFG = 301,
    S_SETRF = 302,
    INVALID = 65536
  };

  static const std::map<OpCodeType, OpCodeItem>& getOpCodeMap() {
    static const std::map<OpCodeType, OpCodeItem> opCodeMap = {
      {OpCodeType::SET_LOAD_BASE,        {"set_load_base",        0xdf000000, 0x01000000}},
      {OpCodeType::SET_LOAD2_BASE,       {"set_load2_base",       0xdf000000, 0x04000000}},
      {OpCodeType::SET_STORE_BASE,       {"set_store_base",       0xdf000000, 0x07000000}},
      {OpCodeType::SET_LOAD_GLB,         {"set_load_glb",         0xdf000001, 0x02000000}},
      {OpCodeType::SET_STORE_GLB,        {"set_store_glb",        0xdf000001, 0x08000000}},
      {OpCodeType::SET_QUANTIZE,         {"set_quantize",         0xdfff0000, 0x09000000}},
      {OpCodeType::SET_DEQUANTIZE,       {"set_dequantize",       0xdf000000, 0x03000000}},
      {OpCodeType::SET_DEQUANTIZE2,      {"set_dequantize2",      0xdf000000, 0x06000000}},
      {OpCodeType::SET_FS,               {"set_fs",               0xdfffff00, 0x0a000000}},
      {OpCodeType::SET_AAI,              {"set_aai",              0xff070000, 0x0b000000}},
      {OpCodeType::VPU_NOP,              {"vpu_nop",              0xffffffff, 0x00000000}},
      {OpCodeType::VPU_END,              {"vpu_end",              0xfffffffe, 0xff000000}},
      {OpCodeType::I8_DLOAD,             {"i8_dload",             0xff0f0000, 0xc0020000}},
      {OpCodeType::I8_LOAD,              {"i8_load",              0xffcf0000, 0xc0000000}},
      {OpCodeType::U8_DLOAD,             {"u8_dload",             0xff0f0000, 0xd0020000}},
      {OpCodeType::U8_LOAD,              {"u8_load",              0xffcf0000, 0xd0000000}},
      {OpCodeType::F16_DLOAD,            {"f16_dload",            0xff0f0000, 0xe0020000}},
      {OpCodeType::F16_LOAD,             {"f16_load",             0xffcf0000, 0xe0000000}},
      {OpCodeType::F32_DLOAD,            {"f32_dload",            0xff0f0000, 0xf0020000}},
      {OpCodeType::F32_LOAD,             {"f32_load",             0xffcf0000, 0xf0000000}},
      {OpCodeType::I32_DLOAD,            {"i32_dload",            0xff0f0000, 0xd4020000}},
      {OpCodeType::I32_LOAD,             {"i32_load",             0xffcf0000, 0xd4000000}},
      {OpCodeType::I8_STORE,             {"i8_store",             0xffcf0000, 0xc8000000}},
      {OpCodeType::U8_STORE,             {"u8_store",             0xffcf0000, 0xd8000000}},
      {OpCodeType::F16_STORE,            {"f16_store",            0xffcf0000, 0xe8000000}},
      {OpCodeType::F32_STORE,            {"f32_store",            0xffcf0000, 0xf8000000}},
      {OpCodeType::I32_STORE,            {"i32_store",            0xffcf0000, 0xdc000000}},
      {OpCodeType::F32_ADD,              {"f32_add",              0xff007fff, 0x90000000}},
      {OpCodeType::F32_SUB,              {"f32_sub",              0xff007fff, 0x91000000}},
      {OpCodeType::F32_MUL,              {"f32_mul",              0xff007fff, 0x92000000}},
      {OpCodeType::F32_MAX,              {"f32_max",              0xff007fff, 0x93000000}},
      {OpCodeType::F32_MIN,              {"f32_min",              0xff007fff, 0x94000000}},
      {OpCodeType::F32_GREATER,          {"f32_greater",          0xffe07fff, 0x97000000}},
      {OpCodeType::F32_GREATER_EQUAL,    {"f32_greater_equal",    0xffe07fff, 0x9a000000}},
      {OpCodeType::F32_LESS,             {"f32_less",             0xffe07fff, 0x99000000}},
      {OpCodeType::F32_LESS_EQUAL,       {"f32_less_equal",       0xffe07fff, 0x9b000000}},
      {OpCodeType::F32_EQUAL,            {"f32_equal",            0xffe07fff, 0x98000000}},
      {OpCodeType::F32_SEL,              {"f32_sel",              0xff007fff, 0x9c000000}},
      {OpCodeType::I32_AND,              {"i32_and",              0xff007fff, 0x80000000}},
      {OpCodeType::I32_OR,               {"i32_or",               0xff007fff, 0x81000000}},
      {OpCodeType::I32_NOT,              {"i32_not",              0xff007fff, 0x82000000}},
      {OpCodeType::I32_XOR,              {"i32_xor",              0xff007fff, 0x83000000}},
      {OpCodeType::I32_XNOR,             {"i32_xnor",             0xff007fff, 0x84000000}},
      {OpCodeType::BF16_ADDI,            {"bf16_addi",            0xff030000, 0xb0000000}},
      {OpCodeType::BF16_SUBI,            {"bf16_subi",            0xff030000, 0xb1000000}},
      {OpCodeType::BF16_MULI,            {"bf16_muli",            0xff030000, 0xb2000000}},
      {OpCodeType::BF16_MAXI,            {"bf16_maxi",            0xff030000, 0xb3000000}},
      {OpCodeType::BF16_MINI,            {"bf16_mini",            0xff030000, 0xb4000000}},
      {OpCodeType::BF16_MOVI,            {"bf16_movi",            0xff1f0000, 0xb5000000}},
      {OpCodeType::BF16_GREATERI,        {"bf16_greateri",        0xffe30000, 0xb7000000}},
      {OpCodeType::BF16_GREATER_EQUALI,  {"bf16_greater_equali",  0xffe30000, 0xba000000}},
      {OpCodeType::BF16_LESSI,           {"bf16_lessi",           0xffe30000, 0xb9000000}},
      {OpCodeType::BF16_LESS_EQUALI,     {"bf16_less_equali",     0xffe30000, 0xbb000000}},
      {OpCodeType::BF16_EQUALI,          {"bf16_equali",          0xffe30000, 0xb8000000}},
      {OpCodeType::BF16_SELI,            {"bf16_seli",            0xff030000, 0xbc000000}},
      {OpCodeType::F16_ADDI,             {"f16_addi",             0xff030000, 0xb0010000}},
      {OpCodeType::F16_SUBI,             {"f16_subi",             0xff030000, 0xb1010000}},
      {OpCodeType::F16_MULI,             {"f16_muli",             0xff030000, 0xb2010000}},
      {OpCodeType::F16_MAXI,             {"f16_maxi",             0xff030000, 0xb3010000}},
      {OpCodeType::F16_MINI,             {"f16_mini",             0xff030000, 0xb4010000}},
      {OpCodeType::F16_MOVI,             {"f16_movi",             0xff1f0000, 0xb5010000}},
      {OpCodeType::F16_GREATERI,         {"f16_greateri",         0xffe30000, 0xb7010000}},
      {OpCodeType::F16_GREATER_EQUALI,   {"f16_greater_equali",   0xffe30000, 0xba010000}},
      {OpCodeType::F16_LESSI,            {"f16_lessi",            0xffe30000, 0xb9010000}},
      {OpCodeType::F16_LESS_EQUALI,      {"f16_less_equali",      0xffe30000, 0xbb010000}},
      {OpCodeType::F16_EQUALI,           {"f16_equali",           0xffe30000, 0xb8010000}},
      {OpCodeType::F16_SELI,             {"f16_seli",             0xff030000, 0xbc010000}},
      {OpCodeType::I16_ADDI,             {"i16_addi",             0xff030000, 0xb0020000}},
      {OpCodeType::I16_SUBI,             {"i16_subi",             0xff030000, 0xb1020000}},
      {OpCodeType::I16_MULI,             {"i16_muli",             0xff030000, 0xb2020000}},
      {OpCodeType::I16_MAXI,             {"i16_maxi",             0xff030000, 0xb3020000}},
      {OpCodeType::I16_MINI,             {"i16_mini",             0xff030000, 0xb4020000}},
      {OpCodeType::I16_MOVI,             {"i16_movi",             0xff1f0000, 0xb5020000}},
      {OpCodeType::I16_GREATERI,         {"i16_greateri",         0xffe30000, 0xb7020000}},
      {OpCodeType::I16_GREATER_EQUALI,   {"i16_greater_equali",   0xffe30000, 0xba020000}},
      {OpCodeType::I16_LESSI,            {"i16_lessi",            0xffe30000, 0xb9020000}},
      {OpCodeType::I16_LESS_EQUALI,      {"i16_less_equali",      0xffe30000, 0xbb020000}},
      {OpCodeType::I16_EQUALI,           {"i16_equali",           0xffe30000, 0xb8020000}},
      {OpCodeType::I16_SELI,             {"i16_seli",             0xff030000, 0xbc020000}},
      {OpCodeType::F32_LUI,              {"f32_lui",              0xff1f0000, 0xbd000000}},
      {OpCodeType::F32_LI,               {"f32_li",               0xff1f0000, 0xbe000000}},
      {OpCodeType::I32_ANDI,             {"i32_andi",             0xff030000, 0xa0000000}},
      {OpCodeType::I32_ORI,              {"i32_ori",              0xff030000, 0xa1000000}},
      {OpCodeType::I32_NOTI,             {"i32_noti",             0xff030000, 0xa2000000}},
      {OpCodeType::I32_XORI,             {"i32_xori",             0xff030000, 0xa3000000}},
      {OpCodeType::I32_XNORI,            {"i32_xnori",            0xff030000, 0xa4000000}},
      {OpCodeType::I32_SHIFTL,           {"i32_shiftl",           0xff007fff, 0x85000000}},
      {OpCodeType::I32_SHIFTR,           {"i32_shiftr",           0xff007fff, 0x86000000}},
      {OpCodeType::I32_SHIFTLI,          {"i32_shiftli",          0xff03ff00, 0xa5000000}},
      {OpCodeType::I32_SHIFTRI,          {"i32_shiftri",          0xff03ff00, 0xa6000000}},
      {OpCodeType::F64_ADD,              {"f64_add",              0xff007fff, 0x90004000}},
      {OpCodeType::F64_SUB,              {"f64_sub",              0xff007fff, 0x91004000}},
      {OpCodeType::F64_MUL,              {"f64_mul",              0xff007fff, 0x92004000}},
      {OpCodeType::F64_MAX,              {"f64_max",              0xff007fff, 0x93004000}},
      {OpCodeType::F64_MIN,              {"f64_min",              0xff007fff, 0x94004000}},
      {OpCodeType::F64_GREATER,          {"f64_greater",          0xffe07fff, 0x97004000}},
      {OpCodeType::F64_GREATER_EQUAL,    {"f64_greater_equal",    0xffe07fff, 0x9a004000}},
      {OpCodeType::F64_LESS,             {"f64_less",             0xffe07fff, 0x99004000}},
      {OpCodeType::F64_LESS_EQUAL,       {"f64_less_equal",       0xffe07fff, 0x9b004000}},
      {OpCodeType::F64_EQUAL,            {"f64_equal",            0xffe07fff, 0x98004000}},
      {OpCodeType::F64_SEL,              {"f64_sel",              0xff007fff, 0x9c004000}},
      {OpCodeType::I64_SHIFTL,           {"i64_shiftl",           0xff007fff, 0x85004000}},
      {OpCodeType::I64_SHIFTR,           {"i64_shiftr",           0xff007fff, 0x86004000}},
      {OpCodeType::I64_SHIFTLI,          {"i64_shiftli",          0xff03ff00, 0xa5004000}},
      {OpCodeType::I64_SHIFTRI,          {"i64_shiftri",          0xff03ff00, 0xa6004000}},
      {OpCodeType::I8_DLOAD_ADD,         {"i8_dload_add",         0xff3f0000, 0xc0030000}},
      {OpCodeType::I8_DLOAD_SUB,         {"i8_dload_sub",         0xff3f0000, 0xc0070000}},
      {OpCodeType::I8_DLOAD_MUL,         {"i8_dload_mul",         0xff3f0000, 0xc1030000}},
      {OpCodeType::I8_DLOAD_MAX,         {"i8_dload_max",         0xff3f0000, 0xc1070000}},
      {OpCodeType::I8_DLOAD_MIN,         {"i8_dload_min",         0xff3f0000, 0xc4030000}},
      {OpCodeType::I8_LOAD_ADD,          {"i8_load_add",          0xff0f0000, 0xc0010000}},
      {OpCodeType::I8_LOAD_SUB,          {"i8_load_sub",          0xff0f0000, 0xc0050000}},
      {OpCodeType::I8_LOAD_MUL,          {"i8_load_mul",          0xff0f0000, 0xc1010000}},
      {OpCodeType::I8_LOAD_MAX,          {"i8_load_max",          0xff0f0000, 0xc1050000}},
      {OpCodeType::I8_LOAD_MIN,          {"i8_load_min",          0xff0f0000, 0xc4010000}},
      {OpCodeType::U8_DLOAD_ADD,         {"u8_dload_add",         0xff3f0000, 0xd0030000}},
      {OpCodeType::U8_DLOAD_SUB,         {"u8_dload_sub",         0xff3f0000, 0xd0070000}},
      {OpCodeType::U8_DLOAD_MUL,         {"u8_dload_mul",         0xff3f0000, 0xd1030000}},
      {OpCodeType::U8_DLOAD_MAX,         {"u8_dload_max",         0xff3f0000, 0xd1070000}},
      {OpCodeType::U8_DLOAD_MIN,         {"u8_dload_min",         0xff3f0000, 0xd4030000}},
      {OpCodeType::U8_LOAD_ADD,          {"u8_load_add",          0xff0f0000, 0xd0010000}},
      {OpCodeType::U8_LOAD_SUB,          {"u8_load_sub",          0xff0f0000, 0xd0050000}},
      {OpCodeType::U8_LOAD_MUL,          {"u8_load_mul",          0xff0f0000, 0xd1010000}},
      {OpCodeType::U8_LOAD_MAX,          {"u8_load_max",          0xff0f0000, 0xd1050000}},
      {OpCodeType::U8_LOAD_MIN,          {"u8_load_min",          0xff0f0000, 0xd4010000}},
      {OpCodeType::F16_DLOAD_ADD,        {"f16_dload_add",        0xff3f0000, 0xe0030000}},
      {OpCodeType::F16_DLOAD_SUB,        {"f16_dload_sub",        0xff3f0000, 0xe0070000}},
      {OpCodeType::F16_DLOAD_MUL,        {"f16_dload_mul",        0xff3f0000, 0xe1030000}},
      {OpCodeType::F16_DLOAD_MAX,        {"f16_dload_max",        0xff3f0000, 0xe1070000}},
      {OpCodeType::F16_DLOAD_MIN,        {"f16_dload_min",        0xff3f0000, 0xe4030000}},
      {OpCodeType::F16_LOAD_ADD,         {"f16_load_add",         0xff0f0000, 0xe0010000}},
      {OpCodeType::F16_LOAD_SUB,         {"f16_load_sub",         0xff0f0000, 0xe0050000}},
      {OpCodeType::F16_LOAD_MUL,         {"f16_load_mul",         0xff0f0000, 0xe1010000}},
      {OpCodeType::F16_LOAD_MAX,         {"f16_load_max",         0xff0f0000, 0xe1050000}},
      {OpCodeType::F16_LOAD_MIN,         {"f16_load_min",         0xff0f0000, 0xe4010000}},
      {OpCodeType::F32_DLOAD_ADD,        {"f32_dload_add",        0xff3f0000, 0xf0030000}},
      {OpCodeType::F32_DLOAD_SUB,        {"f32_dload_sub",        0xff3f0000, 0xf0070000}},
      {OpCodeType::F32_DLOAD_MUL,        {"f32_dload_mul",        0xff3f0000, 0xf1030000}},
      {OpCodeType::F32_DLOAD_MAX,        {"f32_dload_max",        0xff3f0000, 0xf1070000}},
      {OpCodeType::F32_DLOAD_MIN,        {"f32_dload_min",        0xff3f0000, 0xf4030000}},
      {OpCodeType::F32_LOAD_ADD,         {"f32_load_add",         0xff0f0000, 0xf0010000}},
      {OpCodeType::F32_LOAD_SUB,         {"f32_load_sub",         0xff0f0000, 0xf0050000}},
      {OpCodeType::F32_LOAD_MUL,         {"f32_load_mul",         0xff0f0000, 0xf1010000}},
      {OpCodeType::F32_LOAD_MAX,         {"f32_load_max",         0xff0f0000, 0xf1050000}},
      {OpCodeType::F32_LOAD_MIN,         {"f32_load_min",         0xff0f0000, 0xf4010000}},
      {OpCodeType::I32_DLOAD_ADD,        {"i32_dload_add",        0xff3f0000, 0xd00b0000}},
      {OpCodeType::I32_DLOAD_SUB,        {"i32_dload_sub",        0xff3f0000, 0xd00f0000}},
      {OpCodeType::I32_DLOAD_MUL,        {"i32_dload_mul",        0xff3f0000, 0xd10b0000}},
      {OpCodeType::I32_DLOAD_MAX,        {"i32_dload_max",        0xff3f0000, 0xd10f0000}},
      {OpCodeType::I32_DLOAD_MIN,        {"i32_dload_min",        0xff3f0000, 0xd40b0000}},
      {OpCodeType::I32_DLOAD_AND,        {"i32_dload_and",        0xff3f0000, 0xd20b0000}},
      {OpCodeType::I32_DLOAD_OR,         {"i32_dload_or",         0xff3f0000, 0xd20f0000}},
      {OpCodeType::I32_DLOAD_NOT,        {"i32_dload_not",        0xff3f0000, 0xd30b0000}},
      {OpCodeType::I32_DLOAD_XOR,        {"i32_dload_xor",        0xff3f0000, 0xd30f0000}},
      {OpCodeType::I32_DLOAD_XNOR,       {"i32_dload_xnor",       0xff3f0000, 0xd60b0000}},
      {OpCodeType::I32_LOAD_ADD,         {"i32_load_add",         0xff0f0000, 0xd0090000}},
      {OpCodeType::I32_LOAD_SUB,         {"i32_load_sub",         0xff0f0000, 0xd00d0000}},
      {OpCodeType::I32_LOAD_MUL,         {"i32_load_mul",         0xff0f0000, 0xd1090000}},
      {OpCodeType::I32_LOAD_MAX,         {"i32_load_max",         0xff0f0000, 0xd10d0000}},
      {OpCodeType::I32_LOAD_MIN,         {"i32_load_min",         0xff0f0000, 0xd4090000}},
      {OpCodeType::I32_LOAD_AND,         {"i32_load_and",         0xff0f0000, 0xd2090000}},
      {OpCodeType::I32_LOAD_OR,          {"i32_load_or",          0xff0f0000, 0xd20d0000}},
      {OpCodeType::I32_LOAD_NOT,         {"i32_load_not",         0xff0f0000, 0xd3090000}},
      {OpCodeType::I32_LOAD_XOR,         {"i32_load_xor",         0xff0f0000, 0xd30d0000}},
      {OpCodeType::I32_LOAD_XNOR,        {"i32_load_xnor",        0xff0f0000, 0xd6090000}},
      {OpCodeType::I8_ADD_STORE,         {"i8_add_store",         0xff0f0000, 0xc8010000}},
      {OpCodeType::I8_SUB_STORE,         {"i8_sub_store",         0xff0f0000, 0xc8050000}},
      {OpCodeType::I8_MUL_STORE,         {"i8_mul_store",         0xff0f0000, 0xc9010000}},
      {OpCodeType::I8_MAX_STORE,         {"i8_max_store",         0xff0f0000, 0xc9050000}},
      {OpCodeType::I8_MIN_STORE,         {"i8_min_store",         0xff0f0000, 0xcc010000}},
      {OpCodeType::U8_ADD_STORE,         {"u8_add_store",         0xff0f0000, 0xd8010000}},
      {OpCodeType::U8_SUB_STORE,         {"u8_sub_store",         0xff0f0000, 0xd8050000}},
      {OpCodeType::U8_MUL_STORE,         {"u8_mul_store",         0xff0f0000, 0xd9010000}},
      {OpCodeType::U8_MAX_STORE,         {"u8_max_store",         0xff0f0000, 0xd9050000}},
      {OpCodeType::U8_MIN_STORE,         {"u8_min_store",         0xff0f0000, 0xdc010000}},
      {OpCodeType::F16_ADD_STORE,        {"f16_add_store",        0xff0f0000, 0xe8010000}},
      {OpCodeType::F16_SUB_STORE,        {"f16_sub_store",        0xff0f0000, 0xe8050000}},
      {OpCodeType::F16_MUL_STORE,        {"f16_mul_store",        0xff0f0000, 0xe9010000}},
      {OpCodeType::F16_MAX_STORE,        {"f16_max_store",        0xff0f0000, 0xe9050000}},
      {OpCodeType::F16_MIN_STORE,        {"f16_min_store",        0xff0f0000, 0xec010000}},
      {OpCodeType::F32_ADD_STORE,        {"f32_add_store",        0xff0f0000, 0xf8010000}},
      {OpCodeType::F32_SUB_STORE,        {"f32_sub_store",        0xff0f0000, 0xf8050000}},
      {OpCodeType::F32_MUL_STORE,        {"f32_mul_store",        0xff0f0000, 0xf9010000}},
      {OpCodeType::F32_MAX_STORE,        {"f32_max_store",        0xff0f0000, 0xf9050000}},
      {OpCodeType::F32_MIN_STORE,        {"f32_min_store",        0xff0f0000, 0xfc010000}},
      {OpCodeType::I32_ADD_STORE,        {"i32_add_store",        0xff0f0000, 0xd8090000}},
      {OpCodeType::I32_SUB_STORE,        {"i32_sub_store",        0xff0f0000, 0xd80d0000}},
      {OpCodeType::I32_MUL_STORE,        {"i32_mul_store",        0xff0f0000, 0xd9090000}},
      {OpCodeType::I32_MAX_STORE,        {"i32_max_store",        0xff0f0000, 0xd90d0000}},
      {OpCodeType::I32_MIN_STORE,        {"i32_min_store",        0xff0f0000, 0xdc090000}},
      {OpCodeType::I32_AND_STORE,        {"i32_and_store",        0xff0f0000, 0xda090000}},
      {OpCodeType::I32_OR_STORE,         {"i32_or_store",         0xff0f0000, 0xda0d0000}},
      {OpCodeType::I32_NOT_STORE,        {"i32_not_store",        0xff0f0000, 0xdb090000}},
      {OpCodeType::I32_XOR_STORE,        {"i32_xor_store",        0xff0f0000, 0xdb0d0000}},
      {OpCodeType::I32_XNOR_STORE,       {"i32_xnor_store",       0xff0f0000, 0xde090000}},
      {OpCodeType::I8_DLOAD_ADD_STORE,   {"i8_dload_add_store",   0xffff0000, 0xc00a0000}},
      {OpCodeType::I8_DLOAD_SUB_STORE,   {"i8_dload_sub_store",   0xffff0000, 0xc00e0000}},
      {OpCodeType::I8_DLOAD_MUL_STORE,   {"i8_dload_mul_store",   0xffff0000, 0xc10a0000}},
      {OpCodeType::I8_DLOAD_MAX_STORE,   {"i8_dload_max_store",   0xffff0000, 0xc10e0000}},
      {OpCodeType::I8_DLOAD_MIN_STORE,   {"i8_dload_min_store",   0xffff0000, 0xc40a0000}},
      {OpCodeType::I8_LOAD_ADD_STORE,    {"i8_load_add_store",    0xffcf0000, 0xc0080000}},
      {OpCodeType::I8_LOAD_SUB_STORE,    {"i8_load_sub_store",    0xffcf0000, 0xc00c0000}},
      {OpCodeType::I8_LOAD_MUL_STORE,    {"i8_load_mul_store",    0xffcf0000, 0xc1080000}},
      {OpCodeType::I8_LOAD_MAX_STORE,    {"i8_load_max_store",    0xffcf0000, 0xc10c0000}},
      {OpCodeType::I8_LOAD_MIN_STORE,    {"i8_load_min_store",    0xffcf0000, 0xc4080000}},
      {OpCodeType::U8_DLOAD_ADD_STORE,   {"u8_dload_add_store",   0xffff0000, 0xd00a0000}},
      {OpCodeType::U8_DLOAD_SUB_STORE,   {"u8_dload_sub_store",   0xffff0000, 0xd00e0000}},
      {OpCodeType::U8_DLOAD_MUL_STORE,   {"u8_dload_mul_store",   0xffff0000, 0xd10a0000}},
      {OpCodeType::U8_DLOAD_MAX_STORE,   {"u8_dload_max_store",   0xffff0000, 0xd10e0000}},
      {OpCodeType::U8_DLOAD_MIN_STORE,   {"u8_dload_min_store",   0xffff0000, 0xd40a0000}},
      {OpCodeType::U8_LOAD_ADD_STORE,    {"u8_load_add_store",    0xffcf0000, 0xd0080000}},
      {OpCodeType::U8_LOAD_SUB_STORE,    {"u8_load_sub_store",    0xffcf0000, 0xd00c0000}},
      {OpCodeType::U8_LOAD_MUL_STORE,    {"u8_load_mul_store",    0xffcf0000, 0xd1080000}},
      {OpCodeType::U8_LOAD_MAX_STORE,    {"u8_load_max_store",    0xffcf0000, 0xd10c0000}},
      {OpCodeType::U8_LOAD_MIN_STORE,    {"u8_load_min_store",    0xffcf0000, 0xd4080000}},
      {OpCodeType::F16_DLOAD_ADD_STORE,  {"f16_dload_add_store",  0xffff0000, 0xe00a0000}},
      {OpCodeType::F16_DLOAD_SUB_STORE,  {"f16_dload_sub_store",  0xffff0000, 0xe00e0000}},
      {OpCodeType::F16_DLOAD_MUL_STORE,  {"f16_dload_mul_store",  0xffff0000, 0xe10a0000}},
      {OpCodeType::F16_DLOAD_MAX_STORE,  {"f16_dload_max_store",  0xffff0000, 0xe10e0000}},
      {OpCodeType::F16_DLOAD_MIN_STORE,  {"f16_dload_min_store",  0xffff0000, 0xe40a0000}},
      {OpCodeType::F16_LOAD_ADD_STORE,   {"f16_load_add_store",   0xffcf0000, 0xe0080000}},
      {OpCodeType::F16_LOAD_SUB_STORE,   {"f16_load_sub_store",   0xffcf0000, 0xe00c0000}},
      {OpCodeType::F16_LOAD_MUL_STORE,   {"f16_load_mul_store",   0xffcf0000, 0xe1080000}},
      {OpCodeType::F16_LOAD_MAX_STORE,   {"f16_load_max_store",   0xffcf0000, 0xe10c0000}},
      {OpCodeType::F16_LOAD_MIN_STORE,   {"f16_load_min_store",   0xffcf0000, 0xe4080000}},
      {OpCodeType::F32_DLOAD_ADD_STORE,  {"f32_dload_add_store",  0xffff0000, 0xf00a0000}},
      {OpCodeType::F32_DLOAD_SUB_STORE,  {"f32_dload_sub_store",  0xffff0000, 0xf00e0000}},
      {OpCodeType::F32_DLOAD_MUL_STORE,  {"f32_dload_mul_store",  0xffff0000, 0xf10a0000}},
      {OpCodeType::F32_DLOAD_MAX_STORE,  {"f32_dload_max_store",  0xffff0000, 0xf10e0000}},
      {OpCodeType::F32_DLOAD_MIN_STORE,  {"f32_dload_min_store",  0xffff0000, 0xf40a0000}},
      {OpCodeType::F32_LOAD_ADD_STORE,   {"f32_load_add_store",   0xffcf0000, 0xf0080000}},
      {OpCodeType::F32_LOAD_SUB_STORE,   {"f32_load_sub_store",   0xffcf0000, 0xf00c0000}},
      {OpCodeType::F32_LOAD_MUL_STORE,   {"f32_load_mul_store",   0xffcf0000, 0xf1080000}},
      {OpCodeType::F32_LOAD_MAX_STORE,   {"f32_load_max_store",   0xffcf0000, 0xf10c0000}},
      {OpCodeType::F32_LOAD_MIN_STORE,   {"f32_load_min_store",   0xffcf0000, 0xf4080000}},
      {OpCodeType::I32_DLOAD_ADD_STORE,  {"i32_dload_add_store",  0xffff0000, 0xd80a0000}},
      {OpCodeType::I32_DLOAD_SUB_STORE,  {"i32_dload_sub_store",  0xffff0000, 0xd80e0000}},
      {OpCodeType::I32_DLOAD_MUL_STORE,  {"i32_dload_mul_store",  0xffff0000, 0xd90a0000}},
      {OpCodeType::I32_DLOAD_MAX_STORE,  {"i32_dload_max_store",  0xffff0000, 0xd90e0000}},
      {OpCodeType::I32_DLOAD_MIN_STORE,  {"i32_dload_min_store",  0xffff0000, 0xdc0a0000}},
      {OpCodeType::I32_DLOAD_AND_STORE,  {"i32_dload_and_store",  0xffff0000, 0xda0a0000}},
      {OpCodeType::I32_DLOAD_OR_STORE,   {"i32_dload_or_store",   0xffff0000, 0xda0e0000}},
      {OpCodeType::I32_DLOAD_NOT_STORE,  {"i32_dload_not_store",  0xffff0000, 0xdb0a0000}},
      {OpCodeType::I32_DLOAD_XOR_STORE,  {"i32_dload_xor_store",  0xffff0000, 0xdb0e0000}},
      {OpCodeType::I32_DLOAD_XNOR_STORE, {"i32_dload_xnor_store", 0xffff0000, 0xde0a0000}},
      {OpCodeType::I32_LOAD_ADD_STORE,   {"i32_load_add_store",   0xffcf0000, 0xd8080000}},
      {OpCodeType::I32_LOAD_SUB_STORE,   {"i32_load_sub_store",   0xffcf0000, 0xd80c0000}},
      {OpCodeType::I32_LOAD_MUL_STORE,   {"i32_load_mul_store",   0xffcf0000, 0xd9080000}},
      {OpCodeType::I32_LOAD_MAX_STORE,   {"i32_load_max_store",   0xffcf0000, 0xd90c0000}},
      {OpCodeType::I32_LOAD_MIN_STORE,   {"i32_load_min_store",   0xffcf0000, 0xdc080000}},
      {OpCodeType::I32_LOAD_AND_STORE,   {"i32_load_and_store",   0xffcf0000, 0xda080000}},
      {OpCodeType::I32_LOAD_OR_STORE,    {"i32_load_or_store",    0xffcf0000, 0xda0c0000}},
      {OpCodeType::I32_LOAD_NOT_STORE,   {"i32_load_not_store",   0xffcf0000, 0xdb080000}},
      {OpCodeType::I32_LOAD_XOR_STORE,   {"i32_load_xor_store",   0xffcf0000, 0xdb0c0000}},
      {OpCodeType::I32_LOAD_XNOR_STORE,  {"i32_load_xnor_store",  0xffcf0000, 0xde080000}},
      {OpCodeType::S_ADDI,               {"s_addi",               0xff000000, 0x40000000}},
      {OpCodeType::S_ADDIU,              {"s_addiu",              0xff000000, 0x41000000}},
      {OpCodeType::S_SUBI,               {"s_subi",               0xff000000, 0x42000000}},
      {OpCodeType::S_ANDI,               {"s_andi",               0xff000000, 0x43000000}},
      {OpCodeType::S_ORI,                {"s_ori",                0xff000000, 0x44000000}},
      {OpCodeType::S_XORI,               {"s_xori",               0xff000000, 0x45000000}},
      {OpCodeType::S_SGTI,               {"s_sgti",               0xff000000, 0x46000000}},
      {OpCodeType::S_SLTI,               {"s_slti",               0xff000000, 0x47000000}},
      {OpCodeType::S_SEQI,               {"s_seqi",               0xff000000, 0x48000000}},
      {OpCodeType::S_SGEI,               {"s_sgei",               0xff000000, 0x49000000}},
      {OpCodeType::S_SLEI,               {"s_slei",               0xff000000, 0x4a000000}},
      {OpCodeType::S_SRAI,               {"s_srai",               0xff000000, 0x4b000000}},
      {OpCodeType::S_SRLI,               {"s_srli",               0xff000000, 0x4c000000}},
      {OpCodeType::S_SLLI,               {"s_slli",               0xff000000, 0x4d000000}},
      {OpCodeType::S_LUI,                {"s_lui",                0xfff00000, 0x4e000000}},
      {OpCodeType::S_ADD,                {"s_add",                0xff000fff, 0x51000000}},
      {OpCodeType::S_ADDU,               {"s_addu",               0xff000fff, 0x52000000}},
      {OpCodeType::S_SUB,                {"s_sub",                0xff000fff, 0x53000000}},
      {OpCodeType::S_MUL,                {"s_mul",                0xff000fff, 0x54000000}},
      {OpCodeType::S_MULH,               {"s_mulh",               0xff000fff, 0x55000000}},
      {OpCodeType::S_MULHU,              {"s_mulhu",              0xff000fff, 0x56000000}},
      {OpCodeType::S_MIN,                {"s_min",                0xff000fff, 0x57000000}},
      {OpCodeType::S_MAX,                {"s_max",                0xff000fff, 0x58000000}},
      {OpCodeType::S_MINU,               {"s_minu",               0xff000fff, 0x59000000}},
      {OpCodeType::S_MAXU,               {"s_maxu",               0xff000fff, 0x5a000000}},
      {OpCodeType::S_AND,                {"s_and",                0xff000fff, 0x5b000000}},
      {OpCodeType::S_OR,                 {"s_or",                 0xff000fff, 0x5c000000}},
      {OpCodeType::S_XOR,                {"s_xor",                0xff000fff, 0x5d000000}},
      {OpCodeType::S_SGT,                {"s_sgt",                0xff000fff, 0x5e000000}},
      {OpCodeType::S_SLT,                {"s_slt",                0xff000fff, 0x5f000000}},
      {OpCodeType::S_SEQ,                {"s_seq",                0xff000fff, 0x60000000}},
      {OpCodeType::S_SGE,                {"s_sge",                0xff000fff, 0x61000000}},
      {OpCodeType::S_SLE,                {"s_sle",                0xff000fff, 0x62000000}},
      {OpCodeType::S_CMPSEL,             {"s_cmpsel",             0xff000fff, 0x63000000}},
      {OpCodeType::S_SRA,                {"s_sra",                0xff000fff, 0x64000000}},
      {OpCodeType::S_SRL,                {"s_srl",                0xff000fff, 0x65000000}},
      {OpCodeType::S_SLL,                {"s_sll",                0xff000fff, 0x66000000}},
      {OpCodeType::S_JUMP,               {"s_jump",               0xffff0000, 0x69000000}},
      {OpCodeType::S_JAL,                {"s_jal",                0xffff0000, 0x6a000000}},
      {OpCodeType::S_JR,                 {"s_jr",                 0xff0fffff, 0x6b000000}},
      {OpCodeType::S_BNE,                {"s_bne",                0xff000000, 0x6c000000}},
      {OpCodeType::S_BEQ,                {"s_beq",                0xff000000, 0x6d000000}},
      {OpCodeType::S_BLT,                {"s_blt",                0xff000000, 0x6e000000}},
      {OpCodeType::S_BLTU,               {"s_bltu",               0xff000000, 0x6f000000}},
      {OpCodeType::S_BGE,                {"s_bge",                0xff000000, 0x70000000}},
      {OpCodeType::S_BGEU,               {"s_bgeu",               0xff000000, 0x71000000}},
      {OpCodeType::S_SETCFG,             {"s_setcfg",             0xff00001f, 0x74000000}},
      {OpCodeType::S_SETRF,              {"s_setrf",              0xfffffff8, 0x75000000}},
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

  void setArgs(const std::vector<size_t> &args) {
    assert(setters.size() == args.size());
    for (size_t i = 0; i < args.size(); ++i)
      setters[i](args[i]);
  }
  template <typename T>  
  static std::shared_ptr<T> create(const std::vector<size_t> &args) {
    std::shared_ptr<T> x(new T);
    x->setArgs(args);
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
  uint32_t getAddressAlign() { return addressAlign; }
  uint32_t getAlu() { return alu; }
  uint32_t getBankBroadcast() { return bankBroadcast; }
  uint32_t getBankMask() { return bankMask; }
  uint32_t getBaseAddr() { return baseAddr; }
  uint32_t getChannelBroadcast() { return channelBroadcast; }
  uint32_t getClusterBroadcast() { return clusterBroadcast; }
  uint32_t getClusterMask() { return clusterMask; }
  uint32_t getCsr() { return csr; }
  uint32_t getDequantize() { return dequantize; }
  uint32_t getDtype() { return dtype; }
  uint32_t getEnable() { return enable; }
  uint32_t getFsSel() { return fsSel; }
  uint32_t getHasImm() { return hasImm; }
  uint32_t getImm() { return imm; }
  uint32_t getMode() { return mode; }
  uint32_t getOffset() { return offset; }
  uint32_t getQuantize() { return quantize; }
  uint32_t getRd() { return rd; }
  uint32_t getRfSel() { return rfSel; }
  uint32_t getRs() { return rs; }
  uint32_t getRt() { return rt; }
  uint32_t getSpRd0() { return spRd0; }
  uint32_t getSpRd1() { return spRd1; }
  uint32_t getSrcNum() { return srcNum; }
  uint32_t getStride() { return stride; }
  uint32_t getStrideLength() { return strideLength; }

  void setAddressAlign(uint32_t n) { addressAlign = n; }
  void setAlu(uint32_t n) { alu = n; }
  void setBankBroadcast(uint32_t n) { bankBroadcast = n; }
  void setBankMask(uint32_t n) { bankMask = n; }
  void setBaseAddr(uint32_t n) { baseAddr = n; }
  void setChannelBroadcast(uint32_t n) { channelBroadcast = n; }
  void setClusterBroadcast(uint32_t n) { clusterBroadcast = n; }
  void setClusterMask(uint32_t n) { clusterMask = n; }
  void setCsr(uint32_t n) { csr = n; }
  void setDequantize(uint32_t n) { dequantize = n; }
  void setDtype(uint32_t n) { dtype = n; }
  void setEnable(uint32_t n) { enable = n; }
  void setFsSel(uint32_t n) { fsSel = n; }
  void setHasImm(uint32_t n) { hasImm = n; }
  void setImm(uint32_t n) { imm = n; }
  void setMode(uint32_t n) { mode = n; }
  void setOffset(uint32_t n) { offset = n; }
  void setQuantize(uint32_t n) { quantize = n; }
  void setRd(uint32_t n) { rd = n; }
  void setRfSel(uint32_t n) { rfSel = n; }
  void setRs(uint32_t n) { rs = n; }
  void setRt(uint32_t n) { rt = n; }
  void setSpRd0(uint32_t n) { spRd0 = n; }
  void setSpRd1(uint32_t n) { spRd1 = n; }
  void setSrcNum(uint32_t n) { srcNum = n; }
  void setStride(uint32_t n) { stride = n; }
  void setStrideLength(uint32_t n) { strideLength = n; }

protected:
  uint32_t addressAlign = 0;
  uint32_t alu = 0;
  uint32_t bankBroadcast = 0;
  uint32_t bankMask = 0;
  uint32_t baseAddr = 0;
  uint32_t channelBroadcast = 0;
  uint32_t clusterBroadcast = 0;
  uint32_t clusterMask = 0;
  uint32_t csr = 0;
  uint32_t dequantize = 0;
  uint32_t dtype = 0;
  uint32_t enable = 0;
  uint32_t fsSel = 0;
  uint32_t hasImm = 0;
  uint32_t imm = 0;
  uint32_t mode = 0;
  uint32_t offset = 0;
  uint32_t quantize = 0;
  uint32_t rd = 0;
  uint32_t rfSel = 0;
  uint32_t rs = 0;
  uint32_t rt = 0;
  uint32_t spRd0 = 0;
  uint32_t spRd1 = 0;
  uint32_t srcNum = 0;
  uint32_t stride = 0;
  uint32_t strideLength = 0;
};

class DloadComputeStoreInstr {
public:
  DloadComputeStoreInstr(VpuInstr::OpCodeType load, VpuInstr::OpCodeType compute, VpuInstr::OpCodeType store) : 
      load(load), compute(compute), store(store) {}
  virtual ~DloadComputeStoreInstr() = default;
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

class DloadComputeInstr {
public:
  DloadComputeInstr(VpuInstr::OpCodeType load, VpuInstr::OpCodeType compute) : 
      load(load), compute(compute) {}
  virtual ~DloadComputeInstr() = default;
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

class DloadInstr {
public:
  virtual ~DloadInstr() = default;
};

class LoadInstr {
public:
  virtual ~LoadInstr() = default;
};

class StoreInstr {
public:
  virtual ~StoreInstr() = default;
};

class SetLoadBaseInstr : public VpuInstr {
public:
  explicit SetLoadBaseInstr() : VpuInstr(OpCodeType::SET_LOAD_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetLoad2BaseInstr : public VpuInstr {
public:
  explicit SetLoad2BaseInstr() : VpuInstr(OpCodeType::SET_LOAD2_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetStoreBaseInstr : public VpuInstr {
public:
  explicit SetStoreBaseInstr() : VpuInstr(OpCodeType::SET_STORE_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetLoadGlbInstr : public VpuInstr {
public:
  explicit SetLoadGlbInstr() : VpuInstr(OpCodeType::SET_LOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Stride);
    VPU_PUSH_GETTER_SETTER(ClusterMask);
    VPU_PUSH_GETTER_SETTER(BankMask);
    VPU_PUSH_GETTER_SETTER(ClusterBroadcast);
    VPU_PUSH_GETTER_SETTER(BankBroadcast);
    VPU_PUSH_GETTER_SETTER(ChannelBroadcast);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 13);
  VPU_GEN_GETTER_SETTER(BankMask, 12, 9);
  VPU_GEN_GETTER_SETTER(ClusterBroadcast, 8, 6);
  VPU_GEN_GETTER_SETTER(BankBroadcast, 5, 3);
  VPU_GEN_GETTER_SETTER(ChannelBroadcast, 2, 1);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetStoreGlbInstr : public VpuInstr {
public:
  explicit SetStoreGlbInstr() : VpuInstr(OpCodeType::SET_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Stride);
    VPU_PUSH_GETTER_SETTER(ClusterMask);
    VPU_PUSH_GETTER_SETTER(BankMask);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 5);
  VPU_GEN_GETTER_SETTER(BankMask, 4, 1);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetQuantizeInstr : public VpuInstr {
public:
  explicit SetQuantizeInstr() : VpuInstr(OpCodeType::SET_QUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Quantize);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Quantize, 15, 0);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetDequantizeInstr : public VpuInstr {
public:
  explicit SetDequantizeInstr() : VpuInstr(OpCodeType::SET_DEQUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 23, 0);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetDequantize2Instr : public VpuInstr {
public:
  explicit SetDequantize2Instr() : VpuInstr(OpCodeType::SET_DEQUANTIZE2) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 23, 0);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetFsInstr : public VpuInstr {
public:
  explicit SetFsInstr() : VpuInstr(OpCodeType::SET_FS) {
    VPU_PUSH_GETTER_SETTER(FsSel);
    VPU_PUSH_GETTER_SETTER(Csr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(FsSel, 7, 0);
  VPU_GEN_GETTER_SETTER(Csr, 29, 29);
};

class SetAaiInstr : public VpuInstr {
public:
  explicit SetAaiInstr() : VpuInstr(OpCodeType::SET_AAI) {
    VPU_PUSH_GETTER_SETTER(Enable);
    VPU_PUSH_GETTER_SETTER(Mode);
    VPU_PUSH_GETTER_SETTER(StrideLength);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Enable, 23, 23);
  VPU_GEN_GETTER_SETTER(Mode, 22, 19);
  VPU_GEN_GETTER_SETTER(StrideLength, 15, 0);
};

class VpuNopInstr : public VpuInstr {
public:
  explicit VpuNopInstr() : VpuInstr(OpCodeType::VPU_NOP) {
    alu = 1;
  }
};

class VpuEndInstr : public VpuInstr {
public:
  explicit VpuEndInstr() : VpuInstr(OpCodeType::VPU_END) {
    VPU_PUSH_GETTER_SETTER(AddressAlign);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(AddressAlign, 0, 0);
};

class I8DloadInstr : public VpuInstr, public DloadInstr {
public:
  explicit I8DloadInstr() : VpuInstr(OpCodeType::I8_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class I8LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit I8LoadInstr() : VpuInstr(OpCodeType::I8_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8DloadInstr : public VpuInstr, public DloadInstr {
public:
  explicit U8DloadInstr() : VpuInstr(OpCodeType::U8_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class U8LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit U8LoadInstr() : VpuInstr(OpCodeType::U8_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16DloadInstr : public VpuInstr, public DloadInstr {
public:
  explicit F16DloadInstr() : VpuInstr(OpCodeType::F16_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class F16LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit F16LoadInstr() : VpuInstr(OpCodeType::F16_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32DloadInstr : public VpuInstr, public DloadInstr {
public:
  explicit F32DloadInstr() : VpuInstr(OpCodeType::F32_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class F32LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit F32LoadInstr() : VpuInstr(OpCodeType::F32_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32DloadInstr : public VpuInstr, public DloadInstr {
public:
  explicit I32DloadInstr() : VpuInstr(OpCodeType::I32_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class I32LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit I32LoadInstr() : VpuInstr(OpCodeType::I32_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit I8StoreInstr() : VpuInstr(OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit U8StoreInstr() : VpuInstr(OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit F16StoreInstr() : VpuInstr(OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit F32StoreInstr() : VpuInstr(OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit I32StoreInstr() : VpuInstr(OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32AddInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32AddInstr() : VpuInstr(OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32SubInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32SubInstr() : VpuInstr(OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32MulInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32MulInstr() : VpuInstr(OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32MaxInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32MaxInstr() : VpuInstr(OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32MinInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32MinInstr() : VpuInstr(OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32GreaterInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32GreaterInstr() : VpuInstr(OpCodeType::F32_GREATER) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32GreaterEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32GreaterEqualInstr() : VpuInstr(OpCodeType::F32_GREATER_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32LessInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LessInstr() : VpuInstr(OpCodeType::F32_LESS) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32LessEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LessEqualInstr() : VpuInstr(OpCodeType::F32_LESS_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32EqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32EqualInstr() : VpuInstr(OpCodeType::F32_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32SelInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32SelInstr() : VpuInstr(OpCodeType::F32_SEL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32AndInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32AndInstr() : VpuInstr(OpCodeType::I32_AND) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32OrInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32OrInstr() : VpuInstr(OpCodeType::I32_OR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32NotInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32NotInstr() : VpuInstr(OpCodeType::I32_NOT) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32XorInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32XorInstr() : VpuInstr(OpCodeType::I32_XOR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32XnorInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32XnorInstr() : VpuInstr(OpCodeType::I32_XNOR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class Bf16AddiInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16AddiInstr() : VpuInstr(OpCodeType::BF16_ADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class Bf16SubiInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16SubiInstr() : VpuInstr(OpCodeType::BF16_SUBI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class Bf16MuliInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16MuliInstr() : VpuInstr(OpCodeType::BF16_MULI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class Bf16MaxiInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16MaxiInstr() : VpuInstr(OpCodeType::BF16_MAXI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class Bf16MiniInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16MiniInstr() : VpuInstr(OpCodeType::BF16_MINI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class Bf16MoviInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16MoviInstr() : VpuInstr(OpCodeType::BF16_MOVI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class Bf16GreateriInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16GreateriInstr() : VpuInstr(OpCodeType::BF16_GREATERI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class Bf16GreaterEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16GreaterEqualiInstr() : VpuInstr(OpCodeType::BF16_GREATER_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class Bf16LessiInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16LessiInstr() : VpuInstr(OpCodeType::BF16_LESSI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class Bf16LessEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16LessEqualiInstr() : VpuInstr(OpCodeType::BF16_LESS_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class Bf16EqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16EqualiInstr() : VpuInstr(OpCodeType::BF16_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class Bf16SeliInstr : public VpuInstr, public VectorInstr {
public:
  explicit Bf16SeliInstr() : VpuInstr(OpCodeType::BF16_SELI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16AddiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16AddiInstr() : VpuInstr(OpCodeType::F16_ADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16SubiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16SubiInstr() : VpuInstr(OpCodeType::F16_SUBI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16MuliInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16MuliInstr() : VpuInstr(OpCodeType::F16_MULI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16MaxiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16MaxiInstr() : VpuInstr(OpCodeType::F16_MAXI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16MiniInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16MiniInstr() : VpuInstr(OpCodeType::F16_MINI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16MoviInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16MoviInstr() : VpuInstr(OpCodeType::F16_MOVI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16GreateriInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16GreateriInstr() : VpuInstr(OpCodeType::F16_GREATERI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F16GreaterEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16GreaterEqualiInstr() : VpuInstr(OpCodeType::F16_GREATER_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F16LessiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16LessiInstr() : VpuInstr(OpCodeType::F16_LESSI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F16LessEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16LessEqualiInstr() : VpuInstr(OpCodeType::F16_LESS_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F16EqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16EqualiInstr() : VpuInstr(OpCodeType::F16_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F16SeliInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16SeliInstr() : VpuInstr(OpCodeType::F16_SELI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 19;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I16AddiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16AddiInstr() : VpuInstr(OpCodeType::I16_ADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I16SubiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16SubiInstr() : VpuInstr(OpCodeType::I16_SUBI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I16MuliInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16MuliInstr() : VpuInstr(OpCodeType::I16_MULI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I16MaxiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16MaxiInstr() : VpuInstr(OpCodeType::I16_MAXI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I16MiniInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16MiniInstr() : VpuInstr(OpCodeType::I16_MINI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I16MoviInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16MoviInstr() : VpuInstr(OpCodeType::I16_MOVI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I16GreateriInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16GreateriInstr() : VpuInstr(OpCodeType::I16_GREATERI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I16GreaterEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16GreaterEqualiInstr() : VpuInstr(OpCodeType::I16_GREATER_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I16LessiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16LessiInstr() : VpuInstr(OpCodeType::I16_LESSI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I16LessEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16LessEqualiInstr() : VpuInstr(OpCodeType::I16_LESS_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I16EqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16EqualiInstr() : VpuInstr(OpCodeType::I16_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I16SeliInstr : public VpuInstr, public VectorInstr {
public:
  explicit I16SeliInstr() : VpuInstr(OpCodeType::I16_SELI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 2;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32LuiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LuiInstr() : VpuInstr(OpCodeType::F32_LUI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32LiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LiInstr() : VpuInstr(OpCodeType::F32_LI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32AndiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32AndiInstr() : VpuInstr(OpCodeType::I32_ANDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32OriInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32OriInstr() : VpuInstr(OpCodeType::I32_ORI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32NotiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32NotiInstr() : VpuInstr(OpCodeType::I32_NOTI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32XoriInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32XoriInstr() : VpuInstr(OpCodeType::I32_XORI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32XnoriInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32XnoriInstr() : VpuInstr(OpCodeType::I32_XNORI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32ShiftlInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32ShiftlInstr() : VpuInstr(OpCodeType::I32_SHIFTL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32ShiftrInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32ShiftrInstr() : VpuInstr(OpCodeType::I32_SHIFTR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32ShiftliInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32ShiftliInstr() : VpuInstr(OpCodeType::I32_SHIFTLI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 7, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32ShiftriInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32ShiftriInstr() : VpuInstr(OpCodeType::I32_SHIFTRI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 7, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F64AddInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64AddInstr() : VpuInstr(OpCodeType::F64_ADD) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F64SubInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64SubInstr() : VpuInstr(OpCodeType::F64_SUB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F64MulInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64MulInstr() : VpuInstr(OpCodeType::F64_MUL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F64MaxInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64MaxInstr() : VpuInstr(OpCodeType::F64_MAX) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F64MinInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64MinInstr() : VpuInstr(OpCodeType::F64_MIN) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F64GreaterInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64GreaterInstr() : VpuInstr(OpCodeType::F64_GREATER) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F64GreaterEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64GreaterEqualInstr() : VpuInstr(OpCodeType::F64_GREATER_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F64LessInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64LessInstr() : VpuInstr(OpCodeType::F64_LESS) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F64LessEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64LessEqualInstr() : VpuInstr(OpCodeType::F64_LESS_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F64EqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64EqualInstr() : VpuInstr(OpCodeType::F64_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F64SelInstr : public VpuInstr, public VectorInstr {
public:
  explicit F64SelInstr() : VpuInstr(OpCodeType::F64_SEL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 4;
    alu = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I64ShiftlInstr : public VpuInstr, public VectorInstr {
public:
  explicit I64ShiftlInstr() : VpuInstr(OpCodeType::I64_SHIFTL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 4;
    alu = 4;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I64ShiftrInstr : public VpuInstr, public VectorInstr {
public:
  explicit I64ShiftrInstr() : VpuInstr(OpCodeType::I64_SHIFTR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 4;
    alu = 4;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I64ShiftliInstr : public VpuInstr, public VectorInstr {
public:
  explicit I64ShiftliInstr() : VpuInstr(OpCodeType::I64_SHIFTLI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 4;
    alu = 4;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 7, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I64ShiftriInstr : public VpuInstr, public VectorInstr {
public:
  explicit I64ShiftriInstr() : VpuInstr(OpCodeType::I64_SHIFTRI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
    dtype = 4;
    alu = 4;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 7, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I8DloadAddInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadAddInstr() : VpuInstr(OpCodeType::I8_DLOAD_ADD), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadSubInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadSubInstr() : VpuInstr(OpCodeType::I8_DLOAD_SUB), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMulInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadMulInstr() : VpuInstr(OpCodeType::I8_DLOAD_MUL), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMaxInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadMaxInstr() : VpuInstr(OpCodeType::I8_DLOAD_MAX), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMinInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadMinInstr() : VpuInstr(OpCodeType::I8_DLOAD_MIN), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadAddInstr() : VpuInstr(OpCodeType::I8_LOAD_ADD), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadSubInstr() : VpuInstr(OpCodeType::I8_LOAD_SUB), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadMulInstr() : VpuInstr(OpCodeType::I8_LOAD_MUL), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadMaxInstr() : VpuInstr(OpCodeType::I8_LOAD_MAX), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadMinInstr() : VpuInstr(OpCodeType::I8_LOAD_MIN), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadAddInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadAddInstr() : VpuInstr(OpCodeType::U8_DLOAD_ADD), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadSubInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadSubInstr() : VpuInstr(OpCodeType::U8_DLOAD_SUB), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMulInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadMulInstr() : VpuInstr(OpCodeType::U8_DLOAD_MUL), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMaxInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadMaxInstr() : VpuInstr(OpCodeType::U8_DLOAD_MAX), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMinInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadMinInstr() : VpuInstr(OpCodeType::U8_DLOAD_MIN), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadAddInstr() : VpuInstr(OpCodeType::U8_LOAD_ADD), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadSubInstr() : VpuInstr(OpCodeType::U8_LOAD_SUB), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadMulInstr() : VpuInstr(OpCodeType::U8_LOAD_MUL), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadMaxInstr() : VpuInstr(OpCodeType::U8_LOAD_MAX), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadMinInstr() : VpuInstr(OpCodeType::U8_LOAD_MIN), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadAddInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadAddInstr() : VpuInstr(OpCodeType::F16_DLOAD_ADD), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadSubInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadSubInstr() : VpuInstr(OpCodeType::F16_DLOAD_SUB), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMulInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadMulInstr() : VpuInstr(OpCodeType::F16_DLOAD_MUL), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMaxInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadMaxInstr() : VpuInstr(OpCodeType::F16_DLOAD_MAX), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMinInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadMinInstr() : VpuInstr(OpCodeType::F16_DLOAD_MIN), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadAddInstr() : VpuInstr(OpCodeType::F16_LOAD_ADD), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadSubInstr() : VpuInstr(OpCodeType::F16_LOAD_SUB), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadMulInstr() : VpuInstr(OpCodeType::F16_LOAD_MUL), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadMaxInstr() : VpuInstr(OpCodeType::F16_LOAD_MAX), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadMinInstr() : VpuInstr(OpCodeType::F16_LOAD_MIN), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadAddInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F32DloadAddInstr() : VpuInstr(OpCodeType::F32_DLOAD_ADD), DloadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadSubInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F32DloadSubInstr() : VpuInstr(OpCodeType::F32_DLOAD_SUB), DloadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadMulInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F32DloadMulInstr() : VpuInstr(OpCodeType::F32_DLOAD_MUL), DloadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadMaxInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F32DloadMaxInstr() : VpuInstr(OpCodeType::F32_DLOAD_MAX), DloadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadMinInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F32DloadMinInstr() : VpuInstr(OpCodeType::F32_DLOAD_MIN), DloadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F32LoadAddInstr() : VpuInstr(OpCodeType::F32_LOAD_ADD), LoadComputeInstr(OpCodeType::F32_LOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F32LoadSubInstr() : VpuInstr(OpCodeType::F32_LOAD_SUB), LoadComputeInstr(OpCodeType::F32_LOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F32LoadMulInstr() : VpuInstr(OpCodeType::F32_LOAD_MUL), LoadComputeInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F32LoadMaxInstr() : VpuInstr(OpCodeType::F32_LOAD_MAX), LoadComputeInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F32LoadMinInstr() : VpuInstr(OpCodeType::F32_LOAD_MIN), LoadComputeInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadAddInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadAddInstr() : VpuInstr(OpCodeType::I32_DLOAD_ADD), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadSubInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadSubInstr() : VpuInstr(OpCodeType::I32_DLOAD_SUB), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadMulInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadMulInstr() : VpuInstr(OpCodeType::I32_DLOAD_MUL), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadMaxInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadMaxInstr() : VpuInstr(OpCodeType::I32_DLOAD_MAX), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadMinInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadMinInstr() : VpuInstr(OpCodeType::I32_DLOAD_MIN), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadAndInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadAndInstr() : VpuInstr(OpCodeType::I32_DLOAD_AND), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_AND) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadOrInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadOrInstr() : VpuInstr(OpCodeType::I32_DLOAD_OR), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_OR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadNotInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadNotInstr() : VpuInstr(OpCodeType::I32_DLOAD_NOT), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_NOT) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadXorInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadXorInstr() : VpuInstr(OpCodeType::I32_DLOAD_XOR), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_XOR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadXnorInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadXnorInstr() : VpuInstr(OpCodeType::I32_DLOAD_XNOR), DloadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_XNOR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadAddInstr() : VpuInstr(OpCodeType::I32_LOAD_ADD), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadSubInstr() : VpuInstr(OpCodeType::I32_LOAD_SUB), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadMulInstr() : VpuInstr(OpCodeType::I32_LOAD_MUL), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadMaxInstr() : VpuInstr(OpCodeType::I32_LOAD_MAX), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadMinInstr() : VpuInstr(OpCodeType::I32_LOAD_MIN), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadAndInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadAndInstr() : VpuInstr(OpCodeType::I32_LOAD_AND), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_AND) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadOrInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadOrInstr() : VpuInstr(OpCodeType::I32_LOAD_OR), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_OR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadNotInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadNotInstr() : VpuInstr(OpCodeType::I32_LOAD_NOT), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_NOT) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadXorInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadXorInstr() : VpuInstr(OpCodeType::I32_LOAD_XOR), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_XOR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadXnorInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadXnorInstr() : VpuInstr(OpCodeType::I32_LOAD_XNOR), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_XNOR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8AddStoreInstr() : VpuInstr(OpCodeType::I8_ADD_STORE), ComputeStoreInstr(OpCodeType::F32_ADD, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8SubStoreInstr() : VpuInstr(OpCodeType::I8_SUB_STORE), ComputeStoreInstr(OpCodeType::F32_SUB, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8MulStoreInstr() : VpuInstr(OpCodeType::I8_MUL_STORE), ComputeStoreInstr(OpCodeType::F32_MUL, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8MaxStoreInstr() : VpuInstr(OpCodeType::I8_MAX_STORE), ComputeStoreInstr(OpCodeType::F32_MAX, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8MinStoreInstr() : VpuInstr(OpCodeType::I8_MIN_STORE), ComputeStoreInstr(OpCodeType::F32_MIN, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8AddStoreInstr() : VpuInstr(OpCodeType::U8_ADD_STORE), ComputeStoreInstr(OpCodeType::F32_ADD, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8SubStoreInstr() : VpuInstr(OpCodeType::U8_SUB_STORE), ComputeStoreInstr(OpCodeType::F32_SUB, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8MulStoreInstr() : VpuInstr(OpCodeType::U8_MUL_STORE), ComputeStoreInstr(OpCodeType::F32_MUL, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8MaxStoreInstr() : VpuInstr(OpCodeType::U8_MAX_STORE), ComputeStoreInstr(OpCodeType::F32_MAX, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8MinStoreInstr() : VpuInstr(OpCodeType::U8_MIN_STORE), ComputeStoreInstr(OpCodeType::F32_MIN, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16AddStoreInstr() : VpuInstr(OpCodeType::F16_ADD_STORE), ComputeStoreInstr(OpCodeType::F32_ADD, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16SubStoreInstr() : VpuInstr(OpCodeType::F16_SUB_STORE), ComputeStoreInstr(OpCodeType::F32_SUB, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16MulStoreInstr() : VpuInstr(OpCodeType::F16_MUL_STORE), ComputeStoreInstr(OpCodeType::F32_MUL, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16MaxStoreInstr() : VpuInstr(OpCodeType::F16_MAX_STORE), ComputeStoreInstr(OpCodeType::F32_MAX, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16MinStoreInstr() : VpuInstr(OpCodeType::F16_MIN_STORE), ComputeStoreInstr(OpCodeType::F32_MIN, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F32AddStoreInstr() : VpuInstr(OpCodeType::F32_ADD_STORE), ComputeStoreInstr(OpCodeType::F32_ADD, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F32SubStoreInstr() : VpuInstr(OpCodeType::F32_SUB_STORE), ComputeStoreInstr(OpCodeType::F32_SUB, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F32MulStoreInstr() : VpuInstr(OpCodeType::F32_MUL_STORE), ComputeStoreInstr(OpCodeType::F32_MUL, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F32MaxStoreInstr() : VpuInstr(OpCodeType::F32_MAX_STORE), ComputeStoreInstr(OpCodeType::F32_MAX, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F32MinStoreInstr() : VpuInstr(OpCodeType::F32_MIN_STORE), ComputeStoreInstr(OpCodeType::F32_MIN, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32AddStoreInstr() : VpuInstr(OpCodeType::I32_ADD_STORE), ComputeStoreInstr(OpCodeType::F32_ADD, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32SubStoreInstr() : VpuInstr(OpCodeType::I32_SUB_STORE), ComputeStoreInstr(OpCodeType::F32_SUB, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32MulStoreInstr() : VpuInstr(OpCodeType::I32_MUL_STORE), ComputeStoreInstr(OpCodeType::F32_MUL, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32MaxStoreInstr() : VpuInstr(OpCodeType::I32_MAX_STORE), ComputeStoreInstr(OpCodeType::F32_MAX, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32MinStoreInstr() : VpuInstr(OpCodeType::I32_MIN_STORE), ComputeStoreInstr(OpCodeType::F32_MIN, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32AndStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32AndStoreInstr() : VpuInstr(OpCodeType::I32_AND_STORE), ComputeStoreInstr(OpCodeType::I32_AND, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32OrStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32OrStoreInstr() : VpuInstr(OpCodeType::I32_OR_STORE), ComputeStoreInstr(OpCodeType::I32_OR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32NotStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32NotStoreInstr() : VpuInstr(OpCodeType::I32_NOT_STORE), ComputeStoreInstr(OpCodeType::I32_NOT, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32XorStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32XorStoreInstr() : VpuInstr(OpCodeType::I32_XOR_STORE), ComputeStoreInstr(OpCodeType::I32_XOR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32XnorStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32XnorStoreInstr() : VpuInstr(OpCodeType::I32_XNOR_STORE), ComputeStoreInstr(OpCodeType::I32_XNOR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadAddStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadAddStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_ADD_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_ADD, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadSubStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadSubStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_SUB_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_SUB, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMulStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadMulStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MUL_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MUL, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMaxStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadMaxStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MAX_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MAX, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMinStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadMinStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MIN_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MIN, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadAddStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_ADD, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadSubStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_SUB, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMulStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MUL, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMaxStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MAX, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMinStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MIN, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8DloadAddStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadAddStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_ADD_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_ADD, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadSubStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadSubStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_SUB_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_SUB, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMulStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadMulStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MUL_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MUL, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMaxStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadMaxStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MAX_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MAX, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMinStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadMinStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MIN_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MIN, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadAddStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_ADD, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadSubStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_SUB, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMulStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MUL, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMaxStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MAX, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMinStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MIN, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16DloadAddStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadAddStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_ADD_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_ADD, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadSubStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadSubStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_SUB_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_SUB, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMulStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadMulStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MUL_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MUL, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMaxStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadMaxStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MAX_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MAX, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMinStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadMinStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MIN_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MIN, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadAddStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_ADD, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadSubStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_SUB, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMulStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MUL, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMaxStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MAX, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMinStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MIN, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 1;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32DloadAddStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F32DloadAddStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_ADD_STORE), DloadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_ADD, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32DloadSubStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F32DloadSubStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_SUB_STORE), DloadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_SUB, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32DloadMulStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F32DloadMulStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_MUL_STORE), DloadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MUL, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32DloadMaxStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F32DloadMaxStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_MAX_STORE), DloadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MAX, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32DloadMinStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F32DloadMinStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_MIN_STORE), DloadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MIN, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadAddStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_ADD, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadSubStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_SUB, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadMulStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MUL, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadMaxStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MAX, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadMinStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MIN, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32DloadAddStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadAddStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_ADD_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_ADD, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadSubStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadSubStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_SUB_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_SUB, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadMulStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadMulStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_MUL_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_MUL, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadMaxStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadMaxStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_MAX_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_MAX, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadMinStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadMinStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_MIN_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::F32_MIN, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadAndStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadAndStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_AND_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_AND, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadOrStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadOrStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_OR_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_OR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadNotStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadNotStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_NOT_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_NOT, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadXorStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadXorStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_XOR_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_XOR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadXnorStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadXnorStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_XNOR_STORE), DloadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_XNOR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadAddStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::F32_ADD, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadSubStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::F32_SUB, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadMulStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::F32_MUL, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadMaxStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::F32_MAX, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadMinStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::F32_MIN, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadAndStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadAndStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_AND_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_AND, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadOrStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadOrStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_OR_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_OR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadNotStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadNotStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_NOT_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_NOT, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadXorStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadXorStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_XOR_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_XOR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadXnorStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadXnorStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_XNOR_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_XNOR, OpCodeType::I32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 2;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
    hasImm = 1;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
  VPU_GEN_GETTER_SETTER(Rt, 19, 16);
  VPU_GEN_GETTER_SETTER(Rd, 15, 12);
};

class SJumpInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SJumpInstr() : VpuInstr(OpCodeType::S_JUMP) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 3;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SJalInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SJalInstr() : VpuInstr(OpCodeType::S_JAL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 3;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SJrInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SJrInstr() : VpuInstr(OpCodeType::S_JR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    dtype = 3;
  }
  VPU_GEN_GETTER_SETTER(Rs, 23, 20);
};

class SBneInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SBneInstr() : VpuInstr(OpCodeType::S_BNE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
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
    dtype = 3;
  }
  VPU_GEN_GETTER_SETTER(RfSel, 2, 0);
};


std::shared_ptr<VpuInstr> VpuInstr::create(OpCodeType opCode, const std::vector<size_t> &args) {
  std::shared_ptr<VpuInstr> ret = nullptr;
  switch (opCode) {
    case OpCodeType::SET_LOAD_BASE:        ret = std::shared_ptr<SetLoadBaseInstr>(new SetLoadBaseInstr); break;
    case OpCodeType::SET_LOAD2_BASE:       ret = std::shared_ptr<SetLoad2BaseInstr>(new SetLoad2BaseInstr); break;
    case OpCodeType::SET_STORE_BASE:       ret = std::shared_ptr<SetStoreBaseInstr>(new SetStoreBaseInstr); break;
    case OpCodeType::SET_LOAD_GLB:         ret = std::shared_ptr<SetLoadGlbInstr>(new SetLoadGlbInstr); break;
    case OpCodeType::SET_STORE_GLB:        ret = std::shared_ptr<SetStoreGlbInstr>(new SetStoreGlbInstr); break;
    case OpCodeType::SET_QUANTIZE:         ret = std::shared_ptr<SetQuantizeInstr>(new SetQuantizeInstr); break;
    case OpCodeType::SET_DEQUANTIZE:       ret = std::shared_ptr<SetDequantizeInstr>(new SetDequantizeInstr); break;
    case OpCodeType::SET_DEQUANTIZE2:      ret = std::shared_ptr<SetDequantize2Instr>(new SetDequantize2Instr); break;
    case OpCodeType::SET_FS:               ret = std::shared_ptr<SetFsInstr>(new SetFsInstr); break;
    case OpCodeType::SET_AAI:              ret = std::shared_ptr<SetAaiInstr>(new SetAaiInstr); break;
    case OpCodeType::VPU_NOP:              ret = std::shared_ptr<VpuNopInstr>(new VpuNopInstr); break;
    case OpCodeType::VPU_END:              ret = std::shared_ptr<VpuEndInstr>(new VpuEndInstr); break;
    case OpCodeType::I8_DLOAD:             ret = std::shared_ptr<I8DloadInstr>(new I8DloadInstr); break;
    case OpCodeType::I8_LOAD:              ret = std::shared_ptr<I8LoadInstr>(new I8LoadInstr); break;
    case OpCodeType::U8_DLOAD:             ret = std::shared_ptr<U8DloadInstr>(new U8DloadInstr); break;
    case OpCodeType::U8_LOAD:              ret = std::shared_ptr<U8LoadInstr>(new U8LoadInstr); break;
    case OpCodeType::F16_DLOAD:            ret = std::shared_ptr<F16DloadInstr>(new F16DloadInstr); break;
    case OpCodeType::F16_LOAD:             ret = std::shared_ptr<F16LoadInstr>(new F16LoadInstr); break;
    case OpCodeType::F32_DLOAD:            ret = std::shared_ptr<F32DloadInstr>(new F32DloadInstr); break;
    case OpCodeType::F32_LOAD:             ret = std::shared_ptr<F32LoadInstr>(new F32LoadInstr); break;
    case OpCodeType::I32_DLOAD:            ret = std::shared_ptr<I32DloadInstr>(new I32DloadInstr); break;
    case OpCodeType::I32_LOAD:             ret = std::shared_ptr<I32LoadInstr>(new I32LoadInstr); break;
    case OpCodeType::I8_STORE:             ret = std::shared_ptr<I8StoreInstr>(new I8StoreInstr); break;
    case OpCodeType::U8_STORE:             ret = std::shared_ptr<U8StoreInstr>(new U8StoreInstr); break;
    case OpCodeType::F16_STORE:            ret = std::shared_ptr<F16StoreInstr>(new F16StoreInstr); break;
    case OpCodeType::F32_STORE:            ret = std::shared_ptr<F32StoreInstr>(new F32StoreInstr); break;
    case OpCodeType::I32_STORE:            ret = std::shared_ptr<I32StoreInstr>(new I32StoreInstr); break;
    case OpCodeType::F32_ADD:              ret = std::shared_ptr<F32AddInstr>(new F32AddInstr); break;
    case OpCodeType::F32_SUB:              ret = std::shared_ptr<F32SubInstr>(new F32SubInstr); break;
    case OpCodeType::F32_MUL:              ret = std::shared_ptr<F32MulInstr>(new F32MulInstr); break;
    case OpCodeType::F32_MAX:              ret = std::shared_ptr<F32MaxInstr>(new F32MaxInstr); break;
    case OpCodeType::F32_MIN:              ret = std::shared_ptr<F32MinInstr>(new F32MinInstr); break;
    case OpCodeType::F32_GREATER:          ret = std::shared_ptr<F32GreaterInstr>(new F32GreaterInstr); break;
    case OpCodeType::F32_GREATER_EQUAL:    ret = std::shared_ptr<F32GreaterEqualInstr>(new F32GreaterEqualInstr); break;
    case OpCodeType::F32_LESS:             ret = std::shared_ptr<F32LessInstr>(new F32LessInstr); break;
    case OpCodeType::F32_LESS_EQUAL:       ret = std::shared_ptr<F32LessEqualInstr>(new F32LessEqualInstr); break;
    case OpCodeType::F32_EQUAL:            ret = std::shared_ptr<F32EqualInstr>(new F32EqualInstr); break;
    case OpCodeType::F32_SEL:              ret = std::shared_ptr<F32SelInstr>(new F32SelInstr); break;
    case OpCodeType::I32_AND:              ret = std::shared_ptr<I32AndInstr>(new I32AndInstr); break;
    case OpCodeType::I32_OR:               ret = std::shared_ptr<I32OrInstr>(new I32OrInstr); break;
    case OpCodeType::I32_NOT:              ret = std::shared_ptr<I32NotInstr>(new I32NotInstr); break;
    case OpCodeType::I32_XOR:              ret = std::shared_ptr<I32XorInstr>(new I32XorInstr); break;
    case OpCodeType::I32_XNOR:             ret = std::shared_ptr<I32XnorInstr>(new I32XnorInstr); break;
    case OpCodeType::BF16_ADDI:            ret = std::shared_ptr<Bf16AddiInstr>(new Bf16AddiInstr); break;
    case OpCodeType::BF16_SUBI:            ret = std::shared_ptr<Bf16SubiInstr>(new Bf16SubiInstr); break;
    case OpCodeType::BF16_MULI:            ret = std::shared_ptr<Bf16MuliInstr>(new Bf16MuliInstr); break;
    case OpCodeType::BF16_MAXI:            ret = std::shared_ptr<Bf16MaxiInstr>(new Bf16MaxiInstr); break;
    case OpCodeType::BF16_MINI:            ret = std::shared_ptr<Bf16MiniInstr>(new Bf16MiniInstr); break;
    case OpCodeType::BF16_MOVI:            ret = std::shared_ptr<Bf16MoviInstr>(new Bf16MoviInstr); break;
    case OpCodeType::BF16_GREATERI:        ret = std::shared_ptr<Bf16GreateriInstr>(new Bf16GreateriInstr); break;
    case OpCodeType::BF16_GREATER_EQUALI:  ret = std::shared_ptr<Bf16GreaterEqualiInstr>(new Bf16GreaterEqualiInstr); break;
    case OpCodeType::BF16_LESSI:           ret = std::shared_ptr<Bf16LessiInstr>(new Bf16LessiInstr); break;
    case OpCodeType::BF16_LESS_EQUALI:     ret = std::shared_ptr<Bf16LessEqualiInstr>(new Bf16LessEqualiInstr); break;
    case OpCodeType::BF16_EQUALI:          ret = std::shared_ptr<Bf16EqualiInstr>(new Bf16EqualiInstr); break;
    case OpCodeType::BF16_SELI:            ret = std::shared_ptr<Bf16SeliInstr>(new Bf16SeliInstr); break;
    case OpCodeType::F16_ADDI:             ret = std::shared_ptr<F16AddiInstr>(new F16AddiInstr); break;
    case OpCodeType::F16_SUBI:             ret = std::shared_ptr<F16SubiInstr>(new F16SubiInstr); break;
    case OpCodeType::F16_MULI:             ret = std::shared_ptr<F16MuliInstr>(new F16MuliInstr); break;
    case OpCodeType::F16_MAXI:             ret = std::shared_ptr<F16MaxiInstr>(new F16MaxiInstr); break;
    case OpCodeType::F16_MINI:             ret = std::shared_ptr<F16MiniInstr>(new F16MiniInstr); break;
    case OpCodeType::F16_MOVI:             ret = std::shared_ptr<F16MoviInstr>(new F16MoviInstr); break;
    case OpCodeType::F16_GREATERI:         ret = std::shared_ptr<F16GreateriInstr>(new F16GreateriInstr); break;
    case OpCodeType::F16_GREATER_EQUALI:   ret = std::shared_ptr<F16GreaterEqualiInstr>(new F16GreaterEqualiInstr); break;
    case OpCodeType::F16_LESSI:            ret = std::shared_ptr<F16LessiInstr>(new F16LessiInstr); break;
    case OpCodeType::F16_LESS_EQUALI:      ret = std::shared_ptr<F16LessEqualiInstr>(new F16LessEqualiInstr); break;
    case OpCodeType::F16_EQUALI:           ret = std::shared_ptr<F16EqualiInstr>(new F16EqualiInstr); break;
    case OpCodeType::F16_SELI:             ret = std::shared_ptr<F16SeliInstr>(new F16SeliInstr); break;
    case OpCodeType::I16_ADDI:             ret = std::shared_ptr<I16AddiInstr>(new I16AddiInstr); break;
    case OpCodeType::I16_SUBI:             ret = std::shared_ptr<I16SubiInstr>(new I16SubiInstr); break;
    case OpCodeType::I16_MULI:             ret = std::shared_ptr<I16MuliInstr>(new I16MuliInstr); break;
    case OpCodeType::I16_MAXI:             ret = std::shared_ptr<I16MaxiInstr>(new I16MaxiInstr); break;
    case OpCodeType::I16_MINI:             ret = std::shared_ptr<I16MiniInstr>(new I16MiniInstr); break;
    case OpCodeType::I16_MOVI:             ret = std::shared_ptr<I16MoviInstr>(new I16MoviInstr); break;
    case OpCodeType::I16_GREATERI:         ret = std::shared_ptr<I16GreateriInstr>(new I16GreateriInstr); break;
    case OpCodeType::I16_GREATER_EQUALI:   ret = std::shared_ptr<I16GreaterEqualiInstr>(new I16GreaterEqualiInstr); break;
    case OpCodeType::I16_LESSI:            ret = std::shared_ptr<I16LessiInstr>(new I16LessiInstr); break;
    case OpCodeType::I16_LESS_EQUALI:      ret = std::shared_ptr<I16LessEqualiInstr>(new I16LessEqualiInstr); break;
    case OpCodeType::I16_EQUALI:           ret = std::shared_ptr<I16EqualiInstr>(new I16EqualiInstr); break;
    case OpCodeType::I16_SELI:             ret = std::shared_ptr<I16SeliInstr>(new I16SeliInstr); break;
    case OpCodeType::F32_LUI:              ret = std::shared_ptr<F32LuiInstr>(new F32LuiInstr); break;
    case OpCodeType::F32_LI:               ret = std::shared_ptr<F32LiInstr>(new F32LiInstr); break;
    case OpCodeType::I32_ANDI:             ret = std::shared_ptr<I32AndiInstr>(new I32AndiInstr); break;
    case OpCodeType::I32_ORI:              ret = std::shared_ptr<I32OriInstr>(new I32OriInstr); break;
    case OpCodeType::I32_NOTI:             ret = std::shared_ptr<I32NotiInstr>(new I32NotiInstr); break;
    case OpCodeType::I32_XORI:             ret = std::shared_ptr<I32XoriInstr>(new I32XoriInstr); break;
    case OpCodeType::I32_XNORI:            ret = std::shared_ptr<I32XnoriInstr>(new I32XnoriInstr); break;
    case OpCodeType::I32_SHIFTL:           ret = std::shared_ptr<I32ShiftlInstr>(new I32ShiftlInstr); break;
    case OpCodeType::I32_SHIFTR:           ret = std::shared_ptr<I32ShiftrInstr>(new I32ShiftrInstr); break;
    case OpCodeType::I32_SHIFTLI:          ret = std::shared_ptr<I32ShiftliInstr>(new I32ShiftliInstr); break;
    case OpCodeType::I32_SHIFTRI:          ret = std::shared_ptr<I32ShiftriInstr>(new I32ShiftriInstr); break;
    case OpCodeType::F64_ADD:              ret = std::shared_ptr<F64AddInstr>(new F64AddInstr); break;
    case OpCodeType::F64_SUB:              ret = std::shared_ptr<F64SubInstr>(new F64SubInstr); break;
    case OpCodeType::F64_MUL:              ret = std::shared_ptr<F64MulInstr>(new F64MulInstr); break;
    case OpCodeType::F64_MAX:              ret = std::shared_ptr<F64MaxInstr>(new F64MaxInstr); break;
    case OpCodeType::F64_MIN:              ret = std::shared_ptr<F64MinInstr>(new F64MinInstr); break;
    case OpCodeType::F64_GREATER:          ret = std::shared_ptr<F64GreaterInstr>(new F64GreaterInstr); break;
    case OpCodeType::F64_GREATER_EQUAL:    ret = std::shared_ptr<F64GreaterEqualInstr>(new F64GreaterEqualInstr); break;
    case OpCodeType::F64_LESS:             ret = std::shared_ptr<F64LessInstr>(new F64LessInstr); break;
    case OpCodeType::F64_LESS_EQUAL:       ret = std::shared_ptr<F64LessEqualInstr>(new F64LessEqualInstr); break;
    case OpCodeType::F64_EQUAL:            ret = std::shared_ptr<F64EqualInstr>(new F64EqualInstr); break;
    case OpCodeType::F64_SEL:              ret = std::shared_ptr<F64SelInstr>(new F64SelInstr); break;
    case OpCodeType::I64_SHIFTL:           ret = std::shared_ptr<I64ShiftlInstr>(new I64ShiftlInstr); break;
    case OpCodeType::I64_SHIFTR:           ret = std::shared_ptr<I64ShiftrInstr>(new I64ShiftrInstr); break;
    case OpCodeType::I64_SHIFTLI:          ret = std::shared_ptr<I64ShiftliInstr>(new I64ShiftliInstr); break;
    case OpCodeType::I64_SHIFTRI:          ret = std::shared_ptr<I64ShiftriInstr>(new I64ShiftriInstr); break;
    case OpCodeType::I8_DLOAD_ADD:         ret = std::shared_ptr<I8DloadAddInstr>(new I8DloadAddInstr); break;
    case OpCodeType::I8_DLOAD_SUB:         ret = std::shared_ptr<I8DloadSubInstr>(new I8DloadSubInstr); break;
    case OpCodeType::I8_DLOAD_MUL:         ret = std::shared_ptr<I8DloadMulInstr>(new I8DloadMulInstr); break;
    case OpCodeType::I8_DLOAD_MAX:         ret = std::shared_ptr<I8DloadMaxInstr>(new I8DloadMaxInstr); break;
    case OpCodeType::I8_DLOAD_MIN:         ret = std::shared_ptr<I8DloadMinInstr>(new I8DloadMinInstr); break;
    case OpCodeType::I8_LOAD_ADD:          ret = std::shared_ptr<I8LoadAddInstr>(new I8LoadAddInstr); break;
    case OpCodeType::I8_LOAD_SUB:          ret = std::shared_ptr<I8LoadSubInstr>(new I8LoadSubInstr); break;
    case OpCodeType::I8_LOAD_MUL:          ret = std::shared_ptr<I8LoadMulInstr>(new I8LoadMulInstr); break;
    case OpCodeType::I8_LOAD_MAX:          ret = std::shared_ptr<I8LoadMaxInstr>(new I8LoadMaxInstr); break;
    case OpCodeType::I8_LOAD_MIN:          ret = std::shared_ptr<I8LoadMinInstr>(new I8LoadMinInstr); break;
    case OpCodeType::U8_DLOAD_ADD:         ret = std::shared_ptr<U8DloadAddInstr>(new U8DloadAddInstr); break;
    case OpCodeType::U8_DLOAD_SUB:         ret = std::shared_ptr<U8DloadSubInstr>(new U8DloadSubInstr); break;
    case OpCodeType::U8_DLOAD_MUL:         ret = std::shared_ptr<U8DloadMulInstr>(new U8DloadMulInstr); break;
    case OpCodeType::U8_DLOAD_MAX:         ret = std::shared_ptr<U8DloadMaxInstr>(new U8DloadMaxInstr); break;
    case OpCodeType::U8_DLOAD_MIN:         ret = std::shared_ptr<U8DloadMinInstr>(new U8DloadMinInstr); break;
    case OpCodeType::U8_LOAD_ADD:          ret = std::shared_ptr<U8LoadAddInstr>(new U8LoadAddInstr); break;
    case OpCodeType::U8_LOAD_SUB:          ret = std::shared_ptr<U8LoadSubInstr>(new U8LoadSubInstr); break;
    case OpCodeType::U8_LOAD_MUL:          ret = std::shared_ptr<U8LoadMulInstr>(new U8LoadMulInstr); break;
    case OpCodeType::U8_LOAD_MAX:          ret = std::shared_ptr<U8LoadMaxInstr>(new U8LoadMaxInstr); break;
    case OpCodeType::U8_LOAD_MIN:          ret = std::shared_ptr<U8LoadMinInstr>(new U8LoadMinInstr); break;
    case OpCodeType::F16_DLOAD_ADD:        ret = std::shared_ptr<F16DloadAddInstr>(new F16DloadAddInstr); break;
    case OpCodeType::F16_DLOAD_SUB:        ret = std::shared_ptr<F16DloadSubInstr>(new F16DloadSubInstr); break;
    case OpCodeType::F16_DLOAD_MUL:        ret = std::shared_ptr<F16DloadMulInstr>(new F16DloadMulInstr); break;
    case OpCodeType::F16_DLOAD_MAX:        ret = std::shared_ptr<F16DloadMaxInstr>(new F16DloadMaxInstr); break;
    case OpCodeType::F16_DLOAD_MIN:        ret = std::shared_ptr<F16DloadMinInstr>(new F16DloadMinInstr); break;
    case OpCodeType::F16_LOAD_ADD:         ret = std::shared_ptr<F16LoadAddInstr>(new F16LoadAddInstr); break;
    case OpCodeType::F16_LOAD_SUB:         ret = std::shared_ptr<F16LoadSubInstr>(new F16LoadSubInstr); break;
    case OpCodeType::F16_LOAD_MUL:         ret = std::shared_ptr<F16LoadMulInstr>(new F16LoadMulInstr); break;
    case OpCodeType::F16_LOAD_MAX:         ret = std::shared_ptr<F16LoadMaxInstr>(new F16LoadMaxInstr); break;
    case OpCodeType::F16_LOAD_MIN:         ret = std::shared_ptr<F16LoadMinInstr>(new F16LoadMinInstr); break;
    case OpCodeType::F32_DLOAD_ADD:        ret = std::shared_ptr<F32DloadAddInstr>(new F32DloadAddInstr); break;
    case OpCodeType::F32_DLOAD_SUB:        ret = std::shared_ptr<F32DloadSubInstr>(new F32DloadSubInstr); break;
    case OpCodeType::F32_DLOAD_MUL:        ret = std::shared_ptr<F32DloadMulInstr>(new F32DloadMulInstr); break;
    case OpCodeType::F32_DLOAD_MAX:        ret = std::shared_ptr<F32DloadMaxInstr>(new F32DloadMaxInstr); break;
    case OpCodeType::F32_DLOAD_MIN:        ret = std::shared_ptr<F32DloadMinInstr>(new F32DloadMinInstr); break;
    case OpCodeType::F32_LOAD_ADD:         ret = std::shared_ptr<F32LoadAddInstr>(new F32LoadAddInstr); break;
    case OpCodeType::F32_LOAD_SUB:         ret = std::shared_ptr<F32LoadSubInstr>(new F32LoadSubInstr); break;
    case OpCodeType::F32_LOAD_MUL:         ret = std::shared_ptr<F32LoadMulInstr>(new F32LoadMulInstr); break;
    case OpCodeType::F32_LOAD_MAX:         ret = std::shared_ptr<F32LoadMaxInstr>(new F32LoadMaxInstr); break;
    case OpCodeType::F32_LOAD_MIN:         ret = std::shared_ptr<F32LoadMinInstr>(new F32LoadMinInstr); break;
    case OpCodeType::I32_DLOAD_ADD:        ret = std::shared_ptr<I32DloadAddInstr>(new I32DloadAddInstr); break;
    case OpCodeType::I32_DLOAD_SUB:        ret = std::shared_ptr<I32DloadSubInstr>(new I32DloadSubInstr); break;
    case OpCodeType::I32_DLOAD_MUL:        ret = std::shared_ptr<I32DloadMulInstr>(new I32DloadMulInstr); break;
    case OpCodeType::I32_DLOAD_MAX:        ret = std::shared_ptr<I32DloadMaxInstr>(new I32DloadMaxInstr); break;
    case OpCodeType::I32_DLOAD_MIN:        ret = std::shared_ptr<I32DloadMinInstr>(new I32DloadMinInstr); break;
    case OpCodeType::I32_DLOAD_AND:        ret = std::shared_ptr<I32DloadAndInstr>(new I32DloadAndInstr); break;
    case OpCodeType::I32_DLOAD_OR:         ret = std::shared_ptr<I32DloadOrInstr>(new I32DloadOrInstr); break;
    case OpCodeType::I32_DLOAD_NOT:        ret = std::shared_ptr<I32DloadNotInstr>(new I32DloadNotInstr); break;
    case OpCodeType::I32_DLOAD_XOR:        ret = std::shared_ptr<I32DloadXorInstr>(new I32DloadXorInstr); break;
    case OpCodeType::I32_DLOAD_XNOR:       ret = std::shared_ptr<I32DloadXnorInstr>(new I32DloadXnorInstr); break;
    case OpCodeType::I32_LOAD_ADD:         ret = std::shared_ptr<I32LoadAddInstr>(new I32LoadAddInstr); break;
    case OpCodeType::I32_LOAD_SUB:         ret = std::shared_ptr<I32LoadSubInstr>(new I32LoadSubInstr); break;
    case OpCodeType::I32_LOAD_MUL:         ret = std::shared_ptr<I32LoadMulInstr>(new I32LoadMulInstr); break;
    case OpCodeType::I32_LOAD_MAX:         ret = std::shared_ptr<I32LoadMaxInstr>(new I32LoadMaxInstr); break;
    case OpCodeType::I32_LOAD_MIN:         ret = std::shared_ptr<I32LoadMinInstr>(new I32LoadMinInstr); break;
    case OpCodeType::I32_LOAD_AND:         ret = std::shared_ptr<I32LoadAndInstr>(new I32LoadAndInstr); break;
    case OpCodeType::I32_LOAD_OR:          ret = std::shared_ptr<I32LoadOrInstr>(new I32LoadOrInstr); break;
    case OpCodeType::I32_LOAD_NOT:         ret = std::shared_ptr<I32LoadNotInstr>(new I32LoadNotInstr); break;
    case OpCodeType::I32_LOAD_XOR:         ret = std::shared_ptr<I32LoadXorInstr>(new I32LoadXorInstr); break;
    case OpCodeType::I32_LOAD_XNOR:        ret = std::shared_ptr<I32LoadXnorInstr>(new I32LoadXnorInstr); break;
    case OpCodeType::I8_ADD_STORE:         ret = std::shared_ptr<I8AddStoreInstr>(new I8AddStoreInstr); break;
    case OpCodeType::I8_SUB_STORE:         ret = std::shared_ptr<I8SubStoreInstr>(new I8SubStoreInstr); break;
    case OpCodeType::I8_MUL_STORE:         ret = std::shared_ptr<I8MulStoreInstr>(new I8MulStoreInstr); break;
    case OpCodeType::I8_MAX_STORE:         ret = std::shared_ptr<I8MaxStoreInstr>(new I8MaxStoreInstr); break;
    case OpCodeType::I8_MIN_STORE:         ret = std::shared_ptr<I8MinStoreInstr>(new I8MinStoreInstr); break;
    case OpCodeType::U8_ADD_STORE:         ret = std::shared_ptr<U8AddStoreInstr>(new U8AddStoreInstr); break;
    case OpCodeType::U8_SUB_STORE:         ret = std::shared_ptr<U8SubStoreInstr>(new U8SubStoreInstr); break;
    case OpCodeType::U8_MUL_STORE:         ret = std::shared_ptr<U8MulStoreInstr>(new U8MulStoreInstr); break;
    case OpCodeType::U8_MAX_STORE:         ret = std::shared_ptr<U8MaxStoreInstr>(new U8MaxStoreInstr); break;
    case OpCodeType::U8_MIN_STORE:         ret = std::shared_ptr<U8MinStoreInstr>(new U8MinStoreInstr); break;
    case OpCodeType::F16_ADD_STORE:        ret = std::shared_ptr<F16AddStoreInstr>(new F16AddStoreInstr); break;
    case OpCodeType::F16_SUB_STORE:        ret = std::shared_ptr<F16SubStoreInstr>(new F16SubStoreInstr); break;
    case OpCodeType::F16_MUL_STORE:        ret = std::shared_ptr<F16MulStoreInstr>(new F16MulStoreInstr); break;
    case OpCodeType::F16_MAX_STORE:        ret = std::shared_ptr<F16MaxStoreInstr>(new F16MaxStoreInstr); break;
    case OpCodeType::F16_MIN_STORE:        ret = std::shared_ptr<F16MinStoreInstr>(new F16MinStoreInstr); break;
    case OpCodeType::F32_ADD_STORE:        ret = std::shared_ptr<F32AddStoreInstr>(new F32AddStoreInstr); break;
    case OpCodeType::F32_SUB_STORE:        ret = std::shared_ptr<F32SubStoreInstr>(new F32SubStoreInstr); break;
    case OpCodeType::F32_MUL_STORE:        ret = std::shared_ptr<F32MulStoreInstr>(new F32MulStoreInstr); break;
    case OpCodeType::F32_MAX_STORE:        ret = std::shared_ptr<F32MaxStoreInstr>(new F32MaxStoreInstr); break;
    case OpCodeType::F32_MIN_STORE:        ret = std::shared_ptr<F32MinStoreInstr>(new F32MinStoreInstr); break;
    case OpCodeType::I32_ADD_STORE:        ret = std::shared_ptr<I32AddStoreInstr>(new I32AddStoreInstr); break;
    case OpCodeType::I32_SUB_STORE:        ret = std::shared_ptr<I32SubStoreInstr>(new I32SubStoreInstr); break;
    case OpCodeType::I32_MUL_STORE:        ret = std::shared_ptr<I32MulStoreInstr>(new I32MulStoreInstr); break;
    case OpCodeType::I32_MAX_STORE:        ret = std::shared_ptr<I32MaxStoreInstr>(new I32MaxStoreInstr); break;
    case OpCodeType::I32_MIN_STORE:        ret = std::shared_ptr<I32MinStoreInstr>(new I32MinStoreInstr); break;
    case OpCodeType::I32_AND_STORE:        ret = std::shared_ptr<I32AndStoreInstr>(new I32AndStoreInstr); break;
    case OpCodeType::I32_OR_STORE:         ret = std::shared_ptr<I32OrStoreInstr>(new I32OrStoreInstr); break;
    case OpCodeType::I32_NOT_STORE:        ret = std::shared_ptr<I32NotStoreInstr>(new I32NotStoreInstr); break;
    case OpCodeType::I32_XOR_STORE:        ret = std::shared_ptr<I32XorStoreInstr>(new I32XorStoreInstr); break;
    case OpCodeType::I32_XNOR_STORE:       ret = std::shared_ptr<I32XnorStoreInstr>(new I32XnorStoreInstr); break;
    case OpCodeType::I8_DLOAD_ADD_STORE:   ret = std::shared_ptr<I8DloadAddStoreInstr>(new I8DloadAddStoreInstr); break;
    case OpCodeType::I8_DLOAD_SUB_STORE:   ret = std::shared_ptr<I8DloadSubStoreInstr>(new I8DloadSubStoreInstr); break;
    case OpCodeType::I8_DLOAD_MUL_STORE:   ret = std::shared_ptr<I8DloadMulStoreInstr>(new I8DloadMulStoreInstr); break;
    case OpCodeType::I8_DLOAD_MAX_STORE:   ret = std::shared_ptr<I8DloadMaxStoreInstr>(new I8DloadMaxStoreInstr); break;
    case OpCodeType::I8_DLOAD_MIN_STORE:   ret = std::shared_ptr<I8DloadMinStoreInstr>(new I8DloadMinStoreInstr); break;
    case OpCodeType::I8_LOAD_ADD_STORE:    ret = std::shared_ptr<I8LoadAddStoreInstr>(new I8LoadAddStoreInstr); break;
    case OpCodeType::I8_LOAD_SUB_STORE:    ret = std::shared_ptr<I8LoadSubStoreInstr>(new I8LoadSubStoreInstr); break;
    case OpCodeType::I8_LOAD_MUL_STORE:    ret = std::shared_ptr<I8LoadMulStoreInstr>(new I8LoadMulStoreInstr); break;
    case OpCodeType::I8_LOAD_MAX_STORE:    ret = std::shared_ptr<I8LoadMaxStoreInstr>(new I8LoadMaxStoreInstr); break;
    case OpCodeType::I8_LOAD_MIN_STORE:    ret = std::shared_ptr<I8LoadMinStoreInstr>(new I8LoadMinStoreInstr); break;
    case OpCodeType::U8_DLOAD_ADD_STORE:   ret = std::shared_ptr<U8DloadAddStoreInstr>(new U8DloadAddStoreInstr); break;
    case OpCodeType::U8_DLOAD_SUB_STORE:   ret = std::shared_ptr<U8DloadSubStoreInstr>(new U8DloadSubStoreInstr); break;
    case OpCodeType::U8_DLOAD_MUL_STORE:   ret = std::shared_ptr<U8DloadMulStoreInstr>(new U8DloadMulStoreInstr); break;
    case OpCodeType::U8_DLOAD_MAX_STORE:   ret = std::shared_ptr<U8DloadMaxStoreInstr>(new U8DloadMaxStoreInstr); break;
    case OpCodeType::U8_DLOAD_MIN_STORE:   ret = std::shared_ptr<U8DloadMinStoreInstr>(new U8DloadMinStoreInstr); break;
    case OpCodeType::U8_LOAD_ADD_STORE:    ret = std::shared_ptr<U8LoadAddStoreInstr>(new U8LoadAddStoreInstr); break;
    case OpCodeType::U8_LOAD_SUB_STORE:    ret = std::shared_ptr<U8LoadSubStoreInstr>(new U8LoadSubStoreInstr); break;
    case OpCodeType::U8_LOAD_MUL_STORE:    ret = std::shared_ptr<U8LoadMulStoreInstr>(new U8LoadMulStoreInstr); break;
    case OpCodeType::U8_LOAD_MAX_STORE:    ret = std::shared_ptr<U8LoadMaxStoreInstr>(new U8LoadMaxStoreInstr); break;
    case OpCodeType::U8_LOAD_MIN_STORE:    ret = std::shared_ptr<U8LoadMinStoreInstr>(new U8LoadMinStoreInstr); break;
    case OpCodeType::F16_DLOAD_ADD_STORE:  ret = std::shared_ptr<F16DloadAddStoreInstr>(new F16DloadAddStoreInstr); break;
    case OpCodeType::F16_DLOAD_SUB_STORE:  ret = std::shared_ptr<F16DloadSubStoreInstr>(new F16DloadSubStoreInstr); break;
    case OpCodeType::F16_DLOAD_MUL_STORE:  ret = std::shared_ptr<F16DloadMulStoreInstr>(new F16DloadMulStoreInstr); break;
    case OpCodeType::F16_DLOAD_MAX_STORE:  ret = std::shared_ptr<F16DloadMaxStoreInstr>(new F16DloadMaxStoreInstr); break;
    case OpCodeType::F16_DLOAD_MIN_STORE:  ret = std::shared_ptr<F16DloadMinStoreInstr>(new F16DloadMinStoreInstr); break;
    case OpCodeType::F16_LOAD_ADD_STORE:   ret = std::shared_ptr<F16LoadAddStoreInstr>(new F16LoadAddStoreInstr); break;
    case OpCodeType::F16_LOAD_SUB_STORE:   ret = std::shared_ptr<F16LoadSubStoreInstr>(new F16LoadSubStoreInstr); break;
    case OpCodeType::F16_LOAD_MUL_STORE:   ret = std::shared_ptr<F16LoadMulStoreInstr>(new F16LoadMulStoreInstr); break;
    case OpCodeType::F16_LOAD_MAX_STORE:   ret = std::shared_ptr<F16LoadMaxStoreInstr>(new F16LoadMaxStoreInstr); break;
    case OpCodeType::F16_LOAD_MIN_STORE:   ret = std::shared_ptr<F16LoadMinStoreInstr>(new F16LoadMinStoreInstr); break;
    case OpCodeType::F32_DLOAD_ADD_STORE:  ret = std::shared_ptr<F32DloadAddStoreInstr>(new F32DloadAddStoreInstr); break;
    case OpCodeType::F32_DLOAD_SUB_STORE:  ret = std::shared_ptr<F32DloadSubStoreInstr>(new F32DloadSubStoreInstr); break;
    case OpCodeType::F32_DLOAD_MUL_STORE:  ret = std::shared_ptr<F32DloadMulStoreInstr>(new F32DloadMulStoreInstr); break;
    case OpCodeType::F32_DLOAD_MAX_STORE:  ret = std::shared_ptr<F32DloadMaxStoreInstr>(new F32DloadMaxStoreInstr); break;
    case OpCodeType::F32_DLOAD_MIN_STORE:  ret = std::shared_ptr<F32DloadMinStoreInstr>(new F32DloadMinStoreInstr); break;
    case OpCodeType::F32_LOAD_ADD_STORE:   ret = std::shared_ptr<F32LoadAddStoreInstr>(new F32LoadAddStoreInstr); break;
    case OpCodeType::F32_LOAD_SUB_STORE:   ret = std::shared_ptr<F32LoadSubStoreInstr>(new F32LoadSubStoreInstr); break;
    case OpCodeType::F32_LOAD_MUL_STORE:   ret = std::shared_ptr<F32LoadMulStoreInstr>(new F32LoadMulStoreInstr); break;
    case OpCodeType::F32_LOAD_MAX_STORE:   ret = std::shared_ptr<F32LoadMaxStoreInstr>(new F32LoadMaxStoreInstr); break;
    case OpCodeType::F32_LOAD_MIN_STORE:   ret = std::shared_ptr<F32LoadMinStoreInstr>(new F32LoadMinStoreInstr); break;
    case OpCodeType::I32_DLOAD_ADD_STORE:  ret = std::shared_ptr<I32DloadAddStoreInstr>(new I32DloadAddStoreInstr); break;
    case OpCodeType::I32_DLOAD_SUB_STORE:  ret = std::shared_ptr<I32DloadSubStoreInstr>(new I32DloadSubStoreInstr); break;
    case OpCodeType::I32_DLOAD_MUL_STORE:  ret = std::shared_ptr<I32DloadMulStoreInstr>(new I32DloadMulStoreInstr); break;
    case OpCodeType::I32_DLOAD_MAX_STORE:  ret = std::shared_ptr<I32DloadMaxStoreInstr>(new I32DloadMaxStoreInstr); break;
    case OpCodeType::I32_DLOAD_MIN_STORE:  ret = std::shared_ptr<I32DloadMinStoreInstr>(new I32DloadMinStoreInstr); break;
    case OpCodeType::I32_DLOAD_AND_STORE:  ret = std::shared_ptr<I32DloadAndStoreInstr>(new I32DloadAndStoreInstr); break;
    case OpCodeType::I32_DLOAD_OR_STORE:   ret = std::shared_ptr<I32DloadOrStoreInstr>(new I32DloadOrStoreInstr); break;
    case OpCodeType::I32_DLOAD_NOT_STORE:  ret = std::shared_ptr<I32DloadNotStoreInstr>(new I32DloadNotStoreInstr); break;
    case OpCodeType::I32_DLOAD_XOR_STORE:  ret = std::shared_ptr<I32DloadXorStoreInstr>(new I32DloadXorStoreInstr); break;
    case OpCodeType::I32_DLOAD_XNOR_STORE: ret = std::shared_ptr<I32DloadXnorStoreInstr>(new I32DloadXnorStoreInstr); break;
    case OpCodeType::I32_LOAD_ADD_STORE:   ret = std::shared_ptr<I32LoadAddStoreInstr>(new I32LoadAddStoreInstr); break;
    case OpCodeType::I32_LOAD_SUB_STORE:   ret = std::shared_ptr<I32LoadSubStoreInstr>(new I32LoadSubStoreInstr); break;
    case OpCodeType::I32_LOAD_MUL_STORE:   ret = std::shared_ptr<I32LoadMulStoreInstr>(new I32LoadMulStoreInstr); break;
    case OpCodeType::I32_LOAD_MAX_STORE:   ret = std::shared_ptr<I32LoadMaxStoreInstr>(new I32LoadMaxStoreInstr); break;
    case OpCodeType::I32_LOAD_MIN_STORE:   ret = std::shared_ptr<I32LoadMinStoreInstr>(new I32LoadMinStoreInstr); break;
    case OpCodeType::I32_LOAD_AND_STORE:   ret = std::shared_ptr<I32LoadAndStoreInstr>(new I32LoadAndStoreInstr); break;
    case OpCodeType::I32_LOAD_OR_STORE:    ret = std::shared_ptr<I32LoadOrStoreInstr>(new I32LoadOrStoreInstr); break;
    case OpCodeType::I32_LOAD_NOT_STORE:   ret = std::shared_ptr<I32LoadNotStoreInstr>(new I32LoadNotStoreInstr); break;
    case OpCodeType::I32_LOAD_XOR_STORE:   ret = std::shared_ptr<I32LoadXorStoreInstr>(new I32LoadXorStoreInstr); break;
    case OpCodeType::I32_LOAD_XNOR_STORE:  ret = std::shared_ptr<I32LoadXnorStoreInstr>(new I32LoadXnorStoreInstr); break;
    case OpCodeType::S_ADDI:               ret = std::shared_ptr<SAddiInstr>(new SAddiInstr); break;
    case OpCodeType::S_ADDIU:              ret = std::shared_ptr<SAddiuInstr>(new SAddiuInstr); break;
    case OpCodeType::S_SUBI:               ret = std::shared_ptr<SSubiInstr>(new SSubiInstr); break;
    case OpCodeType::S_ANDI:               ret = std::shared_ptr<SAndiInstr>(new SAndiInstr); break;
    case OpCodeType::S_ORI:                ret = std::shared_ptr<SOriInstr>(new SOriInstr); break;
    case OpCodeType::S_XORI:               ret = std::shared_ptr<SXoriInstr>(new SXoriInstr); break;
    case OpCodeType::S_SGTI:               ret = std::shared_ptr<SSgtiInstr>(new SSgtiInstr); break;
    case OpCodeType::S_SLTI:               ret = std::shared_ptr<SSltiInstr>(new SSltiInstr); break;
    case OpCodeType::S_SEQI:               ret = std::shared_ptr<SSeqiInstr>(new SSeqiInstr); break;
    case OpCodeType::S_SGEI:               ret = std::shared_ptr<SSgeiInstr>(new SSgeiInstr); break;
    case OpCodeType::S_SLEI:               ret = std::shared_ptr<SSleiInstr>(new SSleiInstr); break;
    case OpCodeType::S_SRAI:               ret = std::shared_ptr<SSraiInstr>(new SSraiInstr); break;
    case OpCodeType::S_SRLI:               ret = std::shared_ptr<SSrliInstr>(new SSrliInstr); break;
    case OpCodeType::S_SLLI:               ret = std::shared_ptr<SSlliInstr>(new SSlliInstr); break;
    case OpCodeType::S_LUI:                ret = std::shared_ptr<SLuiInstr>(new SLuiInstr); break;
    case OpCodeType::S_ADD:                ret = std::shared_ptr<SAddInstr>(new SAddInstr); break;
    case OpCodeType::S_ADDU:               ret = std::shared_ptr<SAdduInstr>(new SAdduInstr); break;
    case OpCodeType::S_SUB:                ret = std::shared_ptr<SSubInstr>(new SSubInstr); break;
    case OpCodeType::S_MUL:                ret = std::shared_ptr<SMulInstr>(new SMulInstr); break;
    case OpCodeType::S_MULH:               ret = std::shared_ptr<SMulhInstr>(new SMulhInstr); break;
    case OpCodeType::S_MULHU:              ret = std::shared_ptr<SMulhuInstr>(new SMulhuInstr); break;
    case OpCodeType::S_MIN:                ret = std::shared_ptr<SMinInstr>(new SMinInstr); break;
    case OpCodeType::S_MAX:                ret = std::shared_ptr<SMaxInstr>(new SMaxInstr); break;
    case OpCodeType::S_MINU:               ret = std::shared_ptr<SMinuInstr>(new SMinuInstr); break;
    case OpCodeType::S_MAXU:               ret = std::shared_ptr<SMaxuInstr>(new SMaxuInstr); break;
    case OpCodeType::S_AND:                ret = std::shared_ptr<SAndInstr>(new SAndInstr); break;
    case OpCodeType::S_OR:                 ret = std::shared_ptr<SOrInstr>(new SOrInstr); break;
    case OpCodeType::S_XOR:                ret = std::shared_ptr<SXorInstr>(new SXorInstr); break;
    case OpCodeType::S_SGT:                ret = std::shared_ptr<SSgtInstr>(new SSgtInstr); break;
    case OpCodeType::S_SLT:                ret = std::shared_ptr<SSltInstr>(new SSltInstr); break;
    case OpCodeType::S_SEQ:                ret = std::shared_ptr<SSeqInstr>(new SSeqInstr); break;
    case OpCodeType::S_SGE:                ret = std::shared_ptr<SSgeInstr>(new SSgeInstr); break;
    case OpCodeType::S_SLE:                ret = std::shared_ptr<SSleInstr>(new SSleInstr); break;
    case OpCodeType::S_CMPSEL:             ret = std::shared_ptr<SCmpselInstr>(new SCmpselInstr); break;
    case OpCodeType::S_SRA:                ret = std::shared_ptr<SSraInstr>(new SSraInstr); break;
    case OpCodeType::S_SRL:                ret = std::shared_ptr<SSrlInstr>(new SSrlInstr); break;
    case OpCodeType::S_SLL:                ret = std::shared_ptr<SSllInstr>(new SSllInstr); break;
    case OpCodeType::S_JUMP:               ret = std::shared_ptr<SJumpInstr>(new SJumpInstr); break;
    case OpCodeType::S_JAL:                ret = std::shared_ptr<SJalInstr>(new SJalInstr); break;
    case OpCodeType::S_JR:                 ret = std::shared_ptr<SJrInstr>(new SJrInstr); break;
    case OpCodeType::S_BNE:                ret = std::shared_ptr<SBneInstr>(new SBneInstr); break;
    case OpCodeType::S_BEQ:                ret = std::shared_ptr<SBeqInstr>(new SBeqInstr); break;
    case OpCodeType::S_BLT:                ret = std::shared_ptr<SBltInstr>(new SBltInstr); break;
    case OpCodeType::S_BLTU:               ret = std::shared_ptr<SBltuInstr>(new SBltuInstr); break;
    case OpCodeType::S_BGE:                ret = std::shared_ptr<SBgeInstr>(new SBgeInstr); break;
    case OpCodeType::S_BGEU:               ret = std::shared_ptr<SBgeuInstr>(new SBgeuInstr); break;
    case OpCodeType::S_SETCFG:             ret = std::shared_ptr<SSetcfgInstr>(new SSetcfgInstr); break;
    case OpCodeType::S_SETRF:              ret = std::shared_ptr<SSetrfInstr>(new SSetrfInstr); break;
    default: break;
  }
  if (args.size() > 0)
    ret->setArgs(args);
  return ret;
}

} // namespace quark

#endif // QUARK_VPU_INSTR_H
