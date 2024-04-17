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
    uint32_t m = (((uint32_t)1<<(high-low+1))-1)<<low;\
    fields = (fields&(~m)) | (field<<low)&m;\
    VpuInstr::set##field(field); }
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
    NOP = 8,
    OP_END = 9,
    F16_LOAD = 10,
    F16_LOAD_UPPER = 11,
    F16_LOAD_CONST = 12,
    F16_DLOAD = 13,
    F16_DLOAD_UPPER = 14,
    F16_STORE = 15,
    F16_STORE_UPPER = 16,
    I16_STORE = 17,
    I16_STORE_UPPER = 18,
    I8_LOAD = 19,
    I8_LOAD_CONST = 20,
    I8_DLOAD = 21,
    I8_STORE = 22,
    U8_LOAD = 23,
    U8_LOAD_CONST = 24,
    U8_DLOAD = 25,
    U8_STORE = 26,
    F16_ADD = 27,
    F16_SUB = 28,
    F16_MUL = 29,
    F16_MAX = 30,
    F16_MIN = 31,
    F16_GREATER = 32,
    F16_GREATER_EQUAL = 33,
    F16_LESS = 34,
    F16_LESS_EQUAL = 35,
    F16_EQUAL = 36,
    F16_SEL = 37,
    F16_ADDI = 38,
    F16_SUBI = 39,
    F16_MULI = 40,
    F16_MAXI = 41,
    F16_MINI = 42,
    F16_MOVI = 43,
    F16_GREATERI = 44,
    F16_GREATER_EQUALI = 45,
    F16_LESSI = 46,
    F16_LESS_EQUALI = 47,
    F16_EQUALI = 48,
    F16_SELI = 49,
    F16_REDUCE_SUM = 50,
    F16_DLOAD_ADD = 51,
    F16_DLOAD_SUB = 52,
    F16_DLOAD_MUL = 53,
    F16_DLOAD_MAX = 54,
    F16_DLOAD_MIN = 55,
    F16_LOAD_ADD = 56,
    F16_LOAD_SUB = 57,
    F16_LOAD_MUL = 58,
    F16_LOAD_MAX = 59,
    F16_LOAD_MIN = 60,
    F16_ADD_STORE = 61,
    F16_SUB_STORE = 62,
    F16_MUL_STORE = 63,
    F16_MAX_STORE = 64,
    F16_MIN_STORE = 65,
    F16_DLOAD_ADD_STORE = 66,
    F16_DLOAD_SUB_STORE = 67,
    F16_DLOAD_MUL_STORE = 68,
    F16_DLOAD_MAX_STORE = 69,
    F16_DLOAD_MIN_STORE = 70,
    F16_LOAD_ADD_STORE = 71,
    F16_LOAD_SUB_STORE = 72,
    F16_LOAD_MUL_STORE = 73,
    F16_LOAD_MAX_STORE = 74,
    F16_LOAD_MIN_STORE = 75,
    I8_DLOAD_ADD = 76,
    I8_DLOAD_SUB = 77,
    I8_DLOAD_MUL = 78,
    I8_DLOAD_MAX = 79,
    I8_DLOAD_MIN = 80,
    I8_LOAD_ADD = 81,
    I8_LOAD_SUB = 82,
    I8_LOAD_MUL = 83,
    I8_LOAD_MAX = 84,
    I8_LOAD_MIN = 85,
    I8_ADD_STORE = 86,
    I8_SUB_STORE = 87,
    I8_MUL_STORE = 88,
    I8_MAX_STORE = 89,
    I8_MIN_STORE = 90,
    I8_DLOAD_ADD_STORE = 91,
    I8_DLOAD_SUB_STORE = 92,
    I8_DLOAD_MUL_STORE = 93,
    I8_DLOAD_MAX_STORE = 94,
    I8_DLOAD_MIN_STORE = 95,
    I8_LOAD_ADD_STORE = 96,
    I8_LOAD_SUB_STORE = 97,
    I8_LOAD_MUL_STORE = 98,
    I8_LOAD_MAX_STORE = 99,
    I8_LOAD_MIN_STORE = 100,
    U8_DLOAD_ADD = 101,
    U8_DLOAD_SUB = 102,
    U8_DLOAD_MUL = 103,
    U8_DLOAD_MAX = 104,
    U8_DLOAD_MIN = 105,
    U8_LOAD_ADD = 106,
    U8_LOAD_SUB = 107,
    U8_LOAD_MUL = 108,
    U8_LOAD_MAX = 109,
    U8_LOAD_MIN = 110,
    U8_ADD_STORE = 111,
    U8_SUB_STORE = 112,
    U8_MUL_STORE = 113,
    U8_MAX_STORE = 114,
    U8_MIN_STORE = 115,
    U8_DLOAD_ADD_STORE = 116,
    U8_DLOAD_SUB_STORE = 117,
    U8_DLOAD_MUL_STORE = 118,
    U8_DLOAD_MAX_STORE = 119,
    U8_DLOAD_MIN_STORE = 120,
    U8_LOAD_ADD_STORE = 121,
    U8_LOAD_SUB_STORE = 122,
    U8_LOAD_MUL_STORE = 123,
    U8_LOAD_MAX_STORE = 124,
    U8_LOAD_MIN_STORE = 125,
    INVALID = 65536
  };

  static const std::map<OpCodeType, OpCodeItem>& getOpCodeMap() {
    static const std::map<OpCodeType, OpCodeItem> opCodeMap = {
      {OpCodeType::SET_LOAD_BASE,       {"set_load_base",       0xff000000, 0x01000000}},
      {OpCodeType::SET_LOAD2_BASE,      {"set_load2_base",      0xff000000, 0x04000000}},
      {OpCodeType::SET_STORE_BASE,      {"set_store_base",      0xff000000, 0x07000000}},
      {OpCodeType::SET_LOAD_GLB,        {"set_load_glb",        0xff00000f, 0x02000000}},
      {OpCodeType::SET_STORE_GLB,       {"set_store_glb",       0xff0001ff, 0x08000000}},
      {OpCodeType::SET_QUANTIZE,        {"set_quantize",        0xffff0000, 0x09000000}},
      {OpCodeType::SET_DEQUANTIZE,      {"set_dequantize",      0xffff0000, 0x03000000}},
      {OpCodeType::SET_DEQUANTIZE2,     {"set_dequantize2",     0xffff0000, 0x06000000}},
      {OpCodeType::NOP,                 {"nop",                 0xffffffff, 0x00000000}},
      {OpCodeType::OP_END,              {"op_end",              0xffffffff, 0xff000000}},
      {OpCodeType::F16_LOAD,            {"f16_load",            0xffcf0000, 0xe0000000}},
      {OpCodeType::F16_LOAD_UPPER,      {"f16_load_upper",      0xffcf0000, 0xe1000000}},
      {OpCodeType::F16_LOAD_CONST,      {"f16_load_const",      0xffcf0000, 0xe0040000}},
      {OpCodeType::F16_DLOAD,           {"f16_dload",           0xff0f0000, 0xe0020000}},
      {OpCodeType::F16_DLOAD_UPPER,     {"f16_dload_upper",     0xff0f0000, 0xe1020000}},
      {OpCodeType::F16_STORE,           {"f16_store",           0xffcf0000, 0xe8000000}},
      {OpCodeType::F16_STORE_UPPER,     {"f16_store_upper",     0xffcf0000, 0xe9000000}},
      {OpCodeType::I16_STORE,           {"i16_store",           0xffcf0000, 0xca000000}},
      {OpCodeType::I16_STORE_UPPER,     {"i16_store_upper",     0xffcf0000, 0xcb000000}},
      {OpCodeType::I8_LOAD,             {"i8_load",             0xffcf0000, 0xc0000000}},
      {OpCodeType::I8_LOAD_CONST,       {"i8_load_const",       0xffcf0000, 0xc0040000}},
      {OpCodeType::I8_DLOAD,            {"i8_dload",            0xff0f0000, 0xc0020000}},
      {OpCodeType::I8_STORE,            {"i8_store",            0xffcf0000, 0xc8000000}},
      {OpCodeType::U8_LOAD,             {"u8_load",             0xffcf0000, 0xd0000000}},
      {OpCodeType::U8_LOAD_CONST,       {"u8_load_const",       0xffcf0000, 0xd0040000}},
      {OpCodeType::U8_DLOAD,            {"u8_dload",            0xff0f0000, 0xd0020000}},
      {OpCodeType::U8_STORE,            {"u8_store",            0xffcf0000, 0xd8000000}},
      {OpCodeType::F16_ADD,             {"f16_add",             0xff007fff, 0x80000000}},
      {OpCodeType::F16_SUB,             {"f16_sub",             0xff007fff, 0x81000000}},
      {OpCodeType::F16_MUL,             {"f16_mul",             0xff007fff, 0x82000000}},
      {OpCodeType::F16_MAX,             {"f16_max",             0xff007fff, 0x83000000}},
      {OpCodeType::F16_MIN,             {"f16_min",             0xff007fff, 0x84000000}},
      {OpCodeType::F16_GREATER,         {"f16_greater",         0xffe07fff, 0x87000000}},
      {OpCodeType::F16_GREATER_EQUAL,   {"f16_greater_equal",   0xffe07fff, 0x8a000000}},
      {OpCodeType::F16_LESS,            {"f16_less",            0xffe07fff, 0x89000000}},
      {OpCodeType::F16_LESS_EQUAL,      {"f16_less_equal",      0xffe07fff, 0x8b000000}},
      {OpCodeType::F16_EQUAL,           {"f16_equal",           0xffe07fff, 0x88000000}},
      {OpCodeType::F16_SEL,             {"f16_sel",             0xff007fff, 0x8c000000}},
      {OpCodeType::F16_ADDI,            {"f16_addi",            0xff030000, 0xa0000000}},
      {OpCodeType::F16_SUBI,            {"f16_subi",            0xff030000, 0xa1000000}},
      {OpCodeType::F16_MULI,            {"f16_muli",            0xff030000, 0xa2000000}},
      {OpCodeType::F16_MAXI,            {"f16_maxi",            0xff030000, 0xa3000000}},
      {OpCodeType::F16_MINI,            {"f16_mini",            0xff030000, 0xa4000000}},
      {OpCodeType::F16_MOVI,            {"f16_movi",            0xff1f0000, 0xa5000000}},
      {OpCodeType::F16_GREATERI,        {"f16_greateri",        0xffe30000, 0xa7000000}},
      {OpCodeType::F16_GREATER_EQUALI,  {"f16_greater_equali",  0xffe30000, 0xaa000000}},
      {OpCodeType::F16_LESSI,           {"f16_lessi",           0xffe30000, 0xa9000000}},
      {OpCodeType::F16_LESS_EQUALI,     {"f16_less_equali",     0xffe30000, 0xab000000}},
      {OpCodeType::F16_EQUALI,          {"f16_equali",          0xffe30000, 0xa8000000}},
      {OpCodeType::F16_SELI,            {"f16_seli",            0xff030000, 0xac000000}},
      {OpCodeType::F16_REDUCE_SUM,      {"f16_reduce_sum",      0xfffc7fff, 0x86000000}},
      {OpCodeType::F16_DLOAD_ADD,       {"f16_dload_add",       0xff3f0000, 0xe0030000}},
      {OpCodeType::F16_DLOAD_SUB,       {"f16_dload_sub",       0xff3f0000, 0xe1030000}},
      {OpCodeType::F16_DLOAD_MUL,       {"f16_dload_mul",       0xff3f0000, 0xe2030000}},
      {OpCodeType::F16_DLOAD_MAX,       {"f16_dload_max",       0xff3f0000, 0xe3030000}},
      {OpCodeType::F16_DLOAD_MIN,       {"f16_dload_min",       0xff3f0000, 0xe4030000}},
      {OpCodeType::F16_LOAD_ADD,        {"f16_load_add",        0xff0f0000, 0xe0010000}},
      {OpCodeType::F16_LOAD_SUB,        {"f16_load_sub",        0xff0f0000, 0xe1010000}},
      {OpCodeType::F16_LOAD_MUL,        {"f16_load_mul",        0xff0f0000, 0xe2010000}},
      {OpCodeType::F16_LOAD_MAX,        {"f16_load_max",        0xff0f0000, 0xe3010000}},
      {OpCodeType::F16_LOAD_MIN,        {"f16_load_min",        0xff0f0000, 0xe4010000}},
      {OpCodeType::F16_ADD_STORE,       {"f16_add_store",       0xff0f0000, 0xe8010000}},
      {OpCodeType::F16_SUB_STORE,       {"f16_sub_store",       0xff0f0000, 0xe9010000}},
      {OpCodeType::F16_MUL_STORE,       {"f16_mul_store",       0xff0f0000, 0xea010000}},
      {OpCodeType::F16_MAX_STORE,       {"f16_max_store",       0xff0f0000, 0xeb010000}},
      {OpCodeType::F16_MIN_STORE,       {"f16_min_store",       0xff0f0000, 0xec010000}},
      {OpCodeType::F16_DLOAD_ADD_STORE, {"f16_dload_add_store", 0xffff0000, 0xe00a0000}},
      {OpCodeType::F16_DLOAD_SUB_STORE, {"f16_dload_sub_store", 0xffff0000, 0xe10a0000}},
      {OpCodeType::F16_DLOAD_MUL_STORE, {"f16_dload_mul_store", 0xffff0000, 0xe20a0000}},
      {OpCodeType::F16_DLOAD_MAX_STORE, {"f16_dload_max_store", 0xffff0000, 0xe30a0000}},
      {OpCodeType::F16_DLOAD_MIN_STORE, {"f16_dload_min_store", 0xffff0000, 0xe40a0000}},
      {OpCodeType::F16_LOAD_ADD_STORE,  {"f16_load_add_store",  0xffcf0000, 0xe0080000}},
      {OpCodeType::F16_LOAD_SUB_STORE,  {"f16_load_sub_store",  0xffcf0000, 0xe1080000}},
      {OpCodeType::F16_LOAD_MUL_STORE,  {"f16_load_mul_store",  0xffcf0000, 0xe2080000}},
      {OpCodeType::F16_LOAD_MAX_STORE,  {"f16_load_max_store",  0xffcf0000, 0xe3080000}},
      {OpCodeType::F16_LOAD_MIN_STORE,  {"f16_load_min_store",  0xffcf0000, 0xe4080000}},
      {OpCodeType::I8_DLOAD_ADD,        {"i8_dload_add",        0xff3f0000, 0xc0030000}},
      {OpCodeType::I8_DLOAD_SUB,        {"i8_dload_sub",        0xff3f0000, 0xc1030000}},
      {OpCodeType::I8_DLOAD_MUL,        {"i8_dload_mul",        0xff3f0000, 0xc2030000}},
      {OpCodeType::I8_DLOAD_MAX,        {"i8_dload_max",        0xff3f0000, 0xc3030000}},
      {OpCodeType::I8_DLOAD_MIN,        {"i8_dload_min",        0xff3f0000, 0xc4030000}},
      {OpCodeType::I8_LOAD_ADD,         {"i8_load_add",         0xff0f0000, 0xc0010000}},
      {OpCodeType::I8_LOAD_SUB,         {"i8_load_sub",         0xff0f0000, 0xc1010000}},
      {OpCodeType::I8_LOAD_MUL,         {"i8_load_mul",         0xff0f0000, 0xc2010000}},
      {OpCodeType::I8_LOAD_MAX,         {"i8_load_max",         0xff0f0000, 0xc3010000}},
      {OpCodeType::I8_LOAD_MIN,         {"i8_load_min",         0xff0f0000, 0xc4010000}},
      {OpCodeType::I8_ADD_STORE,        {"i8_add_store",        0xff0f0000, 0xc8010000}},
      {OpCodeType::I8_SUB_STORE,        {"i8_sub_store",        0xff0f0000, 0xc9010000}},
      {OpCodeType::I8_MUL_STORE,        {"i8_mul_store",        0xff0f0000, 0xca010000}},
      {OpCodeType::I8_MAX_STORE,        {"i8_max_store",        0xff0f0000, 0xcb010000}},
      {OpCodeType::I8_MIN_STORE,        {"i8_min_store",        0xff0f0000, 0xcc010000}},
      {OpCodeType::I8_DLOAD_ADD_STORE,  {"i8_dload_add_store",  0xffff0000, 0xc00a0000}},
      {OpCodeType::I8_DLOAD_SUB_STORE,  {"i8_dload_sub_store",  0xffff0000, 0xc10a0000}},
      {OpCodeType::I8_DLOAD_MUL_STORE,  {"i8_dload_mul_store",  0xffff0000, 0xc20a0000}},
      {OpCodeType::I8_DLOAD_MAX_STORE,  {"i8_dload_max_store",  0xffff0000, 0xc30a0000}},
      {OpCodeType::I8_DLOAD_MIN_STORE,  {"i8_dload_min_store",  0xffff0000, 0xc40a0000}},
      {OpCodeType::I8_LOAD_ADD_STORE,   {"i8_load_add_store",   0xffcf0000, 0xc0080000}},
      {OpCodeType::I8_LOAD_SUB_STORE,   {"i8_load_sub_store",   0xffcf0000, 0xc1080000}},
      {OpCodeType::I8_LOAD_MUL_STORE,   {"i8_load_mul_store",   0xffcf0000, 0xc2080000}},
      {OpCodeType::I8_LOAD_MAX_STORE,   {"i8_load_max_store",   0xffcf0000, 0xc3080000}},
      {OpCodeType::I8_LOAD_MIN_STORE,   {"i8_load_min_store",   0xffcf0000, 0xc4080000}},
      {OpCodeType::U8_DLOAD_ADD,        {"u8_dload_add",        0xff3f0000, 0xd0030000}},
      {OpCodeType::U8_DLOAD_SUB,        {"u8_dload_sub",        0xff3f0000, 0xd1030000}},
      {OpCodeType::U8_DLOAD_MUL,        {"u8_dload_mul",        0xff3f0000, 0xd2030000}},
      {OpCodeType::U8_DLOAD_MAX,        {"u8_dload_max",        0xff3f0000, 0xd3030000}},
      {OpCodeType::U8_DLOAD_MIN,        {"u8_dload_min",        0xff3f0000, 0xd4030000}},
      {OpCodeType::U8_LOAD_ADD,         {"u8_load_add",         0xff0f0000, 0xd0010000}},
      {OpCodeType::U8_LOAD_SUB,         {"u8_load_sub",         0xff0f0000, 0xd1010000}},
      {OpCodeType::U8_LOAD_MUL,         {"u8_load_mul",         0xff0f0000, 0xd2010000}},
      {OpCodeType::U8_LOAD_MAX,         {"u8_load_max",         0xff0f0000, 0xd3010000}},
      {OpCodeType::U8_LOAD_MIN,         {"u8_load_min",         0xff0f0000, 0xd4010000}},
      {OpCodeType::U8_ADD_STORE,        {"u8_add_store",        0xff0f0000, 0xd8010000}},
      {OpCodeType::U8_SUB_STORE,        {"u8_sub_store",        0xff0f0000, 0xd9010000}},
      {OpCodeType::U8_MUL_STORE,        {"u8_mul_store",        0xff0f0000, 0xda010000}},
      {OpCodeType::U8_MAX_STORE,        {"u8_max_store",        0xff0f0000, 0xdb010000}},
      {OpCodeType::U8_MIN_STORE,        {"u8_min_store",        0xff0f0000, 0xdc010000}},
      {OpCodeType::U8_DLOAD_ADD_STORE,  {"u8_dload_add_store",  0xffff0000, 0xd00a0000}},
      {OpCodeType::U8_DLOAD_SUB_STORE,  {"u8_dload_sub_store",  0xffff0000, 0xd10a0000}},
      {OpCodeType::U8_DLOAD_MUL_STORE,  {"u8_dload_mul_store",  0xffff0000, 0xd20a0000}},
      {OpCodeType::U8_DLOAD_MAX_STORE,  {"u8_dload_max_store",  0xffff0000, 0xd30a0000}},
      {OpCodeType::U8_DLOAD_MIN_STORE,  {"u8_dload_min_store",  0xffff0000, 0xd40a0000}},
      {OpCodeType::U8_LOAD_ADD_STORE,   {"u8_load_add_store",   0xffcf0000, 0xd0080000}},
      {OpCodeType::U8_LOAD_SUB_STORE,   {"u8_load_sub_store",   0xffcf0000, 0xd1080000}},
      {OpCodeType::U8_LOAD_MUL_STORE,   {"u8_load_mul_store",   0xffcf0000, 0xd2080000}},
      {OpCodeType::U8_LOAD_MAX_STORE,   {"u8_load_max_store",   0xffcf0000, 0xd3080000}},
      {OpCodeType::U8_LOAD_MIN_STORE,   {"u8_load_min_store",   0xffcf0000, 0xd4080000}},
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
  uint32_t getBankBroadcast() { return bankBroadcast; }
  uint32_t getBankMask() { return bankMask; }
  uint32_t getBaseAddr() { return baseAddr; }
  uint32_t getClusterBroadcast() { return clusterBroadcast; }
  uint32_t getClusterMask() { return clusterMask; }
  uint32_t getDequantize() { return dequantize; }
  uint32_t getHasImm() { return hasImm; }
  uint32_t getImm() { return imm; }
  uint32_t getIsBf16() { return isBf16; }
  uint32_t getIsInt16() { return isInt16; }
  uint32_t getIsUnsigned() { return isUnsigned; }
  uint32_t getOffset() { return offset; }
  uint32_t getQuantize() { return quantize; }
  uint32_t getRd() { return rd; }
  uint32_t getRs() { return rs; }
  uint32_t getRt() { return rt; }
  uint32_t getStride() { return stride; }
  uint32_t getUpper() { return upper; }

  void setBankBroadcast(uint32_t n) { bankBroadcast = n; }
  void setBankMask(uint32_t n) { bankMask = n; }
  void setBaseAddr(uint32_t n) { baseAddr = n; }
  void setClusterBroadcast(uint32_t n) { clusterBroadcast = n; }
  void setClusterMask(uint32_t n) { clusterMask = n; }
  void setDequantize(uint32_t n) { dequantize = n; }
  void setHasImm(uint32_t n) { hasImm = n; }
  void setImm(uint32_t n) { imm = n; }
  void setIsBf16(uint32_t n) { isBf16 = n; }
  void setIsInt16(uint32_t n) { isInt16 = n; }
  void setIsUnsigned(uint32_t n) { isUnsigned = n; }
  void setOffset(uint32_t n) { offset = n; }
  void setQuantize(uint32_t n) { quantize = n; }
  void setRd(uint32_t n) { rd = n; }
  void setRs(uint32_t n) { rs = n; }
  void setRt(uint32_t n) { rt = n; }
  void setStride(uint32_t n) { stride = n; }
  void setUpper(uint32_t n) { upper = n; }

protected:
  uint32_t bankBroadcast = 0;
  uint32_t bankMask = 0;
  uint32_t baseAddr = 0;
  uint32_t clusterBroadcast = 0;
  uint32_t clusterMask = 0;
  uint32_t dequantize = 0;
  uint32_t hasImm = 0;
  uint32_t imm = 0;
  uint32_t isBf16 = 0;
  uint32_t isInt16 = 0;
  uint32_t isUnsigned = 0;
  uint32_t offset = 0;
  uint32_t quantize = 0;
  uint32_t rd = 0;
  uint32_t rs = 0;
  uint32_t rt = 0;
  uint32_t stride = 0;
  uint32_t upper = 0;
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
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class SetLoad2BaseInstr : public VpuInstr {
public:
  explicit SetLoad2BaseInstr() : VpuInstr(OpCodeType::SET_LOAD2_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
  }
  VPU_GEN_GETTER_SETTER(BaseAddr, 23, 0);
};

class SetStoreBaseInstr : public VpuInstr {
public:
  explicit SetStoreBaseInstr() : VpuInstr(OpCodeType::SET_STORE_BASE) {
    VPU_PUSH_GETTER_SETTER(BaseAddr);
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
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 13);
  VPU_GEN_GETTER_SETTER(BankMask, 12, 9);
  VPU_GEN_GETTER_SETTER(ClusterBroadcast, 8, 7);
  VPU_GEN_GETTER_SETTER(BankBroadcast, 6, 4);
};

class SetStoreGlbInstr : public VpuInstr {
public:
  explicit SetStoreGlbInstr() : VpuInstr(OpCodeType::SET_STORE_GLB) {
    VPU_PUSH_GETTER_SETTER(Stride);
    VPU_PUSH_GETTER_SETTER(ClusterMask);
    VPU_PUSH_GETTER_SETTER(BankMask);
  }
  VPU_GEN_GETTER_SETTER(Stride, 23, 21);
  VPU_GEN_GETTER_SETTER(ClusterMask, 20, 13);
  VPU_GEN_GETTER_SETTER(BankMask, 12, 9);
};

class SetQuantizeInstr : public VpuInstr {
public:
  explicit SetQuantizeInstr() : VpuInstr(OpCodeType::SET_QUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Quantize);
  }
  VPU_GEN_GETTER_SETTER(Quantize, 15, 0);
};

class SetDequantizeInstr : public VpuInstr {
public:
  explicit SetDequantizeInstr() : VpuInstr(OpCodeType::SET_DEQUANTIZE) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 15, 0);
};

