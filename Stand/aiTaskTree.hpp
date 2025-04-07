#pragma once

#include "struct_base.hpp"

#include "atFixedArray.hpp"

namespace rage
{
	class aiTaskTree
	{
	public:
		virtual ~aiTaskTree() = default;

		/* 0x08 */ fwEntity* entity;
		/* 0x10 */ int32_t active_priority;
		PAD(0x14, 0x20) atFixedArray<aiTask*, 5> tasks;

		[[nodiscard]] aiTask* FindTaskByTypeActive(int32_t task_type) const
		{
			if (active_priority != -1)
			{
				return FindTaskByTypeWithPriority(task_type, active_priority);
			}
			return nullptr;
		}

		[[nodiscard]] aiTask* FindTaskByTypeWithPriority(int32_t task_type, int32_t priority) const;
	};
}
