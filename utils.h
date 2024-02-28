#ifndef __UTILS_H
#define __UTILS_H

#include <stack>
#include <set>
#include <typeinfo>
#include <map>
#include <string.h>
#include <future>
#include <functional>
#include <queue>
#include <assert.h>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <list>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <tuple>
#include <numeric>
#include <memory.h>
#include <regex>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <unordered_set>
#include <sys/types.h>
#include <sys/stat.h>

#include "json.h"
#include "defs.h"
#include "hw_config.h"

enum ISAOpCode {
  NOP_OP_CODE = 0x0,
  LOADCFG_OP_CODE = 0x1,
  CONV2D_OP_CODE = 0x2,
  ACTIV_OP_CODE = 0x3,
  TOPK_OP_CODE = 0x4,
  TRANS_OP_CODE = 0x5,
  GLBCPY_OP_CODE = 0x6,
  DMAMV_OP_CODE = 0x7,
  INSTR_GEN_OP_CODE = 0x8,
};
struct FieldMappingEntry {
  uint8_t *mem;
  size_t m_end;
  size_t m_beg;
  size_t f_end;
  size_t f_beg;
  uint8_t *field;
  
  explicit FieldMappingEntry(void *_mem, size_t _m_end, size_t _m_beg, 
    void *_field, size_t _f_end, size_t _f_beg) :
    mem((uint8_t *)_mem), m_end(_m_end), m_beg(_m_beg), 
    f_end(_f_end), f_beg(_f_beg), field((uint8_t *)_field) {
    assert(m_end >= m_beg);
    assert(f_end >= f_beg);
    assert(m_end-_m_beg == f_end-f_beg);
  }
  
  void FieldToMem() {
    for(size_t i = m_beg, j = f_beg; i <= m_end; i+=8, j+=8) {
      SetByte(mem, i, m_end+1, GetByte(field, j, f_end+1));
    }
  }
  
  void MemToField() {
    for(size_t i = f_beg, j = m_beg; i <= f_end; i+=8, j+=8) {
      SetByte(field, i, f_end+1, GetByte(mem, j, m_end+1));
    }
  }

protected:
  static uint8_t GetByte(uint8_t *buf, size_t beg, size_t end) {
    size_t idx = beg/8;
    uint8_t ofs = beg%8;
    size_t len = std::min<size_t>(8, end-beg);
    if(((size_t)8-ofs) < len) {
      return (buf[idx] >> ofs) | (buf[idx+1] << ((size_t)8-ofs));
    }
    return buf[idx] >> ofs;
  }
  
  static void SetByte(uint8_t *buf, size_t beg, size_t end, uint8_t dat) {
    size_t idx = beg/8;
    uint8_t ofs = beg%8;
    size_t len = std::min<size_t>(8, end-beg);
    if(((size_t)8-ofs) < len) {
      size_t len2 = len - ((size_t)8-ofs);
      uint8_t msk = (1<<len2)-1;
      len -= len2;
      buf[idx+1] = (buf[idx+1] & (~msk)) | ((dat >> len) & msk);
    }
    uint8_t msk = ((1<<len)-1) << ofs;
    buf[idx] = (buf[idx] & (~msk)) | ((dat << ofs) & msk);
  }
};

static std::vector<uint64_t> bin8ToBin64(const std::vector<uint8_t> &bin8) {
  std::vector<uint64_t> bin64(bin8.size()/8, 0);
  for(size_t i = 0; i < bin64.size(); i++) {
    for(size_t j = 0; j < 8; j++) {
      bin64[i] |= (uint64_t)bin8[i*8+j] << (j*8);
    }
  }
  return bin64;
};

static std::vector<uint8_t> bin64ToBin8(const std::vector<uint64_t> &bin64) {
  std::vector<uint8_t> bin8(bin64.size()*8, 0);
  for(size_t i = 0; i < bin64.size(); i++) {
    for(size_t j = 0; j < 8; j++) {
      bin8[i*8+j] = 0xFF & (bin64[i] >> (j*8));
    }
  }
  return bin8;
};

static std::vector<uint32_t> bin8ToBin32(const std::vector<uint8_t> &bin8) {
  std::vector<uint32_t> bin32(bin8.size()/4, 0);
  for(size_t i = 0; i < bin32.size(); i++) {
    for(size_t j = 0; j < 4; j++) {
      bin32[i] |= (uint32_t)bin8[i*4+j] << (j*8);
    }
  }
  return bin32;
};

static std::vector<uint8_t> bin32ToBin8(const std::vector<uint32_t> &bin32) {
  std::vector<uint8_t> bin8(bin32.size()*4, 0);
  for(size_t i = 0; i < bin32.size(); i++) {
    for(size_t j = 0; j < 4; j++) {
      bin8[i*4+j] = 0xFF & (bin32[i] >> (j*8));
    }
  }
  return bin8;
};

inline uint32_t getTensorSize(const TensorShape &tsr) {
  uint32_t tsrSize = tsr.N * tsr.H * tsr.W * tsr.C;
  if(tsr.dims.size() > 0) {
    tsrSize = 1;
    for(auto &it : tsr.dims) {
      tsrSize *= it;
    }
  }
  return tsrSize;
}

inline bool jsonExists(const nlohmann::json &node, const std::vector<std::string> &names) {
  auto p = node;
  for(auto &it : names) {
    if(p.find(it) == p.end() || p[it].is_null())
      return false;
    p = p[it];
  }
  return true;
}

