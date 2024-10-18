/*
 * Copyright (c) 2018-2024 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */
#ifndef _VPU_ISA_QUARK_H
#define _VPU_ISA_QUARK_H

#include "utils.h"
#include "quark/vpu_instr.h"
#include "dtype/fp16.h"

#define QUARK_VPU_GEN_COMPUTE(name, type) \
void name(size_t x, size_t y, size_t z, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  switch (alu) {\
    case AluType::INT32_ALU:\
    case AluType::FP32_ALU: f32##type(x, y, z); break;\
    case AluType::INT64_ALU:\
    case AluType::FP64_ALU: f64##type(x, y, z); break;\
    default: break;\
  }\
}
#define QUARK_VPU_GEN_COMPARE(name, type) \
void name(size_t x, size_t y, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  switch (alu) {\
    case AluType::INT32_ALU:\
    case AluType::FP32_ALU: f32##type(x, y);  break;\
    case AluType::INT64_ALU:\
    case AluType::FP64_ALU: f64##type(x, y);  break;\
    default: break;\
  }\
}
#define QUARK_VPU_GEN_COMPUTE_IMM(name, type) \
void name##i(size_t x, double y, size_t z, DataType dtype, AluType alu) {\
  CHECK(alu == AluType::FP32_ALU);\
  if (dtype != DTYPE_FP16 && y != std::floor(y))\
    dtype = DTYPE_BF16;\
  switch (dtype) {\
    case DTYPE_FP32:\
    case DTYPE_FP8_52:\
    case DTYPE_FP8_43:\
    case DTYPE_FP4:\
    case DTYPE_BF16:\
      updateFs(DTYPE_BF16, alu);\
      bf16##type##i(x, bfloat16(y).value, z);\
      break;\
    case DTYPE_FP16:\
      updateFs(DTYPE_FP16, alu);\
      f16##type##i(x, fp16(y).value, z);\
      break;\
    case DTYPE_INT8:\
    case DTYPE_UINT8:\
    case DTYPE_INT4:\
    case DTYPE_INT16:\
      updateFs(DTYPE_INT16, alu);\
      i16##type##i(x, y, z);\
      break;\
    default:\
      break;\
  }\
}
#define QUARK_VPU_GEN_COMPARE_IMM(name, type) \
void name##i(size_t x, float y, DataType dtype, AluType alu) {\
  CHECK(alu == AluType::FP32_ALU);\
  if (dtype != DTYPE_FP16 && y != std::floor(y))\
    dtype = DTYPE_BF16;\
  switch (dtype) {\
    case DTYPE_FP32:\
    case DTYPE_FP8_52:\
    case DTYPE_FP8_43:\
    case DTYPE_FP4:\
    case DTYPE_BF16:\
      updateFs(DTYPE_BF16, alu);\
      bf16##type##i(x, bfloat16(y).value);\
      break;\
    case DTYPE_FP16:\
      updateFs(DTYPE_FP16, alu);\
      f16##type##i(x, fp16(y).value);\
      break;\
    case DTYPE_INT8:\
    case DTYPE_UINT8:\
    case DTYPE_INT4:\
    case DTYPE_INT16:\
      updateFs(DTYPE_INT16, alu);\
      i16##type##i(x, y);\
      break;\
    default:\
      break;\
  }\
}
#define QUARK_VPU_GEN_DLOAD_COMPUTE(type) \
void dload##type(size_t ldOffset, size_t ld2Offset, size_t z, DataType dtype, AluType alu) {\
  size_t offset = adjustOffset(loadHelper, loadHelper2, ldOffset, ld2Offset)/64;\
  updateFs(dtype, alu);\
  switch (dtype) {\
    case DTYPE_INT8:    i8Dload##type(offset, z);   break;\
    case DTYPE_UINT8:   u8Dload##type(offset, z);   break;\
    case DTYPE_BF16:    f16Dload##type(offset, z);  break;\
    case DTYPE_FP32:    f32Dload##type(offset, z);  break;\
    case DTYPE_INT16:   i8Dload##type(offset, z);   break;\
    case DTYPE_INT32:   i32Dload##type(offset, z);  break;\
    case DTYPE_FP8_52:  f16Dload##type(offset, z);  break;\
    case DTYPE_FP8_43:  f32Dload##type(offset, z);  break;\
    case DTYPE_FP16:    f16Dload##type(offset, z);  break;\
    case DTYPE_FP64:    f32Dload##type(offset, z);  break;\
    case DTYPE_INT4:    i8Dload##type(offset, z);   break;\
    case DTYPE_INT64:   i32Dload##type(offset, z);  break;\
    case DTYPE_FP4:     f16Dload##type(offset, z);  break;\
    default: break;\
  }\
}
#define QUARK_VPU_GEN_LOAD_COMPUTE(type) \
void load##type(size_t offset, size_t y, size_t z, DataType dtype, AluType alu) {\
  offset = adjustOffset(loadHelper, offset)/64;\
  updateFs(dtype, alu);\
  switch (dtype) {\
    case DTYPE_INT8:    i8Load##type(offset, y, z);   break;\
    case DTYPE_UINT8:   u8Load##type(offset, y, z);   break;\
    case DTYPE_BF16:    f16Load##type(offset, y, z);  break;\
    case DTYPE_FP32:    f32Load##type(offset, y, z);  break;\
    case DTYPE_INT16:   i8Load##type(offset, y, z);   break;\
    case DTYPE_INT32:   i32Load##type(offset, y, z);  break;\
    case DTYPE_FP8_52:  f16Load##type(offset, y, z);  break;\
    case DTYPE_FP8_43:  f32Load##type(offset, y, z);  break;\
    case DTYPE_FP16:    f16Load##type(offset, y, z);  break;\
    case DTYPE_FP64:    f32Load##type(offset, y, z);  break;\
    case DTYPE_INT4:    i8Load##type(offset, y, z);   break;\
    case DTYPE_INT64:   i32Load##type(offset, y, z);   break;\
    case DTYPE_FP4:     f16Load##type(offset, y, z);  break;\
    default: break;\
  }\
}
#define QUARK_VPU_GEN_COMPUTE_STORE(name, type) \
void name##Store(size_t y, size_t xz, size_t offset, DataType dtype, AluType alu) {\
  offset = adjustOffset(storeHelper, offset)/64;\
  updateFs(dtype, alu);\
  switch (dtype) {\
    case DTYPE_INT8:    i8##type##Store(y, xz, offset);   break;\
    case DTYPE_UINT8:   u8##type##Store(y, xz, offset);   break;\
    case DTYPE_BF16:    f16##type##Store(y, xz, offset);  break;\
    case DTYPE_FP32:    f32##type##Store(y, xz, offset);  break;\
    case DTYPE_INT16:   i8##type##Store(y, xz, offset);   break;\
    case DTYPE_INT32:   i32##type##Store(y, xz, offset);  break;\
    case DTYPE_FP8_52:  f16##type##Store(y, xz, offset);  break;\
    case DTYPE_FP8_43:  f32##type##Store(y, xz, offset);  break;\
    case DTYPE_FP16:    f16##type##Store(y, xz, offset);  break;\
    case DTYPE_FP64:    f32##type##Store(y, xz, offset);  break;\
    case DTYPE_INT4:    i8##type##Store(y, xz, offset);   break;\
    case DTYPE_INT64:   i32##type##Store(y, xz, offset);   break;\
    case DTYPE_FP4:     f16##type##Store(y, xz, offset);  break;\
    default: break;\
  }\
}
#define QUARK_VPU_GEN_DLOAD_COMPUTE_STORE(type) \
void dload##type##Store(size_t ldOffset, size_t ld2Offset, size_t stOffset, DataType dtype, AluType alu) {\
  size_t offset = adjustOffset(loadHelper, loadHelper2, storeHelper, ldOffset, ld2Offset, stOffset)/64;\
  updateFs(dtype, alu);\
  startAai();\
  switch (dtype) {\
    case DTYPE_INT8:    i8Dload##type##Store(offset);   break;\
    case DTYPE_UINT8:   u8Dload##type##Store(offset);   break;\
    case DTYPE_BF16:    f16Dload##type##Store(offset);  break;\
    case DTYPE_FP32:    f32Dload##type##Store(offset);  break;\
    case DTYPE_INT16:   i8Dload##type##Store(offset);   break;\
    case DTYPE_INT32:   i32Dload##type##Store(offset);  break;\
    case DTYPE_FP8_52:  f16Dload##type##Store(offset);  break;\
    case DTYPE_FP8_43:  f32Dload##type##Store(offset);  break;\
    case DTYPE_FP16:    f16Dload##type##Store(offset);  break;\
    case DTYPE_FP64:    f32Dload##type##Store(offset);  break;\
    case DTYPE_INT4:    i8Dload##type##Store(offset);   break;\
    case DTYPE_INT64:   i32Dload##type##Store(offset);   break;\
    case DTYPE_FP4:     f16Dload##type##Store(offset);  break;\
    default: break;\
  }\
  endAai();\
}
#define QUARK_VPU_GEN_LOAD_COMPUTE_STORE(type) \
void load##type##Store(size_t ldOffset, size_t stOffset, size_t y, DataType dtype, AluType alu) {\
  size_t offset = adjustOffset(loadHelper, storeHelper, ldOffset, stOffset)/64;\
  updateFs(dtype, alu);\
  startAai();\
  switch (dtype) {\
    case DTYPE_INT8:    i8Load##type##Store(offset, y);   break;\
    case DTYPE_UINT8:   u8Load##type##Store(offset, y);   break;\
    case DTYPE_BF16:    f16Load##type##Store(offset, y);  break;\
    case DTYPE_FP32:    f32Load##type##Store(offset, y);  break;\
    case DTYPE_INT16:   i8Load##type##Store(offset, y);   break;\
    case DTYPE_INT32:   i32Load##type##Store(offset, y);  break;\
    case DTYPE_FP8_52:  f16Load##type##Store(offset, y);  break;\
    case DTYPE_FP8_43:  f32Load##type##Store(offset, y);  break;\
    case DTYPE_FP16:    f16Load##type##Store(offset, y);  break;\
    case DTYPE_FP64:    f32Load##type##Store(offset, y);  break;\
    case DTYPE_INT4:    i8Load##type##Store(offset, y);   break;\
    case DTYPE_INT64:   i32Load##type##Store(offset, y);   break;\
    case DTYPE_FP4:     f16Load##type##Store(offset, y);  break;\
    default: break;\
  }\
  endAai();\
}
#define QUARK_VPU_GEN_BITOP(name, type) \
void name(size_t x, size_t y, size_t z, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  i32##type(x, y, z);\
}
#define QUARK_VPU_GEN_BITOP_IMM(name, type) \
void name##i(size_t x, double y, size_t z, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  i32##type##i(x, size_t(y), z);\
}
#define QUARK_VPU_GEN_SHIFT(name, type) \
void name(size_t x, size_t y, size_t z, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  switch (dtype) {\
    case DTYPE_INT32: i32##type(x, y, z); break;\
    case DTYPE_INT64: i64##type(x, y, z); break;\
    default: break;\
  }\
}
#define QUARK_VPU_GEN_SHIFT_IMM(name, type) \
void name##i(size_t x, double y, size_t z, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  switch (dtype) {\
    case DTYPE_INT32: i32##type##i(x, size_t(y), z);  break;\
    case DTYPE_INT64: i64##type##i(x, size_t(y), z);  break;\
    default: break;\
  }\
}
#define QUARK_VPU_GEN_DLOAD_BITOP(type) \
void dload##type(size_t ldOffset, size_t ld2Offset, size_t z, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  i32Dload##type(adjustOffset(loadHelper, loadHelper2, ldOffset, ld2Offset)/64, z);\
}
#define QUARK_VPU_GEN_LOAD_BITOP(type) \
void load##type(size_t offset, size_t y, size_t z, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  i32Load##type(adjustOffset(loadHelper, offset)/64, y, z);\
}
#define QUARK_VPU_GEN_BITOP_STORE(name, type) \
void name##Store(size_t y, size_t xz, size_t offset, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  i32##type##Store(y, xz, adjustOffset(storeHelper, offset)/64);\
}
#define QUARK_VPU_GEN_DLOAD_BITOP_STORE(type) \
void dload##type##Store(size_t ldOffset, size_t ld2Offset, size_t stOffset, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  i32Dload##type##Store(adjustOffset(loadHelper, loadHelper2, storeHelper, ldOffset, ld2Offset, stOffset)/64);\
}
#define QUARK_VPU_GEN_LOAD_BITOP_STORE(type) \
void load##type##Store(size_t ldOffset, size_t stOffset, size_t y, DataType dtype, AluType alu) {\
  updateFs(dtype, alu);\
  i32Load##type##Store(adjustOffset(loadHelper, storeHelper, ldOffset, stOffset)/64, y);\
}

