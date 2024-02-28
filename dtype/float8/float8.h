#ifndef __FLOAT8_H
#define __FLOAT8_H

#ifdef __cplusplus
extern "C" {
#endif

// Based on https://arxiv.org/abs/2209.05433

#include <stdint.h>
#include "nfloat18.h"

#ifndef __FP8_TYPE
#define __FP8_TYPE
typedef union {
    float fp32;
    uint8_t value;
} fp8_143;

typedef union {
    float fp32;
    uint8_t value;
} fp8_152;
#endif

enum fp8_format{nv, gr};

DLL_PUBLIC void _mul_fp8_143_float_to_float(const uint8_t *x, const float *y, float *z);
DLL_PUBLIC void _fp8_143_to_float(const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, uint32_t *z );
DLL_PUBLIC void _fp8_152_to_float(const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, uint32_t *z );
DLL_PUBLIC void _float_to_fp8_143(/*fp32 input*/const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, /*output fp8*/uint8_t *z);
DLL_PUBLIC void _float_to_fp8_152(/*fp32 input*/const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, /*output fp8*/uint8_t *z);
DLL_PUBLIC void _dequantize_fp8_m2_to_fp24(/*fp8 input*/const uint8_t *x, /*nf18 u32 input*/const uint8_t *y, 
                                /*0:152,1:143*/uint8_t fp8_dtype, /*0:nv,1:gr*/uint8_t fp8_mode, 
                                uint8_t bias, /*output fp24*/float *z);
DLL_PUBLIC void _quantize_fp24_m1_to_fp8(/*fp24 u32 input*/const uint8_t *x, /*nf18 u32 input*/const uint8_t *y, 
                            /*0:152,1:143*/uint8_t fp8_dtype, /*0:nv,1:gr*/uint8_t fp8_mode, uint8_t bias, 
                            /*output fp8*/uint8_t *z);
DLL_PUBLIC void _dequantize_fp24_m3_to_fp24(/*fp24 u32 input*/const uint8_t *x, /*nf18 u32 input*/const uint8_t *y, 
                                /*output fp24*/uint8_t *z);

#ifdef __cplusplus
}
#endif

#endif
