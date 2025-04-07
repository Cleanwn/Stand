#pragma once

#include "CommandToggle.hpp"

#include <fmt/core.h>

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandVehicleExtra : public CommandToggle
	{
	private:
		int32_t const extra_id;

	public:

		explicit CommandVehicleExtra(CommandList* const parent, int32_t const extra_id)
			: CommandToggle(parent, NOLABEL, CMDNAMES("vehextra"), NOLABEL, false, (CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_FEATURELIST_FINISHLIST), extra_id(extra_id)
		{
			std::string name = LANG_GET("XTRA");
			name.push_back(' ');
			name.append(fmt::to_string(extra_id));
			this->menu_name = LIT(std::move(name));
			this->command_names.at(0).append(utf8ToCmdName(fmt::to_string(extra_id)));
		}

		void onTickInGameViewport() final
		{
			if (auto veh = Util::getVehicle(); veh.isValid())
			{
				if (VEHICLE::DOES_EXTRA_EXIST(veh, extra_id))
				{
					parent->as<CommandListConcealer>()->setVisible(this, true);
					setOnIndication(VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(veh, extra_id));
					return;
				}
			}
			parent->as<CommandListConcealer>()->setVisible(this, false);
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void onChange(Click& click) final
		{
			if (!click.isBulk())
			{
				if (auto veh = Util::getVehicle(click); veh.isValid())
				{
					const bool val = (!m_on);
					ensureScriptThread(click, [this, veh, val]() mutable
					{
						VEHICLE::SET_VEHICLE_EXTRA(veh, extra_id, val);
					});
				}
			}
		}
	};
}
