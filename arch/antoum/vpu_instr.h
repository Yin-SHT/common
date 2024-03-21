/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */
#ifndef ANTOUM_VPU_INSTR_H
#define ANTOUM_VPU_INSTR_H

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

namespace antoum {

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
    VPU_NOP = 8,
    VPU_OP_END = 9,
    VPU_FLOAD_GLB = 10,
    VPU_FLOAD_UPPER_GLB = 11,
    VPU_FLOAD_CONST = 12,
    VPU_FLOAD_DUAL_GLB = 13,
    VPU_FLOAD_DUAL_UPPER_GLB = 14,
    VPU_FSTORE_GLB = 15,
    VPU_FSTORE_UPPER_GLB = 16,
    VPU_I16STORE_GLB = 17,
    VPU_I16STORE_UPPER_GLB = 18,
    VPU_LOAD_GLB = 19,
    VPU_LOAD_CONST = 20,
    VPU_LOAD_DUAL_GLB = 21,
    VPU_STORE_GLB = 22,
    VPU_ULOAD_GLB = 23,
    VPU_ULOAD_CONST = 24,
    VPU_ULOAD_DUAL_GLB = 25,
    VPU_USTORE_GLB = 26,
    VPU_FADD = 27,
    VPU_FSUB = 28,
    VPU_FMUL = 29,
    VPU_FMAX = 30,
    VPU_FMIN = 31,
    VPU_GREATER = 32,
    VPU_GREATER_EQUAL = 33,
    VPU_LESS = 34,
    VPU_LESS_EQUAL = 35,
    VPU_EQUAL = 36,
    VPU_SEL = 37,
    VPU_FADDI = 38,
    VPU_FSUBI = 39,
    VPU_FMULI = 40,
    VPU_FMAXI = 41,
    VPU_FMINI = 42,
    VPU_FMOVI = 43,
    VPU_GREATERI = 44,
    VPU_GREATER_EQUALI = 45,
    VPU_LESSI = 46,
    VPU_LESS_EQUALI = 47,
    VPU_EQUALI = 48,
    VPU_SELI = 49,
    VPU_FREDUCE_SUM = 50,
    VPU_FLOAD_DUAL_FADD = 51,
    VPU_FLOAD_DUAL_FSUB = 52,
    VPU_FLOAD_DUAL_FMUL = 53,
    VPU_FLOAD_DUAL_FMAX = 54,
    VPU_FLOAD_DUAL_FMIN = 55,
    VPU_FLOAD_FADD = 56,
    VPU_FLOAD_FSUB = 57,
    VPU_FLOAD_FMUL = 58,
    VPU_FLOAD_FMAX = 59,
    VPU_FLOAD_FMIN = 60,
    VPU_FADD_FSTORE = 61,
    VPU_FSUB_FSTORE = 62,
    VPU_FMUL_FSTORE = 63,
    VPU_FMAX_FSTORE = 64,
    VPU_FMIN_FSTORE = 65,
    VPU_FLOAD_DUAL_FADD_FSTORE = 66,
    VPU_FLOAD_DUAL_FSUB_FSTORE = 67,
    VPU_FLOAD_DUAL_FMUL_FSTORE = 68,
    VPU_FLOAD_DUAL_FMAX_FSTORE = 69,
    VPU_FLOAD_DUAL_FMIN_FSTORE = 70,
    VPU_FLOAD_FADD_FSTORE = 71,
    VPU_FLOAD_FSUB_FSTORE = 72,
    VPU_FLOAD_FMUL_FSTORE = 73,
    VPU_FLOAD_FMAX_FSTORE = 74,
    VPU_FLOAD_FMIN_FSTORE = 75,
    VPU_LOAD_DUAL_FADD = 76,
    VPU_LOAD_DUAL_FSUB = 77,
    VPU_LOAD_DUAL_FMUL = 78,
    VPU_LOAD_DUAL_FMAX = 79,
    VPU_LOAD_DUAL_FMIN = 80,
    VPU_LOAD_FADD = 81,
    VPU_LOAD_FSUB = 82,
    VPU_LOAD_FMUL = 83,
    VPU_LOAD_FMAX = 84,
    VPU_LOAD_FMIN = 85,
    VPU_FADD_STORE = 86,
    VPU_FSUB_STORE = 87,
    VPU_FMUL_STORE = 88,
    VPU_FMAX_STORE = 89,
    VPU_FMIN_STORE = 90,
    VPU_LOAD_DUAL_FADD_STORE = 91,
    VPU_LOAD_DUAL_FSUB_STORE = 92,
    VPU_LOAD_DUAL_FMUL_STORE = 93,
    VPU_LOAD_DUAL_FMAX_STORE = 94,
    VPU_LOAD_DUAL_FMIN_STORE = 95,
    VPU_LOAD_FADD_STORE = 96,
    VPU_LOAD_FSUB_STORE = 97,
    VPU_LOAD_FMUL_STORE = 98,
    VPU_LOAD_FMAX_STORE = 99,
    VPU_LOAD_FMIN_STORE = 100,
    VPU_ULOAD_DUAL_FADD = 101,
    VPU_ULOAD_DUAL_FSUB = 102,
    VPU_ULOAD_DUAL_FMUL = 103,
    VPU_ULOAD_DUAL_FMAX = 104,
    VPU_ULOAD_DUAL_FMIN = 105,
    VPU_ULOAD_FADD = 106,
    VPU_ULOAD_FSUB = 107,
    VPU_ULOAD_FMUL = 108,
    VPU_ULOAD_FMAX = 109,
    VPU_ULOAD_FMIN = 110,
    VPU_FADD_USTORE = 111,
    VPU_FSUB_USTORE = 112,
    VPU_FMUL_USTORE = 113,
    VPU_FMAX_USTORE = 114,
    VPU_FMIN_USTORE = 115,
    VPU_ULOAD_DUAL_FADD_USTORE = 116,
    VPU_ULOAD_DUAL_FSUB_USTORE = 117,
    VPU_ULOAD_DUAL_FMUL_USTORE = 118,
    VPU_ULOAD_DUAL_FMAX_USTORE = 119,
    VPU_ULOAD_DUAL_FMIN_USTORE = 120,
    VPU_ULOAD_FADD_USTORE = 121,
    VPU_ULOAD_FSUB_USTORE = 122,
    VPU_ULOAD_FMUL_USTORE = 123,
    VPU_ULOAD_FMAX_USTORE = 124,
    VPU_ULOAD_FMIN_USTORE = 125,
    INVALID = 65536
  };

  static const std::map<OpCodeType, OpCodeItem>& getOpCodeMap() {
    static const std::map<OpCodeType, OpCodeItem> opCodeMap = {
      {OpCodeType::VPU_SET_LOAD_BASE,          {"vpu_set_load_base",          0xff000000, 0x01000000}},
      {OpCodeType::VPU_SET_LOAD2_BASE,         {"vpu_set_load2_base",         0xff000000, 0x04000000}},
      {OpCodeType::VPU_SET_STORE_BASE,         {"vpu_set_store_base",         0xff000000, 0x07000000}},
      {OpCodeType::VPU_SET_LOAD_GLB,           {"vpu_set_load_glb",           0xff00000f, 0x02000000}},
      {OpCodeType::VPU_SET_STORE_GLB,          {"vpu_set_store_glb",          0xff0001ff, 0x08000000}},
      {OpCodeType::VPU_SET_QUANTIZE,           {"vpu_set_quantize",           0xffff0000, 0x09000000}},
      {OpCodeType::VPU_SET_DEQUANTIZE,         {"vpu_set_dequantize",         0xffff0000, 0x03000000}},
      {OpCodeType::VPU_SET_DEQUANTIZE2,        {"vpu_set_dequantize2",        0xffff0000, 0x06000000}},
      {OpCodeType::VPU_NOP,                    {"vpu_nop",                    0xffffffff, 0x00000000}},
      {OpCodeType::VPU_OP_END,                 {"vpu_op_end",                 0xffffffff, 0xff000000}},
      {OpCodeType::VPU_FLOAD_GLB,              {"vpu_fload_glb",              0xffcf0000, 0xe0000000}},
      {OpCodeType::VPU_FLOAD_UPPER_GLB,        {"vpu_fload_upper_glb",        0xffcf0000, 0xe1000000}},
      {OpCodeType::VPU_FLOAD_CONST,            {"vpu_fload_const",            0xffcf0000, 0xe0040000}},
      {OpCodeType::VPU_FLOAD_DUAL_GLB,         {"vpu_fload_dual_glb",         0xff0f0000, 0xe0020000}},
      {OpCodeType::VPU_FLOAD_DUAL_UPPER_GLB,   {"vpu_fload_dual_upper_glb",   0xff0f0000, 0xe1020000}},
      {OpCodeType::VPU_FSTORE_GLB,             {"vpu_fstore_glb",             0xffcf0000, 0xe8000000}},
      {OpCodeType::VPU_FSTORE_UPPER_GLB,       {"vpu_fstore_upper_glb",       0xffcf0000, 0xe9000000}},
      {OpCodeType::VPU_I16STORE_GLB,           {"vpu_i16store_glb",           0xffcf0000, 0xca000000}},
      {OpCodeType::VPU_I16STORE_UPPER_GLB,     {"vpu_i16store_upper_glb",     0xffcf0000, 0xcb000000}},
      {OpCodeType::VPU_LOAD_GLB,               {"vpu_load_glb",               0xffcf0000, 0xc0000000}},
      {OpCodeType::VPU_LOAD_CONST,             {"vpu_load_const",             0xffcf0000, 0xc0040000}},
      {OpCodeType::VPU_LOAD_DUAL_GLB,          {"vpu_load_dual_glb",          0xff0f0000, 0xc0020000}},
      {OpCodeType::VPU_STORE_GLB,              {"vpu_store_glb",              0xffcf0000, 0xc8000000}},
      {OpCodeType::VPU_ULOAD_GLB,              {"vpu_uload_glb",              0xffcf0000, 0xd0000000}},
      {OpCodeType::VPU_ULOAD_CONST,            {"vpu_uload_const",            0xffcf0000, 0xd0040000}},
      {OpCodeType::VPU_ULOAD_DUAL_GLB,         {"vpu_uload_dual_glb",         0xff0f0000, 0xd0020000}},
      {OpCodeType::VPU_USTORE_GLB,             {"vpu_ustore_glb",             0xffcf0000, 0xd8000000}},
      {OpCodeType::VPU_FADD,                   {"vpu_fadd",                   0xff007fff, 0x80000000}},
      {OpCodeType::VPU_FSUB,                   {"vpu_fsub",                   0xff007fff, 0x81000000}},
      {OpCodeType::VPU_FMUL,                   {"vpu_fmul",                   0xff007fff, 0x82000000}},
      {OpCodeType::VPU_FMAX,                   {"vpu_fmax",                   0xff007fff, 0x83000000}},
      {OpCodeType::VPU_FMIN,                   {"vpu_fmin",                   0xff007fff, 0x84000000}},
      {OpCodeType::VPU_GREATER,                {"vpu_greater",                0xffe07fff, 0x87000000}},
      {OpCodeType::VPU_GREATER_EQUAL,          {"vpu_greater_equal",          0xffe07fff, 0x8a000000}},
      {OpCodeType::VPU_LESS,                   {"vpu_less",                   0xffe07fff, 0x89000000}},
      {OpCodeType::VPU_LESS_EQUAL,             {"vpu_less_equal",             0xffe07fff, 0x8b000000}},
      {OpCodeType::VPU_EQUAL,                  {"vpu_equal",                  0xffe07fff, 0x88000000}},
      {OpCodeType::VPU_SEL,                    {"vpu_sel",                    0xff007fff, 0x8c000000}},
      {OpCodeType::VPU_FADDI,                  {"vpu_faddi",                  0xff030000, 0xa0000000}},
      {OpCodeType::VPU_FSUBI,                  {"vpu_fsubi",                  0xff030000, 0xa1000000}},
      {OpCodeType::VPU_FMULI,                  {"vpu_fmuli",                  0xff030000, 0xa2000000}},
      {OpCodeType::VPU_FMAXI,                  {"vpu_fmaxi",                  0xff030000, 0xa3000000}},
      {OpCodeType::VPU_FMINI,                  {"vpu_fmini",                  0xff030000, 0xa4000000}},
      {OpCodeType::VPU_FMOVI,                  {"vpu_fmovi",                  0xff1f0000, 0xa5000000}},
      {OpCodeType::VPU_GREATERI,               {"vpu_greateri",               0xffe30000, 0xa7000000}},
      {OpCodeType::VPU_GREATER_EQUALI,         {"vpu_greater_equali",         0xffe30000, 0xaa000000}},
      {OpCodeType::VPU_LESSI,                  {"vpu_lessi",                  0xffe30000, 0xa9000000}},
      {OpCodeType::VPU_LESS_EQUALI,            {"vpu_less_equali",            0xffe30000, 0xab000000}},
      {OpCodeType::VPU_EQUALI,                 {"vpu_equali",                 0xffe30000, 0xa8000000}},
      {OpCodeType::VPU_SELI,                   {"vpu_seli",                   0xff030000, 0xac000000}},
      {OpCodeType::VPU_FREDUCE_SUM,            {"vpu_freduce_sum",            0xfffc7fff, 0x86000000}},
      {OpCodeType::VPU_FLOAD_DUAL_FADD,        {"vpu_fload_dual_fadd",        0xff3f0000, 0xe0030000}},
      {OpCodeType::VPU_FLOAD_DUAL_FSUB,        {"vpu_fload_dual_fsub",        0xff3f0000, 0xe1030000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMUL,        {"vpu_fload_dual_fmul",        0xff3f0000, 0xe2030000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMAX,        {"vpu_fload_dual_fmax",        0xff3f0000, 0xe3030000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMIN,        {"vpu_fload_dual_fmin",        0xff3f0000, 0xe4030000}},
      {OpCodeType::VPU_FLOAD_FADD,             {"vpu_fload_fadd",             0xff0f0000, 0xe0010000}},
      {OpCodeType::VPU_FLOAD_FSUB,             {"vpu_fload_fsub",             0xff0f0000, 0xe1010000}},
      {OpCodeType::VPU_FLOAD_FMUL,             {"vpu_fload_fmul",             0xff0f0000, 0xe2010000}},
      {OpCodeType::VPU_FLOAD_FMAX,             {"vpu_fload_fmax",             0xff0f0000, 0xe3010000}},
      {OpCodeType::VPU_FLOAD_FMIN,             {"vpu_fload_fmin",             0xff0f0000, 0xe4010000}},
      {OpCodeType::VPU_FADD_FSTORE,            {"vpu_fadd_fstore",            0xff0f0000, 0xe8010000}},
      {OpCodeType::VPU_FSUB_FSTORE,            {"vpu_fsub_fstore",            0xff0f0000, 0xe9010000}},
      {OpCodeType::VPU_FMUL_FSTORE,            {"vpu_fmul_fstore",            0xff0f0000, 0xea010000}},
      {OpCodeType::VPU_FMAX_FSTORE,            {"vpu_fmax_fstore",            0xff0f0000, 0xeb010000}},
      {OpCodeType::VPU_FMIN_FSTORE,            {"vpu_fmin_fstore",            0xff0f0000, 0xec010000}},
      {OpCodeType::VPU_FLOAD_DUAL_FADD_FSTORE, {"vpu_fload_dual_fadd_fstore", 0xffff0000, 0xe00a0000}},
      {OpCodeType::VPU_FLOAD_DUAL_FSUB_FSTORE, {"vpu_fload_dual_fsub_fstore", 0xffff0000, 0xe10a0000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMUL_FSTORE, {"vpu_fload_dual_fmul_fstore", 0xffff0000, 0xe20a0000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMAX_FSTORE, {"vpu_fload_dual_fmax_fstore", 0xffff0000, 0xe30a0000}},
      {OpCodeType::VPU_FLOAD_DUAL_FMIN_FSTORE, {"vpu_fload_dual_fmin_fstore", 0xffff0000, 0xe40a0000}},
      {OpCodeType::VPU_FLOAD_FADD_FSTORE,      {"vpu_fload_fadd_fstore",      0xffcf0000, 0xe0080000}},
      {OpCodeType::VPU_FLOAD_FSUB_FSTORE,      {"vpu_fload_fsub_fstore",      0xffcf0000, 0xe1080000}},
      {OpCodeType::VPU_FLOAD_FMUL_FSTORE,      {"vpu_fload_fmul_fstore",      0xffcf0000, 0xe2080000}},
      {OpCodeType::VPU_FLOAD_FMAX_FSTORE,      {"vpu_fload_fmax_fstore",      0xffcf0000, 0xe3080000}},
      {OpCodeType::VPU_FLOAD_FMIN_FSTORE,      {"vpu_fload_fmin_fstore",      0xffcf0000, 0xe4080000}},
      {OpCodeType::VPU_LOAD_DUAL_FADD,         {"vpu_load_dual_fadd",         0xff3f0000, 0xc0030000}},
      {OpCodeType::VPU_LOAD_DUAL_FSUB,         {"vpu_load_dual_fsub",         0xff3f0000, 0xc1030000}},
      {OpCodeType::VPU_LOAD_DUAL_FMUL,         {"vpu_load_dual_fmul",         0xff3f0000, 0xc2030000}},
      {OpCodeType::VPU_LOAD_DUAL_FMAX,         {"vpu_load_dual_fmax",         0xff3f0000, 0xc3030000}},
      {OpCodeType::VPU_LOAD_DUAL_FMIN,         {"vpu_load_dual_fmin",         0xff3f0000, 0xc4030000}},
      {OpCodeType::VPU_LOAD_FADD,              {"vpu_load_fadd",              0xff0f0000, 0xc0010000}},
      {OpCodeType::VPU_LOAD_FSUB,              {"vpu_load_fsub",              0xff0f0000, 0xc1010000}},
      {OpCodeType::VPU_LOAD_FMUL,              {"vpu_load_fmul",              0xff0f0000, 0xc2010000}},
      {OpCodeType::VPU_LOAD_FMAX,              {"vpu_load_fmax",              0xff0f0000, 0xc3010000}},
      {OpCodeType::VPU_LOAD_FMIN,              {"vpu_load_fmin",              0xff0f0000, 0xc4010000}},
      {OpCodeType::VPU_FADD_STORE,             {"vpu_fadd_store",             0xff0f0000, 0xc8010000}},
      {OpCodeType::VPU_FSUB_STORE,             {"vpu_fsub_store",             0xff0f0000, 0xc9010000}},
      {OpCodeType::VPU_FMUL_STORE,             {"vpu_fmul_store",             0xff0f0000, 0xca010000}},
      {OpCodeType::VPU_FMAX_STORE,             {"vpu_fmax_store",             0xff0f0000, 0xcb010000}},
      {OpCodeType::VPU_FMIN_STORE,             {"vpu_fmin_store",             0xff0f0000, 0xcc010000}},
      {OpCodeType::VPU_LOAD_DUAL_FADD_STORE,   {"vpu_load_dual_fadd_store",   0xffff0000, 0xc00a0000}},
      {OpCodeType::VPU_LOAD_DUAL_FSUB_STORE,   {"vpu_load_dual_fsub_store",   0xffff0000, 0xc10a0000}},
      {OpCodeType::VPU_LOAD_DUAL_FMUL_STORE,   {"vpu_load_dual_fmul_store",   0xffff0000, 0xc20a0000}},
      {OpCodeType::VPU_LOAD_DUAL_FMAX_STORE,   {"vpu_load_dual_fmax_store",   0xffff0000, 0xc30a0000}},
      {OpCodeType::VPU_LOAD_DUAL_FMIN_STORE,   {"vpu_load_dual_fmin_store",   0xffff0000, 0xc40a0000}},
      {OpCodeType::VPU_LOAD_FADD_STORE,        {"vpu_load_fadd_store",        0xffcf0000, 0xc0080000}},
      {OpCodeType::VPU_LOAD_FSUB_STORE,        {"vpu_load_fsub_store",        0xffcf0000, 0xc1080000}},
      {OpCodeType::VPU_LOAD_FMUL_STORE,        {"vpu_load_fmul_store",        0xffcf0000, 0xc2080000}},
      {OpCodeType::VPU_LOAD_FMAX_STORE,        {"vpu_load_fmax_store",        0xffcf0000, 0xc3080000}},
      {OpCodeType::VPU_LOAD_FMIN_STORE,        {"vpu_load_fmin_store",        0xffcf0000, 0xc4080000}},
      {OpCodeType::VPU_ULOAD_DUAL_FADD,        {"vpu_uload_dual_fadd",        0xff3f0000, 0xd0030000}},
      {OpCodeType::VPU_ULOAD_DUAL_FSUB,        {"vpu_uload_dual_fsub",        0xff3f0000, 0xd1030000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMUL,        {"vpu_uload_dual_fmul",        0xff3f0000, 0xd2030000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMAX,        {"vpu_uload_dual_fmax",        0xff3f0000, 0xd3030000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMIN,        {"vpu_uload_dual_fmin",        0xff3f0000, 0xd4030000}},
      {OpCodeType::VPU_ULOAD_FADD,             {"vpu_uload_fadd",             0xff0f0000, 0xd0010000}},
      {OpCodeType::VPU_ULOAD_FSUB,             {"vpu_uload_fsub",             0xff0f0000, 0xd1010000}},
      {OpCodeType::VPU_ULOAD_FMUL,             {"vpu_uload_fmul",             0xff0f0000, 0xd2010000}},
      {OpCodeType::VPU_ULOAD_FMAX,             {"vpu_uload_fmax",             0xff0f0000, 0xd3010000}},
      {OpCodeType::VPU_ULOAD_FMIN,             {"vpu_uload_fmin",             0xff0f0000, 0xd4010000}},
      {OpCodeType::VPU_FADD_USTORE,            {"vpu_fadd_ustore",            0xff0f0000, 0xd8010000}},
      {OpCodeType::VPU_FSUB_USTORE,            {"vpu_fsub_ustore",            0xff0f0000, 0xd9010000}},
      {OpCodeType::VPU_FMUL_USTORE,            {"vpu_fmul_ustore",            0xff0f0000, 0xda010000}},
      {OpCodeType::VPU_FMAX_USTORE,            {"vpu_fmax_ustore",            0xff0f0000, 0xdb010000}},
      {OpCodeType::VPU_FMIN_USTORE,            {"vpu_fmin_ustore",            0xff0f0000, 0xdc010000}},
      {OpCodeType::VPU_ULOAD_DUAL_FADD_USTORE, {"vpu_uload_dual_fadd_ustore", 0xffff0000, 0xd00a0000}},
      {OpCodeType::VPU_ULOAD_DUAL_FSUB_USTORE, {"vpu_uload_dual_fsub_ustore", 0xffff0000, 0xd10a0000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMUL_USTORE, {"vpu_uload_dual_fmul_ustore", 0xffff0000, 0xd20a0000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMAX_USTORE, {"vpu_uload_dual_fmax_ustore", 0xffff0000, 0xd30a0000}},
      {OpCodeType::VPU_ULOAD_DUAL_FMIN_USTORE, {"vpu_uload_dual_fmin_ustore", 0xffff0000, 0xd40a0000}},
      {OpCodeType::VPU_ULOAD_FADD_USTORE,      {"vpu_uload_fadd_ustore",      0xffcf0000, 0xd0080000}},
      {OpCodeType::VPU_ULOAD_FSUB_USTORE,      {"vpu_uload_fsub_ustore",      0xffcf0000, 0xd1080000}},
      {OpCodeType::VPU_ULOAD_FMUL_USTORE,      {"vpu_uload_fmul_ustore",      0xffcf0000, 0xd2080000}},
      {OpCodeType::VPU_ULOAD_FMAX_USTORE,      {"vpu_uload_fmax_ustore",      0xffcf0000, 0xd3080000}},
      {OpCodeType::VPU_ULOAD_FMIN_USTORE,      {"vpu_uload_fmin_ustore",      0xffcf0000, 0xd4080000}},
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
  uint32_t nBankBroadcast = 0;
  uint32_t nBankMask = 0;
  uint32_t nBaseAddr = 0;
  uint32_t nClusterBroadcast = 0;
  uint32_t nClusterMask = 0;
  uint32_t nDequantize = 0;
  uint32_t nHasImm = 0;
  uint32_t nImm = 0;
  uint32_t nIsBf16 = 0;
  uint32_t nIsInt16 = 0;
  uint32_t nIsUnsigned = 0;
  uint32_t nOffset = 0;
  uint32_t nQuantize = 0;
  uint32_t nRd = 0;
  uint32_t nRs = 0;
  uint32_t nRt = 0;
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
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 13);
  VPU_GEN_GETTER_SETTER(BankMask, 12, 9);
  VPU_GEN_GETTER_SETTER(ClusterBroadcast, 8, 7);
  VPU_GEN_GETTER_SETTER(BankBroadcast, 6, 4);
};

class VpuSetStoreGlbInstr : public VpuInstr {
public:
  explicit VpuSetStoreGlbInstr() : VpuInstr(OpCodeType::VPU_SET_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Stride);
    VPU_PUSH_GETTER_SETTER(ClusterMask);
    VPU_PUSH_GETTER_SETTER(BankMask);
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 13);
  VPU_GEN_GETTER_SETTER(BankMask, 12, 9);
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
  VPU_GEN_GETTER_SETTER(Dequantize, 15, 0);
};

class VpuSetDequantize2Instr : public VpuInstr {
public:
  explicit VpuSetDequantize2Instr() : VpuInstr(OpCodeType::VPU_SET_DEQUANTIZE2) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 15, 0);
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

class VpuFloadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuFloadGlbInstr() : VpuInstr(OpCodeType::VPU_FLOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuFloadUpperGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuFloadUpperGlbInstr() : VpuInstr(OpCodeType::VPU_FLOAD_UPPER_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsBf16 = 1;
    nUpper = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuFloadConstInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuFloadConstInstr() : VpuInstr(OpCodeType::VPU_FLOAD_CONST) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsBf16 = 1;
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
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class VpuFloadDualUpperGlbInstr : public VpuInstr, public DualLoadInstr {
public:
  explicit VpuFloadDualUpperGlbInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_UPPER_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    nIsBf16 = 1;
    nUpper = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class VpuFstoreGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuFstoreGlbInstr() : VpuInstr(OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFstoreUpperGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuFstoreUpperGlbInstr() : VpuInstr(OpCodeType::VPU_FSTORE_UPPER_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
    nUpper = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuI16storeGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuI16storeGlbInstr() : VpuInstr(OpCodeType::VPU_I16STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsInt16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuI16storeUpperGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuI16storeUpperGlbInstr() : VpuInstr(OpCodeType::VPU_I16STORE_UPPER_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsInt16 = 1;
    nUpper = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
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

class VpuLoadConstInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuLoadConstInstr() : VpuInstr(OpCodeType::VPU_LOAD_CONST) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
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

class VpuStoreGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuStoreGlbInstr() : VpuInstr(OpCodeType::VPU_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadGlbInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuUloadGlbInstr() : VpuInstr(OpCodeType::VPU_ULOAD_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadConstInstr : public VpuInstr, public LoadInstr {
public:
  explicit VpuUloadConstInstr() : VpuInstr(OpCodeType::VPU_ULOAD_CONST) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class VpuUstoreGlbInstr : public VpuInstr, public StoreInstr {
public:
  explicit VpuUstoreGlbInstr() : VpuInstr(OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsUnsigned = 1;
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
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuGreaterInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuGreaterInstr() : VpuInstr(OpCodeType::VPU_GREATER) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuGreaterEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuGreaterEqualInstr() : VpuInstr(OpCodeType::VPU_GREATER_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuLessInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuLessInstr() : VpuInstr(OpCodeType::VPU_LESS) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuLessEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuLessEqualInstr() : VpuInstr(OpCodeType::VPU_LESS_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuEqualInstr() : VpuInstr(OpCodeType::VPU_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
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
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class VpuFreduceSumInstr : public VpuInstr, public VectorInstr {
public:
  explicit VpuFreduceSumInstr() : VpuInstr(OpCodeType::VPU_FREDUCE_SUM) {
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class VpuFloadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFaddInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFsubInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFmulInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFmaxInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuFloadDualFminInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadFaddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFaddInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FADD), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadFsubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFsubInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FSUB), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadFmulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFmulInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMUL), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadFmaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFmaxInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMAX), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFloadFminInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit VpuFloadFminInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMIN), LoadComputeInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFaddFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFaddFstoreInstr() : VpuInstr(OpCodeType::VPU_FADD_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FADD, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFsubFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFsubFstoreInstr() : VpuInstr(OpCodeType::VPU_FSUB_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FSUB, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFmulFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmulFstoreInstr() : VpuInstr(OpCodeType::VPU_FMUL_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FMUL, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFmaxFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFmaxFstoreInstr() : VpuInstr(OpCodeType::VPU_FMAX_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FMAX, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFminFstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFminFstoreInstr() : VpuInstr(OpCodeType::VPU_FMIN_FSTORE), ComputeStoreInstr(OpCodeType::VPU_FMIN, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFaddFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFaddFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FADD_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFsubFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFsubFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FSUB_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFmulFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFmulFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMUL_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFmaxFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFmaxFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMAX_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadDualFminFstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuFloadDualFminFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_DUAL_FMIN_FSTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_FLOAD_DUAL_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuFloadFaddFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFaddFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FADD_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuFloadFsubFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFsubFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FSUB_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuFloadFmulFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFmulFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMUL_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuFloadFmaxFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFmaxFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMAX_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuFloadFminFstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuFloadFminFstoreInstr() : VpuInstr(OpCodeType::VPU_FLOAD_FMIN_FSTORE), LoadComputeStoreInstr(OpCodeType::VPU_FLOAD_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_FSTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
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

class VpuUloadDualFaddInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFaddInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FADD), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFsubInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFsubInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FSUB), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FSUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFmulInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFmulInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMUL), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFmaxInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFmaxInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMAX), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuUloadDualFminInstr : public VpuInstr, public DualLoadComputeInstr {
public:
  explicit VpuUloadDualFminInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMIN), DualLoadComputeInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class VpuFaddUstoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit VpuFaddUstoreInstr() : VpuInstr(OpCodeType::VPU_FADD_USTORE), ComputeStoreInstr(OpCodeType::VPU_FADD, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
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
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFaddUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFaddUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FADD_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFsubUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFsubUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FSUB_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFmulUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFmulUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMUL_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFmaxUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFmaxUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMAX_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadDualFminUstoreInstr : public VpuInstr, public DualLoadComputeStoreInstr {
public:
  explicit VpuUloadDualFminUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_DUAL_FMIN_USTORE), DualLoadComputeStoreInstr(OpCodeType::VPU_ULOAD_DUAL_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class VpuUloadFaddUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFaddUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FADD_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FADD, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadFsubUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFsubUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FSUB_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FSUB, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadFmulUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFmulUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMUL_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMUL, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadFmaxUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFmaxUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMAX_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMAX, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class VpuUloadFminUstoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit VpuUloadFminUstoreInstr() : VpuInstr(OpCodeType::VPU_ULOAD_FMIN_USTORE), LoadComputeStoreInstr(OpCodeType::VPU_ULOAD_GLB, OpCodeType::VPU_FMIN, OpCodeType::VPU_USTORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    nIsUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};


std::shared_ptr<VpuInstr> VpuInstr::create(OpCodeType opCode, const std::vector<size_t> &args) {
  std::shared_ptr<VpuInstr> ret = nullptr;
  switch (opCode) {
    case OpCodeType::VPU_SET_LOAD_BASE:          ret = std::shared_ptr<VpuSetLoadBaseInstr>(new VpuSetLoadBaseInstr); break;
    case OpCodeType::VPU_SET_LOAD2_BASE:         ret = std::shared_ptr<VpuSetLoad2BaseInstr>(new VpuSetLoad2BaseInstr); break;
    case OpCodeType::VPU_SET_STORE_BASE:         ret = std::shared_ptr<VpuSetStoreBaseInstr>(new VpuSetStoreBaseInstr); break;
    case OpCodeType::VPU_SET_LOAD_GLB:           ret = std::shared_ptr<VpuSetLoadGlbInstr>(new VpuSetLoadGlbInstr); break;
    case OpCodeType::VPU_SET_STORE_GLB:          ret = std::shared_ptr<VpuSetStoreGlbInstr>(new VpuSetStoreGlbInstr); break;
    case OpCodeType::VPU_SET_QUANTIZE:           ret = std::shared_ptr<VpuSetQuantizeInstr>(new VpuSetQuantizeInstr); break;
    case OpCodeType::VPU_SET_DEQUANTIZE:         ret = std::shared_ptr<VpuSetDequantizeInstr>(new VpuSetDequantizeInstr); break;
    case OpCodeType::VPU_SET_DEQUANTIZE2:        ret = std::shared_ptr<VpuSetDequantize2Instr>(new VpuSetDequantize2Instr); break;
    case OpCodeType::VPU_NOP:                    ret = std::shared_ptr<VpuNopInstr>(new VpuNopInstr); break;
    case OpCodeType::VPU_OP_END:                 ret = std::shared_ptr<VpuOpEndInstr>(new VpuOpEndInstr); break;
    case OpCodeType::VPU_FLOAD_GLB:              ret = std::shared_ptr<VpuFloadGlbInstr>(new VpuFloadGlbInstr); break;
    case OpCodeType::VPU_FLOAD_UPPER_GLB:        ret = std::shared_ptr<VpuFloadUpperGlbInstr>(new VpuFloadUpperGlbInstr); break;
    case OpCodeType::VPU_FLOAD_CONST:            ret = std::shared_ptr<VpuFloadConstInstr>(new VpuFloadConstInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_GLB:         ret = std::shared_ptr<VpuFloadDualGlbInstr>(new VpuFloadDualGlbInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_UPPER_GLB:   ret = std::shared_ptr<VpuFloadDualUpperGlbInstr>(new VpuFloadDualUpperGlbInstr); break;
    case OpCodeType::VPU_FSTORE_GLB:             ret = std::shared_ptr<VpuFstoreGlbInstr>(new VpuFstoreGlbInstr); break;
    case OpCodeType::VPU_FSTORE_UPPER_GLB:       ret = std::shared_ptr<VpuFstoreUpperGlbInstr>(new VpuFstoreUpperGlbInstr); break;
    case OpCodeType::VPU_I16STORE_GLB:           ret = std::shared_ptr<VpuI16storeGlbInstr>(new VpuI16storeGlbInstr); break;
    case OpCodeType::VPU_I16STORE_UPPER_GLB:     ret = std::shared_ptr<VpuI16storeUpperGlbInstr>(new VpuI16storeUpperGlbInstr); break;
    case OpCodeType::VPU_LOAD_GLB:               ret = std::shared_ptr<VpuLoadGlbInstr>(new VpuLoadGlbInstr); break;
    case OpCodeType::VPU_LOAD_CONST:             ret = std::shared_ptr<VpuLoadConstInstr>(new VpuLoadConstInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_GLB:          ret = std::shared_ptr<VpuLoadDualGlbInstr>(new VpuLoadDualGlbInstr); break;
    case OpCodeType::VPU_STORE_GLB:              ret = std::shared_ptr<VpuStoreGlbInstr>(new VpuStoreGlbInstr); break;
    case OpCodeType::VPU_ULOAD_GLB:              ret = std::shared_ptr<VpuUloadGlbInstr>(new VpuUloadGlbInstr); break;
    case OpCodeType::VPU_ULOAD_CONST:            ret = std::shared_ptr<VpuUloadConstInstr>(new VpuUloadConstInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_GLB:         ret = std::shared_ptr<VpuUloadDualGlbInstr>(new VpuUloadDualGlbInstr); break;
    case OpCodeType::VPU_USTORE_GLB:             ret = std::shared_ptr<VpuUstoreGlbInstr>(new VpuUstoreGlbInstr); break;
    case OpCodeType::VPU_FADD:                   ret = std::shared_ptr<VpuFaddInstr>(new VpuFaddInstr); break;
    case OpCodeType::VPU_FSUB:                   ret = std::shared_ptr<VpuFsubInstr>(new VpuFsubInstr); break;
    case OpCodeType::VPU_FMUL:                   ret = std::shared_ptr<VpuFmulInstr>(new VpuFmulInstr); break;
    case OpCodeType::VPU_FMAX:                   ret = std::shared_ptr<VpuFmaxInstr>(new VpuFmaxInstr); break;
    case OpCodeType::VPU_FMIN:                   ret = std::shared_ptr<VpuFminInstr>(new VpuFminInstr); break;
    case OpCodeType::VPU_GREATER:                ret = std::shared_ptr<VpuGreaterInstr>(new VpuGreaterInstr); break;
    case OpCodeType::VPU_GREATER_EQUAL:          ret = std::shared_ptr<VpuGreaterEqualInstr>(new VpuGreaterEqualInstr); break;
    case OpCodeType::VPU_LESS:                   ret = std::shared_ptr<VpuLessInstr>(new VpuLessInstr); break;
    case OpCodeType::VPU_LESS_EQUAL:             ret = std::shared_ptr<VpuLessEqualInstr>(new VpuLessEqualInstr); break;
    case OpCodeType::VPU_EQUAL:                  ret = std::shared_ptr<VpuEqualInstr>(new VpuEqualInstr); break;
    case OpCodeType::VPU_SEL:                    ret = std::shared_ptr<VpuSelInstr>(new VpuSelInstr); break;
    case OpCodeType::VPU_FADDI:                  ret = std::shared_ptr<VpuFaddiInstr>(new VpuFaddiInstr); break;
    case OpCodeType::VPU_FSUBI:                  ret = std::shared_ptr<VpuFsubiInstr>(new VpuFsubiInstr); break;
    case OpCodeType::VPU_FMULI:                  ret = std::shared_ptr<VpuFmuliInstr>(new VpuFmuliInstr); break;
    case OpCodeType::VPU_FMAXI:                  ret = std::shared_ptr<VpuFmaxiInstr>(new VpuFmaxiInstr); break;
    case OpCodeType::VPU_FMINI:                  ret = std::shared_ptr<VpuFminiInstr>(new VpuFminiInstr); break;
    case OpCodeType::VPU_FMOVI:                  ret = std::shared_ptr<VpuFmoviInstr>(new VpuFmoviInstr); break;
    case OpCodeType::VPU_GREATERI:               ret = std::shared_ptr<VpuGreateriInstr>(new VpuGreateriInstr); break;
    case OpCodeType::VPU_GREATER_EQUALI:         ret = std::shared_ptr<VpuGreaterEqualiInstr>(new VpuGreaterEqualiInstr); break;
    case OpCodeType::VPU_LESSI:                  ret = std::shared_ptr<VpuLessiInstr>(new VpuLessiInstr); break;
    case OpCodeType::VPU_LESS_EQUALI:            ret = std::shared_ptr<VpuLessEqualiInstr>(new VpuLessEqualiInstr); break;
    case OpCodeType::VPU_EQUALI:                 ret = std::shared_ptr<VpuEqualiInstr>(new VpuEqualiInstr); break;
    case OpCodeType::VPU_SELI:                   ret = std::shared_ptr<VpuSeliInstr>(new VpuSeliInstr); break;
    case OpCodeType::VPU_FREDUCE_SUM:            ret = std::shared_ptr<VpuFreduceSumInstr>(new VpuFreduceSumInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FADD:        ret = std::shared_ptr<VpuFloadDualFaddInstr>(new VpuFloadDualFaddInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FSUB:        ret = std::shared_ptr<VpuFloadDualFsubInstr>(new VpuFloadDualFsubInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMUL:        ret = std::shared_ptr<VpuFloadDualFmulInstr>(new VpuFloadDualFmulInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMAX:        ret = std::shared_ptr<VpuFloadDualFmaxInstr>(new VpuFloadDualFmaxInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMIN:        ret = std::shared_ptr<VpuFloadDualFminInstr>(new VpuFloadDualFminInstr); break;
    case OpCodeType::VPU_FLOAD_FADD:             ret = std::shared_ptr<VpuFloadFaddInstr>(new VpuFloadFaddInstr); break;
    case OpCodeType::VPU_FLOAD_FSUB:             ret = std::shared_ptr<VpuFloadFsubInstr>(new VpuFloadFsubInstr); break;
    case OpCodeType::VPU_FLOAD_FMUL:             ret = std::shared_ptr<VpuFloadFmulInstr>(new VpuFloadFmulInstr); break;
    case OpCodeType::VPU_FLOAD_FMAX:             ret = std::shared_ptr<VpuFloadFmaxInstr>(new VpuFloadFmaxInstr); break;
    case OpCodeType::VPU_FLOAD_FMIN:             ret = std::shared_ptr<VpuFloadFminInstr>(new VpuFloadFminInstr); break;
    case OpCodeType::VPU_FADD_FSTORE:            ret = std::shared_ptr<VpuFaddFstoreInstr>(new VpuFaddFstoreInstr); break;
    case OpCodeType::VPU_FSUB_FSTORE:            ret = std::shared_ptr<VpuFsubFstoreInstr>(new VpuFsubFstoreInstr); break;
    case OpCodeType::VPU_FMUL_FSTORE:            ret = std::shared_ptr<VpuFmulFstoreInstr>(new VpuFmulFstoreInstr); break;
    case OpCodeType::VPU_FMAX_FSTORE:            ret = std::shared_ptr<VpuFmaxFstoreInstr>(new VpuFmaxFstoreInstr); break;
    case OpCodeType::VPU_FMIN_FSTORE:            ret = std::shared_ptr<VpuFminFstoreInstr>(new VpuFminFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FADD_FSTORE: ret = std::shared_ptr<VpuFloadDualFaddFstoreInstr>(new VpuFloadDualFaddFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FSUB_FSTORE: ret = std::shared_ptr<VpuFloadDualFsubFstoreInstr>(new VpuFloadDualFsubFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMUL_FSTORE: ret = std::shared_ptr<VpuFloadDualFmulFstoreInstr>(new VpuFloadDualFmulFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMAX_FSTORE: ret = std::shared_ptr<VpuFloadDualFmaxFstoreInstr>(new VpuFloadDualFmaxFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_DUAL_FMIN_FSTORE: ret = std::shared_ptr<VpuFloadDualFminFstoreInstr>(new VpuFloadDualFminFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FADD_FSTORE:      ret = std::shared_ptr<VpuFloadFaddFstoreInstr>(new VpuFloadFaddFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FSUB_FSTORE:      ret = std::shared_ptr<VpuFloadFsubFstoreInstr>(new VpuFloadFsubFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FMUL_FSTORE:      ret = std::shared_ptr<VpuFloadFmulFstoreInstr>(new VpuFloadFmulFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FMAX_FSTORE:      ret = std::shared_ptr<VpuFloadFmaxFstoreInstr>(new VpuFloadFmaxFstoreInstr); break;
    case OpCodeType::VPU_FLOAD_FMIN_FSTORE:      ret = std::shared_ptr<VpuFloadFminFstoreInstr>(new VpuFloadFminFstoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FADD:         ret = std::shared_ptr<VpuLoadDualFaddInstr>(new VpuLoadDualFaddInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FSUB:         ret = std::shared_ptr<VpuLoadDualFsubInstr>(new VpuLoadDualFsubInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMUL:         ret = std::shared_ptr<VpuLoadDualFmulInstr>(new VpuLoadDualFmulInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMAX:         ret = std::shared_ptr<VpuLoadDualFmaxInstr>(new VpuLoadDualFmaxInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMIN:         ret = std::shared_ptr<VpuLoadDualFminInstr>(new VpuLoadDualFminInstr); break;
    case OpCodeType::VPU_LOAD_FADD:              ret = std::shared_ptr<VpuLoadFaddInstr>(new VpuLoadFaddInstr); break;
    case OpCodeType::VPU_LOAD_FSUB:              ret = std::shared_ptr<VpuLoadFsubInstr>(new VpuLoadFsubInstr); break;
    case OpCodeType::VPU_LOAD_FMUL:              ret = std::shared_ptr<VpuLoadFmulInstr>(new VpuLoadFmulInstr); break;
    case OpCodeType::VPU_LOAD_FMAX:              ret = std::shared_ptr<VpuLoadFmaxInstr>(new VpuLoadFmaxInstr); break;
    case OpCodeType::VPU_LOAD_FMIN:              ret = std::shared_ptr<VpuLoadFminInstr>(new VpuLoadFminInstr); break;
    case OpCodeType::VPU_FADD_STORE:             ret = std::shared_ptr<VpuFaddStoreInstr>(new VpuFaddStoreInstr); break;
    case OpCodeType::VPU_FSUB_STORE:             ret = std::shared_ptr<VpuFsubStoreInstr>(new VpuFsubStoreInstr); break;
    case OpCodeType::VPU_FMUL_STORE:             ret = std::shared_ptr<VpuFmulStoreInstr>(new VpuFmulStoreInstr); break;
    case OpCodeType::VPU_FMAX_STORE:             ret = std::shared_ptr<VpuFmaxStoreInstr>(new VpuFmaxStoreInstr); break;
    case OpCodeType::VPU_FMIN_STORE:             ret = std::shared_ptr<VpuFminStoreInstr>(new VpuFminStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FADD_STORE:   ret = std::shared_ptr<VpuLoadDualFaddStoreInstr>(new VpuLoadDualFaddStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FSUB_STORE:   ret = std::shared_ptr<VpuLoadDualFsubStoreInstr>(new VpuLoadDualFsubStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMUL_STORE:   ret = std::shared_ptr<VpuLoadDualFmulStoreInstr>(new VpuLoadDualFmulStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMAX_STORE:   ret = std::shared_ptr<VpuLoadDualFmaxStoreInstr>(new VpuLoadDualFmaxStoreInstr); break;
    case OpCodeType::VPU_LOAD_DUAL_FMIN_STORE:   ret = std::shared_ptr<VpuLoadDualFminStoreInstr>(new VpuLoadDualFminStoreInstr); break;
    case OpCodeType::VPU_LOAD_FADD_STORE:        ret = std::shared_ptr<VpuLoadFaddStoreInstr>(new VpuLoadFaddStoreInstr); break;
    case OpCodeType::VPU_LOAD_FSUB_STORE:        ret = std::shared_ptr<VpuLoadFsubStoreInstr>(new VpuLoadFsubStoreInstr); break;
    case OpCodeType::VPU_LOAD_FMUL_STORE:        ret = std::shared_ptr<VpuLoadFmulStoreInstr>(new VpuLoadFmulStoreInstr); break;
    case OpCodeType::VPU_LOAD_FMAX_STORE:        ret = std::shared_ptr<VpuLoadFmaxStoreInstr>(new VpuLoadFmaxStoreInstr); break;
    case OpCodeType::VPU_LOAD_FMIN_STORE:        ret = std::shared_ptr<VpuLoadFminStoreInstr>(new VpuLoadFminStoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FADD:        ret = std::shared_ptr<VpuUloadDualFaddInstr>(new VpuUloadDualFaddInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FSUB:        ret = std::shared_ptr<VpuUloadDualFsubInstr>(new VpuUloadDualFsubInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMUL:        ret = std::shared_ptr<VpuUloadDualFmulInstr>(new VpuUloadDualFmulInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMAX:        ret = std::shared_ptr<VpuUloadDualFmaxInstr>(new VpuUloadDualFmaxInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMIN:        ret = std::shared_ptr<VpuUloadDualFminInstr>(new VpuUloadDualFminInstr); break;
    case OpCodeType::VPU_ULOAD_FADD:             ret = std::shared_ptr<VpuUloadFaddInstr>(new VpuUloadFaddInstr); break;
    case OpCodeType::VPU_ULOAD_FSUB:             ret = std::shared_ptr<VpuUloadFsubInstr>(new VpuUloadFsubInstr); break;
    case OpCodeType::VPU_ULOAD_FMUL:             ret = std::shared_ptr<VpuUloadFmulInstr>(new VpuUloadFmulInstr); break;
    case OpCodeType::VPU_ULOAD_FMAX:             ret = std::shared_ptr<VpuUloadFmaxInstr>(new VpuUloadFmaxInstr); break;
    case OpCodeType::VPU_ULOAD_FMIN:             ret = std::shared_ptr<VpuUloadFminInstr>(new VpuUloadFminInstr); break;
    case OpCodeType::VPU_FADD_USTORE:            ret = std::shared_ptr<VpuFaddUstoreInstr>(new VpuFaddUstoreInstr); break;
    case OpCodeType::VPU_FSUB_USTORE:            ret = std::shared_ptr<VpuFsubUstoreInstr>(new VpuFsubUstoreInstr); break;
    case OpCodeType::VPU_FMUL_USTORE:            ret = std::shared_ptr<VpuFmulUstoreInstr>(new VpuFmulUstoreInstr); break;
    case OpCodeType::VPU_FMAX_USTORE:            ret = std::shared_ptr<VpuFmaxUstoreInstr>(new VpuFmaxUstoreInstr); break;
    case OpCodeType::VPU_FMIN_USTORE:            ret = std::shared_ptr<VpuFminUstoreInstr>(new VpuFminUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FADD_USTORE: ret = std::shared_ptr<VpuUloadDualFaddUstoreInstr>(new VpuUloadDualFaddUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FSUB_USTORE: ret = std::shared_ptr<VpuUloadDualFsubUstoreInstr>(new VpuUloadDualFsubUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMUL_USTORE: ret = std::shared_ptr<VpuUloadDualFmulUstoreInstr>(new VpuUloadDualFmulUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMAX_USTORE: ret = std::shared_ptr<VpuUloadDualFmaxUstoreInstr>(new VpuUloadDualFmaxUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_DUAL_FMIN_USTORE: ret = std::shared_ptr<VpuUloadDualFminUstoreInstr>(new VpuUloadDualFminUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FADD_USTORE:      ret = std::shared_ptr<VpuUloadFaddUstoreInstr>(new VpuUloadFaddUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FSUB_USTORE:      ret = std::shared_ptr<VpuUloadFsubUstoreInstr>(new VpuUloadFsubUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FMUL_USTORE:      ret = std::shared_ptr<VpuUloadFmulUstoreInstr>(new VpuUloadFmulUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FMAX_USTORE:      ret = std::shared_ptr<VpuUloadFmaxUstoreInstr>(new VpuUloadFmaxUstoreInstr); break;
    case OpCodeType::VPU_ULOAD_FMIN_USTORE:      ret = std::shared_ptr<VpuUloadFminUstoreInstr>(new VpuUloadFminUstoreInstr); break;
    default: break;
  }
  for (size_t i = 0; i < args.size(); ++i)
    ret->setters[i](args[i]);
  return ret;
}

} // namespace antoum

#endif // ANTOUM_VPU_INSTR_H
