/*
 * Filename: te_converter.cpp
 * Author: Changxu Zhang
 * Description: Transpose Eninge Command Converter
 */

#include "antoum/cmd_converter/te_converter.h"
#include <regex>
#include <sstream>
#include <assert.h> 
#include <iostream>
using namespace std;

namespace antoum {
TEConverter::TEConverter(std::shared_ptr<MFPara> para) : TEConverter_t(para) {
    te_isa_namespace =\
    {
        {"TE_CONFIG",
        {{"SRC_STRIDE_0",20},
        {"SRC_STRIDE_1",20},
        {"SRC_STRIDE_2",20},
        {"SRC_STRIDE_3",20},
        {"SRC_STRIDE_4",20},
        {"SRC_STEPBACK_STRIDE_0",20},
        {"SRC_STEPBACK_STRIDE_1",20},
        {"SRC_STEPBACK_STRIDE_2",20},
        {"SRC_STEPBACK_STRIDE_3",20},
        {"SRC_STEPBACK_STRIDE_4",20},
        {"DST_STRIDE_0",20},
        {"DST_STRIDE_1",20},
        {"DST_STRIDE_2",20},
        {"DST_STRIDE_3",20},
        {"DST_STRIDE_4",20},
        {"DST_STEPBACK_STRIDE_0",20},
        {"DST_STEPBACK_STRIDE_1",20},
        {"DST_STEPBACK_STRIDE_2",20},
        {"DST_STEPBACK_STRIDE_3",20},
        {"DST_STEPBACK_STRIDE_4",20},
        {"WRITE_AXIS_SEGMENTS",10},
        {"READ_AXIS_SEGMENTS",10},
        {"WRITE_STRIDE",20},
        {"READ_STRIDE",20},
        {"GROUP_NUM",20},
        {"CONTINUOUS_UNIT_SEGMENTS",10}
        }},
        {"TRANS",
        {{"SRC_START_ADDR",20},
        {"DST_START_ADDR",20},
        {"MODE",2},
        {"CONFIG_IDX",3},
        {"OP_END",1}
        }},
        {"SRC_STRIDE",
        {{"SRC_STRIDE_0",20},
        {"SRC_STRIDE_1",20},
        {"SRC_STRIDE_2",20},
        {"SRC_STRIDE_3",20},
        {"SRC_STRIDE_4",20},
        }},
        {"SRC_STEPBACK",
        {{"SRC_STEPBACK_STRIDE_0",20},
        {"SRC_STEPBACK_STRIDE_1",20},
        {"SRC_STEPBACK_STRIDE_2",20},
        {"SRC_STEPBACK_STRIDE_3",20},
        {"SRC_STEPBACK_STRIDE_4",20},
        }},
        {"DST_STRIDE",
        {{"DST_STRIDE_0",20},
        {"DST_STRIDE_1",20},
        {"DST_STRIDE_2",20},
        {"DST_STRIDE_3",20},
        {"DST_STRIDE_4",20},
        }},
        {"DST_STEPBACK",
        {{"DST_STEPBACK_STRIDE_0",20},
        {"DST_STEPBACK_STRIDE_1",20},
        {"DST_STEPBACK_STRIDE_2",20},
        {"DST_STEPBACK_STRIDE_3",20},
        {"DST_STEPBACK_STRIDE_4",20},
        }},
        {"INNER_LOOP",
        {{"WRITE_AXIS_SEGMENTS",10},
        {"READ_AXIS_SEGMENTS",10},
        {"WRITE_STRIDE",20},
        {"READ_STRIDE",20},
        {"GROUP_NUM",20},
        {"CONTINUOUS_UNIT_SEGMENTS",10}
        }},
        {"OP_END",
        {{"OP_END",1}}}

    };
    _instr = CREATE_CLASS_DPC(TEInstruction_t, para, TEInstruction);
    _config = CREATE_CLASS_DPC(TEConfiguration_t, para, TEConfiguration);
    _bin = new TEBinary_t(1,0);
}

bool TEConverter::SetText(std::string text) {
    std::vector<std::string> v;
    std::stringstream ss(text);
    std::string buf;
    std::map<std::string, uint32_t> fields;
    std::string opcode;
    ss>>opcode;
    
    if(opcode == "TRANS")
    {
        for(int i = 0; i < (int)te_isa_namespace[opcode].size(); i++)
        {
            ss>>buf;
            fields.insert(std::pair<std::string, uint32_t>(te_isa_namespace[opcode][i].first,std::stoul(buf)));
            if(std::stoul(buf) >=(1UL<<te_isa_namespace[opcode][i].second))
            {
                std::cerr<<"[Backend][ISA Error] "<<te_isa_namespace[opcode][i].first << " "<<
                std::stoul(buf)<< " is out of limit "<<(1UL<<te_isa_namespace[opcode][i].second)<<std::endl;
            }
            assert(std::stoul(buf) < (1UL<<te_isa_namespace[opcode][i].second));
        }
        _instr->SetFields(fields);
        _instr->opcode = opcode;
        GetBinary();
        return true;
    }
    else
    {
        return false;
    }
    
}

std::string TEConverter::GetInstructionText() {
    std::stringstream ss;
    std::map<std::string, uint32_t> fields;
    _instr->GetFields(fields);
    ss<<_instr->opcode;
    for(int i = 0; i < (int)te_isa_namespace["TRANS"].size(); i++)
    {
        ss<<" ";
        ss<<fields[te_isa_namespace["TRANS"][i].first];
    }
    return ss.str();
}

vector<std::string> TEConverter::GetConfigurationText(int config_idx) {
    std::vector<std::string> cmd_queue;
    
    std::map<std::string, uint32_t> fields;
    _config->GetFields(fields);
    for(int i = 0; i < (int)te_isa_namespace["TE_CONFIG"].size(); i++)
    {
        std::stringstream ss;
        ss<<"LOAD_CFG";
        ss<<" ";
        ss<<to_string(i * 8 + config_idx);
        ss<<" ";
        ss<<fields[te_isa_namespace["TE_CONFIG"][i].first];
        cmd_queue.push_back(ss.str());

    }
    return cmd_queue;
}

std::vector<std::pair<uint32_t,uint32_t>> TEConverter::GetConfigurationRegs(int config_idx){
    std::vector<std::pair<uint32_t,uint32_t>> regs(te_isa_namespace["TE_CONFIG"].size());
    
    std::map<std::string, uint32_t> fields;
    _config->GetFields(fields);
    for(int i = 0; i < (int)te_isa_namespace["TE_CONFIG"].size(); i++)
    {
        regs[i].first = ASIC_TRANSPOSE_REG_BASE_ADDR + 0xC + (i * 8 + config_idx)*4;
        regs[i].second =  fields[te_isa_namespace["TE_CONFIG"][i].first];
    }
    return regs;
}

bool TEConverter::SetBinary(TEBinary_t bin) {
    assert(bin.size() == 1);//assert(sizeof(bin)/sizeof(bin[0]) == 1);
    std::copy(bin.begin(),bin.end(),_bin->begin());
    _instr->opcode = (SliceBits(8,56,bin[0])==0x05)? "TRANS":"NOP";
    _instr->fields->SRC_START_ADDR = SliceBits(20,36,bin[0]);
    _instr->fields->DST_START_ADDR = SliceBits(20,16,bin[0]);
    _instr->fields->CONFIG_IDX = SliceBits(3,3,bin[0]);
    _instr->fields->MODE = SliceBits(2,1,bin[0]);
    _instr->fields->OP_END = SliceBits(1,0,bin[0]);
    return true;
}

TEBinary_t TEConverter::GetBinary() {
    uint64_t op_code = (_instr->opcode == "TRANS")? 0x05:0x00;
    (*_bin)[0] |= (op_code << 56);
    (*_bin)[0] |= (((uint64_t)_instr->fields->SRC_START_ADDR)<<36);
    (*_bin)[0] |= (((uint64_t)_instr->fields->DST_START_ADDR)<<16);
    (*_bin)[0] |= (((uint64_t)_instr->fields->CONFIG_IDX)<<3);
    (*_bin)[0] |= (((uint64_t)_instr->fields->MODE)<<1);
    (*_bin)[0] |= (uint64_t)_instr->fields->OP_END;
    return *_bin;
}

bool TEConverter::SetInstruction(std::shared_ptr<TEInstruction> instr) {
    _instr = instr->Clone();
    return true;
}
bool TEConverter::SetConfiguration(std::shared_ptr<TEConfiguration> config) {
    _config = config->Clone();
    return true;
}

std::shared_ptr<TEInstruction> TEConverter::GetInstruction() {
    return _instr->Clone();
}

std::shared_ptr<TEConfiguration> TEConverter::GetConfiguration() {
    return _config->Clone();
}

void TEConverter::Dump(std::string file_path) {
    std::ofstream fout(file_path + "Cmd_list.txt",ios::app);
    std::ostringstream ss;
    ss << std::setw(16) << std::setfill('0') << std::hex << 0;
    ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[0];
    ss <<"\n";

    assert(fout);
    fout << ss.str();
    fout.close();
}

bool TEConverter::SettingArgs(std::string opcode, const std::vector<uint32_t> &args) {
    std::stringstream ss;
    ss << opcode;
    std::map<std::string, uint32_t> fields;
    if(args.size()!=te_isa_namespace[opcode].size())
    {
        std::cerr<<"[Backend][ISA Error] "<<opcode << " params length not consistent "<<args.size()<<" <=> "<< te_isa_namespace[opcode].size()<<std::endl;
    }
    assert(args.size()==te_isa_namespace[opcode].size());
    
    for(int i = 0; i < (int)args.size();i++) 
    {
        if(args[i]>=(1UL<<te_isa_namespace[opcode][i].second))
        {
            std::cerr<<"[Backend][ISA Error] "<<te_isa_namespace[opcode][i].first << " "<<args[i]<< " is out of limit "<<(1UL<<te_isa_namespace[opcode][i].second)<<std::endl;
        }
        assert(args[i]<(1UL<<te_isa_namespace[opcode][i].second));
        fields.insert(std::pair<std::string, uint32_t>(te_isa_namespace[opcode][i].first,args[i]));
        
    }
    if(opcode == "TRANS")
    {
        _instr->SetFields(fields);
    }
    else
    {
        _config->SetFields(fields);
    }
    return true;
}

bool TEConverter::SetOpcode(std::string opcode)
{
    _instr->opcode = opcode;
    return true;
}

std::string TEConverter::GetDetail(){
  std::stringstream ss;
  std::map<std::string, uint32_t> fields;
  _instr->GetFields(fields);
  ss << "TRANS" << "\n";
  for (int i = 0; i < int(te_isa_namespace["TRANS"].size()); i++) {
    ss << te_isa_namespace["TRANS"][i].first << " " << fields[te_isa_namespace["TRANS"][i].first] << "\n";
  }
  return ss.str();
}

ConverterBase* TEConverter::LoadText(std::string text) {
    SetText(text);
    return this;
}

std::string TEConverter::ToText() {
    return GetInstructionText();
}

ConverterBase* TEConverter::LoadBinary(const std::vector<uint8_t> &bin) {
    SetBinary(bin8ToBin64(bin));
    return this;
}

std::vector<uint8_t> TEConverter::ToBinary() {
    return bin64ToBin8(GetBinary());
}

ConverterBase* TEConverter::LoadInstr(std::shared_ptr<InstrBase> instr) {
    SetInstruction(std::dynamic_pointer_cast<TEInstruction>(instr));
    return this;
}

std::shared_ptr<InstrBase> TEConverter::ToInstr() {
    return _instr->Clone();
}

}