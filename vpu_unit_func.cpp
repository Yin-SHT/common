#include "dtype/bfloat16.h"
// #include "bfloat24.cpp"
#include "dtype/bfloat24.h"
#include "fp16.h"
#include "vpu_unit_func.h"
#include <cfenv>
#include<stdio.h>
#include<math.h>
using namespace numeric;

static bfloat16 round_to_bf16(float v) {
    uint32_t input;
    self_float f;
    f.fp32 = v;
    input = f.value;
    bfloat16 output;
    uint32_t  sign_bit;

    sign_bit = input & 0x80000000;

    if (std::isnan(v) || std::isinf(v)) {
        if(std::isnan(v)){
            output.value = 0x7fc0;
        }else{
            output.value = sign_bit ? 0xff80 : 0x7f80; // v1.6
        }
    } else {
        uint32_t lsb = (input >> 16) & 1;
        uint32_t rounding_bias = 0x7fff + lsb;
        input += rounding_bias;
        output.value = static_cast<uint16_t>(input >> 16);
        if(std::isinf(float(output))){
            output.value = sign_bit ? 0xff80 : 0x7f80; // v1.6
        }
    }
    return output;
}

// v1.6
// static float subnormal(float src) {
// 	if(std::isnan(src))
// 		return NAN;
// 	if(std::isinf(src))
// 		return src;
// 	if(is_small_number(src) && src < 0)
// 		return -0.0;
// 	if(is_small_number(src) && src > 0)
// 		return 0.0;
// 	if(src > std::pow(2,128))
// 		return std::pow(2,128); ///
// 	if(src < -std::pow(2,128))
// 		return -std::pow(2,128); ///
// 	return src;
// }

static float f32_to_fp24(float src) {
    self_float ret;
    ret.fp32 = src;
    uint32_t sign_bit;

    sign_bit = ret.value & 0x80000000; // v1.6

    if(std::isnan(src)) {
        ret.value = 0x7fc00000;
    }
    else if(std::isinf(src)){
        ret.value = sign_bit ? 0xff800000 : 0x7f800000; // v1.6
    }
    else{
        uint32_t lsb = (ret.value >> 8) & 1;
        uint32_t rounding_bias = 0x7f + lsb;
        ret.value += rounding_bias;
        ret.value &= 0xffffff00;
        if(std::isinf(ret.fp32)){
            // ret.value = sign_bit ? 0xff7fff00 : 0x7f7fff00; // v1.6, result is fp24 saturation
            ret.value = sign_bit ? 0xff800000 : 0x7f800000; // v1.6, result is fp24 saturation
        }
    }
    return ret.fp32;
}

///use in VPU, MOV instruction, r3(fp24) --> r0/r1/r2(bf16)
void bf24_to_bf16(const uint8_t *x, uint8_t *z) {
	self_float lhs;
    bfloat16   res;
	lhs.value = *(uint32_t*)x << 8;
    res = round_to_bf16(lhs.fp32);                   ////first  rounding  ANTMP-610
    res = round_to_bf16(subnormal((float)res));      ////second subnormal
	*(uint16_t*)z = res.value;
}

void bf24_to_bf16_rnd(const uint8_t *x, const uint8_t *rnd_mux, uint8_t *z) { // rnd_mux, 1: round to even, 0: truncate
	self_float lhs;
    bfloat16   res;
    uint8_t    rnd_mode;
    rnd_mode = *(uint8_t*)rnd_mux;
	lhs.value = *(uint32_t*)x << 8;
    if(rnd_mode==1){
        res = round_to_bf16(lhs.fp32);                   ////first  rounding  ANTMP-610
    }
    else{
        res.value = static_cast<uint16_t>(lhs.value >> 16); 
    }
    res = round_to_bf16(subnormal((float)res));      ////second subnormal
	*(uint16_t*)z = res.value;
}



//////int8 --> uint8
static uint8_t int8_cast_uint8(const uint8_t *x) {
    int value;
    uint8_t y;
    value = *(int8_t*)x;
    y = (uint8_t)std::min(std::max((int8_t)value, (int8_t)0), (int8_t)127);
    return y;
}

////scalar int add
void int_add(const uint8_t *x, const uint8_t *y, const uint8_t *sub, uint8_t *z) {
    int a,b,c;
    a = *(int*)x;
    b = *(int*)y;
    b = *sub ? -b : b;
    c = a + b;
    //printf ("parameter a=%d, b=%d, c=%d.\n", a,b,c);
    *(uint32_t*)z = *(uint32_t*)(&c);
}


//////Dequantize   int8/unit8 Dequantize to bf16
void vpu_dequant_to_bf16(const uint8_t *x, const uint8_t *f, const uint8_t *is_unsigned, uint8_t *z) {
    self_float lhs, in_float, factor;
    bfloat16 res;
    uint8_t  input;
    uint8_t  sign_bit;

    factor.value = *(uint32_t*)f;           ////input bf16, low 16bit = 0x0000
    factor.value &= 0x7fffffff;             ////ignore sign bit
    factor.fp32 = subnormal(factor.fp32);   ////////fixed by ASIC DV, align to RTL

    if(*(uint8_t*)is_unsigned) {
        input = *x;
        sign_bit = 0;
    }
    else {
        sign_bit = *(uint8_t*)x & 0x80;
        input    = (uint8_t)(abs(*(int8_t*)x));
    }

    in_float.fp32  = (float)input;
    in_float.value = sign_bit ? (in_float.value|0x80000000) : (in_float.value|0x00000000);     ///sign bit back

    if(std::isnan(factor.fp32)){
        lhs.value = 0x7fc00000; // v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(factor.fp32) && (in_float.fp32==0) ){
        lhs.value = 0x7fc00000; // v1.6 round up, inf * zero = nan
    }
    else if( std::isinf(factor.fp32) && (in_float.fp32!=0) ){
        lhs.value = sign_bit ? 0xff800000 : 0x7f800000; // v1.6, inf*not zero = inf
    }
    else{
        lhs.fp32 = factor.fp32*in_float.fp32;
        if( std::isinf(lhs.fp32) ){
            lhs.value = sign_bit ? 0xff800000 : 0x7f800000;
        }
    }

    res = round_to_bf16(lhs.fp32);                   ////first  rounding  ANTMP-610
    res = round_to_bf16(subnormal((float)res));      ////second subnormal
    *(uint16_t*)z = res.value;

}


