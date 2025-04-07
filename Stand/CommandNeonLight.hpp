#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandNeonLight : public CommandToggle
	{
	private:
		const int neon_id;
	
	public:

		explicit CommandNeonLight(CommandList* const parent, int neon_id, Label&& menu_name, std::vector<CommandName>&& command_names)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), NOLABEL, false, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS), neon_id(neon_id)
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!isInViewport())
			{
				onTickInGameViewport();
			}
			return CommandToggle::onCommand(click, args);
		}

		void onTickInGameViewport() final
		{
			auto veh = Util::getVehicle();
			if(!veh.isValid())
			{
				return;
			}
			setOnIndication(VEHICLE::GET_VEHICLE_NEON_ENABLED(veh, neon_id));
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
		
		void onChange(Click& click) final
		{
			if (!click.isBulk())
			{
				auto veh = Util::getVehicle(click);
				if (veh.isValid())
				{
					const bool on = m_on;
					ensureScriptThread(click, [this, veh, on]() mutable
					{
						VEHICLE::SET_VEHICLE_NEON_ENABLED(veh, neon_id, on);
					});
				}
			}
		}
	};
}
