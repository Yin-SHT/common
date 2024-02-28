/*
 * Filename: ae_converter.cpp
 * Author: Changxu Zhang
 * Description: Activation Eninge Command Converter
 */

#include "antoum/cmd_converter/ae_converter.h"
#include <regex>
#include <sstream>
#include <assert.h> 
#include <iostream>
using namespace std;

namespace antoum {
AEConverter::AEConverter(std::shared_ptr<MFPara> para) : AEConverter_t(para) {
ae_isa_namespace=\
{
    {"ACTIVATE",
    {{"SRC_START_ADDR",20},
     {"SRC_LINE_LENGTH",12},
     {"SRC_LINE_NUM",11},
     {"SRC_LINE_STRIDE",15},
     {"SRC_SURFACE_NUM",7},
     {"SRC_SURFACE_STRIDE",19},
     {"DST_START_ADDR",20},
     {"DST_LINE_LENGTH",12},
     {"DST_LINE_NUM",11},
     {"DST_LINE_STRIDE",15},
     {"DST_SURFACE_NUM",7},
     {"DST_SURFACE_STRIDE",19},
     {"P_VALUE",16},
     {"ACTIVATION_FUNC",4},
     {"OP_END",1}}
    },
    {"ACTIVATE_BASIC",
    {{"SRC_START_ADDR",20},
     {"SRC_LINE_LENGTH",12},
     {"SRC_LINE_NUM",11},
     {"SRC_LINE_STRIDE",15},
     {"SRC_SURFACE_NUM",7},
     {"SRC_SURFACE_STRIDE",19},
     {"DST_START_ADDR",20},
     {"DST_LINE_LENGTH",12},
     {"DST_LINE_NUM",11},
     {"DST_LINE_STRIDE",15},
     {"DST_SURFACE_NUM",7},
     {"DST_SURFACE_STRIDE",19},
     {"P_VALUE",16},
     {"ACTIVATION_FUNC",4},
     {"OP_END",1}}
    },
    {"OP_END",
    {{"OP_END",1}}
    }

};
_instr = CREATE_CLASS_DPC(AEInstruction_t, para, AEInstruction);
_bin = new AEBinary_t(4,0);
}


bool AEConverter::SetText(std::string text) {
    std::vector<std::string> v;
    std::stringstream ss(text);
    std::string buf;
    std::map<std::string, uint32_t> fields;
    std::string opcode;
    ss>>opcode;
    if(opcode == "ACTIVATE")
    {
        for(int i = 0; i < (int)ae_isa_namespace[opcode].size(); i++)
        {
            ss>>buf;
            fields.insert(std::pair<std::string, uint32_t>(ae_isa_namespace[opcode][i].first,std::stoul(buf)));
            assert((int)std::stoul(buf) < (1 << ae_isa_namespace[opcode][i].second));
        }
        _instr->SetFields(fields);
        _instr->opcode = opcode;
    }
    GetBinary();
    return true;
}

std::string AEConverter::GetInstructionText() {
    std::stringstream ss;
    std::map<std::string, uint32_t> fields;
    _instr->GetFields(fields);
    ss<<"ACTIVATE";
    for(int i = 0; i < (int)ae_isa_namespace["ACTIVATE"].size(); i++)
    {
        ss<<" ";
        ss<<fields[ae_isa_namespace["ACTIVATE"][i].first];
    }
    return ss.str();
}



bool AEConverter::SetBinary(AEBinary_t bin) {
    assert(bin.size() == 4);
    std::copy(bin.begin(),bin.end(),_bin->begin());
    _instr->fields->SRC_START_ADDR = SliceBits(20,36,bin[0]);
    _instr->fields->DST_START_ADDR = SliceBits(20,16,bin[0]);
    _instr->fields->ACTIVATION_FUNC = SliceBits(4,4,bin[0]);
    _instr->fields->OP_END = SliceBits(1,0,bin[0]);
    _instr->fields->SRC_LINE_LENGTH = SliceBits(12,52,bin[1]);
    _instr->fields->SRC_LINE_NUM = SliceBits(11,41,bin[1]);
    _instr->fields->SRC_LINE_STRIDE = SliceBits(15,26,bin[1]);
    _instr->fields->SRC_SURFACE_NUM = SliceBits(7,19,bin[1]);
    _instr->fields->SRC_SURFACE_STRIDE = SliceBits(19,0,bin[1]);
    _instr->fields->DST_LINE_LENGTH = SliceBits(12,52,bin[2]);
    _instr->fields->DST_LINE_NUM = SliceBits(11,41,bin[2]);
    _instr->fields->DST_LINE_STRIDE = SliceBits(15,26,bin[2]);
    _instr->fields->DST_SURFACE_NUM = SliceBits(7,19,bin[2]);
    _instr->fields->DST_SURFACE_STRIDE = SliceBits(19,0,bin[2]);
    _instr->fields->P_VALUE = SliceBits(16,48,bin[3]);
    return true;
}

AEBinary_t AEConverter::GetBinary() {
    uint64_t op_code = 0x03;
    (*_bin)[0] |= (op_code << 56);
    (*_bin)[0] |= (((uint64_t)_instr->fields->SRC_START_ADDR)<<36);
    (*_bin)[0] |= (((uint64_t)_instr->fields->DST_START_ADDR)<<16);
    (*_bin)[0] |= (((uint64_t)_instr->fields->ACTIVATION_FUNC)<<4);
    (*_bin)[0] |= (((uint64_t)_instr->fields->SRC_START_ADDR)<<36);
    (*_bin)[0] |= (uint64_t)_instr->fields->OP_END;
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_LINE_LENGTH)<<52);
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_LINE_NUM)<<41);
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_LINE_STRIDE)<<26);
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_SURFACE_NUM)<<19);
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_SURFACE_STRIDE)<<0);
    (*_bin)[2] |= (((uint64_t)_instr->fields->DST_LINE_LENGTH)<<52);
    (*_bin)[2] |= (((uint64_t)_instr->fields->DST_LINE_NUM)<<41);
    (*_bin)[2] |= (((uint64_t)_instr->fields->DST_LINE_STRIDE)<<26);
    (*_bin)[2] |= (((uint64_t)_instr->fields->DST_SURFACE_NUM)<<19);
    (*_bin)[2] |= (((uint64_t)_instr->fields->DST_SURFACE_STRIDE)<<0);
    (*_bin)[3] |= (((uint64_t)_instr->fields->P_VALUE)<<48);
    return *_bin;
}

