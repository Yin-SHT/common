/*
 * Filename: ae_converter.h
 * Author: Changxu Zhang
 * Description: Activation Eninge Command Converter Header file
 */
#ifndef __AE_CONVERTER_H
#define __AE_CONVERTER_H

#include "antoum/cmd_converter/converter_base.h"

typedef std::vector<uint64_t> AEBinary_t;

namespace antoum {
class AEConverter : public AEConverter_t {
public:
    AEConverter(std::shared_ptr<MFPara> para=nullptr);
    virtual ~AEConverter() = default;
    virtual bool SetText(std::string text);
    virtual std::string GetInstructionText();
    virtual bool SetBinary(AEBinary_t bin);
    virtual AEBinary_t GetBinary();
    virtual bool SetInstruction(std::shared_ptr<AEInstruction> instr);
    virtual std::shared_ptr<AEInstruction> GetInstruction();
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
    std::shared_ptr<AEInstruction> _instr;
    AEBinary_t* _bin;
    std::map<std::string,std::vector<std::pair<std::string,uint32_t>>> ae_isa_namespace;
};
}

#endif