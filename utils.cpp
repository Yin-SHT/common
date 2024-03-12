#include "utils.h"
#include "hw_config.h"
#include <csignal>

std::vector<uint64_t> bin8ToBin64(const std::vector<uint8_t> &bin8) {
  std::vector<uint64_t> bin64(bin8.size()/8, 0);
  for(size_t i = 0; i < bin64.size(); i++) {
    for(size_t j = 0; j < 8; j++) {
      bin64[i] |= (uint64_t)bin8[i*8+j] << (j*8);
    }
  }
  return bin64;
}

std::vector<uint8_t> bin64ToBin8(const std::vector<uint64_t> &bin64) {
  std::vector<uint8_t> bin8(bin64.size()*8, 0);
  for(size_t i = 0; i < bin64.size(); i++) {
    for(size_t j = 0; j < 8; j++) {
      bin8[i*8+j] = 0xFF & (bin64[i] >> (j*8));
    }
  }
  return bin8;
}

std::vector<uint32_t> bin8ToBin32(const std::vector<uint8_t> &bin8) {
  std::vector<uint32_t> bin32(bin8.size()/4, 0);
  for(size_t i = 0; i < bin32.size(); i++) {
    for(size_t j = 0; j < 4; j++) {
      bin32[i] |= (uint32_t)bin8[i*4+j] << (j*8);
    }
  }
  return bin32;
}

std::vector<uint8_t> bin32ToBin8(const std::vector<uint32_t> &bin32) {
  std::vector<uint8_t> bin8(bin32.size()*4, 0);
  for(size_t i = 0; i < bin32.size(); i++) {
    for(size_t j = 0; j < 4; j++) {
      bin8[i*4+j] = 0xFF & (bin32[i] >> (j*8));
    }
  }
  return bin8;
}

uint32_t getTensorSize(const TensorShape &tsr) {
  uint32_t tsrSize = tsr.N * tsr.H * tsr.W * tsr.C;
  if(tsr.dims.size() > 0) {
    tsrSize = 1;
    for(auto &it : tsr.dims) {
      tsrSize *= it;
    }
  }
  return tsrSize;
}

bool jsonExists(const nlohmann::json &node, const std::vector<std::string> &names) {
  auto p = node;
  for(auto &it : names) {
    if(p.find(it) == p.end() || p[it].is_null())
      return false;
    p = p[it];
  }
  return true;
}

void split(const std::string &text, std::vector<std::string> &v) {
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

int GetRegionMode() {
  if (antoum::kRegionMode >= 0)
    return (int)antoum::kRegionMode;

  switch (antoum::kGlbBankSize) {
    case 1:     return 0;
    case 2:     return 1;
    case 4:     return 2;
    case 8:     return 3;
    default:    return 0;
  }
}

static int g_hw_version = HW_VER1;
int GetHwVersion() {
  return g_hw_version;
}

void SetHwVersion(int ver) {
  g_hw_version = ver;
}
