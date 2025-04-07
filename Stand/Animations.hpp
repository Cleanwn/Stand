#pragma once

#include <vector>
#include <string> 

#include "AbstractEntity.hpp"
#include "PedBoneId.hpp"
#include "Label.hpp"
#include "natives.hpp"
#include "Script.hpp"

namespace Stand
{
	enum eAnimationFlags
	{
		ANIM_FLAG_NORMAL = 0,
		ANIM_FLAG_REPEAT = 1,
		ANIM_FLAG_STOP_LAST_FRAME = 2,
		ANIM_FLAG_UPPERBODY = 16,
		ANIM_FLAG_ENABLE_PLAYER_CONTROL = 32,
		ANIM_FLAG_CANCELABLE = 120,
	};

	enum class ForcedMovementType : uint8_t
	{
		NONE,
		FORCE_STILL,
		FORCE_MOVEMENT
	};

	struct SimpleAnimation
	{
		Label display_name;
		const char* raw_name;
	};

	struct ComplexAnimation
	{
		Label display_name;
		const char* anim_dict;
		const char* anim_name;
		ForcedMovementType movement_override;

		ComplexAnimation(Label&& display_name, const char* anim_dict, const char* anim_name, ForcedMovementType movement_override = ForcedMovementType::NONE)
			: display_name(std::move(display_name)), anim_dict(anim_dict), anim_name(anim_name), movement_override(movement_override)
		{
		}
	};

	struct ComplexDanceAnimation : public ComplexAnimation
	{
		ComplexDanceAnimation(Label&& display_name, const char* anim_dict, const char* anim_name)
			: ComplexAnimation(std::move(display_name), anim_dict, anim_name, ForcedMovementType::FORCE_STILL)
		{
		}
	};

	struct ObjectData
	{
		Hash model;
		PedBoneId bone;

		Vector3 offset;
		Vector3 rotation;
	};

	struct ComplexObjectAnimation : public ComplexAnimation
	{
		ObjectData object_data;

		ComplexObjectAnimation(Label&& display_name, const char* anim_dictionary, const char* anim_name, ObjectData data, ForcedMovementType movement_override = ForcedMovementType::NONE)
			: ComplexAnimation(std::move(display_name), anim_dictionary, anim_name, movement_override), object_data(data)
		{
		}
	};

	struct Animations
	{
		static const std::vector<SimpleAnimation> scenarios;
		static const std::vector<ComplexAnimation> animations;
		static const std::vector<ComplexDanceAnimation> dances;
		static const std::vector<ComplexObjectAnimation> object_animations;

		static inline bool last_animation_was_partial = false;
		static inline AbstractEntity last_object_attachment = AbstractEntity::invalid();
		static inline const ComplexAnimation* last_complex_animation = nullptr;

		static void loadDict(const char* dict)
		{
			STREAMING::REQUEST_ANIM_DICT(dict);
			while (!STREAMING::HAS_ANIM_DICT_LOADED(dict))
			{
				Script::current()->yield();
			}
		}

		static void unloadDict(const char* dict)
		{
			STREAMING::REMOVE_ANIM_DICT(dict);
		}

		static void loadSet(const char* set)
		{
			STREAMING::REQUEST_ANIM_SET(set);
			while (!STREAMING::HAS_ANIM_SET_LOADED(set))
			{
				Script::current()->yield();
			}
		}

		static void unloadSet(const char* set)
		{
			STREAMING::REMOVE_ANIM_SET(set);
		}

		static void reset()
		{
			if (TASK::PED_HAS_USE_SCENARIO_TASK(g_player_ped))
			{
				TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
			}

			if (Animations::last_complex_animation != nullptr)
			{
				TASK::STOP_ANIM_TASK(g_player_ped, Animations::last_complex_animation->anim_dict, Animations::last_complex_animation->anim_name, 1.0f);
				Animations::last_complex_animation = nullptr;
			}

			if (Animations::last_object_attachment.isValid())
			{
				Animations::last_object_attachment.removeFromPlaneOfExistence();
				Animations::last_object_attachment.reset();
			}
		}
	};
}