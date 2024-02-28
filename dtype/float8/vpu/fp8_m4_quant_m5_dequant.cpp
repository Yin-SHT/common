
//#include <cfenv>
//#include"bfloat24.cpp"
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <map>
#include"dtype/float8/float8.h"
#include "dtype/bfloat16.h"
#include "dtype/bfloat24.h"
#include "fp8_m4_quant_m5_dequant.h"
#include<bitset>

using namespace std;
extern void mul_bf16(const bfloat16 x, const bfloat16 y, bfloat24& z );
// extern void add_bf24(const bfloat24 x, const bfloat24 y, bfloat24& z );
// extern void add_bf24(const uint32_t x, const uint32_t y, uint32_t& z );
// extern void add_bf24_to_bf16(const bfloat24 x, const bfloat24 y, bfloat16& z );
extern void bf24_to_bf16(const bfloat24 x, bfloat16& z);
// extern void bf24_to_bf16(const uint32_t x, int16_t& z);
#define BF16_NEG_INF 0xFF80
#define BF16_POS_INF 0x7F80
#define BF16_POS_ZERO 0x0000
#define BF16_NEG_ZERO 0x8000
#define BF16_NAN_VALUE 0x7FC0

uint8_t GetExponent_vpu(float a)
{
    auto x = (uint32_t*)(&a);
    uint32_t value = *x;
    value = value >> 23;
    uint8_t exp = value & 0xff;
    return exp;
};
uint8_t GetSignificand_vpu(float a)
{
    auto x = (uint32_t*)(&a);
    uint32_t value = *x;
    value = value >> 16;
    uint8_t sig = value & 0x7f;
    return sig;
};

#if 0

 void _fp8_143_to_float(const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, uint32_t *z ) {
    uint8_t sign = 0x1 & (x[0]>>7);
    uint8_t exp = 0x0F & (x[0] >> 3);
    uint8_t val = 0x07 & x[0];
    ////printf("[%d] input para sign=%x, exp= %x, val=%x\n", __LINE__, sign, exp, val);

    if(nv_or_gr == nv) CHECK(bias == 7);

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

    //in V2.0, no keep fp8 subnormal. 
    //if((exp == 0x0) && (val>0x0)) { // subnormal
    //    if (nv_or_gr == gr) {
    //        z[0] = sign<<31;
    //    } else {
    //        z[0] = sign<<31;
    //    }
    //    return;
    //}

    //no special values
    exp = exp  + 127 - bias;
    CHECK ((exp>=0) && (exp<256));
    z[0] = (sign << 31) | (exp<<23) |(val<<20);
    // self_float output;
    // output.value = z_32;
    // //printf("in function [%d] output.value = %0x, output.fp32 = %f\n", __LINE__, output.value, output.fp32);
    //output
    // *((uint32_t *)(z))= z_32;
}

 void _fp8_152_to_float(const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, uint32_t *z ) {
    uint8_t sign = 0x1 & (x[0]>>7);
    uint8_t exp  = 0x1F & (x[0] >> 2);
    uint8_t val  = 0x03 & x[0];
    printf("_fp8_152_to_float [%d] input para sign=%x, exp= %x, val=%x\n", __LINE__, sign, exp, val);

    if(nv_or_gr == nv) CHECK(bias == 15);
    

    if ((sign == 0x00) && (exp == 0x00) && (val == 0x00)) { //all 0
        // output 0
        z[0] = 0x00;
        //printf("_fp8_152_to_float[%d] return 0\n",__LINE__);
        return;
    }

    if ((sign == 0x1) && (exp == 0x00) && (val == 0x00)){
        if( nv_or_gr == gr) { //gr NAN
            z[0] = 0x7fc00000;
        } else { //nv -0
            z[0] = 0x80000000;
        }
        //printf("_fp8_152_to_float[%d] return -0\n",__LINE__);
        return;
    } 
    
    
    if ((nv_or_gr == nv) && (exp == 0x1f) && (val == 0x0)){ //nv inf
        z[0] = sign? 0xff800000:0x7F800000;
        //printf("_fp8_152_to_float[%d] return nv inf\n",__LINE__);
        return;
    }
    
    if ((nv_or_gr == nv) && (exp == 0x1f) && (val > 0x0)){ //nv NAN
        z[0] = 0x7fc00000;
        //printf("_fp8_152_to_float[%d] return nv nan\n",__LINE__);
        return;
    }
    
    //in V2.0, no keep fp8 subnormal. 
    // if((exp == 0x0) && (val>0x0)) { // subnormal
    //     if (nv_or_gr == gr) {
    //         z[0] = sign<<31;
    //     } else {
    //         z[0] = sign<<31;
    //     }
    //     //printf("_fp8_152_to_float[%d] return subnormal %032x\n",__LINE__, z[0]);
    //     return;
    // } 
    
    //no special values
    exp = exp  + 127 - bias;
    //printf("_fp8_152_to_float[%d] exp_of_float=%x\n",__LINE__,exp);
    CHECK ((exp>=0) && (exp<256));
    z[0] = (sign << 31) | (exp<<23) |(val<<21);

    // self_float output;
    // output.value = z_32;
    // //printf("in function [%d] output.value = %0x, output.fp32 = %f\n", __LINE__, output.value, output.fp32);
    //output
}



void _float_to_fp8_143(/*fp32 input*/const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, /*output fp8*/uint8_t *z){
    
    uint32_t x_u32=*(uint32_t *)x;
    uint8_t sign = 0x80 & (x_u32>>24);
    uint8_t exp  = 0xff & (x_u32 >> 23);
    uint32_t val = 0x7fffff & x_u32;

    self_float _temp_float;
    _temp_float.value = x_u32;

    //printf("_float_to_fp8_143:sign= %0x, exp=%0x, val=%0x.\n",sign, exp, val);

    //NAN
    if (std::isnan(_temp_float.fp32)) { //NAN
        if(nv_or_gr==gr) 
            z[0] = 0x80;    //gr NAN
        else
            //z[0] = 0xfF; // nv NAN
            z[0] = sign | 0x7F;
        //printf("_float_to_fp8_143:is nan and return\n");
        return;
    }

    //INF
     if (std::isinf(_temp_float.fp32)) {
        if(nv_or_gr==gr) 
            z[0] = 0x80; // gr inf
        else
            //z[0] = sign | 0x7F;    //nv inf ->NAN
            z[0] = sign | 0x7e;    //nv max
        
        //printf("_float_to_fp8_143:is inf and return\n");
        return;
     }

    //subnormal && zero && less min normal
     //if ((nv_or_gr==gr) and (abs(_temp_float.fp32)<pow(2,1-bias))) { //V1.5, process subnormal 
     if ((nv_or_gr==gr) and (abs(_temp_float.fp32)<pow(2,0-bias))) { //V2.0, do not process subnormal 
         z[0] = 0x0; // gr zero
         //printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e, pow(2,1-bias)= %e\n",_temp_float.fp32,pow(2,1-bias));
         return;
     }

     if ((nv_or_gr == nv) and (abs(_temp_float.fp32)<pow(2,0-bias))){
         z[0] = sign | 0x00;    //nv zero
         //printf("_float_to_fp8_143:is less min normal and return nv zero\n float.fp32= %e, pow(2,1-bias)= %e\n",_temp_float.fp32,pow(2,1-bias));
         return;
     }

   	if(exp==0 && val==0){  //TODO may not be used
        if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero
        //printf("_float_to_fp8_143:[%d] Unbiliveble!!!!!!!!!!!!\n",__LINE__);
        //return;
    }

    //rouding
    //printf("_float_to_fp8_143:before rounding x_u32=%x\n",x_u32);
    uint32_t lsb = (x_u32 >> 20) & 1;
    uint32_t rounding_bias = 0x7ffff + lsb;
    x_u32 += rounding_bias;
    x_u32 &= 0xfff00000;

    //printf("_float_to_fp8_143:after rounding x_u32=%x\n",x_u32);

    // overflow
    // max normal
    //   self_float _temp_float_;
    // _temp_float_.value = x_u32; //
    if ((nv_or_gr==gr) and abs(_temp_float.fp32)>(1.875*pow(2,15-bias))) {
        z[0] = 0x80; //inf
        //z[0] = sign | 0x7f; //gr max normal s.1111.111
        //printf("_float_to_fp8_143:Oops, overflow!!!! return gr max\n");
        return;
    }
    //43 max
    if ((nv_or_gr == nv) and abs(_temp_float.fp32)>(1.75*pow(2,15-bias))){
        //printf("abs(_temp_float.fp32)=%f\n",_temp_float.fp32);
        z[0] = sign | 0x7e; //nv max normal s.1111.110
        //printf("_float_to_fp8_143:Oops, overflow!!!! return nv max\n");
        return;
    }


    exp  = 0xff & (x_u32 >> 23); //pick new exp after rounding 
    //printf("_float_to_fp8_143:exp after cut: %x\n",exp);
    //if((exp+ bias) <127){
    if((exp+ bias) <126){    
         if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero
        return;
    }
    exp = exp + bias - 127 ;


    

    z[0]= sign | ((exp & 0xf)<<3) | ((x_u32 >> 20) & 0x7);
}

