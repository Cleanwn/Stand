#pragma once

#include "CommandToggle.hpp"

#include "ButtonInstructions.hpp"
#include "fmt_arg.hpp"
#include "free_movement.hpp"
#include "input.hpp"
#include "is_session.hpp"

namespace Stand
{
	class CommandFloppyMode : public CommandToggle
	{
	private:
		CommandToggle* grace = nullptr;

		[[nodiscard]] CommandToggle* getGrace()
		{
			if (!grace)
			{
				grace = this->parent->parent->parent->resolveChildByMenuName(LOC("NRGDL"))->as<CommandToggle>();
			}

			return grace;
		}

	public:
		CommandSlider* movement_speed;
		CommandSlider* sprint_speed;
		CommandToggle* button_instructions;
		CommandToggle* prioritise_synching;

	private:
		time_t last_death = 0;

	public:
		explicit CommandFloppyMode(CommandList* const parent)
			: CommandToggle(parent, LOC("FLOPPY"), CMDNAMES_OBF("floppy", "insurancefraud"))
		{
		}

		void onEnable(Click& click) final
		{
			if (getGrace()->m_on)
			{
				Util::toast(LANG_FMT("NO_GRACE", FMT_ARG("feature_name", this->menu_name.getLocalisedUtf8())));
			}

			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(m_on);

				if (g_player_ped.isValid())
				{
					SOUP_IF_UNLIKELY (g_player_ped.isDead()) // yield to hospital respawn in SP
					{
						if (!is_session_started())
						{
							last_death = get_current_time_millis();
						}
					}
					else if (GET_MILLIS_SINCE(last_death) > 7000)
					{
						PED::SET_PED_TO_RAGDOLL(g_player_ped, 1000, 1000, 0, FALSE, FALSE, FALSE);

						if (Input::canMovementCommandPerformMovement())
						{
							v3 move{};

							const auto speed = prioritise_synching->m_on ? 0.5f : movement_speed->getFloatValue();
							const auto sprint = prioritise_synching->m_on ? 0.5f : sprint_speed->getFloatValue();

							if (button_instructions->m_on && !prioritise_synching->m_on)
							{
								ButtonInstructions::setFreeMovementThisTick();
							}

							if (free_movement(move, CAM::GET_FINAL_RENDERED_CAM_ROT(2), speed, sprint))
							{
								if (prioritise_synching->m_on)
								{
									move.z = 0.0f;
								}

								ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(g_player_ped, 1, move.x, move.y, move.z, true, false, true, true);
							}
						}
					}
				}

				HANDLER_END;
			});
		}
	};
}