namespace quark {
class BaseHelper {
public:
  BaseHelper() {}
  std::shared_ptr<BaseHelper> clone() {
    std::shared_ptr<BaseHelper> ret(new BaseHelper());
    *ret = *this;
    return ret;
  }
  void setParam(void *p) {
    param = p;
  }
  void setCallback(int32_t newBase, size_t csr, 
      std::function<void(int32_t, size_t, void *)> cb) {
    this->callback = cb;
    this->base = newBase;
    this->offset = 0;
    this->csr = csr;
    updateBase();
  }
  int32_t adjustOffset(int32_t ofs) {
    ofs &= ~0x3F;
    int32_t maxOffset = 64 << 16;
    int32_t newOffset = ofs;
    bool updateOffset = false;
    if (newOffset >= offset) {
      newOffset -= offset;
      if (newOffset >= maxOffset) {
        offset = ofs;
        newOffset = 0;
        updateOffset = true;
      }
    } else {
      offset = newOffset-maxOffset+64;
      newOffset -= offset;
      updateOffset = true;
    }
    if (updateOffset) {
      updateBase();
    }
    return newOffset;
  }
  int32_t getNewBase() {
    return base + offset;
  }
  void setNewBase(int32_t newBase) {
    offset = newBase-base;
    updateBase();
  }
  int32_t getBase() {
    return base;
  }

protected:
  void updateBase() {
    int32_t newBase = base+offset;
    newBase = (newBase<0) ?(newBase+kSizeSharedMem) :newBase;
    callback(newBase, csr, param);
  }
  std::function<void(int32_t, size_t, void *)> callback = nullptr;
  void *param = 0;
  int32_t base = 0;
  int32_t offset = 0;
  size_t csr = 0;
};
struct LoadDesc {
  int32_t base = -1;
  size_t stride = -1;
  size_t cmsk = -1;
  size_t bmsk = -1;
  size_t cbrd = -1;
  size_t bbrd = -1;
  size_t chbrd = -1;
  float dequan = NAN;
};
struct StoreDesc {
  int32_t base = -1;
  size_t stride = -1;
  size_t cmsk = -1;
  size_t bmsk = -1;
  float quan = NAN;
};

struct AaiConfig {
  bool enable = false;
  uint32_t stride = 0;
  uint32_t length = 0;
};

class VpuIsa {
public:
  VpuIsa();
  ~VpuIsa() = default;
  VpuIsa& operator=(const VpuIsa& isa) = delete;
  std::shared_ptr<VpuIsa> clone();
  void resetFs(bool force=false);
  void setLoadBaseReg(size_t baseReg, size_t tmpReg);
  void setLoad2BaseReg(size_t baseReg, size_t tmpReg);
  void setStoreBaseReg(size_t baseReg, size_t tmpReg);
  void setWeightMode(size_t src, size_t dst);

