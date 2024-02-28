#include "antoum/cmd_converter/vpu_converter.h"
#include <cassert>

namespace antoum {
static std::vector<VPUInstrFormat> g_vpu_instr_table = {
    {{"vpu_set_load_base","base_addr"}, {{"op",SET_LOAD_BASE}}},
    {{"vpu_set_load2_base","base_addr"}, {{"op",SET_LOAD2_BASE}}},
    {{"vpu_set_store_base","base_addr"}, {{"op",SET_STORE_BASE}}},
    {{"vpu_set_load_glb","stride","cluster_mask","bank_mask","cluster_broadcast","bank_broadcast"}, {{"op",SET_LOAD_GLB}}},
    {{"vpu_set_store_glb","stride","cluster_mask","bank_mask"}, {{"op",SET_STORE_GLB}}},
    {{"vpu_set_quantize","quantize"}, {{"op",SET_QUANTIZE}}},
    {{"vpu_set_dequantize","dequantize"}, {{"op",SET_DEQUANTIZE}}},
    {{"vpu_set_dequantize2","dequantize"}, {{"op",SET_DEQUANTIZE2}}},

    {{"vpu_nop"}, {{"op",V_NOP}}},
    {{"vpu_op_end"}, {{"op",V_OP_END}}},
    {{"vpu_fload_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"is_bf16",1}}},
    {{"vpu_fload_upper_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"is_bf16",1},{"upper",1}}},
    {{"vpu_fload_const","offset","rt"}, {{"op",V_LOAD_CONST},{"is_bf16",1}}},
    {{"vpu_fload_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"is_bf16",1}}},
    {{"vpu_fload_dual_upper_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"is_bf16",1},{"upper",1}}},
    {{"vpu_fstore_glb","rt","offset"}, {{"op",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fstore_upper_glb","rt","offset"}, {{"op",V_STORE_GLB},{"is_bf16",1},{"upper",1}}},
    {{"vpu_i16store_glb","rt","offset"}, {{"op",V_STORE_GLB},{"is_bf16",0},{"is_int16",1}}},
    {{"vpu_i16store_upper_glb","rt","offset"}, {{"op",V_STORE_GLB},{"is_bf16",0},{"is_int16",1},{"upper",1}}},

    {{"vpu_load_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_const","offset","rt"}, {{"op",V_LOAD_CONST},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_store_glb","rt","offset"}, {{"op",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},

    {{"vpu_uload_glb","offset","rt"}, {{"op",V_LOAD_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_const","offset","rt"}, {{"op",V_LOAD_CONST},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_glb","offset","rt","rs"}, {{"op",V_LOAD_DUAL_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_ustore_glb","rt","offset"}, {{"op",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},

    {{"vpu_fadd","rs","rt","rd"}, {{"op",V_ADD},{"has_imm",0}}},
    {{"vpu_fsub","rs","rt","rd"}, {{"op",V_SUB},{"has_imm",0}}},
    {{"vpu_fmul","rs","rt","rd"}, {{"op",V_MUL},{"has_imm",0}}},
    {{"vpu_fmax","rs","rt","rd"}, {{"op",V_MAX},{"has_imm",0}}},
    {{"vpu_fmin","rs","rt","rd"}, {{"op",V_MIN},{"has_imm",0}}},
    {{"vpu_greater","rs","rt"}, {{"op",V_GREATER},{"has_imm",0}}},
    {{"vpu_greater_equal","rs","rt"}, {{"op",V_GREATER_EQUAL},{"has_imm",0}}},
    {{"vpu_less","rs","rt"}, {{"op",V_LESS},{"has_imm",0}}},
    {{"vpu_less_equal","rs","rt"}, {{"op",V_LESS_EQUAL},{"has_imm",0}}},
    {{"vpu_equal","rs","rt"}, {{"op",V_EQUAL},{"has_imm",0}}},
    {{"vpu_sel","rs","rt","rd"}, {{"op",V_SEL},{"has_imm",0}}},

    {{"vpu_faddi","rs","imm","rd"}, {{"op",V_ADD},{"has_imm",1}}},
    {{"vpu_fsubi","rs","imm","rd"}, {{"op",V_SUB},{"has_imm",1}}},
    {{"vpu_fmuli","rs","imm","rd"}, {{"op",V_MUL},{"has_imm",1}}},
    {{"vpu_fmaxi","rs","imm","rd"}, {{"op",V_MAX},{"has_imm",1}}},
    {{"vpu_fmini","rs","imm","rd"}, {{"op",V_MIN},{"has_imm",1}}},
    {{"vpu_fmovi","imm","rd"}, {{"op",V_MOV},{"has_imm",1}}},
    {{"vpu_greateri","rs","imm"}, {{"op",V_GREATER},{"has_imm",1}}},
    {{"vpu_greater_equali","rs","imm"}, {{"op",V_GREATER_EQUAL},{"has_imm",1}}},
    {{"vpu_lessi","rs","imm"}, {{"op",V_LESS},{"has_imm",1}}},
    {{"vpu_less_equali","rs","imm"}, {{"op",V_LESS_EQUAL},{"has_imm",1}}},
    {{"vpu_equali","rs","imm"}, {{"op",V_EQUAL},{"has_imm",1}}},
    {{"vpu_seli","rs","imm","rd"}, {{"op",V_SEL},{"has_imm",1}}},
    {{"vpu_freduce_sum","rt"}, {{"op",V_REDUCE_SUM}}},

    // ================= load + compute (BF16) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"vpu_fload_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"is_bf16",1}}},
    {{"vpu_fload_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"is_bf16",1}}},
    {{"vpu_fload_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"is_bf16",1}}},
    {{"vpu_fload_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"is_bf16",1}}},
    {{"vpu_fload_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"is_bf16",1}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"vpu_fload_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"is_bf16",1}}},
    {{"vpu_fload_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"is_bf16",1}}},
    {{"vpu_fload_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"is_bf16",1}}},
    {{"vpu_fload_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"is_bf16",1}}},
    {{"vpu_fload_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"is_bf16",1}}},

    // ================= compute + store (BF16) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"vpu_fadd_fstore","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fsub_fstore","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fmul_fstore","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fmax_fstore","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fmin_fstore","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"is_bf16",1}}},

    // ================= load + compute + store (BF16) =================
    // GLB3 = GLB1 ※ GLB2
    {{"vpu_fload_dual_fadd_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fload_dual_fsub_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fload_dual_fmul_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fload_dual_fmax_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fload_dual_fmin_fstore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"vpu_fload_fadd_fstore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fload_fsub_fstore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fload_fmul_fstore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fload_fmax_fstore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"is_bf16",1}}},
    {{"vpu_fload_fmin_fstore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"is_bf16",1}}},

    // ================= load + compute (INT8) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"vpu_load_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"is_bf16",0},{"is_unsigned",0}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"vpu_load_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"is_bf16",0},{"is_unsigned",0}}},

    // ================= compute + store (INT8) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"vpu_fadd_store","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_fsub_store","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_fmul_store","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_fmax_store","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_fmin_store","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},

    // ================= load + compute + store (INT8) =================
    // GLB3 = GLB1 ※ GLB2
    {{"vpu_load_dual_fadd_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_fsub_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_fmul_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_fmax_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_dual_fmin_store","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"vpu_load_fadd_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_fsub_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_fmul_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_fmax_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},
    {{"vpu_load_fmin_store","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",0}}},

    // ================= load + compute (UINT8) =================
    // 𝑟𝑓𝑑 = GLB1 ※ GLB2
    {{"vpu_uload_dual_fadd","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_fsub","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_fmul","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_fmax","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_fmin","offset","rd"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"is_bf16",0},{"is_unsigned",1}}},
    // 𝑟𝑓𝑑 = GLB1 ※ 𝑟𝑓𝑠
    {{"vpu_uload_fadd","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_fsub","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_fmul","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_fmax","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_fmin","offset","rs","rd"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"is_bf16",0},{"is_unsigned",1}}},

    // ================= compute + store (UINT8) =================
    // 𝑟𝑓𝑑 = 𝑟𝑓𝑑 ※ 𝑟𝑓s, GLB3 = 𝑟𝑓𝑑
    {{"vpu_fadd_ustore","rs","rd","offset"}, {{"op",V_ADD},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_fsub_ustore","rs","rd","offset"}, {{"op",V_SUB},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_fmul_ustore","rs","rd","offset"}, {{"op",V_MUL},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_fmax_ustore","rs","rd","offset"}, {{"op",V_MAX},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_fmin_ustore","rs","rd","offset"}, {{"op",V_MIN},{"op2",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},

    // ================= load + compute + store (UINT8) =================
    // GLB3 = GLB1 ※ GLB2
    {{"vpu_uload_dual_fadd_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_fsub_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_fmul_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_fmax_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_dual_fmin_ustore","offset"}, {{"op",V_LOAD_DUAL_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    // GLB3 = GLB1 ※ 𝑟𝑓𝑡
    {{"vpu_uload_fadd_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_ADD},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_fsub_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_SUB},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_fmul_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MUL},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_fmax_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MAX},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
    {{"vpu_uload_fmin_ustore","offset","rt"}, {{"op",V_LOAD_GLB},{"op2",V_MIN},{"op3",V_STORE_GLB},{"is_bf16",0},{"is_unsigned",1}}},
};

VPUConverter::VPUConverter(std::shared_ptr<MFPara> para) : VPUConverter_t(para) {
    _instr = CREATE_CLASS_DPC(VPUInstruction_t, para, VPUInstruction);
}

std::map<std::string, std::vector<VPUInstrFormat*>> *VPUConverter::GetNameInstrMap() {
    static std::mutex mtx;
    static bool static_init = false;
    static std::map<std::string, std::vector<VPUInstrFormat*>> name_instr_map;

    if(static_init == false) {
        mtx.lock();
        if(static_init == false) {
            for(auto &i : g_vpu_instr_table) {
                name_instr_map[i.names[0]].push_back(&i);
            }
            static_init = true;
        }
        mtx.unlock();
    }
    return &name_instr_map;
}

std::map<VPUOpcode, std::vector<VPUInstrFormat*>> *VPUConverter::GetOpInstrMap() {
    static std::mutex mtx;
    static bool static_init = false;
    static std::map<VPUOpcode, std::vector<VPUInstrFormat*>> op_instr_map;

    if(static_init == false) {
        mtx.lock();
        if(static_init == false) {
            for(auto &i : g_vpu_instr_table) {
                op_instr_map[VPUOpcode(i.fixed_fields["op"])].push_back(&i);
            }
            static_init = true;
        }
        mtx.unlock();
    }
    return &op_instr_map;
}

ConverterBase* VPUConverter::SetText(std::string name, const std::vector<int> &args) {
    VPUInstrFormat *instr_form = NULL;
    auto &name_instr_map = *GetNameInstrMap();
    for(auto &i : name_instr_map[name]) {
        if(i->names.size() == (args.size()+1)) {
            instr_form = i;
            break;
        }
    }
    if(NULL == instr_form)
        return nullptr;

    std::map<std::string, uint32_t> fields(instr_form->fixed_fields);

    for(int i = 0; i < (int)args.size(); i++) {
        fields[instr_form->names[i+1]] = args[i];
    }
    _instr->SetFields(fields);
    return this;
}

ConverterBase* VPUConverter::SetText(std::string text) {
    std::vector<std::string> v;
    std::vector<int> args;
    split(text, v);

    for(int i = 1; i < (int)v.size(); i++) {
        args.emplace_back(std::stoi(v[i]));
    }
    return SetText(v[0], args);
}

std::string VPUConverter::GetText() {
    std::stringstream ss;
    VPUInstrFormat *instr_form = NULL;
    auto &op_instr_map = *GetOpInstrMap();

    std::map<std::string, uint32_t> fields;
    _instr->GetFields(fields);

    for(auto &i : op_instr_map[VPUOpcode(fields["op"])]) {
        bool unmatched = false;
        for(auto &j : i->fixed_fields) {
            if( fields.find(j.first) == fields.end() || j.second != fields[j.first]) {
                unmatched = true;
                break;
            }
        }
        if(false == unmatched && (NULL == instr_form || 
            instr_form->fixed_fields.size() < i->fixed_fields.size())) {
            instr_form = i;
        }
    }

    if(NULL != instr_form) {
        ss << instr_form->names[0];
        for(int i = 1; i < (int)instr_form->names.size(); i++) {
            ss << " ";
            ss << fields[instr_form->names[i]];
        }
    }

    return ss.str();
}

bool VPUConverter::SetBinary(VPUBinary_t bin) {
    assert(bin.size() == MemMapping.at(MM_VPU_CMD).cmd_width);
    _instr->SetFields({});

    if(0x00 == bin[3]) {
        _instr->fields->op = V_NOP;
    }
    else if(0xFF == bin[3]) {
        _instr->fields->op = V_OP_END;
    }
    else if(0x80 & bin[3]) {  // vpu
        uint32_t cmd_bin = ((uint32_t)bin[3]<<24) | ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
        if(0x40 & bin[3]) { // load/store
            _instr->fields->is_bf16 = 0x1 & (cmd_bin >> 29);
            _instr->fields->is_unsigned = 0x1 & (cmd_bin >> 28);
            _instr->fields->offset = 0xFFFF & cmd_bin;

            auto get_compute_op = [&] () {
                switch(0x7 & (cmd_bin >> 24)) {
                case 0: return V_ADD;
                case 1: return V_SUB;
                case 2: return V_MUL;
                case 3: return V_MAX;
                case 4: return V_MIN;
                }
                return V_INVALID;
            };

            if(0x1 & (cmd_bin >> 19)) { // super acceleration
                if(0x1 & (cmd_bin >> 17)) { // dual load
                    _instr->fields->op = V_LOAD_DUAL_GLB;
                }
                else { // single load
                    _instr->fields->op = V_LOAD_GLB;
                    _instr->fields->rt = 0x03 & (cmd_bin >> 20);
                }
                _instr->fields->op2 = get_compute_op();
                _instr->fields->op3 = V_STORE_GLB;
            }
            else if(0x01 & (cmd_bin >> 16)) { // secial acceleration
                _instr->fields->rd = 0x03 & (cmd_bin >> 22);
                if(0x01 & (cmd_bin) >> 27) { // store
                    _instr->fields->op = get_compute_op();
                    _instr->fields->op2 = V_STORE_GLB;
                    _instr->fields->rs = 0x03 & (cmd_bin >> 20);
                }
                else { // load
                    if(0x1 & (cmd_bin >> 17)) { // dual load
                        _instr->fields->op = V_LOAD_DUAL_GLB;
                    }
                    else { // single load
                        _instr->fields->op = V_LOAD_GLB;
                        _instr->fields->rs = 0x03 & (cmd_bin >> 20);
                    }
                    _instr->fields->op2 = get_compute_op();
                }
            }
            else {
                _instr->fields->is_int16 = 0x1 & (cmd_bin >> 25);
                _instr->fields->upper = 0x1 & (cmd_bin >> 24);
                _instr->fields->rt = 0x03 & (cmd_bin >> 20);
                if(0x01 & (cmd_bin) >> 27) { // store
                    _instr->fields->op = V_STORE_GLB;
                }
                else { // load
                    if(0x01 & (cmd_bin) >> 18) { // load const
                        _instr->fields->op = V_LOAD_CONST;
                    }
                    else if(0x01 & (cmd_bin) >> 17) { // dual load
                        _instr->fields->op = V_LOAD_DUAL_GLB;
                        _instr->fields->rs = 0x03 & (cmd_bin >> 22);
                    }
                    else { // single load
                        _instr->fields->op = V_LOAD_GLB;
                    }
                }
            }
        }
        else { // computation
            _instr->fields->has_imm = 0x1 & (cmd_bin >> 29);
            if(_instr->fields->has_imm) {
                _instr->fields->imm = 0xFFFF & cmd_bin;
            }
            else {
                _instr->fields->rt = 0x7 & (cmd_bin >> 15);
            }
            _instr->fields->rs = 0x7 & (cmd_bin >> 18);
            _instr->fields->rd = 0x7 & (cmd_bin >> 21);

            switch(0x1F & (cmd_bin >> 24)) {
            case 0x00: _instr->fields->op = V_ADD;            break;
            case 0x01: _instr->fields->op = V_SUB;            break;
            case 0x02: _instr->fields->op = V_MUL;            break;
            case 0x03: _instr->fields->op = V_MAX;            break;
            case 0x04: _instr->fields->op = V_MIN;            break;
            case 0x05: _instr->fields->op = V_MOV;            break;
            case 0x06: _instr->fields->op = V_REDUCE_SUM;     break;
            case 0x07: _instr->fields->op = V_GREATER;        break;
            case 0x08: _instr->fields->op = V_EQUAL;          break;
            case 0x09: _instr->fields->op = V_LESS;           break;
            case 0x0A: _instr->fields->op = V_GREATER_EQUAL;  break;
            case 0x0B: _instr->fields->op = V_LESS_EQUAL;     break;
            case 0x0C: _instr->fields->op = V_SEL;            break;
            }
        }
    }
    else { // configurable register
        uint32_t label = ((uint32_t)bin[2]<<16) | ((uint32_t)bin[1]<<8) | (uint32_t)bin[0];
        switch(0x7F & bin[3]) {
        case 1: {
            _instr->fields->op = SET_LOAD_BASE;
            _instr->fields->base_addr = label;
        }
        break;
        case 2: {
            _instr->fields->op = SET_LOAD_GLB;
            _instr->fields->stride = 0x7 & (label >> 21);
            _instr->fields->cluster_mask = 0xFF & (label >> 13);
            _instr->fields->bank_mask = 0xF & (label >> 9);
            _instr->fields->cluster_broadcast = 0x3 & (label >> 7);
            _instr->fields->bank_broadcast = 0x7 & (label >> 4);
        }
        break;
        case 3: {
            _instr->fields->op = SET_DEQUANTIZE;
            _instr->fields->dequantize = 0xFFFF & label;
        }
        break;
        case 4: {
            _instr->fields->op = SET_LOAD2_BASE;
            _instr->fields->base_addr = label;
        }
        break;
        case 6: {
            _instr->fields->op = SET_DEQUANTIZE2;
            _instr->fields->dequantize = 0xFFFF & label;
        }
        break;
        case 7: {
            _instr->fields->op = SET_STORE_BASE;
            _instr->fields->base_addr = label;
        }
        break;
        case 8: {
            _instr->fields->op = SET_STORE_GLB;
            _instr->fields->stride = 0x7 & (label >> 21);
            _instr->fields->cluster_mask = 0xFF & (label >> 13);
            _instr->fields->bank_mask = 0xF & (label >> 9);
        }
        break;
        case 9: {
            _instr->fields->op = SET_QUANTIZE;
            _instr->fields->quantize = 0xFFFF & label;
        }
        break;
        }
    }

    _instr->opcode = VPUOpcode(_instr->fields->op);
    _instr->opcode2 = VPUOpcode(_instr->fields->op2);
    _instr->opcode3 = VPUOpcode(_instr->fields->op3);
    return true;
}

VPUBinary_t VPUConverter::GetBinary() {
    VPUBinary_t bin(MemMapping.at(MM_VPU_CMD).cmd_width, 0);
    _instr->fields->op = _instr->opcode;
    _instr->fields->op2 = _instr->opcode2;
    _instr->fields->op3 = _instr->opcode3;

    if(_instr->fields->op == V_NOP) {
        bin[3] = 0x00;
    }
    else if(_instr->fields->op == V_OP_END) {
        bin[3] = 0xFF;
    }
    else if(_instr->fields->op >= V_LOAD_DUAL_GLB) {  // vpu
        uint32_t cmd_bin = 0x80000000;
        if(_instr->fields->op == V_LOAD_DUAL_GLB || _instr->fields->op == V_LOAD_GLB ||
            _instr->fields->op == V_LOAD_CONST || _instr->fields->op == V_STORE_GLB ||
            _instr->fields->op2 == V_STORE_GLB) { // load/store
            cmd_bin |= 0x40000000;
            cmd_bin |= (0x1 & (uint32_t)_instr->fields->is_bf16) << 29;
            cmd_bin |= (0x1 & (uint32_t)_instr->fields->is_unsigned) << 28;
            cmd_bin |= 0xFFFF & (uint32_t)_instr->fields->offset;

            auto get_compute_op = [&] (VPUOpcode opcode) -> uint32_t {
                switch(opcode) {
                case V_ADD: return (0x7 & 0) << 24;
                case V_SUB: return (0x7 & 1) << 24;
                case V_MUL: return (0x7 & 2) << 24;
                case V_MAX: return (0x7 & 3) << 24;
                case V_MIN: return (0x7 & 4) << 24;
                default:    return 0;
                }
            };

            if(_instr->fields->op != V_INVALID && _instr->fields->op3 != V_INVALID) { // super acceleration
                cmd_bin |= 0x1 << 19;
                cmd_bin |= get_compute_op(VPUOpcode(_instr->fields->op2));

                if(_instr->fields->op == V_LOAD_DUAL_GLB) { // dual load
                    cmd_bin |= 0x1 << 17;
                }
                else { // single load
                    cmd_bin |= (0x03 & (uint32_t)_instr->fields->rt) << 20;
                }
            }
            else if(_instr->fields->op != V_INVALID && _instr->fields->op2 != V_INVALID) { // secial acceleration
                cmd_bin |= 0x1 << 16;
                cmd_bin |= (0x03 & (uint32_t)_instr->fields->rd) << 22;
    
                if(_instr->fields->op2 == V_STORE_GLB) { // store
                    cmd_bin |= 0x1 << 27;
                    cmd_bin |= get_compute_op(VPUOpcode(_instr->fields->op));
                    cmd_bin |= (0x03 & (uint32_t)_instr->fields->rs) << 20;
                }
                else { // load
                    if(_instr->fields->op == V_LOAD_DUAL_GLB) { // dual load
                       cmd_bin |= 0x1 << 17;
                    }
                    else { // single load
                        cmd_bin |= (0x03 & (uint32_t)_instr->fields->rs) << 20;
                    }
                    cmd_bin |= get_compute_op(VPUOpcode(_instr->fields->op2));
                }
            }
            else {
                cmd_bin |= (0x01 & (uint32_t)_instr->fields->is_int16) << 25;
                cmd_bin |= (0x01 & (uint32_t)_instr->fields->upper) << 24;
                cmd_bin |= (0x03 & (uint32_t)_instr->fields->rt) << 20;

                if(_instr->fields->op == V_STORE_GLB) { // store
                    cmd_bin |= 0x1 << 27;
                }
                else { // load
                    if(_instr->fields->op == V_LOAD_CONST) { // load const
                        cmd_bin |= 0x1 << 18;
                    }
                    else if(_instr->fields->op == V_LOAD_DUAL_GLB) { // dual load
                        cmd_bin |= 0x1 << 17;
                        cmd_bin |= (0x03 & (uint32_t)_instr->fields->rs) << 22;
                    }
                }
            }
        }
        else { // computation
            cmd_bin |= (0x1 & (uint32_t)_instr->fields->has_imm) << 29;
            if(_instr->fields->has_imm) {
                cmd_bin |= 0xFFFF & (uint32_t)_instr->fields->imm;
            }
            else {
                cmd_bin |= (0x7 & (uint32_t)_instr->fields->rt) << 15;
            }
            cmd_bin |= (0x7 & (uint32_t)_instr->fields->rs) << 18;
            cmd_bin |= (0x7 & (uint32_t)_instr->fields->rd) << 21;

            switch(_instr->fields->op) {
            case V_ADD:            cmd_bin |= 0x00 << 24; break;
            case V_SUB:            cmd_bin |= 0x01 << 24; break;
            case V_MUL:            cmd_bin |= 0x02 << 24; break;
            case V_MAX:            cmd_bin |= 0x03 << 24; break;
            case V_MIN:            cmd_bin |= 0x04 << 24; break;
            case V_MOV:            cmd_bin |= 0x05 << 24; break;
            case V_REDUCE_SUM:     cmd_bin |= 0x06 << 24; break;
            case V_GREATER:        cmd_bin |= 0x07 << 24; break;
            case V_EQUAL:          cmd_bin |= 0x08 << 24; break;
            case V_LESS:           cmd_bin |= 0x09 << 24; break;
            case V_GREATER_EQUAL:  cmd_bin |= 0x0A << 24; break;
            case V_LESS_EQUAL:     cmd_bin |= 0x0B << 24; break;
            case V_SEL:            cmd_bin |= 0x0C << 24; break;
            }
        }
        bin[0] = 0xFF & (cmd_bin >> 0);
        bin[1] = 0xFF & (cmd_bin >> 8);
        bin[2] = 0xFF & (cmd_bin >> 16);
        bin[3] = 0xFF & (cmd_bin >> 24);
    }
    else { // configurable register
        uint32_t label = 0;
        switch(_instr->fields->op) {
        case SET_LOAD_BASE: {
            bin[3] |= 1;
            label = (uint32_t)_instr->fields->base_addr;
        }
        break;
        case SET_LOAD_GLB: {
            bin[3] |= 2;
            label |= (0x7 & (uint32_t)_instr->fields->stride) << 21;
            label |= (0xFF & (uint32_t)_instr->fields->cluster_mask) << 13;
            label |= (0xF & (uint32_t)_instr->fields->bank_mask) << 9;
            label |= (0x3 & (uint32_t)_instr->fields->cluster_broadcast) << 7;
            label |= (0x7 & (uint32_t)_instr->fields->bank_broadcast) << 4;
        }
        break;
        case SET_DEQUANTIZE: {
            bin[3] |= 3;
            label |= 0xFFFF & (uint32_t)_instr->fields->dequantize;
        }
        break;
        case SET_LOAD2_BASE: {
            bin[3] |= 4;
            label = (uint32_t)_instr->fields->base_addr;
        }
        break;
        case SET_DEQUANTIZE2: {
            bin[3] |= 6;
            label |= 0xFFFF & (uint32_t)_instr->fields->dequantize;
        }
        break;
        case SET_STORE_BASE: {
            bin[3] |= 7;
            label = (uint32_t)_instr->fields->base_addr;
        }
        break;
        case SET_STORE_GLB: {
            bin[3] |= 8;
            label |= (0x7 & (uint32_t)_instr->fields->stride) << 21;
            label |= (0xFF & (uint32_t)_instr->fields->cluster_mask) << 13;
            label |= (0xF & (uint32_t)_instr->fields->bank_mask) << 9;
        }
        break;
        case SET_QUANTIZE: {
            bin[3] |= 9;
            label |= 0xFFFF & (uint32_t)_instr->fields->quantize;
        }
        break;
        }
        bin[0] = 0xFF & (label >> 0);
        bin[1] = 0xFF & (label >> 8);
        bin[2] = 0xFF & (label >> 16);
    }

    return bin;
}

bool VPUConverter::Check() {
    assert(_instr->fields->cluster_broadcast <= 3);
    assert(_instr->fields->bank_broadcast <= 6);
    assert(_instr->fields->rs <= 3);
    assert(_instr->fields->rt <= 3);
    assert(_instr->fields->rd <= 3);
    return true;
}

ConverterBase* VPUConverter::LoadText(std::string text) { 
    SetText(text);
    return this;
}

std::string VPUConverter::ToText() { 
    return GetText();
}

ConverterBase* VPUConverter::LoadBinary(const std::vector<uint8_t> &bin) {
    SetBinary(bin);
    return this;
}

std::vector<uint8_t> VPUConverter::ToBinary() {
    return GetBinary();
}

ConverterBase* VPUConverter::LoadInstr(std::shared_ptr<InstrBase> instr) {
    _instr = std::dynamic_pointer_cast<VPUInstruction>(instr)->Clone();
    return this;
}

std::shared_ptr<InstrBase> VPUConverter::ToInstr() {
    Check();
    return _instr->Clone();
}

}