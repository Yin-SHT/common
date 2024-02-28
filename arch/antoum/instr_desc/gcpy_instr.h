#ifndef __GCPY_INSTR_H
#define __GCPY_INSTR_H

#include "instr_base.h"

namespace antoum {
const uint32_t GCPY_MAX_LINE_LENGTH = (1 << 12) * 4;
const uint32_t GCPY_MAX_LINE_NUMBER = (1 << 11);
const uint32_t GCPY_MAX_SURFACE_NUMBER = (1 << 7);

struct GCPYInstrFields : public GCPYInstrFields_t{
  	GCPYInstrFields(std::shared_ptr<MFPara> para=nullptr) : GCPYInstrFields_t(para) {
    } 
	virtual ~GCPYInstrFields() = default;
    virtual std::shared_ptr<GCPYInstrFields> Clone() {
        auto ret = CREATE_CLASS_DPC(GCPYInstrFields_t, _para, GCPYInstrFields);
        *ret = *this;
        return ret;
    }    
	uint32_t SRC_CORE_ID : 2;
	uint32_t SRC_START_ADDR : 21;
	uint32_t SRC_LINE_LENGTH : 12;
	uint32_t SRC_LINE_NUM : 11;
	uint32_t SRC_LINE_STRIDE : 15;
	uint32_t SRC_SURFACE_NUM : 7;
	uint32_t SRC_SURFACE_STRIDE : 19;
	uint32_t DST_CORE_ID : 2;
	uint32_t DST_START_ADDR : 21;
	uint32_t DST_LINE_LENGTH : 12;
	uint32_t DST_LINE_NUM : 11;
	uint32_t DST_LINE_STRIDE : 15;
	uint32_t DST_SURFACE_NUM : 7;
	uint32_t DST_SURFACE_STRIDE : 19; 
	uint32_t PRECISION_MODE : 1;
	uint32_t FUNC_OP : 2;
	uint32_t SYNC : 1;
	uint32_t OP_END : 1;
};

class GCPYInstruction : public GCPYInstruction_t {
	public:
	std::string op_code;
	std::shared_ptr<GCPYInstrFields> fields;

	virtual void SetFields(const std::map<std::string, uint32_t> &field_map) {
		#define SET(x) do { auto it = field_map.find(#x); fields->x = (it != field_map.end()) ? it->second : 0; } while (false)
		SET(SRC_CORE_ID);
		SET(SRC_START_ADDR);
		SET(SRC_LINE_LENGTH);
		SET(SRC_LINE_NUM);
		SET(SRC_LINE_STRIDE);
		SET(SRC_SURFACE_NUM);
		SET(SRC_SURFACE_STRIDE);
		SET(DST_CORE_ID);
		SET(DST_START_ADDR);
		SET(DST_LINE_LENGTH);
		SET(DST_LINE_NUM);
		SET(DST_LINE_STRIDE);
		SET(DST_SURFACE_NUM);
		SET(DST_SURFACE_STRIDE);
		SET(PRECISION_MODE);
		SET(FUNC_OP);
		SET(SYNC);
		SET(OP_END);
		#undef SET
	}

	virtual void GetFields(std::map<std::string, uint32_t> &field_map) {
		#define GET(x) field_map[#x] = fields->x;
		GET(SRC_CORE_ID);
		GET(SRC_START_ADDR);
		GET(SRC_LINE_LENGTH);
		GET(SRC_LINE_NUM);
		GET(SRC_LINE_STRIDE);
		GET(SRC_SURFACE_NUM);
		GET(SRC_SURFACE_STRIDE);
		GET(DST_CORE_ID);
		GET(DST_START_ADDR);
		GET(DST_LINE_LENGTH);
		GET(DST_LINE_NUM);
		GET(DST_LINE_STRIDE);
		GET(DST_SURFACE_NUM);
		GET(DST_SURFACE_STRIDE);
		GET(PRECISION_MODE);
		GET(FUNC_OP);
		GET(SYNC);
		GET(OP_END);
		#undef GET
	}

	GCPYInstruction(std::shared_ptr<MFPara> para=nullptr) : GCPYInstruction_t(para) {
		fields = CREATE_CLASS_DPC(GCPYInstrFields_t, para, GCPYInstrFields); 
		SetFields({});
	}
	virtual ~GCPYInstruction() = default;

	virtual void DumpFields(std::map<std::string, uint32_t> &field_map) {
		GetFields(field_map);
	}
	virtual std::shared_ptr<GCPYInstruction> Clone() {
        auto ret = CREATE_CLASS_DPC(GCPYInstruction_t, _para, GCPYInstruction);
        ret->op_code = this->op_code;
        ret->fields = this->fields->Clone();
        return ret;
    }

	virtual uint32_t GetFuncOp() {
		return fields->FUNC_OP;
	}

	virtual uint32_t GetSync() {
		return fields->SYNC;
	}

	virtual uint32_t GetOpEnd() {
        return fields->OP_END;
    }
};

}

#endif
