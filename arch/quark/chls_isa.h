/*
 * Copyright (c) 2018-2024 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef QUARK_CHLS_ISA_H
#define QUARK_CHLS_ISA_H

#include "utils.h"
#include "quark/chls_instr.h"

namespace quark {

class ChlsIsa {
public:
  explicit ChlsIsa() : callCounts(4, 0) {}

  virtual ~ChlsIsa() = default;

  virtual std::vector<std::shared_ptr<chls::ChlsInstr>> &getInstrs() {
    while ((instrs.size() % (kIduTransGranularity/kSizePerChlsCmd)) != 0) {
      auto pos = instrs.end() - (std::dynamic_pointer_cast<chls::EndInstr>(instrs.back()) ?1 :0);
      instrs.insert(pos, chls::ChlsInstr::create<chls::NopInstr>({}));
    }
    return instrs;
  }

  virtual size_t evalCycles() {
    return instrs.size();
  }

  void nop() {
    instrs.push_back(chls::ChlsInstr::create<chls::NopInstr>({}));
  }

  void str() {
    instrs.push_back(chls::ChlsInstr::create<chls::StrInstr>({}));
  }

  void end() {
    instrs.push_back(chls::ChlsInstr::create<chls::EndInstr>({0}));
  }

  void enb(chls::ChlsInstr::EnableType type, const std::set<size_t>& cores) {
    auto instr = std::make_shared<chls::EnbInstr>();
    instr->setEnType((size_t)type);
    for (auto &it : cores)
      switch (it) {
        case 0: instr->setCore0(1); break;
        case 1: instr->setCore1(1); break;
        case 2: instr->setCore2(1); break;
        case 3: instr->setCore3(1); break;
        default:
          break;
      }
    instrs.push_back(instr);
  }

  void wfi(chls::ChlsInstr::InterruptType type, const std::set<size_t>& cores) {
    auto instr = std::make_shared<chls::WfiInstr>();
    instr->setIrType((size_t)type);
    for (auto &it : cores)
      switch (it) {
        case 0: instr->setCore0(1); break;
        case 1: instr->setCore1(1); break;
        case 2: instr->setCore2(1); break;
        case 3: instr->setCore3(1); break;
        default:
          break;
      }
    instrs.push_back(instr);
  }

  void call(size_t com) {
    instrs.push_back(chls::ChlsInstr::create<chls::CallInstr>({com, callCounts[com]}));
    callCounts[com]++;
  }

  void tms(size_t id) {
    instrs.push_back(chls::ChlsInstr::create<chls::TmsInstr>({id}));
  }

  void tme(size_t id) {
    instrs.push_back(chls::ChlsInstr::create<chls::TmeInstr>({id}));
  }

  void enbInstr(const std::set<size_t>& cores) {
    enb(chls::ChlsInstr::EnableType::Instr, cores);
  }

  void enbCall(const std::set<size_t>& engines) {
    call(0);
  }

  void enbCore(const std::set<size_t>& cores) {
    enb(chls::ChlsInstr::EnableType::Engine, cores);
  }

  void wfiInstr(const std::set<size_t>& cores) {
    wfi(chls::ChlsInstr::InterruptType::Instr, cores);
  }

  void wfiCall(const std::set<size_t>& engines) {
    wfi(chls::ChlsInstr::InterruptType::RVV_Com, {0});
  }

  void wfiCore(const std::set<size_t>& cores) {
    wfi(chls::ChlsInstr::InterruptType::Engine, cores);
  }

protected:
  std::vector<std::shared_ptr<chls::ChlsInstr>> instrs;
  std::vector<size_t> callCounts;
};

} // namespace quark

#endif // QUARK_CHLS_ISA_H
