#pragma once

#include "CommandSliderFloat.hpp"

#include "AbstractEntity.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandSuperRun : public CommandSliderFloat
	{
	public:
		inline static std::vector<Hotkey> default_hotkeys{ VK_LSHIFT };

		explicit CommandSuperRun(CommandList* const parent)
			: CommandSliderFloat(parent, LOC("SONIC"), { CMDNAME("superrun"), CMDNAME("superun") }, NOLABEL, -1000000, 1000000, 0, 100, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, false, default_hotkeys)
		{
		}

		[[nodiscard]] const std::vector<Hotkey>& getDefaultHotkeys() const noexcept final
		{
			return default_hotkeys;
		}

		void onChange(Click& click, int prev_value) final
		{
			if (value != 0)
			{
				int value = this->value;
				registerScriptTickEventHandler(click, [this, value]()
				{
					if (this->value != value)
					{
						return false;
					}
					if (Input::isAnyKeyPressed(hotkeys, INPUT_SPRINT))
					{
						const Vector3 vec = CAM::GET_FINAL_RENDERED_CAM_ROT(2).toDirNoZ() * (float(value) / 100.0f);
						ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(g_player_ped, 1, vec.x, vec.y, 0.0f, true, false, true, true);
					}
					return true;
				});
			}
		}
	};
}
