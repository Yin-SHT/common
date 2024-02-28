/*
 * Filename: te_converter.h
 * Author: Changxu Zhang
 * Description: Transpose Eninge Command Converter Header file
 */
#ifndef __TE_CONVERTER_H
#define __TE_CONVERTER_H

#include "converter_base.h"

typedef std::vector<uint64_t> TEBinary_t;

namespace antoum {
class TEConverter : public TEConverter_t {
public:
    TEConverter(std::shared_ptr<MFPara> para=nullptr);
    virtual ~TEConverter() = default;
    virtual bool SetText(std::string text);
    virtual std::string GetInstructionText();
    virtual std::vector<std::string> GetConfigurationText(int config_idx);
    virtual std::vector<std::pair<uint32_t,uint32_t>> GetConfigurationRegs(int config_idx);
    virtual bool SetBinary(TEBinary_t bin);
    virtual TEBinary_t GetBinary();
    virtual bool SetInstruction(std::shared_ptr<TEInstruction> instr);
    virtual bool SetConfiguration(std::shared_ptr<TEConfiguration> config);
    virtual std::shared_ptr<TEInstruction> GetInstruction();
    virtual std::shared_ptr<TEConfiguration> GetConfiguration();
    virtual void Dump(std::string file_path);
    virtual std::string GetDetail();
public:
    virtual bool SettingArgs(std::string opcode, const std::vector<uint32_t> &args);
    virtual bool SetOpcode(std::string opcode);

    virtual ConverterBase* LoadText(std::string text);
    virtual std::string ToText();
    virtual ConverterBase* LoadBinary(const std::vector<uint8_t> &bin);
    virtual std::vector<uint8_t> ToBinary();
    virtual ConverterBase* LoadInstr(std::shared_ptr<InstrBase> instr);
    virtual std::shared_ptr<InstrBase> ToInstr();

protected:
    std::shared_ptr<TEInstruction> _instr;
    std::shared_ptr<TEConfiguration> _config;
    TEBinary_t* _bin;
    
protected:
    std::map<std::string,std::vector<std::pair<std::string,uint32_t>>>te_isa_namespace;
};
}
#endif
