#ifndef __GCPY_CONVERTER_H
#define __GCPY_CONVERTER_H

#include "converter_base.h"

typedef std::vector<uint64_t> GCPYBinary_t;

namespace antoum {
class GCPYConverter : public GCPYConverter_t {
  public:
    GCPYConverter(std::shared_ptr<MFPara> para=nullptr);
    GCPYConverter(std::string str);
    GCPYConverter(GCPYBinary_t bin);
    GCPYConverter(std::shared_ptr<GCPYInstruction> instr);
    virtual ~GCPYConverter() = default;
    virtual bool SetByString(std::string str);
    virtual std::string GetString();

    virtual bool SetByInstruction(std::shared_ptr<GCPYInstruction> instr);
    virtual std::shared_ptr<GCPYInstruction>  GetInstruction();

    virtual bool SetBinary(GCPYBinary_t bin);
    virtual GCPYBinary_t GetBinary();

    virtual TensorDesc GetSrcDesc();
    virtual TensorDesc GetDstDesc();
    virtual void SetSrcDesc(TensorDesc tsr_desc);
    virtual void SetDstDesc(TensorDesc tsr_desc);

    virtual void Dump(std::string file_path);
    virtual bool SettingArgs(std::string opcode, const std::vector<uint32_t> &args);
    virtual std::string GetDetail();

    virtual ConverterBase* LoadText(std::string text);
    virtual std::string ToText();
    virtual ConverterBase* LoadBinary(const std::vector<uint8_t> &bin);
    virtual std::vector<uint8_t> ToBinary();
    virtual ConverterBase* LoadInstr(std::shared_ptr<InstrBase> instr);
    virtual std::shared_ptr<InstrBase> ToInstr();
    
  protected:
    std::map<std::string,std::vector<std::pair<std::string,uint32_t>>> gcpy_isa_namespace;
    std::shared_ptr<GCPYInstruction> _instr;
    GCPYBinary_t* _bin;
    
};

#define Setting(cvter, ...) cvter.SettingArgs(__VA_ARGS__)
#define SetGCPYInstr(cvter,params) Setting(cvter,"GLBCPY",params)

}
#endif
