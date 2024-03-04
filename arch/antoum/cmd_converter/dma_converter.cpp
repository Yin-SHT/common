#include "antoum/cmd_converter/dma_converter.h"
#include <regex>
#include <iostream>

namespace antoum {

DMAConverter::DMAConverter(std::shared_ptr<MFPara> para) : DMAConverter_t(para), 
    _instr_mem(kSizePerDmaCmd, 0) {
    _instr = CREATE_CLASS_DPC(DMAInstruction_t, para, DMAInstruction);
    _instr_map = {
        FieldMappingEntry(&_instr_mem[0x00], 0, 0, &_instr->OP_END,                   0,  0),
        FieldMappingEntry(&_instr_mem[0x00], 3, 1, &_instr->CONST_BROADCAST_DUP_NUM,  2,  0),
        FieldMappingEntry(&_instr_mem[0x00], 4, 4, &_instr->TENSOR_FORMAT_CONVERT_EN, 0,  0),
        FieldMappingEntry(&_instr_mem[0x00], 5, 5, &_instr->CHANNEL_EXT_EN,           0,  0),
        FieldMappingEntry(&_instr_mem[0x00], 7, 7, &_instr->TENSOR_Cgb,               0,  0),
        FieldMappingEntry(&_instr_mem[0x01], 1, 0, &_instr->TENSOR_Cgb,               2,  1),
        FieldMappingEntry(&_instr_mem[0x01], 7, 2, &_instr->SRC_LINE_LENGTH,          5,  0),
        FieldMappingEntry(&_instr_mem[0x02], 7, 0, &_instr->SRC_LINE_LENGTH,         13,  6),
        FieldMappingEntry(&_instr_mem[0x03], 7, 0, &_instr->SRC_START_ADDR,           7,  0),
        FieldMappingEntry(&_instr_mem[0x04], 7, 0, &_instr->SRC_START_ADDR,          15,  8),
        FieldMappingEntry(&_instr_mem[0x05], 7, 0, &_instr->SRC_START_ADDR,          23, 16),
        FieldMappingEntry(&_instr_mem[0x06], 7, 0, &_instr->SRC_START_ADDR,          31, 24),
        FieldMappingEntry(&_instr_mem[0x07], 7, 0, &_instr->OP_CODE,                  7,  0),
        FieldMappingEntry(&_instr_mem[0x08], 3, 0, &_instr->SRC_DST_MODE,             3,  0),
        FieldMappingEntry(&_instr_mem[0x09], 7, 0, &_instr->SRC_SURFACE_STRIDE,       7,  0),
        FieldMappingEntry(&_instr_mem[0x0A], 7, 0, &_instr->SRC_SURFACE_STRIDE,      15,  8),
        FieldMappingEntry(&_instr_mem[0x0B], 4, 0, &_instr->SRC_SURFACE_STRIDE,      20, 16),
        FieldMappingEntry(&_instr_mem[0x0B], 7, 5, &_instr->SRC_SURFACE_NUMBER,       2,  0),
        FieldMappingEntry(&_instr_mem[0x0C], 3, 0, &_instr->SRC_SURFACE_NUMBER,       6,  3),
        FieldMappingEntry(&_instr_mem[0x0C], 7, 4, &_instr->SRC_LINE_STRIDE,          3,  0),
        FieldMappingEntry(&_instr_mem[0x0D], 7, 0, &_instr->SRC_LINE_STRIDE,         11,  4),
        FieldMappingEntry(&_instr_mem[0x0E], 4, 0, &_instr->SRC_LINE_STRIDE,         16, 12),
        FieldMappingEntry(&_instr_mem[0x0E], 7, 5, &_instr->SRC_LINE_NUMBER,          2,  0),
        FieldMappingEntry(&_instr_mem[0x0F], 7, 0, &_instr->SRC_LINE_NUMBER,         10,  3),
        FieldMappingEntry(&_instr_mem[0x12], 7, 2, &_instr->DST_LINE_LENGTH,          5,  0),
        FieldMappingEntry(&_instr_mem[0x13], 7, 0, &_instr->DST_LINE_LENGTH,         13,  6),
        FieldMappingEntry(&_instr_mem[0x14], 7, 0, &_instr->DST_START_ADDR,           7,  0),
        FieldMappingEntry(&_instr_mem[0x15], 7, 0, &_instr->DST_START_ADDR,          15,  8),
        FieldMappingEntry(&_instr_mem[0x16], 7, 0, &_instr->DST_START_ADDR,          23, 16),
        FieldMappingEntry(&_instr_mem[0x17], 7, 0, &_instr->DST_START_ADDR,          31, 24),
        FieldMappingEntry(&_instr_mem[0x19], 7, 0, &_instr->DST_SURFACE_STRIDE,       7,  0),
        FieldMappingEntry(&_instr_mem[0x1A], 7, 0, &_instr->DST_SURFACE_STRIDE,      15,  8),
        FieldMappingEntry(&_instr_mem[0x1B], 4, 0, &_instr->DST_SURFACE_STRIDE,      20, 16),
        FieldMappingEntry(&_instr_mem[0x1B], 7, 5, &_instr->DST_SURFACE_NUMBER,       2,  0),
        FieldMappingEntry(&_instr_mem[0x1C], 3, 0, &_instr->DST_SURFACE_NUMBER,       6,  3),
        FieldMappingEntry(&_instr_mem[0x1C], 7, 4, &_instr->DST_LINE_STRIDE,          3,  0),
        FieldMappingEntry(&_instr_mem[0x1D], 7, 0, &_instr->DST_LINE_STRIDE,         11,  4),
        FieldMappingEntry(&_instr_mem[0x1E], 4, 0, &_instr->DST_LINE_STRIDE,         16, 12),
        FieldMappingEntry(&_instr_mem[0x1E], 7, 5, &_instr->DST_LINE_NUMBER,          2,  0),
        FieldMappingEntry(&_instr_mem[0x1F], 7, 0, &_instr->DST_LINE_NUMBER,         10,  3),
    };
}

void DMAConverter::SetNOP(uint64_t srcAddr, uint64_t dstAddr) {
    this->LoadBinary(DMABinary_t(kSizePerDmaCmd, 0));
    _instr->OP_CODE = (size_t)IsaOpcode::DMAMV;
    _instr->SRC_DST_MODE = DMAMV_DDR_TO_DDR;
    _instr->SRC_LINE_LENGTH = 16/4 - 1;
    _instr->DST_LINE_LENGTH = 16/4 - 1;
    _instr->SRC_START_ADDR = (kCoreDdrAddr+srcAddr)/4;
    _instr->DST_START_ADDR = (kCoreDdrAddr+dstAddr)/4;
}

bool DMAConverter::SetField(std::string name,uint32_t val) {
    std::map<std::string, uint32_t> fields;
    fields[name] = val;
    _instr->SetFields(fields);
    return true;
}

bool DMAConverter::SliceLength(size_t length, size_t aligned_num, 
    size_t &line_number, size_t &line_length, bool slice_cmd) {
    for(line_number = 1; line_number <= DMA_MAX_LINE_NUMBER; line_number++) {
        if(line_number <= 2 || (line_number%4) == 0) {
            line_length = (length+line_number-1)/line_number;
            if(line_length <= DMA_MAX_LINE_LENGTH && (line_length%aligned_num)==0 &&
                (line_length*line_number) == length) {
                return true;
            }
        }
    }
    if(slice_cmd == false) {
        for(line_number = 1; line_number <= DMA_MAX_LINE_NUMBER; line_number++) {
            line_length = (length+line_number-1)/line_number;
            if(line_length <= DMA_MAX_LINE_LENGTH && (line_length%aligned_num)==0 &&
                (line_length*line_number) == length) {
                return true;
            }
        }
        for(line_number = 1; line_number <= DMA_MAX_LINE_NUMBER; line_number++) {
            line_length = (length+line_number-1)/line_number;
            if(line_length <= DMA_MAX_LINE_LENGTH && (line_length%aligned_num)==0) {
                return true;
            }
        }
        for(line_number = 1; line_number <= DMA_MAX_LINE_NUMBER; line_number++) {
            line_length = (length+line_number-1)/line_number;
            line_length = (line_length+aligned_num-1)/aligned_num*aligned_num;
            if(line_length <= DMA_MAX_LINE_LENGTH)
                return true;
        }
    }
    return false;
}

bool DMAConverter::SliceLength(size_t length, size_t aligned_num, 
    size_t &line_number, size_t &line_length, size_t &surface_number, bool assert_en) {
    for(surface_number = 1; surface_number <= DMA_MAX_SURFACE_NUMBER; surface_number++) {
        SliceLength(length/surface_number, aligned_num, line_number, line_length);
        if((line_length%aligned_num) == 0 && (surface_number*line_number*line_length) == length) {
            return true;
        }
    }
    for(surface_number = 1; surface_number <= DMA_MAX_SURFACE_NUMBER; surface_number++) {
        SliceLength(length/surface_number, aligned_num, line_number, line_length, false);
        if((line_length%aligned_num) == 0 && (surface_number*line_number*line_length) == length) {
            return true;
        }
    }
    if(assert_en) {
        assert(0 && "SliceLength failed");
    }
    return false;
}

std::vector<std::string>* DMAConverter::GetFieldNameList() {
    static std::vector<std::string> dma_mv_format = { 
        "dma_mv",
        "SRC_START_ADDR",
        "SRC_LINE_LENGTH",
        "SRC_LINE_NUMBER",
        "SRC_LINE_STRIDE",
        "SRC_SURFACE_NUMBER",
        "SRC_SURFACE_STRIDE",
        "SRC_DST_MODE",
        "CHANNEL_EXT_EN",
        "TENSOR_FORMAT_CONVERT_EN",
        "CONST_BROADCAST_DUP_NUM",
        "TENSOR_Cgb",
        "DST_START_ADDR",
        "DST_LINE_LENGTH",
        "DST_LINE_NUMBER",
        "DST_LINE_STRIDE",
        "DST_SURFACE_NUMBER",
        "DST_SURFACE_STRIDE",
        "OP_END",
    };
    return &dma_mv_format;
}

size_t DMAConverter::GetWeightGranularity() {
    return 144;
}

TensorDesc DMAConverter::GetSrcDesc() {
    TensorDesc tsr_desc;
    tsr_desc.address           = 4UL * _instr->SRC_START_ADDR;
    tsr_desc.lineLength        = 4UL * _instr->SRC_LINE_LENGTH + 4;
    tsr_desc.lineNumber        = 1UL + _instr->SRC_LINE_NUMBER;
    tsr_desc.lineStride        = 4UL * _instr->SRC_LINE_STRIDE;
    tsr_desc.surfaceNumber     = 1UL + _instr->SRC_SURFACE_NUMBER;
    tsr_desc.surfaceStride     = 4UL * _instr->SRC_SURFACE_STRIDE;
    return tsr_desc;
}

TensorDesc DMAConverter::GetDstDesc() {
    TensorDesc tsr_desc;
    if(DMAMV_DDR_TO_GLBW==_instr->SRC_DST_MODE || DMAMV_DTCM_TO_GLBW==_instr->SRC_DST_MODE) {
        tsr_desc.address           = GetWeightGranularity() * _instr->DST_START_ADDR;
        tsr_desc.lineLength        = GetWeightGranularity() * _instr->DST_LINE_LENGTH + GetWeightGranularity();
        tsr_desc.lineNumber        = 1UL + _instr->DST_LINE_NUMBER;
        tsr_desc.lineStride        = GetWeightGranularity() * _instr->DST_LINE_STRIDE;
        tsr_desc.surfaceNumber     = 1UL + _instr->DST_SURFACE_NUMBER;
        tsr_desc.surfaceStride     = GetWeightGranularity() * _instr->DST_SURFACE_STRIDE;
    }
    else {
        tsr_desc.address           = 4UL * _instr->DST_START_ADDR;
        tsr_desc.lineLength        = 4UL * _instr->DST_LINE_LENGTH + 4;
        tsr_desc.lineNumber        = 1UL + _instr->DST_LINE_NUMBER;
        tsr_desc.lineStride        = 4UL * _instr->DST_LINE_STRIDE;
        tsr_desc.surfaceNumber     = 1UL + _instr->DST_SURFACE_NUMBER;
        tsr_desc.surfaceStride     = 4UL * _instr->DST_SURFACE_STRIDE;
    }
    return tsr_desc;
}

void DMAConverter::SetSrcDesc(TensorDesc tsr_desc) {
    _instr->SRC_START_ADDR = tsr_desc.address/4;
    _instr->SRC_LINE_LENGTH = tsr_desc.lineLength/4 - 1;
    _instr->SRC_LINE_NUMBER = tsr_desc.lineNumber - 1;
    _instr->SRC_LINE_STRIDE = tsr_desc.lineStride/4;
    _instr->SRC_SURFACE_NUMBER = tsr_desc.surfaceNumber - 1;
    _instr->SRC_SURFACE_STRIDE = tsr_desc.surfaceStride/4;
}

void DMAConverter::SetDstDesc(TensorDesc tsr_desc) {
    if(DMAMV_DDR_TO_GLBW==_instr->SRC_DST_MODE || DMAMV_DTCM_TO_GLBW==_instr->SRC_DST_MODE) {
        _instr->DST_START_ADDR = tsr_desc.address/GetWeightGranularity();
        _instr->DST_LINE_LENGTH = tsr_desc.lineLength/GetWeightGranularity() - 1;
        _instr->DST_LINE_NUMBER = tsr_desc.lineNumber - 1;
        _instr->DST_LINE_STRIDE = tsr_desc.lineStride/GetWeightGranularity();
        _instr->DST_SURFACE_NUMBER = tsr_desc.surfaceNumber - 1;
        _instr->DST_SURFACE_STRIDE = tsr_desc.surfaceStride/GetWeightGranularity();
    }
    else {
        _instr->DST_START_ADDR = tsr_desc.address/4;
        _instr->DST_LINE_LENGTH = tsr_desc.lineLength/4 - 1;
        _instr->DST_LINE_NUMBER = tsr_desc.lineNumber - 1;
        _instr->DST_LINE_STRIDE = tsr_desc.lineStride/4;
        _instr->DST_SURFACE_NUMBER = tsr_desc.surfaceNumber - 1;
        _instr->DST_SURFACE_STRIDE = tsr_desc.surfaceStride/4;
    }
}

void DMAConverter::SetSrcLength(uint32_t length, uint32_t aligned_num) {
    size_t line_number, line_length;
    SliceLength(length, aligned_num, line_number, line_length);
    _instr->SRC_LINE_LENGTH = line_length/4 - 1;
    _instr->SRC_LINE_NUMBER = line_number - 1;
    _instr->SRC_LINE_STRIDE = line_length/4;
}

void DMAConverter::SetDstLength(uint32_t length, uint32_t aligned_num) {
    size_t line_number, line_length;
    SliceLength(length, aligned_num, line_number, line_length);
    _instr->DST_LINE_LENGTH = line_length/4 - 1;
    _instr->DST_LINE_NUMBER = line_number - 1;
    _instr->SRC_LINE_STRIDE = line_length/4;
}

bool DMAConverter::Check() {
    auto src_desc = GetSrcDesc();
    auto dst_desc = GetDstDesc();

    CHECK((src_desc.address % 16) == 0);
    CHECK((src_desc.lineLength % 16) == 0);
    CHECK((src_desc.lineStride % 4) == 0);
    CHECK((src_desc.surfaceStride % 4) == 0);
    CHECK((dst_desc.address % 16) == 0);
    CHECK((dst_desc.lineLength % 16) == 0);
    CHECK((dst_desc.lineStride % 4) == 0);
    CHECK((dst_desc.surfaceStride % 4) == 0);
    CHECK(_instr->SRC_DST_MODE <= 12);
    CHECK(_instr->TENSOR_FORMAT_CONVERT_EN == 0 || _instr->TENSOR_Cgb <= 2);
    CHECK(_instr->CONST_BROADCAST_DUP_NUM <= 5);
    CHECK(_instr->CONST_BROADCAST_DUP_NUM != 1);
    CHECK(_instr->CONST_BROADCAST_DUP_NUM != 2);
    CHECK(_instr->CHANNEL_EXT_EN > 0 || 
        src_desc.lineLength*src_desc.lineNumber*src_desc.surfaceNumber == 
        dst_desc.lineLength*dst_desc.lineNumber*dst_desc.surfaceNumber);
    CHECK(_instr->CHANNEL_EXT_EN == 0 || dst_desc.surfaceNumber == 1);
    CHECK(ToText() == CREATE_CLASS_DPC(DMAConverter_t, _para, DMAConverter)->LoadBinary(ToBinary())->ToText());

    switch(DMASrcDstMode(_instr->SRC_DST_MODE)) {
    case DMAMV_DDR_TO_GLB:
    case DMAMV_DTCM_TO_GLB:
        if(_instr->TENSOR_FORMAT_CONVERT_EN) {
            CHECK((dst_desc.lineLength % 512) == 0);
        } else {
            CHECK((dst_desc.lineLength % 64) == 0);
        }
        break;
    case DMAMV_GLB_TO_DDR:
    case DMAMV_GLB_TO_DTCM:
        if(_instr->TENSOR_FORMAT_CONVERT_EN) {
            CHECK((src_desc.lineLength % 512) == 0);
        } else {
            CHECK((src_desc.lineLength % 64) == 0);
        }
        break;
    case DMAMV_DDR_TO_GLBW:
    case DMAMV_DTCM_TO_GLBW:
        CHECK((dst_desc.address % GetWeightGranularity()) == 0);
        CHECK((dst_desc.lineLength % GetWeightGranularity()) == 0);
        CHECK((dst_desc.lineStride % GetWeightGranularity()) == 0);
        CHECK((dst_desc.surfaceStride % GetWeightGranularity()) == 0);
        break;
    case DMAMV_DDR_TO_CMD:
    case DMAMV_DTCM_TO_CMD:
        CHECK(dst_desc.lineNumber <= 2 || (dst_desc.lineNumber % 4) == 0);
        break;
    default:
        break;
    }
    return true;
}

ConverterBase* DMAConverter::LoadText(std::string text) {
    std::vector<std::string> v;
    std::vector<std::string> *instr_form = GetFieldNameList();
    split(text, v);

    if(v.size() > 0) {
        _instr->OP_CODE = (v[0]=="dma_mv") ?(size_t)IsaOpcode::DMAMV :(size_t)IsaOpcode::NOP;
    }

    std::map<std::string, uint32_t> fields;

    for(int i = 1; i < (int)v.size(); i++) {
        fields[instr_form->at(i)] = std::stol(v[i]);
    }
    _instr->SetFields(fields);
    return this;
}

std::string DMAConverter::ToText() {
    std::stringstream ss;
    std::vector<std::string> *instr_form = GetFieldNameList();

    //if(_instr->OP_CODE == NOP_OP_CODE)
    //    return "nop";

    std::map<std::string, uint32_t> fields;
    _instr->GetFields(fields);

    if(NULL != instr_form) {
        ss << instr_form->at(0);
        for(size_t i = 1; i < instr_form->size(); i++) {
            ss << " " << fields[instr_form->at(i)];
        }
    }
    return ss.str();
}

ConverterBase* DMAConverter::LoadBinary(const std::vector<uint8_t> &bin) {
    assert(bin.size() == _instr_mem.size());
    for(size_t i = 0; i < bin.size(); i++) {
        _instr_mem[i] = bin[i];
    }
    for(auto &it : _instr_map) {
        it.MemToField();
    }
    assert(_instr->OP_CODE==(size_t)IsaOpcode::DMAMV || _instr->OP_CODE==(size_t)IsaOpcode::NOP);
    return this;
}

std::vector<uint8_t> DMAConverter::ToBinary() {
    for(auto &it : _instr_map) {
        it.FieldToMem();
    }
    return _instr_mem;
}

ConverterBase* DMAConverter::LoadInstr(std::shared_ptr<InstrBase> instr) {
    *_instr = *std::dynamic_pointer_cast<DMAInstruction>(instr);
    return this;
}

std::shared_ptr<InstrBase> DMAConverter::ToInstr() {
    Check();
    return _instr->Clone();
}

int64_t DMAConverter::EvalCycleNum(int64_t line_length) {
    float cycle_num = 0;
    for(int64_t i = 0; i < line_length; i+= kDmaTransferLength) {
        int64_t cur_length = std::min((int64_t)kDmaTransferLength, line_length-i);
        cycle_num += kDmaExtraLatency;
        cycle_num += (float)cur_length/kDmaBandwidthToDdr;
    }
    return (int64_t)cycle_num;
}

int64_t DMAConverter::EvalCycleNum() {
    int64_t cycle_num = 0;
    auto src_desc = GetSrcDesc();
    auto dst_desc = GetDstDesc();

    auto eval_ddr = [&] (TensorDesc tsr_desc) {
        int64_t ret_cycle = 0;
        for(uint32_t sur = 0; sur < tsr_desc.surfaceNumber; sur++) {
            for(uint32_t line = 0; line < tsr_desc.lineNumber; line++) {
                ret_cycle += EvalCycleNum(tsr_desc.lineLength);
            }
        }
        return ret_cycle;
    };
    auto eval_glb = [] (TensorDesc tsr_desc) {
        return (int64_t)tsr_desc.surfaceNumber*tsr_desc.lineNumber*tsr_desc.lineLength/64;
    };

    switch(_instr->SRC_DST_MODE) {
    case DMAMV_DDR_TO_GLB:
    case DMAMV_DTCM_TO_GLB:
        cycle_num += std::max(eval_ddr(src_desc), eval_glb(dst_desc));
        break;
    case DMAMV_GLB_TO_DDR:
    case DMAMV_GLB_TO_DTCM:
        cycle_num += std::max(eval_glb(src_desc), eval_ddr(dst_desc));
        break;
    default:
        cycle_num += eval_ddr(src_desc);
        break;
    }
    return cycle_num;
}
}