  QUARK_VPU_GEN_COMPUTE(add, Add)
  QUARK_VPU_GEN_COMPUTE(sub, Sub)
  QUARK_VPU_GEN_COMPUTE(mul, Mul)
  QUARK_VPU_GEN_COMPUTE(max, Max)
  QUARK_VPU_GEN_COMPUTE(min, Min)
  QUARK_VPU_GEN_COMPUTE(sel, Sel)
  QUARK_VPU_GEN_COMPUTE_IMM(add, Add)
  QUARK_VPU_GEN_COMPUTE_IMM(sub, Sub)
  QUARK_VPU_GEN_COMPUTE_IMM(mul, Mul)
  QUARK_VPU_GEN_COMPUTE_IMM(max, Max)
  QUARK_VPU_GEN_COMPUTE_IMM(min, Min)
  QUARK_VPU_GEN_COMPUTE_IMM(sel, Sel)
  QUARK_VPU_GEN_BITOP(and_,  And)
  QUARK_VPU_GEN_BITOP(or_,   Or)
  QUARK_VPU_GEN_BITOP(not_,  Not)
  QUARK_VPU_GEN_BITOP(xor_,  Xor)
  QUARK_VPU_GEN_BITOP(xnor_, Xnor)
  QUARK_VPU_GEN_SHIFT(shiftl, Shiftl)
  QUARK_VPU_GEN_SHIFT(shiftr, Shiftr)
  QUARK_VPU_GEN_BITOP_IMM(and,  And)
  QUARK_VPU_GEN_BITOP_IMM(or,   Or)
  QUARK_VPU_GEN_BITOP_IMM(not,  Not)
  QUARK_VPU_GEN_BITOP_IMM(xor,  Xor)
  QUARK_VPU_GEN_BITOP_IMM(xnor, Xnor)
  QUARK_VPU_GEN_SHIFT_IMM(shiftl, Shiftl)
  QUARK_VPU_GEN_SHIFT_IMM(shiftr, Shiftr)
  QUARK_VPU_GEN_COMPARE(greater,      Greater)
  QUARK_VPU_GEN_COMPARE(greaterEqual, GreaterEqual)
  QUARK_VPU_GEN_COMPARE(less,         Less)
  QUARK_VPU_GEN_COMPARE(lessEqual,    LessEqual)
  QUARK_VPU_GEN_COMPARE(equal,        Equal)
  QUARK_VPU_GEN_COMPARE_IMM(greater,      Greater)
  QUARK_VPU_GEN_COMPARE_IMM(greaterEqual, GreaterEqual)
  QUARK_VPU_GEN_COMPARE_IMM(less,         Less)
  QUARK_VPU_GEN_COMPARE_IMM(lessEqual,    LessEqual)
  QUARK_VPU_GEN_COMPARE_IMM(equal,        Equal)
  QUARK_VPU_GEN_DLOAD_COMPUTE(Add)
  QUARK_VPU_GEN_DLOAD_COMPUTE(Sub)
  QUARK_VPU_GEN_DLOAD_COMPUTE(Mul)
  QUARK_VPU_GEN_DLOAD_COMPUTE(Max)
  QUARK_VPU_GEN_DLOAD_COMPUTE(Min)
  QUARK_VPU_GEN_LOAD_COMPUTE(Add)
  QUARK_VPU_GEN_LOAD_COMPUTE(Sub)
  QUARK_VPU_GEN_LOAD_COMPUTE(Mul)
  QUARK_VPU_GEN_LOAD_COMPUTE(Max)
  QUARK_VPU_GEN_LOAD_COMPUTE(Min)
  QUARK_VPU_GEN_COMPUTE_STORE(add, Add)
  QUARK_VPU_GEN_COMPUTE_STORE(sub, Sub)
  QUARK_VPU_GEN_COMPUTE_STORE(mul, Mul)
  QUARK_VPU_GEN_COMPUTE_STORE(max, Max)
  QUARK_VPU_GEN_COMPUTE_STORE(min, Min)
  QUARK_VPU_GEN_DLOAD_COMPUTE_STORE(Add)
  QUARK_VPU_GEN_DLOAD_COMPUTE_STORE(Sub)
  QUARK_VPU_GEN_DLOAD_COMPUTE_STORE(Mul)
  QUARK_VPU_GEN_DLOAD_COMPUTE_STORE(Max)
  QUARK_VPU_GEN_DLOAD_COMPUTE_STORE(Min)
  QUARK_VPU_GEN_LOAD_COMPUTE_STORE(Add)
  QUARK_VPU_GEN_LOAD_COMPUTE_STORE(Sub)
  QUARK_VPU_GEN_LOAD_COMPUTE_STORE(Mul)
  QUARK_VPU_GEN_LOAD_COMPUTE_STORE(Max)
  QUARK_VPU_GEN_LOAD_COMPUTE_STORE(Min)
  QUARK_VPU_GEN_DLOAD_BITOP(And)
  QUARK_VPU_GEN_DLOAD_BITOP(Or)
  QUARK_VPU_GEN_DLOAD_BITOP(Not)
  QUARK_VPU_GEN_DLOAD_BITOP(Xor)
  QUARK_VPU_GEN_DLOAD_BITOP(Xnor)
  QUARK_VPU_GEN_LOAD_BITOP(And)
  QUARK_VPU_GEN_LOAD_BITOP(Or)
  QUARK_VPU_GEN_LOAD_BITOP(Not)
  QUARK_VPU_GEN_LOAD_BITOP(Xor)
  QUARK_VPU_GEN_LOAD_BITOP(Xnor)
  QUARK_VPU_GEN_BITOP_STORE(and,  And)
  QUARK_VPU_GEN_BITOP_STORE(or,   Or)
  QUARK_VPU_GEN_BITOP_STORE(not,  Not)
  QUARK_VPU_GEN_BITOP_STORE(xor,  Xor)
  QUARK_VPU_GEN_BITOP_STORE(xnor, Xnor)
  QUARK_VPU_GEN_DLOAD_BITOP_STORE(And)
  QUARK_VPU_GEN_DLOAD_BITOP_STORE(Or)
  QUARK_VPU_GEN_DLOAD_BITOP_STORE(Not)
  QUARK_VPU_GEN_DLOAD_BITOP_STORE(Xor)
  QUARK_VPU_GEN_DLOAD_BITOP_STORE(Xnor)
  QUARK_VPU_GEN_LOAD_BITOP_STORE(And)
  QUARK_VPU_GEN_LOAD_BITOP_STORE(Or)
  QUARK_VPU_GEN_LOAD_BITOP_STORE(Not)
  QUARK_VPU_GEN_LOAD_BITOP_STORE(Xor)
  QUARK_VPU_GEN_LOAD_BITOP_STORE(Xnor)
  void setLoadBase(size_t baseAddr, size_t csr=0);
  void setLoad2Base(size_t baseAddr, size_t csr=0);
  void setStoreBase(size_t baseAddr, size_t csr=0);
  void load(size_t offset, size_t z, DataType dtype, AluType alu);
  void dload(size_t ldOffset, size_t ld2Offset, size_t z, size_t z2, DataType dtype, AluType alu);
  void store(size_t x, size_t offset, DataType dtype, AluType alu);
  void movi(double y, size_t z, DataType dtype, AluType alu);
  void Li(double y, size_t z, DataType dtype, AluType alu);
  void sLi(size_t y, size_t z);
  void sSetcfg(size_t rs, size_t rt, size_t sp_rd0, size_t sp_rd1);
  void sSetcfg(size_t rs, size_t sp_rd0);
  void sSetLabel(std::string label);
  size_t sGetLabel(std::string label);
  void sClearLabels();
  void sJump(std::string label);
  void sJal(std::string label);
  void sBne(size_t x, size_t y, std::string label);
  void sBeq(size_t x, size_t y, std::string label);
  void sBlt(size_t x, size_t y, std::string label);
  void sBltu(size_t x, size_t y, std::string label);
  void sBge(size_t x, size_t y, std::string label);
  void sBgeu(size_t x, size_t y, std::string label);
  void setAai(size_t stride, size_t length);

