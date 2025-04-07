#pragma once

#include "CommandToggle.hpp"

#include "gta_entity.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandForgeModel : public CommandToggle
	{
	public:
		explicit CommandForgeModel(CommandList* const parent)
			: CommandToggle(parent, LOC("FRGMDL"), CMDNAMES("forgemodel"), NOLABEL, false)
		{
		}

		void onClick(Click& click) final
		{
			if (!m_on)
			{
				if (click.isWeb())
				{
					updateWebState(); // Toggle is not enabled yet!
				}
				if (auto veh = Util::getCurrentlyDrivingVehicle(click); veh.isValid())
				{
					if (veh.isUserPersonalVehicle())
					{
						click.uwotm8(); // Very useless operation, but bricks freemode for us pretty hard.
						return;
					}
					showWarning(click, LANG_GET_W("FRGMDL_W"), 0, [this](ThreadContext thread_context)
					{
						ensureYieldableScriptThread(thread_context, [this]
						{
							Click click(CLICK_AUTO, TC_SCRIPT_YIELDABLE);
							AbstractEntity veh = Util::getCurrentlyDrivingVehicle(click);
							if (veh.isValid())
							{
								m_on = true;
								updateState(click);
								const Hash og_model = ((CVehicle*)veh)->archetype->hash;
								((CVehicle*)veh)->archetype->hash = ATSTRINGHASH("ruffian");
								registerScriptTickEventHandler(TC_SCRIPT_YIELDABLE, [this, og_model, veh{ std::move(veh) }]() mutable
								{
									while (m_on && veh.exists() && Util::getVehicle(false) == veh && ENTITY::IS_ENTITY_VISIBLE(veh))
									{
										return true;
									}
									if (veh.exists())
									{
										((CVehicle*)veh)->archetype->hash = og_model;
									}
									if (m_on)
									{
										m_on = false;
										Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
										updateState(click);
									}
									return false;
								});
							}
						});
					});
				}
			}
			else
			{
				m_on = false;
				updateState(click);
			}
		}
	};
}
