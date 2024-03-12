#ifndef __DEFS_H
#define __DEFS_H

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <map>
#include <mutex>
#include <thread>
#include "glog/logging.h"
#include "dtype/float8/vpu/fp8_m4_quant_m5_dequant.h"
#include "dtype/bfloat16.h"
#include "dtype/bfloat24.h"
#include "dll_visibility.h"
#include "hw_config.h"

using namespace numeric;

typedef bfloat24 BF24;
typedef bfloat16 BF16;
typedef int64_t INT64;
typedef int32_t INT24;
typedef int32_t int24_t;
typedef std::vector<float> fvec1d_t;
typedef std::vector<fvec1d_t> fvec2d_t;
typedef std::vector<fvec2d_t> fvec3d_t;
typedef std::vector<fvec3d_t> fvec4d_t;

typedef std::vector<int32_t> vec1d_t;
typedef std::vector<vec1d_t> vec2d_t;
typedef std::vector<vec2d_t> vec3d_t;
typedef std::vector<vec3d_t> vec4d_t;

typedef std::vector<uint8_t> vec1d_u8;
typedef std::vector<vec1d_u8> vec2d_u8;
typedef std::vector<vec2d_u8> vec3d_u8;
typedef std::vector<vec3d_u8> vec4d_u8;

#undef INT24_MAX
#undef INT24_MIN

#define INT24_MAX ((1<<23) - 1)
#define INT24_MIN (-(1<<23))
#define INT24_ROUND(x) (std::min(std::max((x), INT24_MIN), INT24_MAX))
#define INT32_TO_INT24(x) (0xFFFFFF & (x))
#define INT24_TO_INT32(x) ((0x7FFFFF & (x)) | ((0x800000 & (x)) ?0xFF800000 :0))

enum DataType {
  DTYPE_INT8 = 0,
  DTYPE_BF16,
  DTYPE_INT16,
  DTYPE_INT32,
  DTYPE_INT64,
  DTYPE_UINT8,
  DTYPE_UINT16,
  DTYPE_UINT32,
  DTYPE_UINT64,
  DTYPE_INT24,
  DTYPE_UINT24,
  DTYPE_BF24,
  DTYPE_FP32,
  DTYPE_FP64,
  DTYPE_MF16,
  DTYPE_NF16,
  DTYPE_NF18,
  DTYPE_FP8_52,
  DTYPE_FP8_43,
  DTYPE_FP16,
};

enum HwVersion {
  HW_VER1 = 0,
  HW_VER1_5,
  HW_VER2_0
};

struct TensorDesc {
  uint32_t lineLength;
  uint32_t lineNumber;
  uint32_t lineStride;
  uint32_t surfaceNumber;
  uint32_t surfaceStride;
  uint64_t address;
};

struct TensorShape {
  TensorShape() {}
  TensorShape(uint32_t n, uint32_t h, uint32_t w, uint32_t c):
    N(n), H(h), W(w), C(c) {}
  uint32_t N;
  uint32_t H;
  uint32_t W;
  uint32_t C;
  std::vector<uint32_t> dims;
};

struct TensorShapeDesc {
  TensorShape org;
  TensorShape beg;
  TensorShape end;
  uint64_t addr;
  DataType dtype;
};

const std::map<std::string, DataType> dTypeMapping = {
  { "i8",         DTYPE_INT8   },
  { "int8",       DTYPE_INT8   },
  { "i16",        DTYPE_INT16  },
  { "int16",      DTYPE_INT16  },
  { "i32",        DTYPE_INT32  },
  { "int32",      DTYPE_INT32  },
  { "i64",        DTYPE_INT64  },
  { "int64",      DTYPE_INT64  },
  { "u8",         DTYPE_UINT8  },
  { "uint8",      DTYPE_UINT8  },
  { "u16",        DTYPE_UINT16 },
  { "uint16",     DTYPE_UINT16 },
  { "u24",        DTYPE_UINT24 },
  { "uint24",     DTYPE_UINT24 },
  { "u32",        DTYPE_UINT32 },
  { "uint32",     DTYPE_UINT32 },
  { "u64",        DTYPE_UINT64 },
  { "uint64",     DTYPE_UINT64 },
  { "i24",        DTYPE_INT24  },
  { "int24",      DTYPE_INT24  },
  { "bf16",       DTYPE_BF16   },
  { "bfloat16",   DTYPE_BF16   },
  { "bf24",       DTYPE_BF24   },
  { "bfloat24",   DTYPE_BF24   },
  { "f16",        DTYPE_FP16   },
  { "fp16",       DTYPE_FP16   },
  { "float16",    DTYPE_FP16   },
  { "f32",        DTYPE_FP32   },
  { "fp32",       DTYPE_FP32   },
  { "float32",    DTYPE_FP32   },
  { "f64",        DTYPE_FP64   },
  { "fp64",       DTYPE_FP64   },
  { "float64",    DTYPE_FP64   },
  { "mf16",       DTYPE_MF16   },
  { "nf16",       DTYPE_NF16   },
  { "nf18",       DTYPE_NF18   },
  { "fp8_52",     DTYPE_FP8_52 },
  { "fp8_43",     DTYPE_FP8_43 },
};

const std::map<DataType, int> dTypeWidthMapping = {
  { DTYPE_INT8,   1 },
  { DTYPE_BF16,   2 },
  { DTYPE_INT16,  2 },
  { DTYPE_INT32,  4 },
  { DTYPE_INT64,  8 },
  { DTYPE_UINT8,  1 },
  { DTYPE_UINT16, 2 },
  { DTYPE_UINT32, 4 },
  { DTYPE_UINT64, 8 },
  { DTYPE_INT24,  4 },
  { DTYPE_UINT24, 4 },
  { DTYPE_BF24,   4 },
  { DTYPE_FP32,   4 },
  { DTYPE_FP64,   8 },
  { DTYPE_MF16,   2 },
  { DTYPE_NF16,   2 },
  { DTYPE_NF18,   4 },
  { DTYPE_FP8_52, 1 },
  { DTYPE_FP8_43, 1 },
  { DTYPE_FP16,   2 },
};

inline std::string dataTypeToString(DataType dtype) {
  for(auto &it : dTypeMapping) {
    if(it.second == dtype)
      return it.first;
  }
  return "int8";
}

#ifdef __cplusplus
extern "C"{
#endif

extern DLL_PUBLIC int GetHwVersion();

#ifdef __cplusplus
}
#endif

#endif
