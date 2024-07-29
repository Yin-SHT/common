#include <cassert>
#include "bfloat16.h"
#include "bfloat24.h"
#include "defs.h"

using namespace numeric;
using namespace std;

#define RTL_BF16_ADD

void _mul_bf16(const uint8_t* x, const uint8_t* y, uint8_t* z){
	
	self_float lhs, rhs, res;
	lhs.value = *(uint32_t*)x << 16;
	rhs.value = *(uint32_t*)y << 16;
	res.fp32 = subnormal(subnormal(lhs.fp32) * subnormal(rhs.fp32));
	
	z[0] = 0xFF & (res.value >> 8);
	z[1] = 0xFF & (res.value >> 16);
	z[2] = 0xFF & (res.value >> 24);
}

void mul_bf16(const bfloat16 x, const bfloat16 y, bfloat24& z ){
	z.value = 0;
	_mul_bf16( (uint8_t*)(&x.value), (uint8_t*)(&y.value), (uint8_t*)(&z.value) );
    assert(z.value>>24 ==0);
}

static unsigned int fadd (unsigned int a, unsigned int b) {
    
    //Taking care of zero cases
    //v1: +0 + -0 = -0
    //v1_5: +0 + -0 = +0
    if (a==0){
        if(getHwVersion() == HW_VER1) return b;
        if(getHwVersion() == HW_VER1_5 || getHwVersion() == HW_VER2_0){
            if((b & 0xffffff) == 0x800000) return 0;
            else return b;
        }
    }
    if (b==0){
        if(getHwVersion() == HW_VER1) return a;
        if(getHwVersion() == HW_VER1_5 || getHwVersion() == HW_VER2_0){
            if((a & 0xffffff) == 0x800000) return 0;
            else return a;
        }
    }


    //Extracting information from a and b
    __uint64_t a_sign = (a & 0x800000)>>23;
    __uint64_t a_enc_exp = (a & 0x7f8000)>>15;
    __uint64_t a_mantissa = (a & 0x7fff);


    __uint64_t b_sign = (b & 0x800000)>>23;
    __uint64_t b_enc_exp = (b & 0x7f8000)>>15;
    __uint64_t b_mantissa = (b & 0x7fff);


    __uint64_t a_significand = (a_enc_exp >= 1) ? (a_mantissa | (1<<15)) : a_mantissa;
    __uint64_t b_significand = (b_enc_exp >= 1) ? (b_mantissa | (1<<15)) : b_mantissa;


    //Initially shifting a and b 7 bits left to increase precison for rounding
    __uint64_t a_shift_significand = (a_significand << 16);
    __uint64_t b_shift_significand = (b_significand << 16);

    //Taking care of denormal numbers
    __uint64_t a_exp = ((a_enc_exp == 0) ? 1 : a_enc_exp);
    __uint64_t b_exp = ((b_enc_exp == 0) ? 1 : b_enc_exp);
    __uint64_t ans_exp;
    __uint64_t ans_significand;
    __uint64_t ans_sign;
    //bool ans_denormal = false;

    /* Special Cases */

    //Case when a is NaN
    if (a_exp == 255 && a_mantissa != 0){
        return 0x7fffff;
    }

    //Case when b is NaN
    if (b_exp == 255 && b_mantissa != 0){
        return 0x7fffff;
    }

    //Case when Infinity - Infinity
    if (a_exp == 255 && a_mantissa == 0 && b_exp == 255 && b_mantissa == 0 && a_sign != b_sign){
        return 0x7fffff;
    }

    //Case when a is Infinity
    if (a_exp == 255 && a_mantissa == 0){
        return a;
    }

    //Case when b is Infinty
    if (b_exp == 255 && b_mantissa == 0){
        return b;
    }

    /* Making Exponent Same */


    if (a_exp >= b_exp){
        __uint64_t shift = a_exp-b_exp;

        b_shift_significand = (b_shift_significand >> ((shift>32) ? 32 : shift) );
        ans_exp = a_exp;



    }
    else {
        __uint64_t shift = b_exp-a_exp;
        a_shift_significand = (a_shift_significand >> ((shift>32) ? 32 : shift) );

        ans_exp = b_exp;
    }

    /* Adding Significands */
    if (a_sign == b_sign){
        ans_significand = a_shift_significand + b_shift_significand;
        ans_sign = a_sign;

    }
    else {
        if (a_shift_significand > b_shift_significand){
            ans_sign = a_sign;
            ans_significand = a_shift_significand - b_shift_significand;
        }
        else if (a_shift_significand < b_shift_significand){
            ans_sign = b_sign;
            ans_significand = b_shift_significand - a_shift_significand;
        }
        else {
            ans_sign = 0;
            ans_significand = a_shift_significand - b_shift_significand;

        }
    }

    /* Normalization */
    int i;
    for (i=32; i>0 && ((ans_significand>>i) == 0); i-- ){;}
    
    if (i>15){

        //Rounding
        __uint64_t residual = ((ans_significand&(1<<(i-15-1)))>>(i-15-1));

        __uint64_t sticky = 0;
        for(int j=0;j<i-15-1;j++){
            sticky = sticky | ((ans_significand & (1<<j))>>j);
        }

        if ((int(ans_exp) + (i-15) - 16) > 0 && (int(ans_exp) + (i-15) - 16) < 255){

            ans_significand = (ans_significand>>(i-15));

            ans_exp = ans_exp + (i-15) - 16;

            if (residual==1 && sticky == 1){
                ans_significand += 1;

            }
            else if ((ans_significand&1)==1 && residual ==1 && sticky == 0){
                ans_significand += 1;

            }

            if ((ans_significand>>16)==1){
                ans_significand = (ans_significand>>1);
                ans_exp += 1;

            }
        }

        //Denormal number
        else if (int(ans_exp) + (i-15) - 16 <= 0) {
            //ans_denormal = true;
            ans_significand = ans_significand>>16;
            ans_significand = ans_significand<<(ans_exp-1);
            ans_exp = 0;
        }

        //Overflow
        else if (int(ans_exp) + (i-15) - 16 >= 255){
            ans_significand = (1<<15);
            ans_exp = 255;
        }

    }
    else if (i<=15 && i!=0){
        if ((int(ans_exp) - (15-i) - 16) > 0 && (int(ans_exp) - (15-i) - 16) < 255){
            ans_significand = (ans_significand<<(15-i));
            ans_exp = ans_exp - (15-i) - 16;
        }

        //Denormal Number
        else if (int(ans_exp) - (15-i) - 16 <= 0) {
            //ans_denormal = true;
            ans_significand = ans_significand>>16;
            ans_significand = ans_significand<<(ans_exp-1);
            ans_exp = 0;
        }

        //Overflow
        else if ((int(ans_exp) - (15-i) - 16) >= 255){
            ans_significand = (1<<15);
            ans_exp = 255;
        }


    }

    //When answer is zero
    else if (i==0 && ans_exp < 255){
        ans_exp = 0;
    }

    /* Constructing floating point number from sign, exponent and significand */

    unsigned int ans = (ans_sign<<23) | (ans_exp<<15) | (ans_significand& (0x7FFF));
    return ans;
}

