#include <assert.h>
#include "stdio.h"
#include "dtype/float8/float8.h"


#include "dtype/bfloat24.h"

    typedef union {
        double fp64;
        uint64_t value;
    } self_double;

// Cast fp8 to fp32
 void _fp8_143_to_float(const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, uint32_t *z ) {
    uint8_t sign = 0x1 & (x[0]>>7);
    uint8_t exp = 0x0F & (x[0] >> 3);
    uint8_t val = 0x07 & x[0];
    //printf("[%d] input para sign=%x, exp= %x, val=%x\n", __LINE__, sign, exp, val);

    if(nv_or_gr == nv) assert(bias == 7);

    if ((sign == 0x00) && (exp == 0x00) && (val == 0x00)) { //all 0
        // output 0
        z[0] = 0x00;
        return;
    }
    
    if ((sign == 0x1) && (exp == 0x00) && (val == 0x00)){
        if( nv_or_gr == gr) { //gr NAN
            z[0] = 0x7fc00000;
        } else { //nv -0
            z[0] = 0x80000000;
        }
        return;
    } 
    
    if ((nv_or_gr == nv) && (exp == 0xf) && (val == 0x7)){ //TODO double check
        z[0] = 0x7fc00000;
        return;
    } 
    
    if((exp == 0x0) && (val>0x0)) { // subnormal
        if (nv_or_gr == gr) {
            z[0] = sign<<31;//? gr subnormal?
        } else {
            float mid = (val/pow(2,3))*pow(2,1-bias);
            auto x = (uint32_t*)(&mid);
            z[0] = sign<<31 | *x ;
        }
        return;
    }
    //no special values
    exp = exp  + 127 - bias;
  //  assert ((exp>=0) && (exp<256));
    z[0] = (sign << 31) | (exp<<23) |(val<<20);
    
    // self_float output;
    // output.value = z_32;
    // printf("in function [%d] output.value = %0x, output.fp32 = %f\n", __LINE__, output.value, output.fp32);
    //output
    // *((uint32_t *)(z))= z_32;

}

 void _fp8_152_to_float(const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, uint32_t *z ) {
    uint8_t sign = 0x1 & (x[0]>>7);
    uint8_t exp  = 0x1F & (x[0] >> 2);
    uint8_t val  = 0x03 & x[0];
  //  printf("_fp8_152_to_float [%d] input para sign=%x, exp= %x, val=%x\n", __LINE__, sign, exp, val);

    if(nv_or_gr == nv) assert(bias == 15);
    

    if ((sign == 0x00) && (exp == 0x00) && (val == 0x00)) { //all 0
        // output 0
        z[0] = 0x00;
       // printf("_fp8_152_to_float[%d] return 0\n",__LINE__);
        return;
    }

    if ((sign == 0x1) && (exp == 0x00) && (val == 0x00)){
        if( nv_or_gr == gr) { //gr NAN
            z[0] = 0x7fc00000;
        } else { //nv -0
            z[0] = 0x80000000;
        }
       // printf("_fp8_152_to_float[%d] return -0\n",__LINE__);
        return;
    } 
    
    
    if ((nv_or_gr == nv) && (exp == 0x1f) && (val == 0x0)){ //nv inf
        z[0] = sign? 0xff800000:0x7F800000;
        //printf("_fp8_152_to_float[%d] return nv inf\n",__LINE__);
        return;
    }
    
    if ((nv_or_gr == nv) && (exp == 0x1f) && (val > 0x0)){ //nv NAN
        z[0] = 0x7fc00000;
       // printf("_fp8_152_to_float[%d] return nv nan\n",__LINE__);
        return;
    }
    
    if((exp == 0x0) && (val>0x0)) { // subnormal
        if (nv_or_gr == gr) {
            z[0] = sign<<31;
        } else {
            if(val == 0x01){
              z[0] =(sign << 31) | 0x37800000;
            }
            else if(val==0x02){
               z[0] = (sign<<31) | 0x38000000 ;
            }
            else if(val ==0x03){
               z[0] = (sign<<31) | 0x38400000;
            }

        }
       // printf("_fp8_152_to_float[%d] return subnormal %032x\n",__LINE__, z[0]);
        return;
    } 
    
   //no special values
        exp = exp  + 127 - bias;
      //  printf("_fp8_152_to_float[%d] exp_of_float=%x\n",__LINE__,exp);
      // assert ((exp>=0) && (exp<256));
        z[0] = (sign << 31) | (exp<<23) |(val<<21);
    

    // self_float output;
    // output.value = z_32;
    // printf("in function [%d] output.value = %0x, output.fp32 = %f\n", __LINE__, output.value, output.fp32);
    //output
   

}