void _float_to_fp8_152(/*fp32 input*/const uint8_t *x, fp8_format nv_or_gr, uint8_t bias, /*output fp8*/uint8_t *z){
    
    uint32_t x_u32=*(uint32_t *)x;
    uint8_t sign = 0x80 & (x_u32>>24);
    uint8_t exp  = 0xff & (x_u32 >> 23);
    uint32_t val = 0x7fffff & x_u32;

    self_float _temp_float;
    _temp_float.value = x_u32;

    //printf("sign= %0x, exp=%0x, val=%0x.\n",sign, exp, val);

    //NAN
    if (std::isnan(_temp_float.fp32)) { //NAN
        if(nv_or_gr==gr) 
            z[0] = 0x80;    //gr NAN
        else
            z[0] = sign | 0x7f; // nv NAN
        //printf("is nan and return\n");
        return;
    }

    //INF
     if (std::isinf(_temp_float.fp32)) {
        if(nv_or_gr==gr) 
            z[0] = 0x80; // gr inf
        else
            z[0] = sign | 0x7c;    //nv inf
        
        //printf("_float_to_fp8_152[%d] is inf and return\n",__LINE__);
        return;
     }

    //subnormal && zero && less min normal
     if ((nv_or_gr==gr) and (abs(_temp_float.fp32)<pow(2,0-bias))) {
         z[0] = 0x0; // gr zero
         //printf("\n_float_to_fp8_152[%d]is less min normal and return gr zero\n",__LINE__);
         return;
     }

     if ((nv_or_gr == nv) and (abs(_temp_float.fp32)<pow(2,0-bias))){
         z[0] = sign | 0x00;    //nv zero
         //printf("\n_float_to_fp8_152[%d]is less min normal and return nv zero\n",__LINE__);
         return;
     }

   	if(exp==0 && val==0){  //TODO may not be used
        if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero

        //printf("_float_to_fp8_152:[%d]Unbiliveble!!!!!!!!!!!!\n",__LINE__);
        return;
    }

    //rouding
    //printf("before rounding x_u32=%x\n",x_u32);
    uint32_t lsb = (x_u32 >> 21) & 1;
    uint32_t rounding_bias = 0xfffff + lsb;
    x_u32 += rounding_bias;
    x_u32 &= 0xffe00000;

    //printf("after rounding x_u32=%x\n",x_u32);

    // overflow
    // max normal

    //  self_float _temp_float_;
    // _temp_float_.value = x_u32; //_temp_float.value = x_u32 is before round data  _temp_float_.value is after round data 
    if ((nv_or_gr==gr) and abs(_temp_float.fp32)>(1.75*pow(2,31-bias))) {
      //  z[0] = sign | 0x7f; //gr max normal s.11111.11
        z[0] = 0x80;
        //printf("Oops, overflow!!!! return gr max\n");
        
        return;
    }
    if ((nv_or_gr == nv) and abs(_temp_float.fp32)>(1.75*pow(2,30-bias))){
        // z[0] = sign | 0x7b; //nv max normal s.11110.11
         z[0] = sign | 0x7c; //nv inf
        //printf("Oops, overflow!!!! return nv max\n");
        //printf("\ninput= %f, 1.75*pow(2,30-bias)=%f\n\n",_temp_float.fp32,1.75*pow(2,30-bias));
        return;
    }


    exp  = 0xff & (x_u32 >> 23); //pick new exp after rounding 
    //printf("exp after cut: %x\n",exp);
    //if ((exp+bias)<127){
    if ((exp+bias)<126){    
        if(nv_or_gr==gr) 
            z[0] = 0x0; // gr zero
        else
            z[0] = sign | 0x00;    //nv zero
        return;
    }
    exp = exp + bias - 127 ;
    //CHECK(exp<=0x1f);


    z[0]= sign | ((exp & 0x1f)<<2) | ((x_u32 >> 21) & 0x3);
}

#endif



