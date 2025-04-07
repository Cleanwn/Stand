#pragma once

#include "CommandToggle.hpp"

#include "CommandSelectControllerKey.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"

//#include "drawDebugText.hpp"

namespace Stand
{
	class CommandSuperDrive : public CommandToggle
	{
	private:
		inline static std::vector<Hotkey> default_hotkeys{ 'W' };

		CommandToggleNoCorrelation* const cam_toggle;
		CommandSlider* const speed;
		CommandSelectControllerKey* const controller_key;

	public:
		explicit CommandSuperDrive(CommandList* const parent, CommandToggleNoCorrelation* cam_toggle, CommandSlider* speed, CommandSelectControllerKey* controller_key)
			: CommandToggle(parent, LOC("SPRDRV"), { CMDNAME("superdrive") }, NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_TUTORIAL_CTX_POINT, COMMANDPERM_USERONLY, default_hotkeys), cam_toggle(cam_toggle), speed(speed), controller_key(controller_key)
		{
		}

		[[nodiscard]] const std::vector<Hotkey>& getDefaultHotkeys() const noexcept final
		{
			return default_hotkeys;
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]()
			{
				if (!m_on)
				{
					return false;
				}
				if (g_player_veh.isValid() && g_player_veh.isOwnerOfVehicleAndDriver())
				{
					float normal = Input::getHighestNormal(hotkeys, (ControlInput)controller_key->value);
					//drawDebugText(fmt::to_string(normal));
					if (normal != 0.0f)
					{
						Vector3 vec;
						if (cam_toggle->m_on)
						{
							vec = CAM::GET_FINAL_RENDERED_CAM_ROT(2).toDir();
							vec /= 2.0f;
						}
						else
						{
							vec = ENTITY::GET_ENTITY_FORWARD_VECTOR(g_player_veh);
						}
						vec *= (float(speed->value) / 100.0f);
						vec *= normal;
						ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(g_player_veh, 1, vec.x, vec.y, vec.z, true, false, true, true);
					}
				}
				return true;
			});
		}
	};
}
