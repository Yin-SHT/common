#include "vpu_arith.h"
#include "dtype/bfloat16.h"
#include "dtype/bfloat24.h"
#include "dtype/float8/vpu/fp8_m4_quant_m5_dequant.h"
#include "vpu_unit_func.h"
//#include "fp16.h"
#include <cfenv>
using namespace numeric;

//typedef union {
//    float fp32;
//    uint32_t value;
//} self_float;

void vpu_add(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  bfloat16 lhs, rhs, res;
  lhs.value = *(uint32_t*)x;
  rhs.value = *(uint32_t*)y;
  lhs = (bfloat16)subnormal((float)lhs);
  rhs = (bfloat16)subnormal((float)rhs);
  res = lhs + rhs;
  res = (bfloat16)subnormal((float)res);
  *(uint16_t*)z = res.value;
}

void vpu_sub(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  bfloat16 lhs, rhs, res;
  lhs.value = *(uint32_t*)x;
  rhs.value = *(uint32_t*)y;
  lhs = (bfloat16)subnormal((float)lhs);
  rhs = (bfloat16)subnormal((float)rhs);
  res = lhs - rhs;
  res = (bfloat16)subnormal((float)res);
  *(uint16_t*)z = res.value;
}

void vpu_add_acc(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  self_float lhs, rhs, res;
  lhs.value = *(uint32_t*)x << 8;
  rhs.value = *(uint32_t*)y << 8;
  lhs.fp32 = subnormal(lhs.fp32);
  rhs.fp32 = subnormal(rhs.fp32);
  res.fp32 = f32_to_bf24(lhs.fp32 + rhs.fp32);
  res.fp32 = subnormal(res.fp32);
  z[0] = 0xFF & (res.value >> 8);
  z[1] = 0xFF & (res.value >> 16);
  z[2] = 0xFF & (res.value >> 24);
}

void vpu_sub_acc(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  self_float lhs, rhs, res;
  lhs.value = *(uint32_t*)x << 8;
  rhs.value = *(uint32_t*)y << 8;
  lhs.fp32 = subnormal(lhs.fp32);
  rhs.fp32 = subnormal(rhs.fp32);
  res.fp32 = f32_to_bf24(lhs.fp32 - rhs.fp32);
  res.fp32 = subnormal(res.fp32);
  z[0] = 0xFF & (res.value >> 8);
  z[1] = 0xFF & (res.value >> 16);
  z[2] = 0xFF & (res.value >> 24);
}

void vpu_add_acc_third(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  if(((*(const uint32_t*)x)&0x7FFFFF) == 0 && ((*(const uint32_t*)y)&0x7FFFFF) == 0)
    *(uint32_t*)z = (*(const uint32_t*)x) & (*(const uint32_t*)y) & 0x800000;
  else
    _add_bf24(x, y, z);
}

void vpu_sub_acc_third(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  uint32_t tmp = *(uint32_t*)y;
  tmp = (tmp&0x800000) ?(tmp&0x7fffff) :(tmp|0x800000);
  vpu_add_acc_third(x, (uint8_t*)(&tmp), z);
}

void vpu_mul(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  bfloat16 lhs, rhs, res;
  lhs.value = *(uint16_t*)x;
  rhs.value = *(uint16_t*)y;
  lhs = (bfloat16)subnormal((float)lhs);
  rhs = (bfloat16)subnormal((float)rhs);
  res = lhs * rhs;
  res = (bfloat16)subnormal((float)res);
  *(uint16_t*)z = res.value;
}

void vpu_max(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  bfloat16 lhs, rhs, res;
  lhs.value = *(uint16_t*)x;
  rhs.value = *(uint16_t*)y;
  if(std::isnan((float)lhs) || std::isnan((float)rhs))
    res = bfloat16(NAN);
  else
    res = std::max(lhs, rhs);
  if((lhs==bfloat16(0.0) && rhs==bfloat16(-0.0)) ||
      (lhs==bfloat16(-0.0) && rhs==bfloat16(0.0)))
    res = bfloat16(0.0);
  *(uint16_t*)z = res.value;
}

void vpu_min(const uint8_t *x, const uint8_t *y, uint8_t *z) {
  bfloat16 lhs, rhs, res;
  lhs.value = *(uint16_t*)x;
  rhs.value = *(uint16_t*)y;
  if(std::isnan((float)lhs) || std::isnan((float)rhs))
    res = bfloat16(NAN);
  else
    res = std::min(lhs, rhs);
  if((lhs==bfloat16(0.0) && rhs==bfloat16(-0.0)) ||
      (lhs==bfloat16(-0.0) && rhs==bfloat16(0.0)))
    res = bfloat16(0.0);
  *(uint16_t*)z = res.value;
}

void vpu_mov(const uint8_t *x, uint8_t *z) {
  *(uint16_t*)z = *(uint16_t*)x;
}

