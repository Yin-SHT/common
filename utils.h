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
  ss << "Core" << coreId << "." << "DDR";
  return ss.str();
}

inline std::string getSharedMemoryName(size_t coreId, size_t peId) {
  std::stringstream ss;
  ss << "Core" << coreId << "." << "Pe" << peId << "." << "SharedMemory";
  return ss.str();
}

inline std::string getEngineName(EngineType engine) {
  switch  (engine) {
    case EngineType::SPU:  return "SPU";
    case EngineType::VPU:  return "VPU";
    case EngineType::AE:   return "AE";
    case EngineType::TE:   return "TE";
    case EngineType::SE:   return "SE";
    case EngineType::DMA0: return "DMA0";
    case EngineType::DMA1: return "DMA1";
    case EngineType::DMA2: return "DMA2";
    case EngineType::DMA3: return "DMA3";
    case EngineType::DMA4: return "DMA4";
    case EngineType::DMA5: return "DMA5";
    default:               return "";
  }
}

inline std::string getCmdMemoryName(size_t coreId, size_t peId, EngineType engine) {
  std::stringstream ss;
  ss << "Core" << coreId << "." << "Pe" << peId << "." 
    << getEngineName(engine) << "." << "CmdMemory";
  return ss.str();
}

};

#endif