void dequantize_fp8_to_fp32(const uint8_t *x, const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, /* fp24*/const uint32_t *coeff, uint32_t *z) //fp8_type 0 nv, 1 gr . fp8_em_type 0is52 1is 43
{
   //fp8*fp24=fp32
    float    fp8_f;//TODO

    uint32_t    fp8_u32;
    fp8_format nv_or_gr;
    uint8_t fp8_mode = *fp8_ty;
    uint8_t fp8_dtype = *fp8_em_ty;
    nv_or_gr= fp8_mode ? gr : nv;
   
    uint8_t bias = fp8_dtype ? 7: 15;
    if(fp8_dtype==1)//143
         _fp8_143_to_float(x, nv_or_gr, bias, &fp8_u32);//
    else //152
         _fp8_152_to_float(x, nv_or_gr, bias, &fp8_u32);
    
    self_float _temp_sfloat;
    _temp_sfloat.value = fp8_u32;//in
    fp8_f=_temp_sfloat.fp32;
    
     //fp24 input 00sign.exp.mantissa
    self_float input_fp32;
    input_fp32.value = *(uint32_t *)coeff;
    input_fp32.value <<=8;//fp32
    // all +factor
    input_fp32.fp32 = abs(input_fp32.fp32);
    //printf("value = %x, %f, factor= %f\n",  _temp_sfloat.value, fp8_f, input_fp32.fp32 );
    if(((input_fp32.value >> 23) & 0xff) == 0 && (input_fp32.value & 0x7fffff) != 0)//factor 
        {        
            input_fp32.value = 0;//subnormal
            
        }
   
  if(std::isnan(input_fp32.fp32) || isnan(fp8_f))
    {
        z[0] = 0x7fc00000;
        return;
    }
   else if(std::isinf(input_fp32.fp32) &&  (_temp_sfloat.value == 0x00000000 || _temp_sfloat.value== 0x80000000))// factor inf *0
   {
     z[0] = 0x7fc00000;
        return;
   }
   else if(isinf(fp8_f) && (input_fp32.fp32 == 0))
   {
    z[0] = 0x7fc00000;
        return;
   }
   else if(isinf(fp8_f) || isinf(input_fp32.fp32)){
        z[0]= (fp8_f>0) ? 0x7f800000 : 0xff800000;
        return;
   }
    float z_f=fp8_f*input_fp32.fp32; 
   
    
    self_float z_fp32;
    z_fp32.fp32 = z_f;
   // //printf("mult output fp32 value= %x\n",  z_fp32.value );
    if(std::isnan( z_fp32.fp32)) { //
        z[0] = 0x7fc00000;
        return;
    }
  
    // else if (std::isinf(z_fp32.fp32)){//maxnormal
    //     z[0]= (z_fp32.fp32>0) ? 0x7f7fffff : 0xff7fffff;
    //     return;
    //  }
    //change max to inf
    else if (std::isinf(z_fp32.fp32)){//inf
        // z[0]= z_fp32.fp32;
        // return;
        z[0]= (z_fp32.fp32>0) ? 0x7f800000 : 0xff800000;
        return;
     }
    
    self_float z_out;
   // z_out.fp32 = subnormal(z_fp32.fp32);
     z_out.fp32 = z_fp32.fp32;
    // z[0] = z_fp32.value;
    z[0] = z_out.value;


}
void quantize_fp32_to_fp8(const uint32_t *x, const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, /* bf16*/const uint16_t *coeff, uint8_t *z) //fp8_type 0 nv, 1 gr . fp8_em_type 0is52 1is 43
{//fp32*bf16 = fp8
   

    uint8_t temp;
    fp8_format nv_or_gr;
    uint8_t fp8_mode = *fp8_ty;
    uint8_t fp8_dtype = *fp8_em_ty;
    nv_or_gr= fp8_mode ? gr : nv;
    
    uint8_t bias = fp8_dtype ? 7: 15;
    self_float src;
    src.value = *x;
    bfloat16 y;
    y.value = *coeff;
     // printf("src = %x\n", src.value);
     //uint8_t sign_ = (src.value)>>24 & 0x80; 

       self_float factor;
        
        factor.value = *coeff << 16;
        self_float factor_fp32;
        factor_fp32.value = factor.value;
        
         factor_fp32.fp32 =  abs(factor_fp32.fp32);
        src.fp32 = subnormal(src.fp32);     // v1.6, input x subnormal
        factor_fp32.fp32 = subnormal(factor_fp32.fp32 ); // v1.6, input f subnormal

    if (isnan(src.fp32) || isnan(factor_fp32.fp32)){
        ////printf("get nan input!!!!!\n");
        if (nv_or_gr == nv){ //nv mode
           
            z[0]= 0x7f;   
          // z[0]= ((src.value)>>24 & 0x80) | 0x7f;    // nv 152 & 143 nan     
              // z[0] = sign_ | 0x7f;
        } else { //gr mode
            z[0]= 0x80;  // gr 152 & 143 nan       
        }
        ////printf("_quantize_fp24_m1_to_fp8():return fp8 nan = %x\n",z[0]);
        return;
    } 
   
    else if((isinf(src.fp32) &&  factor_fp32.value == 0x00000000) || (isinf(factor_fp32.fp32) && src.value == 0x00000000) || (isinf(factor_fp32.fp32) && src.value == 0x80000000)){// 0*inf = nan
           if (nv_or_gr == nv){ //nv mode
              // if (fp8_dtype == 0){ 
                z[0]= 0x7f; //nan
              // }  
            //z[0]= ((src.value)>>24 & 0x80) | 0x7f;    // nv 152 & 143 nan   
              //z[0] = sign_ | 0x7f;
        } else { //gr mode
            z[0]= 0x80;  // gr 152 & 143 nan       
        }
        ////printf("_quantize_fp24_m1_to_fp8():return fp8 nan = %x\n",z[0]);
        return;
    }
     //add change to maxnormal nv43
    // else if( ( isinf(src.fp32) || isinf(factor_fp32.fp32)) && (nv_or_gr == nv) && (fp8_dtype == 1) ){
    //      z[0]= ((src.value)>>24 & 0x80) | 0x7e;  //maxnormal
    //      return;
    // }   
    else if( (isinf(src.fp32) || (isinf(factor_fp32.fp32))))
    {
             if (nv_or_gr == nv){ //nv mode
                if (fp8_dtype == 0){ //nv 152 inf
                    z[0]= ((src.value)>>24 & 0x80) | 0x7b;        
                } else { //nv 143 inf
                    z[0]= ((src.value)>>24 & 0x80) | 0x7e;   //max
                }
            } else { //gr mode
                z[0]= 0x80;  // gr 152 & 143 inf       
            }

        return;
    }


        //fp32 rounding to fp24
	    uint32_t lsb = (src.value >> 8) & 1;
		uint32_t rounding_bias = 0x7f + lsb;
		src.value += rounding_bias;
		src.value &= 0xffffff00;
       
        // if (isinf(src.fp32)){//????????? inf to max ?
        //    src.value =  (src.fp32>0) ? 0x7f7fff00 : 0xff7fff00;
        // }//
        


        self_float input_fp32;
        input_fp32.value = src.value; //*(uint32_t *)
        //input_fp32.value >>=8;
      // printf(" input_fp32.value = %x\n", input_fp32.value);
       
       float n;
       n = input_fp32.fp32 * factor_fp32.fp32;
       ////printf("  n =  %x\n",   n);
       ////printf("  input_fp32.fp32 =  %f, factor_fp32.fp32 =  %f\n",   input_fp32.fp32, factor_fp32.fp32);
        if(isnan(n))//7fc0
        {
            if (nv_or_gr == nv){ //nv mode           
                z[0]=0x7f;              
            }else { //gr mode          
              z[0]= 0x80;        
            }
            return;
        }
        if (isinf(n)){
        // //printf("multi result get inf/nan, output max normal\n");
            if (nv_or_gr == nv){ //nv mode
                
                //change max      
                    if (fp8_dtype == 0){ //nv 152 
                    z[0]=(n>0)?0x7b:0xfb;    // max
           
                } else { //nv 143 max
                    z[0]= (n>0)?0x7e:0xfe;  //((n>0)?0x00 : 0x80) | 0x7e; 
                    // temp=   
                }
            } else { //gr mode
                // z[0]= (n>0)?0x7f:0xff; //((n>0)?0x00 : 0x80) | 0x7f;  ;  // gr 152 & 143 maxnormal  
                //change max to inf  
                z[0]= 0x80; //((n>0)?0x00 : 0x80) | 0x7f;  ;  // gr 152 & 143 inf
                // temp=    
            }
          return;
        }
   self_float fp24_out;
   fp24_out.fp32 = f32_to_bf24(n);

   //printf("fp24_out = %f,fp24_out = %x\n", fp24_out.fp32,fp24_out.value);
    if (fp8_dtype==1){ //143
        _float_to_fp8_143((uint8_t *)&fp24_out.fp32,nv_or_gr,bias,&temp);
    } else {
        _float_to_fp8_152((uint8_t *)&fp24_out.fp32,nv_or_gr,bias,&temp);
    }

    z[0]=temp;
}


void quantize_tf32_to_fp8(const uint32_t *x, const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, /* bf16*/const uint16_t *coeff, uint8_t *z) //fp8_type 0 nv, 1 gr . fp8_em_type 0is52 1is 43
{//tp32*bf16 = fp8
   

    uint8_t temp;
    fp8_format nv_or_gr;
    uint8_t fp8_mode = *fp8_ty;
    uint8_t fp8_dtype = *fp8_em_ty;
    nv_or_gr= fp8_mode ? gr : nv;
    
    uint8_t bias = fp8_dtype ? 7: 15;
    self_float src;
    uint32_t x_tp = *(uint32_t *)x;
    src.value = x_tp;
    bfloat16 y;
    y.value = *coeff;

     //uint8_t sign_ = (src.value)>>24 & 0x80; 

       self_float factor;
        
        factor.value = *coeff << 16;
        self_float factor_fp32;
        factor_fp32.value = factor.value;
        
         factor_fp32.fp32 =  abs(factor_fp32.fp32);
        src.fp32 = subnormal(src.fp32);     // v1.6, input x subnormal
        factor_fp32.fp32 = subnormal(factor_fp32.fp32 ); // v1.6, input f subnormal

    if (isnan(src.fp32) || isnan(factor_fp32.fp32)){
        ////printf("get nan input!!!!!\n");
        if (nv_or_gr == nv){ //nv mode
            z[0]= 0x7f;   
          // z[0]= ((src.value)>>24 & 0x80) | 0x7f;    // nv 152 & 143 nan     
              // z[0] = sign_ | 0x7f;
        } else { //gr mode
            z[0]= 0x80;  // gr 152 & 143 nan       
        }
        ////printf("_quantize_fp24_m1_to_fp8():return fp8 nan = %x\n",z[0]);
        return;
    } 
   
    else if((isinf(src.fp32) &&  factor_fp32.value == 0x00000000) || (isinf(factor_fp32.fp32) && src.value == 0x00000000) || (isinf(factor_fp32.fp32) && src.value == 0x80000000)){// 0*inf = nan
           if (nv_or_gr == nv){ //nv mode
              // if (fp8_dtype == 0){ 
                z[0]= 0x7f; //nan
              // }  
            //z[0]= ((src.value)>>24 & 0x80) | 0x7f;    // nv 152 & 143 nan   
              //z[0] = sign_ | 0x7f;
        } else { //gr mode
            z[0]= 0x80;  // gr 152 & 143 nan       
        }
        ////printf("_quantize_fp24_m1_to_fp8():return fp8 nan = %x\n",z[0]);
        return;
    }
     //add change to maxnormal nv43
    // else if( ( isinf(src.fp32) || isinf(factor_fp32.fp32)) && (nv_or_gr == nv) && (fp8_dtype == 1) ){
    //      z[0]= ((src.value)>>24 & 0x80) | 0x7e;  //maxnormal
    //      return;
    // }   
    else if( (isinf(src.fp32) || (isinf(factor_fp32.fp32))))
    {
             if (nv_or_gr == nv){ //nv mode
                if (fp8_dtype == 0){ //nv 152 max
                    z[0]= ((src.value)>>24 & 0x80) | 0x7b;        
                } else { //nv 143 inf
                    z[0]= ((src.value)>>24 & 0x80) | 0x7e;   //max
                }
            } else { //gr mode
                z[0]= 0x80;  // gr 152 & 143 inf       
            }

        return;
    }

        //fp32 rounding to fp24
	    // uint32_t lsb = (src.value >> 8) & 1;
		// uint32_t rounding_bias = 0x7f + lsb;
		// src.value += rounding_bias;
		// src.value &= 0xffffff00;
       
        // if (isinf(src.fp32)){//????????? inf to max ?
        //    src.value =  (src.fp32>0) ? 0x7f7fff00 : 0xff7fff00;
        // }//
        
       

        self_float input_fp32;
        input_fp32.value = src.value; //*(uint32_t *)
        //input_fp32.value >>=8;
       // //printf(" input_fp32.value = %x\n", input_fp32.value);
       
       float n;
       self_float res1;
       n = input_fp32.fp32 * factor_fp32.fp32;
        res1.fp32 = n;
        //round to tf32
        uint32_t lsb = (res1.value >> 13) & 1;
        uint32_t rounding_bias = 0xfff + lsb;
        res1.value += rounding_bias;
        res1.value &= 0xffffe000;
       ////printf("  n =  %x\n",   n);
       //printf("  input_fp32.fp32 =  %f, factor_fp32.fp32 =  %f\n",   input_fp32.fp32, factor_fp32.fp32);
        if(isnan(res1.fp32 ))//7fc0
        {
            if (nv_or_gr == nv){ //nv mode           
                z[0]=0x7f;              
            }else { //gr mode          
              z[0]= 0x80;        
            }
            return;
        }
        if (isinf(res1.fp32 )){
        // //printf("multi result get inf/nan, output max normal\n");
            if (nv_or_gr == nv){ //nv mode
                // if (fp8_dtype == 0){ //nv 152 maxnormal
                //     z[0]=(n>0)?0x7b:0xfb;    // ((n>0)?0x00 : 0x80) | 0x7b;
                //     // temp= (   
                //change max     
                    if (fp8_dtype == 0){ //nv 152 max
                    z[0]=(n>0)?0x7b:0xfb;    // max
             
                } else { //nv 143 max
                    z[0]= (n>0)?0x7e:0xfe;  //((n>0)?0x00 : 0x80) | 0x7e; 
                    // temp=   
                }
            } else { //gr mode
                // z[0]= (n>0)?0x7f:0xff; //((n>0)?0x00 : 0x80) | 0x7f;  ;  // gr 152 & 143 maxnormal  
                //change max to inf  
                z[0]= 0x80; //((n>0)?0x00 : 0x80) | 0x7f;  ;  // gr 152 & 143 inf
                // temp=    
            }
          return;
        }
//    self_float fp24_out;
//    fp24_out.fp32 = f32_to_bf24(n);
 
    if (fp8_dtype==1){ //143
        _float_to_fp8_143((uint8_t *)&res1.fp32 ,nv_or_gr,bias,&temp);
    } else {
        _float_to_fp8_152((uint8_t *)&res1.fp32 ,nv_or_gr,bias,&temp);
    }

    z[0]=temp;
}


   

