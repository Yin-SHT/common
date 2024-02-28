#ifndef __VPU_UNIT_FUNC_H
#define __VPU_UNIT_FUNC_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "dll_visibility.h"

void int_add(const uint8_t *x, const uint8_t *y, const uint8_t *sub, uint8_t *z);

void vpu_dequant_to_bf16(const uint8_t *x, const uint8_t *f, const uint8_t *is_unsigned, uint8_t *z);
void vpu_dequant_to_fp32(const uint8_t *x, const uint8_t *f, const uint8_t *is_unsigned, uint8_t *z);

void int16_dequant_to_fp32(const uint8_t *x, const uint8_t *is_unsigned, uint8_t *z);

void fp32_quant_to_int8(const uint8_t *x, const uint8_t *f, uint8_t *z);
void fp32_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z);
void fp32_quant_to_int16(const uint8_t *x, const uint8_t *f, uint8_t *z);
void fp32_store_to_bf16(const uint8_t *x, uint8_t *z);
void bf16_quant_to_int8(const uint8_t *x, const uint8_t *f, uint8_t *z);
void bf16_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z);
void bf16_quant_to_int16(const uint8_t *x, const uint8_t *f, uint8_t *z);

void fp32_mov(const uint8_t *x, uint8_t *z);
void fp32_add(const uint8_t *x, const uint8_t *y, uint8_t *z);
void fp32_sub(const uint8_t *x, const uint8_t *y, uint8_t *z);
void fp32_mul(const uint8_t *x, const uint8_t *y, uint8_t *z);
void vpu_add(const uint8_t *x, const uint8_t *y, uint8_t *z);
void vpu_mul(const uint8_t *x, const uint8_t *y, uint8_t *z);
void vpu_max(const uint8_t *x, const uint8_t *y, uint8_t *z);
void vpu_min(const uint8_t *x, const uint8_t *y, uint8_t *z);
void fp32_max(const uint8_t *x, const uint8_t *y, uint8_t *z);
void fp32_min(const uint8_t *x, const uint8_t *y, uint8_t *z);

void add_bf24_v2(const uint8_t *x, const uint8_t *y, uint8_t *z);
void bf24_to_bf16(const uint8_t *x, uint8_t *z);
void bf24_to_bf16_rnd(const uint8_t *x, const uint8_t *rnd_mux, uint8_t *z);

// uint16_t Dequantize_fp8_to_bf16(uint8_t src,int fp8_type,int fp8_em_type, bfloat16 bias); 
// uint8_t quantize_bf16_to_fp8(uint16_t src, int fp8_type, int fp8_em_type, bfloat16 bias);

void fp32_to_bf16(const uint8_t *x_pr, uint8_t *y_pr);//V2.0 
void fp32_to_fp16(const uint8_t *x_pr, uint8_t *y_pr);//V2.0 
void fp16_to_fp32(const uint8_t *x_pr, uint8_t *y_pr);//V2.0
void fp32_to_fp24(const uint8_t *x_pr, uint8_t *y_pr);//V2.0 
void fp32_to_tf32(const uint8_t *x_pr, uint8_t *y_pr);//V2.0 

void tf32_quant_to_int8(const uint8_t *x, const uint8_t *f, uint8_t *z);    //V2.0
void tf32_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z);   //V2.0

void tf32_mul(const uint8_t *x, const uint8_t *y, uint8_t *z);//V2.0
void tf32_max(const uint8_t *x, const uint8_t *y, uint8_t *z);//V2.0
void tf32_min(const uint8_t *x, const uint8_t *y, uint8_t *z);//V2.0

#ifdef __cplusplus
}
#endif
#endif
