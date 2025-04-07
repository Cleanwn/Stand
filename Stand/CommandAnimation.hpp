#pragma once

#include "CommandAction.hpp"

#include "AbstractEntity.hpp"
#include "Animations.hpp"
#include "CommandListAnimations.hpp"
#include "Util.hpp"

namespace Stand
{
	template <class AnimationType>
	class CommandAnimation : public CommandAction
	{
	private:
		AnimationType animation;

		[[nodiscard]] bool shouldUsePartialMovement() const
		{
			if (this->animation.movement_override == ForcedMovementType::FORCE_MOVEMENT)
			{
				return true;
			}
			else if (this->animation.movement_override == ForcedMovementType::FORCE_STILL)
			{
				return false;
			}

			return CommandListAnimations::partial_movement;
		}

		static void createAndAttachObject(Click& click, ObjectData& data)
		{
			STREAMING::REQUEST_MODEL(data.model);

			while (!STREAMING::HAS_MODEL_LOADED(data.model))
			{
				Script::current()->yield();
			}

			auto object = Util::createObject(data.model, g_player_ped.getPos());

			if (!object.isValid())
			{
				click.setResponse(LOC("GENFAIL"));
				return;
			}

			ENTITY::ATTACH_ENTITY_TO_ENTITY(object, g_player_ped, PED::GET_PED_BONE_INDEX(g_player_ped, data.bone), data.offset.x, data.offset.y, data.offset.z, data.rotation.x, data.rotation.y, data.rotation.z, true, true, false, true, 1, true, 0);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(data.model);

			Animations::last_object_attachment = object;
		}

	public:
		explicit CommandAnimation(CommandList* const parent, AnimationType animation)
			: CommandAction(parent, std::move(animation.display_name), {}, NOLABEL, CMDFLAGS_ACTION | CMDFLAG_FEATURELIST_FINISHLIST), animation(animation)
		{
			if constexpr (std::is_same_v<SimpleAnimation, AnimationType>)
			{
				this->command_names = CMDNAMES("scen");
			}
			else
			{
				this->command_names = CMDNAMES("anim");
			}

			this->addSuffixToCommandNames(this->menu_name.getEnglishForCommandName());
		}

		void onClick(Click& click) final
		{
			click.ensureScriptThread([this, &click]
			{
				Animations::reset();

				if constexpr (std::is_same_v<SimpleAnimation, AnimationType>)
				{
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
					TASK::TASK_START_SCENARIO_IN_PLACE(g_player_ped, this->animation.raw_name, 0, true);
				}
				else
				{
					const bool partial = shouldUsePartialMovement();

					const char* const dict = this->animation.anim_dict;
					const char* const name = this->animation.anim_name;

					int flag = ANIM_FLAG_REPEAT;

					if (partial)
					{
						flag |= ANIM_FLAG_CANCELABLE;
					}

					if constexpr (std::is_same_v<ComplexObjectAnimation, AnimationType>)
					{
						createAndAttachObject(click, this->animation.object_data);
					}

					Animations::loadDict(dict);

					TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
					TASK::TASK_PLAY_ANIM(g_player_ped, dict, name, 2.0f, 2.0f, CommandListAnimations::looped ? -1 : int(ENTITY::GET_ANIM_DURATION(dict, name) * 1000), flag, 1.0f, false, false, false);

					Animations::unloadDict(dict);
					Animations::last_complex_animation = &this->animation;
					Animations::last_animation_was_partial = partial;
				}
			});
		}
	};
}