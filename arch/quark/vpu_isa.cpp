/*
 * Copyright (c) 2018-2024 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */
#include "quark/vpu_isa.h"

namespace quark {

enum class ScalarSetCfg {
  LOAD_STRIDE = 0,
  LOAD_CLUSTER_MSK,
  LOAD_BANK_MSK,
  LOAD_CLUSTER_BRD,
  LOAD_BANK_BRD,
  LOAD_BASE0,
  LOAD_DQUAN0,
  LOAD_BASE1,
  LOAD_DQUAN1,
  STORE_BASE,
  STORE_STRIDE,
  STORE_CLUSTER_MSK,
  STORE_BANK_MSK,
  STORE_QUAN,
  LOAD_CHANN_BRD,
  BF16_OR_FP16,
};

VpuIsa::VpuIsa() : loadHelper(new BaseHelper()), 
    loadHelper2(new BaseHelper()), storeHelper(new BaseHelper()) {
  loadHelper->setParam(this);
  loadHelper2->setParam(this);
  storeHelper->setParam(this);
}

std::shared_ptr<VpuIsa> VpuIsa::clone() {
  std::shared_ptr<VpuIsa> ret(new VpuIsa);
  ret->loadHelper = loadHelper->clone();
  ret->loadHelper2 = loadHelper2->clone();
  ret->storeHelper = storeHelper->clone();
  ret->loadDesc = loadDesc;
  ret->loadDesc2 = loadDesc2;
  ret->storeDesc = storeDesc;

  ret->loadHelper->setParam(ret.get());
  ret->loadHelper2->setParam(ret.get());
  ret->storeHelper->setParam(ret.get());
  ret->rsHigh = rsHigh;
  ret->rtHigh = rtHigh;
  ret->rdHigh = rdHigh;
  ret->fs0 = fs0;
  ret->fs1 = fs1;
  ret->fs2 = fs2;
  ret->fs3 = fs3;
  ret->fs45 = fs45;
  ret->fs67 = fs67;
  ret->redundantRemoverEn = redundantRemoverEn;
  ret->labelMap = labelMap;
  ret->ldBaseReg = ldBaseReg;
  ret->ld2BaseReg = ld2BaseReg;
  ret->stBaseReg = stBaseReg;
  ret->ldTmpReg = ldTmpReg;
  ret->ld2TmpReg = ld2TmpReg;
  ret->stTmpReg = stTmpReg;
  ret->aaiSteps = aaiSteps;
  ret->aaiLength = aaiLength;

  ret->instrs.clear();
  for(auto &it : instrs)
    ret->instrs.push_back(it->clone());
  return ret;
}

void VpuIsa::setLoadBaseReg(size_t baseReg, size_t tmpReg) {
  if (baseReg != -1) {
    setLoadBase(0);
    ldBaseReg = baseReg;
    ldTmpReg = tmpReg;

    size_t n = loadHelper->getNewBase()&(~0x3f);
    if (n <= 0x7FFF) {
      sAddi(ldBaseReg, n, ldTmpReg);
    } else {
      sLi(n, ldTmpReg);
      sAdd(ldBaseReg, ldTmpReg, ldTmpReg);
    }
    sSetcfg(ldTmpReg, (size_t)ScalarSetCfg::LOAD_BASE0);

  } else {
    ldBaseReg = ldTmpReg = -1;
    setLoadBase(0);
  }
}
void VpuIsa::setLoad2BaseReg(size_t baseReg, size_t tmpReg) {
  if (baseReg != -1) {
    setLoad2Base(0);
    ld2BaseReg = baseReg;
    ld2TmpReg = tmpReg;

    size_t n = loadHelper2->getNewBase()&(~0x3f);
    if (n <= 0x7FFF) {
      sAddi(ld2BaseReg, n, ld2TmpReg);
    } else {
      sLi(n, ld2TmpReg);
      sAdd(ld2BaseReg, ld2TmpReg, ld2TmpReg);
    }
    sSetcfg(ld2TmpReg, (size_t)ScalarSetCfg::LOAD_BASE1);

  } else {
    ld2BaseReg = ld2TmpReg = -1;
    setLoad2Base(0);
  }
}
void VpuIsa::setStoreBaseReg(size_t baseReg, size_t tmpReg) {
  if (baseReg != -1) {
    setStoreBase(0);
    stBaseReg = baseReg;
    stTmpReg = tmpReg;

    size_t n = storeHelper->getNewBase()&(~0x3f);
    if (n <= 0x7FFF) {
      sAddi(stBaseReg, n, stTmpReg);
    } else {
      sLi(n, stTmpReg);
      sAdd(stBaseReg, stTmpReg, stTmpReg);
    }
    sSetcfg(stTmpReg, (size_t)ScalarSetCfg::STORE_BASE);

  } else {
    stBaseReg = stTmpReg = -1;
    setStoreBase(0);
  }
}
void VpuIsa::setLoadBase(size_t baseAddr, size_t csr) {
  loadHelper->setCallback(baseAddr&(~63), csr, 
      [] (int32_t base, size_t csr, void *param) {
    auto x = (quark::VpuIsa*)param;
    if (x->ldBaseReg != -1) {
      size_t n = base&(~0x3f);
      if (n <= 0x7FFF) {
        x->sAddi(x->ldBaseReg, n, x->ldTmpReg);
      } else {
        x->sLi(n, x->ldTmpReg);
        x->sAdd(x->ldBaseReg, x->ldTmpReg, x->ldTmpReg);
      }
      x->sSetcfg(x->ldTmpReg, (size_t)ScalarSetCfg::LOAD_BASE0);
    } else {
      ((VpuIsa*)param)->addIsa<SetLoadBaseInstr>({size_t(base&0xffffff)/64, csr});
    }
  });
}
void VpuIsa::setLoad2Base(size_t baseAddr, size_t csr) {
  loadHelper2->setCallback(baseAddr&(~63), csr, 
      [] (int32_t base, size_t csr, void *param) {
    auto x = (quark::VpuIsa*)param;
    if (x->ld2BaseReg != -1) {
      size_t n = base&(~0x3f);
      if (n <= 0x7FFF) {
        x->sAddi(x->ld2BaseReg, n, x->ld2TmpReg);
      } else {
        x->sLi(n, x->ld2TmpReg);
        x->sAdd(x->ld2BaseReg, x->ld2TmpReg, x->ld2TmpReg);
      }
      x->sSetcfg(x->ld2TmpReg, (size_t)ScalarSetCfg::LOAD_BASE1);
    } else {
      ((VpuIsa*)param)->addIsa<SetLoad2BaseInstr>({size_t(base&0xffffff)/64, csr});
    }
  });
}
void VpuIsa::setStoreBase(size_t baseAddr, size_t csr) {
  storeHelper->setCallback(baseAddr&(~63), csr, 
      [] (int32_t base, size_t csr, void *param) { 
    auto x = (quark::VpuIsa*)param;
    if (x->stBaseReg != -1) {
      size_t n = base&(~0x3f);
      if (n <= 0x7FFF) {
        x->sAddi(x->stBaseReg, n, x->stTmpReg);
      } else {
        x->sLi(n, x->stTmpReg);
        x->sAdd(x->stBaseReg, x->stTmpReg, x->stTmpReg);
      }
      x->sSetcfg(x->stTmpReg, (size_t)ScalarSetCfg::STORE_BASE);
    } else {
      ((VpuIsa*)param)->addIsa<SetStoreBaseInstr>({size_t(base&0xffffff)/64, csr});
    }
  });
}

void VpuIsa::setWeightMode(size_t src, size_t dst) {
  if (fs45 != src || fs67 != dst) {
    fs45 = src;
    fs67 = dst;
    setFs(encodeFs());
  }
}

void VpuIsa::load(size_t offset, size_t z, DataType dtype, AluType alu) {
  offset = adjustOffset(loadHelper, offset)/64;
  updateFs(dtype, alu);
  switch (dtype) {
    case DTYPE_INT8:    i8Load(offset, z);  break;
    case DTYPE_UINT8:   u8Load(offset, z);  break;
    case DTYPE_BF16:    f16Load(offset, z); break;
    case DTYPE_FP32:    f32Load(offset, z); break;
    case DTYPE_INT16:   i8Load(offset, z);  break;
    case DTYPE_INT32:   i32Load(offset, z); break;
    case DTYPE_FP8_52:  f16Load(offset, z); break;
    case DTYPE_FP8_43:  f32Load(offset, z); break;
    case DTYPE_FP16:    f16Load(offset, z); break;
    case DTYPE_FP64:    f32Load(offset, z); break;
    case DTYPE_INT4:    i8Load(offset, z);  break;
    case DTYPE_INT64:   i32Load(offset, z); break;
    case DTYPE_FP4:     f16Load(offset, z); break;
    default: break;
  }
}
void VpuIsa::dload(size_t ldOffset, size_t ld2Offset, size_t z, size_t z2, DataType dtype, AluType alu) {
  size_t offset = adjustOffset(loadHelper, loadHelper2, ldOffset, ld2Offset)/64;
  updateFs(dtype, alu);
  switch (dtype) {
    case DTYPE_INT8:    i8Dload(offset, z, z2);   break;
    case DTYPE_UINT8:   u8Dload(offset, z, z2);   break;
    case DTYPE_BF16:    f16Dload(offset, z, z2);  break;
    case DTYPE_FP32:    f32Dload(offset, z, z2);  break;
    case DTYPE_INT16:   i8Dload(offset, z, z2);   break;
    case DTYPE_INT32:   i32Dload(offset, z, z2);  break;
    case DTYPE_FP8_52:  f16Dload(offset, z, z2);  break;
    case DTYPE_FP8_43:  f32Dload(offset, z, z2);  break;
    case DTYPE_FP16:    f16Dload(offset, z, z2);  break;
    case DTYPE_FP64:    f32Dload(offset, z, z2);  break;
    case DTYPE_INT4:    i8Dload(offset, z, z2);   break;
    case DTYPE_INT64:   i32Dload(offset, z, z2);  break;
    case DTYPE_FP4:     f16Dload(offset, z, z2);  break;
    default: break;
  }
}
void VpuIsa::store(size_t x, size_t offset, DataType dtype, AluType alu) {
  offset = adjustOffset(storeHelper, offset)/64;
  updateFs(dtype, alu);
  switch (dtype) {
    case DTYPE_INT8:    i8Store(x, offset);   break;
    case DTYPE_UINT8:   u8Store(x, offset);   break;
    case DTYPE_BF16:    f16Store(x, offset);  break;
    case DTYPE_FP32:    f32Store(x, offset);  break;
    case DTYPE_INT16:   i8Store(x, offset);   break;
    case DTYPE_INT32:   i32Store(x, offset);  break;
    case DTYPE_FP8_52:  f16Store(x, offset);  break;
    case DTYPE_FP8_43:  f32Store(x, offset);  break;
    case DTYPE_FP16:    f16Store(x, offset);  break;
    case DTYPE_FP64:    f32Store(x, offset);  break;
    case DTYPE_INT4:    i8Store(x, offset);   break;
    case DTYPE_INT64:   i32Store(x, offset);  break;
    case DTYPE_FP4:     f16Store(x, offset);  break;
    default: break;
  }
}
void VpuIsa::movi(double y, size_t z, DataType dtype, AluType alu) {
  CHECK(alu == AluType::FP32_ALU);
  if (dtype != DTYPE_FP16 && y != std::floor(y))
    dtype = DTYPE_BF16;

  switch (dtype) {
    case DTYPE_FP32:
    case DTYPE_FP8_52:
    case DTYPE_FP8_43:
    case DTYPE_FP4:
    case DTYPE_BF16:
      updateFs(DTYPE_BF16, alu);
      bf16Movi(bfloat16(y).value, z);
      break;
    case DTYPE_FP16:
      updateFs(DTYPE_FP16, alu);
      f16Movi(fp16(y).value, z);
      break;
    case DTYPE_INT8:
    case DTYPE_UINT8:
    case DTYPE_INT4:
    case DTYPE_INT16:
      updateFs(DTYPE_INT16, alu);
      i16Movi(y, z);
      break;
    default:
      break;
  }
}
void VpuIsa::Li(double y, size_t z, DataType dtype, AluType alu) {
  self_float t;
  updateFs(dtype, alu);

  if (alu == AluType::INT32_ALU) {
    t.value = y;
  } else if (alu == AluType::FP32_ALU) {
    if (dtype == DTYPE_INT16)
      t.value = y;
    else
      t.fp32 = y;
  } else {
    LOG(FATAL);
  }
  f32Li(0xffff&t.value, z);
  f32Lui(0xffff&(t.value>>16), z);
}
void VpuIsa::sLi(size_t y, size_t z) {
  if (y == 0) {
    sAnd(z, (z>=16) ?16 :0, z);
  } else {
    sLui(0xffff&(y>>16), z);
    sOri(z, 0xffff&(y), z);
  }
}
void VpuIsa::sSetcfg(size_t rs, size_t rt, size_t sp_rd0, size_t sp_rd1) {
  sSetcfg(rs, rt, sp_rd0, sp_rd1, 1);
}
void VpuIsa::sSetcfg(size_t rs, size_t sp_rd0) {
  sSetcfg(rs, 0, sp_rd0, 0, 0);
}
void VpuIsa::sSetLabel(std::string label) {
  CHECK(labelMap.find(label) == labelMap.end()) << " duplicated label `" << label << "`";
  redundantRemoverEn = false;
  labelMap[label] = instrs.size();
}
size_t VpuIsa::sGetLabel(std::string label) {
  CHECK(labelMap.find(label) != labelMap.end()) << " no found label `" << label << "`";
  return labelMap[label];
}
void VpuIsa::sClearLabels() {
  labelMap.clear();
}
void VpuIsa::sJump(std::string label) {
  sJump(getPc(sGetLabel(label)));
}
void VpuIsa::sJal(std::string label) {
  sJal(getPc(sGetLabel(label)));
}
void VpuIsa::sBne(size_t x, size_t y, std::string label) {
  sBne(x, y, getPc(sGetLabel(label)-instrs.size()));
}
void VpuIsa::sBeq(size_t x, size_t y, std::string label) {
  sBeq(x, y, getPc(sGetLabel(label)-instrs.size()));
}
void VpuIsa::sBlt(size_t x, size_t y, std::string label) {
  sBlt(x, y, getPc(sGetLabel(label)-instrs.size()));
}
void VpuIsa::sBltu(size_t x, size_t y, std::string label) {
  sBltu(x, y, getPc(sGetLabel(label)-instrs.size()));
}
void VpuIsa::sBge(size_t x, size_t y, std::string label) {
  sBge(x, y, getPc(sGetLabel(label)-instrs.size()));
}
void VpuIsa::sBgeu(size_t x, size_t y, std::string label) {
  sBgeu(x, y, getPc(sGetLabel(label)-instrs.size()));
}
void VpuIsa::setAai(size_t stride, size_t length) {
  stride /= 64;
  if (stride == 0 || length == 0) {
    setAai(0, 0, 0);
  } else {
    size_t mode = 0;
    for (; (length>>(mode+1)) != 0; ++mode);
    setAai(1, mode, ((stride-1)<<(mode+1)) | (length-1));
  }
}

void VpuIsa::enableAai(const std::vector<size_t> &steps, size_t length) {
  aaiSteps = steps;
  aaiLength = length;
}

void VpuIsa::startAai() {
  if (!aaiSteps.empty())
    setAai(aaiSteps.front(), aaiLength);
}

void VpuIsa::endAai() {
  if (!aaiSteps.empty())
    setAai(0, 0);
}

size_t VpuIsa::adjustOffset(std::shared_ptr<BaseHelper> helper, size_t offset) {
  return (size_t)helper->adjustOffset((int32_t)offset);
}

size_t VpuIsa::adjustOffset(std::shared_ptr<BaseHelper> ld, std::shared_ptr<BaseHelper> st, size_t ldOffset, size_t stOffset) {
  int32_t newLdOffset = ld->adjustOffset((int32_t)ldOffset);
  int32_t newStOffset = st->adjustOffset((int32_t)stOffset);
  if (newLdOffset != newStOffset)
    ld->setNewBase(ld->getNewBase()+newLdOffset-newStOffset);
  return (size_t)newStOffset;
}

size_t VpuIsa::adjustOffset(std::shared_ptr<BaseHelper> ld, std::shared_ptr<BaseHelper> ld2, std::shared_ptr<BaseHelper> st, 
  size_t ldOffset, size_t ld2Offset, size_t stOffset) {
  int32_t newLdOffset = ld->adjustOffset((int32_t)ldOffset);
  int32_t newLd2Offset = ld2->adjustOffset((int32_t)ld2Offset);
  int32_t newStOffset = st->adjustOffset((int32_t)stOffset);

  if (newLdOffset == newLd2Offset && newLdOffset != newStOffset) {
    st->setNewBase(st->getNewBase()+newStOffset-newLdOffset);
    return (size_t)newLdOffset;
  }
  if (newLdOffset != newStOffset)
    ld->setNewBase(ld->getNewBase()+newLdOffset-newStOffset);
  if (newLd2Offset != newStOffset)
    ld2->setNewBase(ld2->getNewBase()+newLd2Offset-newStOffset);
  return (size_t)newStOffset;
}

void VpuIsa::updateRfSel(size_t rs, size_t rt, size_t rd) {
  bool update = false;
  if (rs != -1 && rs != 0 && rs != 16) {
    bool newRsHigh = rs >= 16;
    if (rsHigh != newRsHigh) {
      rsHigh = newRsHigh;
      update = true;
    }
  }
  if (rt != -1 && rt != 0 && rt != 16) {
    bool newRtHigh = rt >= 16;
    if (rtHigh != newRtHigh) {
      rtHigh = newRtHigh;
      update = true;
    }
  }
  if (rd != -1 && rd != 0 && rd != 16) {
    bool newRdHigh = rd >= 16;
    if (rdHigh != newRdHigh) {
      rdHigh = newRdHigh;
      update = true;
    }
  }
  if (update) {
    sSetrf(
      (rsHigh ?0x1 :0x0) |
      (rtHigh ?0x2 :0x0) |
      (rdHigh ?0x4 :0x0)
    );
  }
}

size_t VpuIsa::encodeFs() {
  return (fs0 ?0x1 :0x0) |
         (fs1 ?0x2 :0x0) |
         (fs2 ?0x4 :0x0) |
         (fs3 ?0x8 :0x0) |
         ((0x3 & fs45) << 4) |
         ((0x3 & fs67) << 6);
}

void VpuIsa::resetFs(bool force) {
  if (force || fs0 || fs1 || fs2 || fs3 || fs45 || fs67) {
    fs0 = false;
    fs1 = false;
    fs2 = false;
    fs3 = false;
    fs45 = 0;
    fs67 = 0;
    setFs(encodeFs());
  }

  if (force || rsHigh || rdHigh || rtHigh)
    updateRfSel(1, 1, 1);
}

void VpuIsa::updateFs(DataType dtype, AluType alu) {
  bool newFs0(fs0);
  bool newFs1(fs1);
  bool newFs2(fs2);
  bool newFs3(fs3);

  switch (dtype) {
    case DataType::DTYPE_INT8:    newFs1 = 0; newFs2 = 0; break;
    case DataType::DTYPE_UINT8:   newFs1 = 0; newFs2 = 0; break;
    case DataType::DTYPE_BF16:    newFs1 = 0; newFs2 = 0; break;
    case DataType::DTYPE_FP32:    newFs1 = 0; newFs2 = 0; break;
    case DataType::DTYPE_INT16:   newFs1 = 0; newFs2 = 1; break;
    case DataType::DTYPE_INT32:   newFs1 = 0; newFs2 = 1; break;
    case DataType::DTYPE_FP8_52:  newFs1 = 0; newFs2 = 1; break;
    case DataType::DTYPE_FP8_43:  newFs1 = 0; newFs2 = 1; break;
    case DataType::DTYPE_FP16:    newFs1 = 1; newFs2 = 0; break;
    case DataType::DTYPE_FP64:    newFs1 = 1; newFs2 = 0; break;
    case DataType::DTYPE_INT4:    newFs1 = 1; newFs2 = 1; break;
    case DataType::DTYPE_INT64:   newFs1 = 1; newFs2 = 1; break;
    case DataType::DTYPE_FP4:     newFs1 = 1; newFs2 = 1; break;
    default:
      break;
  }

  switch (alu) {
    case AluType::FP32_ALU:   newFs3 = 0; break;
    case AluType::INT32_ALU:  newFs3 = 0; break;
    case AluType::FP64_ALU:   newFs3 = 1; break;
    case AluType::INT64_ALU:  newFs3 = 1; break;
    default:
      break;
  }

  switch (alu) {
    case AluType::INT32_ALU:  newFs0 = 0; break;
    case AluType::INT64_ALU:  newFs0 = 0; break;
    case AluType::FP32_ALU:   newFs0 = 1; break;
    case AluType::FP64_ALU:   newFs0 = 1; break;
    default:
      break;
  }

  if (newFs0 != fs0 || newFs1 != fs1 || newFs2 != fs2 || newFs3 != fs3) {
    fs0 = newFs0;
    fs1 = newFs1;
    fs2 = newFs2;
    fs3 = newFs3;
    setFs(encodeFs());
  }
}

void VpuIsa::addIsa(std::shared_ptr<VpuInstr> instr) {
  auto isRedundantIsa = [this] (std::shared_ptr<VpuInstr> i) {
    if (std::dynamic_pointer_cast<ScalarInstr>(i)) {
      redundantRemoverEn = false;
      loadDesc.stride = -1;
      loadDesc.cmsk = -1;
      loadDesc.bmsk = -1;
      loadDesc.cbrd = -1;
      loadDesc.bbrd = -1;
      loadDesc.base = -1;
      loadDesc.dequan = -1;
      loadDesc2.base = -1;
      loadDesc2.dequan = -1;
      storeDesc.base = -1;
      storeDesc.stride = -1;
      storeDesc.cmsk = -1;
      storeDesc.bmsk = -1;
      storeDesc.quan = -1;
      loadDesc.bbrd = -1;
    }

    if (auto x = std::dynamic_pointer_cast<SetLoadBaseInstr>(i)) {
      if (loadDesc.base == x->getBaseAddr())
        return true;
      loadDesc.base = x->getBaseAddr();
    } else if (auto x = std::dynamic_pointer_cast<SetLoad2BaseInstr>(i)) {
      if (loadDesc2.base == x->getBaseAddr())
        return true;
      loadDesc2.base = x->getBaseAddr();
    } else if (auto x = std::dynamic_pointer_cast<SetStoreBaseInstr>(i)) {
      if (storeDesc.base == x->getBaseAddr()) 
        return true;
      storeDesc.base = x->getBaseAddr();
    } else if (auto x = std::dynamic_pointer_cast<SetDequantizeInstr>(i)) {
      if (loadDesc.dequan == x->getDequantize())
        return true;
      loadDesc.dequan = x->getDequantize();
    } else if (auto x = std::dynamic_pointer_cast<SetDequantize2Instr>(i)) {
      if (loadDesc2.dequan == x->getDequantize())
        return true;
      loadDesc2.dequan = x->getDequantize();
    } else if (auto x = std::dynamic_pointer_cast<SetQuantizeInstr>(i)) {
      if (storeDesc.quan == x->getQuantize())
        return true;
      storeDesc.quan = x->getQuantize();
    } else if (auto x = std::dynamic_pointer_cast<SetLoadGlbInstr>(i)) {
      if (loadDesc.stride == x->getStride() &&
        loadDesc.cmsk == x->getClusterMask() &&
        loadDesc.bmsk == x->getBankMask() &&
        loadDesc.cbrd == x->getClusterBroadcast() &&
        loadDesc.bbrd == x->getBankBroadcast() &&
        loadDesc.chbrd == x->getChannelBroadcast())
        return true;
      loadDesc.stride = x->getStride();
      loadDesc.cmsk = x->getClusterMask();
      loadDesc.bmsk = x->getBankMask();
      loadDesc.cbrd = x->getClusterBroadcast();
      loadDesc.bbrd = x->getBankBroadcast();
      loadDesc.chbrd = x->getChannelBroadcast();
    } else if (auto x = std::dynamic_pointer_cast<SetStoreGlbInstr>(i)) {
      if (storeDesc.stride == x->getStride() &&
        storeDesc.cmsk == x->getClusterMask() &&
        storeDesc.bmsk == x->getBankMask())
        return true;
      storeDesc.stride = x->getStride();
      storeDesc.cmsk = x->getClusterMask();
      storeDesc.bmsk = x->getBankMask();
    }
    return false;
  };

  if (isRedundantIsa(instr) == false)
    instrs.push_back(instr);
}

void VpuIsa::removeRedundantCmds() {
  auto isLoad = [] (std::shared_ptr<VpuInstr> next) {
    return std::dynamic_pointer_cast<LoadInstr>(next) ||
           std::dynamic_pointer_cast<LoadComputeInstr>(next) ||
           std::dynamic_pointer_cast<LoadComputeStoreInstr>(next) ||
           std::dynamic_pointer_cast<DloadInstr>(next) ||
           std::dynamic_pointer_cast<DloadComputeInstr>(next) ||
           std::dynamic_pointer_cast<DloadComputeStoreInstr>(next);
  };
  auto isDload = [] (std::shared_ptr<VpuInstr> next) {
    return std::dynamic_pointer_cast<DloadInstr>(next) ||
           std::dynamic_pointer_cast<DloadComputeInstr>(next) ||
           std::dynamic_pointer_cast<DloadComputeStoreInstr>(next);
  };
  auto isStore = [] (std::shared_ptr<VpuInstr> next) {
    return std::dynamic_pointer_cast<StoreInstr>(next) ||
           std::dynamic_pointer_cast<ComputeStoreInstr>(next) ||
           std::dynamic_pointer_cast<LoadComputeStoreInstr>(next) ||
           std::dynamic_pointer_cast<DloadComputeStoreInstr>(next);
  };

  const std::map<VpuInstr::OpCodeType, std::function<bool(std::shared_ptr<VpuInstr>)>> relativityMap = {
    { VpuInstr::OpCodeType::SET_LOAD_BASE, isLoad },
    { VpuInstr::OpCodeType::SET_LOAD2_BASE, isDload },
    { VpuInstr::OpCodeType::SET_STORE_BASE, isStore },
    { VpuInstr::OpCodeType::SET_LOAD_GLB, isLoad },
    { VpuInstr::OpCodeType::SET_STORE_GLB, isStore },
    { VpuInstr::OpCodeType::SET_QUANTIZE, isStore },
    { VpuInstr::OpCodeType::SET_DEQUANTIZE, isLoad },
    { VpuInstr::OpCodeType::SET_DEQUANTIZE2, isDload }
  };

  for(auto it = instrs.begin(); it != instrs.end(); ) {
    bool removeFlag = false;
    auto instr = std::dynamic_pointer_cast<VpuInstr>(*it);
    if (redundantRemoverEn) {
      if (relativityMap.find(instr->getOpCode()) != relativityMap.end()) {
        for(auto k = (it+1); k != instrs.end(); k++) {
          auto instr2 = std::dynamic_pointer_cast<VpuInstr>(*k);
          if (instr->getOpCode() == instr2->getOpCode() || std::dynamic_pointer_cast<VpuEndInstr>(instr2)) {
            removeFlag = true;
            break;
          } else if (relativityMap.at(instr->getOpCode())(instr2)) {
            break;
          }
        }
      }
    }
    it = removeFlag ?(instrs.erase(it)) :(it+1);
  }

  if (!instrs.empty() && std::dynamic_pointer_cast<VpuEndInstr>(instrs.back()))
    while ((instrs.size() % (kIduTransGranularity/kSizePerVpuCmd)) != 0)
        instrs.insert(instrs.end()-1, VpuInstr::create<VpuNopInstr>({}));
}

size_t VpuIsa::getCmdSize() {
  removeRedundantCmds();
  return instrs.size();
}

size_t VpuIsa::evalCycles() {
  removeRedundantCmds();
  return evalCycles(instrs);
}

size_t VpuIsa::evalCycles(const std::vector<std::shared_ptr<spu::InstrInterface>> &instrs) const {
  bool rsHigh = false;
  bool rtHigh = false;
  bool rdHigh = false;
  bool flag = false;
  AaiConfig aai;
  std::vector<int32_t> regs(32, 0);
  size_t numCycle = 0, pc = 0;

  auto rs = [&] (size_t n) -> int32_t& { return regs[n + (rsHigh ?16 :0)]; };
  auto rt = [&] (size_t n) -> int32_t& { return regs[n + (rtHigh ?16 :0)]; };
  auto rd = [&] (size_t n) -> int32_t& { return regs[n + (rdHigh ?16 :0)]; };

  for(; numCycle != size_t(-1) && pc < instrs.size(); numCycle++) {
    auto instr = std::dynamic_pointer_cast<VpuInstr>(instrs[pc]);
    if (std::dynamic_pointer_cast<VpuEndInstr>(instr)) {
      numCycle++;
      break;
    }
    auto &r15 = regs[15];
    size_t lastPc = pc;

    if (auto x = std::dynamic_pointer_cast<SAddiInstr>(instr)) {
      rt(x->getRt()) = rs(x->getRs()) + (int16_t)x->getImm();
    } else if (auto x = std::dynamic_pointer_cast<SAddiuInstr>(instr)) {
      rt(x->getRt()) = rs(x->getRs()) + (int16_t)x->getImm();
    } else if (auto x = std::dynamic_pointer_cast<SSubiInstr>(instr)) {
      rt(x->getRt()) = rs(x->getRs()) - (int16_t)x->getImm();
    } else if (auto x = std::dynamic_pointer_cast<SAndiInstr>(instr)) {
      rt(x->getRt()) = rs(x->getRs()) & (uint32_t)((uint16_t)x->getImm());
    } else if (auto x = std::dynamic_pointer_cast<SOriInstr>(instr)) {
      rt(x->getRt()) = rs(x->getRs()) | (uint32_t)((uint16_t)x->getImm());
    } else if (auto x = std::dynamic_pointer_cast<SXoriInstr>(instr)) {
      rt(x->getRt()) = rs(x->getRs()) ^ (uint32_t)((uint16_t)x->getImm());
    } else if (auto x = std::dynamic_pointer_cast<SSgtiInstr>(instr)) {
      flag = rs(x->getRs()) > (int16_t)x->getImm();
      rt(x->getRt()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSltiInstr>(instr)) {
      flag = rs(x->getRs()) < (int16_t)x->getImm();
      rt(x->getRt()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSeqiInstr>(instr)) {
      flag = rs(x->getRs()) == (int16_t)x->getImm();
      rt(x->getRt()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSgeiInstr>(instr)) {
      flag = rs(x->getRs()) >= (int16_t)x->getImm();
      rt(x->getRt()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSleiInstr>(instr)) {
      flag = rs(x->getRs()) <= (int16_t)x->getImm();
      rt(x->getRt()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSraiInstr>(instr)) {
      rt(x->getRt()) = (int32_t)rs(x->getRs()) >> x->getImm();
    } else if (auto x = std::dynamic_pointer_cast<SSrliInstr>(instr)) {
      rt(x->getRt()) = (uint32_t)rs(x->getRs()) >> x->getImm();
    } else if (auto x = std::dynamic_pointer_cast<SSlliInstr>(instr)) {
      rt(x->getRt()) = (uint32_t)rs(x->getRs()) << x->getImm();
    } else if (auto x = std::dynamic_pointer_cast<SLuiInstr>(instr)) {
      rt(x->getRt()) = (uint32_t)x->getImm() << 16;
    } else if (auto x = std::dynamic_pointer_cast<SAddInstr>(instr)) {
      rd(x->getRd()) = rs(x->getRs()) + rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SAdduInstr>(instr)) {
      rd(x->getRd()) = rs(x->getRs()) + rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SSubInstr>(instr)) {
      rd(x->getRd()) = rs(x->getRs()) - rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SMulInstr>(instr)) {
      rd(x->getRd()) = rs(x->getRs()) * rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SMulhInstr>(instr)) {
      rd(x->getRd()) = ((int64_t)rs(x->getRs()) * (int64_t)rt(x->getRs())) >> 32;
    } else if (auto x = std::dynamic_pointer_cast<SMulhuInstr>(instr)) {
      rd(x->getRd()) = ((int64_t)rs(x->getRs()) * (int64_t)rt(x->getRs())) >> 32;
    } else if (auto x = std::dynamic_pointer_cast<SMinInstr>(instr)) {
      rd(x->getRd()) = std::min(rs(x->getRs()), rt(x->getRs()));     
    } else if (auto x = std::dynamic_pointer_cast<SMinuInstr>(instr)) {
      rd(x->getRd()) = std::min(rs(x->getRs()), rt(x->getRs()));
    } else if (auto x = std::dynamic_pointer_cast<SMaxInstr>(instr)) {
      rd(x->getRd()) = std::max(rs(x->getRs()), rt(x->getRs())); 
    } else if (auto x = std::dynamic_pointer_cast<SMaxuInstr>(instr)) {
      rd(x->getRd()) = std::max(rs(x->getRs()), rt(x->getRs()));
    } else if (auto x = std::dynamic_pointer_cast<SAndInstr>(instr)) {
      rd(x->getRd()) = rs(x->getRs()) & rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SOrInstr>(instr)) {
      rd(x->getRd()) = rs(x->getRs()) | rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SXorInstr>(instr)) {
      rd(x->getRd()) = rs(x->getRs()) ^ rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SSgtInstr>(instr)) {
      flag = rs(x->getRs()) > rt(x->getRs());     rd(x->getRd()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSltInstr>(instr)) {
      flag = rs(x->getRs()) < rt(x->getRs());     rd(x->getRd()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSeqInstr>(instr)) {
      flag = rs(x->getRs()) == rt(x->getRs());    rd(x->getRd()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSgeInstr>(instr)) {
      flag = rs(x->getRs()) >= rt(x->getRs());    rd(x->getRd()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SSleInstr>(instr)) {
      flag = rs(x->getRs()) <= rt(x->getRs());    rd(x->getRd()) = flag ?1 :0;
    } else if (auto x = std::dynamic_pointer_cast<SCmpselInstr>(instr)) {
      rd(x->getRd()) = flag ?rs(x->getRs()) :rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SSraInstr>(instr)) {
      rd(x->getRd()) = (int32_t)rs(x->getRs()) >> rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SSrlInstr>(instr)) {
      rd(x->getRd()) = (uint32_t)rs(x->getRs()) >> rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SSllInstr>(instr)) {
      rd(x->getRd()) = (uint32_t)rs(x->getRs()) << rt(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SJumpInstr>(instr)) {
      pc = x->getOffset();
    } else if (auto x = std::dynamic_pointer_cast<SJalInstr>(instr)) {
      r15 = pc;
      pc = x->getOffset();
    } else if (auto x = std::dynamic_pointer_cast<SJrInstr>(instr)) {
      pc = rs(x->getRs());
    } else if (auto x = std::dynamic_pointer_cast<SBneInstr>(instr)) {
      pc += (rs(x->getRs()) != rt(x->getRs())) ?x->getOffset() :0;
    } else if (auto x = std::dynamic_pointer_cast<SBeqInstr>(instr)) {
      pc += (rs(x->getRs()) == rt(x->getRs())) ?x->getOffset() :0;
    } else if (auto x = std::dynamic_pointer_cast<SBltInstr>(instr)) {
      pc += (rs(x->getRs()) < rt(x->getRs())) ?x->getOffset() :0;
    } else if (auto x = std::dynamic_pointer_cast<SBltuInstr>(instr)) {
      pc += (rs(x->getRs()) < rt(x->getRs())) ?x->getOffset() :0;
    } else if (auto x = std::dynamic_pointer_cast<SBgeInstr>(instr)) {
      pc += (rs(x->getRs()) >= rt(x->getRs())) ?x->getOffset() :0;
    } else if (auto x = std::dynamic_pointer_cast<SBgeuInstr>(instr)) {
      pc += (rs(x->getRs()) >= rt(x->getRs())) ?x->getOffset() :0;
    } else if (auto x = std::dynamic_pointer_cast<SSetcfgInstr>(instr)) {
    } else if (auto x = std::dynamic_pointer_cast<SSetrfInstr>(instr)) {
      rsHigh = (instr->getRfSel() & 0x1) ?true :false;
      rtHigh = (instr->getRfSel() & 0x2) ?true :false;
      rdHigh = (instr->getRfSel() & 0x4) ?true :false;
    } else if (auto x = std::dynamic_pointer_cast<SetAaiInstr>(instr)) {
      aai.enable = instr->getEnable();
      aai.stride = (instr->getStrideLength() >> (instr->getMode()+1)) + 1;
      aai.length = (instr->getStrideLength() & ((1 << (instr->getMode()+1))-1)) + 1;
    } else if (auto x = std::dynamic_pointer_cast<DloadComputeStoreInstr>(instr)) {
      if (aai.enable)
        numCycle += aai.length - 1;
    } else if (auto x = std::dynamic_pointer_cast<LoadComputeStoreInstr>(instr)) {
      if (aai.enable)
        numCycle += aai.length - 1;
    }
    regs[0] = regs[16] = 0;
    if (lastPc == pc) {
      pc++;
    } else {
      pc = getPc(pc);
      CHECK(pc < instrs.size());
    }
  }
  return numCycle;
}

const std::vector<std::shared_ptr<spu::InstrInterface>>& VpuIsa::getInstrs(bool end) {
  if (end == true) {
    auto instr = instrs.empty() ?nullptr :std::dynamic_pointer_cast<VpuInstr>(instrs.back());
    if (instr == nullptr || !std::dynamic_pointer_cast<VpuEndInstr>(instr)) {
      vpuNop(); vpuNop(); // op_end can not follow jump/branch instruction
      vpuEnd();
    }
  }
  removeRedundantCmds();
  return instrs;
}

bool VpuIsa::loopExists() {
  for (auto &it : instrs) {
    if (std::dynamic_pointer_cast<SJumpInstr>(it) ||
        std::dynamic_pointer_cast<SJalInstr>(it) || 
        std::dynamic_pointer_cast<SJrInstr>(it) || 
        std::dynamic_pointer_cast<SBneInstr>(it) || 
        std::dynamic_pointer_cast<SBeqInstr>(it) || 
        std::dynamic_pointer_cast<SBltInstr>(it) || 
        std::dynamic_pointer_cast<SBltuInstr>(it) || 
        std::dynamic_pointer_cast<SBgeInstr>(it) || 
        std::dynamic_pointer_cast<SBgeuInstr>(it))
      return true;
  }
  return false;
}

void VpuIsa::getSrcBaseTable(std::vector<std::pair<uint32_t,uint32_t>> &baseTable, const std::map<uint32_t,uint32_t> &baseFilter) {
  for(size_t i = 0; i < instrs.size(); i++) {
    auto instr = std::dynamic_pointer_cast<SetLoadBaseInstr>(instrs[i]);
    if (instr && baseFilter.find(instr->getBaseAddr()) != baseFilter.end()) {
      auto bin8 = instr->toBinary();
      uint32_t newBase = (*(uint32_t*)bin8.data()) - (instr->getBaseAddr() - baseFilter.at(instr->getBaseAddr()));
      baseTable.push_back(std::pair<uint32_t,uint32_t>((i*4), newBase));
    }
  }
}

void VpuIsa::getDstBaseTable(std::vector<std::pair<uint32_t,uint32_t>> &baseTable, const std::map<uint32_t,uint32_t> &baseFilter) {
  for(size_t i = 0; i < instrs.size(); i++) {
    auto instr = std::dynamic_pointer_cast<SetStoreBaseInstr>(instrs[i]);
    if (instr && baseFilter.find(instr->getBaseAddr()) != baseFilter.end()) {
      auto bin8 = instr->toBinary();
      uint32_t newBase = (*(uint32_t*)bin8.data()) - (instr->getBaseAddr() - baseFilter.at(instr->getBaseAddr()));
      baseTable.push_back(std::pair<uint32_t,uint32_t>((i*4), newBase));
    }
  }
}

}