//dequantize fp8 M2 to fp24
void _dequantize_fp8_m2_to_fp24(/*fp8 input*/const uint8_t *x, /*nf18 u32 input*/const uint8_t *y, 
                                    /*0:152,1:143*/uint8_t fp8_dtype, /*0:nv,1:gr*/uint8_t fp8_mode, uint8_t bias, /*output fp24*/float *z){

    float    m2_f;
    float    fp8_f;//TODO

    uint32_t    fp8_u32;
    fp8_format nv_or_gr;
    nv_or_gr=fp8_mode?gr:nv;

    if(fp8_dtype==1)//143
         _fp8_143_to_float(x, nv_or_gr, bias, &fp8_u32 );//TODO
    else //152
        _fp8_152_to_float(x, nv_or_gr, bias, &fp8_u32 );
    
    self_float _temp_sfloat;
    _temp_sfloat.value = fp8_u32;
    fp8_f=_temp_sfloat.fp32;

    _nf18_to_float(y, (uint8_t *)(&m2_f));
    
    // printf("dequantize fp8_u32 value= %x, fp32=%e\n", fp8_u32,fp8_f);
    // printf("dequantize m2_f value= %e\n", m2_f);

    float z_f=fp8_f*m2_f;
    self_float z_fp24;

    //printf("mult output fp32 value= %f\n", z_f);

    z_fp24.fp32=f32_to_bf24(z_f);
   // printf("_dequantize_fp8_m2_to_fp24[%d]final output fp24 value= %x\n", __LINE__, z_fp24.value);

    // output
    *((uint32_t *)(z))= z_fp24.value;

}