static bfloat16 round(bfloat16 src) {
  std::fesetround(FE_TONEAREST);
  return bfloat16(std::nearbyint((float)src));
}

void vpu_cast_int8(const uint8_t *x, uint8_t *z) {
  bfloat16 lhs;
  lhs.value = *(uint16_t*)x;
  lhs = round(lhs);
  *z = (int8_t)std::max(std::min((float)lhs,(float)127), (float)-128);
}

void vpu_cast_uint8(const uint8_t *x, uint8_t *z) {
  bfloat16 lhs;
  lhs.value = *(uint16_t*)x;
  lhs = round(lhs);
  *z = (uint8_t)std::max(std::min((float)lhs,(float)255), (float)0);
}

void vpu_cast_bf16(const uint8_t *x, uint8_t *z) {
  *(uint16_t*)z = bfloat16((int8_t)(*x)).value;
}

void vpu_cast_int16(const uint8_t *x, uint8_t *z) {
  bfloat16 lhs;
  lhs.value = *(uint16_t*)x;
  lhs = round(lhs);
  *(int16_t*)z = (int16_t)std::max(std::min((float)lhs,(float)32767), (float)-32768);
}

float quark_vpu_dequant_bf16_to_fp24(uint32_t x) {
  bfloat16 t;
  t.value = x;
  return (float)t;
}
float quark_vpu_dequant_fp16_to_fp24(uint32_t x) {
  float ret = quark_vpu_dequant_fp16_to_fp32(x);
  return quark_vpu_fp32_to_fp24(ret);
}
float quark_vpu_dequant_i8_to_fp24(uint32_t x, uint32_t f) {
  float ret = quark_vpu_dequant_i8_to_fp32(x, f << 8);
  return quark_vpu_fp32_to_fp24(ret);
}
float quark_vpu_dequant_u8_to_fp24(uint32_t x, uint32_t f) {
  float ret = quark_vpu_dequant_u8_to_fp32(x, f << 8);
  return quark_vpu_fp32_to_fp24(ret);
}
float quark_vpu_dequant_fp8_to_fp24(uint32_t x, uint32_t f, FP8_TYPE type) {
  //uint8_t fp8_type = (type & 0x2) ?1 :0;
  //uint8_t fp8_em_ty = (type & 0x1) ?1 :0;
  //bfloat16 t;
  //dequantize_fp8_to_bf16((uint8_t*)(&x), &fp8_type, &fp8_em_ty, (uint16_t*)(&f), &t.value);
  float ret = quark_vpu_dequant_fp8_to_fp32(x, f<<8, type);
  return quark_vpu_fp32_to_fp24(ret);
}

uint32_t quark_vpu_quant_fp24_to_bf16(float x) {
  if(std::isnan(x))
    x = NAN;
  return bfloat16(x).value;
}
uint32_t quark_vpu_quant_fp24_to_fp16(float x) {
  return quark_vpu_quant_fp32_to_fp16(x);
}
uint32_t quark_vpu_quant_fp24_to_i8(float x, uint32_t f) {
  uint32_t ret = 0;
  f <<= 16;
  x = quark_vpu_fp24_to_tf32(x);
  tf32_quant_to_int8((uint8_t*)(&x), (uint8_t*)(&f), (uint8_t*)(&ret));
  return ret;
}
uint32_t quark_vpu_quant_fp24_to_u8(float x, uint32_t f) {
  uint32_t ret = 0;
  f <<= 16;
  x = quark_vpu_fp24_to_tf32(x);
  tf32_quant_to_uint8((uint8_t*)(&x), (uint8_t*)(&f), (uint8_t*)(&ret));
  return ret;
}
uint32_t quark_vpu_quant_fp24_to_fp8(float x, uint32_t f, FP8_TYPE type) {
  uint32_t ret = 0;
  uint8_t fp8_type = (type & 0x2) ?1 :0;
  uint8_t fp8_em_ty = (type & 0x1) ?1 :0;
  quantize_tf32_to_fp8((uint32_t*)(&x), &fp8_type, &fp8_em_ty, (uint16_t*)(&f), (uint8_t*)(&ret));
  return ret;
}

