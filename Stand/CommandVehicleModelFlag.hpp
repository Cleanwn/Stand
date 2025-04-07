#pragma once

#include "CommandListSelect.hpp"

#include "AbstractEntity.hpp"
#include "CVehicleModelInfo.hpp"
#include "gta_entity.hpp"
#include "VehicleModelFlags.hpp"

namespace Stand
{
	class CommandVehicleModelFlag : public CommandListSelect
	{
	private:
		const CVehicleModelInfo::Flags flag;

	public:
		explicit CommandVehicleModelFlag(CommandList* parent, CVehicleModelInfo::Flags flag, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL)
			: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), {
				{ 0, LOC("OVRDOFF"), CMDNAMES("default") },
				{ 1, LOC("ON"), CMDNAMES("on") },
				{ 2, LOC("DOFF"), CMDNAMES("off") },
			}, 0), flag(flag)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (value == 0)
			{
				if (auto* ptr = g_player_veh.getPointer())
				{
					VehicleModelFlags::reset((CVehicleModelInfo*)ptr->archetype, flag);
				}
			}
			else if (value == 1)
			{
				registerScriptTickEventHandler(click, [this]
				{
					HANDLER_CHECK(value == 1)
					if (auto* ptr = g_player_veh.getPointer())
					{
						VehicleModelFlags::set((CVehicleModelInfo*)ptr->archetype, flag, true);
					}
					HANDLER_END
				});
			}
			else // if (value == 2)
			{
				registerScriptTickEventHandler(click, [this]
				{
					HANDLER_CHECK(value == 2)
					if (auto* ptr = g_player_veh.getPointer())
					{
						VehicleModelFlags::set((CVehicleModelInfo*)ptr->archetype, flag, false);
					}
					HANDLER_END
				});
			}
		}
	};
}