void _float_to_fp8_143(/*fp32 input*/const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, /*output fp8*/uint8_t *z){
    
    uint32_t x_u32=*(u_int32_t *)x;
    uint8_t sign = 0x80 & (x_u32>>24);
    uint8_t exp  = 0xff & (x_u32 >> 23);
    //uint32_t val = 0x7fffff & x_u32;

    self_float _temp_float;
    _temp_float.value = x_u32;

    //printf("_float_to_fp8_143:sign= %0x, exp=%0x, val=%0x.\n",sign, exp, val);

    //NAN
    if (std::isnan(_temp_float.fp32)) { //NAN
        if(nv_or_gr==gr) 
            z[0] = 0x80;    //gr NAN
        else
            z[0] = 0xfF; // nv NAN
        //printf("_float_to_fp8_143:is nan and return\n");
        return;
    }

    //INF
     if (std::isinf(_temp_float.fp32)) {
        if(nv_or_gr==gr) 
            z[0] = 0x80; // gr inf
        else
            z[0] = sign | 0x7e;    //nv inf -> max_normal
        
        //printf("_float_to_fp8_143:is inf and return\n");
        return;
     }

    // //subnormal && zero && less min normal
if ((nv_or_gr==gr) and (abs(_temp_float.fp32)<pow(2,0-bias))) {
        z[0] = 0x0; // gr zero
        //printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e, pow(2,1-bias)= %e\n",_temp_float.fp32,pow(2,1-bias));
        return;
    }

    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)<=pow(2,-10))){
        z[0] = sign | 0x00;    //nv zero
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)>pow(2,-10) && abs(_temp_float.fp32)< 3*pow(2,-10))){
        z[0] = sign | 0x01;    //nv subnormal 1.  0.5-1.5
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)>= 3*pow(2,-10) && abs(_temp_float.fp32)<= 5*pow(2,-10))){
        z[0] = sign | 0x02;    //nv subnormal 2.  1.5-2.5
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)> 5*pow(2,-10) && abs(_temp_float.fp32)< 7*pow(2,-10))){
        z[0] = sign | 0x03;    //nv subnormal 3.  2.5-3.5
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)>= 7*pow(2,-10) && abs(_temp_float.fp32)<= 9*pow(2,-10))){
        z[0] = sign | 0x04;    //nv subnormal 4.  3.5-4.5
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)> 9*pow(2,-10) && abs(_temp_float.fp32)< 11*pow(2,-10))){
        z[0] = sign | 0x05;    //nv subnormal 5.  4.5-5.5
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)>= 11*pow(2,-10) && abs(_temp_float.fp32)<= 13*pow(2,-10))){
        z[0] = sign | 0x06;    //nv subnormal 6.  5.5-6.5
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)> 13*pow(2,-10) && abs(_temp_float.fp32)< 15*pow(2,-10))){
        z[0] = sign | 0x07;    //nv  7.  6.5-7.5
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)>= 15*pow(2,-10) && abs(_temp_float.fp32)< 17*pow(2,-10))){
        z[0] = sign | 0x08;    //nv minnormal 7.  6.5-7.5
       // printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e\n",_temp_float.fp32);
        return;
    }
   	if(exp==0){  //TODO may not be used
        if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        // else
        //     z[0] = sign | 0x00;    //nv zero

       // printf("_float_to_fp8_143:[%d] Unbiliveble!!!!!!!!!!!!\n",__LINE__);
        //return;
    }

    //rouding
  //  printf("_float_to_fp8_143:before rounding x_u32=%x\n",x_u32);
    uint32_t lsb = (x_u32 >> 20) & 1;
    uint32_t rounding_bias = 0x7ffff + lsb;
    x_u32 += rounding_bias;
    x_u32 &= 0xfff00000;

  //  printf("_float_to_fp8_143:after rounding x_u32=%x\n",x_u32);

    // overflow
    // max normal

    if ((nv_or_gr==gr) and abs(_temp_float.fp32)>(1.875*pow(2,15-bias))) {
        z[0] = sign | 0x80; //gr inf 10000000
      //  printf("_float_to_fp8_143:Oops, overflow!!!! return gr inf\n");
        return;
    }
    if ((nv_or_gr == nv) and abs(_temp_float.fp32)>(1.75*pow(2,15-bias))){
       // printf("abs(_temp_float.fp32)=%f\n",_temp_float.fp32);
        z[0] = sign | 0x7e; //nv max normal s.1111.110
       // printf("_float_to_fp8_143:Oops, overflow!!!! return nv max\n");
        return;
    }


    exp  = 0xff & (x_u32 >> 23); //pick new exp after rounding 
   // printf("_float_to_fp8_143:exp after cut: %x\n",exp);
    if((exp+ bias) <127){
         if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero
        return;
    }
    exp = exp + bias - 127 ;


    

    z[0]= sign | ((exp & 0xf)<<3) | ((x_u32 >> 20) & 0x7);
}

