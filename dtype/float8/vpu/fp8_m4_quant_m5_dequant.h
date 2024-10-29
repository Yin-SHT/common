#ifndef __FP8_Q_DQ_H
#define __FP8_Q_DQ_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stdbool.h>

// typedef union {
//     float fp32;
//     uint32_t value;
// } self_float;

// void int_add(const uint8_t *x, const uint8_t *y, const uint8_t *sub, uint8_t *z);

// void vpu_dequant_to_bf16(const uint8_t *x, const uint8_t *f, const uint8_t *is_unsigned, uint8_t *z);
// void vpu_dequant_to_fp32(const uint8_t *x, const uint8_t *f, const uint8_t *is_unsigned, uint8_t *z);

// void fp32_quant_to_int8(const uint8_t *x, const uint8_t *f, uint8_t *z);
// void fp32_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z);
// void fp32_quant_to_int16(const uint8_t *x, const uint8_t *f, uint8_t *z);
// void fp32_store_to_bf16(const uint8_t *x, uint8_t *z);
// void bf16_quant_to_int8(const uint8_t *x, const uint8_t *f, uint8_t *z);
// void bf16_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z);
// void bf16_quant_to_int16(const uint8_t *x, const uint8_t *f, uint8_t *z);

// void fp32_add(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void fp32_mul(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void vpu_add(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void vpu_mul(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void vpu_max(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void vpu_min(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void fp32_max(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void fp32_min(const uint8_t *x, const uint8_t *y, uint8_t *z);

// void add_bf24(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void bf24_to_bf16(const uint8_t *x, uint8_t *z);

void dequantize_fp8_to_bf16(const uint8_t *x, const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, const uint16_t *coeff, uint16_t *z); //fp8_type 0 nv, 1 gr . fp8_em_type 0is52 1is 43
void quantize_bf16_to_fp8(const uint16_t *x , const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, const uint16_t *coeff, uint8_t *z); //fp8_type 0 nv, 1 gr. fp8_em_type 0 is 52 ,1 is 43.   43  52

void dequantize_fp8_to_fp32(const uint8_t *x, const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, /* fp24*/const uint32_t *coeff, uint32_t *z); //fp8_type 0 nv, 1 gr . fp8_em_type 0is52 1is 43
void quantize_fp32_to_fp8(const uint32_t *x, const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, /* bf16*/const uint16_t *coeff, uint8_t *z); //fp8_type 0 nv, 1 gr . fp8_em_type 0is52 1is 43
void quantize_tf32_to_fp8(const uint32_t *x , const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, const uint16_t *coeff, uint8_t *z); //fp8_type 0 nv, 1 gr. fp8_em_type 0 is 52 ,1 is 43.   43  52

#ifdef __cplusplus
}
#endif
#endif