//////Dequantize   int8/unit8 Dequantize to fp32
void vpu_dequant_to_fp32(const uint8_t *x, const uint8_t *f, const uint8_t *is_unsigned, uint8_t *z) {
    self_float lhs, in_float, factor;
    bfloat16 res;
    uint8_t  input;
    uint8_t  sign_bit;

    factor.value = *(uint32_t*)f;    ////input fp24, low 8bit = 0x00
    factor.value &= 0x7fffffff;      ////ignore sign bit
    factor.fp32 = subnormal(factor.fp32);

    if(*(uint8_t*)is_unsigned) {
        input = *x;
        sign_bit = 0;
    }
    else {
        sign_bit = *(uint8_t*)x & 0x80;
        input    = (uint8_t)(abs(*(int8_t*)x));
        //printf ("0000, input sign: %d.\n", sign_bit);
        //printf ("0000, input abs(x): %d.\n", input);
    }

    in_float.fp32  = (float)input;
    //printf ("1111, input float: %f.\n", in_float.fp32);
    in_float.value = sign_bit ? (in_float.value|0x80000000) : (in_float.value|0x00000000);     ///sign bit back
    //printf ("1111, input float add sign: %f.\n", in_float.fp32);

    if(std::isnan(factor.fp32)){
        lhs.value = 0x7fc00000; // v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(factor.fp32) && (in_float.fp32==0) ){
        lhs.value = 0x7fc00000; // v1.6 round up, inf * zero = nan
    }
    else if( std::isinf(factor.fp32) && (in_float.fp32!=0) ){
        lhs.value = sign_bit ? 0xff800000 : 0x7f800000; // v1.6, inf*not zero = inf
    }
    else{
        lhs.fp32 = factor.fp32*in_float.fp32;
        if( std::isinf(lhs.fp32) ){
            // lhs.value = sign_bit ? 0xff7fffff : 0x7f7fffff;
            lhs.value = sign_bit ? 0xff800000 : 0x7f800000;
        }
    }

    lhs.fp32 = subnormal(lhs.fp32);

    *(uint32_t*)z = lhs.value;
}

//----------------int16 Dequantize to fp32--V2.0 added by ASIC DV--------------------- 
void int16_dequant_to_fp32(const uint8_t *x, const uint8_t *is_unsigned, uint8_t *z) {
    self_float lhs, in_float;
    bfloat16 res;
    uint8_t  input;
    uint8_t  sign_bit;

    if(*(uint8_t*)is_unsigned) {
        input = *x;
        sign_bit = 0;
    }
    else {
        sign_bit = *(uint8_t*)x & 0x80;
        input    = (uint8_t)(abs(*(int16_t*)x));//abs value of int16
    }

    in_float.fp32  = (float)input;
    in_float.value = sign_bit ? (in_float.value|0x80000000) : (in_float.value|0x00000000);     ///sign bit back

    lhs.fp32 = subnormal(in_float.fp32);
    *(uint32_t*)z = lhs.value;
}
//------------------------------------------------------------------------------------ 


static float round_to_int(float src) {
    std::fesetround(FE_TONEAREST);
    return float(std::nearbyint(src));
}


