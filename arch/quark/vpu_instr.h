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
  SCALAR_ALU = 0,
  FP32_ALU,
  INT32_ALU,
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
    SET_CMASK_HIGH = 9,
    NOP = 10,
    OP_END = 11,
    I8_DLOAD = 12,
    I8_LOAD = 13,
    U8_DLOAD = 14,
    U8_LOAD = 15,
    E43_DLOAD = 16,
    E43_LOAD = 17,
    E52_DLOAD = 18,
    E52_LOAD = 19,
    I16_DLOAD = 20,
    I16_LOAD = 21,
    F16_DLOAD = 22,
    F16_LOAD = 23,
    F32_DLOAD = 24,
    F32_LOAD = 25,
    I32_DLOAD = 26,
    I32_LOAD = 27,
    I8_STORE = 28,
    U8_STORE = 29,
    E43_STORE = 30,
    E52_STORE = 31,
    I16_STORE = 32,
    F16_STORE = 33,
    F32_STORE = 34,
    I32_I32STORE = 35,
    F32_ADD = 36,
    F32_SUB = 37,
    F32_MUL = 38,
    F32_MAX = 39,
    F32_MIN = 40,
    F32_GREATER = 41,
    F32_GREATER_EQUAL = 42,
    F32_LESS = 43,
    F32_LESS_EQUAL = 44,
    F32_EQUAL = 45,
    F32_SEL = 46,
    F32_ADDI = 47,
    F32_SUBI = 48,
    F32_MULI = 49,
    F32_MAXI = 50,
    F32_MINI = 51,
    F32_MOVI = 52,
    F32_GREATERI = 53,
    F32_GREATER_EQUALI = 54,
    F32_LESSI = 55,
    F32_LESS_EQUALI = 56,
    F32_EQUALI = 57,
    F32_SELI = 58,
    F32_LUI = 59,
    F32_LI = 60,
    I32_ADD = 61,
    I32_SUB = 62,
    I32_MUL = 63,
    I32_MAX = 64,
    I32_MIN = 65,
    I32_GREATER = 66,
    I32_GREATER_EQUAL = 67,
    I32_LESS = 68,
    I32_LESS_EQUAL = 69,
    I32_EQUAL = 70,
    I32_SEL = 71,
    I32_ADDI = 72,
    I32_SUBI = 73,
    I32_MULI = 74,
    I32_MAXI = 75,
    I32_MINI = 76,
    I32_MOVI = 77,
    I32_GREATERI = 78,
    I32_GREATER_EQUALI = 79,
    I32_LESSI = 80,
    I32_LESS_EQUALI = 81,
    I32_EQUALI = 82,
    I32_SELI = 83,
    I32_LUI = 84,
    I32_LI = 85,
    I8_DLOAD_ADD = 86,
    I8_DLOAD_SUB = 87,
    I8_DLOAD_MUL = 88,
    I8_DLOAD_MAX = 89,
    I8_DLOAD_MIN = 90,
    I8_LOAD_ADD = 91,
    I8_LOAD_SUB = 92,
    I8_LOAD_MUL = 93,
    I8_LOAD_MAX = 94,
    I8_LOAD_MIN = 95,
    U8_DLOAD_ADD = 96,
    U8_DLOAD_SUB = 97,
    U8_DLOAD_MUL = 98,
    U8_DLOAD_MAX = 99,
    U8_DLOAD_MIN = 100,
    U8_LOAD_ADD = 101,
    U8_LOAD_SUB = 102,
    U8_LOAD_MUL = 103,
    U8_LOAD_MAX = 104,
    U8_LOAD_MIN = 105,
    E43_DLOAD_ADD = 106,
    E43_DLOAD_SUB = 107,
    E43_DLOAD_MUL = 108,
    E43_DLOAD_MAX = 109,
    E43_DLOAD_MIN = 110,
    E43_LOAD_ADD = 111,
    E43_LOAD_SUB = 112,
    E43_LOAD_MUL = 113,
    E43_LOAD_MAX = 114,
    E43_LOAD_MIN = 115,
    E52_DLOAD_ADD = 116,
    E52_DLOAD_SUB = 117,
    E52_DLOAD_MUL = 118,
    E52_DLOAD_MAX = 119,
    E52_DLOAD_MIN = 120,
    E52_LOAD_ADD = 121,
    E52_LOAD_SUB = 122,
    E52_LOAD_MUL = 123,
    E52_LOAD_MAX = 124,
    E52_LOAD_MIN = 125,
    I16_DLOAD_ADD = 126,
    I16_DLOAD_SUB = 127,
    I16_DLOAD_MUL = 128,
    I16_DLOAD_MAX = 129,
    I16_DLOAD_MIN = 130,
    I16_LOAD_ADD = 131,
    I16_LOAD_SUB = 132,
    I16_LOAD_MUL = 133,
    I16_LOAD_MAX = 134,
    I16_LOAD_MIN = 135,
    F16_DLOAD_ADD = 136,
    F16_DLOAD_SUB = 137,
    F16_DLOAD_MUL = 138,
    F16_DLOAD_MAX = 139,
    F16_DLOAD_MIN = 140,
    F16_LOAD_ADD = 141,
    F16_LOAD_SUB = 142,
    F16_LOAD_MUL = 143,
    F16_LOAD_MAX = 144,
    F16_LOAD_MIN = 145,
    F32_DLOAD_ADD = 146,
    F32_DLOAD_SUB = 147,
    F32_DLOAD_MUL = 148,
    F32_DLOAD_MAX = 149,
    F32_DLOAD_MIN = 150,
    F32_LOAD_ADD = 151,
    F32_LOAD_SUB = 152,
    F32_LOAD_MUL = 153,
    F32_LOAD_MAX = 154,
    F32_LOAD_MIN = 155,
    I32_DLOAD_ADD = 156,
    I32_DLOAD_SUB = 157,
    I32_DLOAD_MUL = 158,
    I32_DLOAD_MAX = 159,
    I32_DLOAD_MIN = 160,
    I32_LOAD_ADD = 161,
    I32_LOAD_SUB = 162,
    I32_LOAD_MUL = 163,
    I32_LOAD_MAX = 164,
    I32_LOAD_MIN = 165,
    I8_ADD_STORE = 166,
    I8_SUB_STORE = 167,
    I8_MUL_STORE = 168,
    I8_MAX_STORE = 169,
    I8_MIN_STORE = 170,
    U8_ADD_STORE = 171,
    U8_SUB_STORE = 172,
    U8_MUL_STORE = 173,
    U8_MAX_STORE = 174,
    U8_MIN_STORE = 175,
    E43_ADD_STORE = 176,
    E43_SUB_STORE = 177,
    E43_MUL_STORE = 178,
    E43_MAX_STORE = 179,
    E43_MIN_STORE = 180,
    E52_ADD_STORE = 181,
    E52_SUB_STORE = 182,
    E52_MUL_STORE = 183,
    E52_MAX_STORE = 184,
    E52_MIN_STORE = 185,
    I16_ADD_STORE = 186,
    I16_SUB_STORE = 187,
    I16_MUL_STORE = 188,
    I16_MAX_STORE = 189,
    I16_MIN_STORE = 190,
    F16_ADD_STORE = 191,
    F16_SUB_STORE = 192,
    F16_MUL_STORE = 193,
    F16_MAX_STORE = 194,
    F16_MIN_STORE = 195,
    F32_ADD_STORE = 196,
    F32_SUB_STORE = 197,
    F32_MUL_STORE = 198,
    F32_MAX_STORE = 199,
    F32_MIN_STORE = 200,
    I32_ADD_STORE = 201,
    I32_SUB_STORE = 202,
    I32_MUL_STORE = 203,
    I32_MAX_STORE = 204,
    I32_MIN_STORE = 205,
    I8_DLOAD_ADD_STORE = 206,
    I8_DLOAD_SUB_STORE = 207,
    I8_DLOAD_MUL_STORE = 208,
    I8_DLOAD_MAX_STORE = 209,
    I8_DLOAD_MIN_STORE = 210,
    I8_LOAD_ADD_STORE = 211,
    I8_LOAD_SUB_STORE = 212,
    I8_LOAD_MUL_STORE = 213,
    I8_LOAD_MAX_STORE = 214,
    I8_LOAD_MIN_STORE = 215,
    U8_DLOAD_ADD_STORE = 216,
    U8_DLOAD_SUB_STORE = 217,
    U8_DLOAD_MUL_STORE = 218,
    U8_DLOAD_MAX_STORE = 219,
    U8_DLOAD_MIN_STORE = 220,
    U8_LOAD_ADD_STORE = 221,
    U8_LOAD_SUB_STORE = 222,
    U8_LOAD_MUL_STORE = 223,
    U8_LOAD_MAX_STORE = 224,
    U8_LOAD_MIN_STORE = 225,
    E43_DLOAD_ADD_STORE = 226,
    E43_DLOAD_SUB_STORE = 227,
    E43_DLOAD_MUL_STORE = 228,
    E43_DLOAD_MAX_STORE = 229,
    E43_DLOAD_MIN_STORE = 230,
    E43_LOAD_ADD_STORE = 231,
    E43_LOAD_SUB_STORE = 232,
    E43_LOAD_MUL_STORE = 233,
    E43_LOAD_MAX_STORE = 234,
    E43_LOAD_MIN_STORE = 235,
    E52_DLOAD_ADD_STORE = 236,
    E52_DLOAD_SUB_STORE = 237,
    E52_DLOAD_MUL_STORE = 238,
    E52_DLOAD_MAX_STORE = 239,
    E52_DLOAD_MIN_STORE = 240,
    E52_LOAD_ADD_STORE = 241,
    E52_LOAD_SUB_STORE = 242,
    E52_LOAD_MUL_STORE = 243,
    E52_LOAD_MAX_STORE = 244,
    E52_LOAD_MIN_STORE = 245,
    I16_DLOAD_ADD_STORE = 246,
    I16_DLOAD_SUB_STORE = 247,
    I16_DLOAD_MUL_STORE = 248,
    I16_DLOAD_MAX_STORE = 249,
    I16_DLOAD_MIN_STORE = 250,
    I16_LOAD_ADD_STORE = 251,
    I16_LOAD_SUB_STORE = 252,
    I16_LOAD_MUL_STORE = 253,
    I16_LOAD_MAX_STORE = 254,
    I16_LOAD_MIN_STORE = 255,
    F16_DLOAD_ADD_STORE = 256,
    F16_DLOAD_SUB_STORE = 257,
    F16_DLOAD_MUL_STORE = 258,
    F16_DLOAD_MAX_STORE = 259,
    F16_DLOAD_MIN_STORE = 260,
    F16_LOAD_ADD_STORE = 261,
    F16_LOAD_SUB_STORE = 262,
    F16_LOAD_MUL_STORE = 263,
    F16_LOAD_MAX_STORE = 264,
    F16_LOAD_MIN_STORE = 265,
    F32_DLOAD_ADD_STORE = 266,
    F32_DLOAD_SUB_STORE = 267,
    F32_DLOAD_MUL_STORE = 268,
    F32_DLOAD_MAX_STORE = 269,
    F32_DLOAD_MIN_STORE = 270,
    F32_LOAD_ADD_STORE = 271,
    F32_LOAD_SUB_STORE = 272,
    F32_LOAD_MUL_STORE = 273,
    F32_LOAD_MAX_STORE = 274,
    F32_LOAD_MIN_STORE = 275,
    I32_DLOAD_ADD_STORE = 276,
    I32_DLOAD_SUB_STORE = 277,
    I32_DLOAD_MUL_STORE = 278,
    I32_DLOAD_MAX_STORE = 279,
    I32_DLOAD_MIN_STORE = 280,
    I32_LOAD_ADD_STORE = 281,
    I32_LOAD_SUB_STORE = 282,
    I32_LOAD_MUL_STORE = 283,
    I32_LOAD_MAX_STORE = 284,
    I32_LOAD_MIN_STORE = 285,
    S_ADDI = 286,
    S_ADDIU = 287,
    S_SUBI = 288,
    S_ANDI = 289,
    S_ORI = 290,
    S_XORI = 291,
    S_SGTI = 292,
    S_SLTI = 293,
    S_SEQI = 294,
    S_SGEI = 295,
    S_SLEI = 296,
    S_SRAI = 297,
    S_SRLI = 298,
    S_SLLI = 299,
    S_LUI = 300,
    S_ADD = 301,
    S_ADDU = 302,
    S_SUB = 303,
    S_MUL = 304,
    S_MULH = 305,
    S_MULHU = 306,
    S_MIN = 307,
    S_MAX = 308,
    S_MINU = 309,
    S_MAXU = 310,
    S_AND = 311,
    S_OR = 312,
    S_XOR = 313,
    S_SGT = 314,
    S_SLT = 315,
    S_SEQ = 316,
    S_SGE = 317,
    S_SLE = 318,
    S_CMPSEL = 319,
    S_SRA = 320,
    S_SRL = 321,
    S_SLL = 322,
    S_JUMP = 323,
    S_JAL = 324,
    S_JR = 325,
    S_BNE = 326,
    S_BEQ = 327,
    S_BLT = 328,
    S_BLTU = 329,
    S_BGE = 330,
    S_BGEU = 331,
    S_SETCFG = 332,
    S_SETRF = 333,
    INVALID = 65536
  };

  static const std::map<OpCodeType, OpCodeItem>& getOpCodeMap() {
    static const std::map<OpCodeType, OpCodeItem> opCodeMap = {
      {OpCodeType::SET_LOAD_BASE,       {"set_load_base",       0xff000000, 0x01000000}},
      {OpCodeType::SET_LOAD2_BASE,      {"set_load2_base",      0xff000000, 0x04000000}},
      {OpCodeType::SET_STORE_BASE,      {"set_store_base",      0xff000000, 0x07000000}},
      {OpCodeType::SET_LOAD_GLB,        {"set_load_glb",        0xff000001, 0x02000000}},
      {OpCodeType::SET_STORE_GLB,       {"set_store_glb",       0xff000001, 0x08000000}},
      {OpCodeType::SET_QUANTIZE,        {"set_quantize",        0xffff0000, 0x09000000}},
      {OpCodeType::SET_DEQUANTIZE,      {"set_dequantize",      0xff000000, 0x03000000}},
      {OpCodeType::SET_DEQUANTIZE2,     {"set_dequantize2",     0xff000000, 0x06000000}},
      {OpCodeType::SET_FS,              {"set_fs",              0xffffff00, 0x0a000000}},
      {OpCodeType::SET_CMASK_HIGH,      {"set_cmask_high",      0xff00ffff, 0x0b000000}},
      {OpCodeType::NOP,                 {"nop",                 0xffffffff, 0x00000000}},
      {OpCodeType::OP_END,              {"op_end",              0xffffffff, 0xff000000}},
      {OpCodeType::I8_DLOAD,            {"i8_dload",            0xff0f0000, 0xc4020000}},
      {OpCodeType::I8_LOAD,             {"i8_load",             0xffcf0000, 0xc4000000}},
      {OpCodeType::U8_DLOAD,            {"u8_dload",            0xff0f0000, 0xd4020000}},
      {OpCodeType::U8_LOAD,             {"u8_load",             0xffcf0000, 0xd4000000}},
      {OpCodeType::E43_DLOAD,           {"e43_dload",           0xff0f0000, 0xf6020000}},
      {OpCodeType::E43_LOAD,            {"e43_load",            0xffcf0000, 0xf6000000}},
      {OpCodeType::E52_DLOAD,           {"e52_dload",           0xff0f0000, 0xe6020000}},
      {OpCodeType::E52_LOAD,            {"e52_load",            0xffcf0000, 0xe6000000}},
      {OpCodeType::I16_DLOAD,           {"i16_dload",           0xff0f0000, 0xc6020000}},
      {OpCodeType::I16_LOAD,            {"i16_load",            0xffcf0000, 0xc6000000}},
      {OpCodeType::F16_DLOAD,           {"f16_dload",           0xff0f0000, 0xe4020000}},
      {OpCodeType::F16_LOAD,            {"f16_load",            0xffcf0000, 0xe4000000}},
      {OpCodeType::F32_DLOAD,           {"f32_dload",           0xff0f0000, 0xf4020000}},
      {OpCodeType::F32_LOAD,            {"f32_load",            0xffcf0000, 0xf4000000}},
      {OpCodeType::I32_DLOAD,           {"i32_dload",           0xff0f0000, 0xd2020000}},
      {OpCodeType::I32_LOAD,            {"i32_load",            0xffcf0000, 0xd2000000}},
      {OpCodeType::I8_STORE,            {"i8_store",            0xffcf0000, 0xcc000000}},
      {OpCodeType::U8_STORE,            {"u8_store",            0xffcf0000, 0xdc000000}},
      {OpCodeType::E43_STORE,           {"e43_store",           0xffcf0000, 0xfe000000}},
      {OpCodeType::E52_STORE,           {"e52_store",           0xffcf0000, 0xee000000}},
      {OpCodeType::I16_STORE,           {"i16_store",           0xffcf0000, 0xce000000}},
      {OpCodeType::F16_STORE,           {"f16_store",           0xffcf0000, 0xec000000}},
      {OpCodeType::F32_STORE,           {"f32_store",           0xffcf0000, 0xfc000000}},
      {OpCodeType::I32_I32STORE,        {"i32_i32store",        0xffcf0000, 0xda000000}},
      {OpCodeType::F32_ADD,             {"f32_add",             0xff007fff, 0x90000000}},
      {OpCodeType::F32_SUB,             {"f32_sub",             0xff007fff, 0x91000000}},
      {OpCodeType::F32_MUL,             {"f32_mul",             0xff007fff, 0x92000000}},
      {OpCodeType::F32_MAX,             {"f32_max",             0xff007fff, 0x93000000}},
      {OpCodeType::F32_MIN,             {"f32_min",             0xff007fff, 0x94000000}},
      {OpCodeType::F32_GREATER,         {"f32_greater",         0xffe07fff, 0x97000000}},
      {OpCodeType::F32_GREATER_EQUAL,   {"f32_greater_equal",   0xffe07fff, 0x9a000000}},
      {OpCodeType::F32_LESS,            {"f32_less",            0xffe07fff, 0x99000000}},
      {OpCodeType::F32_LESS_EQUAL,      {"f32_less_equal",      0xffe07fff, 0x9b000000}},
      {OpCodeType::F32_EQUAL,           {"f32_equal",           0xffe07fff, 0x98000000}},
      {OpCodeType::F32_SEL,             {"f32_sel",             0xff007fff, 0x9c000000}},
      {OpCodeType::F32_ADDI,            {"f32_addi",            0xff030000, 0xb0000000}},
      {OpCodeType::F32_SUBI,            {"f32_subi",            0xff030000, 0xb1000000}},
      {OpCodeType::F32_MULI,            {"f32_muli",            0xff030000, 0xb2000000}},
      {OpCodeType::F32_MAXI,            {"f32_maxi",            0xff030000, 0xb3000000}},
      {OpCodeType::F32_MINI,            {"f32_mini",            0xff030000, 0xb4000000}},
      {OpCodeType::F32_MOVI,            {"f32_movi",            0xff1f0000, 0xb5000000}},
      {OpCodeType::F32_GREATERI,        {"f32_greateri",        0xffe30000, 0xb7000000}},
      {OpCodeType::F32_GREATER_EQUALI,  {"f32_greater_equali",  0xffe30000, 0xba000000}},
      {OpCodeType::F32_LESSI,           {"f32_lessi",           0xffe30000, 0xb9000000}},
      {OpCodeType::F32_LESS_EQUALI,     {"f32_less_equali",     0xffe30000, 0xbb000000}},
      {OpCodeType::F32_EQUALI,          {"f32_equali",          0xffe30000, 0xb8000000}},
      {OpCodeType::F32_SELI,            {"f32_seli",            0xff030000, 0xbc000000}},
      {OpCodeType::F32_LUI,             {"f32_lui",             0xff1f0000, 0xbd000000}},
      {OpCodeType::F32_LI,              {"f32_li",              0xff1f0000, 0xbe000000}},
      {OpCodeType::I32_ADD,             {"i32_add",             0xff007fff, 0x80000000}},
      {OpCodeType::I32_SUB,             {"i32_sub",             0xff007fff, 0x81000000}},
      {OpCodeType::I32_MUL,             {"i32_mul",             0xff007fff, 0x82000000}},
      {OpCodeType::I32_MAX,             {"i32_max",             0xff007fff, 0x83000000}},
      {OpCodeType::I32_MIN,             {"i32_min",             0xff007fff, 0x84000000}},
      {OpCodeType::I32_GREATER,         {"i32_greater",         0xffe07fff, 0x87000000}},
      {OpCodeType::I32_GREATER_EQUAL,   {"i32_greater_equal",   0xffe07fff, 0x8a000000}},
      {OpCodeType::I32_LESS,            {"i32_less",            0xffe07fff, 0x89000000}},
      {OpCodeType::I32_LESS_EQUAL,      {"i32_less_equal",      0xffe07fff, 0x8b000000}},
      {OpCodeType::I32_EQUAL,           {"i32_equal",           0xffe07fff, 0x88000000}},
      {OpCodeType::I32_SEL,             {"i32_sel",             0xff007fff, 0x8c000000}},
      {OpCodeType::I32_ADDI,            {"i32_addi",            0xff030000, 0xa0000000}},
      {OpCodeType::I32_SUBI,            {"i32_subi",            0xff030000, 0xa1000000}},
      {OpCodeType::I32_MULI,            {"i32_muli",            0xff030000, 0xa2000000}},
      {OpCodeType::I32_MAXI,            {"i32_maxi",            0xff030000, 0xa3000000}},
      {OpCodeType::I32_MINI,            {"i32_mini",            0xff030000, 0xa4000000}},
      {OpCodeType::I32_MOVI,            {"i32_movi",            0xff1f0000, 0xa5000000}},
      {OpCodeType::I32_GREATERI,        {"i32_greateri",        0xffe30000, 0xa7000000}},
      {OpCodeType::I32_GREATER_EQUALI,  {"i32_greater_equali",  0xffe30000, 0xaa000000}},
      {OpCodeType::I32_LESSI,           {"i32_lessi",           0xffe30000, 0xa9000000}},
      {OpCodeType::I32_LESS_EQUALI,     {"i32_less_equali",     0xffe30000, 0xab000000}},
      {OpCodeType::I32_EQUALI,          {"i32_equali",          0xffe30000, 0xa8000000}},
      {OpCodeType::I32_SELI,            {"i32_seli",            0xff030000, 0xac000000}},
      {OpCodeType::I32_LUI,             {"i32_lui",             0xff1f0000, 0xad000000}},
      {OpCodeType::I32_LI,              {"i32_li",              0xff1f0000, 0xae000000}},
      {OpCodeType::I8_DLOAD_ADD,        {"i8_dload_add",        0xff3f0000, 0xc0030000}},
      {OpCodeType::I8_DLOAD_SUB,        {"i8_dload_sub",        0xff3f0000, 0xc0070000}},
      {OpCodeType::I8_DLOAD_MUL,        {"i8_dload_mul",        0xff3f0000, 0xc1030000}},
      {OpCodeType::I8_DLOAD_MAX,        {"i8_dload_max",        0xff3f0000, 0xc1070000}},
      {OpCodeType::I8_DLOAD_MIN,        {"i8_dload_min",        0xff3f0000, 0xc4030000}},
      {OpCodeType::I8_LOAD_ADD,         {"i8_load_add",         0xff0f0000, 0xc0010000}},
      {OpCodeType::I8_LOAD_SUB,         {"i8_load_sub",         0xff0f0000, 0xc0050000}},
      {OpCodeType::I8_LOAD_MUL,         {"i8_load_mul",         0xff0f0000, 0xc1010000}},
      {OpCodeType::I8_LOAD_MAX,         {"i8_load_max",         0xff0f0000, 0xc1050000}},
      {OpCodeType::I8_LOAD_MIN,         {"i8_load_min",         0xff0f0000, 0xc4010000}},
      {OpCodeType::U8_DLOAD_ADD,        {"u8_dload_add",        0xff3f0000, 0xd0030000}},
      {OpCodeType::U8_DLOAD_SUB,        {"u8_dload_sub",        0xff3f0000, 0xd0070000}},
      {OpCodeType::U8_DLOAD_MUL,        {"u8_dload_mul",        0xff3f0000, 0xd1030000}},
      {OpCodeType::U8_DLOAD_MAX,        {"u8_dload_max",        0xff3f0000, 0xd1070000}},
      {OpCodeType::U8_DLOAD_MIN,        {"u8_dload_min",        0xff3f0000, 0xd4030000}},
      {OpCodeType::U8_LOAD_ADD,         {"u8_load_add",         0xff0f0000, 0xd0010000}},
      {OpCodeType::U8_LOAD_SUB,         {"u8_load_sub",         0xff0f0000, 0xd0050000}},
      {OpCodeType::U8_LOAD_MUL,         {"u8_load_mul",         0xff0f0000, 0xd1010000}},
      {OpCodeType::U8_LOAD_MAX,         {"u8_load_max",         0xff0f0000, 0xd1050000}},
      {OpCodeType::U8_LOAD_MIN,         {"u8_load_min",         0xff0f0000, 0xd4010000}},
      {OpCodeType::E43_DLOAD_ADD,       {"e43_dload_add",       0xff3f0000, 0xf2030000}},
      {OpCodeType::E43_DLOAD_SUB,       {"e43_dload_sub",       0xff3f0000, 0xf2070000}},
      {OpCodeType::E43_DLOAD_MUL,       {"e43_dload_mul",       0xff3f0000, 0xf3030000}},
      {OpCodeType::E43_DLOAD_MAX,       {"e43_dload_max",       0xff3f0000, 0xf3070000}},
      {OpCodeType::E43_DLOAD_MIN,       {"e43_dload_min",       0xff3f0000, 0xf6030000}},
      {OpCodeType::E43_LOAD_ADD,        {"e43_load_add",        0xff0f0000, 0xf2010000}},
      {OpCodeType::E43_LOAD_SUB,        {"e43_load_sub",        0xff0f0000, 0xf2050000}},
      {OpCodeType::E43_LOAD_MUL,        {"e43_load_mul",        0xff0f0000, 0xf3010000}},
      {OpCodeType::E43_LOAD_MAX,        {"e43_load_max",        0xff0f0000, 0xf3050000}},
      {OpCodeType::E43_LOAD_MIN,        {"e43_load_min",        0xff0f0000, 0xf6010000}},
      {OpCodeType::E52_DLOAD_ADD,       {"e52_dload_add",       0xff3f0000, 0xe2030000}},
      {OpCodeType::E52_DLOAD_SUB,       {"e52_dload_sub",       0xff3f0000, 0xe2070000}},
      {OpCodeType::E52_DLOAD_MUL,       {"e52_dload_mul",       0xff3f0000, 0xe3030000}},
      {OpCodeType::E52_DLOAD_MAX,       {"e52_dload_max",       0xff3f0000, 0xe3070000}},
      {OpCodeType::E52_DLOAD_MIN,       {"e52_dload_min",       0xff3f0000, 0xe6030000}},
      {OpCodeType::E52_LOAD_ADD,        {"e52_load_add",        0xff0f0000, 0xe2010000}},
      {OpCodeType::E52_LOAD_SUB,        {"e52_load_sub",        0xff0f0000, 0xe2050000}},
      {OpCodeType::E52_LOAD_MUL,        {"e52_load_mul",        0xff0f0000, 0xe3010000}},
      {OpCodeType::E52_LOAD_MAX,        {"e52_load_max",        0xff0f0000, 0xe3050000}},
      {OpCodeType::E52_LOAD_MIN,        {"e52_load_min",        0xff0f0000, 0xe6010000}},
      {OpCodeType::I16_DLOAD_ADD,       {"i16_dload_add",       0xff3f0000, 0xc2030000}},
      {OpCodeType::I16_DLOAD_SUB,       {"i16_dload_sub",       0xff3f0000, 0xc2070000}},
      {OpCodeType::I16_DLOAD_MUL,       {"i16_dload_mul",       0xff3f0000, 0xc3030000}},
      {OpCodeType::I16_DLOAD_MAX,       {"i16_dload_max",       0xff3f0000, 0xc3070000}},
      {OpCodeType::I16_DLOAD_MIN,       {"i16_dload_min",       0xff3f0000, 0xc6030000}},
      {OpCodeType::I16_LOAD_ADD,        {"i16_load_add",        0xff0f0000, 0xc2010000}},
      {OpCodeType::I16_LOAD_SUB,        {"i16_load_sub",        0xff0f0000, 0xc2050000}},
      {OpCodeType::I16_LOAD_MUL,        {"i16_load_mul",        0xff0f0000, 0xc3010000}},
      {OpCodeType::I16_LOAD_MAX,        {"i16_load_max",        0xff0f0000, 0xc3050000}},
      {OpCodeType::I16_LOAD_MIN,        {"i16_load_min",        0xff0f0000, 0xc6010000}},
      {OpCodeType::F16_DLOAD_ADD,       {"f16_dload_add",       0xff3f0000, 0xe0030000}},
      {OpCodeType::F16_DLOAD_SUB,       {"f16_dload_sub",       0xff3f0000, 0xe0070000}},
      {OpCodeType::F16_DLOAD_MUL,       {"f16_dload_mul",       0xff3f0000, 0xe1030000}},
      {OpCodeType::F16_DLOAD_MAX,       {"f16_dload_max",       0xff3f0000, 0xe1070000}},
      {OpCodeType::F16_DLOAD_MIN,       {"f16_dload_min",       0xff3f0000, 0xe4030000}},
      {OpCodeType::F16_LOAD_ADD,        {"f16_load_add",        0xff0f0000, 0xe0010000}},
      {OpCodeType::F16_LOAD_SUB,        {"f16_load_sub",        0xff0f0000, 0xe0050000}},
      {OpCodeType::F16_LOAD_MUL,        {"f16_load_mul",        0xff0f0000, 0xe1010000}},
      {OpCodeType::F16_LOAD_MAX,        {"f16_load_max",        0xff0f0000, 0xe1050000}},
      {OpCodeType::F16_LOAD_MIN,        {"f16_load_min",        0xff0f0000, 0xe4010000}},
      {OpCodeType::F32_DLOAD_ADD,       {"f32_dload_add",       0xff3f0000, 0xf0030000}},
      {OpCodeType::F32_DLOAD_SUB,       {"f32_dload_sub",       0xff3f0000, 0xf0070000}},
      {OpCodeType::F32_DLOAD_MUL,       {"f32_dload_mul",       0xff3f0000, 0xf1030000}},
      {OpCodeType::F32_DLOAD_MAX,       {"f32_dload_max",       0xff3f0000, 0xf1070000}},
      {OpCodeType::F32_DLOAD_MIN,       {"f32_dload_min",       0xff3f0000, 0xf4030000}},
      {OpCodeType::F32_LOAD_ADD,        {"f32_load_add",        0xff0f0000, 0xf0010000}},
      {OpCodeType::F32_LOAD_SUB,        {"f32_load_sub",        0xff0f0000, 0xf0050000}},
      {OpCodeType::F32_LOAD_MUL,        {"f32_load_mul",        0xff0f0000, 0xf1010000}},
      {OpCodeType::F32_LOAD_MAX,        {"f32_load_max",        0xff0f0000, 0xf1050000}},
      {OpCodeType::F32_LOAD_MIN,        {"f32_load_min",        0xff0f0000, 0xf4010000}},
      {OpCodeType::I32_DLOAD_ADD,       {"i32_dload_add",       0xff3f0000, 0xd20b0000}},
      {OpCodeType::I32_DLOAD_SUB,       {"i32_dload_sub",       0xff3f0000, 0xd20f0000}},
      {OpCodeType::I32_DLOAD_MUL,       {"i32_dload_mul",       0xff3f0000, 0xd30b0000}},
      {OpCodeType::I32_DLOAD_MAX,       {"i32_dload_max",       0xff3f0000, 0xd30f0000}},
      {OpCodeType::I32_DLOAD_MIN,       {"i32_dload_min",       0xff3f0000, 0xd60b0000}},
      {OpCodeType::I32_LOAD_ADD,        {"i32_load_add",        0xff0f0000, 0xd2090000}},
      {OpCodeType::I32_LOAD_SUB,        {"i32_load_sub",        0xff0f0000, 0xd20d0000}},
      {OpCodeType::I32_LOAD_MUL,        {"i32_load_mul",        0xff0f0000, 0xd3090000}},
      {OpCodeType::I32_LOAD_MAX,        {"i32_load_max",        0xff0f0000, 0xd30d0000}},
      {OpCodeType::I32_LOAD_MIN,        {"i32_load_min",        0xff0f0000, 0xd6090000}},
      {OpCodeType::I8_ADD_STORE,        {"i8_add_store",        0xff0f0000, 0xc8010000}},
      {OpCodeType::I8_SUB_STORE,        {"i8_sub_store",        0xff0f0000, 0xc8050000}},
      {OpCodeType::I8_MUL_STORE,        {"i8_mul_store",        0xff0f0000, 0xc9010000}},
      {OpCodeType::I8_MAX_STORE,        {"i8_max_store",        0xff0f0000, 0xc9050000}},
      {OpCodeType::I8_MIN_STORE,        {"i8_min_store",        0xff0f0000, 0xcc010000}},
      {OpCodeType::U8_ADD_STORE,        {"u8_add_store",        0xff0f0000, 0xd8010000}},
      {OpCodeType::U8_SUB_STORE,        {"u8_sub_store",        0xff0f0000, 0xd8050000}},
      {OpCodeType::U8_MUL_STORE,        {"u8_mul_store",        0xff0f0000, 0xd9010000}},
      {OpCodeType::U8_MAX_STORE,        {"u8_max_store",        0xff0f0000, 0xd9050000}},
      {OpCodeType::U8_MIN_STORE,        {"u8_min_store",        0xff0f0000, 0xdc010000}},
      {OpCodeType::E43_ADD_STORE,       {"e43_add_store",       0xff0f0000, 0xfa010000}},
      {OpCodeType::E43_SUB_STORE,       {"e43_sub_store",       0xff0f0000, 0xfa050000}},
      {OpCodeType::E43_MUL_STORE,       {"e43_mul_store",       0xff0f0000, 0xfb010000}},
      {OpCodeType::E43_MAX_STORE,       {"e43_max_store",       0xff0f0000, 0xfb050000}},
      {OpCodeType::E43_MIN_STORE,       {"e43_min_store",       0xff0f0000, 0xfe010000}},
      {OpCodeType::E52_ADD_STORE,       {"e52_add_store",       0xff0f0000, 0xea010000}},
      {OpCodeType::E52_SUB_STORE,       {"e52_sub_store",       0xff0f0000, 0xea050000}},
      {OpCodeType::E52_MUL_STORE,       {"e52_mul_store",       0xff0f0000, 0xeb010000}},
      {OpCodeType::E52_MAX_STORE,       {"e52_max_store",       0xff0f0000, 0xeb050000}},
      {OpCodeType::E52_MIN_STORE,       {"e52_min_store",       0xff0f0000, 0xee010000}},
      {OpCodeType::I16_ADD_STORE,       {"i16_add_store",       0xff0f0000, 0xca010000}},
      {OpCodeType::I16_SUB_STORE,       {"i16_sub_store",       0xff0f0000, 0xca050000}},
      {OpCodeType::I16_MUL_STORE,       {"i16_mul_store",       0xff0f0000, 0xcb010000}},
      {OpCodeType::I16_MAX_STORE,       {"i16_max_store",       0xff0f0000, 0xcb050000}},
      {OpCodeType::I16_MIN_STORE,       {"i16_min_store",       0xff0f0000, 0xce010000}},
      {OpCodeType::F16_ADD_STORE,       {"f16_add_store",       0xff0f0000, 0xe8010000}},
      {OpCodeType::F16_SUB_STORE,       {"f16_sub_store",       0xff0f0000, 0xe8050000}},
      {OpCodeType::F16_MUL_STORE,       {"f16_mul_store",       0xff0f0000, 0xe9010000}},
      {OpCodeType::F16_MAX_STORE,       {"f16_max_store",       0xff0f0000, 0xe9050000}},
      {OpCodeType::F16_MIN_STORE,       {"f16_min_store",       0xff0f0000, 0xec010000}},
      {OpCodeType::F32_ADD_STORE,       {"f32_add_store",       0xff0f0000, 0xf8010000}},
      {OpCodeType::F32_SUB_STORE,       {"f32_sub_store",       0xff0f0000, 0xf8050000}},
      {OpCodeType::F32_MUL_STORE,       {"f32_mul_store",       0xff0f0000, 0xf9010000}},
      {OpCodeType::F32_MAX_STORE,       {"f32_max_store",       0xff0f0000, 0xf9050000}},
      {OpCodeType::F32_MIN_STORE,       {"f32_min_store",       0xff0f0000, 0xfc010000}},
      {OpCodeType::I32_ADD_STORE,       {"i32_add_store",       0xff0f0000, 0xda090000}},
      {OpCodeType::I32_SUB_STORE,       {"i32_sub_store",       0xff0f0000, 0xda0d0000}},
      {OpCodeType::I32_MUL_STORE,       {"i32_mul_store",       0xff0f0000, 0xdb090000}},
      {OpCodeType::I32_MAX_STORE,       {"i32_max_store",       0xff0f0000, 0xdb0d0000}},
      {OpCodeType::I32_MIN_STORE,       {"i32_min_store",       0xff0f0000, 0xde090000}},
      {OpCodeType::I8_DLOAD_ADD_STORE,  {"i8_dload_add_store",  0xffff0000, 0xc00a0000}},
      {OpCodeType::I8_DLOAD_SUB_STORE,  {"i8_dload_sub_store",  0xffff0000, 0xc00e0000}},
      {OpCodeType::I8_DLOAD_MUL_STORE,  {"i8_dload_mul_store",  0xffff0000, 0xc10a0000}},
      {OpCodeType::I8_DLOAD_MAX_STORE,  {"i8_dload_max_store",  0xffff0000, 0xc10e0000}},
      {OpCodeType::I8_DLOAD_MIN_STORE,  {"i8_dload_min_store",  0xffff0000, 0xc40a0000}},
      {OpCodeType::I8_LOAD_ADD_STORE,   {"i8_load_add_store",   0xffcf0000, 0xc0080000}},
      {OpCodeType::I8_LOAD_SUB_STORE,   {"i8_load_sub_store",   0xffcf0000, 0xc00c0000}},
      {OpCodeType::I8_LOAD_MUL_STORE,   {"i8_load_mul_store",   0xffcf0000, 0xc1080000}},
      {OpCodeType::I8_LOAD_MAX_STORE,   {"i8_load_max_store",   0xffcf0000, 0xc10c0000}},
      {OpCodeType::I8_LOAD_MIN_STORE,   {"i8_load_min_store",   0xffcf0000, 0xc4080000}},
      {OpCodeType::U8_DLOAD_ADD_STORE,  {"u8_dload_add_store",  0xffff0000, 0xd00a0000}},
      {OpCodeType::U8_DLOAD_SUB_STORE,  {"u8_dload_sub_store",  0xffff0000, 0xd00e0000}},
      {OpCodeType::U8_DLOAD_MUL_STORE,  {"u8_dload_mul_store",  0xffff0000, 0xd10a0000}},
      {OpCodeType::U8_DLOAD_MAX_STORE,  {"u8_dload_max_store",  0xffff0000, 0xd10e0000}},
      {OpCodeType::U8_DLOAD_MIN_STORE,  {"u8_dload_min_store",  0xffff0000, 0xd40a0000}},
      {OpCodeType::U8_LOAD_ADD_STORE,   {"u8_load_add_store",   0xffcf0000, 0xd0080000}},
      {OpCodeType::U8_LOAD_SUB_STORE,   {"u8_load_sub_store",   0xffcf0000, 0xd00c0000}},
      {OpCodeType::U8_LOAD_MUL_STORE,   {"u8_load_mul_store",   0xffcf0000, 0xd1080000}},
      {OpCodeType::U8_LOAD_MAX_STORE,   {"u8_load_max_store",   0xffcf0000, 0xd10c0000}},
      {OpCodeType::U8_LOAD_MIN_STORE,   {"u8_load_min_store",   0xffcf0000, 0xd4080000}},
      {OpCodeType::E43_DLOAD_ADD_STORE, {"e43_dload_add_store", 0xffff0000, 0xf20a0000}},
      {OpCodeType::E43_DLOAD_SUB_STORE, {"e43_dload_sub_store", 0xffff0000, 0xf20e0000}},
      {OpCodeType::E43_DLOAD_MUL_STORE, {"e43_dload_mul_store", 0xffff0000, 0xf30a0000}},
      {OpCodeType::E43_DLOAD_MAX_STORE, {"e43_dload_max_store", 0xffff0000, 0xf30e0000}},
      {OpCodeType::E43_DLOAD_MIN_STORE, {"e43_dload_min_store", 0xffff0000, 0xf60a0000}},
      {OpCodeType::E43_LOAD_ADD_STORE,  {"e43_load_add_store",  0xffcf0000, 0xf2080000}},
      {OpCodeType::E43_LOAD_SUB_STORE,  {"e43_load_sub_store",  0xffcf0000, 0xf20c0000}},
      {OpCodeType::E43_LOAD_MUL_STORE,  {"e43_load_mul_store",  0xffcf0000, 0xf3080000}},
      {OpCodeType::E43_LOAD_MAX_STORE,  {"e43_load_max_store",  0xffcf0000, 0xf30c0000}},
      {OpCodeType::E43_LOAD_MIN_STORE,  {"e43_load_min_store",  0xffcf0000, 0xf6080000}},
      {OpCodeType::E52_DLOAD_ADD_STORE, {"e52_dload_add_store", 0xffff0000, 0xe20a0000}},
      {OpCodeType::E52_DLOAD_SUB_STORE, {"e52_dload_sub_store", 0xffff0000, 0xe20e0000}},
      {OpCodeType::E52_DLOAD_MUL_STORE, {"e52_dload_mul_store", 0xffff0000, 0xe30a0000}},
      {OpCodeType::E52_DLOAD_MAX_STORE, {"e52_dload_max_store", 0xffff0000, 0xe30e0000}},
      {OpCodeType::E52_DLOAD_MIN_STORE, {"e52_dload_min_store", 0xffff0000, 0xe60a0000}},
      {OpCodeType::E52_LOAD_ADD_STORE,  {"e52_load_add_store",  0xffcf0000, 0xe2080000}},
      {OpCodeType::E52_LOAD_SUB_STORE,  {"e52_load_sub_store",  0xffcf0000, 0xe20c0000}},
      {OpCodeType::E52_LOAD_MUL_STORE,  {"e52_load_mul_store",  0xffcf0000, 0xe3080000}},
      {OpCodeType::E52_LOAD_MAX_STORE,  {"e52_load_max_store",  0xffcf0000, 0xe30c0000}},
      {OpCodeType::E52_LOAD_MIN_STORE,  {"e52_load_min_store",  0xffcf0000, 0xe6080000}},
      {OpCodeType::I16_DLOAD_ADD_STORE, {"i16_dload_add_store", 0xffff0000, 0xc20a0000}},
      {OpCodeType::I16_DLOAD_SUB_STORE, {"i16_dload_sub_store", 0xffff0000, 0xc20e0000}},
      {OpCodeType::I16_DLOAD_MUL_STORE, {"i16_dload_mul_store", 0xffff0000, 0xc30a0000}},
      {OpCodeType::I16_DLOAD_MAX_STORE, {"i16_dload_max_store", 0xffff0000, 0xc30e0000}},
      {OpCodeType::I16_DLOAD_MIN_STORE, {"i16_dload_min_store", 0xffff0000, 0xc60a0000}},
      {OpCodeType::I16_LOAD_ADD_STORE,  {"i16_load_add_store",  0xffcf0000, 0xc2080000}},
      {OpCodeType::I16_LOAD_SUB_STORE,  {"i16_load_sub_store",  0xffcf0000, 0xc20c0000}},
      {OpCodeType::I16_LOAD_MUL_STORE,  {"i16_load_mul_store",  0xffcf0000, 0xc3080000}},
      {OpCodeType::I16_LOAD_MAX_STORE,  {"i16_load_max_store",  0xffcf0000, 0xc30c0000}},
      {OpCodeType::I16_LOAD_MIN_STORE,  {"i16_load_min_store",  0xffcf0000, 0xc6080000}},
      {OpCodeType::F16_DLOAD_ADD_STORE, {"f16_dload_add_store", 0xffff0000, 0xe00a0000}},
      {OpCodeType::F16_DLOAD_SUB_STORE, {"f16_dload_sub_store", 0xffff0000, 0xe00e0000}},
      {OpCodeType::F16_DLOAD_MUL_STORE, {"f16_dload_mul_store", 0xffff0000, 0xe10a0000}},
      {OpCodeType::F16_DLOAD_MAX_STORE, {"f16_dload_max_store", 0xffff0000, 0xe10e0000}},
      {OpCodeType::F16_DLOAD_MIN_STORE, {"f16_dload_min_store", 0xffff0000, 0xe40a0000}},
      {OpCodeType::F16_LOAD_ADD_STORE,  {"f16_load_add_store",  0xffcf0000, 0xe0080000}},
      {OpCodeType::F16_LOAD_SUB_STORE,  {"f16_load_sub_store",  0xffcf0000, 0xe00c0000}},
      {OpCodeType::F16_LOAD_MUL_STORE,  {"f16_load_mul_store",  0xffcf0000, 0xe1080000}},
      {OpCodeType::F16_LOAD_MAX_STORE,  {"f16_load_max_store",  0xffcf0000, 0xe10c0000}},
      {OpCodeType::F16_LOAD_MIN_STORE,  {"f16_load_min_store",  0xffcf0000, 0xe4080000}},
      {OpCodeType::F32_DLOAD_ADD_STORE, {"f32_dload_add_store", 0xffff0000, 0xf00a0000}},
      {OpCodeType::F32_DLOAD_SUB_STORE, {"f32_dload_sub_store", 0xffff0000, 0xf00e0000}},
      {OpCodeType::F32_DLOAD_MUL_STORE, {"f32_dload_mul_store", 0xffff0000, 0xf10a0000}},
      {OpCodeType::F32_DLOAD_MAX_STORE, {"f32_dload_max_store", 0xffff0000, 0xf10e0000}},
      {OpCodeType::F32_DLOAD_MIN_STORE, {"f32_dload_min_store", 0xffff0000, 0xf40a0000}},
      {OpCodeType::F32_LOAD_ADD_STORE,  {"f32_load_add_store",  0xffcf0000, 0xf0080000}},
      {OpCodeType::F32_LOAD_SUB_STORE,  {"f32_load_sub_store",  0xffcf0000, 0xf00c0000}},
      {OpCodeType::F32_LOAD_MUL_STORE,  {"f32_load_mul_store",  0xffcf0000, 0xf1080000}},
      {OpCodeType::F32_LOAD_MAX_STORE,  {"f32_load_max_store",  0xffcf0000, 0xf10c0000}},
      {OpCodeType::F32_LOAD_MIN_STORE,  {"f32_load_min_store",  0xffcf0000, 0xf4080000}},
      {OpCodeType::I32_DLOAD_ADD_STORE, {"i32_dload_add_store", 0xffff0000, 0xda0a0000}},
      {OpCodeType::I32_DLOAD_SUB_STORE, {"i32_dload_sub_store", 0xffff0000, 0xda0e0000}},
      {OpCodeType::I32_DLOAD_MUL_STORE, {"i32_dload_mul_store", 0xffff0000, 0xdb0a0000}},
      {OpCodeType::I32_DLOAD_MAX_STORE, {"i32_dload_max_store", 0xffff0000, 0xdb0e0000}},
      {OpCodeType::I32_DLOAD_MIN_STORE, {"i32_dload_min_store", 0xffff0000, 0xde0a0000}},
      {OpCodeType::I32_LOAD_ADD_STORE,  {"i32_load_add_store",  0xffcf0000, 0xda080000}},
      {OpCodeType::I32_LOAD_SUB_STORE,  {"i32_load_sub_store",  0xffcf0000, 0xda0c0000}},
      {OpCodeType::I32_LOAD_MUL_STORE,  {"i32_load_mul_store",  0xffcf0000, 0xdb080000}},
      {OpCodeType::I32_LOAD_MAX_STORE,  {"i32_load_max_store",  0xffcf0000, 0xdb0c0000}},
      {OpCodeType::I32_LOAD_MIN_STORE,  {"i32_load_min_store",  0xffcf0000, 0xde080000}},
      {OpCodeType::S_ADDI,              {"s_addi",              0xff000000, 0x40000000}},
      {OpCodeType::S_ADDIU,             {"s_addiu",             0xff000000, 0x41000000}},
      {OpCodeType::S_SUBI,              {"s_subi",              0xff000000, 0x42000000}},
      {OpCodeType::S_ANDI,              {"s_andi",              0xff000000, 0x43000000}},
      {OpCodeType::S_ORI,               {"s_ori",               0xff000000, 0x44000000}},
      {OpCodeType::S_XORI,              {"s_xori",              0xff000000, 0x45000000}},
      {OpCodeType::S_SGTI,              {"s_sgti",              0xff000000, 0x46000000}},
      {OpCodeType::S_SLTI,              {"s_slti",              0xff000000, 0x47000000}},
      {OpCodeType::S_SEQI,              {"s_seqi",              0xff000000, 0x48000000}},
      {OpCodeType::S_SGEI,              {"s_sgei",              0xff000000, 0x49000000}},
      {OpCodeType::S_SLEI,              {"s_slei",              0xff000000, 0x4a000000}},
      {OpCodeType::S_SRAI,              {"s_srai",              0xff000000, 0x4b000000}},
      {OpCodeType::S_SRLI,              {"s_srli",              0xff000000, 0x4c000000}},
      {OpCodeType::S_SLLI,              {"s_slli",              0xff000000, 0x4d000000}},
      {OpCodeType::S_LUI,               {"s_lui",               0xfff00000, 0x4e000000}},
      {OpCodeType::S_ADD,               {"s_add",               0xff000fff, 0x51000000}},
      {OpCodeType::S_ADDU,              {"s_addu",              0xff000fff, 0x52000000}},
      {OpCodeType::S_SUB,               {"s_sub",               0xff000fff, 0x53000000}},
      {OpCodeType::S_MUL,               {"s_mul",               0xff000fff, 0x54000000}},
      {OpCodeType::S_MULH,              {"s_mulh",              0xff000fff, 0x55000000}},
      {OpCodeType::S_MULHU,             {"s_mulhu",             0xff000fff, 0x56000000}},
      {OpCodeType::S_MIN,               {"s_min",               0xff000fff, 0x57000000}},
      {OpCodeType::S_MAX,               {"s_max",               0xff000fff, 0x58000000}},
      {OpCodeType::S_MINU,              {"s_minu",              0xff000fff, 0x59000000}},
      {OpCodeType::S_MAXU,              {"s_maxu",              0xff000fff, 0x5a000000}},
      {OpCodeType::S_AND,               {"s_and",               0xff000fff, 0x5b000000}},
      {OpCodeType::S_OR,                {"s_or",                0xff000fff, 0x5c000000}},
      {OpCodeType::S_XOR,               {"s_xor",               0xff000fff, 0x5d000000}},
      {OpCodeType::S_SGT,               {"s_sgt",               0xff000fff, 0x5e000000}},
      {OpCodeType::S_SLT,               {"s_slt",               0xff000fff, 0x5f000000}},
      {OpCodeType::S_SEQ,               {"s_seq",               0xff000fff, 0x60000000}},
      {OpCodeType::S_SGE,               {"s_sge",               0xff000fff, 0x61000000}},
      {OpCodeType::S_SLE,               {"s_sle",               0xff000fff, 0x62000000}},
      {OpCodeType::S_CMPSEL,            {"s_cmpsel",            0xff000fff, 0x63000000}},
      {OpCodeType::S_SRA,               {"s_sra",               0xff000fff, 0x64000000}},
      {OpCodeType::S_SRL,               {"s_srl",               0xff000fff, 0x65000000}},
      {OpCodeType::S_SLL,               {"s_sll",               0xff000fff, 0x66000000}},
      {OpCodeType::S_JUMP,              {"s_jump",              0xffff0000, 0x69000000}},
      {OpCodeType::S_JAL,               {"s_jal",               0xffff0000, 0x6a000000}},
      {OpCodeType::S_JR,                {"s_jr",                0xff0fffff, 0x6b000000}},
      {OpCodeType::S_BNE,               {"s_bne",               0xff000000, 0x6c000000}},
      {OpCodeType::S_BEQ,               {"s_beq",               0xff000000, 0x6d000000}},
      {OpCodeType::S_BLT,               {"s_blt",               0xff000000, 0x6e000000}},
      {OpCodeType::S_BLTU,              {"s_bltu",              0xff000000, 0x6f000000}},
      {OpCodeType::S_BGE,               {"s_bge",               0xff000000, 0x70000000}},
      {OpCodeType::S_BGEU,              {"s_bgeu",              0xff000000, 0x71000000}},
      {OpCodeType::S_SETCFG,            {"s_setcfg",            0xff00001f, 0x74000000}},
      {OpCodeType::S_SETRF,             {"s_setrf",             0xfffffff8, 0x75000000}},
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

class SetLoadBaseInstr : public VpuInstr {
public:
  explicit SetLoadBaseInstr() : VpuInstr(OpCodeType::SET_LOAD_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class SetLoad2BaseInstr : public VpuInstr {
public:
  explicit SetLoad2BaseInstr() : VpuInstr(OpCodeType::SET_LOAD2_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class SetStoreBaseInstr : public VpuInstr {
public:
  explicit SetStoreBaseInstr() : VpuInstr(OpCodeType::SET_STORE_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
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
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 13);
  VPU_GEN_GETTER_SETTER(BankMask, 12, 9);
  VPU_GEN_GETTER_SETTER(ClusterBroadcast, 8, 6);
  VPU_GEN_GETTER_SETTER(BankBroadcast, 5, 3);
  VPU_GEN_GETTER_SETTER(ChannelBroadcast, 2, 1);
};

class SetStoreGlbInstr : public VpuInstr {
public:
  explicit SetStoreGlbInstr() : VpuInstr(OpCodeType::SET_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Stride);
    VPU_PUSH_GETTER_SETTER(ClusterMask);
    VPU_PUSH_GETTER_SETTER(BankMask);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 5);
  VPU_GEN_GETTER_SETTER(BankMask, 4, 1);
};

class SetQuantizeInstr : public VpuInstr {
public:
  explicit SetQuantizeInstr() : VpuInstr(OpCodeType::SET_QUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Quantize);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Quantize, 15, 0);
};

class SetDequantizeInstr : public VpuInstr {
public:
  explicit SetDequantizeInstr() : VpuInstr(OpCodeType::SET_DEQUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 23, 0);
};

class SetDequantize2Instr : public VpuInstr {
public:
  explicit SetDequantize2Instr() : VpuInstr(OpCodeType::SET_DEQUANTIZE2) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 23, 0);
};

class SetFsInstr : public VpuInstr {
public:
  explicit SetFsInstr() : VpuInstr(OpCodeType::SET_FS) {
    VPU_PUSH_GETTER_SETTER(FsSel);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(FsSel, 7, 0);
};

class SetCmaskHighInstr : public VpuInstr {
public:
  explicit SetCmaskHighInstr() : VpuInstr(OpCodeType::SET_CMASK_HIGH) {
    VPU_PUSH_GETTER_SETTER(ClusterMask);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(ClusterMask, 23, 16);
};

class NopInstr : public VpuInstr {
public:
  explicit NopInstr() : VpuInstr(OpCodeType::NOP) {
    nAlu = 1;
  }
};

class OpEndInstr : public VpuInstr {
public:
  explicit OpEndInstr() : VpuInstr(OpCodeType::OP_END) {
    nAlu = 1;
  }
};

class I8DloadInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit I8DloadInstr() : VpuInstr(OpCodeType::I8_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nAlu = 1;
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
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8DloadInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit U8DloadInstr() : VpuInstr(OpCodeType::U8_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E43DloadInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit E43DloadInstr() : VpuInstr(OpCodeType::E43_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class E43LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit E43LoadInstr() : VpuInstr(OpCodeType::E43_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E52DloadInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit E52DloadInstr() : VpuInstr(OpCodeType::E52_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class E52LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit E52LoadInstr() : VpuInstr(OpCodeType::E52_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I16DloadInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit I16DloadInstr() : VpuInstr(OpCodeType::I16_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class I16LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit I16LoadInstr() : VpuInstr(OpCodeType::I16_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16DloadInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit F16DloadInstr() : VpuInstr(OpCodeType::F16_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32DloadInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit F32DloadInstr() : VpuInstr(OpCodeType::F32_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32DloadInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit I32DloadInstr() : VpuInstr(OpCodeType::I32_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 3;
    nAlu = 2;
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
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit I8StoreInstr() : VpuInstr(OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit U8StoreInstr() : VpuInstr(OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit E43StoreInstr() : VpuInstr(OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit E52StoreInstr() : VpuInstr(OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit I16StoreInstr() : VpuInstr(OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit F16StoreInstr() : VpuInstr(OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit F32StoreInstr() : VpuInstr(OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32I32storeInstr : public VpuInstr, public StoreInstr {
public:
  explicit I32I32storeInstr() : VpuInstr(OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32GreaterEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32GreaterEqualInstr() : VpuInstr(OpCodeType::F32_GREATER_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32LessInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LessInstr() : VpuInstr(OpCodeType::F32_LESS) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32LessEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LessEqualInstr() : VpuInstr(OpCodeType::F32_LESS_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F32EqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32EqualInstr() : VpuInstr(OpCodeType::F32_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32AddiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32AddiInstr() : VpuInstr(OpCodeType::F32_ADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32SubiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32SubiInstr() : VpuInstr(OpCodeType::F32_SUBI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32MuliInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32MuliInstr() : VpuInstr(OpCodeType::F32_MULI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32MaxiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32MaxiInstr() : VpuInstr(OpCodeType::F32_MAXI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32MiniInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32MiniInstr() : VpuInstr(OpCodeType::F32_MINI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32MoviInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32MoviInstr() : VpuInstr(OpCodeType::F32_MOVI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32GreateriInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32GreateriInstr() : VpuInstr(OpCodeType::F32_GREATERI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32GreaterEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32GreaterEqualiInstr() : VpuInstr(OpCodeType::F32_GREATER_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32LessiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LessiInstr() : VpuInstr(OpCodeType::F32_LESSI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32LessEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LessEqualiInstr() : VpuInstr(OpCodeType::F32_LESS_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32EqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32EqualiInstr() : VpuInstr(OpCodeType::F32_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32SeliInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32SeliInstr() : VpuInstr(OpCodeType::F32_SELI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
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
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32LiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LiInstr() : VpuInstr(OpCodeType::F32_LI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32AddInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32AddInstr() : VpuInstr(OpCodeType::I32_ADD) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32SubInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32SubInstr() : VpuInstr(OpCodeType::I32_SUB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32MulInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32MulInstr() : VpuInstr(OpCodeType::I32_MUL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32MaxInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32MaxInstr() : VpuInstr(OpCodeType::I32_MAX) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32MinInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32MinInstr() : VpuInstr(OpCodeType::I32_MIN) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32GreaterInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32GreaterInstr() : VpuInstr(OpCodeType::I32_GREATER) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class I32GreaterEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32GreaterEqualInstr() : VpuInstr(OpCodeType::I32_GREATER_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class I32LessInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32LessInstr() : VpuInstr(OpCodeType::I32_LESS) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class I32LessEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32LessEqualInstr() : VpuInstr(OpCodeType::I32_LESS_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class I32EqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32EqualInstr() : VpuInstr(OpCodeType::I32_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class I32SelInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32SelInstr() : VpuInstr(OpCodeType::I32_SEL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32AddiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32AddiInstr() : VpuInstr(OpCodeType::I32_ADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32SubiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32SubiInstr() : VpuInstr(OpCodeType::I32_SUBI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32MuliInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32MuliInstr() : VpuInstr(OpCodeType::I32_MULI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32MaxiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32MaxiInstr() : VpuInstr(OpCodeType::I32_MAXI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32MiniInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32MiniInstr() : VpuInstr(OpCodeType::I32_MINI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32MoviInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32MoviInstr() : VpuInstr(OpCodeType::I32_MOVI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32GreateriInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32GreateriInstr() : VpuInstr(OpCodeType::I32_GREATERI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I32GreaterEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32GreaterEqualiInstr() : VpuInstr(OpCodeType::I32_GREATER_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I32LessiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32LessiInstr() : VpuInstr(OpCodeType::I32_LESSI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I32LessEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32LessEqualiInstr() : VpuInstr(OpCodeType::I32_LESS_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I32EqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32EqualiInstr() : VpuInstr(OpCodeType::I32_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class I32SeliInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32SeliInstr() : VpuInstr(OpCodeType::I32_SELI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32LuiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32LuiInstr() : VpuInstr(OpCodeType::I32_LUI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I32LiInstr : public VpuInstr, public VectorInstr {
public:
  explicit I32LiInstr() : VpuInstr(OpCodeType::I32_LI) {
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    nHasImm = 1;
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class I8DloadAddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I8DloadAddInstr() : VpuInstr(OpCodeType::I8_DLOAD_ADD), DualLoadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadSubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I8DloadSubInstr() : VpuInstr(OpCodeType::I8_DLOAD_SUB), DualLoadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I8DloadMulInstr() : VpuInstr(OpCodeType::I8_DLOAD_MUL), DualLoadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I8DloadMaxInstr() : VpuInstr(OpCodeType::I8_DLOAD_MAX), DualLoadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMinInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I8DloadMinInstr() : VpuInstr(OpCodeType::I8_DLOAD_MIN), DualLoadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nAlu = 1;
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
    nAlu = 1;
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
    nAlu = 1;
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
    nAlu = 1;
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
    nAlu = 1;
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
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadAddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit U8DloadAddInstr() : VpuInstr(OpCodeType::U8_DLOAD_ADD), DualLoadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadSubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit U8DloadSubInstr() : VpuInstr(OpCodeType::U8_DLOAD_SUB), DualLoadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit U8DloadMulInstr() : VpuInstr(OpCodeType::U8_DLOAD_MUL), DualLoadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit U8DloadMaxInstr() : VpuInstr(OpCodeType::U8_DLOAD_MAX), DualLoadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMinInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit U8DloadMinInstr() : VpuInstr(OpCodeType::U8_DLOAD_MIN), DualLoadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43DloadAddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E43DloadAddInstr() : VpuInstr(OpCodeType::E43_DLOAD_ADD), DualLoadComputeInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43DloadSubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E43DloadSubInstr() : VpuInstr(OpCodeType::E43_DLOAD_SUB), DualLoadComputeInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43DloadMulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E43DloadMulInstr() : VpuInstr(OpCodeType::E43_DLOAD_MUL), DualLoadComputeInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43DloadMaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E43DloadMaxInstr() : VpuInstr(OpCodeType::E43_DLOAD_MAX), DualLoadComputeInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43DloadMinInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E43DloadMinInstr() : VpuInstr(OpCodeType::E43_DLOAD_MIN), DualLoadComputeInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E43LoadAddInstr() : VpuInstr(OpCodeType::E43_LOAD_ADD), LoadComputeInstr(OpCodeType::E43_LOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E43LoadSubInstr() : VpuInstr(OpCodeType::E43_LOAD_SUB), LoadComputeInstr(OpCodeType::E43_LOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E43LoadMulInstr() : VpuInstr(OpCodeType::E43_LOAD_MUL), LoadComputeInstr(OpCodeType::E43_LOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E43LoadMaxInstr() : VpuInstr(OpCodeType::E43_LOAD_MAX), LoadComputeInstr(OpCodeType::E43_LOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E43LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E43LoadMinInstr() : VpuInstr(OpCodeType::E43_LOAD_MIN), LoadComputeInstr(OpCodeType::E43_LOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52DloadAddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E52DloadAddInstr() : VpuInstr(OpCodeType::E52_DLOAD_ADD), DualLoadComputeInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52DloadSubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E52DloadSubInstr() : VpuInstr(OpCodeType::E52_DLOAD_SUB), DualLoadComputeInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52DloadMulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E52DloadMulInstr() : VpuInstr(OpCodeType::E52_DLOAD_MUL), DualLoadComputeInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52DloadMaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E52DloadMaxInstr() : VpuInstr(OpCodeType::E52_DLOAD_MAX), DualLoadComputeInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52DloadMinInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit E52DloadMinInstr() : VpuInstr(OpCodeType::E52_DLOAD_MIN), DualLoadComputeInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E52LoadAddInstr() : VpuInstr(OpCodeType::E52_LOAD_ADD), LoadComputeInstr(OpCodeType::E52_LOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E52LoadSubInstr() : VpuInstr(OpCodeType::E52_LOAD_SUB), LoadComputeInstr(OpCodeType::E52_LOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E52LoadMulInstr() : VpuInstr(OpCodeType::E52_LOAD_MUL), LoadComputeInstr(OpCodeType::E52_LOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E52LoadMaxInstr() : VpuInstr(OpCodeType::E52_LOAD_MAX), LoadComputeInstr(OpCodeType::E52_LOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class E52LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit E52LoadMinInstr() : VpuInstr(OpCodeType::E52_LOAD_MIN), LoadComputeInstr(OpCodeType::E52_LOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16DloadAddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I16DloadAddInstr() : VpuInstr(OpCodeType::I16_DLOAD_ADD), DualLoadComputeInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16DloadSubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I16DloadSubInstr() : VpuInstr(OpCodeType::I16_DLOAD_SUB), DualLoadComputeInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16DloadMulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I16DloadMulInstr() : VpuInstr(OpCodeType::I16_DLOAD_MUL), DualLoadComputeInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16DloadMaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I16DloadMaxInstr() : VpuInstr(OpCodeType::I16_DLOAD_MAX), DualLoadComputeInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16DloadMinInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I16DloadMinInstr() : VpuInstr(OpCodeType::I16_DLOAD_MIN), DualLoadComputeInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I16LoadAddInstr() : VpuInstr(OpCodeType::I16_LOAD_ADD), LoadComputeInstr(OpCodeType::I16_LOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I16LoadSubInstr() : VpuInstr(OpCodeType::I16_LOAD_SUB), LoadComputeInstr(OpCodeType::I16_LOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I16LoadMulInstr() : VpuInstr(OpCodeType::I16_LOAD_MUL), LoadComputeInstr(OpCodeType::I16_LOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I16LoadMaxInstr() : VpuInstr(OpCodeType::I16_LOAD_MAX), LoadComputeInstr(OpCodeType::I16_LOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I16LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I16LoadMinInstr() : VpuInstr(OpCodeType::I16_LOAD_MIN), LoadComputeInstr(OpCodeType::I16_LOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadAddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F16DloadAddInstr() : VpuInstr(OpCodeType::F16_DLOAD_ADD), DualLoadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadSubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F16DloadSubInstr() : VpuInstr(OpCodeType::F16_DLOAD_SUB), DualLoadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F16DloadMulInstr() : VpuInstr(OpCodeType::F16_DLOAD_MUL), DualLoadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F16DloadMaxInstr() : VpuInstr(OpCodeType::F16_DLOAD_MAX), DualLoadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMinInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F16DloadMinInstr() : VpuInstr(OpCodeType::F16_DLOAD_MIN), DualLoadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadAddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F32DloadAddInstr() : VpuInstr(OpCodeType::F32_DLOAD_ADD), DualLoadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadSubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F32DloadSubInstr() : VpuInstr(OpCodeType::F32_DLOAD_SUB), DualLoadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadMulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F32DloadMulInstr() : VpuInstr(OpCodeType::F32_DLOAD_MUL), DualLoadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadMaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F32DloadMaxInstr() : VpuInstr(OpCodeType::F32_DLOAD_MAX), DualLoadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F32DloadMinInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit F32DloadMinInstr() : VpuInstr(OpCodeType::F32_DLOAD_MIN), DualLoadComputeInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadAddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I32DloadAddInstr() : VpuInstr(OpCodeType::I32_DLOAD_ADD), DualLoadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadSubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I32DloadSubInstr() : VpuInstr(OpCodeType::I32_DLOAD_SUB), DualLoadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadMulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I32DloadMulInstr() : VpuInstr(OpCodeType::I32_DLOAD_MUL), DualLoadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadMaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I32DloadMaxInstr() : VpuInstr(OpCodeType::I32_DLOAD_MAX), DualLoadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadMinInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit I32DloadMinInstr() : VpuInstr(OpCodeType::I32_DLOAD_MIN), DualLoadComputeInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadAddInstr() : VpuInstr(OpCodeType::I32_LOAD_ADD), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadSubInstr() : VpuInstr(OpCodeType::I32_LOAD_SUB), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadMulInstr() : VpuInstr(OpCodeType::I32_LOAD_MUL), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadMaxInstr() : VpuInstr(OpCodeType::I32_LOAD_MAX), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadMinInstr() : VpuInstr(OpCodeType::I32_LOAD_MIN), LoadComputeInstr(OpCodeType::I32_LOAD, OpCodeType::I32_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nDtype = 3;
    nAlu = 2;
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
    nAlu = 1;
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
    nAlu = 1;
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
    nAlu = 1;
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
    nAlu = 1;
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
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
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
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E43AddStoreInstr() : VpuInstr(OpCodeType::E43_ADD_STORE), ComputeStoreInstr(OpCodeType::F32_ADD, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E43SubStoreInstr() : VpuInstr(OpCodeType::E43_SUB_STORE), ComputeStoreInstr(OpCodeType::F32_SUB, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E43MulStoreInstr() : VpuInstr(OpCodeType::E43_MUL_STORE), ComputeStoreInstr(OpCodeType::F32_MUL, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E43MaxStoreInstr() : VpuInstr(OpCodeType::E43_MAX_STORE), ComputeStoreInstr(OpCodeType::F32_MAX, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E43MinStoreInstr() : VpuInstr(OpCodeType::E43_MIN_STORE), ComputeStoreInstr(OpCodeType::F32_MIN, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E52AddStoreInstr() : VpuInstr(OpCodeType::E52_ADD_STORE), ComputeStoreInstr(OpCodeType::F32_ADD, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E52SubStoreInstr() : VpuInstr(OpCodeType::E52_SUB_STORE), ComputeStoreInstr(OpCodeType::F32_SUB, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E52MulStoreInstr() : VpuInstr(OpCodeType::E52_MUL_STORE), ComputeStoreInstr(OpCodeType::F32_MUL, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E52MaxStoreInstr() : VpuInstr(OpCodeType::E52_MAX_STORE), ComputeStoreInstr(OpCodeType::F32_MAX, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit E52MinStoreInstr() : VpuInstr(OpCodeType::E52_MIN_STORE), ComputeStoreInstr(OpCodeType::F32_MIN, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I16AddStoreInstr() : VpuInstr(OpCodeType::I16_ADD_STORE), ComputeStoreInstr(OpCodeType::F32_ADD, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I16SubStoreInstr() : VpuInstr(OpCodeType::I16_SUB_STORE), ComputeStoreInstr(OpCodeType::F32_SUB, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I16MulStoreInstr() : VpuInstr(OpCodeType::I16_MUL_STORE), ComputeStoreInstr(OpCodeType::F32_MUL, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I16MaxStoreInstr() : VpuInstr(OpCodeType::I16_MAX_STORE), ComputeStoreInstr(OpCodeType::F32_MAX, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I16MinStoreInstr() : VpuInstr(OpCodeType::I16_MIN_STORE), ComputeStoreInstr(OpCodeType::F32_MIN, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 1;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
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
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32AddStoreInstr() : VpuInstr(OpCodeType::I32_ADD_STORE), ComputeStoreInstr(OpCodeType::I32_ADD, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32SubStoreInstr() : VpuInstr(OpCodeType::I32_SUB_STORE), ComputeStoreInstr(OpCodeType::I32_SUB, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32MulStoreInstr() : VpuInstr(OpCodeType::I32_MUL_STORE), ComputeStoreInstr(OpCodeType::I32_MUL, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32MaxStoreInstr() : VpuInstr(OpCodeType::I32_MAX_STORE), ComputeStoreInstr(OpCodeType::I32_MAX, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32MinStoreInstr() : VpuInstr(OpCodeType::I32_MIN_STORE), ComputeStoreInstr(OpCodeType::I32_MIN, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadAddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I8DloadAddStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_ADD_STORE), DualLoadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_ADD, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadSubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I8DloadSubStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_SUB_STORE), DualLoadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_SUB, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I8DloadMulStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MUL_STORE), DualLoadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MUL, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I8DloadMaxStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MAX_STORE), DualLoadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MAX, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMinStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I8DloadMinStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MIN_STORE), DualLoadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F32_MIN, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadAddStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_ADD, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadSubStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_SUB, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMulStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MUL, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMaxStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MAX, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMinStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F32_MIN, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8DloadAddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit U8DloadAddStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_ADD_STORE), DualLoadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_ADD, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadSubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit U8DloadSubStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_SUB_STORE), DualLoadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_SUB, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit U8DloadMulStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MUL_STORE), DualLoadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MUL, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit U8DloadMaxStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MAX_STORE), DualLoadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MAX, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMinStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit U8DloadMinStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MIN_STORE), DualLoadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F32_MIN, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadAddStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_ADD, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadSubStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_SUB, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMulStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MUL, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMaxStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MAX, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMinStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F32_MIN, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 5;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E43DloadAddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E43DloadAddStoreInstr() : VpuInstr(OpCodeType::E43_DLOAD_ADD_STORE), DualLoadComputeStoreInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_ADD, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43DloadSubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E43DloadSubStoreInstr() : VpuInstr(OpCodeType::E43_DLOAD_SUB_STORE), DualLoadComputeStoreInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_SUB, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43DloadMulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E43DloadMulStoreInstr() : VpuInstr(OpCodeType::E43_DLOAD_MUL_STORE), DualLoadComputeStoreInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_MUL, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43DloadMaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E43DloadMaxStoreInstr() : VpuInstr(OpCodeType::E43_DLOAD_MAX_STORE), DualLoadComputeStoreInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_MAX, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43DloadMinStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E43DloadMinStoreInstr() : VpuInstr(OpCodeType::E43_DLOAD_MIN_STORE), DualLoadComputeStoreInstr(OpCodeType::E43_DLOAD, OpCodeType::F32_MIN, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E43LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E43LoadAddStoreInstr() : VpuInstr(OpCodeType::E43_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::E43_LOAD, OpCodeType::F32_ADD, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E43LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E43LoadSubStoreInstr() : VpuInstr(OpCodeType::E43_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::E43_LOAD, OpCodeType::F32_SUB, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E43LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E43LoadMulStoreInstr() : VpuInstr(OpCodeType::E43_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::E43_LOAD, OpCodeType::F32_MUL, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E43LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E43LoadMaxStoreInstr() : VpuInstr(OpCodeType::E43_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::E43_LOAD, OpCodeType::F32_MAX, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E43LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E43LoadMinStoreInstr() : VpuInstr(OpCodeType::E43_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::E43_LOAD, OpCodeType::F32_MIN, OpCodeType::E43_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 18;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E52DloadAddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E52DloadAddStoreInstr() : VpuInstr(OpCodeType::E52_DLOAD_ADD_STORE), DualLoadComputeStoreInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_ADD, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52DloadSubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E52DloadSubStoreInstr() : VpuInstr(OpCodeType::E52_DLOAD_SUB_STORE), DualLoadComputeStoreInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_SUB, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52DloadMulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E52DloadMulStoreInstr() : VpuInstr(OpCodeType::E52_DLOAD_MUL_STORE), DualLoadComputeStoreInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_MUL, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52DloadMaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E52DloadMaxStoreInstr() : VpuInstr(OpCodeType::E52_DLOAD_MAX_STORE), DualLoadComputeStoreInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_MAX, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52DloadMinStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit E52DloadMinStoreInstr() : VpuInstr(OpCodeType::E52_DLOAD_MIN_STORE), DualLoadComputeStoreInstr(OpCodeType::E52_DLOAD, OpCodeType::F32_MIN, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class E52LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E52LoadAddStoreInstr() : VpuInstr(OpCodeType::E52_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::E52_LOAD, OpCodeType::F32_ADD, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E52LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E52LoadSubStoreInstr() : VpuInstr(OpCodeType::E52_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::E52_LOAD, OpCodeType::F32_SUB, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E52LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E52LoadMulStoreInstr() : VpuInstr(OpCodeType::E52_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::E52_LOAD, OpCodeType::F32_MUL, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E52LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E52LoadMaxStoreInstr() : VpuInstr(OpCodeType::E52_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::E52_LOAD, OpCodeType::F32_MAX, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class E52LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit E52LoadMinStoreInstr() : VpuInstr(OpCodeType::E52_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::E52_LOAD, OpCodeType::F32_MIN, OpCodeType::E52_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 17;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I16DloadAddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I16DloadAddStoreInstr() : VpuInstr(OpCodeType::I16_DLOAD_ADD_STORE), DualLoadComputeStoreInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_ADD, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16DloadSubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I16DloadSubStoreInstr() : VpuInstr(OpCodeType::I16_DLOAD_SUB_STORE), DualLoadComputeStoreInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_SUB, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16DloadMulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I16DloadMulStoreInstr() : VpuInstr(OpCodeType::I16_DLOAD_MUL_STORE), DualLoadComputeStoreInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_MUL, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16DloadMaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I16DloadMaxStoreInstr() : VpuInstr(OpCodeType::I16_DLOAD_MAX_STORE), DualLoadComputeStoreInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_MAX, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16DloadMinStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I16DloadMinStoreInstr() : VpuInstr(OpCodeType::I16_DLOAD_MIN_STORE), DualLoadComputeStoreInstr(OpCodeType::I16_DLOAD, OpCodeType::F32_MIN, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I16LoadAddStoreInstr() : VpuInstr(OpCodeType::I16_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::I16_LOAD, OpCodeType::F32_ADD, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I16LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I16LoadSubStoreInstr() : VpuInstr(OpCodeType::I16_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::I16_LOAD, OpCodeType::F32_SUB, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I16LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I16LoadMulStoreInstr() : VpuInstr(OpCodeType::I16_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::I16_LOAD, OpCodeType::F32_MUL, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I16LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I16LoadMaxStoreInstr() : VpuInstr(OpCodeType::I16_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::I16_LOAD, OpCodeType::F32_MAX, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I16LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I16LoadMinStoreInstr() : VpuInstr(OpCodeType::I16_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::I16_LOAD, OpCodeType::F32_MIN, OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 2;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16DloadAddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F16DloadAddStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_ADD_STORE), DualLoadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_ADD, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadSubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F16DloadSubStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_SUB_STORE), DualLoadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_SUB, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F16DloadMulStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MUL_STORE), DualLoadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MUL, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F16DloadMaxStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MAX_STORE), DualLoadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MAX, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMinStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F16DloadMinStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MIN_STORE), DualLoadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F32_MIN, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadAddStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_ADD, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadSubStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_SUB, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMulStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MUL, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMaxStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MAX, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMinStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F32_MIN, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 1;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32DloadAddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F32DloadAddStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_ADD_STORE), DualLoadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_ADD, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32DloadSubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F32DloadSubStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_SUB_STORE), DualLoadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_SUB, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32DloadMulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F32DloadMulStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_MUL_STORE), DualLoadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MUL, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32DloadMaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F32DloadMaxStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_MAX_STORE), DualLoadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MAX, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32DloadMinStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit F32DloadMinStoreInstr() : VpuInstr(OpCodeType::F32_DLOAD_MIN_STORE), DualLoadComputeStoreInstr(OpCodeType::F32_DLOAD, OpCodeType::F32_MIN, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F32LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadAddStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_ADD, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadSubStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_SUB, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadMulStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MUL, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadMaxStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MAX, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F32LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F32LoadMinStoreInstr() : VpuInstr(OpCodeType::F32_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::F32_LOAD, OpCodeType::F32_MIN, OpCodeType::F32_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 12;
    nAlu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32DloadAddStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I32DloadAddStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_ADD_STORE), DualLoadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_ADD, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadSubStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I32DloadSubStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_SUB_STORE), DualLoadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_SUB, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadMulStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I32DloadMulStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_MUL_STORE), DualLoadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_MUL, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadMaxStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I32DloadMaxStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_MAX_STORE), DualLoadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_MAX, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadMinStoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit I32DloadMinStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_MIN_STORE), DualLoadComputeStoreInstr(OpCodeType::I32_DLOAD, OpCodeType::I32_MIN, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadAddStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_ADD, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadSubStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_SUB, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadMulStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_MUL, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadMaxStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_MAX, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
    nAlu = 2;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadMinStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::I32_LOAD, OpCodeType::I32_MIN, OpCodeType::I32_I32STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nDtype = 3;
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
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SJalInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SJalInstr() : VpuInstr(OpCodeType::S_JAL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nDtype = 3;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class SJrInstr : public VpuInstr, public ScalarInstr {
public:
  explicit SJrInstr() : VpuInstr(OpCodeType::S_JR) {
    VPU_PUSH_GETTER_SETTER(Rs);
    nDtype = 3;
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
  }
  VPU_GEN_GETTER_SETTER(RfSel, 2, 0);
};


std::shared_ptr<VpuInstr> VpuInstr::create(OpCodeType opCode, const std::vector<size_t> &args) {
  std::shared_ptr<VpuInstr> ret = nullptr;
  switch (opCode) {
    case OpCodeType::SET_LOAD_BASE:       ret = std::shared_ptr<SetLoadBaseInstr>(new SetLoadBaseInstr); break;
    case OpCodeType::SET_LOAD2_BASE:      ret = std::shared_ptr<SetLoad2BaseInstr>(new SetLoad2BaseInstr); break;
    case OpCodeType::SET_STORE_BASE:      ret = std::shared_ptr<SetStoreBaseInstr>(new SetStoreBaseInstr); break;
    case OpCodeType::SET_LOAD_GLB:        ret = std::shared_ptr<SetLoadGlbInstr>(new SetLoadGlbInstr); break;
    case OpCodeType::SET_STORE_GLB:       ret = std::shared_ptr<SetStoreGlbInstr>(new SetStoreGlbInstr); break;
    case OpCodeType::SET_QUANTIZE:        ret = std::shared_ptr<SetQuantizeInstr>(new SetQuantizeInstr); break;
    case OpCodeType::SET_DEQUANTIZE:      ret = std::shared_ptr<SetDequantizeInstr>(new SetDequantizeInstr); break;
    case OpCodeType::SET_DEQUANTIZE2:     ret = std::shared_ptr<SetDequantize2Instr>(new SetDequantize2Instr); break;
    case OpCodeType::SET_FS:              ret = std::shared_ptr<SetFsInstr>(new SetFsInstr); break;
    case OpCodeType::SET_CMASK_HIGH:      ret = std::shared_ptr<SetCmaskHighInstr>(new SetCmaskHighInstr); break;
    case OpCodeType::NOP:                 ret = std::shared_ptr<NopInstr>(new NopInstr); break;
    case OpCodeType::OP_END:              ret = std::shared_ptr<OpEndInstr>(new OpEndInstr); break;
    case OpCodeType::I8_DLOAD:            ret = std::shared_ptr<I8DloadInstr>(new I8DloadInstr); break;
    case OpCodeType::I8_LOAD:             ret = std::shared_ptr<I8LoadInstr>(new I8LoadInstr); break;
    case OpCodeType::U8_DLOAD:            ret = std::shared_ptr<U8DloadInstr>(new U8DloadInstr); break;
    case OpCodeType::U8_LOAD:             ret = std::shared_ptr<U8LoadInstr>(new U8LoadInstr); break;
    case OpCodeType::E43_DLOAD:           ret = std::shared_ptr<E43DloadInstr>(new E43DloadInstr); break;
    case OpCodeType::E43_LOAD:            ret = std::shared_ptr<E43LoadInstr>(new E43LoadInstr); break;
    case OpCodeType::E52_DLOAD:           ret = std::shared_ptr<E52DloadInstr>(new E52DloadInstr); break;
    case OpCodeType::E52_LOAD:            ret = std::shared_ptr<E52LoadInstr>(new E52LoadInstr); break;
    case OpCodeType::I16_DLOAD:           ret = std::shared_ptr<I16DloadInstr>(new I16DloadInstr); break;
    case OpCodeType::I16_LOAD:            ret = std::shared_ptr<I16LoadInstr>(new I16LoadInstr); break;
    case OpCodeType::F16_DLOAD:           ret = std::shared_ptr<F16DloadInstr>(new F16DloadInstr); break;
    case OpCodeType::F16_LOAD:            ret = std::shared_ptr<F16LoadInstr>(new F16LoadInstr); break;
    case OpCodeType::F32_DLOAD:           ret = std::shared_ptr<F32DloadInstr>(new F32DloadInstr); break;
    case OpCodeType::F32_LOAD:            ret = std::shared_ptr<F32LoadInstr>(new F32LoadInstr); break;
    case OpCodeType::I32_DLOAD:           ret = std::shared_ptr<I32DloadInstr>(new I32DloadInstr); break;
    case OpCodeType::I32_LOAD:            ret = std::shared_ptr<I32LoadInstr>(new I32LoadInstr); break;
    case OpCodeType::I8_STORE:            ret = std::shared_ptr<I8StoreInstr>(new I8StoreInstr); break;
    case OpCodeType::U8_STORE:            ret = std::shared_ptr<U8StoreInstr>(new U8StoreInstr); break;
    case OpCodeType::E43_STORE:           ret = std::shared_ptr<E43StoreInstr>(new E43StoreInstr); break;
    case OpCodeType::E52_STORE:           ret = std::shared_ptr<E52StoreInstr>(new E52StoreInstr); break;
    case OpCodeType::I16_STORE:           ret = std::shared_ptr<I16StoreInstr>(new I16StoreInstr); break;
    case OpCodeType::F16_STORE:           ret = std::shared_ptr<F16StoreInstr>(new F16StoreInstr); break;
    case OpCodeType::F32_STORE:           ret = std::shared_ptr<F32StoreInstr>(new F32StoreInstr); break;
    case OpCodeType::I32_I32STORE:        ret = std::shared_ptr<I32I32storeInstr>(new I32I32storeInstr); break;
    case OpCodeType::F32_ADD:             ret = std::shared_ptr<F32AddInstr>(new F32AddInstr); break;
    case OpCodeType::F32_SUB:             ret = std::shared_ptr<F32SubInstr>(new F32SubInstr); break;
    case OpCodeType::F32_MUL:             ret = std::shared_ptr<F32MulInstr>(new F32MulInstr); break;
    case OpCodeType::F32_MAX:             ret = std::shared_ptr<F32MaxInstr>(new F32MaxInstr); break;
    case OpCodeType::F32_MIN:             ret = std::shared_ptr<F32MinInstr>(new F32MinInstr); break;
    case OpCodeType::F32_GREATER:         ret = std::shared_ptr<F32GreaterInstr>(new F32GreaterInstr); break;
    case OpCodeType::F32_GREATER_EQUAL:   ret = std::shared_ptr<F32GreaterEqualInstr>(new F32GreaterEqualInstr); break;
    case OpCodeType::F32_LESS:            ret = std::shared_ptr<F32LessInstr>(new F32LessInstr); break;
    case OpCodeType::F32_LESS_EQUAL:      ret = std::shared_ptr<F32LessEqualInstr>(new F32LessEqualInstr); break;
    case OpCodeType::F32_EQUAL:           ret = std::shared_ptr<F32EqualInstr>(new F32EqualInstr); break;
    case OpCodeType::F32_SEL:             ret = std::shared_ptr<F32SelInstr>(new F32SelInstr); break;
    case OpCodeType::F32_ADDI:            ret = std::shared_ptr<F32AddiInstr>(new F32AddiInstr); break;
    case OpCodeType::F32_SUBI:            ret = std::shared_ptr<F32SubiInstr>(new F32SubiInstr); break;
    case OpCodeType::F32_MULI:            ret = std::shared_ptr<F32MuliInstr>(new F32MuliInstr); break;
    case OpCodeType::F32_MAXI:            ret = std::shared_ptr<F32MaxiInstr>(new F32MaxiInstr); break;
    case OpCodeType::F32_MINI:            ret = std::shared_ptr<F32MiniInstr>(new F32MiniInstr); break;
    case OpCodeType::F32_MOVI:            ret = std::shared_ptr<F32MoviInstr>(new F32MoviInstr); break;
    case OpCodeType::F32_GREATERI:        ret = std::shared_ptr<F32GreateriInstr>(new F32GreateriInstr); break;
    case OpCodeType::F32_GREATER_EQUALI:  ret = std::shared_ptr<F32GreaterEqualiInstr>(new F32GreaterEqualiInstr); break;
    case OpCodeType::F32_LESSI:           ret = std::shared_ptr<F32LessiInstr>(new F32LessiInstr); break;
    case OpCodeType::F32_LESS_EQUALI:     ret = std::shared_ptr<F32LessEqualiInstr>(new F32LessEqualiInstr); break;
    case OpCodeType::F32_EQUALI:          ret = std::shared_ptr<F32EqualiInstr>(new F32EqualiInstr); break;
    case OpCodeType::F32_SELI:            ret = std::shared_ptr<F32SeliInstr>(new F32SeliInstr); break;
    case OpCodeType::F32_LUI:             ret = std::shared_ptr<F32LuiInstr>(new F32LuiInstr); break;
    case OpCodeType::F32_LI:              ret = std::shared_ptr<F32LiInstr>(new F32LiInstr); break;
    case OpCodeType::I32_ADD:             ret = std::shared_ptr<I32AddInstr>(new I32AddInstr); break;
    case OpCodeType::I32_SUB:             ret = std::shared_ptr<I32SubInstr>(new I32SubInstr); break;
    case OpCodeType::I32_MUL:             ret = std::shared_ptr<I32MulInstr>(new I32MulInstr); break;
    case OpCodeType::I32_MAX:             ret = std::shared_ptr<I32MaxInstr>(new I32MaxInstr); break;
    case OpCodeType::I32_MIN:             ret = std::shared_ptr<I32MinInstr>(new I32MinInstr); break;
    case OpCodeType::I32_GREATER:         ret = std::shared_ptr<I32GreaterInstr>(new I32GreaterInstr); break;
    case OpCodeType::I32_GREATER_EQUAL:   ret = std::shared_ptr<I32GreaterEqualInstr>(new I32GreaterEqualInstr); break;
    case OpCodeType::I32_LESS:            ret = std::shared_ptr<I32LessInstr>(new I32LessInstr); break;
    case OpCodeType::I32_LESS_EQUAL:      ret = std::shared_ptr<I32LessEqualInstr>(new I32LessEqualInstr); break;
    case OpCodeType::I32_EQUAL:           ret = std::shared_ptr<I32EqualInstr>(new I32EqualInstr); break;
    case OpCodeType::I32_SEL:             ret = std::shared_ptr<I32SelInstr>(new I32SelInstr); break;
    case OpCodeType::I32_ADDI:            ret = std::shared_ptr<I32AddiInstr>(new I32AddiInstr); break;
    case OpCodeType::I32_SUBI:            ret = std::shared_ptr<I32SubiInstr>(new I32SubiInstr); break;
    case OpCodeType::I32_MULI:            ret = std::shared_ptr<I32MuliInstr>(new I32MuliInstr); break;
    case OpCodeType::I32_MAXI:            ret = std::shared_ptr<I32MaxiInstr>(new I32MaxiInstr); break;
    case OpCodeType::I32_MINI:            ret = std::shared_ptr<I32MiniInstr>(new I32MiniInstr); break;
    case OpCodeType::I32_MOVI:            ret = std::shared_ptr<I32MoviInstr>(new I32MoviInstr); break;
    case OpCodeType::I32_GREATERI:        ret = std::shared_ptr<I32GreateriInstr>(new I32GreateriInstr); break;
    case OpCodeType::I32_GREATER_EQUALI:  ret = std::shared_ptr<I32GreaterEqualiInstr>(new I32GreaterEqualiInstr); break;
    case OpCodeType::I32_LESSI:           ret = std::shared_ptr<I32LessiInstr>(new I32LessiInstr); break;
    case OpCodeType::I32_LESS_EQUALI:     ret = std::shared_ptr<I32LessEqualiInstr>(new I32LessEqualiInstr); break;
    case OpCodeType::I32_EQUALI:          ret = std::shared_ptr<I32EqualiInstr>(new I32EqualiInstr); break;
    case OpCodeType::I32_SELI:            ret = std::shared_ptr<I32SeliInstr>(new I32SeliInstr); break;
    case OpCodeType::I32_LUI:             ret = std::shared_ptr<I32LuiInstr>(new I32LuiInstr); break;
    case OpCodeType::I32_LI:              ret = std::shared_ptr<I32LiInstr>(new I32LiInstr); break;
    case OpCodeType::I8_DLOAD_ADD:        ret = std::shared_ptr<I8DloadAddInstr>(new I8DloadAddInstr); break;
    case OpCodeType::I8_DLOAD_SUB:        ret = std::shared_ptr<I8DloadSubInstr>(new I8DloadSubInstr); break;
    case OpCodeType::I8_DLOAD_MUL:        ret = std::shared_ptr<I8DloadMulInstr>(new I8DloadMulInstr); break;
    case OpCodeType::I8_DLOAD_MAX:        ret = std::shared_ptr<I8DloadMaxInstr>(new I8DloadMaxInstr); break;
    case OpCodeType::I8_DLOAD_MIN:        ret = std::shared_ptr<I8DloadMinInstr>(new I8DloadMinInstr); break;
    case OpCodeType::I8_LOAD_ADD:         ret = std::shared_ptr<I8LoadAddInstr>(new I8LoadAddInstr); break;
    case OpCodeType::I8_LOAD_SUB:         ret = std::shared_ptr<I8LoadSubInstr>(new I8LoadSubInstr); break;
    case OpCodeType::I8_LOAD_MUL:         ret = std::shared_ptr<I8LoadMulInstr>(new I8LoadMulInstr); break;
    case OpCodeType::I8_LOAD_MAX:         ret = std::shared_ptr<I8LoadMaxInstr>(new I8LoadMaxInstr); break;
    case OpCodeType::I8_LOAD_MIN:         ret = std::shared_ptr<I8LoadMinInstr>(new I8LoadMinInstr); break;
    case OpCodeType::U8_DLOAD_ADD:        ret = std::shared_ptr<U8DloadAddInstr>(new U8DloadAddInstr); break;
    case OpCodeType::U8_DLOAD_SUB:        ret = std::shared_ptr<U8DloadSubInstr>(new U8DloadSubInstr); break;
    case OpCodeType::U8_DLOAD_MUL:        ret = std::shared_ptr<U8DloadMulInstr>(new U8DloadMulInstr); break;
    case OpCodeType::U8_DLOAD_MAX:        ret = std::shared_ptr<U8DloadMaxInstr>(new U8DloadMaxInstr); break;
    case OpCodeType::U8_DLOAD_MIN:        ret = std::shared_ptr<U8DloadMinInstr>(new U8DloadMinInstr); break;
    case OpCodeType::U8_LOAD_ADD:         ret = std::shared_ptr<U8LoadAddInstr>(new U8LoadAddInstr); break;
    case OpCodeType::U8_LOAD_SUB:         ret = std::shared_ptr<U8LoadSubInstr>(new U8LoadSubInstr); break;
    case OpCodeType::U8_LOAD_MUL:         ret = std::shared_ptr<U8LoadMulInstr>(new U8LoadMulInstr); break;
    case OpCodeType::U8_LOAD_MAX:         ret = std::shared_ptr<U8LoadMaxInstr>(new U8LoadMaxInstr); break;
    case OpCodeType::U8_LOAD_MIN:         ret = std::shared_ptr<U8LoadMinInstr>(new U8LoadMinInstr); break;
    case OpCodeType::E43_DLOAD_ADD:       ret = std::shared_ptr<E43DloadAddInstr>(new E43DloadAddInstr); break;
    case OpCodeType::E43_DLOAD_SUB:       ret = std::shared_ptr<E43DloadSubInstr>(new E43DloadSubInstr); break;
    case OpCodeType::E43_DLOAD_MUL:       ret = std::shared_ptr<E43DloadMulInstr>(new E43DloadMulInstr); break;
    case OpCodeType::E43_DLOAD_MAX:       ret = std::shared_ptr<E43DloadMaxInstr>(new E43DloadMaxInstr); break;
    case OpCodeType::E43_DLOAD_MIN:       ret = std::shared_ptr<E43DloadMinInstr>(new E43DloadMinInstr); break;
    case OpCodeType::E43_LOAD_ADD:        ret = std::shared_ptr<E43LoadAddInstr>(new E43LoadAddInstr); break;
    case OpCodeType::E43_LOAD_SUB:        ret = std::shared_ptr<E43LoadSubInstr>(new E43LoadSubInstr); break;
    case OpCodeType::E43_LOAD_MUL:        ret = std::shared_ptr<E43LoadMulInstr>(new E43LoadMulInstr); break;
    case OpCodeType::E43_LOAD_MAX:        ret = std::shared_ptr<E43LoadMaxInstr>(new E43LoadMaxInstr); break;
    case OpCodeType::E43_LOAD_MIN:        ret = std::shared_ptr<E43LoadMinInstr>(new E43LoadMinInstr); break;
    case OpCodeType::E52_DLOAD_ADD:       ret = std::shared_ptr<E52DloadAddInstr>(new E52DloadAddInstr); break;
    case OpCodeType::E52_DLOAD_SUB:       ret = std::shared_ptr<E52DloadSubInstr>(new E52DloadSubInstr); break;
    case OpCodeType::E52_DLOAD_MUL:       ret = std::shared_ptr<E52DloadMulInstr>(new E52DloadMulInstr); break;
    case OpCodeType::E52_DLOAD_MAX:       ret = std::shared_ptr<E52DloadMaxInstr>(new E52DloadMaxInstr); break;
    case OpCodeType::E52_DLOAD_MIN:       ret = std::shared_ptr<E52DloadMinInstr>(new E52DloadMinInstr); break;
    case OpCodeType::E52_LOAD_ADD:        ret = std::shared_ptr<E52LoadAddInstr>(new E52LoadAddInstr); break;
    case OpCodeType::E52_LOAD_SUB:        ret = std::shared_ptr<E52LoadSubInstr>(new E52LoadSubInstr); break;
    case OpCodeType::E52_LOAD_MUL:        ret = std::shared_ptr<E52LoadMulInstr>(new E52LoadMulInstr); break;
    case OpCodeType::E52_LOAD_MAX:        ret = std::shared_ptr<E52LoadMaxInstr>(new E52LoadMaxInstr); break;
    case OpCodeType::E52_LOAD_MIN:        ret = std::shared_ptr<E52LoadMinInstr>(new E52LoadMinInstr); break;
    case OpCodeType::I16_DLOAD_ADD:       ret = std::shared_ptr<I16DloadAddInstr>(new I16DloadAddInstr); break;
    case OpCodeType::I16_DLOAD_SUB:       ret = std::shared_ptr<I16DloadSubInstr>(new I16DloadSubInstr); break;
    case OpCodeType::I16_DLOAD_MUL:       ret = std::shared_ptr<I16DloadMulInstr>(new I16DloadMulInstr); break;
    case OpCodeType::I16_DLOAD_MAX:       ret = std::shared_ptr<I16DloadMaxInstr>(new I16DloadMaxInstr); break;
    case OpCodeType::I16_DLOAD_MIN:       ret = std::shared_ptr<I16DloadMinInstr>(new I16DloadMinInstr); break;
    case OpCodeType::I16_LOAD_ADD:        ret = std::shared_ptr<I16LoadAddInstr>(new I16LoadAddInstr); break;
    case OpCodeType::I16_LOAD_SUB:        ret = std::shared_ptr<I16LoadSubInstr>(new I16LoadSubInstr); break;
    case OpCodeType::I16_LOAD_MUL:        ret = std::shared_ptr<I16LoadMulInstr>(new I16LoadMulInstr); break;
    case OpCodeType::I16_LOAD_MAX:        ret = std::shared_ptr<I16LoadMaxInstr>(new I16LoadMaxInstr); break;
    case OpCodeType::I16_LOAD_MIN:        ret = std::shared_ptr<I16LoadMinInstr>(new I16LoadMinInstr); break;
    case OpCodeType::F16_DLOAD_ADD:       ret = std::shared_ptr<F16DloadAddInstr>(new F16DloadAddInstr); break;
    case OpCodeType::F16_DLOAD_SUB:       ret = std::shared_ptr<F16DloadSubInstr>(new F16DloadSubInstr); break;
    case OpCodeType::F16_DLOAD_MUL:       ret = std::shared_ptr<F16DloadMulInstr>(new F16DloadMulInstr); break;
    case OpCodeType::F16_DLOAD_MAX:       ret = std::shared_ptr<F16DloadMaxInstr>(new F16DloadMaxInstr); break;
    case OpCodeType::F16_DLOAD_MIN:       ret = std::shared_ptr<F16DloadMinInstr>(new F16DloadMinInstr); break;
    case OpCodeType::F16_LOAD_ADD:        ret = std::shared_ptr<F16LoadAddInstr>(new F16LoadAddInstr); break;
    case OpCodeType::F16_LOAD_SUB:        ret = std::shared_ptr<F16LoadSubInstr>(new F16LoadSubInstr); break;
    case OpCodeType::F16_LOAD_MUL:        ret = std::shared_ptr<F16LoadMulInstr>(new F16LoadMulInstr); break;
    case OpCodeType::F16_LOAD_MAX:        ret = std::shared_ptr<F16LoadMaxInstr>(new F16LoadMaxInstr); break;
    case OpCodeType::F16_LOAD_MIN:        ret = std::shared_ptr<F16LoadMinInstr>(new F16LoadMinInstr); break;
    case OpCodeType::F32_DLOAD_ADD:       ret = std::shared_ptr<F32DloadAddInstr>(new F32DloadAddInstr); break;
    case OpCodeType::F32_DLOAD_SUB:       ret = std::shared_ptr<F32DloadSubInstr>(new F32DloadSubInstr); break;
    case OpCodeType::F32_DLOAD_MUL:       ret = std::shared_ptr<F32DloadMulInstr>(new F32DloadMulInstr); break;
    case OpCodeType::F32_DLOAD_MAX:       ret = std::shared_ptr<F32DloadMaxInstr>(new F32DloadMaxInstr); break;
    case OpCodeType::F32_DLOAD_MIN:       ret = std::shared_ptr<F32DloadMinInstr>(new F32DloadMinInstr); break;
    case OpCodeType::F32_LOAD_ADD:        ret = std::shared_ptr<F32LoadAddInstr>(new F32LoadAddInstr); break;
    case OpCodeType::F32_LOAD_SUB:        ret = std::shared_ptr<F32LoadSubInstr>(new F32LoadSubInstr); break;
    case OpCodeType::F32_LOAD_MUL:        ret = std::shared_ptr<F32LoadMulInstr>(new F32LoadMulInstr); break;
    case OpCodeType::F32_LOAD_MAX:        ret = std::shared_ptr<F32LoadMaxInstr>(new F32LoadMaxInstr); break;
    case OpCodeType::F32_LOAD_MIN:        ret = std::shared_ptr<F32LoadMinInstr>(new F32LoadMinInstr); break;
    case OpCodeType::I32_DLOAD_ADD:       ret = std::shared_ptr<I32DloadAddInstr>(new I32DloadAddInstr); break;
    case OpCodeType::I32_DLOAD_SUB:       ret = std::shared_ptr<I32DloadSubInstr>(new I32DloadSubInstr); break;
    case OpCodeType::I32_DLOAD_MUL:       ret = std::shared_ptr<I32DloadMulInstr>(new I32DloadMulInstr); break;
    case OpCodeType::I32_DLOAD_MAX:       ret = std::shared_ptr<I32DloadMaxInstr>(new I32DloadMaxInstr); break;
    case OpCodeType::I32_DLOAD_MIN:       ret = std::shared_ptr<I32DloadMinInstr>(new I32DloadMinInstr); break;
    case OpCodeType::I32_LOAD_ADD:        ret = std::shared_ptr<I32LoadAddInstr>(new I32LoadAddInstr); break;
    case OpCodeType::I32_LOAD_SUB:        ret = std::shared_ptr<I32LoadSubInstr>(new I32LoadSubInstr); break;
    case OpCodeType::I32_LOAD_MUL:        ret = std::shared_ptr<I32LoadMulInstr>(new I32LoadMulInstr); break;
    case OpCodeType::I32_LOAD_MAX:        ret = std::shared_ptr<I32LoadMaxInstr>(new I32LoadMaxInstr); break;
    case OpCodeType::I32_LOAD_MIN:        ret = std::shared_ptr<I32LoadMinInstr>(new I32LoadMinInstr); break;
    case OpCodeType::I8_ADD_STORE:        ret = std::shared_ptr<I8AddStoreInstr>(new I8AddStoreInstr); break;
    case OpCodeType::I8_SUB_STORE:        ret = std::shared_ptr<I8SubStoreInstr>(new I8SubStoreInstr); break;
    case OpCodeType::I8_MUL_STORE:        ret = std::shared_ptr<I8MulStoreInstr>(new I8MulStoreInstr); break;
    case OpCodeType::I8_MAX_STORE:        ret = std::shared_ptr<I8MaxStoreInstr>(new I8MaxStoreInstr); break;
    case OpCodeType::I8_MIN_STORE:        ret = std::shared_ptr<I8MinStoreInstr>(new I8MinStoreInstr); break;
    case OpCodeType::U8_ADD_STORE:        ret = std::shared_ptr<U8AddStoreInstr>(new U8AddStoreInstr); break;
    case OpCodeType::U8_SUB_STORE:        ret = std::shared_ptr<U8SubStoreInstr>(new U8SubStoreInstr); break;
    case OpCodeType::U8_MUL_STORE:        ret = std::shared_ptr<U8MulStoreInstr>(new U8MulStoreInstr); break;
    case OpCodeType::U8_MAX_STORE:        ret = std::shared_ptr<U8MaxStoreInstr>(new U8MaxStoreInstr); break;
    case OpCodeType::U8_MIN_STORE:        ret = std::shared_ptr<U8MinStoreInstr>(new U8MinStoreInstr); break;
    case OpCodeType::E43_ADD_STORE:       ret = std::shared_ptr<E43AddStoreInstr>(new E43AddStoreInstr); break;
    case OpCodeType::E43_SUB_STORE:       ret = std::shared_ptr<E43SubStoreInstr>(new E43SubStoreInstr); break;
    case OpCodeType::E43_MUL_STORE:       ret = std::shared_ptr<E43MulStoreInstr>(new E43MulStoreInstr); break;
    case OpCodeType::E43_MAX_STORE:       ret = std::shared_ptr<E43MaxStoreInstr>(new E43MaxStoreInstr); break;
    case OpCodeType::E43_MIN_STORE:       ret = std::shared_ptr<E43MinStoreInstr>(new E43MinStoreInstr); break;
    case OpCodeType::E52_ADD_STORE:       ret = std::shared_ptr<E52AddStoreInstr>(new E52AddStoreInstr); break;
    case OpCodeType::E52_SUB_STORE:       ret = std::shared_ptr<E52SubStoreInstr>(new E52SubStoreInstr); break;
    case OpCodeType::E52_MUL_STORE:       ret = std::shared_ptr<E52MulStoreInstr>(new E52MulStoreInstr); break;
    case OpCodeType::E52_MAX_STORE:       ret = std::shared_ptr<E52MaxStoreInstr>(new E52MaxStoreInstr); break;
    case OpCodeType::E52_MIN_STORE:       ret = std::shared_ptr<E52MinStoreInstr>(new E52MinStoreInstr); break;
    case OpCodeType::I16_ADD_STORE:       ret = std::shared_ptr<I16AddStoreInstr>(new I16AddStoreInstr); break;
    case OpCodeType::I16_SUB_STORE:       ret = std::shared_ptr<I16SubStoreInstr>(new I16SubStoreInstr); break;
    case OpCodeType::I16_MUL_STORE:       ret = std::shared_ptr<I16MulStoreInstr>(new I16MulStoreInstr); break;
    case OpCodeType::I16_MAX_STORE:       ret = std::shared_ptr<I16MaxStoreInstr>(new I16MaxStoreInstr); break;
    case OpCodeType::I16_MIN_STORE:       ret = std::shared_ptr<I16MinStoreInstr>(new I16MinStoreInstr); break;
    case OpCodeType::F16_ADD_STORE:       ret = std::shared_ptr<F16AddStoreInstr>(new F16AddStoreInstr); break;
    case OpCodeType::F16_SUB_STORE:       ret = std::shared_ptr<F16SubStoreInstr>(new F16SubStoreInstr); break;
    case OpCodeType::F16_MUL_STORE:       ret = std::shared_ptr<F16MulStoreInstr>(new F16MulStoreInstr); break;
    case OpCodeType::F16_MAX_STORE:       ret = std::shared_ptr<F16MaxStoreInstr>(new F16MaxStoreInstr); break;
    case OpCodeType::F16_MIN_STORE:       ret = std::shared_ptr<F16MinStoreInstr>(new F16MinStoreInstr); break;
    case OpCodeType::F32_ADD_STORE:       ret = std::shared_ptr<F32AddStoreInstr>(new F32AddStoreInstr); break;
    case OpCodeType::F32_SUB_STORE:       ret = std::shared_ptr<F32SubStoreInstr>(new F32SubStoreInstr); break;
    case OpCodeType::F32_MUL_STORE:       ret = std::shared_ptr<F32MulStoreInstr>(new F32MulStoreInstr); break;
    case OpCodeType::F32_MAX_STORE:       ret = std::shared_ptr<F32MaxStoreInstr>(new F32MaxStoreInstr); break;
    case OpCodeType::F32_MIN_STORE:       ret = std::shared_ptr<F32MinStoreInstr>(new F32MinStoreInstr); break;
    case OpCodeType::I32_ADD_STORE:       ret = std::shared_ptr<I32AddStoreInstr>(new I32AddStoreInstr); break;
    case OpCodeType::I32_SUB_STORE:       ret = std::shared_ptr<I32SubStoreInstr>(new I32SubStoreInstr); break;
    case OpCodeType::I32_MUL_STORE:       ret = std::shared_ptr<I32MulStoreInstr>(new I32MulStoreInstr); break;
    case OpCodeType::I32_MAX_STORE:       ret = std::shared_ptr<I32MaxStoreInstr>(new I32MaxStoreInstr); break;
    case OpCodeType::I32_MIN_STORE:       ret = std::shared_ptr<I32MinStoreInstr>(new I32MinStoreInstr); break;
    case OpCodeType::I8_DLOAD_ADD_STORE:  ret = std::shared_ptr<I8DloadAddStoreInstr>(new I8DloadAddStoreInstr); break;
    case OpCodeType::I8_DLOAD_SUB_STORE:  ret = std::shared_ptr<I8DloadSubStoreInstr>(new I8DloadSubStoreInstr); break;
    case OpCodeType::I8_DLOAD_MUL_STORE:  ret = std::shared_ptr<I8DloadMulStoreInstr>(new I8DloadMulStoreInstr); break;
    case OpCodeType::I8_DLOAD_MAX_STORE:  ret = std::shared_ptr<I8DloadMaxStoreInstr>(new I8DloadMaxStoreInstr); break;
    case OpCodeType::I8_DLOAD_MIN_STORE:  ret = std::shared_ptr<I8DloadMinStoreInstr>(new I8DloadMinStoreInstr); break;
    case OpCodeType::I8_LOAD_ADD_STORE:   ret = std::shared_ptr<I8LoadAddStoreInstr>(new I8LoadAddStoreInstr); break;
    case OpCodeType::I8_LOAD_SUB_STORE:   ret = std::shared_ptr<I8LoadSubStoreInstr>(new I8LoadSubStoreInstr); break;
    case OpCodeType::I8_LOAD_MUL_STORE:   ret = std::shared_ptr<I8LoadMulStoreInstr>(new I8LoadMulStoreInstr); break;
    case OpCodeType::I8_LOAD_MAX_STORE:   ret = std::shared_ptr<I8LoadMaxStoreInstr>(new I8LoadMaxStoreInstr); break;
    case OpCodeType::I8_LOAD_MIN_STORE:   ret = std::shared_ptr<I8LoadMinStoreInstr>(new I8LoadMinStoreInstr); break;
    case OpCodeType::U8_DLOAD_ADD_STORE:  ret = std::shared_ptr<U8DloadAddStoreInstr>(new U8DloadAddStoreInstr); break;
    case OpCodeType::U8_DLOAD_SUB_STORE:  ret = std::shared_ptr<U8DloadSubStoreInstr>(new U8DloadSubStoreInstr); break;
    case OpCodeType::U8_DLOAD_MUL_STORE:  ret = std::shared_ptr<U8DloadMulStoreInstr>(new U8DloadMulStoreInstr); break;
    case OpCodeType::U8_DLOAD_MAX_STORE:  ret = std::shared_ptr<U8DloadMaxStoreInstr>(new U8DloadMaxStoreInstr); break;
    case OpCodeType::U8_DLOAD_MIN_STORE:  ret = std::shared_ptr<U8DloadMinStoreInstr>(new U8DloadMinStoreInstr); break;
    case OpCodeType::U8_LOAD_ADD_STORE:   ret = std::shared_ptr<U8LoadAddStoreInstr>(new U8LoadAddStoreInstr); break;
    case OpCodeType::U8_LOAD_SUB_STORE:   ret = std::shared_ptr<U8LoadSubStoreInstr>(new U8LoadSubStoreInstr); break;
    case OpCodeType::U8_LOAD_MUL_STORE:   ret = std::shared_ptr<U8LoadMulStoreInstr>(new U8LoadMulStoreInstr); break;
    case OpCodeType::U8_LOAD_MAX_STORE:   ret = std::shared_ptr<U8LoadMaxStoreInstr>(new U8LoadMaxStoreInstr); break;
    case OpCodeType::U8_LOAD_MIN_STORE:   ret = std::shared_ptr<U8LoadMinStoreInstr>(new U8LoadMinStoreInstr); break;
    case OpCodeType::E43_DLOAD_ADD_STORE: ret = std::shared_ptr<E43DloadAddStoreInstr>(new E43DloadAddStoreInstr); break;
    case OpCodeType::E43_DLOAD_SUB_STORE: ret = std::shared_ptr<E43DloadSubStoreInstr>(new E43DloadSubStoreInstr); break;
    case OpCodeType::E43_DLOAD_MUL_STORE: ret = std::shared_ptr<E43DloadMulStoreInstr>(new E43DloadMulStoreInstr); break;
    case OpCodeType::E43_DLOAD_MAX_STORE: ret = std::shared_ptr<E43DloadMaxStoreInstr>(new E43DloadMaxStoreInstr); break;
    case OpCodeType::E43_DLOAD_MIN_STORE: ret = std::shared_ptr<E43DloadMinStoreInstr>(new E43DloadMinStoreInstr); break;
    case OpCodeType::E43_LOAD_ADD_STORE:  ret = std::shared_ptr<E43LoadAddStoreInstr>(new E43LoadAddStoreInstr); break;
    case OpCodeType::E43_LOAD_SUB_STORE:  ret = std::shared_ptr<E43LoadSubStoreInstr>(new E43LoadSubStoreInstr); break;
    case OpCodeType::E43_LOAD_MUL_STORE:  ret = std::shared_ptr<E43LoadMulStoreInstr>(new E43LoadMulStoreInstr); break;
    case OpCodeType::E43_LOAD_MAX_STORE:  ret = std::shared_ptr<E43LoadMaxStoreInstr>(new E43LoadMaxStoreInstr); break;
    case OpCodeType::E43_LOAD_MIN_STORE:  ret = std::shared_ptr<E43LoadMinStoreInstr>(new E43LoadMinStoreInstr); break;
    case OpCodeType::E52_DLOAD_ADD_STORE: ret = std::shared_ptr<E52DloadAddStoreInstr>(new E52DloadAddStoreInstr); break;
    case OpCodeType::E52_DLOAD_SUB_STORE: ret = std::shared_ptr<E52DloadSubStoreInstr>(new E52DloadSubStoreInstr); break;
    case OpCodeType::E52_DLOAD_MUL_STORE: ret = std::shared_ptr<E52DloadMulStoreInstr>(new E52DloadMulStoreInstr); break;
    case OpCodeType::E52_DLOAD_MAX_STORE: ret = std::shared_ptr<E52DloadMaxStoreInstr>(new E52DloadMaxStoreInstr); break;
    case OpCodeType::E52_DLOAD_MIN_STORE: ret = std::shared_ptr<E52DloadMinStoreInstr>(new E52DloadMinStoreInstr); break;
    case OpCodeType::E52_LOAD_ADD_STORE:  ret = std::shared_ptr<E52LoadAddStoreInstr>(new E52LoadAddStoreInstr); break;
    case OpCodeType::E52_LOAD_SUB_STORE:  ret = std::shared_ptr<E52LoadSubStoreInstr>(new E52LoadSubStoreInstr); break;
    case OpCodeType::E52_LOAD_MUL_STORE:  ret = std::shared_ptr<E52LoadMulStoreInstr>(new E52LoadMulStoreInstr); break;
    case OpCodeType::E52_LOAD_MAX_STORE:  ret = std::shared_ptr<E52LoadMaxStoreInstr>(new E52LoadMaxStoreInstr); break;
    case OpCodeType::E52_LOAD_MIN_STORE:  ret = std::shared_ptr<E52LoadMinStoreInstr>(new E52LoadMinStoreInstr); break;
    case OpCodeType::I16_DLOAD_ADD_STORE: ret = std::shared_ptr<I16DloadAddStoreInstr>(new I16DloadAddStoreInstr); break;
    case OpCodeType::I16_DLOAD_SUB_STORE: ret = std::shared_ptr<I16DloadSubStoreInstr>(new I16DloadSubStoreInstr); break;
    case OpCodeType::I16_DLOAD_MUL_STORE: ret = std::shared_ptr<I16DloadMulStoreInstr>(new I16DloadMulStoreInstr); break;
    case OpCodeType::I16_DLOAD_MAX_STORE: ret = std::shared_ptr<I16DloadMaxStoreInstr>(new I16DloadMaxStoreInstr); break;
    case OpCodeType::I16_DLOAD_MIN_STORE: ret = std::shared_ptr<I16DloadMinStoreInstr>(new I16DloadMinStoreInstr); break;
    case OpCodeType::I16_LOAD_ADD_STORE:  ret = std::shared_ptr<I16LoadAddStoreInstr>(new I16LoadAddStoreInstr); break;
    case OpCodeType::I16_LOAD_SUB_STORE:  ret = std::shared_ptr<I16LoadSubStoreInstr>(new I16LoadSubStoreInstr); break;
    case OpCodeType::I16_LOAD_MUL_STORE:  ret = std::shared_ptr<I16LoadMulStoreInstr>(new I16LoadMulStoreInstr); break;
    case OpCodeType::I16_LOAD_MAX_STORE:  ret = std::shared_ptr<I16LoadMaxStoreInstr>(new I16LoadMaxStoreInstr); break;
    case OpCodeType::I16_LOAD_MIN_STORE:  ret = std::shared_ptr<I16LoadMinStoreInstr>(new I16LoadMinStoreInstr); break;
    case OpCodeType::F16_DLOAD_ADD_STORE: ret = std::shared_ptr<F16DloadAddStoreInstr>(new F16DloadAddStoreInstr); break;
    case OpCodeType::F16_DLOAD_SUB_STORE: ret = std::shared_ptr<F16DloadSubStoreInstr>(new F16DloadSubStoreInstr); break;
    case OpCodeType::F16_DLOAD_MUL_STORE: ret = std::shared_ptr<F16DloadMulStoreInstr>(new F16DloadMulStoreInstr); break;
    case OpCodeType::F16_DLOAD_MAX_STORE: ret = std::shared_ptr<F16DloadMaxStoreInstr>(new F16DloadMaxStoreInstr); break;
    case OpCodeType::F16_DLOAD_MIN_STORE: ret = std::shared_ptr<F16DloadMinStoreInstr>(new F16DloadMinStoreInstr); break;
    case OpCodeType::F16_LOAD_ADD_STORE:  ret = std::shared_ptr<F16LoadAddStoreInstr>(new F16LoadAddStoreInstr); break;
    case OpCodeType::F16_LOAD_SUB_STORE:  ret = std::shared_ptr<F16LoadSubStoreInstr>(new F16LoadSubStoreInstr); break;
    case OpCodeType::F16_LOAD_MUL_STORE:  ret = std::shared_ptr<F16LoadMulStoreInstr>(new F16LoadMulStoreInstr); break;
    case OpCodeType::F16_LOAD_MAX_STORE:  ret = std::shared_ptr<F16LoadMaxStoreInstr>(new F16LoadMaxStoreInstr); break;
    case OpCodeType::F16_LOAD_MIN_STORE:  ret = std::shared_ptr<F16LoadMinStoreInstr>(new F16LoadMinStoreInstr); break;
    case OpCodeType::F32_DLOAD_ADD_STORE: ret = std::shared_ptr<F32DloadAddStoreInstr>(new F32DloadAddStoreInstr); break;
    case OpCodeType::F32_DLOAD_SUB_STORE: ret = std::shared_ptr<F32DloadSubStoreInstr>(new F32DloadSubStoreInstr); break;
    case OpCodeType::F32_DLOAD_MUL_STORE: ret = std::shared_ptr<F32DloadMulStoreInstr>(new F32DloadMulStoreInstr); break;
    case OpCodeType::F32_DLOAD_MAX_STORE: ret = std::shared_ptr<F32DloadMaxStoreInstr>(new F32DloadMaxStoreInstr); break;
    case OpCodeType::F32_DLOAD_MIN_STORE: ret = std::shared_ptr<F32DloadMinStoreInstr>(new F32DloadMinStoreInstr); break;
    case OpCodeType::F32_LOAD_ADD_STORE:  ret = std::shared_ptr<F32LoadAddStoreInstr>(new F32LoadAddStoreInstr); break;
    case OpCodeType::F32_LOAD_SUB_STORE:  ret = std::shared_ptr<F32LoadSubStoreInstr>(new F32LoadSubStoreInstr); break;
    case OpCodeType::F32_LOAD_MUL_STORE:  ret = std::shared_ptr<F32LoadMulStoreInstr>(new F32LoadMulStoreInstr); break;
    case OpCodeType::F32_LOAD_MAX_STORE:  ret = std::shared_ptr<F32LoadMaxStoreInstr>(new F32LoadMaxStoreInstr); break;
    case OpCodeType::F32_LOAD_MIN_STORE:  ret = std::shared_ptr<F32LoadMinStoreInstr>(new F32LoadMinStoreInstr); break;
    case OpCodeType::I32_DLOAD_ADD_STORE: ret = std::shared_ptr<I32DloadAddStoreInstr>(new I32DloadAddStoreInstr); break;
    case OpCodeType::I32_DLOAD_SUB_STORE: ret = std::shared_ptr<I32DloadSubStoreInstr>(new I32DloadSubStoreInstr); break;
    case OpCodeType::I32_DLOAD_MUL_STORE: ret = std::shared_ptr<I32DloadMulStoreInstr>(new I32DloadMulStoreInstr); break;
    case OpCodeType::I32_DLOAD_MAX_STORE: ret = std::shared_ptr<I32DloadMaxStoreInstr>(new I32DloadMaxStoreInstr); break;
    case OpCodeType::I32_DLOAD_MIN_STORE: ret = std::shared_ptr<I32DloadMinStoreInstr>(new I32DloadMinStoreInstr); break;
    case OpCodeType::I32_LOAD_ADD_STORE:  ret = std::shared_ptr<I32LoadAddStoreInstr>(new I32LoadAddStoreInstr); break;
    case OpCodeType::I32_LOAD_SUB_STORE:  ret = std::shared_ptr<I32LoadSubStoreInstr>(new I32LoadSubStoreInstr); break;
    case OpCodeType::I32_LOAD_MUL_STORE:  ret = std::shared_ptr<I32LoadMulStoreInstr>(new I32LoadMulStoreInstr); break;
    case OpCodeType::I32_LOAD_MAX_STORE:  ret = std::shared_ptr<I32LoadMaxStoreInstr>(new I32LoadMaxStoreInstr); break;
    case OpCodeType::I32_LOAD_MIN_STORE:  ret = std::shared_ptr<I32LoadMinStoreInstr>(new I32LoadMinStoreInstr); break;
    case OpCodeType::S_ADDI:              ret = std::shared_ptr<SAddiInstr>(new SAddiInstr); break;
    case OpCodeType::S_ADDIU:             ret = std::shared_ptr<SAddiuInstr>(new SAddiuInstr); break;
    case OpCodeType::S_SUBI:              ret = std::shared_ptr<SSubiInstr>(new SSubiInstr); break;
    case OpCodeType::S_ANDI:              ret = std::shared_ptr<SAndiInstr>(new SAndiInstr); break;
    case OpCodeType::S_ORI:               ret = std::shared_ptr<SOriInstr>(new SOriInstr); break;
    case OpCodeType::S_XORI:              ret = std::shared_ptr<SXoriInstr>(new SXoriInstr); break;
    case OpCodeType::S_SGTI:              ret = std::shared_ptr<SSgtiInstr>(new SSgtiInstr); break;
    case OpCodeType::S_SLTI:              ret = std::shared_ptr<SSltiInstr>(new SSltiInstr); break;
    case OpCodeType::S_SEQI:              ret = std::shared_ptr<SSeqiInstr>(new SSeqiInstr); break;
    case OpCodeType::S_SGEI:              ret = std::shared_ptr<SSgeiInstr>(new SSgeiInstr); break;
    case OpCodeType::S_SLEI:              ret = std::shared_ptr<SSleiInstr>(new SSleiInstr); break;
    case OpCodeType::S_SRAI:              ret = std::shared_ptr<SSraiInstr>(new SSraiInstr); break;
    case OpCodeType::S_SRLI:              ret = std::shared_ptr<SSrliInstr>(new SSrliInstr); break;
    case OpCodeType::S_SLLI:              ret = std::shared_ptr<SSlliInstr>(new SSlliInstr); break;
    case OpCodeType::S_LUI:               ret = std::shared_ptr<SLuiInstr>(new SLuiInstr); break;
    case OpCodeType::S_ADD:               ret = std::shared_ptr<SAddInstr>(new SAddInstr); break;
    case OpCodeType::S_ADDU:              ret = std::shared_ptr<SAdduInstr>(new SAdduInstr); break;
    case OpCodeType::S_SUB:               ret = std::shared_ptr<SSubInstr>(new SSubInstr); break;
    case OpCodeType::S_MUL:               ret = std::shared_ptr<SMulInstr>(new SMulInstr); break;
    case OpCodeType::S_MULH:              ret = std::shared_ptr<SMulhInstr>(new SMulhInstr); break;
    case OpCodeType::S_MULHU:             ret = std::shared_ptr<SMulhuInstr>(new SMulhuInstr); break;
    case OpCodeType::S_MIN:               ret = std::shared_ptr<SMinInstr>(new SMinInstr); break;
    case OpCodeType::S_MAX:               ret = std::shared_ptr<SMaxInstr>(new SMaxInstr); break;
    case OpCodeType::S_MINU:              ret = std::shared_ptr<SMinuInstr>(new SMinuInstr); break;
    case OpCodeType::S_MAXU:              ret = std::shared_ptr<SMaxuInstr>(new SMaxuInstr); break;
    case OpCodeType::S_AND:               ret = std::shared_ptr<SAndInstr>(new SAndInstr); break;
    case OpCodeType::S_OR:                ret = std::shared_ptr<SOrInstr>(new SOrInstr); break;
    case OpCodeType::S_XOR:               ret = std::shared_ptr<SXorInstr>(new SXorInstr); break;
    case OpCodeType::S_SGT:               ret = std::shared_ptr<SSgtInstr>(new SSgtInstr); break;
    case OpCodeType::S_SLT:               ret = std::shared_ptr<SSltInstr>(new SSltInstr); break;
    case OpCodeType::S_SEQ:               ret = std::shared_ptr<SSeqInstr>(new SSeqInstr); break;
    case OpCodeType::S_SGE:               ret = std::shared_ptr<SSgeInstr>(new SSgeInstr); break;
    case OpCodeType::S_SLE:               ret = std::shared_ptr<SSleInstr>(new SSleInstr); break;
    case OpCodeType::S_CMPSEL:            ret = std::shared_ptr<SCmpselInstr>(new SCmpselInstr); break;
    case OpCodeType::S_SRA:               ret = std::shared_ptr<SSraInstr>(new SSraInstr); break;
    case OpCodeType::S_SRL:               ret = std::shared_ptr<SSrlInstr>(new SSrlInstr); break;
    case OpCodeType::S_SLL:               ret = std::shared_ptr<SSllInstr>(new SSllInstr); break;
    case OpCodeType::S_JUMP:              ret = std::shared_ptr<SJumpInstr>(new SJumpInstr); break;
    case OpCodeType::S_JAL:               ret = std::shared_ptr<SJalInstr>(new SJalInstr); break;
    case OpCodeType::S_JR:                ret = std::shared_ptr<SJrInstr>(new SJrInstr); break;
    case OpCodeType::S_BNE:               ret = std::shared_ptr<SBneInstr>(new SBneInstr); break;
    case OpCodeType::S_BEQ:               ret = std::shared_ptr<SBeqInstr>(new SBeqInstr); break;
    case OpCodeType::S_BLT:               ret = std::shared_ptr<SBltInstr>(new SBltInstr); break;
    case OpCodeType::S_BLTU:              ret = std::shared_ptr<SBltuInstr>(new SBltuInstr); break;
    case OpCodeType::S_BGE:               ret = std::shared_ptr<SBgeInstr>(new SBgeInstr); break;
    case OpCodeType::S_BGEU:              ret = std::shared_ptr<SBgeuInstr>(new SBgeuInstr); break;
    case OpCodeType::S_SETCFG:            ret = std::shared_ptr<SSetcfgInstr>(new SSetcfgInstr); break;
    case OpCodeType::S_SETRF:             ret = std::shared_ptr<SSetrfInstr>(new SSetrfInstr); break;
    default: break;
  }
  for (size_t i = 0; i < args.size(); ++i)
    ret->setters[i](args[i]);
  return ret;
}

} // namespace quark

#endif // QUARK_VPU_INSTR_H