inline void split(const std::string &text, std::vector<std::string> &v) {
  size_t beg = 0, _size = text.size();
  v.clear();
  while(beg < _size) {
    for(; beg < _size && text[beg] == ' '; beg++);
    if(beg < _size) {
      std::string s;
      for(; beg < _size && text[beg] != ' '; beg++) {
        s.push_back(text[beg]);
      }
      v.emplace_back(s);
    }
  }
}

enum EngineIndex {
  DMA_ENGINE_IDX = 0,
  GCPY_ENGINE_IDX,
  PEA_ENGINE_IDX,
  VPU_ENGINE_IDX,
  TRANSPOSE_ENGINE_IDX,
  TOPK_ENGINE_IDX,
  ACTIV_ENGINE_IDX,
  DMA1_ENGINE_IDX,
  DMA2_ENGINE_IDX,
  DMA3_ENGINE_IDX,
  DMA4_ENGINE_IDX,
  DMA5_ENGINE_IDX,
  DMA6_ENGINE_IDX,
  DMA7_ENGINE_IDX,
  ENGINE_SIZE,
  CONFIG_ENGINE_IDX = ENGINE_SIZE,
  START_ENGINE_IDX,
  END_ENGINE_IDX,
  CPU_ENGINE_IDX,
  PP_ENGINE_IDX,
  INVLIAD_ENGINE_IDX = 255,
};

#define CORE_INT_STAT       (0x202000 + 0x420)
#define BASE_ENGINE_CTRL    (0x201500)
#define ENGINE_OP_ENABLE    (1)
#define EXT_ENGINE_NUM 5

inline MemMappingIdx findMemMappingIdx(uint64_t addr) {
  for(auto &it : MemMapping) {
    if(addr >= it.second.begin_addr && addr <= it.second.end_addr)
      return it.first;
  }
  return MM_INVALID;
}

typedef struct {
  struct {
    bool STATIONARY_CMD;
    bool IR_PINGPONG_EN;
    bool CRC_INSERTED;
    bool HAS_CRC_INSERTED;
    int  REGION_MODE;
    bool SEQ_MODE;
    int  GLB_W_SIZE;
    int  GLB_BANK_SIZE;
    bool GLB_SLOT_EN;
    bool REMOVE_REDUNDANCY_DEP;
    bool DMA_OPTIMIZE_EN;
    bool TSR_CONVERT_EN;
    bool SAVE_TSR_CONVERT_EN;
    long DDR_ALIGNED_BYTES;
    bool REALLOCATE_DDR_EN;
    long DDR_W_START_OFFSET;
    long DDR_I_START_OFFSET;
    long DDR_CACHE_START_OFFSET;
    long BTCM_CACHE_START_OFFSET;
    bool BTCM_CMD_CACHE_EN;
    bool AUTO_PLACEHOLDER;
    std::string TO_PLACEHOLDER;
    bool VPU_ROUNDING_ACTIVE;
    bool ACT_ROUNDING_ACTIVE;
    int SPLIT_TSR_SIZE;
    int CONFIG_BIN_VER;
    bool BACK_STATUS_EN;
    bool FROM_DMA_STAGE;
    bool OPT_BF24_ACCURACY;
    bool ASSERT_GLB_CONFLICT;
    bool DMA_FAKE_NOP_EN;
    std::string TIMING_LOG;
    uint64_t MODEL_FLOPS;
    size_t CORE_JOBS;
    bool ATTACH_CR5_INSTR;
    bool VPU_SCALAR_EN;
    size_t FP8_FORMAT; // 0: nv, 1: gr
  
  } BACKEND;
  
  struct {
    bool DUMP_FOR_RTL;
    bool DUMP_TSR_EN;
    bool TRACE_ISA_EN;
    bool PRINT_STATUS_EN;
    bool RAND_GLB_EN;
    long INFERENCE_SIZE;
    bool THIRD_BF24_ADD;
    std::string DUMP_LIST_FILE;
  
  } KMD;
  
  struct {
    long HW_FREQ;
    long FW_LATENCY;
    bool IGNORE_IO;
    bool DMA_PRIORITY;
    bool REAL_DMA_SCHEDULE;
    float DDR_BANDWIDTH;
  
  } PERF;
  
  struct {
    std::string PEA_UNIT_TEST_DIR;
    std::string PEA_LOG_DEBUG_PATH; 
    std::string PEA_RTL_LOG_PATH;
    bool PRINT_PEA_PROCESS;
    bool PRINT_FUSED_PROCESS;
    bool DEBUG_PEA_COMPILER;
    bool PEA_RTL_LOG;
    bool PEA_GH_GW;
    bool PEA_FORCE_DENCE; 
    bool PEA_OUTPUT_BACKDOOR; 
    bool FUSEDENGINE_QUANTIZE_DISABLE; 
    bool PEA_LOG_DEBUG;
    bool PEA_LOG_INF_NAN; 
    bool PEA_LOG_GLBWR16; 
    bool PEA_L1_ACC_VERIFY;
    bool TSR_CONVERT_EN;
    bool SAVE_TSR_CONVERT_EN;
    bool HW_FP8_LIB_EN;
  } PEA;

} ConfigType;

void SetConfigByJson(nlohmann::json &js, bool dumpJson=false);
std::shared_ptr<ConfigType> GetConfig();
int GetRegionMode();

#ifdef __cplusplus
extern "C"{
#endif

DLL_PUBLIC int GetHwVersion();
DLL_PUBLIC void SetHwVersion(int ver);

#ifdef __cplusplus
}
#endif

#endif