void _double_to_fp8_143(/*fp32 input*/const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, /*output fp8*/uint8_t *z){
    
    uint64_t x_u64=*(uint64_t *)x;
    uint8_t sign = 0x80 & (x_u64>>56);
    uint16_t exp  = 0x7ff & (x_u64 >> 52);
    //uint64_t val = 0xfffffffffffff & x_u64;

    self_double _temp_double;
    _temp_double.value = x_u64;

    // printf("\n_double_to_fp8_143[%d] x_u64=%lx\n",__LINE__,x_u64);
    // printf("_double_to_fp8_143:sign= %0x, exp=%0x, val=%0x.\n",sign, exp, val);

    //NAN
    if (std::isnan(_temp_double.fp64)) { //NAN
        if(nv_or_gr==gr) 
            z[0] = 0x80;    //gr NAN
        else
            z[0] = 0xfF; // nv NAN
        //printf("_double_to_fp8_143:is nan and return\n");
        return;
    }

    //INF
     if (std::isinf(_temp_double.fp64)) {
        if(nv_or_gr==gr) 
            z[0] = 0x80; // gr inf
        else
            z[0] = sign | 0x7e;    //nv inf -> max_normal
        
       // printf("_double_to_fp8_143:is inf and return\n");
        return;
     }

    // //subnormal && zero && less min normal
if ((nv_or_gr==gr) and (abs(_temp_double.fp64)<pow(2,0-bias))) {
        z[0] = 0x0; // gr zero
       // printf("_double_to_fp8_143:is less min normal and return nv zero\n double.fp64= %e, pow(2,1-bias)= %e\n",_temp_double.fp64,pow(2,1-bias));
        return;
    }

    // if ((nv_or_gr == nv) and (abs(_temp_double.fp64)<pow(2,1-bias))){
    //     z[0] = sign | 0x00;    //nv zero
    //     printf("_double_to_fp8_143:is less min normal and return nv zero\n double.fp64= %e, pow(2,1-bias)= %e\n",_temp_double.fp64,pow(2,1-bias));
    //     return;
    // }

   	if(exp==0){  //TODO may not be used
        if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero

      //  printf("_double_to_fp8_143:[%d] Unbiliveble!!!!!!!!!!!!\n",__LINE__);
        //return;
    }

    //rouding
   // printf("_double_to_fp8_143:before rounding x_u64=%lx\n",x_u64);
    uint32_t lsb = (x_u64 >> 49) & 1;
    uint64_t rounding_bias = 0xffffffffffff + lsb;
    x_u64 += rounding_bias;
    x_u64 &= 0xfffe000000000000;

   // printf("_double_to_fp8_143:after rounding x_u64=%lx\n",x_u64);

    // overflow
    // max normal
    //_temp_double.value=x_u64;

    if ((nv_or_gr==gr) and abs(_temp_double.fp64)>(1.875*pow(2,15-bias))) {
        z[0] = sign | 0x80; //gr inf 10000000
        //printf("_float_to_fp8_143:Oops, overflow!!!! return gr inf\n");
        return;
    }
    if ((nv_or_gr == nv) and abs(_temp_double.fp64)>(1.75*pow(2,15-bias))){
      //  printf("abs(_temp_double.fp32)=%e\n",_temp_double.fp64);
        z[0] = sign | 0x7e; //nv max normal s.1111.110
      //  printf("_double_to_fp8_143:Oops, overflow!!!! return nv max\n");
        return;
    }


    exp  = 0x7ff & (x_u64 >> 52); //pick new exp after rounding 
    //printf("_double_to_fp8_143:exp after cut: %x\n",exp);
    if((exp+ bias) <1023){
         if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero
        return;
    }
    exp = exp + bias - 1023 ;

    z[0]= sign | ((exp & 0xf)<<3) | ((x_u64 >> 49) & 0x7);
}

