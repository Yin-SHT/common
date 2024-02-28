#ifndef __MUL_FP8_H
#define __MUL_FP8_H

#include <assert.h>
#include "stdio.h"
#include "float8.h"
#include "dll_visibility.h"
#include <iostream>
#include "dtype/bfloat24.h"

DLL_PUBLIC void mul_fp8_143_fp8_143_to_fp24(const uint8_t *x, const uint8_t *y, float& z, fp8_format nv_or_gr);
DLL_PUBLIC void mul_fp8_152_fp8_152_to_fp24(const uint8_t *x, const uint8_t *y, float& z, fp8_format nv_or_gr);
DLL_PUBLIC void mul_fp8_152_fp8_143_to_fp24(const uint8_t *x, const uint8_t *y, float& z, fp8_format nv_or_gr);//152*143

#endif