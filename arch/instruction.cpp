#include "instruction.h"
#include "antoum/instr_desc/instr_base.h"
#include "antoum/cmd_converter/ae_converter.h"
#include "antoum/cmd_converter/dma_converter.h"
#include "antoum/cmd_converter/gcpy_converter.h"
#include "antoum/cmd_converter/pea_converter.h"
#include "antoum/cmd_converter/te_converter.h"
#include "antoum/cmd_converter/tke_converter.h"
#include "antoum/cmd_converter/vpu_converter.h"

#include "quark/vpu_instr.h"
#include "quark/vpu_converter.h"

void registerInstructionClass() {
    REGISTER_CLASS(VPUInstrFields_t, antoum::VPUInstrFields, HW_VER1);
    REGISTER_CLASS(VPUOperation_t, antoum::VPUOperation, HW_VER1);
    REGISTER_CLASS(VPUInstruction_t, antoum::VPUInstruction, HW_VER1);
    REGISTER_CLASS(AEInstrFields_t, antoum::AEInstrFields, HW_VER1);
    REGISTER_CLASS(AEInstruction_t, antoum::AEInstruction, HW_VER1);
    REGISTER_CLASS(DMAInstruction_t, antoum::DMAInstruction, HW_VER1);
    REGISTER_CLASS(GCPYInstrFields_t, antoum::GCPYInstrFields, HW_VER1);
    REGISTER_CLASS(GCPYInstruction_t, antoum::GCPYInstruction, HW_VER1);
    REGISTER_CLASS(PEAInstruction_t, antoum::PEAInstruction, HW_VER1);
    REGISTER_CLASS(PEAInstrFields_t, antoum::PEAInstrFields, HW_VER1);
    REGISTER_CLASS(TKEInstrFields_t, antoum::TKEInstrFields, HW_VER1);
    REGISTER_CLASS(TKEInstruction_t, antoum::TKEInstruction, HW_VER1);
    REGISTER_CLASS(TEInstrFields_t, antoum::TEInstrFields, HW_VER1);
    REGISTER_CLASS(TEConfigFields_t, antoum::TEConfigFields, HW_VER1);
    REGISTER_CLASS(TEInstruction_t, antoum::TEInstruction, HW_VER1);
    REGISTER_CLASS(TEConfiguration_t, antoum::TEConfiguration, HW_VER1);

    REGISTER_CLASS(AEConverter_t, antoum::AEConverter, HW_VER1);
    REGISTER_CLASS(DMAConverter_t, antoum::DMAConverter, HW_VER1);
    REGISTER_CLASS(GCPYConverter_t, antoum::GCPYConverter, HW_VER1);
    REGISTER_CLASS(PEAConverter_t, antoum::PEAConverter, HW_VER1);
    REGISTER_CLASS(TEConverter_t, antoum::TEConverter, HW_VER1);
    REGISTER_CLASS(TKEConverter_t, antoum::TKEConverter, HW_VER1);
    REGISTER_CLASS(VPUConverter_t, antoum::VPUConverter, HW_VER1);

    REGISTER_CLASS(VPUInstrFields_t, quark::VPUInstrFields, HW_VER2_0);
    REGISTER_CLASS(VPUOperation_t, quark::VPUOperation, HW_VER2_0);
    REGISTER_CLASS(VPUInstruction_t, quark::VPUInstruction, HW_VER2_0);
    REGISTER_CLASS(VPUConverter_t, quark::VPUConverter, HW_VER2_0);
}