void _float_to_fp8_152(/*fp32 input*/const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, /*output fp8*/uint8_t *z){
    
    uint32_t x_u32=*(u_int32_t *)x;
    uint8_t sign = 0x80 & (x_u32>>24);
    uint8_t exp  = 0xff & (x_u32 >> 23);
    //uint32_t val = 0x7fffff & x_u32;

    self_float _temp_float;
    _temp_float.value = x_u32;

    //printf("sign= %0x, exp=%0x, val=%0x.\n",sign, exp, val);

    //NAN
    if (std::isnan(_temp_float.fp32)) { //NAN
        if(nv_or_gr==gr) 
            z[0] = 0x80;    //gr NAN
        else
            z[0] = 0xff; // nv NAN
    //    printf("is nan and return\n");
        return;
    }

    //INF
     if (std::isinf(_temp_float.fp32)) {
        if(nv_or_gr==gr) 
            z[0] = 0x80; // gr inf
        else
            z[0] = sign | 0x7b;    //nv inf
        
    //    printf("_float_to_fp8_152[%d] is inf and return\n",__LINE__);
        return;
     }

    // //subnormal && zero && less min normal
    if ((nv_or_gr==gr) and (abs(_temp_float.fp32)<pow(2,0-bias))) {
        z[0] = 0x0; // gr zero
    //    printf("\n_float_to_fp8_152[%d]is less min normal and return gr zero\n",__LINE__);
        return;
    }

    // if ((nv_or_gr == nv) and (abs(_temp_float.fp32)<pow(2,1-bias))){
    //     z[0] = sign | 0x00;    //nv zero
    //     printf("\n_float_to_fp8_152[%d]is less min normal and return nv zero\n",__LINE__);
    //     return;
    // } 1.17549e-38 7.62939e-06abs(_temp_float.fp32)>=pow(1.17549,0-38)
    // if ((nv_or_gr == nv) and (abs(_temp_float.fp32)<=7.62939453125e-6)){//0x37000000
    //     z[0] = sign | 0x00;    //nv minsubnormal 
    // // printf("\n_float_to_fp8_152[%d]is min normal\n",__LINE__);7.62939
    //     return;
    //  }abs(_temp_float.fp32)<=7.62939453125e-6)
     if ((nv_or_gr == nv) and (abs(_temp_float.fp32)<=7.62939453125e-6)){//0x37000000
        z[0] = sign | 0x00;    //nv minsubnormal 
    // printf("\n_float_to_fp8_152[%d]is min normal\n",__LINE__);7.62939
        return;
     }
     if ((nv_or_gr == nv) and (abs(_temp_float.fp32)< 2.288818359375e-5 && abs(_temp_float.fp32)>7.62939453125e-6)){//1.5e-5 2e-16
        z[0] = sign | 0x01;    //nv minsubnormal 1.53184e-05 2.28286
    // printf("\n_float_to_fp8_152[%d]is min normal\n",__LINE__);
        return;
     }
    // if ((nv_or_gr == nv) and (abs(_temp_float.fp32)< 2.288818359375e-5 && abs(_temp_float.fp32)>7.62939453125e-6)){//1.5e-5 2e-16
    //     z[0] = sign | 0x01;    //nv minsubnormal 1.53184e-05 2.28286
    // // printf("\n_float_to_fp8_152[%d]is min normal\n",__LINE__);
    //     return;
    //  }
     if ((nv_or_gr == nv) and (abs(_temp_float.fp32)< 3.81469762942288070917129516602E-5 && abs(_temp_float.fp32)>=2.288818359375e-5)){
         z[0] = sign | 0x02;    //nv minsubnormal 2.28882e-05 2.28882 0x38200001 
        // printf("\n_float_to_fp8_152[%d]is min normal\n",__LINE__);
         return;
     }
    if ((nv_or_gr == nv) and (abs(_temp_float.fp32)< 5.340576171875E-5 && abs(_temp_float.fp32)>= 3.81469762942288070917129516602E-5)){
         z[0] = sign | 0x03;    //nv minsubnormal 
        // printf("\n_float_to_fp8_152[%d]is min normal\n",__LINE__);
         return;
     }
    
      if ((nv_or_gr == nv) and (abs(_temp_float.fp32)<=pow(2,0-14) && abs(_temp_float.fp32)>= 5.340576171875E-5)){
         z[0] = sign | 0x04;    //nv minnormal
        // printf("\n_float_to_fp8_152[%d]is min normal\n",__LINE__);
         return;
     }
   	// if(exp==0){  //TODO may not be used
    //     if(nv_or_gr==gr) 
    //         z[0] = 0x0; // gr zero
    //     else
    //         z[0] = sign | 0x00;    //nv zero

    // //    printf("_float_to_fp8_152:[%d]Unbiliveble!!!!!!!!!!!!\n",__LINE__);
    //     return;
    // }

    //rouding
//printf("before rounding x_u32=%x\n",x_u32);
    uint32_t lsb = (x_u32 >> 21) & 1;
    uint32_t rounding_bias = 0xfffff + lsb;
    x_u32 += rounding_bias;
    x_u32 &= 0xffe00000;

   // printf("after rounding x_u32=%x\n",x_u32);

    // overflow
    // max normal

    if ((nv_or_gr==gr) and abs(_temp_float.fp32)>(1.75*pow(2,31-bias))) {
        z[0] = sign | 0x80; //gr inf 10000000
     //   printf("Oops, overflow!!!! return gr inf\n");
     //   printf("res=%e, great than %e",_temp_float.fp32, (1.75*pow(2,31-bias)));
        return;
    }
    if ((nv_or_gr == nv) and abs(_temp_float.fp32)>(1.75*pow(2,30-bias))){
        z[0] = sign | 0x7b; //nv inf s.11111.00
        //nv max normal
        //printf("Oops, overflow!!!! return nv inf\n"); //TODO
        //printf("\ninput= %f, 1.75*pow(2,30-%d)=%f\n\n",_temp_float.fp32,bias,1.75*pow(2,30-bias));
        return;
    }


    exp  = 0xff & (x_u32 >> 23); //pick new exp after rounding 
    //printf("exp after cut: %x\n",exp);
    if ((exp+bias)<127){
        if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero
        return;
    }
    exp = exp + bias - 127 ;
    //assert(exp<=0x1f);


    z[0]= sign | ((exp & 0x1f)<<2) | ((x_u32 >> 21) & 0x3);
}