float quark_vpu_dequant_bf16_to_fp32(uint32_t x) {
  bfloat16 t;
  t.value = x;
  return (float)t;
}
float quark_vpu_dequant_fp16_to_fp32(uint32_t x) {
  float ret = 0;
  x <<= 16;
  fp16_to_fp32((uint8_t*)(&x), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_dequant_i16_to_fp32(uint32_t x) {
  return (int16_t)(uint16_t)x;
}
float quark_vpu_dequant_i8_to_fp32(uint32_t x, uint32_t f) {
  float ret = 0;
  uint8_t int_type = 0;
  f <<= 8;
  vpu_dequant_to_fp32((uint8_t*)(&x), (uint8_t*)(&f), (uint8_t*)(&int_type), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_dequant_u8_to_fp32(uint32_t x, uint32_t f) {
  float ret = 0;
  uint8_t int_type = 1;
  f <<= 8;
  vpu_dequant_to_fp32((uint8_t*)(&x), (uint8_t*)(&f), (uint8_t*)(&int_type), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_dequant_fp8_to_fp32(uint32_t x, uint32_t f, FP8_TYPE type) {
  float ret = 0;
  uint8_t fp8_type = (type & 0x2) ?1 :0;
  uint8_t fp8_em_ty = (type & 0x1) ?1 :0;
  dequantize_fp8_to_fp32((uint8_t*)(&x), &fp8_type, &fp8_em_ty, &f, (uint32_t*)(&ret));
  return ret;
}

float quark_vpu_dequant_i4_to_fp32(uint32_t x, uint32_t f) {
  // TODO: To be implemented
  (void)f;
  return int8_t((x&0x8) ?(0xf0|x) :x);
}

float quark_vpu_dequant_fp4_to_fp32(uint32_t x, uint32_t f) {
  // TODO: To be implemented
  (void)f;
  return int8_t(x);
}

uint32_t quark_vpu_quant_fp32_to_bf16(float x) {
  uint32_t ret = 0;
  fp32_store_to_bf16((uint8_t*)(&x),  (uint8_t*)(&ret));
  return ret;
}
uint32_t quark_vpu_quant_fp32_to_fp16(float x) {
  uint32_t ret = 0;
  fp32_to_fp16((uint8_t*)(&x), (uint8_t*)(&ret));
  return ret >> 16;
}
uint32_t quark_vpu_quant_fp32_to_i16(float x, uint32_t f) {
  uint32_t ret = 0;
  f <<= 16;
  fp32_quant_to_int16((uint8_t*)(&x), (uint8_t*)(&f), (uint8_t*)(&ret));
  return ret;
}
uint32_t quark_vpu_quant_fp32_to_i8(float x, uint32_t f) {
  uint32_t ret = 0;
  f <<= 16;
  fp32_quant_to_int8((uint8_t*)(&x), (uint8_t*)(&f), (uint8_t*)(&ret));
  return ret;
}
uint32_t quark_vpu_quant_fp32_to_u8(float x, uint32_t f) {
  uint32_t ret = 0;
  f <<= 16;
  fp32_quant_to_uint8((uint8_t*)(&x), (uint8_t*)(&f), (uint8_t*)(&ret));
  return ret;
}
uint32_t quark_vpu_quant_fp32_to_fp8(float x, uint32_t f, FP8_TYPE type) {
  uint32_t ret = 0;
  uint8_t fp8_type = (type & 0x2) ?1 :0;
  uint8_t fp8_em_ty = (type & 0x1) ?1 :0;
  quantize_fp32_to_fp8((uint32_t*)(&x), &fp8_type, &fp8_em_ty, (uint16_t*)(&f), (uint8_t*)(&ret));
  return ret;
}

uint32_t quark_vpu_quant_fp32_to_i4(float x, uint32_t f) {
  // TODO: To be implemented
  (void)f;
  return 0xf & int8_t(x);
}

uint32_t quark_vpu_quant_fp32_to_fp4(float x, uint32_t f) {
  // TODO: To be implemented
  (void)f;
  return 0xf & int8_t(x);
}

float quark_vpu_fp32_to_fp24(float x) {
  float ret = 0;
  fp32_to_fp24((uint8_t*)(&x), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_fp24_to_tf32(float x) {
  float ret = 0;
  fp32_to_tf32((uint8_t*)(&x), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_add_fp32(float x, float y) {
  float ret = 0;
  fp32_add((uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_sub_fp32(float x, float y) {
  float ret = 0;
  fp32_sub((uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_mul_fp32(float x, float y) {
  float ret = 0;
  fp32_mul((uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_min_fp32(float x, float y) {
  float ret = 0;
  fp32_min((uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_max_fp32(float x, float y) {
  float ret = 0;
  fp32_max((uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&ret));
  return ret;
}

float quark_vpu_add_fp24(float x, float y) {
  union_float _x, _y, _z;
  _x.fp32 = x;
  _y.fp32 = y;
  _x.value >>= 8;
  _y.value >>= 8;
  add_bf24_v2((uint8_t*)(&_x), (uint8_t*)(&_y), (uint8_t*)(&_z));
  _z.value <<= 8;
  return _z.fp32;
}
float quark_vpu_sub_fp24(float x, float y) {
  return quark_vpu_add_fp24(x, -y);
}
float quark_vpu_mul_tf32(float x, float y) {
  float ret = 0;
  tf32_mul((uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_min_tf32(float x, float y) {
  float ret = 0;
  tf32_min((uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&ret));
  return ret;
}
float quark_vpu_max_tf32(float x, float y) {
  float ret = 0;
  tf32_max((uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&ret));
  return ret;
}