//uint16_t dequantize_fp8_to_bf16(uint8_t src,int fp8_type,int fp8_em_type, bfloat16 coefficient) //fp8_type 0 nv, 1 gr . fp8_em_type 0is52 1is 43
void dequantize_fp8_to_bf16(const uint8_t *x, const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, const uint16_t *coeff, uint16_t *z) //fp8_type 0 nv, 1 gr . fp8_em_type 0is52 1is 43
{
    //fp8*bf16 = bf16
    uint8_t fp8_type = *fp8_ty;
    uint8_t fp8_em_type = *fp8_em_ty;
    uint8_t src = *x;
    uint16_t coefficien = *coeff;
    bfloat16 coefficient;
    //coefficient.value = coefficien;
   // printf("coefficient = %x\n",coefficient.value);
  
        self_float factor1;
        if(coefficien == 0x8000)
         {
            coefficien = 0x0000;
         }
        factor1.value = coefficien << 16;
        self_float factor_fp32;
        factor_fp32.value = factor1.value;
         if(factor_fp32.fp32<0)
        {
            factor_fp32.fp32 = - factor_fp32.fp32;
        }
        coefficient.value = factor_fp32.value >> 16;

    bfloat16 temp;
    bfloat24 mid_out;
    uint16_t _sign = (src >> 7) & 1;
   // //printf("_sign = %x\n",_sign);
    int mC1 = fp8_em_type?3:2;
    uint16_t _mantissa = fp8_em_type?(src & 0x07):(src & 0x03);
    //int mC1 = fp8_c_em_type?2:3;
   ////printf("_mantissa = %x\n",_mantissa);
    uint16_t _exponent = fp8_em_type?((src >> mC1) & 0x0f):((src >> mC1) & 0x1f);
   //------bf16
    uint16_t value = coefficient.value;
    value = value >> 7;
    //uint8_t exp_coefficient = value & 0xff;
    //uint16_t valu = coefficient.value;
    //uint8_t mantissa_coefficient = valu & 0x7f;
     if(GetExponent_vpu(float(coefficient))==0 && GetSignificand_vpu(float(coefficient))!=0 )//factor=subnormal
            {            
                coefficient.value = 0;
              //printf("coefficient = %x\n",coefficient.value);
            }

   // //printf("coefficient = %x\n",coefficient.value);
   //-------------------------------- 
    if(fp8_type==0) //nv
    {
        if(GetExponent_vpu(float(coefficient))==0xff && GetSignificand_vpu(float(coefficient)) !=0 )//nan
        {
            temp.value = BF16_NAN_VALUE;
           //  //printf("coefficient000000 = %x\n",temp.value);
            
        }
         else if((_exponent == 0xf && _mantissa== 0x7 && fp8_em_type==1) ||(_exponent == 0x1f && _mantissa != 0 && fp8_em_type==0))
        {
            temp.value = BF16_NAN_VALUE;
           // //printf("coefficient1111111 = %x\n",temp.value);
        }
        else if(_sign==0  && _exponent==0)//|| _mantissa==0.  subnormal / +0
        {
            if(GetExponent_vpu(float(coefficient))==0xff && GetSignificand_vpu(float(coefficient))==0 )//factor=inf
            {
                temp.value = BF16_NAN_VALUE;
                 ////printf("coefficient = %x\n",temp.value);
            }
            // else{
            // temp.value = BF16_POS_ZERO;
            // }
        }
        else if(coefficient.value ==0x0000 && ((src == 0x7c || src == 0xfc) && fp8_em_type ==0) )//0*inf //52
        {
            temp.value = BF16_NAN_VALUE;
        }
        else if(_sign==1  && _exponent==0)//&& _mantissa==0 subnormal / -0
        {
             
            if(GetExponent_vpu(float(coefficient))==0xff && GetSignificand_vpu(float(coefficient))==0 )//factor=inf
            {
                temp.value = BF16_NAN_VALUE;
            }
            // else{
            // temp.value = BF16_NEG_ZERO;
            // }
        }
        else if(_exponent == 0x1f && _mantissa ==0 && fp8_em_type ==0)//52
        {
            if(_sign==0)
                {
                temp.value = BF16_POS_INF;
                //+
                }
            else{
                temp.value = BF16_NEG_INF;//-
                }
        }
       
        else if(std:: isinf(float(coefficient)))
        {
            temp.value = (_sign>0) ? BF16_NEG_INF : BF16_POS_INF;
        }

        else{   
            
                // if( _exponent==0){
                   
                // }
                // else{

                // }
                 //-------------------------------- 
                uint16_t exponent_mid_16 = fp8_em_type?  (_exponent + 120):(_exponent + 112);//fp8->bf16  
                // //printf<<_exponent<<endl;
                // //printf<<exponent_mid_16<<endl;
                uint16_t mantissa_mid_16 = fp8_em_type? _mantissa<<4 : _mantissa<<5 ;
                uint16_t data_mid_ = (_sign<<15) | (exponent_mid_16<<7) | (mantissa_mid_16); //(exponent_mid_16<<7)
                
                ////printf<<exponent_mid_16<<endl;
                // //printf<< data_mid_<<endl;
                bfloat16 data_mid;
                data_mid.value = data_mid_;
                //printf("data_mid.value = %x\n",data_mid.value);
                //float z;
               
               // //printf("coefficient.value = %x\n",coefficient.value);
                //self_float facot ;
                //facot.value = coefficient.value;
                 //printf("facor.value = %x, %e\n",facot.value,facot.fp32);
                 mul_bf16(data_mid, coefficient, mid_out);
            
                 //printf("mid_out = %x\n",mid_out.value);
               
                //mid_out.value = 0x7f7f80; 
                if((((mid_out.value >>15) & 0xfe)==0xfe) && ((mid_out.value & 0x7f80)==0x7f80))//rounding 01111110.11111111
                {
                    temp.value= _sign<<15 | 0x7f80;
                   // //printf("mid_out1213 = %x\n",mid_out.value);
                }            
                // else if((((mid_out.value >>15) & 0xff)==0xff) && ((mid_out.value & 0x7fff)==0)) //inf.    exp= 1 .mant= 0
                // {//if(mid_out.value >>15 - 0xff >= 0)            
                //     temp.value = _sign<<15 | 0x7f7f;//inf-1
                    
                //     temp.value = _sign<<15 | 0x7f80;//inf
                //    //  //printf(" _sign = %x\n", _sign<<15);
                //   // //printf(" temp.value = %x\n", temp.value);
                // }
                //change max to inf
                else if((((mid_out.value >>15) & 0xff)==0xff) && ((mid_out.value & 0x7fff)==0)) //inf. 
                {
                    temp.value = _sign<<15 | 0x7f80;//inf
                }
                else{
                   
                    bf24_to_bf16(mid_out,temp);//   
                    if(isinf(float(temp)))
                    {
                        temp.value = _sign<<15 | 0x7f80;//inf
                    }
                }          
            }
    }
    else //gr
    {
        if(GetExponent_vpu(float(coefficient))==0xff && GetSignificand_vpu(float(coefficient)) !=0 )//factor = nan
        {
            temp.value = BF16_NAN_VALUE;
        }
       else if(_sign==0  && _exponent==0 && _mantissa==0)
        {
            if(GetExponent_vpu(float(coefficient))==0xff && GetSignificand_vpu(float(coefficient))==0 )//factor=inf
            {
                temp.value = BF16_NAN_VALUE;
            }
            // else{
            // temp.value = BF16_POS_ZERO;
            // }//factor=inf
            
        }
        else if(_sign==1  && _exponent==0 && _mantissa==0)
        {
            temp.value = BF16_NAN_VALUE;                     ////// nan/inf
        }
        else if( _exponent==0 && _mantissa !=0)
        {
            //subnormal
            if(_sign==0)
            {
                if(GetExponent_vpu(float(coefficient))==0xff && GetSignificand_vpu(float(coefficient))==0 )//factor=inf
                {
                    temp.value = BF16_NAN_VALUE;
                }
                // else{
                // temp.value = BF16_POS_ZERO;
                // }//factor=inf
            }
            else{
                 if(GetExponent_vpu(float(coefficient))==0xff && GetSignificand_vpu(float(coefficient))==0 )//factor=inf
                {
                    temp.value = BF16_NAN_VALUE;
                }
                // else{
                // temp.value = BF16_NEG_ZERO;//and factor is positive
                // }//factor=inf
            }
        }
        else if(std:: isinf(float(coefficient)))
        {
            temp.value = (_sign==0) ? BF16_POS_INF : BF16_NEG_INF;
        }
        else{
               //-------------------------------- 
            uint16_t exponent_mid_16 = fp8_em_type?  ((int)_exponent + 120) : ((int)_exponent + 112);//fp8->bf16 
            // //printf<<_exponent<<endl;
            // //printf<<exponent_mid_16<<endl;
            uint16_t mantissa_mid_16 = fp8_em_type?    _mantissa<<4 : _mantissa<<5 ;
           // ////printf("exponent_mid_16 = %x\n",exponent_mid_16);
           // ////printf("mantissa_mid_16 = %x\n",mantissa_mid_16);
            uint16_t data_mid = _sign<<15 | exponent_mid_16<<7 | mantissa_mid_16; 
            bfloat16 mid_ ;
            // //printf<<data_mid<<endl;
            mid_.value = data_mid;
           // //printf(" mid_.value = %x\n", mid_.value);
            mul_bf16(mid_, coefficient, mid_out);//fp24
           // //printf("mid_out = %x\n",mid_out.value);
           
            if((((mid_out.value >>15) & 0xfe)==0xfe) && ((mid_out.value & 0x7f80)==0x7f80))//rounding 01111110.11111111
            {
                temp.value= _sign<<15 | 0x7f80;//inf
            }     
            // else if((((mid_out.value>>15) & 0xff) == 0xff) && ((mid_out.value & 0x7fff)==0)) 
            // {  
            //         temp.value = _sign<<15 | 0x7f7f;
            // }
            //change max to inf
           else if((((mid_out.value>>15) & 0xff) == 0xff) && ((mid_out.value & 0x7fff)==0))
                {
                    temp.value = _sign<<15 | 0x7f80;//inf
                }
            else{
                bf24_to_bf16(mid_out,temp);//    
                if(isinf(float(temp)))
                {
                    temp.value = _sign<<15 | 0x7f80;//inf
                }
             }    
        }
    }
    //return temp.value; 
    *(uint16_t*)z = temp.value;
}


 

