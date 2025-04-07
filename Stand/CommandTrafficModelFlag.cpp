#include "CommandTrafficModelFlag.hpp"

#include <soup/macros.hpp>

#include "AbstractEntity.hpp"
#include "gta_vehicle.hpp"
#include "VehicleModelFlags.hpp"

namespace Stand
{
	CommandTrafficModelFlag::CommandTrafficModelFlag(CommandList* parent, CVehicleModelInfo::Flags flag, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
		: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), {
			{ 0, LOC("OVRDOFF"), CMDNAMES("default") },
			{ 1, LOC("ON"), CMDNAMES("on") },
			{ 2, LOC("DOFF"), CMDNAMES("off") },
			}, 0), flag(flag)
	{
	}

	void CommandTrafficModelFlag::onChange(Click& click, long long prev_value)
	{
		if (value == 0)
		{
			ensureScriptThread(click, [this]
			{
				VehicleModelFlags::reset(flag);
			});
		}
		else
		{
			auto value = this->value;
			registerScriptTickEventHandler(click, [this, value]
			{
				HANDLER_CHECK(value == this->value);
				AbstractEntity::getAllVehicles([&](AbstractEntity&& ent)
				{
					if (ent != g_player_veh)
					{
						if (auto* ptr = ent.getPointer())
						{
							VehicleModelFlags::set((CVehicleModelInfo*)ptr->archetype, flag, value == 1);
						}
					}
					CONSUMER_CONTINUE;
				});
				HANDLER_END;
			});
		}
	}
}
