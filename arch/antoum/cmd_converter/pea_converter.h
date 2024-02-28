/*
 * Filename: pea_converter.h
 * Author: Changxu Zhang
 * Description: PEArray Eninge Command Converter Header file
 */
#ifndef __PEA_CONVERTER_H
#define __PEA_CONVERTER_H

#include "converter_base.h"


typedef std::vector<uint64_t> PEABinary_t;

namespace antoum {
class PEAConverter : public PEAConverter_t {
public:
    PEAConverter(std::shared_ptr<MFPara> para=nullptr);
    virtual ~PEAConverter() = default;
    virtual bool SetText(std::string text);
    virtual std::string GetInstructionText();
    virtual std::vector<std::string> GetConfigurationText();
    virtual bool SetBinary(PEABinary_t bin);
    virtual PEABinary_t GetBinary();
    virtual bool SetInstruction(std::shared_ptr<PEAInstruction> instr);
    virtual bool SetConfiguration(const PEAConfiguration &instr);
    virtual std::shared_ptr<PEAInstruction> GetInstruction();
    virtual PEAConfiguration GetConfiguration();
    virtual void Dump(std::string file_path);
    virtual void Clear();
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
    std::shared_ptr<PEAInstruction> _instr;
    PEAConfiguration _config;
    PEABinary_t* _bin;
    std::map<std::string,std::vector<std::pair<std::string,uint32_t>>> pea_isa_namespace;
};

#define SettingPEA(cvter, ...) cvter->SettingArgs(__VA_ARGS__)

// #define ConfigConv2D(cvter,Rq,Rr,Sq,Sr,Cgb,Cgb_wr,Kq,Kr,Cq,Cr,nnz_cgb_cq,nnz_cgb_cr,precision_mode)\
//         SettingPEA(cvter,"CONV2D_CONFIG", {cvter,Rq,Rr,Sq,Sr,Cgb,Cgb_wr,Kq,Kr,Cq,Cr,nnz_cgb_cq,nnz_cgb_cr,precision_mode})
// #define ConfigQuant(cvter,bias_add_en,activation_en,element_op_en,activation_func,bias_start_addr,quant_scale_start_addr,quant_bias_start_addr,dequant_scale_start_addr)  
//         SettingPEA(cvter,"QUANT_CONFIG",{bias_add_en,activation_en,element_op_en,activation_func,bias_start_addr,quant_scale_start_addr,quant_bias_start_addr,dequant_scale_start_addr})
#define ConfigConv2D(cvter,params) SettingPEA(cvter,"CONV2D_CONFIG", params)
#define SetSrcAddress(cvter,params) SettingPEA(cvter,"CONV2D_SRC_ADDRESS",params)
#define SetDstAddress(cvter,params) SettingPEA(cvter,"CONV2D_DST_ADDRESS",params)
#define SetDstSize(cvter,params) SettingPEA(cvter,"CONV2D_DST_SIZE",params)
#define SetWeightAddress(cvter,params) SettingPEA(cvter,"CONV2D_WEIGHT_ADDRESS",params)
#define SetKernel(cvter,params) SettingPEA(cvter,"CONV2D_KERNEL",params)
#define SetChannel(cvter,params) SettingPEA(cvter,"CONV2D_CHANNEL",params)
#define SetStride(cvter,params) SettingPEA(cvter,"CONV2D_STRIDE",params)
#define SetPadding(cvter,params) SettingPEA(cvter,"CONV2D_PADDING",params)
#define SetSize(cvter,params) SettingPEA(cvter,"CONV2D_SIZE",params)
#define SetQuant(cvter,params) SettingPEA(cvter,"CONV2D_QUANT",params)
#define SetBias(cvter,params) SettingPEA(cvter,"CONV2D_BIAS",params)
#define SetActivation(cvter,params) SettingPEA(cvter,"CONV2D_ACTIV",params)
#define SetElementOp(cvter,params) SettingPEA(cvter,"CONV2D_ELEMENT_OP",params)
#define SetSum(cvter,params) SettingPEA(cvter,"CONV2D_SUM",params)
#define SetOpEnd(cvter,params) SettingPEA(cvter,"CONV2D_END",params)
#define SetPrecision(cvter,params) SettingPEA(cvter,"CONV2D_PRECISION",params)
#define GenInstr(cvter)  cvter->GetInstructionText()
#define GenConfig(cvter) cvter->GetConfigurationText()

}

#endif
