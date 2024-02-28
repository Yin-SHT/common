/*
 * Filename: pea_converter.cpp
 * Author: Changxu Zhang
 * Description: PEArray Eninge Command Converter
 */

#include "antoum/cmd_converter/pea_converter.h"
#include <regex>
#include <sstream>
#include <assert.h> 
#include <iostream>
using namespace std;

namespace antoum {
PEAConverter::PEAConverter(std::shared_ptr<MFPara> para) : PEAConverter_t(para) {
pea_isa_namespace=\
{
    {"CONV2D_CONFIG",
    {{"EMPTY",0}}},
    {"CONV2D",
    {{"SRC_START_ADDR",20},                            
    {"SRC_LINE_STRIDE",15},              
    {"SRC_SURFACE_NUM",7},             
    {"SRC_SURFACE_STRIDE",19},           
    {"WEIGHT_BUF_START_ADDR",15},       
    {"WEIGHT_LINE_LENGTH",10},
    {"WEIGHT_LINE_NUM",5},
    {"WEIGHT_LINE_STRIDE",3},
    {"WEIGHT_SUF_NUM",7},
    {"WEIGHT_SUF_STRIDE",5},               
    {"DST_START_ADDR",20},               
    {"DST_LINE_LENGTH",12},             
    {"DST_LINE_STRIDE",15},             
    {"DST_SURFACE_NUM",7},            
    {"DST_SURFACE_STRIDE",19},
    {"DST_MODE",2},
    {"Rq",2},
    {"Rr",2},
    {"Sq",2},
    {"Sr",2},
    {"Cgb",3},
    {"Cgb_wr",3},
    {"Kq",8},
    {"Kr",6},
    {"Kr_bcast",1},
    {"Kg_pt",3},
    {"Cq",8},
    {"Cr",3},
    {"nnz_cgb_cq",6},
    {"nnz_cgb_cr",6},
    {"FILTER_STRIDE",1},          
    {"PAD_TOP_MARGIN",3},
    {"PAD_BOTTOM_MARGIN",3},
    {"PAD_LEFT_MARGIN",3},
    {"PAD_RIGHT_MARGIN",3},
    {"ZERO_PAD_TOP_EN",1},
    {"ZERO_PAD_BOTTOM_EN",1},
    {"ZERO_PAD_LEFT_EN",1},
    {"ZERO_PAD_RIGHT_EN",1},
    {"WIDTH",12},
    {"HEIGHT",12},
    {"PRECISION_MODE",1},
    {"BIAS_ADD_EN",1},
    {"ACTIVATION_EN",1},
    {"ELEMENT_OP_EN",1},
    {"ACTIVATION_FUNC",1},
    {"ACTIVATION_TABLE_START",1},
    {"BIAS_START_ADDR",15},
    {"QUANT_SCALE_START_ADDR",15},
    {"DEQUANT_SCALE_START_ADDR",15},
    {"ELEMENT_OP_SRC_START_ADDR",20},
    {"USE_PARTIAL_SUM_EN",1},
    {"GEN_FINAL_SUM_EN",1},
    {"DST_VALUE_WR_MASK",4},
    {"WEIGHT_DENSE_ENABLE",1},
    {"OP_END",1}
    }},
    {"CONV2D_SRC_ADDRESS",
    {{"SRC_START_ADDR",20},                            
    {"SRC_LINE_STRIDE",15},              
    {"SRC_SURFACE_NUM",7},             
    {"SRC_SURFACE_STRIDE",19}
    }},
    {"CONV2D_DST_ADDRESS",
    {{"DST_START_ADDR",20},                              
    {"DST_LINE_STRIDE",15},             
    {"DST_SURFACE_NUM",7},            
    {"DST_SURFACE_STRIDE",19}}},
    {"CONV2D_DST_SIZE",
    {{"DST_LINE_LENGTH",12},             
    {"DST_VALUE_WR_MASK",4}}},
    {"CONV2D_WEIGHT_ADDRESS",
    {{"WEIGHT_BUF_START_ADDR",15},       
    {"WEIGHT_LINE_LENGTH",10},
    {"WEIGHT_LINE_NUM",5},
    {"WEIGHT_LINE_STRIDE",3},
    {"WEIGHT_SUF_NUM",7},
    {"WEIGHT_SUF_STRIDE",5},
    {"WEIGHT_DENSE_ENABLE",1}
    }},
    {"CONV2D_KERNEL",
    {{"Rq",2},
    {"Rr",2},
    {"Sq",2},
    {"Sr",2},
    {"Kq",8},
    {"Kr",6},
    {"Kg_pt",3},
    {"nnz_cgb_cq",6},
    {"nnz_cgb_cr",6}}},
    {"CONV2D_CHANNEL",
    {{"Cgb",3},
    {"Cgb_wr",3},
    {"Cq",8},
    {"Cr",3},
    {"Kr_bcast",1}}},
    {"CONV2D_STRIDE",
    {{"DST_MODE",2},
    {"FILTER_STRIDE",1}}},
    {"CONV2D_PADDING",
    {{"PAD_TOP_MARGIN",3},
    {"PAD_BOTTOM_MARGIN",3},
    {"PAD_LEFT_MARGIN",3},
    {"PAD_RIGHT_MARGIN",3},
    {"ZERO_PAD_TOP_EN",1},
    {"ZERO_PAD_BOTTOM_EN",1},
    {"ZERO_PAD_LEFT_EN",1},
    {"ZERO_PAD_RIGHT_EN",1}}},
    {"CONV2D_SIZE",
    {{"WIDTH",12},
    {"HEIGHT",12}}},
    {"CONV2D_BIAS",
    {{"BIAS_ADD_EN",1},
    {"BIAS_START_ADDR",15}}},
    {"CONV2D_QUANT",
    {{"QUANT_SCALE_START_ADDR",15},
    {"DEQUANT_SCALE_START_ADDR",15}}},
    {"CONV2D_ACTIV",
    {{"ACTIVATION_EN",1},
    {"ACTIVATION_FUNC",1},
    {"ACTIVATION_TABLE_START",1}}},
    {"CONV2D_ELEMENT_OP",
    {{"ELEMENT_OP_EN",1},
    {"ELEMENT_OP_SRC_START_ADDR",20}}},
    {"CONV2D_SUM",
    {{"USE_PARTIAL_SUM_EN",1},
    {"GEN_FINAL_SUM_EN",1}}},
    {"CONV2D_END",
    {{"OP_END",1}}},
    {"CONV2D_PRECISION",
    {{"PRECISION_MODE",1}}}
};
_instr = CREATE_CLASS_DPC(PEAInstruction_t, para, PEAInstruction);
_bin = new PEABinary_t(6,0);
}

bool PEAConverter::SetText(std::string text) {
    std::vector<std::string> v;
    std::stringstream ss(text);
    std::string buf;
    std::map<std::string, uint32_t> fields;
    std::string opcode;
    ss>>opcode;
    if(opcode == "CONV2D")
    {
        for(int i = 0; i < (int)pea_isa_namespace[opcode].size(); i++)
        {
            ss>>buf;
            fields.insert(std::pair<std::string, uint32_t>(pea_isa_namespace[opcode][i].first,(uint32_t)std::stol(buf)));
            assert(std::stoul(buf)<(1UL<<pea_isa_namespace[opcode][i].second));
        }
        _instr->SetFields(fields);
        _instr->opcode = opcode;
    }
    if(opcode == "LOAD_PEA_CFG")
    {
        ss>>buf;
        uint32_t reg_addr = (uint32_t)std::stoi(buf);
        ss>>buf;
        uint32_t reg_value = (uint32_t)std::stoi(buf);
        fields.insert(std::pair<std::string, uint32_t>(pea_isa_namespace["PEA_CONFIG"][reg_addr].first,reg_value));
        _config.SetFields(fields);
    }
    GetBinary();
    return true;
}

std::string PEAConverter::GetInstructionText() {
    std::stringstream ss;
    std::map<std::string, uint32_t> fields;
    _instr->GetFields(fields);
    ss<<"CONV2D";
    for(int i = 0; i < (int)pea_isa_namespace["CONV2D"].size(); i++)
    {
        ss<<" ";
        ss<<fields[pea_isa_namespace["CONV2D"][i].first];
    }
    return ss.str();
}

vector<std::string> PEAConverter::GetConfigurationText() {
    std::vector<std::string> cmd_queue;
    
    std::map<std::string, uint32_t> fields;
    _config.GetFields(fields);
    for(int i = 0; i < (int)pea_isa_namespace["PEA_CONFIG"].size(); i++)
    {
        std::stringstream ss;
        ss<<"LOAD_PEA_CFG";
        ss<<" ";
        ss<<to_string(i);
        ss<<" ";
        ss<<fields[pea_isa_namespace["PEA_CONFIG"][i].first];
        cmd_queue.push_back(ss.str());

    }
    return cmd_queue;
}

bool PEAConverter::SetBinary(PEABinary_t bin) {
    assert(bin.size() == 6);
    std::copy(bin.begin(),bin.end(),_bin->begin());
    _instr->fields->SRC_START_ADDR = SliceBits(20,36,bin[0]);
    _instr->fields->DST_START_ADDR = SliceBits(20,16,bin[0]);
    _instr->fields->WEIGHT_DENSE_ENABLE = SliceBits(1,15,bin[0]);
    _instr->fields->FILTER_STRIDE = SliceBits(1,14,bin[0]);
    _instr->fields->DST_MODE = SliceBits(2,12,bin[0]);
    _instr->fields->ZERO_PAD_TOP_EN = SliceBits(1,11,bin[0]);
    _instr->fields->ZERO_PAD_BOTTOM_EN = SliceBits(1,10,bin[0]);
    _instr->fields->ZERO_PAD_LEFT_EN = SliceBits(1,9,bin[0]);
    _instr->fields->ZERO_PAD_RIGHT_EN = SliceBits(1,8,bin[0]);
    _instr->fields->GEN_FINAL_SUM_EN = SliceBits(1,7,bin[0]);
    _instr->fields->USE_PARTIAL_SUM_EN = SliceBits(1,6,bin[0]);
    _instr->fields->PRECISION_MODE = SliceBits(1,5,bin[0]);
    _instr->fields->BIAS_ADD_EN = SliceBits(1,4,bin[0]);
    _instr->fields->ACTIVATION_EN = SliceBits(1,3,bin[0]);
    _instr->fields->ELEMENT_OP_EN = SliceBits(1,2,bin[0]);
    _instr->fields->ACTIVATION_FUNC = SliceBits(1,1,bin[0]);
    _instr->fields->OP_END = SliceBits(1,0,bin[0]);
    _instr->fields->ELEMENT_OP_SRC_START_ADDR = SliceBits(20,44,bin[1]);
    _instr->fields->SRC_LINE_STRIDE = SliceBits(15,29,bin[1]);
    _instr->fields->SRC_SURFACE_NUM = SliceBits(7,22,bin[1]);
    _instr->fields->SRC_SURFACE_STRIDE = SliceBits(19,3,bin[1]);
    _instr->fields->Cgb = SliceBits(3,61,bin[2]);
    _instr->fields->Cgb_wr = SliceBits(3,58,bin[2]);
    _instr->fields->Rq = SliceBits(2,56,bin[2]);
    _instr->fields->Rr = SliceBits(2,54,bin[2]);
    _instr->fields->Sq = SliceBits(2,52,bin[2]);
    _instr->fields->Sr = SliceBits(2,50,bin[2]);
    _instr->fields->Kq = SliceBits(8,42,bin[2]);
    _instr->fields->Kr_bcast = SliceBits(1,39,bin[2]); 
    _instr->fields->Kr = SliceBits(6,33,bin[2]);
    _instr->fields->Kg_pt = SliceBits(3,30,bin[2]);
    _instr->fields->Cq = SliceBits(8,22,bin[2]);
    _instr->fields->Cr = SliceBits(3,16,bin[2]);
    _instr->fields->QUANT_SCALE_START_ADDR = SliceBits(15,1,bin[2]);
    _instr->fields->DEQUANT_SCALE_START_ADDR = SliceBits(15,49,bin[3]);
    _instr->fields->WEIGHT_BUF_START_ADDR = SliceBits(15,34,bin[3]);
    _instr->fields->WEIGHT_LINE_LENGTH = SliceBits(10,24,bin[3]);
    _instr->fields->WEIGHT_LINE_NUM = SliceBits(5,19,bin[3]);
    _instr->fields->WEIGHT_LINE_STRIDE = SliceBits(3,16,bin[3]);
    _instr->fields->WEIGHT_SUF_NUM = SliceBits(7,9,bin[3]);
    _instr->fields->WEIGHT_SUF_STRIDE = SliceBits(5,4,bin[3]);
    _instr->fields->ACTIVATION_TABLE_START = SliceBits(1,3,bin[3]);
    _instr->fields->BIAS_START_ADDR = SliceBits(15,49,bin[4]);
    _instr->fields->HEIGHT = SliceBits(12,37,bin[4]);
    _instr->fields->WIDTH = SliceBits(12,25,bin[4]);
    _instr->fields->nnz_cgb_cq = SliceBits(6,19,bin[4]);
    _instr->fields->nnz_cgb_cr = SliceBits(6,13,bin[4]);
    _instr->fields->PAD_TOP_MARGIN = SliceBits(3,10,bin[4]);
    _instr->fields->PAD_BOTTOM_MARGIN = SliceBits(3,7,bin[4]);
    _instr->fields->PAD_LEFT_MARGIN = SliceBits(3,4,bin[4]);
    _instr->fields->PAD_RIGHT_MARGIN = SliceBits(3,1,bin[4]);
    _instr->fields->DST_LINE_LENGTH = SliceBits(12,52,bin[5]);
    _instr->fields->DST_LINE_STRIDE = SliceBits(15,30,bin[5]);
    _instr->fields->DST_SURFACE_NUM = SliceBits(7,23,bin[5]);
    _instr->fields->DST_SURFACE_STRIDE = SliceBits(19,4,bin[5]);
    _instr->fields->DST_VALUE_WR_MASK = SliceBits(4,0,bin[5]);
    return true;
}

PEABinary_t PEAConverter::GetBinary() {
    uint64_t op_code = 0x02;
    (*_bin)[0] |= (op_code << 56);
    (*_bin)[0] |= (((uint64_t)_instr->fields->SRC_START_ADDR)<<36);
    (*_bin)[0] |= (((uint64_t)_instr->fields->DST_START_ADDR)<<16);
    (*_bin)[0] |= (((uint64_t)_instr->fields->WEIGHT_DENSE_ENABLE)<<15);
    (*_bin)[0] |= (((uint64_t)_instr->fields->FILTER_STRIDE)<<14);
    (*_bin)[0] |= (((uint64_t)_instr->fields->DST_MODE)<<12);
    (*_bin)[0] |= (((uint64_t)_instr->fields->ZERO_PAD_TOP_EN)<<11);
    (*_bin)[0] |= (((uint64_t)_instr->fields->ZERO_PAD_BOTTOM_EN)<<10);
    (*_bin)[0] |= (((uint64_t)_instr->fields->ZERO_PAD_LEFT_EN)<<9);
    (*_bin)[0] |= (((uint64_t)_instr->fields->ZERO_PAD_RIGHT_EN)<<8);
    (*_bin)[0] |= (((uint64_t)_instr->fields->GEN_FINAL_SUM_EN)<<7);
    (*_bin)[0] |= (((uint64_t)_instr->fields->USE_PARTIAL_SUM_EN)<<6);
    (*_bin)[0] |= (((uint64_t)_instr->fields->PRECISION_MODE)<<5);
    (*_bin)[0] |= (((uint64_t)_instr->fields->BIAS_ADD_EN)<<4);
    (*_bin)[0] |= (((uint64_t)_instr->fields->ACTIVATION_EN)<<3);
    (*_bin)[0] |= (((uint64_t)_instr->fields->ELEMENT_OP_EN)<<2);
    (*_bin)[0] |= (((uint64_t)_instr->fields->ACTIVATION_FUNC)<<1);
    (*_bin)[0] |= (uint64_t)_instr->fields->OP_END;
    (*_bin)[1] |= (((uint64_t)_instr->fields->ELEMENT_OP_SRC_START_ADDR)<<44);
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_LINE_STRIDE)<<29);
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_SURFACE_NUM)<<22);
    (*_bin)[1] |= (((uint64_t)_instr->fields->SRC_SURFACE_STRIDE)<<3);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Cgb)<<61);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Cgb_wr)<<58);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Rq)<<56);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Rr)<<54);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Sq)<<52);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Sr)<<50);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Kq)<<42);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Kr_bcast)<<39);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Kr)<<33);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Kg_pt)<<30);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Cq)<<22);
    (*_bin)[2] |= (((uint64_t)_instr->fields->Cr)<<16);
    (*_bin)[2] |= (((uint64_t)_instr->fields->QUANT_SCALE_START_ADDR)<<1);
    (*_bin)[3] |= (((uint64_t)_instr->fields->DEQUANT_SCALE_START_ADDR)<<49);
    (*_bin)[3] |= (((uint64_t)_instr->fields->WEIGHT_BUF_START_ADDR)<<34);
    (*_bin)[3] |= (((uint64_t)_instr->fields->WEIGHT_LINE_LENGTH)<<24);
    (*_bin)[3] |= (((uint64_t)_instr->fields->WEIGHT_LINE_NUM)<<19);
    (*_bin)[3] |= (((uint64_t)_instr->fields->WEIGHT_LINE_STRIDE)<<16);
    (*_bin)[3] |= (((uint64_t)_instr->fields->WEIGHT_SUF_NUM)<<9);
    (*_bin)[3] |= (((uint64_t)_instr->fields->WEIGHT_SUF_STRIDE)<<4);
    (*_bin)[3] |= (((uint64_t)_instr->fields->ACTIVATION_TABLE_START)<<3);
    (*_bin)[4] |= (((uint64_t)_instr->fields->BIAS_START_ADDR)<<49);
    (*_bin)[4] |= (((uint64_t)_instr->fields->HEIGHT)<<37);
    (*_bin)[4] |= (((uint64_t)_instr->fields->WIDTH)<<25);
    (*_bin)[4] |= (((uint64_t)_instr->fields->nnz_cgb_cq)<<19);
    (*_bin)[4] |= (((uint64_t)_instr->fields->nnz_cgb_cr)<<13);
    (*_bin)[4] |= (((uint64_t)_instr->fields->PAD_TOP_MARGIN)<<10);
    (*_bin)[4] |= (((uint64_t)_instr->fields->PAD_BOTTOM_MARGIN)<<7);
    (*_bin)[4] |= (((uint64_t)_instr->fields->PAD_LEFT_MARGIN)<<4);
    (*_bin)[4] |= (((uint64_t)_instr->fields->PAD_RIGHT_MARGIN)<<1);
    (*_bin)[5] |= (((uint64_t)_instr->fields->DST_LINE_LENGTH)<<52);
    (*_bin)[5] |= (((uint64_t)_instr->fields->DST_LINE_STRIDE)<<30);
    (*_bin)[5] |= (((uint64_t)_instr->fields->DST_SURFACE_NUM)<<23);
    (*_bin)[5] |= (((uint64_t)_instr->fields->DST_SURFACE_STRIDE)<<4);
    (*_bin)[5] |= (((uint64_t)_instr->fields->DST_VALUE_WR_MASK)<<0);
    return *_bin;
}

