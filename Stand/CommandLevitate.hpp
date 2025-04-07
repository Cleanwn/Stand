#pragma once

#include "CommandToggle.hpp"

#include "get_current_time_millis.hpp"
#include "vector.hpp"

namespace Stand
{
	class CommandLevitate : public CommandToggle
	{
	private:
		bool water = false;
		v3 last_set_pos{};
		v3 momentum_pos{};
		time_t momentum_time = 0;
		bool grace_job_busy = false;
		bool z_extra_alt = false;
		float z_extra = 0.0f;
		float speed = 0.0f;
		CommandSlider* const base_speed;
		CommandSlider* const sprint_speed;
		CommandSlider* const acceleration;
		CommandToggleNoCorrelation* const ignore_pitch;
		CommandToggleNoCorrelation* const keep_momentum;
		CommandToggleNoCorrelation* const apply_pitch_to_entity;
		CommandToggleNoCorrelation* const only_rotate_on_movement;
		CommandToggleNoCorrelation* const show_button_instructions;
		CommandSlider* const passive_min;
		CommandSlider* const passive_max;
		CommandSlider* const passive_speed;
		CommandSlider* const assist_up;
		CommandSlider* const assist_down;
		CommandSlider* const assist_deadzone;
		CommandSlider* const assist_snap;

	public:
		explicit CommandLevitate(
			CommandList* const parent,
			CommandSlider* speed,
			CommandSlider* sprint_speed,
			CommandSlider* acceleration,
			CommandToggleNoCorrelation* ignore_pitch,
			CommandToggleNoCorrelation* keep_momentum,
			CommandToggleNoCorrelation* apply_pitch_to_entity,
			CommandToggleNoCorrelation* only_rotate_on_movement,
			CommandToggleNoCorrelation* show_button_instructions,
			CommandSlider* passive_min,
			CommandSlider* passive_max,
			CommandSlider* passive_speed,
			CommandSlider* assist_up,
			CommandSlider* assist_down,
			CommandSlider* assist_deadzone,
			CommandSlider* assist_snap
		)
			: CommandToggle(parent, LOC("LEV"), { CMDNAME("levitate"), CMDNAME("levitation") }, NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_TUTORIAL_CTX_POINT),
			base_speed(speed),
			sprint_speed(sprint_speed),
			acceleration(acceleration),
			ignore_pitch(ignore_pitch),
			keep_momentum(keep_momentum),
			apply_pitch_to_entity(apply_pitch_to_entity),
			only_rotate_on_movement(only_rotate_on_movement),
			show_button_instructions(show_button_instructions),
			passive_min(passive_min),
			passive_max(passive_max),
			passive_speed(passive_speed),
			assist_up(assist_up),
			assist_down(assist_down),
			assist_deadzone(assist_deadzone),
			assist_snap(assist_snap)
		{
		}

		void onChange(Click& click) final;

		[[nodiscard]] static bool isPedTooHigh() noexcept;
		[[nodiscard]] static bool isFirstPerson() noexcept;

		void onEnable(Click& click) final;
	};
}
