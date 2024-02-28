#ifndef __VPU_ARITH_H
#define __VPU_ARITH_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "dll_visibility.h"

DLL_PUBLIC void vpu_add(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_sub(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_add_acc(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_sub_acc(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_add_acc_third(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_sub_acc_third(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_mul(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_max(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_min(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void vpu_mov(const uint8_t *x, uint8_t *z);
DLL_PUBLIC void vpu_cast_int8(const uint8_t *x, uint8_t *z);
DLL_PUBLIC void vpu_cast_uint8(const uint8_t *x, uint8_t *z);
DLL_PUBLIC void vpu_cast_bf16(const uint8_t *x, uint8_t *z);
DLL_PUBLIC void vpu_cast_int16(const uint8_t *x, uint8_t *z);

typedef enum {
    FP8_NV_52 = 0x00,
    FP8_NV_43 = 0x01,
    FP8_GR_52 = 0x10,
    FP8_GR_43 = 0x11,
} FP8_TYPE;

inline uint16_t fp16_subnormal(uint16_t x) {
    return (((x >> 10) & 0x1f) == 0) ?(x & 0x8000) :x;
}

DLL_PUBLIC float quark_vpu_dequant_bf16_to_fp24(uint32_t x);
DLL_PUBLIC float quark_vpu_dequant_fp16_to_fp24(uint32_t x);
DLL_PUBLIC float quark_vpu_dequant_i8_to_fp24(uint32_t x, uint32_t f);
DLL_PUBLIC float quark_vpu_dequant_u8_to_fp24(uint32_t x, uint32_t f);
DLL_PUBLIC float quark_vpu_dequant_fp8_to_fp24(uint32_t x, uint32_t f, FP8_TYPE type);

DLL_PUBLIC uint32_t quark_vpu_quant_fp24_to_bf16(float x);
DLL_PUBLIC uint32_t quark_vpu_quant_fp24_to_fp16(float x);
DLL_PUBLIC uint32_t quark_vpu_quant_fp24_to_i8(float x, uint32_t f);
DLL_PUBLIC uint32_t quark_vpu_quant_fp24_to_u8(float x, uint32_t f);
DLL_PUBLIC uint32_t quark_vpu_quant_fp24_to_fp8(float x, uint32_t f, FP8_TYPE type);

DLL_PUBLIC float quark_vpu_dequant_bf16_to_fp32(uint32_t x);
DLL_PUBLIC float quark_vpu_dequant_fp16_to_fp32(uint32_t x);
DLL_PUBLIC float quark_vpu_dequant_i16_to_fp32(uint32_t x);
DLL_PUBLIC float quark_vpu_dequant_i8_to_fp32(uint32_t x, uint32_t f);
DLL_PUBLIC float quark_vpu_dequant_u8_to_fp32(uint32_t x, uint32_t f);
DLL_PUBLIC float quark_vpu_dequant_fp8_to_fp32(uint32_t x, uint32_t f, FP8_TYPE type);

DLL_PUBLIC uint32_t quark_vpu_quant_fp32_to_bf16(float x);
DLL_PUBLIC uint32_t quark_vpu_quant_fp32_to_fp16(float x);
DLL_PUBLIC uint32_t quark_vpu_quant_fp32_to_i16(float x, uint32_t f);
DLL_PUBLIC uint32_t quark_vpu_quant_fp32_to_i8(float x, uint32_t f);
DLL_PUBLIC uint32_t quark_vpu_quant_fp32_to_u8(float x, uint32_t f);
DLL_PUBLIC uint32_t quark_vpu_quant_fp32_to_fp8(float x, uint32_t f, FP8_TYPE type);

DLL_PUBLIC float quark_vpu_fp32_to_fp24(float x);
DLL_PUBLIC float quark_vpu_fp24_to_tf32(float x);
DLL_PUBLIC float quark_vpu_add_fp32(float x, float y);
DLL_PUBLIC float quark_vpu_sub_fp32(float x, float y);
DLL_PUBLIC float quark_vpu_mul_fp32(float x, float y);
DLL_PUBLIC float quark_vpu_min_fp32(float x, float y);
DLL_PUBLIC float quark_vpu_max_fp32(float x, float y);

DLL_PUBLIC float quark_vpu_add_fp24(float x, float y);
DLL_PUBLIC float quark_vpu_sub_fp24(float x, float y);
DLL_PUBLIC float quark_vpu_mul_tf32(float x, float y);
DLL_PUBLIC float quark_vpu_min_tf32(float x, float y);
DLL_PUBLIC float quark_vpu_max_tf32(float x, float y);

#ifdef __cplusplus
}
#endif
#endif
