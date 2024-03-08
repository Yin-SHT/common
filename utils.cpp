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

static std::shared_ptr<ConfigType> g_config = nullptr;
static std::mutex config_mtx;

void SetConfigByJson(nlohmann::json &js, bool dumpJson) {
  config_mtx.lock();

  auto config = g_config;
  if(dumpJson == false || config == nullptr) {
    config = std::shared_ptr<ConfigType>(new ConfigType);
  }

  #undef DDR_W_START_OFFSET
  #undef DDR_I_START_OFFSET
  #undef DDR_CACHE_START_OFFSET
  #define CONFIG(sec,name,default_value) do {\
    if(dumpJson) {\
      js[#sec][#name] = config->sec.name;\
    } else if(jsonExists(js, {#sec,#name})) {\
      config->sec.name = js[#sec][#name];\
    } else {\
      config->sec.name = default_value;\
      js[#sec][#name] = default_value;\
    }\
  } while(0)

  // === section = name ========================== default
  CONFIG(BACKEND  ,STATIONARY_CMD                 ,false );
  CONFIG(BACKEND  ,IR_PINGPONG_EN                 ,false );
  CONFIG(BACKEND  ,CRC_INSERTED                   ,true  );
  CONFIG(BACKEND  ,HAS_CRC_INSERTED               ,false );
  CONFIG(BACKEND  ,REGION_MODE                    ,3     );
  CONFIG(BACKEND  ,SEQ_MODE                       ,false );
  CONFIG(BACKEND  ,GLB_W_SIZE                     ,2     );
  CONFIG(BACKEND  ,GLB_BANK_SIZE                  ,8     );
  CONFIG(BACKEND  ,GLB_SLOT_EN                    ,false );
  CONFIG(BACKEND  ,REMOVE_REDUNDANCY_DEP          ,false );
  CONFIG(BACKEND  ,DMA_OPTIMIZE_EN                ,false );
  CONFIG(BACKEND  ,TSR_CONVERT_EN                 ,((::GetHwVersion()==HW_VER1_5||::GetHwVersion()==HW_VER2_0) ?false :true));
  CONFIG(BACKEND  ,SAVE_TSR_CONVERT_EN            ,((::GetHwVersion()==HW_VER1_5||::GetHwVersion()==HW_VER2_0) ?false :true));
  CONFIG(BACKEND  ,DDR_ALIGNED_BYTES              ,32    );
  CONFIG(BACKEND  ,REALLOCATE_DDR_EN              ,true  );
  CONFIG(BACKEND  ,DDR_W_START_OFFSET             ,0x15400000);
  CONFIG(BACKEND  ,DDR_I_START_OFFSET             ,0x80000000);
  CONFIG(BACKEND  ,DDR_CACHE_START_OFFSET         ,0xc0000000);
  CONFIG(BACKEND  ,BTCM_CACHE_START_OFFSET        ,0x20000);
  CONFIG(BACKEND  ,BTCM_CMD_CACHE_EN              ,true  );
  CONFIG(BACKEND  ,AUTO_PLACEHOLDER               ,false );
  CONFIG(BACKEND  ,TO_PLACEHOLDER                 ,""    );
  CONFIG(BACKEND  ,VPU_ROUNDING_ACTIVE            ,true  );
  CONFIG(BACKEND  ,ACT_ROUNDING_ACTIVE            ,true  );
  CONFIG(BACKEND  ,SPLIT_TSR_SIZE                 ,51200 );
  CONFIG(BACKEND  ,CONFIG_BIN_VER                 ,1     );
  CONFIG(BACKEND  ,BACK_STATUS_EN                 ,true  );
  CONFIG(BACKEND  ,FROM_DMA_STAGE                 ,false );
  CONFIG(BACKEND  ,OPT_BF24_ACCURACY              ,false );
  CONFIG(BACKEND  ,ASSERT_GLB_CONFLICT            ,true  );
  CONFIG(BACKEND  ,DMA_FAKE_NOP_EN                ,false );
  CONFIG(BACKEND  ,TIMING_LOG                     ,""    );
  CONFIG(BACKEND  ,MODEL_FLOPS                    ,0     );
  CONFIG(BACKEND  ,CORE_JOBS                      ,1     );
  CONFIG(BACKEND  ,ATTACH_CR5_INSTR               ,false );
  CONFIG(BACKEND  ,VPU_SCALAR_EN                  ,true  );
  CONFIG(BACKEND  ,FP8_FORMAT                     ,0     );
  CONFIG(KMD      ,DUMP_FOR_RTL                   ,false );
  CONFIG(KMD      ,DUMP_TSR_EN                    ,true  );
  CONFIG(KMD      ,TRACE_ISA_EN                   ,false );
  CONFIG(KMD      ,PRINT_STATUS_EN                ,false  );
  CONFIG(KMD      ,RAND_GLB_EN                    ,false );
  CONFIG(KMD      ,INFERENCE_SIZE                 ,2     );
  CONFIG(KMD      ,THIRD_BF24_ADD                 ,true  );
  CONFIG(KMD      ,DUMP_LIST_FILE                 ,""    );
  CONFIG(PERF     ,HW_FREQ                        ,700000000);
  CONFIG(PERF     ,FW_LATENCY                     ,50    );
  CONFIG(PERF     ,IGNORE_IO                      ,false );
  CONFIG(PERF     ,DMA_PRIORITY                   ,true  );
  CONFIG(PERF     ,REAL_DMA_SCHEDULE              ,false );
  CONFIG(PERF     ,DDR_BANDWIDTH                  ,12.0  );
  CONFIG(PEA      ,PEA_GH_GW                      ,true  );
  CONFIG(PEA      ,PEA_FORCE_DENCE                ,false );
  CONFIG(PEA      ,PEA_OUTPUT_BACKDOOR            ,false );
  CONFIG(PEA      ,PEA_RTL_LOG                    ,false );
  CONFIG(PEA      ,PEA_LOG_INF_NAN                ,false );
  CONFIG(PEA      ,PEA_LOG_GLBWR16                ,false );
  CONFIG(PEA      ,PEA_LOG_DEBUG                  ,false );
  CONFIG(PEA      ,PEA_UNIT_TEST_DIR              ,"./"  );
  CONFIG(PEA      ,PEA_LOG_DEBUG_PATH             ,"./"  );
  CONFIG(PEA      ,PEA_RTL_LOG_PATH               ,"./"  );
  CONFIG(PEA      ,PRINT_PEA_PROCESS              ,false );
  CONFIG(PEA      ,PRINT_FUSED_PROCESS            ,false );
  CONFIG(PEA      ,FUSEDENGINE_QUANTIZE_DISABLE   ,false );
  CONFIG(PEA      ,DEBUG_PEA_COMPILER             ,false );
  CONFIG(PEA      ,PEA_L1_ACC_VERIFY              ,false );
  CONFIG(PEA      ,TSR_CONVERT_EN                 ,false );
  CONFIG(PEA      ,SAVE_TSR_CONVERT_EN            ,false );
  CONFIG(PEA      ,HW_FP8_LIB_EN                  ,true );

  if(g_config != config) {
    g_config = config;
  }
  #undef CONFIG
  config_mtx.unlock();
}

std::shared_ptr<ConfigType> GetConfig() {
  if(g_config == nullptr) {
      config_mtx.lock();
      config_mtx.unlock();
  
      if(g_config == nullptr) {
        nlohmann::json js = {};
        SetConfigByJson(js);
      }
  }
  return g_config;
}

int GetRegionMode() {
  if(GetConfig()->BACKEND.REGION_MODE >= 0) {
      return GetConfig()->BACKEND.REGION_MODE;
  }
  switch(GetConfig()->BACKEND.GLB_BANK_SIZE) {
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
