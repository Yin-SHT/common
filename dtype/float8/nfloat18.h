#ifndef __NFLOAT18_H
#define __NFLOAT18_H

#ifdef __cplusplus
extern "C" {
#endif

// New FLOAT 18
// S + EXP + MAN
// Similar to IEEE
// BUT!
// No NaN
// No Inf
// No subnorm
// Stored as (i.e. E=6, M=10)
// uint8_t ~[3]
// [0]      [1]      [2]
// SEEEEEEM MMMMMMMM M

// #define __NFLOAT18_EXP__ 7
// // EXP <= 7
// #define __NFLOAT18_MAN__ 11
// // MAN <= 11

#include <stdint.h>
#include "dll_visibility.h"

#ifndef __NF18_TYPE
#define __NF18_TYPE
typedef union {
    float fp32;
    uint32_t value;
} nf18;
#endif


// typedef union {
//     float fp32;
//     uint32_t value;
// } self_float;

DLL_PUBLIC void _nf18_to_float(const uint8_t *x, uint8_t *z);
DLL_PUBLIC void _float_to_nf18(const float x, uint8_t *z);

#ifdef __cplusplus
}
#endif
#endif