// uint8_t quantize_bf16_to_fp8(uint16_t src, int fp8_type, int fp8_em_type, bfloat16 coefficient) //fp8_type 0 nv, 1 gr. fp8_em_type 0 is 52 ,1 is 43.   43  52
// void quantize_tf32_to_fp8(const uint8_t *x , const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, const uint8_t *coeff, uint8_t *z) //fp8_type 0 nv, 1 gr. fp8_em_type 0 is 52 ,1 is 43.   43  52
// {   //bf16*bf16 = fp8
// //tf32*tf32
//     fp8_format nv_or_gr;
//     nv_or_gr=fp8_ty?gr:nv;

//     uint8_t fp8_type = *fp8_ty;
//     uint8_t fp8_em_type = *fp8_em_ty;
   
//    self_float x_in;
//    self_float factor_in;
//    uint32_t x_tp = *(uint32_t *)x;
//    //printf(" x_tp = %x\n",x_tp);
//    uint32_t factor_tp = *(uint32_t *)coeff;
//    x_in.value = x_tp;
//    factor_in.value = factor_tp;
   
//     // uint16_t src;
//     // src = *x;
//     // bfloat24 mid_out;
//     // bfloat16 res;
//     // res.value = src;
//     //  uint16_t coefficien = *coeff;
//     // bfloat16 coefficient;
//    // coefficient.value = coefficien;
// ////printf("coefficient =  %x\n", coefficient.value);
//        // self_float factor1;
//         if(factor_in.value == 0x80000000)
//          {
//             factor_in.value = 0x00000000;
//          }
//         //factor1.value = coefficien << 16;
//         //self_float factor_fp32;
//         //factor_fp32.value = factor1.value;
//         if(factor_in.fp32<0)
//         {
//             factor_in.fp32 = - factor_in.fp32;
//         }
//        // coefficient.value = factor_in.value >> 16;

//     uint8_t temp;

//     int eQ = fp8_em_type?4:5;
//     int mQ = 7-eQ;
//     //uint8_t sign = (src>>8) & 0x80;
//     uint8_t sign = *(uint32_t *)x >> 24 & 0x80;
//     //printf("sign1 =  %x\n", sign);
//     if(is_small_number(factor_in.fp32)){
       
//         factor_in.value =(factor_in.fp32>0)?0x0:0x80000000;   
//     }
//     //printf(" GetSignificand_vpu(float(res))!=0 =  %x\n",  GetSignificand_vpu(float(res)));    
   
//    // res = (bfloat16)subnormal((float)res);     // v1.6, input x subnormal
//    // coefficient = (bfloat16)subnormal((float)coefficient); // v1.6, input f subnormal
  
//     if(fp8_type==0)//nv
//     { 
//        // if(fp8_em_type)//52
//         //{
//         if(isnan(x_in.fp32) || isnan(factor_in.fp32)){
//           temp = 0x7f;
       
//          }

//         else{
      
//                 if(is_small_number(x_in.fp32)){
                
//                     x_in.value =(x_in.fp32>0)?0x0:0x80000000;   
//                 }
//                 // 0*inf = nan 
//                 if(isinf(x_in.fp32))
//                 {
//                     if(factor_in.value == 0x0 || factor_in.value == 0x80000000){
//                         z[0] = 0x7f;
//                         return;
//                     }
//                 }
//                 else if(isinf(factor_in.fp32))
//                 {
//                     if(x_in.value == 0x0 || x_in.value == 0x80000000)
//                     {
//                         z[0] = 0x7f;
//                         return;
//                     }
//                 }
//                 else if(( std::isinf(factor_in.fp32) || std::isinf(x_in.fp32)) && fp8_em_type==0 ){
                    
