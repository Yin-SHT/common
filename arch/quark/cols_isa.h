/*
 * Copyright (c) 2018-2024 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef QUARK_COLS_ISA_H
#define QUARK_COLS_ISA_H

#include "utils.h"
#include "quark/cols_instr.h"

namespace quark {

class ColsIsa {
public:
  explicit ColsIsa() : callCounts(4, 0) {}

  virtual ~ColsIsa() = default;

  virtual std::vector<std::shared_ptr<cols::ColsInstr>> &getInstrs() {
    while ((instrs.size() % (kIduTransGranularity/kSizePerColsCmd)) != 0) {
      auto pos = instrs.end() - (std::dynamic_pointer_cast<cols::EndInstr>(instrs.back()) ?1 :0);
      instrs.insert(pos, cols::ColsInstr::create<cols::NopInstr>({}));
    }
    return instrs;
  }

  virtual size_t evalCycles() {
    return instrs.size();
  }

  void nop() {
    instrs.push_back(cols::ColsInstr::create<cols::NopInstr>({}));
  }

  void str() {
    instrs.push_back(cols::ColsInstr::create<cols::StrInstr>({}));
  }

  void end() {
    instrs.push_back(cols::ColsInstr::create<cols::EndInstr>({0}));
  }

  void enb(cols::ColsInstr::EnableType type, const std::set<size_t>& pes) {
    auto instr = std::make_shared<cols::EnbInstr>();
    instr->setEnType((size_t)type);
    for (auto &it : pes)
      switch (it) {
        case 0:   instr->setPe0(1);     break;
        case 1:   instr->setPe1(1);     break;
        case 2:   instr->setPe2(1);     break;
        case 3:   instr->setPe3(1);     break;
        case 4:   instr->setPe4(1);     break;
        case 5:   instr->setPe5(1);     break;
        case 6:   instr->setPe6(1);     break;
        case 7:   instr->setPe7(1);     break;
        case 8:   instr->setPe8(1);     break;
        case 9:   instr->setPe9(1);     break;
        case 10:  instr->setPe10(1);    break;
        case 11:  instr->setPe11(1);    break;
        case 12:  instr->setPe12(1);    break;
        case 13:  instr->setPe13(1);    break;
        case 14:  instr->setPe14(1);    break;
        case 15:  instr->setPe15(1);    break;
        case 16:  instr->setCodmac0(1); break;
        case 17:  instr->setCodmac1(1); break;
        case 18:  instr->setCodmac2(1); break;
        default:
          break;
      }
    instrs.push_back(instr);
  }

  void wfi(cols::ColsInstr::InterruptType type, const std::set<size_t>& pes) {
    auto instr = std::make_shared<cols::WfiInstr>();
    instr->setIrType((size_t)type);
    for (auto &it : pes)
      switch (it) {
        case 0:   instr->setPe0(1);     break;
        case 1:   instr->setPe1(1);     break;
        case 2:   instr->setPe2(1);     break;
        case 3:   instr->setPe3(1);     break;
        case 4:   instr->setPe4(1);     break;
        case 5:   instr->setPe5(1);     break;
        case 6:   instr->setPe6(1);     break;
        case 7:   instr->setPe7(1);     break;
        case 8:   instr->setPe8(1);     break;
        case 9:   instr->setPe9(1);     break;
        case 10:  instr->setPe10(1);    break;
        case 11:  instr->setPe11(1);    break;
        case 12:  instr->setPe12(1);    break;
        case 13:  instr->setPe13(1);    break;
        case 14:  instr->setPe14(1);    break;
        case 15:  instr->setPe15(1);    break;
        case 16:  instr->setCodmac0(1); break;
        case 17:  instr->setCodmac1(1); break;
        case 18:  instr->setCodmac2(1); break;
        default:
          break;
      }
    instrs.push_back(instr);
  }

  void call(size_t com) {
    instrs.push_back(cols::ColsInstr::create<cols::CallInstr>({com, callCounts[com]}));
    callCounts[com]++;
  }

  void tms(size_t id) {
    instrs.push_back(cols::ColsInstr::create<cols::TmsInstr>({id}));
  }

  void tme(size_t id) {
    instrs.push_back(cols::ColsInstr::create<cols::TmeInstr>({id}));
  }

  void enbInstr(const std::set<size_t>& pes) {
    enb(cols::ColsInstr::EnableType::Instr, pes);
  }

  void enbCall(const std::set<size_t>& engines) {
    call(0);
  }

  void enbPe(const std::set<size_t>& pes) {
    enb(cols::ColsInstr::EnableType::Engine, pes);
  }

  void wfiInstr(const std::set<size_t>& pes) {
    wfi(cols::ColsInstr::InterruptType::Instr, pes);
  }

  void wfiCall(const std::set<size_t>& engines) {
    wfi(cols::ColsInstr::InterruptType::RVV_Com, {0});
  }

  void wfiPe(const std::set<size_t>& pes) {
    wfi(cols::ColsInstr::InterruptType::Engine, pes);
  }

protected:
  std::vector<std::shared_ptr<cols::ColsInstr>> instrs;
  std::vector<size_t> callCounts;
};

} // namespace quark

#endif // QUARK_COLS_ISA_H
