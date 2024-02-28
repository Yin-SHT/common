/*
 * Filename: tke_converter.cpp
 * Author: Changxu Zhang
 * Description: TopK Eninge Command Converter
 */

#include "antoum/cmd_converter/tke_converter.h"
#include <regex>
#include <sstream>
#include <assert.h> 
#include <iostream>
using namespace std;

namespace antoum {
TKEConverter::TKEConverter(std::shared_ptr<MFPara> para) : TKEConverter_t(para) {
    tke_isa_namespace=\
    {
        {"TKE_CONFIG",
        {{"THRESHOLD",20}}},
        {"TOPK",
        {{"SRC_VALUE_START_ADDR",20},
        {"SRC_INDEX_START_ADDR",20},
        {"DST_VALUE_START_ADDR",20},
        {"DST_INDEX_START_ADDR",20},
        {"N",15},
        {"K",11},
        {"READ_MASK_RPT_CYCLE",7},
        {"FUNC_MODE",1},
        {"PRECISION_MODE",1},
        {"OP_END",1}
        }},
    };
    _instr = CREATE_CLASS_DPC(TKEInstruction_t, para, TKEInstruction);
    _bin = new TKEBinary_t(2,0);
}


bool TKEConverter::SetText(std::string text) {
    std::vector<std::string> v;
    std::stringstream ss(text);
    std::string buf;
    std::map<std::string, uint32_t> fields;
    std::string opcode;
    ss>>opcode;
    for(int i = 0; i < (int)tke_isa_namespace[opcode].size(); i++)
    {
        ss>>buf;
        fields.insert(std::pair<std::string, uint32_t>(tke_isa_namespace[opcode][i].first,std::stoul(buf)));
        assert(std::stoul(buf)<(1UL<<tke_isa_namespace[opcode][i].second));
    }
    _instr->SetFields(fields);
    _instr->opcode = opcode;
    GetBinary();
    return true;
}

std::string TKEConverter::GetInstructionText() {
    std::stringstream ss;
    std::map<std::string, uint32_t> fields;
    _instr->GetFields(fields);
    ss<<"TOPK";
    for(int i = 0; i < (int)tke_isa_namespace["TOPK"].size(); i++)
    {
        ss<<" ";
        ss<<fields[tke_isa_namespace["TOPK"][i].first];
    }
    return ss.str();
}



bool TKEConverter::SetBinary(TKEBinary_t bin) {
    assert(bin.size() == 2);
    std::copy(bin.begin(),bin.end(),_bin->begin());
    _instr->fields->SRC_INDEX_START_ADDR = SliceBits(20,36,bin[0]);
    _instr->fields->N = SliceBits(15,21,bin[0]);
    _instr->fields->K = SliceBits(11,10,bin[0]);
    _instr->fields->READ_MASK_RPT_CYCLE = SliceBits(7,3,bin[0]);
    _instr->fields->FUNC_MODE = SliceBits(1,2,bin[0]);
    _instr->fields->PRECISION_MODE = SliceBits(1,1,bin[0]);
    _instr->fields->OP_END = SliceBits(1,0,bin[0]);
    _instr->fields->SRC_VALUE_START_ADDR = SliceBits(20,44,bin[1]);
    _instr->fields->DST_INDEX_START_ADDR = SliceBits(20,24,bin[1]);
    _instr->fields->DST_VALUE_START_ADDR = SliceBits(20,4,bin[1]);
    return true;
}

TKEBinary_t TKEConverter::GetBinary() {
    uint64_t op_code = 0x04;
    (*_bin)[0] |= (op_code << 56);
    (*_bin)[0] |= (((uint64_t)_instr->fields->SRC_INDEX_START_ADDR)<<36);
    (*_bin)[0] |= (((uint64_t)_instr->fields->N)<<21);
    (*_bin)[0] |= (((uint64_t)_instr->fields->K)<<10);
    (*_bin)[0] |= (((uint64_t)_instr->fields->READ_MASK_RPT_CYCLE)<<3);
    (*_bin)[0] |= (((uint64_t)_instr->fields->FUNC_MODE)<<2);
    (*_bin)[0] |= (((uint64_t)_instr->fields->PRECISION_MODE)<<1);
    (*_bin)[0] |= (uint64_t)_instr->fields->OP_END;
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_VALUE_START_ADDR)<<44);
    (*_bin)[1] |= (((uint64_t)_instr->fields->DST_INDEX_START_ADDR)<<24);
    (*_bin)[1] |= (((uint64_t)_instr->fields->DST_VALUE_START_ADDR)<<4);
    return *_bin;
}

bool TKEConverter::SetInstruction(std::shared_ptr<TKEInstruction> instr) {
    _instr = instr->Clone();
    return true;
}

std::shared_ptr<TKEInstruction> TKEConverter::GetInstruction() {
    return _instr->Clone();
}



void TKEConverter::Dump(std::string file_path) {
    std::ofstream fout(file_path + "Cmd_list.txt",ios::app);
    std::ostringstream ss;
    for(int qidx = (*_bin).size() - 1; qidx >= 0;qidx--)
    {
        ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[qidx];
        if(qidx % 2 == 0)
        {
            ss <<"\n";
        }
    }
    

    assert(fout);
    fout << ss.str();
    fout.close();
}

bool TKEConverter::SettingArgs(std::string opcode, const std::vector<uint32_t> &args) {
    std::stringstream ss;
    ss << opcode;
    std::map<std::string, uint32_t> fields;
    assert(args.size()==tke_isa_namespace[opcode].size());
    
    for(int i = 0; i < (int)args.size();i++) {
        assert(args[i]<(1UL<<tke_isa_namespace["TOPK"][i].second));
        fields.insert(std::pair<std::string, uint32_t>(tke_isa_namespace[opcode][i].first,args[i]));
    }

    _instr->SetFields(fields);
    return true;
}

std::string TKEConverter::GetDetail(){
  std::stringstream ss;
  std::map<std::string, uint32_t> fields;
  _instr->GetFields(fields);
  ss << "TOPK" << "\n";
  for (int i = 0; i < int(tke_isa_namespace["TOPK"].size()); i++) {
    ss << tke_isa_namespace["TOPK"][i].first << " " << fields[tke_isa_namespace["TOPK"][i].first] << "\n";
    
  }
  return ss.str();
}

ConverterBase* TKEConverter::LoadText(std::string text) { 
    SetText(text);
    return this;
}

std::string TKEConverter::ToText() {
    return GetInstructionText();
}

ConverterBase* TKEConverter::LoadBinary(const std::vector<uint8_t> &bin) {
    SetBinary(bin8ToBin64(bin));
    return this;
}

std::vector<uint8_t> TKEConverter::ToBinary() {
    return bin64ToBin8(GetBinary());
}

ConverterBase* TKEConverter::LoadInstr(std::shared_ptr<InstrBase> instr) {
    SetInstruction(std::dynamic_pointer_cast<TKEInstruction>(instr));
    return this;
}

std::shared_ptr<InstrBase> TKEConverter::ToInstr() {
    return _instr->Clone();
}

}