void _double_to_fp8_152(/*fp64 input*/const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, /*output fp8*/uint8_t *z){
    
    uint64_t x_u64=*(uint64_t *)x;
    uint8_t sign = 0x80 & (x_u64>>56);
    uint16_t exp  = 0x7ff & (x_u64 >> 52);
    //uint64_t val = 0xfffffffffffff & x_u64;

    self_double _temp_double;
    _temp_double.value = x_u64;

  //  printf("\n_double_to_fp8_152[%d] x_u64=%lx\n",__LINE__,x_u64);
   // printf("sign= %0x, exp=%0x, val=%0lx.\n",sign, exp, val);

    //NAN
    if (std::isnan(_temp_double.fp64)) { //NAN
        if(nv_or_gr==gr) 
            z[0] = 0x80;    //gr NAN
        else
            z[0] = 0xff; // nv NAN
       // printf("is nan and return\n");
        return;
    }

    //INF
     if (std::isinf(_temp_double.fp64)) {
        if(nv_or_gr==gr) 
            z[0] = 0x80; // gr inf
        else
            z[0] = sign | 0x7c;    //nv inf
        
       // printf("_float_to_fp8_152[%d] is inf and return\n",__LINE__);
        return;
     }

    // //subnormal && zero && less min normal
if ((nv_or_gr==gr) and (abs(_temp_double.fp64)<pow(2,0-bias))) {
        z[0] = 0x0; // gr zero
      //  printf("\n_double_to_fp8_152[%d]is less min normal and return gr zero\n",__LINE__);
        return;
    }

    // if ((nv_or_gr == nv) and (abs(_temp_double.fp64)<pow(2,1-bias))){
    //     z[0] = sign | 0x00;    //nv zero
    //     printf("\n_double_to_fp8_152[%d]is less min normal and return nv zero\n",__LINE__);
    //     return;
    // }

   	if(exp==0){  //TODO may not be used
        if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero

       // printf("_double_to_fp8_152:[%d]Unbiliveble!!!!!!!!!!!!\n",__LINE__);
        return;
    }

    //rounding fp64 to fp8_52
    //printf("before rounding x_u64=%lx\n",x_u64);
    uint32_t lsb = (x_u64 >> 50) & 1;
    uint64_t rounding_bias = 0x1ffffffffffff + lsb;
    x_u64 += rounding_bias;
    x_u64 &= 0xfffc000000000000;

    //printf("after rounding x_u64=%lx\n",x_u64);

    // overflow
    // max normal
    //_temp_double.value = x_u64;
    if ((nv_or_gr==gr) and abs(_temp_double.fp64)>(1.75*pow(2,31-bias))) {
        z[0] = sign | 0x80; //gr inf 10000000
       // printf("Oops, overflow!!!! return gr inf\n");
      //  printf("res=%e, great than %e",_temp_double.fp64, (1.75*pow(2,31-bias)));
        return;
    }
    if ((nv_or_gr == nv) and abs(_temp_double.fp64)>(1.75*pow(2,30-bias))){
        z[0] = sign | 0x7c; //nv inf s.11111.00
       // printf("Oops, overflow!!!! return nv inf\n"); //TODO
       // printf("\ninput= %f, 1.75*pow(2,30-%d)=%f\n\n",_temp_double.fp64,bias,1.75*pow(2,30-bias));
        return;
    }


    exp  = 0x7ff & (x_u64 >> 52); //pick new exp after rounding 
    //printf("exp after cut: %x\n",exp);
    if ((exp+bias)<1023){
        if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero
        return;
    }
    exp = exp + bias - 1023 ;
    //assert(exp<=0x1f);


    z[0]= sign | ((exp & 0x1f)<<2) | ((x_u64 >> 50) & 0x3);
}

