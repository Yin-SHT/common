//Created by Yucheng on 2023/4/10

#ifndef MOFFETT_FP16_H
#define MOFFETT_FP16_H

#include <complex>
#include <cmath>
#include <cstdint>
#include <iostream>

#define FP16_DEVICE_FUNC
namespace numeric {

struct fp16 {
    // The default constructor must yield a zero value, not an uninitialized value.
    FP16_DEVICE_FUNC fp16() : value(ZERO_VALUE) {}

    FP16_DEVICE_FUNC explicit fp16(const float v) {
        value = round_to_fp16(v).value;
    }

    FP16_DEVICE_FUNC explicit fp16(const double val)
        : fp16(static_cast<float>(val)) {}

    FP16_DEVICE_FUNC explicit fp16(const std::complex<float>& val)
        : fp16(val.real()) {}

    FP16_DEVICE_FUNC explicit fp16(const std::complex<double>& val)
        : fp16(static_cast<float>(val.real())) {}

    FP16_DEVICE_FUNC explicit fp16(const unsigned short val)
        : fp16(static_cast<float>(val)) {}

    FP16_DEVICE_FUNC explicit fp16(const unsigned int val)
        : fp16(static_cast<float>(val)) {}

    FP16_DEVICE_FUNC explicit fp16(const int val)
        : fp16(static_cast<float>(val)) {}

    FP16_DEVICE_FUNC explicit fp16(const long val)
        : fp16(static_cast<float>(val)) {}

    FP16_DEVICE_FUNC explicit fp16(const long long val)
        : fp16(static_cast<float>(val)) {}

    template <class T>
    FP16_DEVICE_FUNC explicit fp16(const T& val)
        : fp16(static_cast<float>(val)) {}

    FP16_DEVICE_FUNC explicit operator float() const {
        return fp16_to_float(value);
    }

