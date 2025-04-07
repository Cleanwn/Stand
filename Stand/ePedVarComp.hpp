#pragma once

#include <cstdint>

using ePedVarComp_t = uint32_t;

enum ePedVarComp
{
	PV_COMP_INVALID = -1,
	PV_COMP_HEAD = 0,
	PV_COMP_BERD,
	PV_COMP_HAIR,
	PV_COMP_UPPR,
	PV_COMP_LOWR,
	PV_COMP_HAND,
	PV_COMP_FEET,
	PV_COMP_TEEF,
	PV_COMP_ACCS,
	PV_COMP_TASK,
	PV_COMP_DECL,
	PV_COMP_JBIB,
	PV_MAX_COMP
};