bool PEAConverter::SetInstruction(std::shared_ptr<PEAInstruction> instr) {
    _instr = instr->Clone();
    return true;
}
bool PEAConverter::SetConfiguration(const PEAConfiguration &config) {
    _config = config;
    return true;
}

std::shared_ptr<PEAInstruction> PEAConverter::GetInstruction() {
    return _instr->Clone();
}

PEAConfiguration PEAConverter::GetConfiguration() {
    return _config;
}

void PEAConverter::Dump(std::string file_path) {
    std::ofstream fout(file_path + "PEA_CMD.txt",ios::app);
    std::ostringstream ss;
    for(int qidx = _bin->size() - 1; qidx >= 0;qidx--)
    {
        ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[qidx];
    }
    ss <<"\n";

    assert(fout);
    fout << ss.str();
    fout.close();
}

void PEAConverter::Clear()
{
    for(int i = 0; i < (int)_bin->size(); i++)
    {
        (*_bin)[i] = 0;
    }
}

bool PEAConverter::SettingArgs(std::string opcode, const std::vector<uint32_t> &args) {
    std::stringstream ss;
    ss << opcode;
    std::map<std::string, uint32_t> fields;
    if(args.size()!=pea_isa_namespace[opcode].size()) {
        LOG(ERROR)<<"Args number inconsistent. Fields:"<<opcode;
    }
    assert(args.size()==pea_isa_namespace[opcode].size());
    
    for(int i = 0; i < (int)args.size();i++) {
        if (args[i]+1!=0)
        {
            if(args[i] >= (1UL << pea_isa_namespace[opcode][i].second))
            {
                LOG(INFO)<<"[Assert Error] "<<pea_isa_namespace[opcode][i].first\
                <<" Get =>"<<args[i]<< " Bit limit => "<<pea_isa_namespace[opcode][i].second<<endl;
            }
            // assert(args[i] < (1UL << pea_isa_namespace[opcode][i].second));
        }
        fields.insert(std::pair<std::string, uint32_t>(pea_isa_namespace[opcode][i].first,args[i]));
    }
    if(opcode == "CONV2D_CONFIG")
    {
        _config.SetFields(fields);
    }
    else
    {
        _instr->SetFields(fields);
    }
    return false;
}

std::string PEAConverter::GetDetail(){
  std::stringstream ss;
  std::map<std::string, uint32_t> fields;
  _instr->GetFields(fields);
  ss << "CONV2D" << "\n";
  for (int i = 0; i < int(pea_isa_namespace["CONV2D"].size()); i++) {
    ss << pea_isa_namespace["CONV2D"][i].first << " " << fields[pea_isa_namespace["CONV2D"][i].first] << "\n";
  }
  return ss.str();
}

ConverterBase* PEAConverter::LoadText(std::string text) {
    SetText(text);
    return this;
}

std::string PEAConverter::ToText() {
    return GetInstructionText();
}

ConverterBase* PEAConverter::LoadBinary(const std::vector<uint8_t> &bin) {
    SetBinary(bin8ToBin64(bin));
    return this;
}

std::vector<uint8_t> PEAConverter::ToBinary() {
    return bin64ToBin8(GetBinary());
}

ConverterBase* PEAConverter::LoadInstr(std::shared_ptr<InstrBase> instr) {
    SetInstruction(std::dynamic_pointer_cast<PEAInstruction>(instr));
    return this;
}

std::shared_ptr<InstrBase> PEAConverter::ToInstr() {
    return _instr->Clone();
}

}