    FP16_DEVICE_FUNC explicit operator bool() const {
        return static_cast<bool>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator short() const {
        return static_cast<short>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator int() const {
        return static_cast<int>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator long() const {
        return static_cast<long>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator char() const {
        return static_cast<char>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator signed char() const {
        return static_cast<signed char>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator unsigned char() const {
        return static_cast<unsigned char>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator unsigned short() const {
        return static_cast<unsigned short>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator unsigned int() const {
        return static_cast<unsigned int>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator unsigned long() const {
        return static_cast<unsigned long>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator unsigned long long() const {
        return static_cast<unsigned long long>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator long long() const {
        return static_cast<long long>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator double() const {
        return static_cast<double>(float(*this));
    }

    FP16_DEVICE_FUNC explicit operator std::complex<float>() const {
        return std::complex<float>(float(*this), float(0.0));
    }

    FP16_DEVICE_FUNC explicit operator std::complex<double>() const {
        return std::complex<double>(double(*this), double(0.0));
    }

    union FP32 {
        uint32_t u;
        float f;
    };

    // Converts a float to fp16, with round-nearest-to-even as rounding method.
    FP16_DEVICE_FUNC static fp16 round_to_fp16(float v) {
        FP32 f;
        f.f = v;
        uint32_t input = f.u;
        uint32_t exponent = (input >> 23) & 0xFF;
        uint32_t mantissa = input & 0x7FFFFF;
        fp16 output;

        if ((exponent == 0xFF) && (mantissa != 0x0)) {
            output.value = 0x7FFF;
        } else if ((exponent == 0xFF) && (mantissa == 0x0)) {
            input = (input & 0x80000000) | 0x7C000000;
            output.value = static_cast<uint16_t>(input >> 16);
        } else if (exponent > 0x8E) {
            input = (input & 0x80000000) | 0x7C000000;
            output.value = static_cast<uint16_t>(input >> 16);
        } else if (((input >= 0x387FE000) && (input < 0x38800000)) || ((input >= 0xB87FE000) && (input < 0xB8800000))) {
            input = (input & 0x80000000) | 0x04000000;
            output.value = static_cast<uint16_t>(input >> 16);
        } else if (exponent <= 0x70) {
            output.value = static_cast<uint16_t>((input & 0x80000000) >> 16);
        } else {
            uint32_t lsb = (input >> 13) & 1;
            uint32_t rounding_bias = 0x0FFF + lsb;
            input += rounding_bias;
            exponent = (input >> 23) & 0xFF;
            mantissa = input & 0x7FFFFF;
            exponent -= 0x70;
            input = (input & 0x80000000) | (exponent << 13) | (mantissa >> 10);
            output.value = static_cast<uint16_t>(input >> 16);
        }
        return output;
    }

    FP16_DEVICE_FUNC static float fp16_to_float(uint16_t x) {
        FP32 f;
        uint16_t sign = (x >> 15) & 0x1;
        uint16_t exponent = (x >> 10) & 0x1F;
        uint16_t mantissa = x & 0x3FF;

        if (exponent == 0x0) {
            f.u = sign << 31;
        } else if ((exponent == 0x1F) && (mantissa != 0x0)) {
            f.u = 0x7FC00000;
        } else if ((exponent == 0x1F) && (mantissa == 0x0)) {
            f.u = (0x7F800000 | (sign << 31));
        } else {
            exponent += 0x70;
            f.u = (sign << 31) | (exponent << 23) | (mantissa << 13);
        }

        return f.f;
    }

    static fp16 epsilon() {
        fp16 x;
        x.value = 0x1400;  // 0x1.0p-7 in fp16
        return x;
    }

    static fp16 highest() {
        fp16 x;
        x.value = 0x7BFF;  // 0x1.FEp15 in fp16
        return x;
    }

    static fp16 lowest() {
        fp16 x;
        x.value = 0xFBFF;  // -0x1.FEp15 in fp16
        return x;
    }

    static fp16 min_positive_normal() {
        fp16 x;
        x.value = 0x0400;  // 0x1.0p-14 in fp16
        return x;
    }

    bool IsZero() const { return (value & 0x7FFF) == ZERO_VALUE; }

    uint16_t value;

private:
    static const uint16_t ZERO_VALUE = 0;

    FP16_DEVICE_FUNC static bool float_isnan(const float& x) {
#ifdef __CUDA_ARCH__
        return ::isnan(x);
#else
        return std::isnan(x);
#endif
    }
};

FP16_DEVICE_FUNC inline std::ostream& operator<<(std::ostream& os, const fp16& dt) {
    os << static_cast<float>(dt);
    return os;
}

FP16_DEVICE_FUNC inline fp16 operator+(fp16 a, fp16 b) {
    return fp16(static_cast<float>(a) + static_cast<float>(b));
}
FP16_DEVICE_FUNC inline fp16 operator+(fp16 a, int b) {
    return fp16(static_cast<float>(a) + static_cast<float>(b));
}
FP16_DEVICE_FUNC inline fp16 operator+(int a, fp16 b) {
    return fp16(static_cast<float>(a) + static_cast<float>(b));
}
FP16_DEVICE_FUNC inline fp16 operator-(fp16 a, fp16 b) {
    return fp16(static_cast<float>(a) - static_cast<float>(b));
}
FP16_DEVICE_FUNC inline fp16 operator*(fp16 a, fp16 b) {
    return fp16(static_cast<float>(a) * static_cast<float>(b));
}
FP16_DEVICE_FUNC inline fp16 operator/(fp16 a, fp16 b) {
    return fp16(static_cast<float>(a) / static_cast<float>(b));
}
FP16_DEVICE_FUNC inline fp16 operator-(fp16 a) {
    a.value ^= 0x8000;
    return a;
}
FP16_DEVICE_FUNC inline bool operator<(fp16 a, fp16 b) {
    return static_cast<float>(a) < static_cast<float>(b);
}
FP16_DEVICE_FUNC inline bool operator<=(fp16 a, fp16 b) {
    return static_cast<float>(a) <= static_cast<float>(b);
}
FP16_DEVICE_FUNC inline bool operator==(fp16 a, fp16 b) {
    return static_cast<float>(a) == static_cast<float>(b);
}
FP16_DEVICE_FUNC inline bool operator!=(fp16 a, fp16 b) {
    return static_cast<float>(a) != static_cast<float>(b);
}
FP16_DEVICE_FUNC inline bool operator>(fp16 a, fp16 b) {
    return static_cast<float>(a) > static_cast<float>(b);
}
FP16_DEVICE_FUNC inline bool operator>=(fp16 a, fp16 b) {
    return static_cast<float>(a) >= static_cast<float>(b);
}
FP16_DEVICE_FUNC inline fp16& operator+=(fp16& a, fp16 b) {
    a = a + b;
    return a;
}
FP16_DEVICE_FUNC inline fp16& operator-=(fp16& a, fp16 b) {
    a = a - b;
    return a;
}
FP16_DEVICE_FUNC inline fp16 operator++(fp16& a) {
    a += fp16(1);
    return a;
}
FP16_DEVICE_FUNC inline fp16 operator--(fp16& a) {
    a -= fp16(1);
    return a;
}
FP16_DEVICE_FUNC inline fp16 operator++(fp16& a, int) {
    fp16 original_value = a;
    ++a;
    return original_value;
}
FP16_DEVICE_FUNC inline fp16 operator--(fp16& a, int) {
    fp16 original_value = a;
    --a;
    return original_value;
}
FP16_DEVICE_FUNC inline fp16& operator*=(fp16& a, fp16 b) {
    a = a * b;
    return a;
}
FP16_DEVICE_FUNC inline fp16& operator/=(fp16& a, fp16 b) {
    a = a / b;
    return a;
}

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
    inline fp16 log10(const fp16& a) { return fp16(std::log10(float(a))); }
    inline fp16 sqrt(const fp16& a) { return fp16(std::sqrt(float(a))); }
    inline fp16 pow(const fp16& a, const fp16& b) { return fp16(std::pow(float(a), float(b))); }
    inline fp16 sin(const fp16& a) { return fp16(std::sin(float(a))); }
    inline fp16 cos(const fp16& a) { return fp16(std::cos(float(a))); }
    inline fp16 tan(const fp16& a) { return fp16(std::tan(float(a))); }
    inline fp16 tanh(const fp16& a) { return fp16(std::tanh(float(a))); }
    inline fp16 floor(const fp16& a) { return fp16(std::floor(float(a))); }
    inline fp16 ceil(const fp16& a) { return fp16(std::ceil(float(a))); }
}  // namespace std

#endif //MOFFETT_FP16_H