static unsigned int fadd_to_bf16 (unsigned int a, unsigned int b) {
    
    //Taking care of zero cases
    //if (a==0){
    //    return b;
    //}
    //if (b==0){
    //    return a;
    //}


    //Extracting information from a and b
    __uint64_t a_sign = (a & 0x800000)>>23;
    __uint64_t a_enc_exp = (a & 0x7f8000)>>15;
    __uint64_t a_mantissa = (a & 0x7fff);


    __uint64_t b_sign = (b & 0x800000)>>23;
    __uint64_t b_enc_exp = (b & 0x7f8000)>>15;
    __uint64_t b_mantissa = (b & 0x7fff);


    __uint64_t a_significand = (a_enc_exp >= 1) ? (a_mantissa | (1<<15)) : a_mantissa;
    __uint64_t b_significand = (b_enc_exp >= 1) ? (b_mantissa | (1<<15)) : b_mantissa;


    //Initially shifting a and b 7 bits left to increase precison for rounding
    __uint64_t a_shift_significand = (a_significand << 16);
    __uint64_t b_shift_significand = (b_significand << 16);

    //Taking care of denormal numbers
    __uint64_t a_exp = ((a_enc_exp == 0) ? 1 : a_enc_exp);
    __uint64_t b_exp = ((b_enc_exp == 0) ? 1 : b_enc_exp);
    __uint64_t ans_exp;
    __uint64_t ans_significand;
    __uint64_t ans_sign;
    //bool ans_denormal = false;

    /* Special Cases */

    //Case when a is NaN
    //
    
    if (a == 0x800000 && b == 0) {
     return 0x8000;}
    if (a == 0 && b == 0x800000) {
     return 0x8000;}
    //
    //
    if (a_exp == 255 && a_mantissa != 0){
        return 0x7fc0;
    }

    //Case when b is NaN
    if (b_exp == 255 && b_mantissa != 0){
        return 0x7fc0;
    }

    //Case when Infinity - Infinity
    if (a_exp == 255 && a_mantissa == 0 && b_exp == 255 && b_mantissa == 0 && a_sign != b_sign){
        return 0x7fc0;
    }

    //Case when a is Infinity
    if (a_exp == 255 && a_mantissa == 0){
        return a>>8;
    }

    //Case when b is Infinty
    if (b_exp == 255 && b_mantissa == 0){
        return b>>8;
    }

    /* Making Exponent Same */


    if (a_exp >= b_exp){
        __uint64_t shift = a_exp-b_exp;

        b_shift_significand = (b_shift_significand >> ((shift>32) ? 32 : shift) );
        ans_exp = a_exp;



    }
    else {
        __uint64_t shift = b_exp-a_exp;
        a_shift_significand = (a_shift_significand >> ((shift>32) ? 32 : shift) );

        ans_exp = b_exp;
    }

    /* Adding Significands */
    if (a_sign == b_sign){
        ans_significand = a_shift_significand + b_shift_significand;
        ans_sign = a_sign;

    }
    else {
        if (a_shift_significand > b_shift_significand){
            ans_sign = a_sign;
            ans_significand = a_shift_significand - b_shift_significand;
        }
        else if (a_shift_significand < b_shift_significand){
            ans_sign = b_sign;
            ans_significand = b_shift_significand - a_shift_significand;
        }
        else {
            ans_sign = 0;
            ans_significand = a_shift_significand - b_shift_significand;

        }
    }

    /* Normalization */
    int i;
    for (i=32; i>0 && ((ans_significand>>i) == 0); i-- ){;}
    
    if (i>7){

        //Rounding
        __uint64_t residual = ((ans_significand&(1<<(i-7-1)))>>(i-7-1));

        __uint64_t sticky = 0;
        for(int j=0;j<i-7-1;j++){
            sticky = sticky | ((ans_significand & (1<<j))>>j);
        }

        if ((int(ans_exp) + (i-7) - 24) > 0 && (int(ans_exp) + (i-7) - 24) < 255){

            ans_significand = (ans_significand>>(i-7));

            ans_exp = ans_exp + (i-7) - 24;

            if (residual==1 && sticky == 1){
                ans_significand += 1;

            }
            else if ((ans_significand&1)==1 && residual ==1 && sticky == 0){
                ans_significand += 1;

            }

            if ((ans_significand>>8)==1){
                ans_significand = (ans_significand>>1);
                ans_exp += 1;

            }
        }

        //Denormal number
        else if (int(ans_exp) + (i-7) - 24 <= 0) {
            //ans_denormal = true;
            ans_significand = ans_significand>>24;
            ans_significand = ans_significand<<(ans_exp-1);
            ans_exp = 0;
        }

        //Overflow
        else if (int(ans_exp) + (i-7) - 24 >= 255){
            ans_significand = (1<<7);
            ans_exp = 255;
        }

    }
    else if (i<=7 && i!=0){
        if ((int(ans_exp) - (7-i) - 24) > 0 && (int(ans_exp) - (7-i) - 24) < 255){
            ans_significand = (ans_significand<<(7-i));
            ans_exp = ans_exp - (7-i) - 24;
        }

        //Denormal Number
        else if (int(ans_exp) - (7-i) - 24 <= 0) {
            //ans_denormal = true;
            ans_significand = ans_significand>>24;
            ans_significand = ans_significand<<(ans_exp-1);
            ans_exp = 0;
        }

        //Overflow
        else if ((int(ans_exp) - (7-i) - 24) >= 255){
            ans_significand = (1<<7);
            ans_exp = 255;
        }


    }

    //When answer is zero
    else if (i==0 && ans_exp < 255){
        ans_exp = 0;
    }

    /* Constructing floating point number from sign, exponent and significand */

    unsigned int temp_ans = (ans_sign<<15) | (ans_exp<<7) | (ans_significand& (0x7F));
    unsigned int ans = (ans_exp == 0) ? (ans_sign == 0) ? 0 :32768  : temp_ans;
  
    return ans;
}

