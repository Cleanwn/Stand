#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandPlayerVehSpin : public CommandPlayerToggle
	{
	private:
		AbstractEntity prev_veh;

	public:
		explicit CommandPlayerVehSpin(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("SPINVEH"), CMDNAMES_OBF("spin"), NOLABEL, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(m_on)
				auto p = PP_PTR_SINGLE->getPlayer();
				if (auto veh = p.getVehicle(); veh.isValid())
				{
					if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(veh))
					{
						if (prev_veh.isValid()
							&& prev_veh != veh
							)
						{
							prev_veh.setCanMigrate(true);
						}
						veh.setCanMigrate(false);
						prev_veh = veh;

						ENTITY::APPLY_FORCE_TO_ENTITY(veh, 5, 0.0f, 0.0f, 250.0f, 0, 0, 0, 0, true, false, true, false, true);
					}
					else
					{
						NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(veh);
					}
				}
				HANDLER_END
			});
		}

		void onDisable(Click& click) final
		{
			if (prev_veh.isValid())
			{
				prev_veh.setCanMigrate(true);
				prev_veh = AbstractEntity::invalid();
			}
		}
	};
}
