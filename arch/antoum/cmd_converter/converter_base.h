#ifndef __CONVERTER_BASE_H
#define __CONVERTER_BASE_H

#include "utils.h"
#include "hw_config.h"
#include "instruction.h"

class ConverterBase : public MFObject {
public:
    ConverterBase(std::shared_ptr<MFPara> para) : MFObject(para) {}
    virtual ~ConverterBase() = default;
    virtual ConverterBase* LoadText(std::string text) = 0;
    virtual std::string ToText() = 0;
    virtual ConverterBase* LoadBinary(const std::vector<uint8_t> &bin) = 0;
    virtual std::vector<uint8_t> ToBinary() = 0;
    virtual ConverterBase* LoadInstr(std::shared_ptr<InstrBase> instr) = 0;
    virtual std::shared_ptr<InstrBase> ToInstr() = 0;
    virtual uint32_t SliceBits(int length, int low, uint64_t data) {
        uint64_t mask = pow(2,length) -1;
        mask = mask<<low;
        uint32_t sliced_data = (data&mask)>>low;
        assert(sliced_data < (1ULL<<length));
        return sliced_data;
    };
};

class AEConverter_t : public ConverterBase {
    public:
        AEConverter_t(std::shared_ptr<MFPara> para) : ConverterBase(para) {};
};
class DMAConverter_t : public ConverterBase {
    public:
        DMAConverter_t(std::shared_ptr<MFPara> para) : ConverterBase(para) {};
};
class GCPYConverter_t : public ConverterBase {
    public:
        GCPYConverter_t(std::shared_ptr<MFPara> para) : ConverterBase(para) {};
};
class PEAConverter_t : public ConverterBase {
    public:
        PEAConverter_t(std::shared_ptr<MFPara> para) : ConverterBase(para) {};
};
class TEConverter_t : public ConverterBase {
    public:
        TEConverter_t(std::shared_ptr<MFPara> para) : ConverterBase(para) {};
};
class TKEConverter_t : public ConverterBase {
    public:
        TKEConverter_t(std::shared_ptr<MFPara> para) : ConverterBase(para) {};
};

#endif