void _add_bf24(const uint8_t *x, const uint8_t *y, uint8_t *z) {
	self_float lhs, rhs, res;
	lhs.value = *(uint32_t*)x << 8;
	rhs.value = *(uint32_t*)y << 8;
	//res.fp32 = f32_to_bf24(subnormal(subnormal(lhs.fp32) + subnormal(rhs.fp32)));
	
    if(is_small_number(lhs.fp32) && is_small_number(rhs.fp32) && 
        ((0x80000000 & lhs.value) ^ (0x80000000 & rhs.value))) {
    }
    else {
        lhs.fp32 = subnormal(lhs.fp32);
	    rhs.fp32 = subnormal(rhs.fp32);
    }
	
	lhs.value >>= 8;
	rhs.value >>= 8;
	res.value = fadd(lhs.value, rhs.value);
	res.value <<= 8;
	
	res.fp32 = f32_to_bf24(subnormal(res.fp32));
	
	z[0] = 0xFF & (res.value >> 8);
	z[1] = 0xFF & (res.value >> 16);
	z[2] = 0xFF & (res.value >> 24);
}

void _add_bf24_to_bf16(const uint8_t *x, const uint8_t *y, uint8_t *z) {
	self_float lhs, rhs, res;
	lhs.value = *(uint32_t*)x << 8;
	rhs.value = *(uint32_t*)y << 8;
	//res.fp32 = f32_to_bf24(subnormal(subnormal(lhs.fp32) + subnormal(rhs.fp32)));
	
    //if(is_small_number(lhs.fp32) && is_small_number(rhs.fp32) && 
    //    ((0x80000000 & lhs.value) ^ (0x80000000 & rhs.value))) {
    //}
    //else {
        lhs.fp32 = subnormal(lhs.fp32);
	    rhs.fp32 = subnormal(rhs.fp32);
    //}
	
	lhs.value >>= 8;
	rhs.value >>= 8;
	res.value = fadd_to_bf16(lhs.value, rhs.value);
	res.value <<= 8;
	
	//res.fp32 = f32_to_bf24(subnormal(res.fp32));
	
	z[0] = 0xFF & (res.value >> 8);
	z[1] = 0xFF & (res.value >> 16);
}

