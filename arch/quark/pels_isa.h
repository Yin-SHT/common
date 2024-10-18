/*
 * Copyright (c) 2018-2024 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef QUARK_PELS_ISA_H
#define QUARK_PELS_ISA_H

#include "utils.h"
#include "quark/pels_instr.h"

namespace quark {

class PelsIsa {
public:
  explicit PelsIsa() : callCounts(4, 0) {}

  virtual ~PelsIsa() = default;

  virtual std::vector<std::shared_ptr<pels::PelsInstr>> &getInstrs() {
    while ((instrs.size() % (kIduTransGranularity/kSizePerPelsCmd)) != 0) {
      auto pos = instrs.end() - (std::dynamic_pointer_cast<pels::EndInstr>(instrs.back()) ?1 :0);
      instrs.insert(pos, pels::PelsInstr::create<pels::NopInstr>({}));
    }
    return instrs;
  }

  virtual size_t evalCycles() {
    return instrs.size();
  }

  void nop() {
    instrs.push_back(pels::PelsInstr::create<pels::NopInstr>({}));
  }

  void str() {
    instrs.push_back(pels::PelsInstr::create<pels::StrInstr>({}));
  }

  void end() {
    instrs.push_back(pels::PelsInstr::create<pels::EndInstr>({0}));
  }

  void enb(pels::PelsInstr::EnableType type, const std::set<size_t>& engines) {
    auto instr = std::make_shared<pels::EnbInstr>();
    instr->setEnType((size_t)type);
    for (auto &it : engines)
      switch (EngineType(it)) {
        case EngineType::SPU:     instr->setSpu(1);   break;
        case EngineType::VPU:     instr->setVpu(1);   break;
        case EngineType::AE:      instr->setAe(1);    break;
        case EngineType::TE:      instr->setTe(1);    break;
        case EngineType::SE:      instr->setSe(1);    break;
        case EngineType::DMA0:    instr->setDma0(1);  break;
        case EngineType::DMA1:    instr->setDma1(1);  break;
        case EngineType::DMA2:    instr->setDma2(1);  break;
        case EngineType::DMA3:    instr->setDma3(1);  break;
        case EngineType::DMA4:    instr->setDma4(1);  break;
        case EngineType::DMA5:    instr->setDma5(1);  break;
        default:
          break;
      }
    instrs.push_back(instr);
  }

  void wfi(pels::PelsInstr::InterruptType type, const std::set<size_t>& engines) {
    auto instr = std::make_shared<pels::WfiInstr>();
    instr->setIrType((size_t)type);
    for (auto &it : engines)
      switch (EngineType(it)) {
        case EngineType::SPU:     instr->setSpu(1);   break;
        case EngineType::VPU:     instr->setVpu(1);   break;
        case EngineType::AE:      instr->setAe(1);    break;
        case EngineType::TE:      instr->setTe(1);    break;
        case EngineType::SE:      instr->setSe(1);    break;
        case EngineType::DMA0:    instr->setDma0(1);  break;
        case EngineType::DMA1:    instr->setDma1(1);  break;
        case EngineType::DMA2:    instr->setDma2(1);  break;
        case EngineType::DMA3:    instr->setDma3(1);  break;
        case EngineType::DMA4:    instr->setDma4(1);  break;
        case EngineType::DMA5:    instr->setDma5(1);  break;
        case EngineType::PELS_I:  instr->setRvv0(1);  break;
        case EngineType::PELS_E:  instr->setRvv1(1);  break;
        case EngineType::PELS_O:  instr->setRvv2(1);  break;
        default:
          break;
      }
    instrs.push_back(instr);
  }

  void call(const std::set<size_t>& engines) {
    size_t com = 0;
    for (auto &it : engines)
      switch (EngineType(it)) {
        case EngineType::PELS_I:  com = 0;  break;
        case EngineType::PELS_E:  com = 1;  break;
        case EngineType::PELS_O:  com = 2;  break;
        default:
          break;
      }

    auto instr = std::make_shared<pels::CallInstr>();
    instr->setCom(com);
    instr->setFunc(callCounts[com]);
    instrs.push_back(instr);
    callCounts[com]++;
  }

  void tms(size_t id) {
    instrs.push_back(pels::PelsInstr::create<pels::TmsInstr>({id}));
  }

  void tme(size_t id) {
    instrs.push_back(pels::PelsInstr::create<pels::TmeInstr>({id}));
  }

  void enbInstr(const std::set<size_t>& engines) {
    enb(pels::PelsInstr::EnableType::Instr, engines);
  }

  void enbCall(const std::set<size_t>& engines) {
    call(engines);
  }

  void enbEngine(const std::set<size_t>& engines) {
    enb(pels::PelsInstr::EnableType::Engine, engines);
  }

  void wfiInstr(const std::set<size_t>& engines) {
    wfi(pels::PelsInstr::InterruptType::Instr, engines);
  }

  void wfiCall(const std::set<size_t>& engines) {
    wfi(pels::PelsInstr::InterruptType::RVV_Com, engines);
  }

  void wfiEngine(const std::set<size_t>& engines) {
    wfi(pels::PelsInstr::InterruptType::Engine, engines);
  }

  void lckIn() {
    instrs.push_back(pels::PelsInstr::create<pels::LckInstr>({0}));
  }

  void ulckIn() {
    instrs.push_back(pels::PelsInstr::create<pels::UlckInstr>({0}));
  }

  void wfiIn() {
    auto instr = std::make_shared<pels::WfiInstr>();
    instr->setIrType((size_t)pels::PelsInstr::InterruptType::Unlock);
    instr->setInBuf(1);
    instrs.push_back(instr);
  }

  void lckOut() {
    instrs.push_back(pels::PelsInstr::create<pels::LckInstr>({1}));
  }

  void ulckOut() {
    instrs.push_back(pels::PelsInstr::create<pels::UlckInstr>({1}));
  }

  void wfiOut() {
    auto instr = std::make_shared<pels::WfiInstr>();
    instr->setIrType((size_t)pels::PelsInstr::InterruptType::Unlock);
    instr->setOutBuf(1);
    instrs.push_back(instr);
  }

protected:
  bool hasCallOp(const std::set<size_t>& engines) {
    for (auto &it : engines)
      switch (EngineType(it)) {
        case EngineType::PELS_I:
        case EngineType::PELS_E:
        case EngineType::PELS_O:
          return true;
        default:
          break; 
      }
    return false;
  }

protected:
  std::vector<std::shared_ptr<pels::PelsInstr>> instrs;
  std::vector<size_t> callCounts;
};

} // namespace quark

#endif // QUARK_PELS_ISA_H
