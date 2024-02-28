#ifndef __DMA_CONVERTER_H
#define __DMA_CONVERTER_H

#include "converter_base.h"

typedef std::vector<uint8_t> DMABinary_t;

namespace antoum {
class DMAConverter : public DMAConverter_t {
public:
    DMAConverter(std::shared_ptr<MFPara> para=nullptr);
    virtual ~DMAConverter() = default;
    virtual void SetNOP(uint64_t srcAddr, uint64_t dstAddr);
    virtual bool SetField(std::string name,uint32_t val);

    static bool SliceLength(size_t length, size_t aligned_num, 
        size_t &line_number, size_t &line_length, bool slice_cmd=true);
    static bool SliceLength(size_t length, size_t aligned_num, 
        size_t &line_number, size_t &line_length, size_t &surface_number, bool assert_en=true);

    virtual std::vector<std::string>* GetFieldNameList();
    virtual size_t GetWeightGranularity();
    virtual TensorDesc GetSrcDesc();
    virtual TensorDesc GetDstDesc();
    virtual void SetSrcDesc(TensorDesc tsr_desc);
    virtual void SetDstDesc(TensorDesc tsr_desc);
    virtual void SetSrcLength(uint32_t length, uint32_t aligned_num=4);
    virtual void SetDstLength(uint32_t length, uint32_t aligned_num=4);

    virtual bool Check();

    virtual ConverterBase* LoadText(std::string text);
    virtual std::string ToText();
    virtual ConverterBase* LoadBinary(const std::vector<uint8_t> &bin);
    virtual std::vector<uint8_t> ToBinary();
    virtual ConverterBase* LoadInstr(std::shared_ptr<InstrBase> instr);
    virtual std::shared_ptr<InstrBase> ToInstr();
    virtual int64_t EvalCycleNum(int64_t line_length);
    virtual int64_t EvalCycleNum();

    static const int OP_END_POS = 0x00;
    static const int OP_END_MSK = 0x01;
    static const int SRC_START_ADDR_POS = 0x03;
    static const int DST_START_ADDR_POS = 0x14;

protected:
    std::shared_ptr<DMAInstruction> _instr;
    std::vector<uint8_t> _instr_mem;
    std::vector<FieldMappingEntry> _instr_map;
};
}
#endif
