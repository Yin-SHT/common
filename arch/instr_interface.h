/*
 * Copyright (c) 2018-2023 MOFFETT AI. All Rights Reserved.

 * NOTICE: All intellectual and technical information contained herein
 * are proprietary to MOFFETT AI. Any unauthorized disemination,
 * copying or redistribution of this file via any medium is strictly prohibited,
 * unless you get a prior written permission or an applicable license agreement
 * from MOFFETT AI.
 */

#ifndef SPU_INSTR_INTERFACE_H
#define SPU_INSTR_INTERFACE_H

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

namespace spu {

class InstrInterface {
public:
  virtual ~InstrInterface() = default;
  virtual bool loadText(const std::string& text) = 0;
  virtual const std::string& toText() = 0;
  virtual bool loadBinary(const std::vector<uint8_t>& bin) = 0;
  virtual const std::vector<uint8_t>& toBinary() = 0;
  virtual std::shared_ptr<InstrInterface> clone() = 0;
};

} // namespace spu

#endif // SPU_INSTR_INTERFACE_H
