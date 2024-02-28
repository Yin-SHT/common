#include "antoum/cmd_converter/gcpy_converter.h"
#include <sstream>

namespace antoum {
GCPYConverter::GCPYConverter(std::shared_ptr<MFPara> para) : GCPYConverter_t(para) {
gcpy_isa_namespace = 
    { {"GLBCPY",
		{{"SRC_CORE_ID",2},
		{"SRC_START_ADDR",21},
		{"SRC_LINE_LENGTH",12},
		{"SRC_LINE_NUM",11},
		{"SRC_LINE_STRIDE",15},
		{"SRC_SURFACE_NUM",7},
		{"SRC_SURFACE_STRIDE",19},
		{"DST_CORE_ID",2},
		{"DST_START_ADDR",21},
		{"DST_LINE_LENGTH",12},
		{"DST_LINE_NUM",11},
		{"DST_LINE_STRIDE",15},
		{"DST_SURFACE_NUM",7},
		{"DST_SURFACE_STRIDE",19},
		{"PRECISION_MODE",1},
		{"FUNC_OP",2},
		{"SYNC",1},
		{"OP_END",1}}}
    };
    _instr = CREATE_CLASS_DPC(GCPYInstruction_t, para, GCPYInstruction);
    _bin = new GCPYBinary_t(4,0);
}

GCPYConverter::GCPYConverter(std::string str) : GCPYConverter_t(_para) {
    //GCPYConverter(_para); 
    SetByString(str); 
}

GCPYConverter::GCPYConverter(GCPYBinary_t bin) : GCPYConverter_t(_para) { 
    //GCPYConverter(_para); 
    SetBinary(bin); 
}

GCPYConverter::GCPYConverter(std::shared_ptr<GCPYInstruction> instr) : GCPYConverter_t(_para) { 
    //GCPYConverter(_para); 
    SetByInstruction(instr); 
}

bool GCPYConverter::SetByString(std::string str) {
  std::vector<std::string> v;
  split(str, v);

  assert(v.size() == gcpy_isa_namespace["GLBCPY"].size() + 1);
  std::map<std::string, uint32_t> fields;

  for (int i = 1; i < int(v.size()); i++) {
    fields[gcpy_isa_namespace["GLBCPY"][i-1].first] = std::stoi(v[i]);
    assert(std::stoul(v[i]) < (1ULL<<gcpy_isa_namespace["GLBCPY"][i-1].second));
  }
  _instr->SetFields(fields);
  GetBinary();
  return true;
}

std::string GCPYConverter::GetString() {
	std::stringstream ss;

	std::map<std::string, uint32_t> fields;
	_instr->GetFields(fields);

  	ss << "GLBCPY";
	for (int i = 0; i < int(gcpy_isa_namespace["GLBCPY"].size()); i++) {
		ss << " " << fields[gcpy_isa_namespace["GLBCPY"][i].first];
	}

  return ss.str();
}

bool GCPYConverter::SetByInstruction(std::shared_ptr<GCPYInstruction> instr) {
	_instr = instr->Clone();
	GetBinary();
	return true;
}

std::shared_ptr<GCPYInstruction> GCPYConverter::GetInstruction() {
  	return _instr->Clone();
}

bool GCPYConverter::SetBinary(GCPYBinary_t bin) {
    assert(bin.size() == 4);
    std::copy(bin.begin(),bin.end(),_bin->begin());
    _instr->fields->SRC_START_ADDR = SliceBits(21,35,bin[0]);
    _instr->fields->DST_START_ADDR = SliceBits(21,14,bin[0]);
    _instr->fields->SRC_CORE_ID = SliceBits(2,7,bin[0]);
    _instr->fields->DST_CORE_ID = SliceBits(2,5,bin[0]);
    _instr->fields->FUNC_OP = SliceBits(2,3,bin[0]);
    _instr->fields->PRECISION_MODE = SliceBits(1,2,bin[0]);
    _instr->fields->SYNC = SliceBits(1,1,bin[0]);
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
    return true;
}

GCPYBinary_t GCPYConverter::GetBinary() {
    // _bin = new GCPYBinary_t(4,0);
    uint64_t op_code = 0x06;
    (*_bin)[0] |= (op_code << 56);
    (*_bin)[0] |= (((uint64_t)_instr->fields->SRC_START_ADDR)<<35);
    (*_bin)[0] |= (((uint64_t)_instr->fields->DST_START_ADDR)<<14);
    (*_bin)[0] |= (((uint64_t)_instr->fields->SRC_CORE_ID)<<7);
    (*_bin)[0] |= (((uint64_t)_instr->fields->DST_CORE_ID)<<5);
    (*_bin)[0] |= (((uint64_t)_instr->fields->FUNC_OP)<<3);
    (*_bin)[0] |= (((uint64_t)_instr->fields->PRECISION_MODE)<<2);
    (*_bin)[0] |= (((uint64_t)_instr->fields->SYNC)<<1);
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
    return *_bin;
}

TensorDesc GCPYConverter::GetSrcDesc() {
	TensorDesc tsr_desc;
	if(_instr->fields->FUNC_OP == 0 || _instr->fields->FUNC_OP == 2) {  // C2CLoadTensor, Tensor2Weight
		tsr_desc.address         = (_instr->fields->SRC_START_ADDR) << 4;
		tsr_desc.lineLength      = (_instr->fields->SRC_LINE_LENGTH +1) << 4;
		tsr_desc.lineNumber      = _instr->fields->SRC_LINE_NUM + 1;
		tsr_desc.surfaceNumber   = _instr->fields->SRC_SURFACE_NUM + 1;
		tsr_desc.lineStride      = _instr->fields->SRC_LINE_STRIDE << 4;
		tsr_desc.surfaceStride   = _instr->fields->SRC_SURFACE_STRIDE << 4;
	}
	else if(_instr->fields->FUNC_OP == 1) {  // C2CLoadWeight
		tsr_desc.address         = (_instr->fields->SRC_START_ADDR) * 144;
		tsr_desc.lineLength      = (_instr->fields->SRC_LINE_LENGTH +1) * 144;
		tsr_desc.lineNumber      = _instr->fields->SRC_LINE_NUM + 1;
		tsr_desc.surfaceNumber   = _instr->fields->SRC_SURFACE_NUM + 1;
		tsr_desc.lineStride      = _instr->fields->SRC_LINE_STRIDE * 144;
		tsr_desc.surfaceStride   = _instr->fields->SRC_SURFACE_STRIDE * 144;
	}
	return tsr_desc;
}

TensorDesc GCPYConverter::GetDstDesc() {
	TensorDesc tsr_desc;
	if(_instr->fields->FUNC_OP == 0) {  // C2CLoadTensor
		tsr_desc.address         = (_instr->fields->DST_START_ADDR) << 4;
		tsr_desc.lineLength      = (_instr->fields->DST_LINE_LENGTH +1) << 4;
		tsr_desc.lineNumber      = _instr->fields->DST_LINE_NUM + 1;
		tsr_desc.surfaceNumber   = _instr->fields->DST_SURFACE_NUM + 1;
		tsr_desc.lineStride      = _instr->fields->DST_LINE_STRIDE << 4;
		tsr_desc.surfaceStride   = _instr->fields->DST_SURFACE_STRIDE << 4;
	}
	else if(_instr->fields->FUNC_OP == 1 || _instr->fields->FUNC_OP == 2) {  // Tensor2Weight, C2CLoadWeight
		tsr_desc.address         = (_instr->fields->DST_START_ADDR) * 144;
		tsr_desc.lineLength      = (_instr->fields->DST_LINE_LENGTH +1) * 144;
		tsr_desc.lineNumber      = _instr->fields->DST_LINE_NUM + 1;
		tsr_desc.surfaceNumber   = _instr->fields->DST_SURFACE_NUM + 1;
		tsr_desc.lineStride      = _instr->fields->DST_LINE_STRIDE * 144;
		tsr_desc.surfaceStride   = _instr->fields->DST_SURFACE_STRIDE * 144;
	}
	return tsr_desc;
}

void GCPYConverter::SetSrcDesc(TensorDesc tsr_desc) {
    if(_instr->fields->FUNC_OP == 0 || _instr->fields->FUNC_OP == 2) {  // C2CLoadTensor, Tensor2Weight
        _instr->fields->SRC_START_ADDR      = tsr_desc.address >> 4;
        _instr->fields->SRC_LINE_LENGTH     = (tsr_desc.lineLength >> 4) - 1;
        _instr->fields->SRC_LINE_NUM        = tsr_desc.lineNumber - 1;
        _instr->fields->SRC_LINE_STRIDE     = tsr_desc.lineStride >> 4;
        _instr->fields->SRC_SURFACE_NUM     = tsr_desc.surfaceNumber - 1;
        _instr->fields->SRC_SURFACE_STRIDE  = tsr_desc.surfaceStride >> 4;
    }
    else if(_instr->fields->FUNC_OP == 1) {  // C2CLoadWeight
        _instr->fields->SRC_START_ADDR      = tsr_desc.address/144;
        _instr->fields->SRC_LINE_LENGTH     = (tsr_desc.lineLength/144) - 1;
        _instr->fields->SRC_LINE_NUM        = tsr_desc.lineNumber - 1;
        _instr->fields->SRC_LINE_STRIDE     = tsr_desc.lineStride/144;
        _instr->fields->SRC_SURFACE_NUM     = tsr_desc.surfaceNumber - 1;
        _instr->fields->SRC_SURFACE_STRIDE  = tsr_desc.surfaceStride/144;
    }
}

void GCPYConverter::SetDstDesc(TensorDesc tsr_desc) {
    if(_instr->fields->FUNC_OP == 0) {  // C2CLoadTensor
        _instr->fields->DST_START_ADDR      = tsr_desc.address >> 4;
        _instr->fields->DST_LINE_LENGTH     = (tsr_desc.lineLength >> 4) - 1;
        _instr->fields->DST_LINE_NUM        = tsr_desc.lineNumber - 1;
        _instr->fields->DST_LINE_STRIDE     = tsr_desc.lineStride >> 4;
        _instr->fields->DST_SURFACE_NUM     = tsr_desc.surfaceNumber - 1;
        _instr->fields->DST_SURFACE_STRIDE  = tsr_desc.surfaceStride >> 4;
    }
    else if(_instr->fields->FUNC_OP == 1 || _instr->fields->FUNC_OP == 2) {  // Tensor2Weight, C2CLoadWeight
        _instr->fields->DST_START_ADDR      = tsr_desc.address/144;
        _instr->fields->DST_LINE_LENGTH     = (tsr_desc.lineLength/144) - 1;
        _instr->fields->DST_LINE_NUM        = tsr_desc.lineNumber - 1;
        _instr->fields->DST_LINE_STRIDE     = tsr_desc.lineStride/144;
        _instr->fields->DST_SURFACE_NUM     = tsr_desc.surfaceNumber - 1;
        _instr->fields->DST_SURFACE_STRIDE  = tsr_desc.surfaceStride/144;
    }
}

bool GCPYConverter::SettingArgs(std::string opcode, const std::vector<uint32_t> &args) {
    std::stringstream ss;
    ss << opcode;
    std::map<std::string, uint32_t> fields;
    assert(args.size()==gcpy_isa_namespace[opcode].size());
    
    for(int i = 0; i < (int)args.size();i++) {
        assert(args[i]<(1UL<<gcpy_isa_namespace[opcode][i].second));
        fields.insert(std::pair<std::string, uint32_t>(gcpy_isa_namespace[opcode][i].first,args[i]));
    }

    _instr->SetFields(fields);
    return true;
}

void GCPYConverter::Dump(std::string file_path) {
    std::ofstream fout(file_path + "Cmd_list.txt",std::ios::app);
    std::ostringstream ss;
    ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[3];
    ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[2];
    ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[1];
    ss << std::setw(16) << std::setfill('0') << std::hex << (*_bin)[0];
    ss <<"\n";
    assert(fout);
    fout << ss.str();
    fout.close();
}

std::string GCPYConverter::GetDetail(){
	std::stringstream ss;
	std::map<std::string, uint32_t> fields;
	_instr->GetFields(fields);
	ss << "GLBCPY" << "\n";
	for (int i = 0; i < int(gcpy_isa_namespace.size()); i++) {
		ss << gcpy_isa_namespace["GLBCPY"][i].first << " " << fields[gcpy_isa_namespace["GLBCPY"][i].first] << "\n";
	}
  	return ss.str();
}

ConverterBase* GCPYConverter::LoadText(std::string text) {
    SetByString(text);
    return this;
}

std::string GCPYConverter::ToText() {
    return GetString();
}

ConverterBase* GCPYConverter::LoadBinary(const std::vector<uint8_t> &bin) {
    SetBinary(bin8ToBin64(bin));
    return this;
}

std::vector<uint8_t> GCPYConverter::ToBinary() {
    return bin64ToBin8(GetBinary());
}

ConverterBase* GCPYConverter::LoadInstr(std::shared_ptr<InstrBase> instr) {
    SetByInstruction(std::dynamic_pointer_cast<GCPYInstruction>(instr));
    return this;
}

std::shared_ptr<InstrBase> GCPYConverter::ToInstr() {
    return _instr->Clone();
}
}