//quantize fp24 M1 to fp8
void _quantize_fp24_m1_to_fp8(/*fp24 u32 input*/const uint8_t *x, /*nf18 u32 input*/const uint8_t *y, 
                            /*0:152,1:143*/uint8_t fp8_dtype, /*0:nv,1:gr*/uint8_t fp8_mode, uint8_t bias, 
                            /*output fp8*/uint8_t *z)
{
    //fp24 input 00sign.exp.mantissa
    self_float input_fp32;
    input_fp32.value = *(uint32_t *)x;
    input_fp32.value <<=8;

    fp8_format nv_or_gr;
    nv_or_gr=fp8_mode?gr:nv;

    //TODO need consider subnormal inf NAN
    if (isinf(input_fp32.fp32)){
       // printf("get inf input!!!!!\n");
        if (nv_or_gr == nv){ //nv mode
            if (fp8_dtype == 0){ //nv 152 inf
                z[0]= ((input_fp32.value)>>24 & 0x80) | 0x7c;        
            } else { //nv 143 inf
                z[0]= ((input_fp32.value)>>24 & 0x80) | 0x7e;   
            }
        } else { //gr mode
            z[0]= 0x80;  // gr 152 & 143 inf       
        }

        return;
    }

    if (isnan(input_fp32.fp32)){
       // printf("get nan input!!!!!\n");
        if (nv_or_gr == nv){ //nv mode
            //z[0]= 0x7f;   
            z[0]= ((input_fp32.value)>>24 & 0x80) | 0x7f;    // nv 152 & 143 nan          
        } else { //gr mode
            z[0]= 0x80;  // gr 152 & 143 nan       
        }
       // printf("_quantize_fp24_m1_to_fp8():return fp8 nan = %x\n",z[0]);
        return;
    }

    float m1_f;
    _nf18_to_float(y, (uint8_t *)(&m1_f));

    self_double res_64;
    res_64.fp64=(double)input_fp32.fp32*(double)m1_f;


    // printf("_quantize_fp24_m1_to_fp8[%d]:input fp24 to fp32:%e\n",__LINE__, input_fp32.fp32);
    // printf("_quantize_fp24_m1_to_fp8[%d]:input nf18 to fp32:%e\n",__LINE__,m1_f);
    // printf("_quantize_fp24_m1_to_fp8[%d]:z_f after multi:%e(Hex 0x%lx)\n",__LINE__,res_64.fp64,res_64.value);

    double z_f = res_64.fp64;
    
    //check NAN INF after multi
    if (std::isinf(z_f) or std::isnan(z_f)){
       // printf("multi result get inf/nan, output inf\n");
        if (nv_or_gr == nv){ //nv mode
            if (fp8_dtype == 0){ //nv 152 inf 
                //z[0]= ((z_f>0)?0x00 : 0x80) | 0x7b;
                z[0]= (z_f>0)?0x7c:0xfc;        
            } else { //nv 143 inf
                //z[0]= ((z_f>0)?0x00 : 0x80) | 0x7e; 
                z[0]= (z_f>0)?0x7e:0xfe;    
            }
        } else { //gr mode
            // gr 152 & 143 inf   
            z[0]= 0x80;    
        }

        return;
    }

    uint8_t z_fp8;
    if (fp8_dtype==1){ //143
        //_float_to_fp8_143((uint8_t *)&z_f,nv_or_gr,bias,&z_fp8);
        _double_to_fp8_143((uint8_t *)&z_f,nv_or_gr,bias,&z_fp8);
    } else {
        //_float_to_fp8_152((uint8_t *)&z_f,nv_or_gr,bias,&z_fp8);
        _double_to_fp8_152((uint8_t *)&z_f,nv_or_gr,bias,&z_fp8);
    }

    z[0]=z_fp8;
}