//                     z[0] = (x_in.fp32<0) ? 0xfc : 0x7c;//  inf*not zero = inf
//                     return;
//                 }
//                 else if(( std::isinf(factor_in.fp32) || std::isinf(x_in.fp32)) && fp8_em_type==1 ){
//                     z[0] = sign | 0x7e;// max
//                     return;
//                 }
                
//                 // self_float lhs, rhs, res1;
//                 // lhs.value = res.value << 16;
//                 // rhs.value = coefficient.value << 16;//*(uint32_t*)
//                 // res1.fp32 = subnormal(subnormal(lhs.fp32) * subnormal(rhs.fp32));
//                  self_float  res1;
//                 res1.fp32 = subnormal(subnormal(x_in.fp32) * subnormal(factor_in.fp32));
//                 // //printf("res1.fp32 =  %x\n", res1.value);
//                if(std::isinf(res1.fp32))
//                {
//                   if(fp8_em_type)//43
//                   {
//                     z[0] =  sign | 0x7e;
//                   }
//                   else{//52
//                     z[0] =  sign | 0x7c;
//                   }
//                   return;
//                }     
                
//                 //fp32 rounding to tp32(19)
//                 uint32_t lsb = (res1.value >> 13) & 1;
//                 uint32_t rounding_bias = 0xfff + lsb;
//                 res1.value += rounding_bias;
//                 res1.value &= 0xffffe000;
//                 //printf("res1.value = %x, res1.fp32 = %f\n",res1.value, res1.fp32);

//                 // tf32 rounding to bf16
//                 // bfloat16 x0;
//                 // uint32_t lsb1 = (res1.value >> 16) & 1;
//                 // uint32_t rounding_bias1 = 0x7fff + lsb1;
//                 // res1.value += rounding_bias1;
//                 // x0.value = static_cast<uint16_t>(res1.value >> 16);

//                 // //fp32 rounding to fp24
//                 // uint32_t lsb = (res1.value >> 8) & 1;
//                 // uint32_t rounding_bias = 0x7f + lsb;
//                 // res1.value += rounding_bias;
//                 // res1.value &= 0xffffff00;
            
//                 // bfloat16 x0;
//                 // //fp24 rounding to bf16
//                 // uint32_t lsb1 = (res1.value >> 16) & 1;
//                 // uint32_t rounding_bias1 = 0x7fff + lsb1;
//                 // res1.value += rounding_bias1;
//                 // x0.value = static_cast<uint16_t>(res1.value >> 16);

//               // //printf("sign =  %x\n", sign); 
//                 uint8_t _exp1  = 0xff & (res1.value >> 23) ; //8 mid的exp
//                 uint8_t exp_f  = fp8_em_type ?  _exp1 - 120 : _exp1 - 112;
            
//                 uint32_t mantiss_q = res1.value & 0x7fffff;//15
//               //  //printf("mantiss_q =  %x\n", mantiss_q); 
//                 uint32_t runding_coefficient1= mantiss_q>> 15-mQ & 1 ;
//                 uint32_t mantiss_q_ = mantiss_q;
//                 //-------------------------------------------------------------------------------
           
//                 if(!fp8_em_type)//52  
//                 {
                    
//                    //52
//                     // if((int)_exp1>143)//127+16
//                     // {
//                     //     temp = sign | 0x7b;//fp8_pos_max_normal_nv_52;                       
//                     // }
//                     //change  max to inf
//                     if(std::isinf(res1.fp32))
//                      {
//                         temp = sign | 0x7c;
//                      }
//                     if((int)_exp1>143)//127+16
//                     {
//                         temp = sign | 0x7c;//fp8_pos_inf_nv_52;                       
//                     }
//                     else if((int)_exp1 < 112)//127-14  𝑆.00001.00=1∙2^(−14)=2^(−14)  (𝑏=15) min_normal  subnormal
//                     {                 

//                         temp = sign;//+-0
                    
//                     }    
//                     else{
//                        // //printf(" x.value =  %x\n",  x0.value);
//                         self_float out1;
//                         out1.value = res1.value;
//                        //  //printf(" out1.value  =  %x\n",  out1.value);
//                          _float_to_fp8_152((uint8_t *)&out1.fp32,nv,15,&temp);
               
//                     }
//                 }    
//                 else{//43                
//                     if(isinf(float(x0)))
//                     {
//                         temp =sign | 0x7e;//fp8_max_normal_nv_43; 
//                     }
//                     else if( _exp1 > 135) //127+8 
//                     {//
                        
//                         temp = sign | 0x7e;//fp8_max_normal_nv_43;  
//                         //temp = sign | 0x7f;//fp8_inf_normal_nv_43; 
                        
//                     } 
//                     else if((int)_exp1 < 120) //127-6//
//                     {                       
//                          temp = sign;                                                       
//                     }
                       
//                     else{
//                         self_float out1;
//                         out1.value = x0.value << 16;
//                        //  //printf(" out1.value  =  %x\n",  out1.value);
//                          _float_to_fp8_143((uint8_t *)&out1.fp32,nv,7,&temp);
                 
//                     }
//                 }
//             }          
//     }                
//     else{//gr
        
//            //if(std::isinf(float(res)) || std::isnan(float(res)))// res.value ==BF16_NAN_VALUE || res.value == BF16_POS_INF|| res.value == BF16_NEG_INF)
//             if(isinf(x_in.fp32) || isnan(x_in.fp32))
//             {
//                     z[0] = 0x80 ;//TODO 
//                     //printf("temp_nan =  %x\n", z[0]);
//                     return; //fp8_amd_em52_nan_inf;//1.0000.000
//             }
//             else if(std::isnan(factor_in.fp32))
//             {
//                 z[0] =0x80;
//                 return; 
//             }     
//             else{
//                 //printf(" GetSignificand_vpu(float(res))!=0 =  %x\n",  GetSignificand_vpu(float(res)));
//                   if(is_small_number(x_in.fp32)){
//                     x_in.value =(x_in.fp32>0)?0x0:0x80000000;   
//                 }
      
//                 // zero*inf=nan. factor is positive, 
//                 if(isinf(x_in.fp32))
//                 {
//                   if(factor_in.value == 0x0 || factor_in.value == 0x80000000){
//                      z[0] = 0x80;
//                      return;
//                     }
//                 }
//                 else if(isinf(factor_in.fp32))
//                 {
//                     if(x_in.value == 0x0 || x_in.value == 0x80000000)
//                     {
//                         z[0] = 0x80;
//                         return;
//                     }
//                 }
//                 else if( std::isinf(factor_in.fp32) || std::isinf(x_in.fp32)){
                    
//                     z[0] = 0x80;//  inf*not zero = inf
//                     return;
//                 }
           
//                 self_float res1;
//                 res1.fp32 =  subnormal(subnormal(x_in.fp32) * subnormal(factor_in.fp32));
//                // uint8_t sign = (src>>8) & 0x80;
                
//                  if(std::isinf(res1.fp32))
//                 {
//                     z[0] =  0x80;    
//                     return;
//                 }
                
//                 //fp32 rounding to tp32(19)
//                 uint32_t lsb = (res1.value >> 13) & 1;
//                 uint32_t rounding_bias = 0xfff + lsb;
//                 res1.value += rounding_bias;
//                 res1.value &= 0xffffe000;
//                 //printf("res1.value = %x, res1.fp32 = %f\n",res1.value, res1.fp32);

//                 //tf32 rounding to bf16
//                 // bfloat16 x0;
//                 // uint32_t lsb1 = (res1.value >> 16) & 1;
//                 // uint32_t rounding_bias1 = 0x7fff + lsb1;
//                 // res1.value += rounding_bias1;
//                 // x0.value = static_cast<uint16_t>(res1.value >> 16);
//                 //printf("x0 bf16 = %x\n",x0.value);

//                 uint8_t _exp1  = 0xff & (res1.value >> 23) ; //8 mid的exp
                
//                 uint8_t exp_f  = fp8_em_type ?  _exp1 - 0x78 : _exp1 - 0x70 ;
//                // //printf<<(int)exp_f<<" fp8 "<<endl;
//                 uint32_t mantiss_q = res1.value & 0x7fffff;//15
//                // //printf("mantiss_q =  %x\n", mantiss_q); 
//                 uint32_t runding_coefficient1= mantiss_q>> 15-mQ & 1 ;
//                 uint32_t mantiss_q_ = mantiss_q;
                         
