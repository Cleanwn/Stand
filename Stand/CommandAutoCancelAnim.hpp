#pragma once

#include "CommandToggle.hpp"

#include "Animations.hpp"
#include "ExecCtx.hpp"
#include "input.hpp"

namespace Stand
{
	class CommandAutoCancelAnim : public CommandToggle
	{
	private:
		static bool movementKeyPressed()
		{
			return Input::isControlPressed(INPUT_MOVE_UP_ONLY)
				|| Input::isControlPressed(INPUT_MOVE_DOWN_ONLY)
				|| Input::isControlPressed(INPUT_MOVE_LEFT_ONLY)
				|| Input::isControlPressed(INPUT_MOVE_RIGHT_ONLY);
		}

	public:
		explicit CommandAutoCancelAnim(CommandList* const parent)
			: CommandToggle(parent, LOC("AUTO_CANCL"), {}, LOC("AUTO_CANCL_H"))
		{
		}

		void onChange(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (this->m_on && movementKeyPressed())
				{
					if (Animations::last_complex_animation != nullptr
						&& !ENTITY::IS_ENTITY_PLAYING_ANIM(g_player_ped, Animations::last_complex_animation->anim_dict, Animations::last_complex_animation->anim_name, 3)
						)
					{
						Animations::last_complex_animation = nullptr;
						Animations::reset();
					}
					else if (!Animations::last_animation_was_partial)
					{
						Animations::reset();
					}
					else if (TASK::PED_HAS_USE_SCENARIO_TASK(g_player_ped))
					{
						TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
					}
				}

				return this->m_on;
			});
		}
	};
}