class SetDequantize2Instr : public VpuInstr {
public:
  explicit SetDequantize2Instr() : VpuInstr(OpCodeType::SET_DEQUANTIZE2) {
    VPU_PUSH_GETTER_SETTER(Dequantize);
  }
  VPU_GEN_GETTER_SETTER(Dequantize, 15, 0);
};

class NopInstr : public VpuInstr {
public:
  explicit NopInstr() : VpuInstr(OpCodeType::NOP) {
  }
};

class OpEndInstr : public VpuInstr {
public:
  explicit OpEndInstr() : VpuInstr(OpCodeType::OP_END) {
  }
};

class F16LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit F16LoadInstr() : VpuInstr(OpCodeType::F16_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadUpperInstr : public VpuInstr, public LoadInstr {
public:
  explicit F16LoadUpperInstr() : VpuInstr(OpCodeType::F16_LOAD_UPPER) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isBf16 = 1;
    upper = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadConstInstr : public VpuInstr {
public:
  explicit F16LoadConstInstr() : VpuInstr(OpCodeType::F16_LOAD_CONST) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isBf16 = 1;
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
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class F16DloadUpperInstr : public VpuInstr, public DloadInstr {
public:
  explicit F16DloadUpperInstr() : VpuInstr(OpCodeType::F16_DLOAD_UPPER) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
    isBf16 = 1;
    upper = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class F16StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit F16StoreInstr() : VpuInstr(OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16StoreUpperInstr : public VpuInstr, public StoreInstr {
public:
  explicit F16StoreUpperInstr() : VpuInstr(OpCodeType::F16_STORE_UPPER) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
    upper = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit I16StoreInstr() : VpuInstr(OpCodeType::I16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    isInt16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I16StoreUpperInstr : public VpuInstr, public StoreInstr {
public:
  explicit I16StoreUpperInstr() : VpuInstr(OpCodeType::I16_STORE_UPPER) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    isInt16 = 1;
    upper = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit I8LoadInstr() : VpuInstr(OpCodeType::I8_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadConstInstr : public VpuInstr {
public:
  explicit I8LoadConstInstr() : VpuInstr(OpCodeType::I8_LOAD_CONST) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8DloadInstr : public VpuInstr, public DloadInstr {
public:
  explicit I8DloadInstr() : VpuInstr(OpCodeType::I8_DLOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rs);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class I8StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit I8StoreInstr() : VpuInstr(OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8LoadInstr : public VpuInstr, public LoadInstr {
public:
  explicit U8LoadInstr() : VpuInstr(OpCodeType::U8_LOAD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadConstInstr : public VpuInstr {
public:
  explicit U8LoadConstInstr() : VpuInstr(OpCodeType::U8_LOAD_CONST) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isUnsigned = 1;
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
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Rs, 23, 22);
};

class U8StoreInstr : public VpuInstr, public StoreInstr {
public:
  explicit U8StoreInstr() : VpuInstr(OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16AddInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16AddInstr() : VpuInstr(OpCodeType::F16_ADD) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16SubInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16SubInstr() : VpuInstr(OpCodeType::F16_SUB) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16MulInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16MulInstr() : VpuInstr(OpCodeType::F16_MUL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16MaxInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16MaxInstr() : VpuInstr(OpCodeType::F16_MAX) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16MinInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16MinInstr() : VpuInstr(OpCodeType::F16_MIN) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16GreaterInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16GreaterInstr() : VpuInstr(OpCodeType::F16_GREATER) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F16GreaterEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16GreaterEqualInstr() : VpuInstr(OpCodeType::F16_GREATER_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F16LessInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16LessInstr() : VpuInstr(OpCodeType::F16_LESS) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F16LessEqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16LessEqualInstr() : VpuInstr(OpCodeType::F16_LESS_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F16EqualInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16EqualInstr() : VpuInstr(OpCodeType::F16_EQUAL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F16SelInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16SelInstr() : VpuInstr(OpCodeType::F16_SEL) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rt);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16AddiInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16AddiInstr() : VpuInstr(OpCodeType::F16_ADDI) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Imm);
    VPU_PUSH_GETTER_SETTER(Rd);
    hasImm = 1;
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
  }
  VPU_GEN_GETTER_SETTER(Rs, 20, 18);
  VPU_GEN_GETTER_SETTER(Imm, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 21);
};

class F16ReduceSumInstr : public VpuInstr, public VectorInstr {
public:
  explicit F16ReduceSumInstr() : VpuInstr(OpCodeType::F16_REDUCE_SUM) {
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Rt, 17, 15);
};

class F16DloadAddInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadAddInstr() : VpuInstr(OpCodeType::F16_DLOAD_ADD), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadSubInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadSubInstr() : VpuInstr(OpCodeType::F16_DLOAD_SUB), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMulInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadMulInstr() : VpuInstr(OpCodeType::F16_DLOAD_MUL), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMaxInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadMaxInstr() : VpuInstr(OpCodeType::F16_DLOAD_MAX), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16DloadMinInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit F16DloadMinInstr() : VpuInstr(OpCodeType::F16_DLOAD_MIN), DloadComputeInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadAddInstr() : VpuInstr(OpCodeType::F16_LOAD_ADD), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F16_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadSubInstr() : VpuInstr(OpCodeType::F16_LOAD_SUB), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F16_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadMulInstr() : VpuInstr(OpCodeType::F16_LOAD_MUL), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F16_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadMaxInstr() : VpuInstr(OpCodeType::F16_LOAD_MAX), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F16_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit F16LoadMinInstr() : VpuInstr(OpCodeType::F16_LOAD_MIN), LoadComputeInstr(OpCodeType::F16_LOAD, OpCodeType::F16_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class F16AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16AddStoreInstr() : VpuInstr(OpCodeType::F16_ADD_STORE), ComputeStoreInstr(OpCodeType::F16_ADD, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16SubStoreInstr() : VpuInstr(OpCodeType::F16_SUB_STORE), ComputeStoreInstr(OpCodeType::F16_SUB, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16MulStoreInstr() : VpuInstr(OpCodeType::F16_MUL_STORE), ComputeStoreInstr(OpCodeType::F16_MUL, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16MaxStoreInstr() : VpuInstr(OpCodeType::F16_MAX_STORE), ComputeStoreInstr(OpCodeType::F16_MAX, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit F16MinStoreInstr() : VpuInstr(OpCodeType::F16_MIN_STORE), ComputeStoreInstr(OpCodeType::F16_MIN, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadAddStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadAddStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_ADD_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_ADD, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadSubStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadSubStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_SUB_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_SUB, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMulStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadMulStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MUL_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_MUL, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMaxStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadMaxStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MAX_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_MAX, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16DloadMinStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit F16DloadMinStoreInstr() : VpuInstr(OpCodeType::F16_DLOAD_MIN_STORE), DloadComputeStoreInstr(OpCodeType::F16_DLOAD, OpCodeType::F16_MIN, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class F16LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadAddStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F16_ADD, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadSubStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F16_SUB, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMulStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F16_MUL, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMaxStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F16_MAX, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class F16LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit F16LoadMinStoreInstr() : VpuInstr(OpCodeType::F16_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::F16_LOAD, OpCodeType::F16_MIN, OpCodeType::F16_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isBf16 = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8DloadAddInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadAddInstr() : VpuInstr(OpCodeType::I8_DLOAD_ADD), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadSubInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadSubInstr() : VpuInstr(OpCodeType::I8_DLOAD_SUB), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMulInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadMulInstr() : VpuInstr(OpCodeType::I8_DLOAD_MUL), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMaxInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadMaxInstr() : VpuInstr(OpCodeType::I8_DLOAD_MAX), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8DloadMinInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit I8DloadMinInstr() : VpuInstr(OpCodeType::I8_DLOAD_MIN), DloadComputeInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadAddInstr() : VpuInstr(OpCodeType::I8_LOAD_ADD), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F16_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadSubInstr() : VpuInstr(OpCodeType::I8_LOAD_SUB), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F16_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadMulInstr() : VpuInstr(OpCodeType::I8_LOAD_MUL), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F16_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadMaxInstr() : VpuInstr(OpCodeType::I8_LOAD_MAX), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F16_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit I8LoadMinInstr() : VpuInstr(OpCodeType::I8_LOAD_MIN), LoadComputeInstr(OpCodeType::I8_LOAD, OpCodeType::F16_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class I8AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8AddStoreInstr() : VpuInstr(OpCodeType::I8_ADD_STORE), ComputeStoreInstr(OpCodeType::F16_ADD, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8SubStoreInstr() : VpuInstr(OpCodeType::I8_SUB_STORE), ComputeStoreInstr(OpCodeType::F16_SUB, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8MulStoreInstr() : VpuInstr(OpCodeType::I8_MUL_STORE), ComputeStoreInstr(OpCodeType::F16_MUL, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8MaxStoreInstr() : VpuInstr(OpCodeType::I8_MAX_STORE), ComputeStoreInstr(OpCodeType::F16_MAX, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit I8MinStoreInstr() : VpuInstr(OpCodeType::I8_MIN_STORE), ComputeStoreInstr(OpCodeType::F16_MIN, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadAddStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadAddStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_ADD_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_ADD, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadSubStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadSubStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_SUB_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_SUB, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMulStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadMulStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MUL_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_MUL, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMaxStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadMaxStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MAX_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_MAX, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8DloadMinStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit I8DloadMinStoreInstr() : VpuInstr(OpCodeType::I8_DLOAD_MIN_STORE), DloadComputeStoreInstr(OpCodeType::I8_DLOAD, OpCodeType::F16_MIN, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class I8LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadAddStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F16_ADD, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadSubStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F16_SUB, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMulStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F16_MUL, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMaxStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F16_MAX, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class I8LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit I8LoadMinStoreInstr() : VpuInstr(OpCodeType::I8_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::I8_LOAD, OpCodeType::F16_MIN, OpCodeType::I8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8DloadAddInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadAddInstr() : VpuInstr(OpCodeType::U8_DLOAD_ADD), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadSubInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadSubInstr() : VpuInstr(OpCodeType::U8_DLOAD_SUB), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMulInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadMulInstr() : VpuInstr(OpCodeType::U8_DLOAD_MUL), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMaxInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadMaxInstr() : VpuInstr(OpCodeType::U8_DLOAD_MAX), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8DloadMinInstr : public VpuInstr, public DloadComputeInstr {
public:
  explicit U8DloadMinInstr() : VpuInstr(OpCodeType::U8_DLOAD_MIN), DloadComputeInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadAddInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadAddInstr() : VpuInstr(OpCodeType::U8_LOAD_ADD), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F16_ADD) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadSubInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadSubInstr() : VpuInstr(OpCodeType::U8_LOAD_SUB), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F16_SUB) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadMulInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadMulInstr() : VpuInstr(OpCodeType::U8_LOAD_MUL), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F16_MUL) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadMaxInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadMaxInstr() : VpuInstr(OpCodeType::U8_LOAD_MAX), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F16_MAX) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8LoadMinInstr : public VpuInstr, public LoadComputeInstr {
public:
  explicit U8LoadMinInstr() : VpuInstr(OpCodeType::U8_LOAD_MIN), LoadComputeInstr(OpCodeType::U8_LOAD, OpCodeType::F16_MIN) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
};

class U8AddStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8AddStoreInstr() : VpuInstr(OpCodeType::U8_ADD_STORE), ComputeStoreInstr(OpCodeType::F16_ADD, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8SubStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8SubStoreInstr() : VpuInstr(OpCodeType::U8_SUB_STORE), ComputeStoreInstr(OpCodeType::F16_SUB, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8MulStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8MulStoreInstr() : VpuInstr(OpCodeType::U8_MUL_STORE), ComputeStoreInstr(OpCodeType::F16_MUL, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8MaxStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8MaxStoreInstr() : VpuInstr(OpCodeType::U8_MAX_STORE), ComputeStoreInstr(OpCodeType::F16_MAX, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8MinStoreInstr : public VpuInstr, public ComputeStoreInstr {
public:
  explicit U8MinStoreInstr() : VpuInstr(OpCodeType::U8_MIN_STORE), ComputeStoreInstr(OpCodeType::F16_MIN, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Rs);
    VPU_PUSH_GETTER_SETTER(Rd);
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Rs, 21, 20);
  VPU_GEN_GETTER_SETTER(Rd, 23, 22);
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadAddStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadAddStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_ADD_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_ADD, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadSubStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadSubStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_SUB_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_SUB, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMulStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadMulStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MUL_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_MUL, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMaxStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadMaxStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MAX_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_MAX, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8DloadMinStoreInstr : public VpuInstr, public DloadComputeStoreInstr {
public:
  explicit U8DloadMinStoreInstr() : VpuInstr(OpCodeType::U8_DLOAD_MIN_STORE), DloadComputeStoreInstr(OpCodeType::U8_DLOAD, OpCodeType::F16_MIN, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
};

class U8LoadAddStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadAddStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_ADD_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F16_ADD, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadSubStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadSubStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_SUB_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F16_SUB, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMulStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMulStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MUL_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F16_MUL, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMaxStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMaxStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MAX_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F16_MAX, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
};

class U8LoadMinStoreInstr : public VpuInstr, public LoadComputeStoreInstr {
public:
  explicit U8LoadMinStoreInstr() : VpuInstr(OpCodeType::U8_LOAD_MIN_STORE), LoadComputeStoreInstr(OpCodeType::U8_LOAD, OpCodeType::F16_MIN, OpCodeType::U8_STORE) {
    VPU_PUSH_GETTER_SETTER(Offset);
    VPU_PUSH_GETTER_SETTER(Rt);
    isUnsigned = 1;
  }
  VPU_GEN_GETTER_SETTER(Offset, 15, 0);
  VPU_GEN_GETTER_SETTER(Rt, 21, 20);
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
    case OpCodeType::NOP:                 ret = std::shared_ptr<NopInstr>(new NopInstr); break;
    case OpCodeType::OP_END:              ret = std::shared_ptr<OpEndInstr>(new OpEndInstr); break;
    case OpCodeType::F16_LOAD:            ret = std::shared_ptr<F16LoadInstr>(new F16LoadInstr); break;
    case OpCodeType::F16_LOAD_UPPER:      ret = std::shared_ptr<F16LoadUpperInstr>(new F16LoadUpperInstr); break;
    case OpCodeType::F16_LOAD_CONST:      ret = std::shared_ptr<F16LoadConstInstr>(new F16LoadConstInstr); break;
    case OpCodeType::F16_DLOAD:           ret = std::shared_ptr<F16DloadInstr>(new F16DloadInstr); break;
    case OpCodeType::F16_DLOAD_UPPER:     ret = std::shared_ptr<F16DloadUpperInstr>(new F16DloadUpperInstr); break;
    case OpCodeType::F16_STORE:           ret = std::shared_ptr<F16StoreInstr>(new F16StoreInstr); break;
    case OpCodeType::F16_STORE_UPPER:     ret = std::shared_ptr<F16StoreUpperInstr>(new F16StoreUpperInstr); break;
    case OpCodeType::I16_STORE:           ret = std::shared_ptr<I16StoreInstr>(new I16StoreInstr); break;
    case OpCodeType::I16_STORE_UPPER:     ret = std::shared_ptr<I16StoreUpperInstr>(new I16StoreUpperInstr); break;
    case OpCodeType::I8_LOAD:             ret = std::shared_ptr<I8LoadInstr>(new I8LoadInstr); break;
    case OpCodeType::I8_LOAD_CONST:       ret = std::shared_ptr<I8LoadConstInstr>(new I8LoadConstInstr); break;
    case OpCodeType::I8_DLOAD:            ret = std::shared_ptr<I8DloadInstr>(new I8DloadInstr); break;
    case OpCodeType::I8_STORE:            ret = std::shared_ptr<I8StoreInstr>(new I8StoreInstr); break;
    case OpCodeType::U8_LOAD:             ret = std::shared_ptr<U8LoadInstr>(new U8LoadInstr); break;
    case OpCodeType::U8_LOAD_CONST:       ret = std::shared_ptr<U8LoadConstInstr>(new U8LoadConstInstr); break;
    case OpCodeType::U8_DLOAD:            ret = std::shared_ptr<U8DloadInstr>(new U8DloadInstr); break;
    case OpCodeType::U8_STORE:            ret = std::shared_ptr<U8StoreInstr>(new U8StoreInstr); break;
    case OpCodeType::F16_ADD:             ret = std::shared_ptr<F16AddInstr>(new F16AddInstr); break;
    case OpCodeType::F16_SUB:             ret = std::shared_ptr<F16SubInstr>(new F16SubInstr); break;
    case OpCodeType::F16_MUL:             ret = std::shared_ptr<F16MulInstr>(new F16MulInstr); break;
    case OpCodeType::F16_MAX:             ret = std::shared_ptr<F16MaxInstr>(new F16MaxInstr); break;
    case OpCodeType::F16_MIN:             ret = std::shared_ptr<F16MinInstr>(new F16MinInstr); break;
    case OpCodeType::F16_GREATER:         ret = std::shared_ptr<F16GreaterInstr>(new F16GreaterInstr); break;
    case OpCodeType::F16_GREATER_EQUAL:   ret = std::shared_ptr<F16GreaterEqualInstr>(new F16GreaterEqualInstr); break;
    case OpCodeType::F16_LESS:            ret = std::shared_ptr<F16LessInstr>(new F16LessInstr); break;
    case OpCodeType::F16_LESS_EQUAL:      ret = std::shared_ptr<F16LessEqualInstr>(new F16LessEqualInstr); break;
    case OpCodeType::F16_EQUAL:           ret = std::shared_ptr<F16EqualInstr>(new F16EqualInstr); break;
    case OpCodeType::F16_SEL:             ret = std::shared_ptr<F16SelInstr>(new F16SelInstr); break;
    case OpCodeType::F16_ADDI:            ret = std::shared_ptr<F16AddiInstr>(new F16AddiInstr); break;
    case OpCodeType::F16_SUBI:            ret = std::shared_ptr<F16SubiInstr>(new F16SubiInstr); break;
    case OpCodeType::F16_MULI:            ret = std::shared_ptr<F16MuliInstr>(new F16MuliInstr); break;
    case OpCodeType::F16_MAXI:            ret = std::shared_ptr<F16MaxiInstr>(new F16MaxiInstr); break;
    case OpCodeType::F16_MINI:            ret = std::shared_ptr<F16MiniInstr>(new F16MiniInstr); break;
    case OpCodeType::F16_MOVI:            ret = std::shared_ptr<F16MoviInstr>(new F16MoviInstr); break;
    case OpCodeType::F16_GREATERI:        ret = std::shared_ptr<F16GreateriInstr>(new F16GreateriInstr); break;
    case OpCodeType::F16_GREATER_EQUALI:  ret = std::shared_ptr<F16GreaterEqualiInstr>(new F16GreaterEqualiInstr); break;
    case OpCodeType::F16_LESSI:           ret = std::shared_ptr<F16LessiInstr>(new F16LessiInstr); break;
    case OpCodeType::F16_LESS_EQUALI:     ret = std::shared_ptr<F16LessEqualiInstr>(new F16LessEqualiInstr); break;
    case OpCodeType::F16_EQUALI:          ret = std::shared_ptr<F16EqualiInstr>(new F16EqualiInstr); break;
    case OpCodeType::F16_SELI:            ret = std::shared_ptr<F16SeliInstr>(new F16SeliInstr); break;
    case OpCodeType::F16_REDUCE_SUM:      ret = std::shared_ptr<F16ReduceSumInstr>(new F16ReduceSumInstr); break;
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
    case OpCodeType::F16_ADD_STORE:       ret = std::shared_ptr<F16AddStoreInstr>(new F16AddStoreInstr); break;
    case OpCodeType::F16_SUB_STORE:       ret = std::shared_ptr<F16SubStoreInstr>(new F16SubStoreInstr); break;
    case OpCodeType::F16_MUL_STORE:       ret = std::shared_ptr<F16MulStoreInstr>(new F16MulStoreInstr); break;
    case OpCodeType::F16_MAX_STORE:       ret = std::shared_ptr<F16MaxStoreInstr>(new F16MaxStoreInstr); break;
    case OpCodeType::F16_MIN_STORE:       ret = std::shared_ptr<F16MinStoreInstr>(new F16MinStoreInstr); break;
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
    case OpCodeType::I8_ADD_STORE:        ret = std::shared_ptr<I8AddStoreInstr>(new I8AddStoreInstr); break;
    case OpCodeType::I8_SUB_STORE:        ret = std::shared_ptr<I8SubStoreInstr>(new I8SubStoreInstr); break;
    case OpCodeType::I8_MUL_STORE:        ret = std::shared_ptr<I8MulStoreInstr>(new I8MulStoreInstr); break;
    case OpCodeType::I8_MAX_STORE:        ret = std::shared_ptr<I8MaxStoreInstr>(new I8MaxStoreInstr); break;
    case OpCodeType::I8_MIN_STORE:        ret = std::shared_ptr<I8MinStoreInstr>(new I8MinStoreInstr); break;
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
    case OpCodeType::U8_ADD_STORE:        ret = std::shared_ptr<U8AddStoreInstr>(new U8AddStoreInstr); break;
    case OpCodeType::U8_SUB_STORE:        ret = std::shared_ptr<U8SubStoreInstr>(new U8SubStoreInstr); break;
    case OpCodeType::U8_MUL_STORE:        ret = std::shared_ptr<U8MulStoreInstr>(new U8MulStoreInstr); break;
    case OpCodeType::U8_MAX_STORE:        ret = std::shared_ptr<U8MaxStoreInstr>(new U8MaxStoreInstr); break;
    case OpCodeType::U8_MIN_STORE:        ret = std::shared_ptr<U8MinStoreInstr>(new U8MinStoreInstr); break;
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
    default: break;
  }
  if (args.size() > 0)
    ret->setArgs(args);
  return ret;
}

} // namespace antoum

#endif // ANTOUM_VPU_INSTR_H
