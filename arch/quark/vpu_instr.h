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
    SET_LOAD_AAI = 9,
    SET_STORE_AAI = 10,
    VPU_NOP = 11,
    VPU_END = 12,
    I8_DLOAD = 13,
    I8_LOAD = 14,
    U8_DLOAD = 15,
    U8_LOAD = 16,
    F16_DLOAD = 17,
    F16_LOAD = 18,
    F32_DLOAD = 19,
    F32_LOAD = 20,
    I8_STORE = 21,
    U8_STORE = 22,
    F16_STORE = 23,
    F32_STORE = 24,
    F32_ADD = 25,
    F32_SUB = 26,
    F32_MUL = 27,
    F32_MAX = 28,
    F32_MIN = 29,
    F32_GREATER = 30,
    F32_GREATER_EQUAL = 31,
    F32_LESS = 32,
    F32_LESS_EQUAL = 33,
    F32_EQUAL = 34,
    F32_SEL = 35,
    I32_AND = 36,
    I32_OR = 37,
    I32_NOT = 38,
    I32_XOR = 39,
    I32_XNOR = 40,
    F32_ADDI = 41,
    F32_SUBI = 42,
    F32_MULI = 43,
    F32_MAXI = 44,
    F32_MINI = 45,
    F32_MOVI = 46,
    F32_GREATERI = 47,
    F32_GREATER_EQUALI = 48,
    F32_LESSI = 49,
    F32_LESS_EQUALI = 50,
    F32_EQUALI = 51,
    F32_SELI = 52,
    F32_LUI = 53,
    F32_LI = 54,
    I32_ANDI = 55,
    I32_ORI = 56,
    I32_NOTI = 57,
    I32_XORI = 58,
    I32_XNORI = 59,
    I8_DLOAD_ADD = 60,
    I8_DLOAD_SUB = 61,
    I8_DLOAD_MUL = 62,
    I8_DLOAD_MAX = 63,
    I8_DLOAD_MIN = 64,
    I8_LOAD_ADD = 65,
    I8_LOAD_SUB = 66,
    I8_LOAD_MUL = 67,
    I8_LOAD_MAX = 68,
    I8_LOAD_MIN = 69,
    U8_DLOAD_ADD = 70,
    U8_DLOAD_SUB = 71,
    U8_DLOAD_MUL = 72,
    U8_DLOAD_MAX = 73,
    U8_DLOAD_MIN = 74,
    U8_LOAD_ADD = 75,
    U8_LOAD_SUB = 76,
    U8_LOAD_MUL = 77,
    U8_LOAD_MAX = 78,
    U8_LOAD_MIN = 79,
    F16_DLOAD_ADD = 80,
    F16_DLOAD_SUB = 81,
    F16_DLOAD_MUL = 82,
    F16_DLOAD_MAX = 83,
    F16_DLOAD_MIN = 84,
    F16_LOAD_ADD = 85,
    F16_LOAD_SUB = 86,
    F16_LOAD_MUL = 87,
    F16_LOAD_MAX = 88,
    F16_LOAD_MIN = 89,
    F32_DLOAD_ADD = 90,
    F32_DLOAD_SUB = 91,
    F32_DLOAD_MUL = 92,
    F32_DLOAD_MAX = 93,
    F32_DLOAD_MIN = 94,
    F32_LOAD_ADD = 95,
    F32_LOAD_SUB = 96,
    F32_LOAD_MUL = 97,
    F32_LOAD_MAX = 98,
    F32_LOAD_MIN = 99,
    I32_DLOAD_AND = 100,
    I32_DLOAD_OR = 101,
    I32_DLOAD_NOT = 102,
    I32_DLOAD_XOR = 103,
    I32_DLOAD_XNOR = 104,
    I32_LOAD_AND = 105,
    I32_LOAD_OR = 106,
    I32_LOAD_NOT = 107,
    I32_LOAD_XOR = 108,
    I32_LOAD_XNOR = 109,
    I8_ADD_STORE = 110,
    I8_SUB_STORE = 111,
    I8_MUL_STORE = 112,
    I8_MAX_STORE = 113,
    I8_MIN_STORE = 114,
    U8_ADD_STORE = 115,
    U8_SUB_STORE = 116,
    U8_MUL_STORE = 117,
    U8_MAX_STORE = 118,
    U8_MIN_STORE = 119,
    F16_ADD_STORE = 120,
    F16_SUB_STORE = 121,
    F16_MUL_STORE = 122,
    F16_MAX_STORE = 123,
    F16_MIN_STORE = 124,
    F32_ADD_STORE = 125,
    F32_SUB_STORE = 126,
    F32_MUL_STORE = 127,
    F32_MAX_STORE = 128,
    F32_MIN_STORE = 129,
    I32_AND_STORE = 130,
    I32_OR_STORE = 131,
    I32_NOT_STORE = 132,
    I32_XOR_STORE = 133,
    I32_XNOR_STORE = 134,
    I8_DLOAD_ADD_STORE = 135,
    I8_DLOAD_SUB_STORE = 136,
    I8_DLOAD_MUL_STORE = 137,
    I8_DLOAD_MAX_STORE = 138,
    I8_DLOAD_MIN_STORE = 139,
    I8_LOAD_ADD_STORE = 140,
    I8_LOAD_SUB_STORE = 141,
    I8_LOAD_MUL_STORE = 142,
    I8_LOAD_MAX_STORE = 143,
    I8_LOAD_MIN_STORE = 144,
    U8_DLOAD_ADD_STORE = 145,
    U8_DLOAD_SUB_STORE = 146,
    U8_DLOAD_MUL_STORE = 147,
    U8_DLOAD_MAX_STORE = 148,
    U8_DLOAD_MIN_STORE = 149,
    U8_LOAD_ADD_STORE = 150,
    U8_LOAD_SUB_STORE = 151,
    U8_LOAD_MUL_STORE = 152,
    U8_LOAD_MAX_STORE = 153,
    U8_LOAD_MIN_STORE = 154,
    F16_DLOAD_ADD_STORE = 155,
    F16_DLOAD_SUB_STORE = 156,
    F16_DLOAD_MUL_STORE = 157,
    F16_DLOAD_MAX_STORE = 158,
    F16_DLOAD_MIN_STORE = 159,
    F16_LOAD_ADD_STORE = 160,
    F16_LOAD_SUB_STORE = 161,
    F16_LOAD_MUL_STORE = 162,
    F16_LOAD_MAX_STORE = 163,
    F16_LOAD_MIN_STORE = 164,
    F32_DLOAD_ADD_STORE = 165,
    F32_DLOAD_SUB_STORE = 166,
    F32_DLOAD_MUL_STORE = 167,
    F32_DLOAD_MAX_STORE = 168,
    F32_DLOAD_MIN_STORE = 169,
    F32_LOAD_ADD_STORE = 170,
    F32_LOAD_SUB_STORE = 171,
    F32_LOAD_MUL_STORE = 172,
    F32_LOAD_MAX_STORE = 173,
    F32_LOAD_MIN_STORE = 174,
    I32_DLOAD_AND_STORE = 175,
    I32_DLOAD_OR_STORE = 176,
    I32_DLOAD_NOT_STORE = 177,
    I32_DLOAD_XOR_STORE = 178,
    I32_DLOAD_XNOR_STORE = 179,
    I32_LOAD_AND_STORE = 180,
    I32_LOAD_OR_STORE = 181,
    I32_LOAD_NOT_STORE = 182,
    I32_LOAD_XOR_STORE = 183,
    I32_LOAD_XNOR_STORE = 184,
    S_ADDI = 185,
    S_ADDIU = 186,
    S_SUBI = 187,
    S_ANDI = 188,
    S_ORI = 189,
    S_XORI = 190,
    S_SGTI = 191,
    S_SLTI = 192,
    S_SEQI = 193,
    S_SGEI = 194,
    S_SLEI = 195,
    S_SRAI = 196,
    S_SRLI = 197,
    S_SLLI = 198,
    S_LUI = 199,
    S_ADD = 200,
    S_ADDU = 201,
    S_SUB = 202,
    S_MUL = 203,
    S_MULH = 204,
    S_MULHU = 205,
    S_MIN = 206,
    S_MAX = 207,
    S_MINU = 208,
    S_MAXU = 209,
    S_AND = 210,
    S_OR = 211,
    S_XOR = 212,
    S_SGT = 213,
    S_SLT = 214,
    S_SEQ = 215,
    S_SGE = 216,
    S_SLE = 217,
    S_CMPSEL = 218,
    S_SRA = 219,
    S_SRL = 220,
    S_SLL = 221,
    S_JUMP = 222,
    S_JAL = 223,
    S_JR = 224,
    S_BNE = 225,
    S_BEQ = 226,
    S_BLT = 227,
    S_BLTU = 228,
    S_BGE = 229,
    S_BGEU = 230,
    S_SETCFG = 231,
    S_SETRF = 232,
    INVALID = 65536
  };

  static const std::map<OpCodeType, OpCodeItem>& getOpCodeMap() {
    static const std::map<OpCodeType, OpCodeItem> opCodeMap = {
      {OpCodeType::SET_LOAD_BASE,        {"set_load_base",        0xff000000, 0x01000000}},
      {OpCodeType::SET_LOAD2_BASE,       {"set_load2_base",       0xff000000, 0x04000000}},
      {OpCodeType::SET_STORE_BASE,       {"set_store_base",       0xff000000, 0x07000000}},
      {OpCodeType::SET_LOAD_GLB,         {"set_load_glb",         0xff000001, 0x02000000}},
      {OpCodeType::SET_STORE_GLB,        {"set_store_glb",        0xff000001, 0x08000000}},
      {OpCodeType::SET_QUANTIZE,         {"set_quantize",         0xffff0000, 0x09000000}},
      {OpCodeType::SET_DEQUANTIZE,       {"set_dequantize",       0xff000000, 0x03000000}},
      {OpCodeType::SET_DEQUANTIZE2,      {"set_dequantize2",      0xff000000, 0x06000000}},
      {OpCodeType::SET_FS,               {"set_fs",               0xffffff00, 0x0a000000}},
      {OpCodeType::SET_LOAD_AAI,         {"set_load_aai",         0xff070000, 0x0b000000}},
      {OpCodeType::SET_STORE_AAI,        {"set_store_aai",        0xff070000, 0x0c000000}},
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
      {OpCodeType::I8_STORE,             {"i8_store",             0xffcf0000, 0xc8000000}},
      {OpCodeType::U8_STORE,             {"u8_store",             0xffcf0000, 0xd8000000}},
      {OpCodeType::F16_STORE,            {"f16_store",            0xffcf0000, 0xe8000000}},
      {OpCodeType::F32_STORE,            {"f32_store",            0xffcf0000, 0xf8000000}},
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
      {OpCodeType::F32_ADDI,             {"f32_addi",             0xff030000, 0xb0000000}},
      {OpCodeType::F32_SUBI,             {"f32_subi",             0xff030000, 0xb1000000}},
      {OpCodeType::F32_MULI,             {"f32_muli",             0xff030000, 0xb2000000}},
      {OpCodeType::F32_MAXI,             {"f32_maxi",             0xff030000, 0xb3000000}},
      {OpCodeType::F32_MINI,             {"f32_mini",             0xff030000, 0xb4000000}},
      {OpCodeType::F32_MOVI,             {"f32_movi",             0xff1f0000, 0xb5000000}},
      {OpCodeType::F32_GREATERI,         {"f32_greateri",         0xffe30000, 0xb7000000}},
      {OpCodeType::F32_GREATER_EQUALI,   {"f32_greater_equali",   0xffe30000, 0xba000000}},
      {OpCodeType::F32_LESSI,            {"f32_lessi",            0xffe30000, 0xb9000000}},
      {OpCodeType::F32_LESS_EQUALI,      {"f32_less_equali",      0xffe30000, 0xbb000000}},
      {OpCodeType::F32_EQUALI,           {"f32_equali",           0xffe30000, 0xb8000000}},
      {OpCodeType::F32_SELI,             {"f32_seli",             0xff030000, 0xbc000000}},
      {OpCodeType::F32_LUI,              {"f32_lui",              0xff1f0000, 0xbd000000}},
      {OpCodeType::F32_LI,               {"f32_li",               0xff1f0000, 0xbe000000}},
      {OpCodeType::I32_ANDI,             {"i32_andi",             0xff030000, 0xa0000000}},
      {OpCodeType::I32_ORI,              {"i32_ori",              0xff030000, 0xa1000000}},
      {OpCodeType::I32_NOTI,             {"i32_noti",             0xff030000, 0xa2000000}},
      {OpCodeType::I32_XORI,             {"i32_xori",             0xff030000, 0xa3000000}},
      {OpCodeType::I32_XNORI,            {"i32_xnori",            0xff030000, 0xa4000000}},
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
      {OpCodeType::I32_DLOAD_AND,        {"i32_dload_and",        0xff3f0000, 0xd2030000}},
      {OpCodeType::I32_DLOAD_OR,         {"i32_dload_or",         0xff3f0000, 0xd2070000}},
      {OpCodeType::I32_DLOAD_NOT,        {"i32_dload_not",        0xff3f0000, 0xd3030000}},
      {OpCodeType::I32_DLOAD_XOR,        {"i32_dload_xor",        0xff3f0000, 0xd3070000}},
      {OpCodeType::I32_DLOAD_XNOR,       {"i32_dload_xnor",       0xff3f0000, 0xd6030000}},
      {OpCodeType::I32_LOAD_AND,         {"i32_load_and",         0xff0f0000, 0xd2010000}},
      {OpCodeType::I32_LOAD_OR,          {"i32_load_or",          0xff0f0000, 0xd2050000}},
      {OpCodeType::I32_LOAD_NOT,         {"i32_load_not",         0xff0f0000, 0xd3010000}},
      {OpCodeType::I32_LOAD_XOR,         {"i32_load_xor",         0xff0f0000, 0xd3050000}},
      {OpCodeType::I32_LOAD_XNOR,        {"i32_load_xnor",        0xff0f0000, 0xd6010000}},
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
      {OpCodeType::I32_AND_STORE,        {"i32_and_store",        0xff0f0000, 0xda010000}},
      {OpCodeType::I32_OR_STORE,         {"i32_or_store",         0xff0f0000, 0xda050000}},
      {OpCodeType::I32_NOT_STORE,        {"i32_not_store",        0xff0f0000, 0xdb010000}},
      {OpCodeType::I32_XOR_STORE,        {"i32_xor_store",        0xff0f0000, 0xdb050000}},
      {OpCodeType::I32_XNOR_STORE,       {"i32_xnor_store",       0xff0f0000, 0xde010000}},
      {OpCodeType::I8_DLOAD_ADD_STORE,   {"i8_dload_add_store",   0xffff0000, 0xc80a0000}},
      {OpCodeType::I8_DLOAD_SUB_STORE,   {"i8_dload_sub_store",   0xffff0000, 0xc80e0000}},
      {OpCodeType::I8_DLOAD_MUL_STORE,   {"i8_dload_mul_store",   0xffff0000, 0xc90a0000}},
      {OpCodeType::I8_DLOAD_MAX_STORE,   {"i8_dload_max_store",   0xffff0000, 0xc90e0000}},
      {OpCodeType::I8_DLOAD_MIN_STORE,   {"i8_dload_min_store",   0xffff0000, 0xcc0a0000}},
      {OpCodeType::I8_LOAD_ADD_STORE,    {"i8_load_add_store",    0xffcf0000, 0xc8080000}},
      {OpCodeType::I8_LOAD_SUB_STORE,    {"i8_load_sub_store",    0xffcf0000, 0xc80c0000}},
      {OpCodeType::I8_LOAD_MUL_STORE,    {"i8_load_mul_store",    0xffcf0000, 0xc9080000}},
      {OpCodeType::I8_LOAD_MAX_STORE,    {"i8_load_max_store",    0xffcf0000, 0xc90c0000}},
      {OpCodeType::I8_LOAD_MIN_STORE,    {"i8_load_min_store",    0xffcf0000, 0xcc080000}},
      {OpCodeType::U8_DLOAD_ADD_STORE,   {"u8_dload_add_store",   0xffff0000, 0xd80a0000}},
      {OpCodeType::U8_DLOAD_SUB_STORE,   {"u8_dload_sub_store",   0xffff0000, 0xd80e0000}},
      {OpCodeType::U8_DLOAD_MUL_STORE,   {"u8_dload_mul_store",   0xffff0000, 0xd90a0000}},
      {OpCodeType::U8_DLOAD_MAX_STORE,   {"u8_dload_max_store",   0xffff0000, 0xd90e0000}},
      {OpCodeType::U8_DLOAD_MIN_STORE,   {"u8_dload_min_store",   0xffff0000, 0xdc0a0000}},
      {OpCodeType::U8_LOAD_ADD_STORE,    {"u8_load_add_store",    0xffcf0000, 0xd8080000}},
      {OpCodeType::U8_LOAD_SUB_STORE,    {"u8_load_sub_store",    0xffcf0000, 0xd80c0000}},
      {OpCodeType::U8_LOAD_MUL_STORE,    {"u8_load_mul_store",    0xffcf0000, 0xd9080000}},
      {OpCodeType::U8_LOAD_MAX_STORE,    {"u8_load_max_store",    0xffcf0000, 0xd90c0000}},
      {OpCodeType::U8_LOAD_MIN_STORE,    {"u8_load_min_store",    0xffcf0000, 0xdc080000}},
      {OpCodeType::F16_DLOAD_ADD_STORE,  {"f16_dload_add_store",  0xffff0000, 0xe80a0000}},
      {OpCodeType::F16_DLOAD_SUB_STORE,  {"f16_dload_sub_store",  0xffff0000, 0xe80e0000}},
      {OpCodeType::F16_DLOAD_MUL_STORE,  {"f16_dload_mul_store",  0xffff0000, 0xe90a0000}},
      {OpCodeType::F16_DLOAD_MAX_STORE,  {"f16_dload_max_store",  0xffff0000, 0xe90e0000}},
      {OpCodeType::F16_DLOAD_MIN_STORE,  {"f16_dload_min_store",  0xffff0000, 0xec0a0000}},
      {OpCodeType::F16_LOAD_ADD_STORE,   {"f16_load_add_store",   0xffcf0000, 0xe8080000}},
      {OpCodeType::F16_LOAD_SUB_STORE,   {"f16_load_sub_store",   0xffcf0000, 0xe80c0000}},
      {OpCodeType::F16_LOAD_MUL_STORE,   {"f16_load_mul_store",   0xffcf0000, 0xe9080000}},
      {OpCodeType::F16_LOAD_MAX_STORE,   {"f16_load_max_store",   0xffcf0000, 0xe90c0000}},
      {OpCodeType::F16_LOAD_MIN_STORE,   {"f16_load_min_store",   0xffcf0000, 0xec080000}},
      {OpCodeType::F32_DLOAD_ADD_STORE,  {"f32_dload_add_store",  0xffff0000, 0xf80a0000}},
      {OpCodeType::F32_DLOAD_SUB_STORE,  {"f32_dload_sub_store",  0xffff0000, 0xf80e0000}},
      {OpCodeType::F32_DLOAD_MUL_STORE,  {"f32_dload_mul_store",  0xffff0000, 0xf90a0000}},
      {OpCodeType::F32_DLOAD_MAX_STORE,  {"f32_dload_max_store",  0xffff0000, 0xf90e0000}},
      {OpCodeType::F32_DLOAD_MIN_STORE,  {"f32_dload_min_store",  0xffff0000, 0xfc0a0000}},
      {OpCodeType::F32_LOAD_ADD_STORE,   {"f32_load_add_store",   0xffcf0000, 0xf8080000}},
      {OpCodeType::F32_LOAD_SUB_STORE,   {"f32_load_sub_store",   0xffcf0000, 0xf80c0000}},
      {OpCodeType::F32_LOAD_MUL_STORE,   {"f32_load_mul_store",   0xffcf0000, 0xf9080000}},
      {OpCodeType::F32_LOAD_MAX_STORE,   {"f32_load_max_store",   0xffcf0000, 0xf90c0000}},
      {OpCodeType::F32_LOAD_MIN_STORE,   {"f32_load_min_store",   0xffcf0000, 0xfc080000}},
      {OpCodeType::I32_DLOAD_AND_STORE,  {"i32_dload_and_store",  0xffff0000, 0xda0a0000}},
      {OpCodeType::I32_DLOAD_OR_STORE,   {"i32_dload_or_store",   0xffff0000, 0xda0e0000}},
      {OpCodeType::I32_DLOAD_NOT_STORE,  {"i32_dload_not_store",  0xffff0000, 0xdb0a0000}},
      {OpCodeType::I32_DLOAD_XOR_STORE,  {"i32_dload_xor_store",  0xffff0000, 0xdb0e0000}},
      {OpCodeType::I32_DLOAD_XNOR_STORE, {"i32_dload_xnor_store", 0xffff0000, 0xde0a0000}},
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
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class SetLoad2BaseInstr : public VpuInstr {
public:
  explicit SetLoad2BaseInstr() : VpuInstr(OpCodeType::SET_LOAD2_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class SetStoreBaseInstr : public VpuInstr {
public:
  explicit SetStoreBaseInstr() : VpuInstr(OpCodeType::SET_STORE_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
    alu = 1;
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
    alu = 1;
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
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 5);
  VPU_GEN_GETTER_SETTER(BankMask, 4, 1);
};

class SetQuantizeInstr : public VpuInstr {
public:
  explicit SetQuantizeInstr() : VpuInstr(OpCodeType::SET_QUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Quantize);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Quantize, 15, 0);
};

class SetDequantizeInstr : public VpuInstr {
public:
  explicit SetDequantizeInstr() : VpuInstr(OpCodeType::SET_DEQUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 23, 0);
};

class SetDequantize2Instr : public VpuInstr {
public:
  explicit SetDequantize2Instr() : VpuInstr(OpCodeType::SET_DEQUANTIZE2) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 23, 0);
};

class SetFsInstr : public VpuInstr {
public:
  explicit SetFsInstr() : VpuInstr(OpCodeType::SET_FS) {
    VPU_PUSH_GETTER_SETTER(FsSel);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(FsSel, 7, 0);
};

class SetLoadAaiInstr : public VpuInstr {
public:
  explicit SetLoadAaiInstr() : VpuInstr(OpCodeType::SET_LOAD_AAI) {
    VPU_PUSH_GETTER_SETTER(Enable);
    VPU_PUSH_GETTER_SETTER(Mode);
    VPU_PUSH_GETTER_SETTER(StrideLength);
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Enable, 23, 23);
  VPU_GEN_GETTER_SETTER(Mode, 22, 19);
  VPU_GEN_GETTER_SETTER(StrideLength, 15, 0);
};

class SetStoreAaiInstr : public VpuInstr {
public:
  explicit SetStoreAaiInstr() : VpuInstr(OpCodeType::SET_STORE_AAI) {
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
    alu = 1;
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
    alu = 1;
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
    alu = 1;
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
    alu = 1;
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
    alu = 1;
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
    hasImm = 1;
    dtype = 12;
    alu = 1;
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
    hasImm = 1;
    dtype = 12;
    alu = 1;
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
    hasImm = 1;
    dtype = 12;
    alu = 1;
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
    hasImm = 1;
    dtype = 12;
    alu = 1;
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
    hasImm = 1;
    dtype = 12;
    alu = 1;
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
    hasImm = 1;
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F32GreateriInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32GreateriInstr() : VpuInstr(OpCodeType::F32_GREATERI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32GreaterEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32GreaterEqualiInstr() : VpuInstr(OpCodeType::F32_GREATER_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32LessiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LessiInstr() : VpuInstr(OpCodeType::F32_LESSI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32LessEqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32LessEqualiInstr() : VpuInstr(OpCodeType::F32_LESS_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 12;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
};

class F32EqualiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F32EqualiInstr() : VpuInstr(OpCodeType::F32_EQUALI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    hasImm = 1;
    dtype = 12;
    alu = 1;
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
    hasImm = 1;
    dtype = 12;
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
    alu = 1;
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
    alu = 1;
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
    alu = 1;
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
    alu = 1;
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
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
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

class I32DloadAndInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadAndInstr() : VpuInstr(OpCodeType::I32_DLOAD_AND), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_AND) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadOrInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadOrInstr() : VpuInstr(OpCodeType::I32_DLOAD_OR), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_OR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadNotInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadNotInstr() : VpuInstr(OpCodeType::I32_DLOAD_NOT), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_NOT) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadXorInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadXorInstr() : VpuInstr(OpCodeType::I32_DLOAD_XOR), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_XOR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32DloadXnorInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I32DloadXnorInstr() : VpuInstr(OpCodeType::I32_DLOAD_XNOR), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_XNOR) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I32LoadAndInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadAndInstr() : VpuInstr(OpCodeType::I32_LOAD_AND), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::I32_AND) {
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

class I32LoadOrInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadOrInstr() : VpuInstr(OpCodeType::I32_LOAD_OR), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::I32_OR) {
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

class I32LoadNotInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadNotInstr() : VpuInstr(OpCodeType::I32_LOAD_NOT), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::I32_NOT) {
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

class I32LoadXorInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadXorInstr() : VpuInstr(OpCodeType::I32_LOAD_XOR), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::I32_XOR) {
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

class I32LoadXnorInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I32LoadXnorInstr() : VpuInstr(OpCodeType::I32_LOAD_XNOR), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::I32_XNOR) {
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

class I32AndStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32AndStoreInstr() : VpuInstr(OpCodeType::I32_AND_STORE), ComputeStoreInstr(OpCodeType::I32_AND, OpCodeType::U8_STORE) {
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

class I32OrStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32OrStoreInstr() : VpuInstr(OpCodeType::I32_OR_STORE), ComputeStoreInstr(OpCodeType::I32_OR, OpCodeType::U8_STORE) {
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

class I32NotStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32NotStoreInstr() : VpuInstr(OpCodeType::I32_NOT_STORE), ComputeStoreInstr(OpCodeType::I32_NOT, OpCodeType::U8_STORE) {
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

class I32XorStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32XorStoreInstr() : VpuInstr(OpCodeType::I32_XOR_STORE), ComputeStoreInstr(OpCodeType::I32_XOR, OpCodeType::U8_STORE) {
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

class I32XnorStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I32XnorStoreInstr() : VpuInstr(OpCodeType::I32_XNOR_STORE), ComputeStoreInstr(OpCodeType::I32_XNOR, OpCodeType::U8_STORE) {
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

class I32DloadAndStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadAndStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_AND_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_AND, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadOrStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadOrStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_OR_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_OR, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadNotStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadNotStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_NOT_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_NOT, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadXorStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadXorStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_XOR_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_XOR, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32DloadXnorStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I32DloadXnorStoreInstr() : VpuInstr(OpCodeType::I32_DLOAD_XNOR_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::I32_XNOR, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I32LoadAndStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadAndStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_AND_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::I32_AND, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadOrStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadOrStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_OR_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::I32_OR, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadNotStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadNotStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_NOT_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::I32_NOT, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadXorStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadXorStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_XOR_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::I32_XOR, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I32LoadXnorStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I32LoadXnorStoreInstr() : VpuInstr(OpCodeType::I32_LOAD_XNOR_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::I32_XNOR, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    dtype = 5;
    alu = 1;
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
    case OpCodeType::SET_LOAD_AAI:         ret = std::shared_ptr<SetLoadAaiInstr>(new SetLoadAaiInstr); break;
    case OpCodeType::SET_STORE_AAI:        ret = std::shared_ptr<SetStoreAaiInstr>(new SetStoreAaiInstr); break;
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
    case OpCodeType::I8_STORE:             ret = std::shared_ptr<I8StoreInstr>(new I8StoreInstr); break;
    case OpCodeType::U8_STORE:             ret = std::shared_ptr<U8StoreInstr>(new U8StoreInstr); break;
    case OpCodeType::F16_STORE:            ret = std::shared_ptr<F16StoreInstr>(new F16StoreInstr); break;
    case OpCodeType::F32_STORE:            ret = std::shared_ptr<F32StoreInstr>(new F32StoreInstr); break;
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
    case OpCodeType::F32_ADDI:             ret = std::shared_ptr<F32AddiInstr>(new F32AddiInstr); break;
    case OpCodeType::F32_SUBI:             ret = std::shared_ptr<F32SubiInstr>(new F32SubiInstr); break;
    case OpCodeType::F32_MULI:             ret = std::shared_ptr<F32MuliInstr>(new F32MuliInstr); break;
    case OpCodeType::F32_MAXI:             ret = std::shared_ptr<F32MaxiInstr>(new F32MaxiInstr); break;
    case OpCodeType::F32_MINI:             ret = std::shared_ptr<F32MiniInstr>(new F32MiniInstr); break;
    case OpCodeType::F32_MOVI:             ret = std::shared_ptr<F32MoviInstr>(new F32MoviInstr); break;
    case OpCodeType::F32_GREATERI:         ret = std::shared_ptr<F32GreateriInstr>(new F32GreateriInstr); break;
    case OpCodeType::F32_GREATER_EQUALI:   ret = std::shared_ptr<F32GreaterEqualiInstr>(new F32GreaterEqualiInstr); break;
    case OpCodeType::F32_LESSI:            ret = std::shared_ptr<F32LessiInstr>(new F32LessiInstr); break;
    case OpCodeType::F32_LESS_EQUALI:      ret = std::shared_ptr<F32LessEqualiInstr>(new F32LessEqualiInstr); break;
    case OpCodeType::F32_EQUALI:           ret = std::shared_ptr<F32EqualiInstr>(new F32EqualiInstr); break;
    case OpCodeType::F32_SELI:             ret = std::shared_ptr<F32SeliInstr>(new F32SeliInstr); break;
    case OpCodeType::F32_LUI:              ret = std::shared_ptr<F32LuiInstr>(new F32LuiInstr); break;
    case OpCodeType::F32_LI:               ret = std::shared_ptr<F32LiInstr>(new F32LiInstr); break;
    case OpCodeType::I32_ANDI:             ret = std::shared_ptr<I32AndiInstr>(new I32AndiInstr); break;
    case OpCodeType::I32_ORI:              ret = std::shared_ptr<I32OriInstr>(new I32OriInstr); break;
    case OpCodeType::I32_NOTI:             ret = std::shared_ptr<I32NotiInstr>(new I32NotiInstr); break;
    case OpCodeType::I32_XORI:             ret = std::shared_ptr<I32XoriInstr>(new I32XoriInstr); break;
    case OpCodeType::I32_XNORI:            ret = std::shared_ptr<I32XnoriInstr>(new I32XnoriInstr); break;
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
    case OpCodeType::I32_DLOAD_AND:        ret = std::shared_ptr<I32DloadAndInstr>(new I32DloadAndInstr); break;
    case OpCodeType::I32_DLOAD_OR:         ret = std::shared_ptr<I32DloadOrInstr>(new I32DloadOrInstr); break;
    case OpCodeType::I32_DLOAD_NOT:        ret = std::shared_ptr<I32DloadNotInstr>(new I32DloadNotInstr); break;
    case OpCodeType::I32_DLOAD_XOR:        ret = std::shared_ptr<I32DloadXorInstr>(new I32DloadXorInstr); break;
    case OpCodeType::I32_DLOAD_XNOR:       ret = std::shared_ptr<I32DloadXnorInstr>(new I32DloadXnorInstr); break;
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
    case OpCodeType::I32_DLOAD_AND_STORE:  ret = std::shared_ptr<I32DloadAndStoreInstr>(new I32DloadAndStoreInstr); break;
    case OpCodeType::I32_DLOAD_OR_STORE:   ret = std::shared_ptr<I32DloadOrStoreInstr>(new I32DloadOrStoreInstr); break;
    case OpCodeType::I32_DLOAD_NOT_STORE:  ret = std::shared_ptr<I32DloadNotStoreInstr>(new I32DloadNotStoreInstr); break;
    case OpCodeType::I32_DLOAD_XOR_STORE:  ret = std::shared_ptr<I32DloadXorStoreInstr>(new I32DloadXorStoreInstr); break;
    case OpCodeType::I32_DLOAD_XNOR_STORE: ret = std::shared_ptr<I32DloadXnorStoreInstr>(new I32DloadXnorStoreInstr); break;
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