void _dequantize_fp24_m3_to_fp24(/*fp24 u32 input*/const uint8_t *x, /*nf18 u32 input*/const uint8_t *y, 
                                /*output fp24*/uint8_t *z)
{
    //fp24 input 00sign.exp.mantissa
    self_float input_fp32;
    input_fp32.value = *(uint32_t *)x;
    input_fp32.value <<=8;

  //  printf("_dequantize_fp24_m3_to_fp24[]input fp24 value= %x, float = %e\n",input_fp32.value,input_fp32.fp32);
    //TODO need consider subnormal inf NAN
    if(isnan(input_fp32.fp32)){
        *(uint32_t *)z=0x7fc000;
        return;
    }

    if(isinf(input_fp32.fp32)){
        *(uint32_t *)z=(input_fp32.fp32>0)?0x7f8000:0xff8000;
        return;
    }

    // if(is_small_number(input_fp32.fp32)){
    //     printf("_dequantize_fp24_m3_to_fp24[%d] input subnormal\n",__LINE__);
    //     *(uint32_t *)z=(input_fp32.fp32>0)?0x0:0x800000;
    //     return;
    // }

    float m3_f;
    _nf18_to_float(y, (uint8_t *)(&m3_f));

  //  printf("_dequantize_fp24_m3_to_fp24[]input nf18 float = %e\n",m3_f);
    //result mantissa need 16bit+11bit which beyond fp32 23bit matissa,


    self_double z_double;
    
    z_double.fp64 =(double)input_fp32.fp32*(double)m3_f;

   // printf("_dequantize_fp24_m3_to_fp24[%d]z_double.value after product %lx(hex)\n",__LINE__, z_double.value);



    //rounding fp64 to fp24
    uint64_t lsb =( z_double.value>> 37 ) & 1;
    uint64_t rounding_bias = 0xfffffffff + lsb;
    z_double.value += rounding_bias;
    z_double.value &= 0xffffffe000000000;

   // printf("_dequantize_fp24_m3_to_fp24[%d]z_double.value after rounding %lx(hex)\n",__LINE__, z_double.value);

    self_float z_fp24;
    z_fp24.fp32=(float)z_double.fp64;

    //inf check after product
    if (std::isinf(z_fp24.fp32)){
        *(uint32_t *)z=(z_fp24.fp32>0)?0x7f7fff:0xff7fff;
        return;
    }

    // //subnormal check after product and rounding
    // if(is_small_number(z_fp24.fp32)){
    //     printf("_dequantize_fp24_m3_to_fp24[%d] subnormal\n",__LINE__);
    //     *(uint32_t *)z=(z_fp24.fp32>0)?0x0:0x800000;
    //     return;
    // }

    //printf("_dequantize_fp24_m3_to_fp24[%d] z_fp24.fp32 value is %x\n",__LINE__, z_fp24.value);

    *(uint32_t *)z=(z_fp24.value>>8); //output 00.sign.exp.mantissa
   // printf("_dequantize_fp24_m3_to_fp24[%d]: output fp24.value= %x, fp24.fp24=%e\n",__LINE__, z_fp24.value>>8, z_fp24.fp32);
}