//////vpu store quantize   fp32 quantize to int8/unit8
void fp32_quant_to_int8(const uint8_t *x, const uint8_t *f, uint8_t *z) {
    self_float in_x, x_fp24, factor, quant_out;
    uint32_t sign_bit;

    /////1. fp32 round to fp24
    in_x.value  = *(uint32_t*)x;
    //in_x.fp32   = subnormal(in_x.fp32);
    //x_fp24.fp32 = f32_to_fp24(in_x.fp32);    ///rounding to fp24

    x_fp24.fp32 = f32_to_fp24(in_x.fp32);    ////first rounding ANTMP-610
    x_fp24.fp32 = subnormal(x_fp24.fp32);    ////second subnormal


    /////2. gen quant result
    sign_bit = x_fp24.value & 0x80000000; // v1.6, get x sign bit
    x_fp24.value &= 0x7fffffff;// v1.6, ignore sign bit
    factor.value = *(uint32_t*)f;      ///quant is bf16, low 16bit is 0
    factor.value &= 0x7fffffff;        ////ignore sign bit
    factor.fp32   = subnormal(factor.fp32);
    // v1.6
    if( std::isnan(factor.fp32) || std::isnan(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(factor.fp32) && (x_fp24.value==0x00000000) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, inf * zero = nan
    }
    else if( (factor.value==0x00000000) && std::isinf(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(factor.fp32) || std::isinf(x_fp24.fp32) ){
        quant_out.value = sign_bit ? 0xff800000 : 0x7f800000;// v1.6, inf*not zero = inf
    }
    else{
        quant_out.fp32 = f32_to_fp24(factor.fp32*x_fp24.fp32);
        fp32_to_tf32((uint8_t*)(&quant_out.fp32), (uint8_t*)(&quant_out.fp32)); //temp
        if( std::isinf(quant_out.fp32) ){
            // quant_out.value = 0x7f7fff00;
            quant_out.value = 0x7f800000;
        }
        quant_out.value = sign_bit ? (quant_out.value | 0x80000000) : quant_out.value;
    }



    //printf ("fp32_quant_to_fp24, input float: %f.\n", x_fp24.fp32);
    //printf ("fp32_quant_to_fp24, input factor: %f.\n", factor.fp32);

    /////3. quant result cast to int8
    //quant_out.fp32 = round_to_int(fp32_quant_to_fp24(x, f));
    quant_out.fp32 = round_to_int(quant_out.fp32);
    //if(std::isnan(in_x.fp32)) {
    //    *z = (int8_t)127;              /////input NaN,   will output max int
    //}
    //else {
        *z = (int8_t)std::max(std::min((float)quant_out.fp32, (float)127), (float)-128);
    //}
}

void fp32_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z) {
    self_float in_x, x_fp24, factor, quant_out;
    uint32_t sign_bit;

    /////1. fp32 round to fp24
    in_x.value  = *(uint32_t*)x;
    //in_x.fp32   = subnormal(in_x.fp32);
    //x_fp24.fp32 = f32_to_fp24(in_x.fp32);    ///rounding to fp24

    x_fp24.fp32 = f32_to_fp24(in_x.fp32);    ////first rounding ANTMP-610
    x_fp24.fp32 = subnormal(x_fp24.fp32);    ////second subnormal


    /////2. gen quant result
    sign_bit = x_fp24.value & 0x80000000; // v1.6, get x sign bit
    x_fp24.value &= 0x7fffffff;// v1.6, ignore sign bit
    factor.value = *(uint32_t*)f;      ///quant is bf16, low 16bit is 0
    factor.value &= 0x7fffffff;        ////ignore sign bit
    factor.fp32   = subnormal(factor.fp32);
    // v1.6
    if( std::isnan(factor.fp32) || std::isnan(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(factor.fp32) && (x_fp24.value==0x00000000) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, inf * zero = nan
    }
    else if( (factor.value==0x00000000) && std::isinf(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(factor.fp32) || std::isinf(x_fp24.fp32) ){
        quant_out.value = sign_bit ? 0xff800000 : 0x7f800000;// v1.6, inf*not zero = inf
    }
    else{
        quant_out.fp32 = f32_to_fp24(factor.fp32*x_fp24.fp32);
        if( std::isinf(quant_out.fp32) ){
            quant_out.value = 0x7f800000;
        }
        quant_out.value = sign_bit ? (quant_out.value | 0x80000000) : quant_out.value;
    }
    // quant_out.fp32 = f32_to_fp24(factor.fp32*x_fp24.fp32);

    /////3. quant result cast to uint8
    //quant_out.fp32 = round_to_int(fp32_quant_to_fp24(x, f));
    quant_out.fp32 = round_to_int(quant_out.fp32);
    //if(std::isnan(in_x.fp32)) {
    //    *z = (int8_t)255;              /////input NaN,   will output max int
    //}
    //else {
        *z = (uint8_t)std::max(std::min((float)quant_out.fp32, (float)255), (float)0);
    //}
}

void fp32_quant_to_int16(const uint8_t *x, const uint8_t *f, uint8_t *z) {
    self_float in_x, x_fp24, factor, quant_out;
    uint32_t sign_bit;

    /////1. fp32 round to fp24
    in_x.value  = *(uint32_t*)x;
    //in_x.fp32   = subnormal(in_x.fp32);
    //x_fp24.fp32 = f32_to_fp24(in_x.fp32);    ///rounding to fp24

    x_fp24.fp32 = f32_to_fp24(in_x.fp32);    ////first rounding ANTMP-610
    x_fp24.fp32 = subnormal(x_fp24.fp32);    ////second subnormal

    /////2. gen quant result
    sign_bit = x_fp24.value & 0x80000000; // v1.6, get x sign bit
    x_fp24.value &= 0x7fffffff;// v1.6, ignore sign bit
    factor.value = *(uint32_t*)f;      ///quant is bf16, low 16bit is 0
    factor.value &= 0x7fffffff;        ////ignore sign bit
    factor.fp32   = subnormal(factor.fp32);
    // v1.6
    if( std::isnan(factor.fp32) || std::isnan(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(factor.fp32) && (x_fp24.value==0x00000000) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, inf * zero = nan
    }
    else if( (factor.value==0x00000000) && std::isinf(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(factor.fp32) || std::isinf(x_fp24.fp32) ){
        quant_out.value = sign_bit ? 0xff800000 : 0x7f800000;// v1.6, inf*not zero = inf
    }
    else{
        quant_out.fp32 = f32_to_fp24(factor.fp32*x_fp24.fp32);
        if( std::isinf(quant_out.fp32) ){
            // quant_out.value = 0x7f7fff00;
            quant_out.value = 0x7f800000;
        }
        quant_out.value = sign_bit ? (quant_out.value | 0x80000000) : quant_out.value;
    }
    // quant_out.fp32 = f32_to_fp24(factor.fp32*x_fp24.fp32);

    /////3. quant result cast to int16
    //quant_out.fp32 = round_to_int(fp32_quant_to_fp24(x, f));
    quant_out.fp32 = round_to_int(quant_out.fp32);
    //if(std::isnan(in_x.fp32)) {
    //    *(int16_t*)z = (int16_t)32767;              /////input NaN,   will output max int
    //}
    //else {
        *(int16_t*)z = (int16_t)std::max(std::min((float)quant_out.fp32, (float)32767), (float)-32768);
    //}
}

void fp32_store_to_bf16(const uint8_t *x, uint8_t *z) {
    /////as rtl, two step rounding
    self_float in_x/*, x_fp24*/;
    bfloat16 x_bf16;

    in_x.value  = *(uint32_t*)x;
    x_bf16 = round_to_bf16(in_x.fp32);                   ////first  rounding  ANTMP-610
    x_bf16 = round_to_bf16(subnormal((float)x_bf16));    ////second subnormal

    *(uint16_t*)z = x_bf16.value;
}


void bf16_quant_to_int8(const uint8_t *x, const uint8_t *f, uint8_t *z) {
    self_float quant_out, y_float;
    bfloat16 x_in;
    bfloat16 factor;
    bfloat16 y_out;
    uint16_t sign_bit;

    /////1. quant with factor
    x_in.value = *(uint16_t*)x;
    sign_bit = x_in.value & 0x8000;  // v1.6, get x sign bit
    x_in.value &= 0x7fff;                 ////ignore sign bit
    factor.value = *(uint16_t*)f;           
    factor.value &= 0x7fff;                 ////ignore sign bit
    x_in = round_to_bf16(subnormal((float)x_in));     // v1.6, input x subnormal
    factor = round_to_bf16(subnormal((float)factor)); // v1.6, input f subnormal
    if( std::isnan(float(factor)) || std::isnan(float(x_in)) ){
        y_out.value = 0x7fc0; // v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(float(factor)) && (x_in.value==0x0000) ){
        y_out.value = 0x7fc0; // v1.6 round up, inf * zero = nan
    }
    else if( (factor.value==0x0000) && std::isinf(float(x_in)) ){
        y_out.value = 0x7fc0; // v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(float(factor)) || std::isinf(float(x_in)) ){
        y_out.value = sign_bit ? 0xff80 : 0x7f80;// v1.6, inf*not zero = inf
    }
    else{
        // vpu_mul(x, (uint8_t*)(&factor.value), (uint8_t*)(&y_out.value));
        y_out = x_in * factor;
        y_out.value = sign_bit ? (y_out.value | 0x8000) : y_out.value;
        if( std::isinf(float(y_out)) ){
            // y_out.value = sign_bit ? 0xff7f : 0x7f7f;
            y_out.value = sign_bit ? 0xff80 : 0x7f80;
        }
    }
    //printf ("parameter y_out=%f.\n", (float)y_out);
    y_float.fp32 =  (float)y_out;

    /////2. quant result bf16 cast to int
    quant_out.fp32 = round_to_int(y_float.fp32);
    //quant_out.fp32 = round_to_int(bf16_quant_to_fp24(x, f));

    /////3. quant int cast to int8
    *z = (int8_t)std::max(std::min((float)quant_out.fp32, (float)127), (float)-128);

}

// void bf16_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z) {
//     self_float quant_out, y_float;
//     bfloat16 factor;
//     bfloat16 y_out;

//     /////1. quant with factor
//     factor.value = *(uint16_t*)f;           
//     factor.value &= 0x7fff;                 ////ignore sign bit
//     vpu_mul(x, (uint8_t*)(&factor.value), (uint8_t*)(&y_out.value));
//     y_float.fp32 =  (float)y_out;

//     /////2. quant result bf16 cast to int
//     quant_out.fp32 = round_to_int(y_float.fp32);
//     //quant_out.fp32 = round_to_int(bf16_quant_to_fp24(x, f));

//     /////3. quant int cast to uint8
//     *z = (int8_t)std::max(std::min((float)quant_out.fp32, (float)255), (float)0);
// }

// v1.6
void bf16_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z) {
    self_float quant_out, y_float;
    bfloat16 x_in;
    bfloat16 factor;
    bfloat16 y_out;
    uint16_t sign_bit;

    /////1. quant with factor
    x_in.value = *(uint16_t*)x;
    sign_bit = x_in.value & 0x8000;  // v1.6, get x sign bit
    x_in.value &= 0x7fff;                 ////ignore sign bit
    factor.value = *(uint16_t*)f;           
    factor.value &= 0x7fff;                 ////ignore sign bit
    x_in = round_to_bf16(subnormal((float)x_in));     // v1.6, input x subnormal
    factor = round_to_bf16(subnormal((float)factor)); // v1.6, input f subnormal
    // printf("sign_bit is %x", sign_bit);
    if( std::isnan(float(factor)) || std::isnan(float(x_in)) ){
        y_out.value = 0x7fc0; // v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(float(factor)) && (x_in.value==0x0000) ){
        y_out.value = 0x7fc0; // v1.6 round up, inf * zero = nan
    }
    else if( (factor.value==0x0000) && std::isinf(float(x_in)) ){
        y_out.value = 0x7fc0; // v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(float(factor)) || std::isinf(float(x_in)) ){
        y_out.value = sign_bit ? 0xff80 : 0x7f80;// v1.6, inf*not zero = inf
    }
    else{
        // vpu_mul(x, (uint8_t*)(&factor.value), (uint8_t*)(&y_out.value));
        y_out = x_in * factor;
        y_out.value = sign_bit ? (y_out.value | 0x8000) : y_out.value;
        if( std::isinf(float(y_out)) ){
            // y_out.value = sign_bit ? 0xff7f : 0x7f7f;
            y_out.value = sign_bit ? 0xff80 : 0x7f80;
        }
    }
    //printf ("parameter y_out=%f.\n", (float)y_out);
    y_float.fp32 =  (float)y_out;

    /////2. quant result bf16 cast to int
    quant_out.fp32 = round_to_int(y_float.fp32);
    //quant_out.fp32 = round_to_int(bf16_quant_to_fp24(x, f));

    /////3. quant int cast to uint8
    *z = (uint8_t)std::max(std::min((float)quant_out.fp32, (float)255), (float)0);
}

void bf16_quant_to_int16(const uint8_t *x, const uint8_t *f, uint8_t *z) {
    self_float quant_out, y_float;
    bfloat16 x_in;
    bfloat16 factor;
    bfloat16 y_out;
    uint16_t sign_bit;

    /////1. quant with factor
    x_in.value = *(uint16_t*)x;
    sign_bit = x_in.value & 0x8000;  // v1.6, get x sign bit
    x_in.value &= 0x7fff;                 ////ignore sign bit
    factor.value = *(uint16_t*)f;           
    factor.value &= 0x7fff;                 ////ignore sign bit
    x_in = round_to_bf16(subnormal((float)x_in));     // v1.6, input x subnormal
    factor = round_to_bf16(subnormal((float)factor)); // v1.6, input f subnormal
    // vpu_mul(x, (uint8_t*)(&factor.value), (uint8_t*)(&y_out.value));
    // v1.6
    if( std::isnan(float(factor)) || std::isnan(float(x_in)) ){
        y_out.value = 0x7fc0; // v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(float(factor)) && (x_in.value==0x0000) ){
        y_out.value = 0x7fc0; // v1.6 round up, inf * zero = nan
    }
    else if( (factor.value==0x0000) && std::isinf(float(x_in)) ){
        y_out.value = 0x7fc0; // v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(float(factor)) || std::isinf(float(x_in)) ){
        y_out.value = sign_bit ? 0xff80 : 0x7f80;// v1.6, inf*not zero = inf
    }
    else{
        // vpu_mul(x, (uint8_t*)(&factor.value), (uint8_t*)(&y_out.value));
        y_out = x_in * factor;
        y_out.value = sign_bit ? (y_out.value | 0x8000) : y_out.value;
        if( std::isinf(float(y_out)) ){
            // y_out.value = sign_bit ? 0xff7f : 0x7f7f;
            y_out.value = sign_bit ? 0xff80 : 0x7f80;
        }
    }
    y_float.fp32 =  (float)y_out;

    /////2. quant result bf16 cast to int
    quant_out.fp32 = round_to_int(y_float.fp32);
    //quant_out.fp32 = round_to_int(bf16_quant_to_fp24(x, f));
    
    /////3. quant int cast to int16
    *(int16_t*)z = (int16_t)std::max(std::min((float)quant_out.fp32, (float)32767), (float)-32768);
}


static unsigned int fadd (unsigned int a, unsigned int b) {
    
    //Taking care of zero cases
    if (a==0){
        return b;
    }
    if (b==0){
        return a;
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
    __uint64_t ans_significand = 0;
    __uint64_t ans_sign = 0;
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
        else if (a_shift_significand == b_shift_significand) {
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

#if 0
inline static float f32_to_bf24(float src) {
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
#endif


///////inherit  v1.0 
///////use in VPU ADD/SUB instruction, input type is BF24
void add_bf24_v2(const uint8_t *x, const uint8_t *y, uint8_t *z) {
	self_float lhs, rhs, res;
	lhs.value = *(uint32_t*)x << 8;
	rhs.value = *(uint32_t*)y << 8;
    
    lhs.fp32 = subnormal(lhs.fp32);
 	  rhs.fp32 = subnormal(rhs.fp32);

    if(lhs.fp32 == 0.0 && rhs.fp32 == 0.0 ) {                             ////maybe -0.0 or 0.0
        if((0x80000000 & lhs.value) && (0x80000000 & rhs.value)) {        ////-0.0 + -0.0
            res.fp32 = -0.0;
        }
        else {
            res.fp32 = 0.0;
        }
    }
    else {
	    lhs.value >>= 8;
	    rhs.value >>= 8;
	    res.value = fadd(lhs.value, rhs.value);
	    res.value <<= 8;

        //res.fp32 = lhs.fp32 + rhs.fp32;  
		    //uint32_t lsb = (res.value >> 8) & 1;
	      //uint32_t rounding_bias = 0x7f + lsb;
		    //res.value += rounding_bias;
		    //res.value &= 0xffffff00;

      res.fp32 = subnormal(res.fp32);////first rounding ANTMP-610; second subnormal
    }
	z[0] = 0xFF & (res.value >> 8);
	z[1] = 0xFF & (res.value >> 16);
	z[2] = 0xFF & (res.value >> 24);
}


#if 0
///////inherit  v1.0 vpu_add
///////use for vpu reduce sum
void vpu_add(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    bfloat16 lhs, rhs, res;
    lhs.value = *(uint16_t*)x;
    rhs.value = *(uint16_t*)y;
    lhs = round_to_bf16(subnormal((float)lhs));
    rhs = round_to_bf16(subnormal((float)rhs));
    res = lhs + rhs;
    res = round_to_bf16(subnormal((float)res));
    *(uint16_t*)z = res.value;
}

///////inherit  v1.0 vpu_mul
// void vpu_mul(const uint8_t *x, const uint8_t *y, uint8_t *z) {
//     bfloat16 lhs, rhs, res;
//     lhs.value = *(uint16_t*)x;
//     rhs.value = *(uint16_t*)y;
//     lhs = (bfloat16)subnormal((float)lhs);
//     rhs = (bfloat16)subnormal((float)rhs);
//     res = lhs * rhs;
//     res = (bfloat16)subnormal((float)res);
//     *(uint16_t*)z = res.value;
// }

// v1.6
void vpu_mul(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    bfloat16 lhs, rhs, res;
    uint16_t sign_bit_l, sign_bit_r, sign_bit;
    lhs.value = *(uint16_t*)x;
    rhs.value = *(uint16_t*)y;
    sign_bit_l = lhs.value & 0x8000; // v1.6, get sign bit
    sign_bit_r = rhs.value & 0x8000; // v1.6, get sign bit
    sign_bit = sign_bit_l ^ sign_bit_r;
    lhs.value &= 0x7fff; // ignore sign bit
    rhs.value &= 0x7fff; // ignore sign bit
    lhs = round_to_bf16(subnormal((float)lhs));
    rhs = round_to_bf16(subnormal((float)rhs));
    if( std::isnan(float(lhs)) || std::isnan(float(rhs)) ){
        res.value = 0x7fc0; // v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(float(lhs)) && (rhs.value==0x0000) ){
        res.value = 0x7fc0; // v1.6 round up, inf * zero = nan
    }
    else if( (lhs.value==0x0000) && std::isinf(float(rhs)) ){
        res.value = 0x7fc0; // v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(float(lhs)) || std::isinf(float(rhs)) ){
        res.value = sign_bit ? 0xff80 : 0x7f80;// v1.6, inf*not zero = inf
    }
    else{
        res = lhs * rhs;
        if( std::isinf(float(res)) ){
            // res.value = 0x7f7f;
            res.value = 0x7f80;
        }
        res.value = sign_bit ? (res.value | 0x8000) : res.value;
    }
    res = round_to_bf16(subnormal((float)res));
    *(uint16_t*)z = res.value;
}
///////inherit  v1.0 vpu_max
void vpu_max(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    bfloat16 lhs, rhs, res;
    lhs.value = *(uint16_t*)x;
    rhs.value = *(uint16_t*)y;

    lhs = round_to_bf16(subnormal((float)lhs));       ////v1.5, add subnormal
    rhs = round_to_bf16(subnormal((float)rhs));       ////v1.5, add subnormal
    if(std::isnan((float)lhs) || std::isnan((float)rhs)) {
        res = round_to_bf16(NAN);
    }
    else {
        res = std::max(lhs, rhs);     ////v1.0
    }
    if((lhs==round_to_bf16(0.0) && rhs==round_to_bf16(-0.0)) ||
        lhs==round_to_bf16(-0.0) && rhs==round_to_bf16(0.0)) {
        res = round_to_bf16(0.0);
    }
    *(uint16_t*)z = res.value;
}

///////inherit  v1.0 vpu_min
void vpu_min(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    bfloat16 lhs, rhs, res;
    lhs.value = *(uint16_t*)x;
    rhs.value = *(uint16_t*)y;

    lhs = round_to_bf16(subnormal((float)lhs));       ////v1.5, add subnormal
    rhs = round_to_bf16(subnormal((float)rhs));       ////v1.5, add subnormal
    if(std::isnan((float)lhs) || std::isnan((float)rhs)) {
        res = round_to_bf16(NAN);
    }
    else {
        res = std::min(lhs, rhs);
    }
    if((lhs==round_to_bf16(0.0) && rhs==round_to_bf16(-0.0)) ||
        lhs==round_to_bf16(-0.0) && rhs==round_to_bf16(0.0)) {
        res = round_to_bf16(0.0);
    }
    *(uint16_t*)z = res.value;
}
#endif

void fp32_mov(const uint8_t *x, uint8_t *z) {
    *(uint32_t*)z = *(uint32_t*)x;
}

///////fp32_mul, new add in v1.5
void fp32_mul(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    self_float x_in, y_in, z_out;
    uint32_t sign_bit_l, sign_bit_r, sign_bit;

    x_in.value  = *(uint32_t*)x;
    y_in.value  = *(uint32_t*)y;

    sign_bit_l = x_in.value & 0x80000000; // v1.6, get sign bit
    sign_bit_r = y_in.value & 0x80000000; // v1.6, get sign bit
    sign_bit = sign_bit_l ^ sign_bit_r;
    x_in.value &= 0x7fffffff; // ignore sign bit
    y_in.value &= 0x7fffffff; // ignore sign bit

    x_in.fp32 = subnormal(x_in.fp32);
    y_in.fp32 = subnormal(y_in.fp32);
    // z_out.fp32 = x_in.fp32*y_in.fp32;
    if( std::isnan(x_in.fp32) || std::isnan(y_in.fp32) ){
        z_out.value = 0x7fc00000; // v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(x_in.fp32) && (y_in.value==0x00000000) ){
        z_out.value = 0x7fc00000; // v1.6 round up, inf * zero = nan
    }
    else if( (x_in.value==0x00000000) && std::isinf(y_in.fp32) ){
        z_out.value = 0x7fc00000; // v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(x_in.fp32) || std::isinf(y_in.fp32) ){
        z_out.value = sign_bit ? 0xff800000 : 0x7f800000;// v1.6, inf*not zero = inf
    }
    else{
        z_out.fp32 = x_in.fp32 * y_in.fp32;
        if( std::isinf(z_out.fp32) ){
            // z_out.value = 0x7f7f;
            z_out.value = 0x7f800000;
        }
        z_out.value = sign_bit ? (z_out.value | 0x80000000) : z_out.value;
    }
    z_out.fp32 = subnormal(z_out.fp32);
    *(uint32_t*)z = z_out.value;
}


///////fp32_add, new add in v1.5
void fp32_add(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    self_float x_in, y_in, z_out;

    x_in.value  = *(uint32_t*)x;
    y_in.value  = *(uint32_t*)y;

    x_in.fp32 = subnormal(x_in.fp32);
    y_in.fp32 = subnormal(y_in.fp32);
    z_out.fp32 = x_in.fp32 + y_in.fp32;
    z_out.fp32 = subnormal(z_out.fp32);

    *(uint32_t*)z = z_out.value;
}

void fp32_sub(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    self_float x_in, y_in, z_out;

    x_in.value  = *(uint32_t*)x;
    y_in.value  = *(uint32_t*)y;

    x_in.fp32 = subnormal(x_in.fp32);
    y_in.fp32 = subnormal(y_in.fp32);
    z_out.fp32 = x_in.fp32 - y_in.fp32;
    z_out.fp32 = subnormal(z_out.fp32);

    *(uint32_t*)z = z_out.value;
}

///////fp32_max, new add in v1.5
void fp32_max(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    self_float x_in, y_in, z_out;

    x_in.value  = *(uint32_t*)x;
    y_in.value  = *(uint32_t*)y;
    x_in.fp32 = subnormal(x_in.fp32);
    y_in.fp32 = subnormal(y_in.fp32);

    if(std::isnan(x_in.fp32) || std::isnan(y_in.fp32)) {
        z_out.fp32 = NAN;
    }
    else if((x_in.fp32==0.0 && y_in.fp32==-0.0) || (x_in.fp32==-0.0 && y_in.fp32==0.0)) {
        z_out.fp32 =0.0;
    }
    else {
        z_out.fp32 = std::max(x_in.fp32, y_in.fp32);
    }

    *(uint32_t*)z = z_out.value;
}

///////fp32_min, new add in v1.5
void fp32_min(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    self_float x_in, y_in, z_out;

    x_in.value  = *(uint32_t*)x;
    y_in.value  = *(uint32_t*)y;
    x_in.fp32 = subnormal(x_in.fp32);
    y_in.fp32 = subnormal(y_in.fp32);

    if(std::isnan(x_in.fp32) || std::isnan(y_in.fp32)) {
        z_out.fp32 = NAN;
    }
    else if((x_in.fp32==0.0 && y_in.fp32==-0.0) || (x_in.fp32==-0.0 && y_in.fp32==0.0)) {
        z_out.fp32 =0.0;
    }
    else {
        z_out.fp32 = std::min(x_in.fp32, y_in.fp32);
    }

    *(uint32_t*)z = z_out.value;
}

//===========================================================================
//V2.0 added for optimized data-flow, usewd for both vpu&act
void fp32_to_bf16(const uint8_t *x_pr, uint8_t *y_pr)   //used for quant bf16.
{
    self_float sf; 
    bfloat16 y_bf16;

    sf.value = (*(uint32_t*)x_pr);
    y_bf16 = bfloat16(sf.fp32);     //convert to bf16.
    sf.value = y_bf16.value << 16;  //padding to 32bit.
    sf.fp32 = subnormal(sf.fp32);
    *(uint32_t*)y_pr = sf.value;
};
void fp32_to_fp16(const uint8_t *x_pr, uint8_t *y_pr)   //used for quant fp16.
{
    self_float sf; 
    fp16 y_fp16;

    sf.value = (*(uint32_t*)x_pr);
    y_fp16 = fp16(sf.fp32);         //convert to fp16.
    sf.value = y_fp16.value << 16;  //padding to 32bit. 
    *(uint32_t*)y_pr = sf.value;
};
void fp16_to_fp32(const uint8_t *x_pr, uint8_t *y_pr)   //used for dequant fp16.
{
    fp16 x_fp16;
    float y;

    x_fp16.value = (*(uint32_t*)x_pr) >> 16;//traete bit [31:16] in the addr as fp16.  
    y = (float)x_fp16;
    y = subnormal(y);
    auto x = (uint32_t*)(&y);
    *(uint32_t*)y_pr = *x;
};

void fp32_to_fp24(const uint8_t *x_pr, uint8_t *y_pr)   //used for dequant int8.
{
    self_float sf; 

    sf.value = (*(uint32_t*)x_pr);
    uint32_t lsb = (sf.value >> 8) & 1;
    uint32_t rounding_bias = 0x007F + lsb;  //23-bit -> 15-bit.
    sf.value += rounding_bias;              //input in (1.8.15.8)
    sf.value &= 0xFFFFFF00;                 //truncate to 24-bit. 
    sf.fp32 = subnormal(sf.fp32);
    *(uint32_t*)y_pr = sf.value;
};

void fp32_to_tf32(const uint8_t *x_pr, uint8_t *y_pr)   //used for quant and vpu-tf32-alu.
{
    self_float sf; 

    sf.value = (*(uint32_t*)x_pr);
    uint32_t lsb = (sf.value >> 13) & 1;
    uint32_t rounding_bias = 0x0FFF + lsb;  //23-bit -> 10-bit.
    sf.value += rounding_bias;              //input in (1.8.10.13)
    sf.value &= 0xFFFFE000;                 //truncate to 19-bit. 
    sf.fp32 = subnormal(sf.fp32);
    *(uint32_t*)y_pr = sf.value;
};

//tf32 quant to int8, consider the input x as fp32(tf32 + padding zeros)
//basicly same as fp32_quant_to_int8, just rnd the quant-out to tf32, instead of fp24. 
void tf32_quant_to_int8(const uint8_t *x, const uint8_t *f, uint8_t *z) {
    self_float in_x, x_fp24, factor, quant_out;
    uint32_t sign_bit;

    /////1. fp32 round to fp24
    in_x.value  = *(uint32_t*)x;

    x_fp24.fp32 = f32_to_fp24(in_x.fp32);    ////first rounding ANTMP-610
    x_fp24.fp32 = subnormal(x_fp24.fp32);    ////second subnormal

    /////2. gen quant result
    sign_bit = x_fp24.value & 0x80000000; // v1.6, get x sign bit
    x_fp24.value &= 0x7fffffff;// v1.6, ignore sign bit
    factor.value = *(uint32_t*)f;      ///quant is bf16, low 16bit is 0
    factor.value &= 0x7fffffff;        ////ignore sign bit
    factor.fp32   = subnormal(factor.fp32);
    // v1.6
    if( std::isnan(factor.fp32) || std::isnan(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(factor.fp32) && (x_fp24.value==0x00000000) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, inf * zero = nan
    }
    else if( (factor.value==0x00000000) && std::isinf(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(factor.fp32) || std::isinf(x_fp24.fp32) ){
        quant_out.value = sign_bit ? 0xff800000 : 0x7f800000;// v1.6, inf*not zero = inf
    }
    else{
        quant_out.fp32 = factor.fp32*x_fp24.fp32;
        fp32_to_tf32((uint8_t*)(&quant_out.fp32), (uint8_t*)(&quant_out.fp32)); //directly rnd fp32 to tf32. 
        if( std::isinf(quant_out.fp32) ){
            quant_out.value = 0x7f800000;
        }
        quant_out.value = sign_bit ? (quant_out.value | 0x80000000) : quant_out.value;
    }

    /////3. quant result cast to uint8
    quant_out.fp32 = round_to_int(quant_out.fp32);
    *z = (int8_t)std::max(std::min((float)quant_out.fp32, (float)127), (float)-128);
}

//tf32 quant to uint8, consider the input x as fp32(tf32 + padding zeros)
//basicly same as fp32_quant_to_uint8, just rnd the quant-out to tf32, instead of fp24. 
void tf32_quant_to_uint8(const uint8_t *x, const uint8_t *f, uint8_t *z) {
    self_float in_x, x_fp24, factor, quant_out;
    uint32_t sign_bit;

    /////1. fp32 round to fp24
    in_x.value  = *(uint32_t*)x;

    x_fp24.fp32 = f32_to_fp24(in_x.fp32);    ////first rounding ANTMP-610
    x_fp24.fp32 = subnormal(x_fp24.fp32);    ////second subnormal

    /////2. gen quant result
    sign_bit = x_fp24.value & 0x80000000; // v1.6, get x sign bit
    x_fp24.value &= 0x7fffffff;// v1.6, ignore sign bit
    factor.value = *(uint32_t*)f;      ///quant is bf16, low 16bit is 0
    factor.value &= 0x7fffffff;        ////ignore sign bit
    factor.fp32   = subnormal(factor.fp32);
    // v1.6
    if( std::isnan(factor.fp32) || std::isnan(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(factor.fp32) && (x_fp24.value==0x00000000) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, inf * zero = nan
    }
    else if( (factor.value==0x00000000) && std::isinf(x_fp24.fp32) ){
        quant_out.value = 0x7fc00000;// v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(factor.fp32) || std::isinf(x_fp24.fp32) ){
        quant_out.value = sign_bit ? 0xff800000 : 0x7f800000;// v1.6, inf*not zero = inf
    }
    else{
        quant_out.fp32 = factor.fp32*x_fp24.fp32;
        fp32_to_tf32((uint8_t*)(&quant_out.fp32), (uint8_t*)(&quant_out.fp32)); //directly rnd fp32 to tf32. 
        if( std::isinf(quant_out.fp32) ){
            quant_out.value = 0x7f800000;
        }
        quant_out.value = sign_bit ? (quant_out.value | 0x80000000) : quant_out.value;
    }

    /////3. quant result cast to uint8
    quant_out.fp32 = round_to_int(quant_out.fp32);
    *z = (uint8_t)std::max(std::min((float)quant_out.fp32, (float)255), (float)0);
}

//add tf32 alu for add/max/min, to replace old bf16-alu. 
void tf32_mul(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    self_float x_in, y_in, z_out;
    uint32_t sign_bit_l, sign_bit_r, sign_bit;

    x_in.value  = *(uint32_t*)x;
    y_in.value  = *(uint32_t*)y;

    sign_bit_l = x_in.value & 0x80000000; // v1.6, get sign bit
    sign_bit_r = y_in.value & 0x80000000; // v1.6, get sign bit
    sign_bit = sign_bit_l ^ sign_bit_r;

    x_in.value &= 0x7fffffff; // ignore sign bit
    y_in.value &= 0x7fffffff; // ignore sign bit
    x_in.fp32 = subnormal(x_in.fp32);
    y_in.fp32 = subnormal(y_in.fp32);

    if( std::isnan(x_in.fp32) || std::isnan(y_in.fp32) ){
        z_out.value = 0x7fc00000; // v1.6 round up , nan * anything = nan
    }
    else if( std::isinf(x_in.fp32) && (y_in.value==0x00000000) ){
        z_out.value = 0x7fc00000; // v1.6 round up, inf * zero = nan
    }
    else if( (x_in.value==0x00000000) && std::isinf(y_in.fp32) ){
        z_out.value = 0x7fc00000; // v1.6 round up, zero * inf = nan
    }
    else if( std::isinf(x_in.fp32) || std::isinf(y_in.fp32) ){
        z_out.value = sign_bit ? 0xff800000 : 0x7f800000;// v1.6, inf*not zero = inf
    }
    else{
        z_out.fp32 = x_in.fp32 * y_in.fp32;
        if( std::isinf(z_out.fp32) ){
            z_out.value = 0x7f800000;
        }
        z_out.value = sign_bit ? (z_out.value | 0x80000000) : z_out.value;
    }

    uint32_t lsb = (z_out.value >> 13) & 1;
    uint32_t rounding_bias = 0x0FFF + lsb;  //23-bit -> 10-bit.
    z_out.value += rounding_bias;           //input in (1.8.10.13)
    z_out.value &= 0xFFFFE000;              //truncate to 19-bit. 

    z_out.fp32 = subnormal(z_out.fp32);
    *(uint32_t*)z = z_out.value;
}

void tf32_max(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    self_float x_in, y_in, z_out;

    x_in.value  = *(uint32_t*)x;
    y_in.value  = *(uint32_t*)y;
    x_in.fp32 = subnormal(x_in.fp32);
    y_in.fp32 = subnormal(y_in.fp32);

    if(std::isnan(x_in.fp32) || std::isnan(y_in.fp32)) {
        z_out.fp32 = NAN;
    }
    else if((x_in.fp32==0.0 && y_in.fp32==-0.0) || (x_in.fp32==-0.0 && y_in.fp32==0.0)) {
        z_out.fp32 =0.0;
    }
    else {
        z_out.fp32 = std::max(x_in.fp32, y_in.fp32);
    }

    uint32_t lsb = (z_out.value >> 13) & 1;
    uint32_t rounding_bias = 0x0FFF + lsb;  //23-bit -> 10-bit.
    z_out.value += rounding_bias;           //input in (1.8.10.13)
    z_out.value &= 0xFFFFE000;              //truncate to 19-bit. 

    *(uint32_t*)z = z_out.value;
}

void tf32_min(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    self_float x_in, y_in, z_out;

    x_in.value  = *(uint32_t*)x;
    y_in.value  = *(uint32_t*)y;
    x_in.fp32 = subnormal(x_in.fp32);
    y_in.fp32 = subnormal(y_in.fp32);

    if(std::isnan(x_in.fp32) || std::isnan(y_in.fp32)) {
        z_out.fp32 = NAN;
    }
    else if((x_in.fp32==0.0 && y_in.fp32==-0.0) || (x_in.fp32==-0.0 && y_in.fp32==0.0)) {
        z_out.fp32 =0.0;
    }
    else {
        z_out.fp32 = std::min(x_in.fp32, y_in.fp32);
    }

    uint32_t lsb = (z_out.value >> 13) & 1;
    uint32_t rounding_bias = 0x0FFF + lsb;  //23-bit -> 10-bit.
    z_out.value += rounding_bias;           //input in (1.8.10.13)
    z_out.value &= 0xFFFFE000;              //truncate to 19-bit. 

    *(uint32_t*)z = z_out.value;
}

//===========================================================================

