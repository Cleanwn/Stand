#include "rl_matching.hpp"

#include <cstring> // strcpy

#include <soup/ObfusString.hpp>

namespace rage
{
	rlMatchingFilter::rlMatchingFilter()
	{
		memset(this, 0, sizeof(*this));

		filter_id = ~1u; // AD_HOC_FILTER_ID

		strcpy(filter_name, soup::ObfusString("Group").c_str());

		attr_ids[0] = MMATTR_DISCRIMINATOR;
		attr_ids[1] = MMATTR_MM_GROUP_1;
		attr_ids[2] = MMATTR_MM_GROUP_2;
		attr_ids[3] = MMATTR_AIM_TYPE;
		attr_ids[4] = MMATTR_REGION;
		attr_ids[5] = MMATTR_ACTIVITY_TYPE;
		attr_ids[6] = MMATTR_ACTIVITY_ID;
		attr_ids[7] = MMATTR_ACTIVITY_PLAYERS;

		strcpy(attribute_names[0], soup::ObfusString("MMATTR_DISCRIMINATOR").c_str());
		strcpy(attribute_names[1], soup::ObfusString("MMATTR_MM_GROUP_1").c_str());
		strcpy(attribute_names[2], soup::ObfusString("MMATTR_MM_GROUP_2").c_str());
		strcpy(attribute_names[3], soup::ObfusString("MMATTR_AIM_TYPE").c_str());
		strcpy(attribute_names[4], soup::ObfusString("MMATTR_REGION").c_str());
		strcpy(attribute_names[5], soup::ObfusString("MMATTR_ACTIVITY_TYPE").c_str());
		strcpy(attribute_names[6], soup::ObfusString("MMATTR_ACTIVITY_ID").c_str());
		strcpy(attribute_names[7], soup::ObfusString("MMATTR_ACTIVITY_PLAYERS").c_str());

		num_attributes = 8;
	}

	void rlMatchingFilter::addCondition(uint32_t lefthand_attr_id, uint32_t oprtor, uint32_t righthand)
	{
		conditions_lefthand_attr_ids_index[num_conditions] = getAttrIdIdx(lefthand_attr_id);
		conditions_operators[num_conditions] = oprtor;
		conditions_param_ids[num_conditions] = lefthand_attr_id;
		conditions_righthand_values[num_conditions] = righthand;
		conditions_righthand_values_mask |= (1 << num_conditions);
		++num_conditions;
	}

	uint32_t rlMatchingFilter::getAttrIdIdx(uint32_t attr_id)
	{
		for (uint32_t i = 0; i != num_attributes; ++i)
		{
			if (attr_ids[i] == attr_id)
			{
				return i;
			}
		}
		throw 0;
	}
}
