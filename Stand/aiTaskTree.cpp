#include "aiTaskTree.hpp"

#include "gta_task.hpp"

namespace rage
{
	aiTask* aiTaskTree::FindTaskByTypeWithPriority(int32_t task_type, int32_t priority) const
	{
		for (aiTask* task = tasks.elements[priority]; task != nullptr; task = task->sub_task)
		{
			if (task->type_id == task_type)
			{
				return task;
			}
		}
		return nullptr;
	}
}
