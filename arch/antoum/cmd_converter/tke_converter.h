/*
 * Filename: tke_converter.h
 * Author: Changxu Zhang
 * Description: TopK Eninge Command Converter Header file
 */
#ifndef __TKE_CONVERTER_H
#define __TKE_CONVERTER_H

#include "converter_base.h"

typedef std::vector<uint64_t> TKEBinary_t;

namespace antoum {
class TKEConverter : public TKEConverter_t {
public:
    TKEConverter(std::shared_ptr<MFPara> para=nullptr);
    virtual ~TKEConverter() = default;
    virtual bool SetText(std::string text);
    virtual std::string GetInstructionText();
    virtual bool SetBinary(TKEBinary_t bin);
    virtual TKEBinary_t GetBinary();
    virtual bool SetInstruction(std::shared_ptr<TKEInstruction>);
    virtual std::shared_ptr<TKEInstruction> GetInstruction();
    virtual void Dump(std::string file_path);
    virtual std::string GetDetail();
public:
    virtual bool SettingArgs(std::string opcode, const std::vector<uint32_t> &args);

    virtual ConverterBase* LoadText(std::string text);
    virtual std::string ToText();
    virtual ConverterBase* LoadBinary(const std::vector<uint8_t> &bin);
    virtual std::vector<uint8_t> ToBinary();
    virtual ConverterBase* LoadInstr(std::shared_ptr<InstrBase> instr);
    virtual std::shared_ptr<InstrBase> ToInstr();

protected:
    std::shared_ptr<TKEInstruction> _instr; 
    TKEBinary_t* _bin;
    std::map<std::string,std::vector<std::pair<std::string,std::uint32_t>>> tke_isa_namespace;
};
}

#endif