bool AEConverter::SetInstruction(std::shared_ptr<AEInstruction> instr) {
    _instr = instr->Clone();
    return true;
}

std::shared_ptr<AEInstruction> AEConverter::GetInstruction() {
    return _instr->Clone();
}

void AEConverter::Dump(std::string file_path) {
    std::ofstream fout(file_path + "Cmd_list.txt",ios::app);
    std::ostringstream ss;
    for(int qidx = 0; qidx < (int)_bin->size();qidx+=2)
    {
        ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[qidx+1];
        ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[qidx];
        ss <<"\n";
    }
    

    assert(fout);
    fout << ss.str();
    fout.close();
}

bool AEConverter::SettingArgs(std::string opcode, const std::vector<uint32_t> &args) {
    std::stringstream ss;
    ss << opcode;
    std::map<std::string, uint32_t> fields;
    assert(args.size()==ae_isa_namespace[opcode].size());
    
    for(int i = 0; i < (int)args.size();i++) {
        if (args[i]+1!=0)
        {
            assert(args[i]<(1UL<<ae_isa_namespace[opcode][i].second));
        }
        fields.insert(std::pair<std::string, uint32_t>(ae_isa_namespace[opcode][i].first,args[i]));
    }
    _instr->SetFields(fields);
    return true;
}

std::string AEConverter::GetDetail(){
  std::stringstream ss;
  std::map<std::string, uint32_t> fields;
  _instr->GetFields(fields);
  ss << "ACTIVATE" << "\n";
  for (int i = 0; i < int(ae_isa_namespace["ACTIVATE"].size()); i++) {
    ss << ae_isa_namespace["ACTIVATE"][i].first << " " << fields[ae_isa_namespace["ACTIVATE"][i].first] << "\n";
  }
  return ss.str();
}

ConverterBase* AEConverter::LoadText(std::string text) {
    SetText(text);
    return this;
}

std::string AEConverter::ToText() {
    return GetInstructionText();
}

ConverterBase* AEConverter::LoadBinary(const std::vector<uint8_t> &bin) {
    SetBinary(bin8ToBin64(bin));
    return this;
}

std::vector<uint8_t> AEConverter::ToBinary() {
    return bin64ToBin8(GetBinary());
}

ConverterBase* AEConverter::LoadInstr(std::shared_ptr<InstrBase> instr) {
    SetInstruction(std::dynamic_pointer_cast<AEInstruction>(instr));
    return this;
}

std::shared_ptr<InstrBase> AEConverter::ToInstr() {
    return _instr->Clone();
}

}