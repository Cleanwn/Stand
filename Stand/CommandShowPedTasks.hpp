#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "CPedIntelligence.hpp"
#include "gta_ped.hpp"
//#include "natives.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandShowPedTasks : public CommandToggle
	{
	public:
		explicit CommandShowPedTasks(CommandList* const parent)
			: CommandToggle(parent, LIT("Show Ped Tasks"), { CMDNAME("showpedtasks") })
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				std::vector<AbstractEntity> vec;
				AbstractEntity::getAllPeds(vec);
				for (AbstractEntity& ent : vec)
				{
					/*std::wstring from_native;
					for (int i = 0; i < 1000; i++)
					{
						if (TASK::GET_IS_TASK_ACTIVE(ent, i))
						{
							StringUtils::list_append(from_native, fmt::to_wstring(i));
						}
					}
					if (from_native.empty())
					{
						from_native = L"No tasks";
					}*/

					std::wstring from_memory;
					if (CTaskInfo* ti = ent.getCPed()->intelligence->task_info)
					{
						auto best_priority = ti->getPriority();
						for (; ti; ti = ti->next)
						{
							auto priority = ti->getPriority();
							if (best_priority < priority)
							{
								break;
							}
							best_priority = priority;
							std::wstring str = fmt::to_wstring(ti->task_id);
							/*str.push_back(L'[');
							str.append(fmt::to_wstring(ti->getPriority()));
							str.push_back(L']');*/
							StringUtils::list_append(from_memory, std::move(str));
						}
					}
					if (from_memory.empty())
					{
						from_memory = L"No tasks";
					}

					/*std::wstring tasks = L"From native: ";
					tasks.append(from_native);
					tasks.append(L"\nFrom memory: ");
					tasks.append(from_memory);
					g_renderer.draw3dTextThisTickH(ent.getPos(), std::move(tasks), 1.5);*/

					g_renderer.draw3dTextThisTickH(ent.getPos(), std::move(from_memory), 1.5);
				}
			});
		}
	};
}
