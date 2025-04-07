#pragma once

#include "CommandToggle.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandHalloween : public CommandToggle
	{
	public:
		explicit CommandHalloween(CommandList* const parent)
			: CommandToggle(parent, LOC("TNBL_HLWN"), CMDNAMES("halloween"))
		{
		}

		void onEnable(Click& click) final
		{
			registerAsyncTickEventHandler(click, [this]
			{
				constexpr int tunables[] = {
					GLOBAL_ENABLE_HEIST_MASKS_HALLOWEEN,
					GLOBAL_TURN_ON_HALLOWEEN_VEHICLES,
					GLOBAL_TURN_ON_HALLOWEEN_MASKS,
					GLOBAL_TURN_ON_HALLOWEEN_FACEPAINT,
					GLOBAL_TURN_ON_HALLOWEEN_BOBBLEHEADS,
					GLOBAL_TURN_ON_HALLOWEEN_CLOTHING,
					GLOBAL_TURN_ON_HALLOWEEN_WEAPONS,
					GLOBAL_TURN_ON_HALLOWEEN_HORNS,
					GLOBAL_ENABLE_BIKER_SANCTUS
				};
				for (int tunable : tunables)
				{
					*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(tunable).as<BOOL*>() = m_on;
				}
				return m_on;
			});
		}
	};
}