void _add_bf24_old(const uint8_t *x, const uint8_t *y, uint8_t *z) {
	self_float lhs, rhs, res;
	lhs.value = *(uint32_t*)x << 8;
	rhs.value = *(uint32_t*)y << 8;
	res.fp32 = f32_to_bf24(subnormal(subnormal(lhs.fp32) + subnormal(rhs.fp32)));
	z[0] = 0xFF & (res.value >> 8);
	z[1] = 0xFF & (res.value >> 16);
	z[2] = 0xFF & (res.value >> 24);
}

void add_bf24(const bfloat24 x, const bfloat24 y, bfloat24& z ){
	z.value = 0;
#ifdef	RTL_BF16_ADD
	_add_bf24( (uint8_t*)(&x.value), (uint8_t*)(&y.value), (uint8_t*)(&z.value) );
#else
    _add_bf24_old( (uint8_t*)(&x.value), (uint8_t*)(&y.value), (uint8_t*)(&z.value) );
#endif
    assert((z.value)>>24 ==0);
}

void add_bf24(const uint32_t x, const uint32_t y, uint32_t& z ){
	
    z = 0;
#ifdef	RTL_BF16_ADD
	_add_bf24( (uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&z) );
#else
    _add_bf24_old( (uint8_t*)(&x.value), (uint8_t*)(&y.value), (uint8_t*)(&z.value) );
#endif
    assert(z>>24 ==0);
}

void add_bf24_to_bf16(const bfloat24 x, const bfloat24 y, bfloat16& z ){
	_add_bf24_to_bf16( (uint8_t*)(&x.value), (uint8_t*)(&y.value), (uint8_t*)(&z.value) );
}

void add_bf24_to_bf16(const uint32_t x, const uint32_t y, uint16_t& z ){
	_add_bf24_to_bf16( (uint8_t*)(&x), (uint8_t*)(&y), (uint8_t*)(&z) );
}

void _bf24_to_bf16(const uint8_t *x, uint8_t *z) {
	self_float lhs;
	lhs.value = *(uint32_t*)x << 8;
	bfloat16 res(lhs.fp32);
	*(uint16_t*)z = res.value;
}

void bf24_to_bf16(const bfloat24 x, bfloat16& z) {

    _bf24_to_bf16((uint8_t*)(&x.value), (uint8_t*)(&z.value));
}

void bf24_to_bf16(const uint32_t x, int16_t& z) {

    _bf24_to_bf16((uint8_t*)(&x), (uint8_t*)(&z));
}

// void bf24_to_bf16(const uint32_t x, uint32_t& z) {

//     _bf24_to_bf16((uint8_t*)(&x), (uint8_t*)(&z));
// }

float _bf24_to_float(const uint8_t *x) {
	self_float lhs;
	lhs.value = *(uint32_t*)x << 8;
	return lhs.fp32;
}

float bf24_to_float(const bfloat24 x) {

	return _bf24_to_float((uint8_t*)(&x.value));
}

float bf24_to_float(const uint32_t x) {

	return _bf24_to_float((uint8_t*)(&x));
}

void _float_to_bf24(float x, uint8_t *z) {
	self_float lhs;
	lhs.fp32 = f32_to_bf24(x);
	z[0] = 0xFF & (lhs.value >> 8);
	z[1] = 0xFF & (lhs.value >> 16);
	z[2] = 0xFF & (lhs.value >> 24);
}