  void enableAai(const std::vector<size_t> &steps, size_t length);
  void startAai();
  void endAai();
  size_t getCmdSize();
  size_t evalCycles();
  size_t evalCycles(const std::vector<std::shared_ptr<spu::InstrInterface>> &instrs) const;
  const std::vector<std::shared_ptr<spu::InstrInterface>>& getInstrs(bool end=true);
  bool loopExists();
  void getSrcBaseTable(std::vector<std::pair<uint32_t,uint32_t>> &baseTable, const std::map<uint32_t,uint32_t> &baseFilter);
  void getDstBaseTable(std::vector<std::pair<uint32_t,uint32_t>> &baseTable, const std::map<uint32_t,uint32_t> &baseFilter);

protected:
  size_t getPc(size_t pc) const { return pc%(kSizeVpuCmdMem/kSizePerVpuCmd); }
  size_t adjustOffset(std::shared_ptr<BaseHelper> helper, size_t offset);
  size_t adjustOffset(std::shared_ptr<BaseHelper> ld, std::shared_ptr<BaseHelper> st, size_t ldOffset, size_t stOffset);
  size_t adjustOffset(std::shared_ptr<BaseHelper> ld, std::shared_ptr<BaseHelper> ld2, std::shared_ptr<BaseHelper> st, 
      size_t ldOffset, size_t ld2Offset, size_t stOffset);
  void updateRfSel(size_t rs, size_t rt, size_t rd);
  size_t encodeFs();
  void updateFs(DataType dtype, AluType alu);
  template<typename T>
  void addIsa(const std::vector<size_t> &args);
  void addIsa(std::shared_ptr<VpuInstr> instr);
  void removeRedundantCmds();

protected:
  std::shared_ptr<BaseHelper> loadHelper;
  std::shared_ptr<BaseHelper> loadHelper2;
  std::shared_ptr<BaseHelper> storeHelper;
  LoadDesc loadDesc;
  LoadDesc loadDesc2;
  StoreDesc storeDesc;
  std::vector<std::shared_ptr<spu::InstrInterface>> instrs;
  bool rsHigh = false;
  bool rtHigh = false;
  bool rdHigh = false;
  bool fs0 = false;
  bool fs1 = false;
  bool fs2 = false;
  bool fs3 = false;
  uint32_t fs45 = 0;
  uint32_t fs67 = 0;
  bool redundantRemoverEn = true;
  std::map<std::string, size_t> labelMap;
  size_t ldBaseReg = -1;
  size_t ld2BaseReg = -1;
  size_t stBaseReg = -1;
  size_t ldTmpReg = -1;
  size_t ld2TmpReg = -1;
  size_t stTmpReg = -1;
  std::vector<size_t> aaiSteps;
  size_t aaiLength = 0;

public:
  //void setLoadBase(size_t baseAddr) {
  //  addIsa<SetLoadBaseInstr>({baseAddr&0xffffff});
  //}
  //void setLoad2Base(size_t baseAddr) {
  //  addIsa<SetLoad2BaseInstr>({baseAddr&0xffffff});
  //}
  //void setStoreBase(size_t baseAddr) {
  //  addIsa<SetStoreBaseInstr>({baseAddr&0xffffff});
  //}
  void setLoadGlb(size_t stride, size_t clusterMask, size_t bankMask, size_t clusterBroadcast, size_t bankBroadcast, size_t channelBroadcast, size_t csr=0) {
    addIsa<SetLoadGlbInstr>({stride&0x7, clusterMask&0xff, bankMask&0xf, clusterBroadcast&0x7, bankBroadcast&0x7, channelBroadcast&0x3, csr&0x1});
  }
  void setStoreGlb(size_t stride, size_t clusterMask, size_t bankMask, size_t csr=0) {
    addIsa<SetStoreGlbInstr>({stride&0x7, clusterMask&0xffff, bankMask&0xf, csr&0x1});
  }
  void setQuantize(float quantize, size_t csr=0) {
    addIsa<SetQuantizeInstr>({bfloat16(quantize).value, csr&0x1});
  }
  void setDequantize(float dequantize, size_t csr=0) {
    bfloat24 x;
    x.fp32 = f32_to_bf24(dequantize);
    addIsa<SetDequantizeInstr>({x.value >> 8, csr&0x1});
  }
  void setDequantize2(size_t dequantize, size_t csr=0) {
    bfloat24 x;
    x.fp32 = f32_to_bf24(dequantize);
    addIsa<SetDequantize2Instr>({x.value >> 8, csr&0x1});
  }
  void setFs(size_t fsSel, size_t csr=0) {
    addIsa<SetFsInstr>({fsSel&0xff, csr&0x1});
  }
  void setAai(size_t enable, size_t mode, size_t strideLength) {
    addIsa<SetAaiInstr>({enable&0x1, mode&0xf, strideLength&0xffff});
  }
  void vpuNop() {
    addIsa<VpuNopInstr>({});
  }
  void vpuEnd(size_t addressAlign=0) {
    addIsa<VpuEndInstr>({addressAlign&0x1});
  }
  void i8Dload(size_t offset, size_t rt, size_t rs) {
    addIsa<I8DloadInstr>({offset&0xffff, rt&0x3, rs&0x3});
  }
  void i8Load(size_t offset, size_t rt) {
    addIsa<I8LoadInstr>({offset&0xffff, rt&0x3});
  }
  void u8Dload(size_t offset, size_t rt, size_t rs) {
    addIsa<U8DloadInstr>({offset&0xffff, rt&0x3, rs&0x3});
  }
  void u8Load(size_t offset, size_t rt) {
    addIsa<U8LoadInstr>({offset&0xffff, rt&0x3});
  }
  void f16Dload(size_t offset, size_t rt, size_t rs) {
    addIsa<F16DloadInstr>({offset&0xffff, rt&0x3, rs&0x3});
  }
  void f16Load(size_t offset, size_t rt) {
    addIsa<F16LoadInstr>({offset&0xffff, rt&0x3});
  }
  void f32Dload(size_t offset, size_t rt, size_t rs) {
    addIsa<F32DloadInstr>({offset&0xffff, rt&0x3, rs&0x3});
  }
  void f32Load(size_t offset, size_t rt) {
    addIsa<F32LoadInstr>({offset&0xffff, rt&0x3});
  }
  void i32Dload(size_t offset, size_t rt, size_t rs) {
    addIsa<I32DloadInstr>({offset&0xffff, rt&0x3, rs&0x3});
  }
  void i32Load(size_t offset, size_t rt) {
    addIsa<I32LoadInstr>({offset&0xffff, rt&0x3});
  }
  void i8Store(size_t rt, size_t offset) {
    addIsa<I8StoreInstr>({rt&0x3, offset&0xffff});
  }
  void u8Store(size_t rt, size_t offset) {
    addIsa<U8StoreInstr>({rt&0x3, offset&0xffff});
  }
  void f16Store(size_t rt, size_t offset) {
    addIsa<F16StoreInstr>({rt&0x3, offset&0xffff});
  }
  void f32Store(size_t rt, size_t offset) {
    addIsa<F32StoreInstr>({rt&0x3, offset&0xffff});
  }
  void i32Store(size_t rt, size_t offset) {
    addIsa<I32StoreInstr>({rt&0x3, offset&0xffff});
  }
  void f32Add(size_t rs, size_t rt, size_t rd) {
    addIsa<F32AddInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f32Sub(size_t rs, size_t rt, size_t rd) {
    addIsa<F32SubInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f32Mul(size_t rs, size_t rt, size_t rd) {
    addIsa<F32MulInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f32Max(size_t rs, size_t rt, size_t rd) {
    addIsa<F32MaxInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f32Min(size_t rs, size_t rt, size_t rd) {
    addIsa<F32MinInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f32Greater(size_t rs, size_t rt) {
    addIsa<F32GreaterInstr>({rs&0x7, rt&0x7});
  }
  void f32GreaterEqual(size_t rs, size_t rt) {
    addIsa<F32GreaterEqualInstr>({rs&0x7, rt&0x7});
  }
  void f32Less(size_t rs, size_t rt) {
    addIsa<F32LessInstr>({rs&0x7, rt&0x7});
  }
  void f32LessEqual(size_t rs, size_t rt) {
    addIsa<F32LessEqualInstr>({rs&0x7, rt&0x7});
  }
  void f32Equal(size_t rs, size_t rt) {
    addIsa<F32EqualInstr>({rs&0x7, rt&0x7});
  }
  void f32Sel(size_t rs, size_t rt, size_t rd) {
    addIsa<F32SelInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i32And(size_t rs, size_t rt, size_t rd) {
    addIsa<I32AndInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i32Or(size_t rs, size_t rt, size_t rd) {
    addIsa<I32OrInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i32Not(size_t rs, size_t rt, size_t rd) {
    addIsa<I32NotInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i32Xor(size_t rs, size_t rt, size_t rd) {
    addIsa<I32XorInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i32Xnor(size_t rs, size_t rt, size_t rd) {
    addIsa<I32XnorInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void bf16Addi(size_t rs, size_t imm, size_t rd) {
    addIsa<Bf16AddiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void bf16Subi(size_t rs, size_t imm, size_t rd) {
    addIsa<Bf16SubiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void bf16Muli(size_t rs, size_t imm, size_t rd) {
    addIsa<Bf16MuliInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void bf16Maxi(size_t rs, size_t imm, size_t rd) {
    addIsa<Bf16MaxiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void bf16Mini(size_t rs, size_t imm, size_t rd) {
    addIsa<Bf16MiniInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void bf16Movi(size_t imm, size_t rd) {
    addIsa<Bf16MoviInstr>({imm&0xffff, rd&0x7});
  }
  void bf16Greateri(size_t rs, size_t imm) {
    addIsa<Bf16GreateriInstr>({rs&0x7, imm&0xffff});
  }
  void bf16GreaterEquali(size_t rs, size_t imm) {
    addIsa<Bf16GreaterEqualiInstr>({rs&0x7, imm&0xffff});
  }
  void bf16Lessi(size_t rs, size_t imm) {
    addIsa<Bf16LessiInstr>({rs&0x7, imm&0xffff});
  }
  void bf16LessEquali(size_t rs, size_t imm) {
    addIsa<Bf16LessEqualiInstr>({rs&0x7, imm&0xffff});
  }
  void bf16Equali(size_t rs, size_t imm) {
    addIsa<Bf16EqualiInstr>({rs&0x7, imm&0xffff});
  }
  void bf16Seli(size_t rs, size_t imm, size_t rd) {
    addIsa<Bf16SeliInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void f16Addi(size_t rs, size_t imm, size_t rd) {
    addIsa<F16AddiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void f16Subi(size_t rs, size_t imm, size_t rd) {
    addIsa<F16SubiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void f16Muli(size_t rs, size_t imm, size_t rd) {
    addIsa<F16MuliInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void f16Maxi(size_t rs, size_t imm, size_t rd) {
    addIsa<F16MaxiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void f16Mini(size_t rs, size_t imm, size_t rd) {
    addIsa<F16MiniInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void f16Movi(size_t imm, size_t rd) {
    addIsa<F16MoviInstr>({imm&0xffff, rd&0x7});
  }
  void f16Greateri(size_t rs, size_t imm) {
    addIsa<F16GreateriInstr>({rs&0x7, imm&0xffff});
  }
  void f16GreaterEquali(size_t rs, size_t imm) {
    addIsa<F16GreaterEqualiInstr>({rs&0x7, imm&0xffff});
  }
  void f16Lessi(size_t rs, size_t imm) {
    addIsa<F16LessiInstr>({rs&0x7, imm&0xffff});
  }
  void f16LessEquali(size_t rs, size_t imm) {
    addIsa<F16LessEqualiInstr>({rs&0x7, imm&0xffff});
  }
  void f16Equali(size_t rs, size_t imm) {
    addIsa<F16EqualiInstr>({rs&0x7, imm&0xffff});
  }
  void f16Seli(size_t rs, size_t imm, size_t rd) {
    addIsa<F16SeliInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i16Addi(size_t rs, size_t imm, size_t rd) {
    addIsa<I16AddiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i16Subi(size_t rs, size_t imm, size_t rd) {
    addIsa<I16SubiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i16Muli(size_t rs, size_t imm, size_t rd) {
    addIsa<I16MuliInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i16Maxi(size_t rs, size_t imm, size_t rd) {
    addIsa<I16MaxiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i16Mini(size_t rs, size_t imm, size_t rd) {
    addIsa<I16MiniInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i16Movi(size_t imm, size_t rd) {
    addIsa<I16MoviInstr>({imm&0xffff, rd&0x7});
  }
  void i16Greateri(size_t rs, size_t imm) {
    addIsa<I16GreateriInstr>({rs&0x7, imm&0xffff});
  }
  void i16GreaterEquali(size_t rs, size_t imm) {
    addIsa<I16GreaterEqualiInstr>({rs&0x7, imm&0xffff});
  }
  void i16Lessi(size_t rs, size_t imm) {
    addIsa<I16LessiInstr>({rs&0x7, imm&0xffff});
  }
  void i16LessEquali(size_t rs, size_t imm) {
    addIsa<I16LessEqualiInstr>({rs&0x7, imm&0xffff});
  }
  void i16Equali(size_t rs, size_t imm) {
    addIsa<I16EqualiInstr>({rs&0x7, imm&0xffff});
  }
  void i16Seli(size_t rs, size_t imm, size_t rd) {
    addIsa<I16SeliInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void f32Lui(size_t imm, size_t rd) {
    addIsa<F32LuiInstr>({imm&0xffff, rd&0x7});
  }
  void f32Li(size_t imm, size_t rd) {
    addIsa<F32LiInstr>({imm&0xffff, rd&0x7});
  }
  void i32Andi(size_t rs, size_t imm, size_t rd) {
    addIsa<I32AndiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i32Ori(size_t rs, size_t imm, size_t rd) {
    addIsa<I32OriInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i32Noti(size_t rs, size_t imm, size_t rd) {
    addIsa<I32NotiInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i32Xori(size_t rs, size_t imm, size_t rd) {
    addIsa<I32XoriInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i32Xnori(size_t rs, size_t imm, size_t rd) {
    addIsa<I32XnoriInstr>({rs&0x7, imm&0xffff, rd&0x7});
  }
  void i32Shiftl(size_t rs, size_t rt, size_t rd) {
    addIsa<I32ShiftlInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i32Shiftr(size_t rs, size_t rt, size_t rd) {
    addIsa<I32ShiftrInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i32Shiftli(size_t rs, size_t imm, size_t rd) {
    addIsa<I32ShiftliInstr>({rs&0x7, imm&0xff, rd&0x7});
  }
  void i32Shiftri(size_t rs, size_t imm, size_t rd) {
    addIsa<I32ShiftriInstr>({rs&0x7, imm&0xff, rd&0x7});
  }
  void f64Add(size_t rs, size_t rt, size_t rd) {
    addIsa<F64AddInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f64Sub(size_t rs, size_t rt, size_t rd) {
    addIsa<F64SubInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f64Mul(size_t rs, size_t rt, size_t rd) {
    addIsa<F64MulInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f64Max(size_t rs, size_t rt, size_t rd) {
    addIsa<F64MaxInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f64Min(size_t rs, size_t rt, size_t rd) {
    addIsa<F64MinInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void f64Greater(size_t rs, size_t rt) {
    addIsa<F64GreaterInstr>({rs&0x7, rt&0x7});
  }
  void f64GreaterEqual(size_t rs, size_t rt) {
    addIsa<F64GreaterEqualInstr>({rs&0x7, rt&0x7});
  }
  void f64Less(size_t rs, size_t rt) {
    addIsa<F64LessInstr>({rs&0x7, rt&0x7});
  }
  void f64LessEqual(size_t rs, size_t rt) {
    addIsa<F64LessEqualInstr>({rs&0x7, rt&0x7});
  }
  void f64Equal(size_t rs, size_t rt) {
    addIsa<F64EqualInstr>({rs&0x7, rt&0x7});
  }
  void f64Sel(size_t rs, size_t rt, size_t rd) {
    addIsa<F64SelInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i64Shiftl(size_t rs, size_t rt, size_t rd) {
    addIsa<I64ShiftlInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i64Shiftr(size_t rs, size_t rt, size_t rd) {
    addIsa<I64ShiftrInstr>({rs&0x7, rt&0x7, rd&0x7});
  }
  void i64Shiftli(size_t rs, size_t imm, size_t rd) {
    addIsa<I64ShiftliInstr>({rs&0x7, imm&0xff, rd&0x7});
  }
  void i64Shiftri(size_t rs, size_t imm, size_t rd) {
    addIsa<I64ShiftriInstr>({rs&0x7, imm&0xff, rd&0x7});
  }
  void i8DloadAdd(size_t offset, size_t rd) {
    addIsa<I8DloadAddInstr>({offset&0xffff, rd&0x3});
  }
  void i8DloadSub(size_t offset, size_t rd) {
    addIsa<I8DloadSubInstr>({offset&0xffff, rd&0x3});
  }
  void i8DloadMul(size_t offset, size_t rd) {
    addIsa<I8DloadMulInstr>({offset&0xffff, rd&0x3});
  }
  void i8DloadMax(size_t offset, size_t rd) {
    addIsa<I8DloadMaxInstr>({offset&0xffff, rd&0x3});
  }
  void i8DloadMin(size_t offset, size_t rd) {
    addIsa<I8DloadMinInstr>({offset&0xffff, rd&0x3});
  }
  void i8LoadAdd(size_t offset, size_t rs, size_t rd) {
    addIsa<I8LoadAddInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i8LoadSub(size_t offset, size_t rs, size_t rd) {
    addIsa<I8LoadSubInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i8LoadMul(size_t offset, size_t rs, size_t rd) {
    addIsa<I8LoadMulInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i8LoadMax(size_t offset, size_t rs, size_t rd) {
    addIsa<I8LoadMaxInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i8LoadMin(size_t offset, size_t rs, size_t rd) {
    addIsa<I8LoadMinInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void u8DloadAdd(size_t offset, size_t rd) {
    addIsa<U8DloadAddInstr>({offset&0xffff, rd&0x3});
  }
  void u8DloadSub(size_t offset, size_t rd) {
    addIsa<U8DloadSubInstr>({offset&0xffff, rd&0x3});
  }
  void u8DloadMul(size_t offset, size_t rd) {
    addIsa<U8DloadMulInstr>({offset&0xffff, rd&0x3});
  }
  void u8DloadMax(size_t offset, size_t rd) {
    addIsa<U8DloadMaxInstr>({offset&0xffff, rd&0x3});
  }
  void u8DloadMin(size_t offset, size_t rd) {
    addIsa<U8DloadMinInstr>({offset&0xffff, rd&0x3});
  }
  void u8LoadAdd(size_t offset, size_t rs, size_t rd) {
    addIsa<U8LoadAddInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void u8LoadSub(size_t offset, size_t rs, size_t rd) {
    addIsa<U8LoadSubInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void u8LoadMul(size_t offset, size_t rs, size_t rd) {
    addIsa<U8LoadMulInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void u8LoadMax(size_t offset, size_t rs, size_t rd) {
    addIsa<U8LoadMaxInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void u8LoadMin(size_t offset, size_t rs, size_t rd) {
    addIsa<U8LoadMinInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f16DloadAdd(size_t offset, size_t rd) {
    addIsa<F16DloadAddInstr>({offset&0xffff, rd&0x3});
  }
  void f16DloadSub(size_t offset, size_t rd) {
    addIsa<F16DloadSubInstr>({offset&0xffff, rd&0x3});
  }
  void f16DloadMul(size_t offset, size_t rd) {
    addIsa<F16DloadMulInstr>({offset&0xffff, rd&0x3});
  }
  void f16DloadMax(size_t offset, size_t rd) {
    addIsa<F16DloadMaxInstr>({offset&0xffff, rd&0x3});
  }
  void f16DloadMin(size_t offset, size_t rd) {
    addIsa<F16DloadMinInstr>({offset&0xffff, rd&0x3});
  }
  void f16LoadAdd(size_t offset, size_t rs, size_t rd) {
    addIsa<F16LoadAddInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f16LoadSub(size_t offset, size_t rs, size_t rd) {
    addIsa<F16LoadSubInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f16LoadMul(size_t offset, size_t rs, size_t rd) {
    addIsa<F16LoadMulInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f16LoadMax(size_t offset, size_t rs, size_t rd) {
    addIsa<F16LoadMaxInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f16LoadMin(size_t offset, size_t rs, size_t rd) {
    addIsa<F16LoadMinInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f32DloadAdd(size_t offset, size_t rd) {
    addIsa<F32DloadAddInstr>({offset&0xffff, rd&0x3});
  }
  void f32DloadSub(size_t offset, size_t rd) {
    addIsa<F32DloadSubInstr>({offset&0xffff, rd&0x3});
  }
  void f32DloadMul(size_t offset, size_t rd) {
    addIsa<F32DloadMulInstr>({offset&0xffff, rd&0x3});
  }
  void f32DloadMax(size_t offset, size_t rd) {
    addIsa<F32DloadMaxInstr>({offset&0xffff, rd&0x3});
  }
  void f32DloadMin(size_t offset, size_t rd) {
    addIsa<F32DloadMinInstr>({offset&0xffff, rd&0x3});
  }
  void f32LoadAdd(size_t offset, size_t rs, size_t rd) {
    addIsa<F32LoadAddInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f32LoadSub(size_t offset, size_t rs, size_t rd) {
    addIsa<F32LoadSubInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f32LoadMul(size_t offset, size_t rs, size_t rd) {
    addIsa<F32LoadMulInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f32LoadMax(size_t offset, size_t rs, size_t rd) {
    addIsa<F32LoadMaxInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void f32LoadMin(size_t offset, size_t rs, size_t rd) {
    addIsa<F32LoadMinInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32DloadAdd(size_t offset, size_t rd) {
    addIsa<I32DloadAddInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadSub(size_t offset, size_t rd) {
    addIsa<I32DloadSubInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadMul(size_t offset, size_t rd) {
    addIsa<I32DloadMulInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadMax(size_t offset, size_t rd) {
    addIsa<I32DloadMaxInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadMin(size_t offset, size_t rd) {
    addIsa<I32DloadMinInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadAnd(size_t offset, size_t rd) {
    addIsa<I32DloadAndInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadOr(size_t offset, size_t rd) {
    addIsa<I32DloadOrInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadNot(size_t offset, size_t rd) {
    addIsa<I32DloadNotInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadXor(size_t offset, size_t rd) {
    addIsa<I32DloadXorInstr>({offset&0xffff, rd&0x3});
  }
  void i32DloadXnor(size_t offset, size_t rd) {
    addIsa<I32DloadXnorInstr>({offset&0xffff, rd&0x3});
  }
  void i32LoadAdd(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadAddInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadSub(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadSubInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadMul(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadMulInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadMax(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadMaxInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadMin(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadMinInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadAnd(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadAndInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadOr(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadOrInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadNot(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadNotInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadXor(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadXorInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i32LoadXnor(size_t offset, size_t rs, size_t rd) {
    addIsa<I32LoadXnorInstr>({offset&0xffff, rs&0x3, rd&0x3});
  }
  void i8AddStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I8AddStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i8SubStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I8SubStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i8MulStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I8MulStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i8MaxStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I8MaxStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i8MinStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I8MinStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void u8AddStore(size_t rs, size_t rd, size_t offset) {
    addIsa<U8AddStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void u8SubStore(size_t rs, size_t rd, size_t offset) {
    addIsa<U8SubStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void u8MulStore(size_t rs, size_t rd, size_t offset) {
    addIsa<U8MulStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void u8MaxStore(size_t rs, size_t rd, size_t offset) {
    addIsa<U8MaxStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void u8MinStore(size_t rs, size_t rd, size_t offset) {
    addIsa<U8MinStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f16AddStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F16AddStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f16SubStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F16SubStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f16MulStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F16MulStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f16MaxStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F16MaxStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f16MinStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F16MinStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f32AddStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F32AddStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f32SubStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F32SubStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f32MulStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F32MulStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f32MaxStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F32MaxStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void f32MinStore(size_t rs, size_t rd, size_t offset) {
    addIsa<F32MinStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32AddStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32AddStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32SubStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32SubStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32MulStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32MulStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32MaxStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32MaxStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32MinStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32MinStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32AndStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32AndStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32OrStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32OrStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32NotStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32NotStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32XorStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32XorStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i32XnorStore(size_t rs, size_t rd, size_t offset) {
    addIsa<I32XnorStoreInstr>({rs&0x3, rd&0x3, offset&0xffff});
  }
  void i8DloadAddStore(size_t offset) {
    addIsa<I8DloadAddStoreInstr>({offset&0xffff});
  }
  void i8DloadSubStore(size_t offset) {
    addIsa<I8DloadSubStoreInstr>({offset&0xffff});
  }
  void i8DloadMulStore(size_t offset) {
    addIsa<I8DloadMulStoreInstr>({offset&0xffff});
  }
  void i8DloadMaxStore(size_t offset) {
    addIsa<I8DloadMaxStoreInstr>({offset&0xffff});
  }
  void i8DloadMinStore(size_t offset) {
    addIsa<I8DloadMinStoreInstr>({offset&0xffff});
  }
  void i8LoadAddStore(size_t offset, size_t rt) {
    addIsa<I8LoadAddStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i8LoadSubStore(size_t offset, size_t rt) {
    addIsa<I8LoadSubStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i8LoadMulStore(size_t offset, size_t rt) {
    addIsa<I8LoadMulStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i8LoadMaxStore(size_t offset, size_t rt) {
    addIsa<I8LoadMaxStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i8LoadMinStore(size_t offset, size_t rt) {
    addIsa<I8LoadMinStoreInstr>({offset&0xffff, rt&0x3});
  }
  void u8DloadAddStore(size_t offset) {
    addIsa<U8DloadAddStoreInstr>({offset&0xffff});
  }
  void u8DloadSubStore(size_t offset) {
    addIsa<U8DloadSubStoreInstr>({offset&0xffff});
  }
  void u8DloadMulStore(size_t offset) {
    addIsa<U8DloadMulStoreInstr>({offset&0xffff});
  }
  void u8DloadMaxStore(size_t offset) {
    addIsa<U8DloadMaxStoreInstr>({offset&0xffff});
  }
  void u8DloadMinStore(size_t offset) {
    addIsa<U8DloadMinStoreInstr>({offset&0xffff});
  }
  void u8LoadAddStore(size_t offset, size_t rt) {
    addIsa<U8LoadAddStoreInstr>({offset&0xffff, rt&0x3});
  }
  void u8LoadSubStore(size_t offset, size_t rt) {
    addIsa<U8LoadSubStoreInstr>({offset&0xffff, rt&0x3});
  }
  void u8LoadMulStore(size_t offset, size_t rt) {
    addIsa<U8LoadMulStoreInstr>({offset&0xffff, rt&0x3});
  }
  void u8LoadMaxStore(size_t offset, size_t rt) {
    addIsa<U8LoadMaxStoreInstr>({offset&0xffff, rt&0x3});
  }
  void u8LoadMinStore(size_t offset, size_t rt) {
    addIsa<U8LoadMinStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f16DloadAddStore(size_t offset) {
    addIsa<F16DloadAddStoreInstr>({offset&0xffff});
  }
  void f16DloadSubStore(size_t offset) {
    addIsa<F16DloadSubStoreInstr>({offset&0xffff});
  }
  void f16DloadMulStore(size_t offset) {
    addIsa<F16DloadMulStoreInstr>({offset&0xffff});
  }
  void f16DloadMaxStore(size_t offset) {
    addIsa<F16DloadMaxStoreInstr>({offset&0xffff});
  }
  void f16DloadMinStore(size_t offset) {
    addIsa<F16DloadMinStoreInstr>({offset&0xffff});
  }
  void f16LoadAddStore(size_t offset, size_t rt) {
    addIsa<F16LoadAddStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f16LoadSubStore(size_t offset, size_t rt) {
    addIsa<F16LoadSubStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f16LoadMulStore(size_t offset, size_t rt) {
    addIsa<F16LoadMulStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f16LoadMaxStore(size_t offset, size_t rt) {
    addIsa<F16LoadMaxStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f16LoadMinStore(size_t offset, size_t rt) {
    addIsa<F16LoadMinStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f32DloadAddStore(size_t offset) {
    addIsa<F32DloadAddStoreInstr>({offset&0xffff});
  }
  void f32DloadSubStore(size_t offset) {
    addIsa<F32DloadSubStoreInstr>({offset&0xffff});
  }
  void f32DloadMulStore(size_t offset) {
    addIsa<F32DloadMulStoreInstr>({offset&0xffff});
  }
  void f32DloadMaxStore(size_t offset) {
    addIsa<F32DloadMaxStoreInstr>({offset&0xffff});
  }
  void f32DloadMinStore(size_t offset) {
    addIsa<F32DloadMinStoreInstr>({offset&0xffff});
  }
  void f32LoadAddStore(size_t offset, size_t rt) {
    addIsa<F32LoadAddStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f32LoadSubStore(size_t offset, size_t rt) {
    addIsa<F32LoadSubStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f32LoadMulStore(size_t offset, size_t rt) {
    addIsa<F32LoadMulStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f32LoadMaxStore(size_t offset, size_t rt) {
    addIsa<F32LoadMaxStoreInstr>({offset&0xffff, rt&0x3});
  }
  void f32LoadMinStore(size_t offset, size_t rt) {
    addIsa<F32LoadMinStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32DloadAddStore(size_t offset) {
    addIsa<I32DloadAddStoreInstr>({offset&0xffff});
  }
  void i32DloadSubStore(size_t offset) {
    addIsa<I32DloadSubStoreInstr>({offset&0xffff});
  }
  void i32DloadMulStore(size_t offset) {
    addIsa<I32DloadMulStoreInstr>({offset&0xffff});
  }
  void i32DloadMaxStore(size_t offset) {
    addIsa<I32DloadMaxStoreInstr>({offset&0xffff});
  }
  void i32DloadMinStore(size_t offset) {
    addIsa<I32DloadMinStoreInstr>({offset&0xffff});
  }
  void i32DloadAndStore(size_t offset) {
    addIsa<I32DloadAndStoreInstr>({offset&0xffff});
  }
  void i32DloadOrStore(size_t offset) {
    addIsa<I32DloadOrStoreInstr>({offset&0xffff});
  }
  void i32DloadNotStore(size_t offset) {
    addIsa<I32DloadNotStoreInstr>({offset&0xffff});
  }
  void i32DloadXorStore(size_t offset) {
    addIsa<I32DloadXorStoreInstr>({offset&0xffff});
  }
  void i32DloadXnorStore(size_t offset) {
    addIsa<I32DloadXnorStoreInstr>({offset&0xffff});
  }
  void i32LoadAddStore(size_t offset, size_t rt) {
    addIsa<I32LoadAddStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadSubStore(size_t offset, size_t rt) {
    addIsa<I32LoadSubStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadMulStore(size_t offset, size_t rt) {
    addIsa<I32LoadMulStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadMaxStore(size_t offset, size_t rt) {
    addIsa<I32LoadMaxStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadMinStore(size_t offset, size_t rt) {
    addIsa<I32LoadMinStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadAndStore(size_t offset, size_t rt) {
    addIsa<I32LoadAndStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadOrStore(size_t offset, size_t rt) {
    addIsa<I32LoadOrStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadNotStore(size_t offset, size_t rt) {
    addIsa<I32LoadNotStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadXorStore(size_t offset, size_t rt) {
    addIsa<I32LoadXorStoreInstr>({offset&0xffff, rt&0x3});
  }
  void i32LoadXnorStore(size_t offset, size_t rt) {
    addIsa<I32LoadXnorStoreInstr>({offset&0xffff, rt&0x3});
  }
  void sAddi(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SAddiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sAddiu(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SAddiuInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSubi(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSubiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sAndi(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SAndiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sOri(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SOriInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sXori(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SXoriInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSgti(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSgtiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSlti(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSltiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSeqi(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSeqiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSgei(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSgeiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSlei(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSleiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSrai(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSraiInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSrli(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSrliInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sSlli(size_t rs, size_t imm, size_t rt) {
    updateRfSel(rs, rt, -1);
    addIsa<SSlliInstr>({rs&0xf, imm&0xffff, rt&0xf});
  }
  void sLui(size_t imm, size_t rt) {
    updateRfSel(-1, rt, -1);
    addIsa<SLuiInstr>({imm&0xffff, rt&0xf});
  }
  void sAdd(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SAddInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sAddu(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SAdduInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSub(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSubInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sMul(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SMulInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sMulh(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SMulhInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sMulhu(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SMulhuInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sMin(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SMinInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sMax(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SMaxInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sMinu(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SMinuInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sMaxu(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SMaxuInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sAnd(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SAndInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sOr(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SOrInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sXor(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SXorInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSgt(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSgtInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSlt(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSltInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSeq(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSeqInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSge(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSgeInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSle(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSleInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sCmpsel(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SCmpselInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSra(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSraInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSrl(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSrlInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sSll(size_t rs, size_t rt, size_t rd) {
    updateRfSel(rs, rt, rd);
    addIsa<SSllInstr>({rs&0xf, rt&0xf, rd&0xf});
  }
  void sJump(size_t offset) {
    addIsa<SJumpInstr>({offset&0xffff});
  }
  void sJal(size_t offset) {
    addIsa<SJalInstr>({offset&0xffff});
  }
  void sJr(size_t rs) {
    updateRfSel(rs, -1, -1);
    addIsa<SJrInstr>({rs&0xf});
  }
  void sBne(size_t rs, size_t rt, size_t offset) {
    updateRfSel(rs, rt, -1);
    addIsa<SBneInstr>({rs&0xf, rt&0xf, offset&0xffff});
  }
  void sBeq(size_t rs, size_t rt, size_t offset) {
    updateRfSel(rs, rt, -1);
    addIsa<SBeqInstr>({rs&0xf, rt&0xf, offset&0xffff});
  }
  void sBlt(size_t rs, size_t rt, size_t offset) {
    updateRfSel(rs, rt, -1);
    addIsa<SBltInstr>({rs&0xf, rt&0xf, offset&0xffff});
  }
  void sBltu(size_t rs, size_t rt, size_t offset) {
    updateRfSel(rs, rt, -1);
    addIsa<SBltuInstr>({rs&0xf, rt&0xf, offset&0xffff});
  }
  void sBge(size_t rs, size_t rt, size_t offset) {
    updateRfSel(rs, rt, -1);
    addIsa<SBgeInstr>({rs&0xf, rt&0xf, offset&0xffff});
  }
  void sBgeu(size_t rs, size_t rt, size_t offset) {
    updateRfSel(rs, rt, -1);
    addIsa<SBgeuInstr>({rs&0xf, rt&0xf, offset&0xffff});
  }
  void sSetcfg(size_t rs, size_t rt, size_t spRd0, size_t spRd1, size_t srcNum) {
    updateRfSel(rs, rt, -1);
    addIsa<SSetcfgInstr>({rs&0xf, rt&0xf, spRd0&0x1f, spRd1&0x1f, srcNum&0x1});
  }
  void sSetrf(size_t rfSel) {
    addIsa<SSetrfInstr>({rfSel&0x7});
  }
};

template<typename T>
void VpuIsa::addIsa(const std::vector<size_t> &args) {
  auto instr = std::dynamic_pointer_cast<VpuInstr>(VpuInstr::create<T>(args));
  addIsa(instr);
}

}
#endif
