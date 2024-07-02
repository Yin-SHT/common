//Created by Yucheng on 2023/4/10

#ifndef MOFFETT_FP16_H
#define MOFFETT_FP16_H

#include <complex>
#include <cmath>

#define FP16_DEVICE_FUNC
namespace numeric {

struct fp16 {
    // The default constructor must yield a zero value, not an uninitialized
    // value; some TF kernels use T() as a zero value.
    FP16_DEVICE_FUNC fp16() : value(ZERO_VALUE) {}

    //fp32-to-fp16
    FP16_DEVICE_FUNC explicit fp16(const float v) {
        value = round_to_fp16(v).value;
    }

    template <class T>
    FP16_DEVICE_FUNC explicit fp16(const T& val)
            : fp16(static_cast<float>(val)) {}

    //fp16-to-fp32
    FP16_DEVICE_FUNC explicit operator float() const {
        float result = 0;
        result = fp16_to_float(value);

    //    uint16_t* q = reinterpret_cast<uint16_t*>(&result);
    //#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    //    q[0] = value;
    //#else
    //    q[1] = value;
    //#endif
        return result;
    }

    union FP32 {
        unsigned int u;
        float f;
    };
    //==================================
    //fp32<->fp16 conversion. 
    // Converts a float point to fp16, with round-nearest-to-even
    FP16_DEVICE_FUNC static fp16 round_to_fp16(float v) {
        uint32_t input;
        uint32_t exponent;
        uint32_t mantissa;
        fp16 output;
        FP32 f;
        f.f = v;
        input = f.u;
        exponent = (input>>23) & 0xFF; //extract 8-bit exponent
        mantissa = input & 0x7FFFFF;   //extract 23-bit fraction

        if ((exponent==0xFF) && (mantissa!=0x0)) {      //fp32 nan value.
            output.value = 0x7FFF;                      //FP16-NAN=7FFFF. 
        } else if ((exponent==0xFF)&&(mantissa==0x0)) { //fp32 +-inf.
            input = (input&0x80000000) | (0x7C000000);
            output.value = static_cast<uint16_t>(input >> 16);
        } else if( exponent>0x8E ) {                    //overflow
            input = (input&0x80000000) | (0x7C000000);
            output.value = static_cast<uint16_t>(input >> 16);
        //} else if ( ((input>0x387FC000)&&(input<0x38800000)) || ((input>0xB87FC000)&&(input<0xB8800000)) ) {
        } else if ( ((input>=0x387FE000)&&(input<0x38800000)) || ((input>=0xB87FE000)&&(input<0xB8800000)) ) {
            input = (input&0x80000000) | (0x04000000);  //between max_subnormal and min_normal.
            output.value = static_cast<uint16_t>(input >> 16);
        } else if (exponent<=0x70) {                    //+-0 and subnormal
            output.value = static_cast<uint16_t>((input&0x80000000)>>16);
        } else {
            uint32_t lsb = (input>>13) & 1;
            uint32_t rounding_bias = 0x0FFF + lsb;  //23-bit -> 10-bit. 
            input += rounding_bias;                 //input in (1.8.10.13)
            exponent = (input>>23) & 0xFF;          //update 8-bit exponent
            mantissa = input & 0x7FFFFF;            //update 23-bit fraction
            exponent -= 0x70; //fp32.e - 112.
            input = (input&0x80000000) | (exponent<<26) | (mantissa<<3);
            output.value = static_cast<uint16_t>(input>>16);
        }
        return output;
    }  

    FP16_DEVICE_FUNC static float fp16_to_float(uint16_t x) {
        FP32 f; 
        uint16_t sign = ((x >> 15) & 0x1);
        uint16_t exponent = (x >> 10) & 0x1F;
        uint16_t mantissa = (x & 0x3FF);

        if(exponent == 0x0){//zero&subnormal
            f.u = sign << 31; 
        }else if ((exponent == 0x1F) && (mantissa != 0x0)){
            f.u = 0x7FC00000;                   //NAN
        }else if ((exponent == 0x1F) && (mantissa == 0x0)){
            f.u = (0x7F800000 | (sign<<31));    //INF+-
        }else{
            exponent += 0x70;//+127-15. 
            f.u = (sign<<31) | (exponent<<23) | (mantissa<<13); 
        }

        float output = f.f;
        return output;
    }  
    //==================================

    uint16_t value;
    
    bool IsZero() const { return (value & 0x7FFF) == ZERO_VALUE; }
    //static const uint16_t NAN_VALUE = 0x7FC0;// A value that represents "not a number".

private:
    static const uint16_t ZERO_VALUE = 0;// A value that represents "zero".

    FP16_DEVICE_FUNC static bool float_isnan(const float& x) {
    #ifdef __CUDA_ARCH__
        return ::isnan(x);
    #else
        return std::isnan(x);
    #endif
    }
}; // end struct fp16.  
// inline operators.  

}  // end namespace numeric


namespace std {
    template <>
    struct hash<numeric::fp16> {
        size_t operator()(const numeric::fp16& v) const {
            return hash<float>()(static_cast<float>(v));
        }
    };

    using numeric::fp16;
    inline bool isinf(const fp16& a) { return std::isinf(float(a)); }
    inline bool isnan(const fp16& a) { return std::isnan(float(a)); }
    inline bool isfinite(const fp16& a) { return std::isfinite(float(a)); }
    inline fp16 abs(const fp16& a) { return fp16(std::abs(float(a))); }
    inline fp16 exp(const fp16& a) { return fp16(std::exp(float(a))); }
    inline fp16 log(const fp16& a) { return fp16(std::log(float(a))); }
    inline fp16 log10(const fp16& a) {
        return fp16(std::log10(float(a)));
    }
    inline fp16 sqrt(const fp16& a) {
        return fp16(std::sqrt(float(a)));
    }
    inline fp16 pow(const fp16& a, const fp16& b) {
        return fp16(std::pow(float(a), float(b)));
    }
    inline fp16 sin(const fp16& a) { return fp16(std::sin(float(a))); }
    inline fp16 cos(const fp16& a) { return fp16(std::cos(float(a))); }
    inline fp16 tan(const fp16& a) { return fp16(std::tan(float(a))); }
    inline fp16 tanh(const fp16& a) {
        return fp16(std::tanh(float(a)));
    }
    inline fp16 floor(const fp16& a) {
        return fp16(std::floor(float(a)));
    }
    inline fp16 ceil(const fp16& a) {
        return fp16(std::ceil(float(a)));
    }
}  // namespace std

#endif //MOFFETT_FP16_H
