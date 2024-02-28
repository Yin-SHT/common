
#include <cassert>
#include "nfloat18.h"
#include <stdio.h>
#include <math.h>
#include "defs.h"
#include "dtype/bfloat24.h"

// Cast nf18 to fp32
void _nf18_to_float(/*uint32_t*/const uint8_t *x, uint8_t *z) {
    uint8_t     k = ((*(uint32_t *)x)>>11)& 0x7f;
    if ((k & 0x40) == 0x40) k=k| 0x80;
    int    f = (*(uint32_t *)x) & 0x7ff;

    //printf("nfloat18.cpp[%d]input nf18 = %0x\n",__LINE__, *(uint32_t *)x);
    //printf("nfloat18.cpp[%d]input k = %0x(Hex) %d(Dec), f = %0x(Hex) %d(Dec)\n",__LINE__, k,int8_t(k),f,f);

    self_float z_f;
    z_f.fp32 = f*pow(2,(int8_t(k)-11));

    *((uint32_t *)(z))=z_f.value;

    // //printf("k=%x, (int(k)-11) = %d, f=%x, 2^(int(k)-11)=%f, z_f=%f\n", k, (int(k)-11), f, pow(2,(int(k)-11)), z_f.fp32);
    // //printf("z_f.value= %0x\n",z_f.value);
}

// void _nf18_to_float(const uint8_t *x, uint8_t *z) {
//     uint8_t sign = 0x80 & x[0];
//     uint8_t exp = (0x7F & x[0]) >> (7 - __NFLOAT18_EXP__);
//     uint32_t val = 0x00000000;
//     uint8_t *val_8 = (uint8_t *)(&val);

//     if (1 + __NFLOAT18_EXP__ + __NFLOAT18_MAN__ <= 8) {
//         val_8[0] = z[0] & ((0x01 << (7 - __NFLOAT18_EXP__)) - 1);
//     } else if (1 + __NFLOAT18_EXP__ + __NFLOAT18_MAN__ <= 16) {
//         val_8[0] = z[0] & ((0x01 << (7 - __NFLOAT18_EXP__)) - 1);
//         val_8[1] = z[1];
//     } else if (1 + __NFLOAT18_EXP__ + __NFLOAT18_MAN__ <= 24) {
//         val_8[0] = z[0] & ((0x01 << (7 - __NFLOAT18_EXP__)) - 1);
//         val_8[1] = z[1];
//         val_8[2] = z[2];
//     } else {
//         // Error.
//     }
//     val >>= 32 - 1 - __NFLOAT18_EXP__ - __NFLOAT18_MAN__;
//     val <<= 23 - __NFLOAT18_MAN__;

//     exp += 0x7F - ((0x01 << (__NFLOAT18_EXP__ - 1)) - 1);

//     *((uint32_t *)(&z)) = ((0x000000FF & sign) << 24) | ((0x000000FF & exp) << 23) | val;
// }

// cast fp32 to nf18
void _float_to_nf18(const float x, uint8_t *z) {
    self_float inp;
    inp.fp32 = x;
    uint32_t S = (inp.value >> 31) & 0x01; // 1 bit
    uint32_t E = (inp.value >> 23) & 0xFF; // 8 bit
    uint32_t M = (inp.value) & 0x7FFFFF; // 23 bit
    CHECK(S == 0);
    CHECK(E <= 189);
    CHECK(E >= 52);
    uint32_t y = fmax(62-(int)E, 0); // 0~10

    // printf("E = %d\n",E);
    
    uint32_t K = (uint32_t)(E - 137 + 11 + y) & 0x7F; // 7 bit
    uint32_t F = (uint32_t)(pow(2, 10-y) + floor((float)M/(pow(2, 23-10+y)))) & 0x7FF; // 11 bit

    uint32_t res = ((K << 11) | F) & 0x3FFFF; // 18 bit
    *((uint32_t *)(z)) = res;
}