//                 if(!fp8_em_type)//52
//                 {
//                     //52
                        
//                         // if((int)_exp1>143)//127+16
//                         // {
//                         // temp = sign | 0x7f; //fp8_pos_max_normal_amd_52;      
//                         // //printf("temp1121 =  %x\n", temp);            
//                         // }
//                         //cahnge to inf
//                         if(std::isinf(float(x0)))
//                         {
//                             temp = 0x80;
//                         }
//                         else if((int)_exp1>143)//127+16
//                         {
//                             temp = 0x80; //fp8_inf__amd_52;      
//                             //printf("temp1121 =  %x\n", temp);            
//                         }
//                         else if((int)_exp1 < 112)//127-14  𝑆.00001.00=1∙2^(−14)=2^(−14)  (𝑏=15) min_normal  subnormal
//                         {                                           
//                             temp = 0;//0                                                            
//                         }  
                   
                   
//                     else{
                        
//                         self_float out1;
//                         out1.value = x0.value << 16;

//                          _float_to_fp8_152((uint8_t *)&out1.value,gr,15,&temp);
                       
//                     }
//                 }
//                 else {//43
                   
//                     // if( _exp1 > 135) 
//                     // {
//                     //     temp = sign| 0x7f;//max
//                     // }
//                     //cahnge to inf
//                     if(std::isinf(float(x0)))
//                     {
//                         temp = 0x80;
//                     }
//                     else if((int)_exp1>143)//127+16
//                     {
//                         temp = 0x80;//fp8_pos_inf_normal_amd_52;      
//                         //printf("temp1121 =  %x\n", temp);            
//                     }
//                     else if(_exp1 < 120) //127-6//
//                     {               
//                         temp = 0;                            //}
//                     }        
                    
//                     else{
                        
//                        // //printf(" x.value =  %x\n",  x0.value);
//                         self_float out1;
//                         out1.value = x0.value << 16;
//                         // //printf(" out1.value  =  %x\n",  out1.value);
//                          _float_to_fp8_143((uint8_t *)&out1.fp32,gr,7,&temp);
                       
//                     }
//                 }  
            
//             }
//         }           

//    z[0] = temp;
// }


