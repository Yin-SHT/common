#ifndef __BFLOAT24_H
#define __BFLOAT24_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
// #include "bfloat16.h"
#include "dll_visibility.h"

typedef union {
	float fp32;
	uint32_t value;
} bfloat24;

typedef union {
	float fp32;
    double fp64;
	uint64_t value;
} self_float;

typedef self_float union_float;

DLL_PUBLIC void _mul_bf16(const uint8_t* x, const uint8_t* y, uint8_t* z);
// void mul_bf16(const bfloat16 x, const bfloat16 y, bfloat24& z );

DLL_PUBLIC void _add_bf24(const uint8_t *x, const uint8_t *y, uint8_t *z);
DLL_PUBLIC void _add_bf24_to_bf16(const uint8_t *x, const uint8_t *y, uint8_t *z);
// void add_bf24(const bfloat24 x, const bfloat24 y, bfloat24& z );
// void add_bf24_to_bf16(const bfloat24 x, const bfloat24 y, bfloat16& z );

DLL_PUBLIC void _bf24_to_bf16(const uint8_t *x, uint8_t *z);
DLL_PUBLIC float _bf24_to_float(const uint8_t *x);
DLL_PUBLIC void _float_to_bf24(float x, uint8_t *z);

#ifdef __cplusplus
}
#endif

#include <math.h>
inline float subnormal(float src) {
    if(std::isnan(src)) {
		return NAN;
    } else {
        auto x = (self_float*)(&src);
        uint32_t exponent = (x->value >> 23) & 0xff;
        uint32_t mantissa = (x->value >> 0 ) & 0x7fffff;

        if(exponent == 0 && mantissa != 0) {
            x->value &= 0x80000000;
        }
    }
	return src;
}

inline float f32_to_bf24(float src) {
	self_float ret;
	ret.fp32 = src;

	if(std::isnan(src)) {
        ret.value = 0x7fc00000;
    }
	else{
		uint32_t lsb = (ret.value >> 8) & 1;
		uint32_t rounding_bias = 0x7f + lsb;
		ret.value += rounding_bias;
		ret.value &= 0xffffff00;
	}
	return ret.fp32;
}

inline bool is_small_number(float src) {
    auto x = (self_float*)(&src);
    uint32_t exponent = (x->value >> 23) & 0xff;
    uint32_t mantissa = (x->value >> 0 ) & 0x7fffff;
    if(exponent == 0 && mantissa != 0) {
        return true;
    }
    return false;
}

inline uint32_t bf24_round_to_bf16(const uint32_t input_fp24) {
    self_float ret;
    ret.value = input_fp24<<8;
    if (std::isnan(ret.fp32)) {
        return 0x7fc0;
    }
    if (std::isinf(ret.fp32)){
        return (ret.fp32>0)?0x7f80:0xff80;
    }
    uint32_t lsb = (ret.value >>16) & 1;
    uint32_t rounding_bias = 0x7fff + lsb;
    ret.value += rounding_bias;
    ret.value &= 0xffff0000;
    ret.fp32 = subnormal(ret.fp32);
    ret.value >>=16;
    return ret.value;
}

#endif

