#ifndef __INSTR_BASE_H
#define __INSTR_BASE_H

#include "utils.h"
#include "hw_config.h"

class InstrBase : public MFObject {
public:
    InstrBase(std::shared_ptr<MFPara> para) : MFObject(para) {}
    virtual ~InstrBase() = default;
    virtual void DumpFields(std::map<std::string, uint32_t> &field_map) = 0;
};

class AEInstrFields_t : public MFObject {
    public:
        AEInstrFields_t(std::shared_ptr<MFPara> para) : MFObject(para) {};
};
class AEInstruction_t : public InstrBase {
    public:
        AEInstruction_t(std::shared_ptr<MFPara> para) : InstrBase(para) {};
};
class DMAInstruction_t : public InstrBase {
    public:
        DMAInstruction_t(std::shared_ptr<MFPara> para) : InstrBase(para) {};
};
class GCPYInstrFields_t : public MFObject {
    public:
        GCPYInstrFields_t(std::shared_ptr<MFPara> para) : MFObject(para) {};
};
class GCPYInstruction_t : public InstrBase {
    public:
        GCPYInstruction_t(std::shared_ptr<MFPara> para) : InstrBase(para) {};
};
class PEAInstrFields_t : public MFObject {
    public:
        PEAInstrFields_t(std::shared_ptr<MFPara> para) : MFObject(para) {};
};
class PEAInstruction_t : public InstrBase {
    public:
        PEAInstruction_t(std::shared_ptr<MFPara> para) : InstrBase(para) {};
};
class TKEInstrFields_t : public MFObject {
    public:
        TKEInstrFields_t(std::shared_ptr<MFPara> para) : MFObject(para) {};
};
class TKEInstruction_t : public InstrBase {
    public:
        TKEInstruction_t(std::shared_ptr<MFPara> para) : InstrBase(para) {};
};
class TEInstrFields_t : public MFObject {
    public:
        TEInstrFields_t(std::shared_ptr<MFPara> para) : MFObject(para) {};
};
class TEConfigFields_t : public MFObject {
    public:
        TEConfigFields_t(std::shared_ptr<MFPara> para) : MFObject(para) {};
};
class TEInstruction_t : public InstrBase {
    public:
        TEInstruction_t(std::shared_ptr<MFPara> para) : InstrBase(para) {};
};
class TEConfiguration_t : public MFObject {
    public:
        TEConfiguration_t(std::shared_ptr<MFPara> para) : MFObject(para) {};
};

#endif