// uint8_t quantize_bf16_to_fp8(uint16_t src, int fp8_type, int fp8_em_type, bfloat16 coefficient) //fp8_type 0 nv, 1 gr. fp8_em_type 0 is 52 ,1 is 43.   43  52
void quantize_bf16_to_fp8(const uint16_t *x , const uint8_t *fp8_ty, const uint8_t *fp8_em_ty, const uint16_t *coeff, uint8_t *z) //fp8_type 0 nv, 1 gr. fp8_em_type 0 is 52 ,1 is 43.   43  52
{   //bf16*bf16 = fp8
    //fp8_format nv_or_gr;
    //nv_or_gr=fp8_ty?gr:nv;

    uint8_t fp8_type = *fp8_ty;
    uint8_t fp8_em_type = *fp8_em_ty;
    uint16_t src;
    src = *x;
    bfloat24 mid_out;
    bfloat16 res;
    res.value = src;
     uint16_t coefficien = *coeff;
    bfloat16 coefficient;
   // coefficient.value = coefficien;
////printf("coefficient =  %x\n", coefficient.value);
        self_float factor1;
        if(coefficien == 0x8000)
         {
            coefficien = 0x0000;
         }
        factor1.value = coefficien << 16;
        self_float factor_fp32;
        factor_fp32.value = factor1.value;
         if(factor_fp32.fp32<0)
        {
            factor_fp32.fp32 = - factor_fp32.fp32;
        }
        coefficient.value = factor_fp32.value >> 16;

    uint8_t temp;

    //int eQ = fp8_em_type?4:5;
    //int mQ = 7-eQ;
    uint8_t sign = (src>>8) & 0x80;
    //printf("sign1 =  %x\n", sign);
    if(GetExponent_vpu(float(coefficient))==0 && GetSignificand_vpu(float(coefficient))!=0 )//factor=subnormal
        {           
            coefficient.value = 0;
            ////printf("coefficient = %x\n",coefficient.value);
        }
    //printf(" GetSignificand_vpu(float(res))!=0 =  %x\n",  GetSignificand_vpu(float(res)));    
   
   
    res = (bfloat16)subnormal((float)res);     // v1.6, input x subnormal
    coefficient = (bfloat16)subnormal((float)coefficient); // v1.6, input f subnormal
   
   
    //printf(" GetSignificand_vpu(float(res))!=0 =  %x\n",  GetSignificand_vpu(float(res)));
    //printf("GetExponent_vpu(float(res)) = %x\n",GetExponent_vpu(float(res)));
    if(fp8_type==0)//nv
    { 
       // if(fp8_em_type)//52
        //{
            if(std::isnan(float(res)))//nan
            {
                temp = 0x7f; //fp8_nv_em52_nan;//
              // temp = sign | 0x7f;
              //  //printf<<"nan_nv"<<endl;
            }
            else if(std::isnan(float(coefficient)))
            {
                  // temp = sign | 0x7f;
                   temp = 0x7f;
            }
           
            else{
                
                if(GetExponent_vpu(float(res))==0 && GetSignificand_vpu(float(res))!=0 ) //subnormal
                    {
                        if(sign)
                        {
                            res.value = BF16_NEG_ZERO;
                           // //printf("res.value =  %x\n", res.value);
                        }
                        else {
                            res.value = BF16_POS_ZERO;
                           // //printf("res.value =  %x\n", res.value);
                        }
                        
                    }
                    // 0*inf = nan 
                if((res.value == BF16_NEG_ZERO || res.value == BF16_POS_ZERO) && std::isinf(float(coefficient)))//(GetExponent_vpu(float(coefficient))==0xff && GetSignificand_vpu(float(coefficient))==0) )
                {   
                    //uint8_t sign_factor = (coefficien>>8) & 0x80;

                    //z[0] = (sign==sign_factor) ? (0x00 |0x7f ): (0x80|0x7f) ;//TODO 
                    // if(fp8_em_type==0)
                    // {
                       z[0] = 0x7f; //nv52
                    // }  
                    // z[0] = sign | 0x7f;//nan
                  //  //printf("temp_nan =  %x\n", z[0]);
                    return;
                }
                
                //else if((res.value == BF16_NEG_INF || res.value == BF16_POS_INF) && (coefficient.value == 0x0000))
               else if((std::isinf(float(res)) && (coefficient.value == 0x0000)))
                {
                    //    if(fp8_em_type==0)
                    //     {
                         z[0] = 0x7f; //nv52
                    //     }  
                     //z[0] = sign | 0x7f;//nan
                   // //printf("temp_nan =  %x\n", z[0]);
                    return;
                }
                else if(( std::isinf(float(res)) || std::isinf(float(coefficient))) && fp8_em_type==0 ){
                    
                    z[0] = sign ? 0xfc : 0x7c;//  inf*not zero = inf
                    return;
                }
                else if(( std::isinf(float(res)) || std::isinf(float(coefficient))) && fp8_em_type==1 ){
                   // z[0] =  0x7f;
                   //  z[0] = sign | 0x7f;// inf->nan
                    z[0] = sign | 0x7e;// max
                    return;
                }


                mul_bf16(res, coefficient, mid_out);//fp24
                
               // //printf("mid_out =  %x\n", mid_out.value);
                self_float lhs, rhs, res1;
                lhs.value = res.value << 16;
                rhs.value = coefficient.value << 16;//*(uint32_t*)
                res1.fp32 = subnormal(subnormal(lhs.fp32) * subnormal(rhs.fp32));
                // //printf("res1.fp32 =  %x\n", res1.value);
               if(std::isinf(res1.fp32))
               {
                  if(fp8_em_type)//43
                  {
                    z[0] =  sign | 0x7e;
                  }
                  else{//52
                    z[0] =  sign | 0x7c;
                  }
                  return;
               }     

                //fp32 rounding to fp24
                uint32_t lsb = (res1.value >> 8) & 1;
                uint32_t rounding_bias = 0x7f + lsb;
                res1.value += rounding_bias;
                res1.value &= 0xffffff00;
            
                bfloat16 x0;
                //fp24 rounding to bf16
                uint32_t lsb1 = (res1.value >> 16) & 1;
                uint32_t rounding_bias1 = 0x7fff + lsb1;
                res1.value += rounding_bias1;
                x0.value = static_cast<uint16_t>(res1.value >> 16);

              // //printf("sign =  %x\n", sign); 
                 uint8_t _exp1  = 0xff & (res1.value >> 23) ; //8 mid的exp
                  //uint8_t exp_f  = fp8_em_type ?  _exp1 - 120 : _exp1 - 112;
            
                //uint32_t mantiss_q = res1.value & 0x7fffff;//15
              //  //printf("mantiss_q =  %x\n", mantiss_q); 
                //uint32_t runding_coefficient1= mantiss_q>> 15-mQ & 1 ;
                //uint32_t mantiss_q_ = mantiss_q;
                //-------------------------------------------------------------------------------
           
                if(!fp8_em_type)//52  
                {
                    
                   //52
                    // if((int)_exp1>143)//127+16
                    // {
                    //     temp = sign | 0x7b;//fp8_pos_max_normal_nv_52;                       
                    // }
                    //change  max to inf
                    if(std::isinf(float(x0)))
                     {
                        temp = sign | 0x7c;
                     }
                    if((int)_exp1>143)//127+16
                    {
                        temp = sign | 0x7c;//fp8_pos_inf_nv_52;                       
                    }
                    else if((int)_exp1 < 112)//127-14  𝑆.00001.00=1∙2^(−14)=2^(−14)  (𝑏=15) min_normal  subnormal
                    {                 

                                temp = sign;//+-0
                    
                    }    

                   
                    else{
                        //uint32_t t = mid_out.value <<8;
                       // //printf("mid_out.value =  %x\n", mid_out.value);

                        
                       // //printf(" x.value =  %x\n",  x0.value);
                        self_float out1;
                        out1.value = x0.value << 16;
                       //  //printf(" out1.value  =  %x\n",  out1.value);

                         _float_to_fp8_152((uint8_t *)&out1.fp32,nv,15,&temp);
               
                    }
                }    
                else{//43                
                    if(isinf(float(x0)))
                    {
                        temp =sign | 0x7e;//fp8_max_normal_nv_43; 
                    }
                    else if( _exp1 > 135) //127+8 
                    {//
                        
                        temp = sign | 0x7e;//fp8_max_normal_nv_43;  
                        //temp = sign | 0x7f;//fp8_inf_normal_nv_43; 
                        
                    } 
                    else if((int)_exp1 < 120) //127-6//
                    {                       
                         temp = sign;                                                       
                    }
                       
                    else{
                          //uint32_t t = mid_out.value <<8;                       
                      //  //printf(" x.value =  %x\n",  x0.value);
                        self_float out1;
                        out1.value = x0.value << 16;
                       //  //printf(" out1.value  =  %x\n",  out1.value);

                         _float_to_fp8_143((uint8_t *)&out1.fp32,nv,7,&temp);
                 
                    }
                }
            }          
    }                
    else{//gr
        
           if(std::isinf(float(res)) || std::isnan(float(res)))// res.value ==BF16_NAN_VALUE || res.value == BF16_POS_INF|| res.value == BF16_NEG_INF)
            {
                    z[0] = 0x80 ;//TODO 
                    //printf("temp_nan =  %x\n", z[0]);
                    return; //fp8_amd_em52_nan_inf;//1.0000.000
            }
            else if(std::isnan(float(coefficient)))
            {
                z[0] =0x80;
                return; 
            }
           
            else{
                //printf(" GetSignificand_vpu(float(res))!=0 =  %x\n",  GetSignificand_vpu(float(res)));
                if(GetExponent_vpu(float(res))==0 && GetSignificand_vpu(float(res))!=0 ) //subnormal
                    {
                        if(sign== 0x80)
                        {
                            res.value = BF16_NEG_ZERO;
                            //printf("res.value =  %x\n", res.value);
                        }
                        else {
                            res.value = BF16_POS_ZERO;
                            //printf("res.value =  %x\n", res.value);
                        }
                        
                    }
                // zero*inf=nan. factor is positive, 
                if((res.value == BF16_NEG_ZERO || res.value == BF16_POS_ZERO) && (std::isinf(float(coefficient)) ))
                {                       

                    z[0] = 0x80 ;//TODO 
                    ////printf("temp_nan =  %x\n", z[0]);
                    return;
                }
                  else if((std::isinf(float(res))) && (coefficient.value ==0x0000))
                {
                    z[0] = 0x80;
                   // //printf("temp_nan =  %x\n", z[0]);
                    return;
                }
                else if( std::isinf(float(res)) || std::isinf(float(coefficient)) ){
                    
                    z[0] = 0x80;//  inf*not zero = inf
                    return;
                }
                


                
                 mul_bf16(res, coefficient, mid_out);//fp24
                 
                self_float lhs, rhs, res1;
                lhs.value = res.value << 16;
                rhs.value = coefficient.value << 16;//*(uint32_t*)
                res1.fp32 = subnormal(subnormal(lhs.fp32) * subnormal(rhs.fp32));
                        
                //uint8_t sign = (src>>8) & 0x80;
                
                 if(std::isinf(res1.fp32))
                {
                    z[0] =  0x80;    
                    return;
                }

                //fp32 rounding to fp24
                uint32_t lsb = (res1.value >> 8) & 1;
                uint32_t rounding_bias = 0x7f + lsb;
                res1.value += rounding_bias;
                res1.value &= 0xffffff00;
               
                bfloat16 x0;
                //fp24 to bf16
                uint32_t lsb1 = (res1.value >> 16) & 1;//fp32->bf16
                uint32_t rounding_bias1 = 0x7fff + lsb1;
                res1.value += rounding_bias1;
                x0.value = res1.value >> 16;

                uint8_t _exp1  = 0xff & (res1.value >> 23) ; //8 mid的exp
                
                //uint8_t exp_f  = fp8_em_type ?  _exp1 - 0x78 : _exp1 - 0x70 ;
               // //printf<<(int)exp_f<<" fp8 "<<endl;
                //uint32_t mantiss_q = res1.value & 0x7fffff;//15
               // //printf("mantiss_q =  %x\n", mantiss_q); 
                //uint32_t runding_coefficient1= mantiss_q>> 15-mQ & 1 ;
                //uint32_t mantiss_q_ = mantiss_q;
                         
                if(!fp8_em_type)//52
                {
                    //52
                        
                        // if((int)_exp1>143)//127+16
                        // {
                        // temp = sign | 0x7f; //fp8_pos_max_normal_amd_52;      
                        // //printf("temp1121 =  %x\n", temp);            
                        // }
                        //cahnge to inf
                        if(std::isinf(float(x0)))
                        {
                            temp = 0x80;
                        }
                        else if((int)_exp1>143)//127+16
                        {
                            temp = 0x80; //fp8_inf__amd_52;      
                            //printf("temp1121 =  %x\n", temp);            
                        }
                        else if((int)_exp1 < 112)//127-14  𝑆.00001.00=1∙2^(−14)=2^(−14)  (𝑏=15) min_normal  subnormal
                        {                                           
                            temp = 0;//0                                                            
                        }  
                   
                   
                    else{
                        
                        self_float out1;
                        out1.value = x0.value << 16;

                         _float_to_fp8_152((uint8_t *)&out1.value,gr,15,&temp);
                       
                    }
                }
                else {//43
                   
                    // if( _exp1 > 135) 
                    // {
                    //     temp = sign| 0x7f;//max
                    // }
                    //cahnge to inf
                    if(std::isinf(float(x0)))
                    {
                        temp = 0x80;
                    }
                    else if((int)_exp1>143)//127+16
                    {
                        temp = 0x80;//fp8_pos_inf_normal_amd_52;      
                        //printf("temp1121 =  %x\n", temp);            
                    }
                    else if(_exp1 < 120) //127-6//
                    {               
                        temp = 0;                            //}
                    }        
                    
                    else{
                         //uint32_t t = mid_out.value <<8;
                        // bfloat16 x0;
                       // x0 = round_to_bfloat16(res1.fp32); 
                        //uint32_t lsb1 = (res1.value >> 16) & 1;
                       // uint32_t rounding_bias1 = 0x7fff + lsb1;
                       // res1.value += rounding_bias1;
                        //x0.value = static_cast<uint16_t>(res1.value >> 16);

                       // //printf(" x.value =  %x\n",  x0.value);
                        self_float out1;
                        out1.value = x0.value << 16;
                        // //printf(" out1.value  =  %x\n",  out1.value);
                         _float_to_fp8_143((uint8_t *)&out1.fp32,gr,7,&temp);
                       
                    }
                }  
            
            }
        }           

   z[0] = temp;
}


