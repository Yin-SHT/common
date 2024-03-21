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
#include <signal.h>

#include "json.h"
#include "defs.h"
#include "hw_config.h"

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

std::vector<uint64_t> bin8ToBin64(const std::vector<uint8_t> &bin8);
std::vector<uint8_t> bin64ToBin8(const std::vector<uint64_t> &bin64);
std::vector<uint32_t> bin8ToBin32(const std::vector<uint8_t> &bin8);
std::vector<uint8_t> bin32ToBin8(const std::vector<uint32_t> &bin32);
uint32_t getTensorSize(const TensorShape &tsr);
bool jsonExists(const nlohmann::json &node, const std::vector<std::string> &names);
void split(const std::string &text, std::vector<std::string> &v);

int GetRegionMode();

#ifdef __cplusplus
extern "C"{
#endif

DLL_PUBLIC int GetHwVersion();
DLL_PUBLIC void SetHwVersion(int ver);

#ifdef __cplusplus
}
#endif

namespace spu {
class Initializer {
protected:
  Initializer(std::string exeName, std::function<void()> moreInitFunc) {
    FLAGS_colorlogtostderr = true;
    google::InitGoogleLogging(exeName.c_str());
    google::SetStderrLogging(google::GLOG_INFO);
    signal(SIGABRT, &abortHandler);
    signal(SIGSEGV, &abortHandler);

    if (moreInitFunc) {
      moreInitFunc();
    }
  }

  static void abortHandler(int signal_number) {
    if (SIGSEGV == signal_number) {
      LOG(ERROR) << google::GetStackTrace();
      abort();
    }
  }

public:
  static Initializer *getInstance(std::string exeName, 
      std::function<void()> moreInitFunc=nullptr) {
    static Initializer instance(exeName, moreInitFunc);
    return &instance;
  }
  virtual ~Initializer() {
    google::ShutdownGoogleLogging();
  }
};
} // namespace spu

namespace quark {

inline std::string getDdrName(size_t coreId) {
  std::stringstream ss;
  ss << "core" << coreId << "." << "ddr";
  return ss.str();
}

inline std::string getSharedMemoryName(size_t coreId, size_t peId) {
  std::stringstream ss;
  ss << "core" << coreId << "." << "pe" << peId << "." << "sm";
  return ss.str();
}

inline std::string getEngineName(EngineType engine) {
  switch  (engine) {
    case EngineType::SPU:  return "spu";
    case EngineType::VPU:  return "vpu";
    case EngineType::AE:   return "ae";
    case EngineType::TE:   return "te";
    case EngineType::SE:   return "se";
    case EngineType::DMA0: return "dma0";
    case EngineType::DMA1: return "dma1";
    case EngineType::DMA2: return "dma2";
    case EngineType::DMA3: return "dma3";
    case EngineType::DMA4: return "dma4";
    case EngineType::DMA5: return "dma5";
    default:               return "";
  }
}

inline std::string getCmdMemoryName(size_t coreId, size_t peId, EngineType engine) {
  std::stringstream ss;
  ss << "core" << coreId << "." << "pe" << peId << "." 
    << getEngineName(engine) << "." << "cmd";
  return ss.str();
}

};

#endif
