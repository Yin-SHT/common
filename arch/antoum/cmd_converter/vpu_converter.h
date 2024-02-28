#ifndef __VPU_CONVERTER_H
#define __VPU_CONVERTER_H

#include "converter_base.h"

typedef std::vector<uint8_t> VPUBinary_t;


struct VPUInstrFormat {
    std::vector<std::string> names;
    std::map<std::string, uint32_t> fixed_fields;
};

namespace antoum {
class VPUConverter : public VPUConverter_t {
public:
    VPUConverter(std::shared_ptr<MFPara> para);
    virtual ~VPUConverter() = default;
    virtual ConverterBase* SetText(std::string name, const std::vector<int> &args);
    virtual ConverterBase* SetText(std::string text);
    virtual std::string GetText();

    virtual bool SetBinary(VPUBinary_t bin);
    virtual VPUBinary_t GetBinary();
    virtual bool Check();

    virtual ConverterBase* LoadText(std::string text);
    virtual std::string ToText();
    virtual ConverterBase* LoadBinary(const std::vector<uint8_t> &bin);
    virtual std::vector<uint8_t> ToBinary();
    virtual ConverterBase* LoadInstr(std::shared_ptr<InstrBase> instr);
    virtual std::shared_ptr<InstrBase> ToInstr();

protected:
    virtual std::map<std::string, std::vector<VPUInstrFormat*>> *GetNameInstrMap();
    virtual std::map<VPUOpcode, std::vector<VPUInstrFormat*>> *GetOpInstrMap();

public:
    std::shared_ptr<VPUInstruction> _instr;
};
}
#endif
