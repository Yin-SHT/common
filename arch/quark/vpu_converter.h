#ifndef __VPU_CONVERTER_QUARK_H
#define __VPU_CONVERTER_QUARK_H

#include "antoum/cmd_converter/vpu_converter.h"
#include "quark/vpu_instr.h"

namespace quark {
class VPUConverter : public antoum::VPUConverter {
public:
    VPUConverter(std::shared_ptr<MFPara> para);
    virtual ~VPUConverter() = default;
    virtual bool Check();

    virtual ConverterBase* LoadText(std::string text);
    virtual std::string ToText();
    virtual ConverterBase* LoadBinary(const std::vector<uint8_t> &bin);
    virtual std::vector<uint8_t> ToBinary();

protected:
    virtual std::map<std::string, std::vector<VPUInstrFormat*>> *GetNameInstrMap();
    virtual std::map<VPUOpcode, std::vector<VPUInstrFormat*>> *GetOpInstrMap();

private:
    
};
}
#endif
