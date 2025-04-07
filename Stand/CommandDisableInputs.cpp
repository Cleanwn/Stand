#include "CommandDisableInputs.hpp"

#include <array>

#include "CommandListAction.hpp"
#include "CommandListActionItem.hpp"
#include "gta_input.hpp"
#include "natives.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandDisableInput::CommandDisableInput(CommandList* const parent, int input)
		: CommandToggle(parent, LIT(g_control_input_names[input]), {}, NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_FEATURELIST_FINISHLIST), input(input)
	{
		command_names = { std::move(CommandName(CMDNAME("disable")).append(menu_name.getEnglishForCommandName())) };
		StringUtils::simplify(command_names.at(0));
	}

	void CommandDisableInput::onEnable(Click& click)
	{
		registerScriptTickEventHandler(click, [this]
		{
			if (!CommandDisableInputInvert::instance->m_on)
			{
				PAD::DISABLE_CONTROL_ACTION(0, input, m_on);
			}
			return m_on;
		});
	}

	CommandDisableInputInvert::CommandDisableInputInvert(CommandList* const parent)
		: CommandToggle(parent, LOC("IVRT"))
	{
		instance = this;
	}

	void CommandDisableInputInvert::onEnable(Click& click)
	{
		registerScriptTickEventHandler(click, [this]
		{
			if (!m_on)
			{
				return false;
			}
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			bool first = true;
			for (auto& child : parent->children)
			{
				if (child->type != COMMAND_TOGGLE)
				{
					continue;
				}
				if (first)
				{
					first = false;
					continue;
				}
				auto* const cmd = ((CommandDisableInput*)child.get());
				if (cmd->m_on)
				{
					PAD::ENABLE_CONTROL_ACTION(0, cmd->input, true);
				}
			}
			return true;
		});
	}

	using predicate_t = bool(*)(const int input);

	static bool predicate_mouse(const int input)
	{
		switch (input)
		{
		case INPUT_NEXT_CAMERA:
		case INPUT_CINEMATIC_SLOWMO:
		case INPUT_SCRIPTED_FLY_UD:
		case INPUT_SCRIPTED_FLY_LR:
		case INPUT_SCRIPTED_FLY_ZUP:
		case INPUT_SCRIPTED_FLY_ZDOWN:
		case INPUT_CHARACTER_WHEEL:
		case INPUT_MULTIPLAYER_INFO:
		case INPUT_SPRINT:
		case INPUT_JUMP:
		case INPUT_ENTER:
		case INPUT_LOOK_BEHIND:
		case INPUT_SPECIAL_ABILITY:
		case INPUT_SPECIAL_ABILITY_SECONDARY:
		case INPUT_MOVE_LR:
		case INPUT_MOVE_UD:
		case INPUT_MOVE_UP_ONLY:
		case INPUT_MOVE_DOWN_ONLY:
		case INPUT_MOVE_LEFT_ONLY:
		case INPUT_MOVE_RIGHT_ONLY:
		case INPUT_DUCK:
		case INPUT_PICKUP:
		case INPUT_COVER:
		case INPUT_RELOAD:
		case INPUT_TALK:
		case INPUT_DETONATE:
		case INPUT_HUD_SPECIAL:
		case INPUT_ARREST:
		case INPUT_CONTEXT:
		case INPUT_CONTEXT_SECONDARY:
		case INPUT_WEAPON_SPECIAL:
		case INPUT_WEAPON_SPECIAL_TWO:
		case INPUT_DIVE:
		case INPUT_DROP_WEAPON:
		case INPUT_DROP_AMMO:
		case INPUT_THROW_GRENADE:
		case INPUT_VEH_MOVE_LR:
		case INPUT_VEH_MOVE_UD:
		case INPUT_VEH_MOVE_UP_ONLY:
		case INPUT_VEH_MOVE_DOWN_ONLY:
		case INPUT_VEH_MOVE_LEFT_ONLY:
		case INPUT_VEH_MOVE_RIGHT_ONLY:
		case INPUT_VEH_SPECIAL:
		case INPUT_VEH_ACCELERATE:
		case INPUT_VEH_BRAKE:
		case INPUT_VEH_DUCK:
		case INPUT_VEH_HEADLIGHT:
		case INPUT_VEH_EXIT:
		case INPUT_VEH_HANDBRAKE:
		case INPUT_VEH_HOTWIRE_LEFT:
		case INPUT_VEH_HOTWIRE_RIGHT:
		case INPUT_VEH_LOOK_BEHIND:
		case INPUT_VEH_CIN_CAM:
		case INPUT_VEH_HORN:
		case INPUT_VEH_FLY_THROTTLE_UP:
		case INPUT_VEH_FLY_THROTTLE_DOWN:
		case INPUT_VEH_FLY_YAW_LEFT:
		case INPUT_VEH_FLY_YAW_RIGHT:
		case INPUT_VEH_SPECIAL_ABILITY_FRANKLIN:
		case INPUT_VEH_STUNT_UD:
		case INPUT_VEH_ROOF:
		case INPUT_VEH_JUMP:
		case INPUT_VEH_GRAPPLING_HOOK:
		case INPUT_VEH_SHUFFLE:
		case INPUT_VEH_DROP_PROJECTILE:
		case INPUT_VEH_FLY_ROLL_LR:
		case INPUT_VEH_FLY_ROLL_LEFT_ONLY:
		case INPUT_VEH_FLY_ROLL_RIGHT_ONLY:
		case INPUT_VEH_FLY_PITCH_UD:
		case INPUT_VEH_FLY_PITCH_UP_ONLY:
		case INPUT_VEH_FLY_PITCH_DOWN_ONLY:
		case INPUT_VEH_FLY_UNDERCARRIAGE:
		case INPUT_VEH_FLY_SELECT_TARGET_LEFT:
		case INPUT_VEH_FLY_SELECT_TARGET_RIGHT:
		case INPUT_VEH_FLY_VERTICAL_FLIGHT_MODE:
		case INPUT_VEH_FLY_DUCK:
		case INPUT_VEH_SUB_TURN_LR:
		case INPUT_VEH_SUB_TURN_LEFT_ONLY:
		case INPUT_VEH_SUB_TURN_RIGHT_ONLY:
		case INPUT_VEH_SUB_PITCH_UD:
		case INPUT_VEH_SUB_PITCH_UP_ONLY:
		case INPUT_VEH_SUB_PITCH_DOWN_ONLY:
		case INPUT_VEH_SUB_THROTTLE_UP:
		case INPUT_VEH_SUB_THROTTLE_DOWN:
		case INPUT_VEH_SUB_ASCEND:
		case INPUT_VEH_SUB_DESCEND:
		case INPUT_VEH_SUB_TURN_HARD_LEFT:
		case INPUT_VEH_SUB_TURN_HARD_RIGHT:
		case INPUT_VEH_PUSHBIKE_PEDAL:
		case INPUT_VEH_PUSHBIKE_SPRINT:
		case INPUT_VEH_PUSHBIKE_FRONT_BRAKE:
		case INPUT_VEH_PUSHBIKE_REAR_BRAKE:
		case INPUT_MELEE_ATTACK_LIGHT:
		case INPUT_MELEE_ATTACK_HEAVY:
		case INPUT_MELEE_BLOCK:
		case INPUT_PARACHUTE_DETACH:
		case INPUT_PARACHUTE_TURN_LR:
		case INPUT_PARACHUTE_TURN_LEFT_ONLY:
		case INPUT_PARACHUTE_TURN_RIGHT_ONLY:
		case INPUT_PARACHUTE_PITCH_UD:
		case INPUT_PARACHUTE_PITCH_UP_ONLY:
		case INPUT_PARACHUTE_PITCH_DOWN_ONLY:
		case INPUT_PARACHUTE_BRAKE_LEFT:
		case INPUT_PARACHUTE_BRAKE_RIGHT:
		case INPUT_PARACHUTE_SMOKE:
		case INPUT_PARACHUTE_PRECISION_LANDING:
		case INPUT_MAP:
		case INPUT_SAVE_REPLAY_CLIP:
		case INPUT_SPECIAL_ABILITY_PC:
		case INPUT_CELLPHONE_UP:
		case INPUT_CELLPHONE_DOWN:
		case INPUT_CELLPHONE_LEFT:
		case INPUT_CELLPHONE_RIGHT:
		case INPUT_CELLPHONE_SELECT:
		case INPUT_CELLPHONE_CANCEL:
		case INPUT_CELLPHONE_OPTION:
		case INPUT_CELLPHONE_EXTRA_OPTION:
		case INPUT_CELLPHONE_SCROLL_FORWARD:
		case INPUT_CELLPHONE_SCROLL_BACKWARD:
		case INPUT_CELLPHONE_CAMERA_FOCUS_LOCK:
		case INPUT_CELLPHONE_CAMERA_GRID:
		case INPUT_CELLPHONE_CAMERA_SELFIE:
		case INPUT_CELLPHONE_CAMERA_DOF:
		case INPUT_CELLPHONE_CAMERA_EXPRESSION:
		case INPUT_FRONTEND_DOWN:
		case INPUT_FRONTEND_UP:
		case INPUT_FRONTEND_LEFT:
		case INPUT_FRONTEND_RIGHT:
		case INPUT_FRONTEND_RDOWN:
		case INPUT_FRONTEND_RUP:
		case INPUT_FRONTEND_RLEFT:
		case INPUT_FRONTEND_RRIGHT:
		case INPUT_FRONTEND_AXIS_X:
		case INPUT_FRONTEND_AXIS_Y:
		case INPUT_FRONTEND_RIGHT_AXIS_X:
		case INPUT_FRONTEND_RIGHT_AXIS_Y:
		case INPUT_FRONTEND_PAUSE:
		case INPUT_FRONTEND_PAUSE_ALTERNATE:
		case INPUT_FRONTEND_ACCEPT:
		case INPUT_FRONTEND_CANCEL:
		case INPUT_FRONTEND_X:
		case INPUT_FRONTEND_Y:
		case INPUT_FRONTEND_LB:
		case INPUT_FRONTEND_RB:
		case INPUT_FRONTEND_LT:
		case INPUT_FRONTEND_RT:
		case INPUT_FRONTEND_LS:
		case INPUT_FRONTEND_RS:
		case INPUT_FRONTEND_LEADERBOARD:
		case INPUT_FRONTEND_SOCIAL_CLUB:
		case INPUT_FRONTEND_SOCIAL_CLUB_SECONDARY:
		case INPUT_FRONTEND_DELETE:
		case INPUT_FRONTEND_ENDSCREEN_ACCEPT:
		case INPUT_FRONTEND_ENDSCREEN_EXPAND:
		case INPUT_FRONTEND_SELECT:
		case INPUT_SCRIPT_LEFT_AXIS_X:
		case INPUT_SCRIPT_LEFT_AXIS_Y:
		case INPUT_SCRIPT_RIGHT_AXIS_X:
		case INPUT_SCRIPT_RIGHT_AXIS_Y:
		case INPUT_SCRIPT_RUP:
		case INPUT_SCRIPT_RDOWN:
		case INPUT_SCRIPT_RLEFT:
		case INPUT_SCRIPT_RRIGHT:
		case INPUT_SCRIPT_LB:
		case INPUT_SCRIPT_RB:
		case INPUT_SCRIPT_LT:
		case INPUT_SCRIPT_RT:
		case INPUT_SCRIPT_LS:
		case INPUT_SCRIPT_RS:
		case INPUT_SCRIPT_PAD_UP:
		case INPUT_SCRIPT_PAD_DOWN:
		case INPUT_SCRIPT_PAD_LEFT:
		case INPUT_SCRIPT_PAD_RIGHT:
		case INPUT_SCRIPT_SELECT:
		case INPUT_CURSOR_X:
		case INPUT_CURSOR_Y:
		case INPUT_ENTER_CHEAT_CODE:
		case INPUT_INTERACTION_MENU:
		case INPUT_MP_TEXT_CHAT_ALL:
		case INPUT_MP_TEXT_CHAT_TEAM:
		case INPUT_MP_TEXT_CHAT_FRIENDS:
		case INPUT_MP_TEXT_CHAT_CREW:
		case INPUT_PUSH_TO_TALK:
		case INPUT_CREATOR_LS:
		case INPUT_CREATOR_RS:
		case INPUT_CREATOR_LT:
		case INPUT_CREATOR_RT:
		case INPUT_CREATOR_MENU_TOGGLE:
		case INPUT_CREATOR_ACCEPT:
		case INPUT_CREATOR_DELETE:
		case INPUT_RAPPEL_JUMP:
		case INPUT_RAPPEL_LONG_JUMP:
		case INPUT_RAPPEL_SMASH_WINDOW:
		case INPUT_MELEE_ATTACK1:
		case INPUT_MELEE_ATTACK2:
		case INPUT_WHISTLE:
		case INPUT_MOVE_LEFT:
		case INPUT_MOVE_RIGHT:
		case INPUT_MOVE_UP:
		case INPUT_MOVE_DOWN:
		case INPUT_SNIPER_ZOOM_IN:
		case INPUT_SNIPER_ZOOM_OUT:
		case INPUT_SNIPER_ZOOM_IN_ALTERNATE:
		case INPUT_SNIPER_ZOOM_OUT_ALTERNATE:
		case INPUT_VEH_MOVE_LEFT:
		case INPUT_VEH_MOVE_RIGHT:
		case INPUT_VEH_MOVE_UP:
		case INPUT_VEH_MOVE_DOWN:
		case INPUT_REPLAY_START_STOP_RECORDING:
		case INPUT_REPLAY_START_STOP_RECORDING_SECONDARY:
		case INPUT_REPLAY_MARKER_DELETE:
		case INPUT_REPLAY_CLIP_DELETE:
		case INPUT_REPLAY_PAUSE:
		case INPUT_REPLAY_REWIND:
		case INPUT_REPLAY_FFWD:
		case INPUT_REPLAY_NEWMARKER:
		case INPUT_REPLAY_RECORD:
		case INPUT_REPLAY_SCREENSHOT:
		case INPUT_REPLAY_HIDEHUD:
		case INPUT_REPLAY_STARTPOINT:
		case INPUT_REPLAY_ENDPOINT:
		case INPUT_REPLAY_ADVANCE:
		case INPUT_REPLAY_BACK:
		case INPUT_REPLAY_TOOLS:
		case INPUT_REPLAY_RESTART:
		case INPUT_REPLAY_SHOWHOTKEY:
		case INPUT_REPLAY_CYCLEMARKERLEFT:
		case INPUT_REPLAY_CYCLEMARKERRIGHT:
		case INPUT_REPLAY_FOVINCREASE:
		case INPUT_REPLAY_FOVDECREASE:
		case INPUT_REPLAY_SAVE:
		case INPUT_REPLAY_TOGGLETIME:
		case INPUT_REPLAY_TOGGLETIPS:
		case INPUT_REPLAY_PREVIEW:
		case INPUT_REPLAY_TOGGLE_TIMELINE:
		case INPUT_REPLAY_TIMELINE_PICKUP_CLIP:
		case INPUT_REPLAY_TIMELINE_DUPLICATE_CLIP:
		case INPUT_REPLAY_TIMELINE_PLACE_CLIP:
		case INPUT_REPLAY_CTRL:
		case INPUT_REPLAY_TIMELINE_SAVE:
		case INPUT_REPLAY_PREVIEW_AUDIO:
		case INPUT_VEH_HYDRAULICS_CONTROL_TOGGLE:
		case INPUT_VEH_HYDRAULICS_CONTROL_LEFT:
		case INPUT_VEH_HYDRAULICS_CONTROL_RIGHT:
		case INPUT_VEH_HYDRAULICS_CONTROL_UP:
		case INPUT_VEH_HYDRAULICS_CONTROL_DOWN:
		case INPUT_VEH_HYDRAULICS_CONTROL_UD:
		case INPUT_VEH_HYDRAULICS_CONTROL_LR:
		case INPUT_SWITCH_VISOR:
		case INPUT_VEH_MELEE_HOLD:
		case INPUT_REPLAY_SNAPMATIC_PHOTO:
		case INPUT_VEH_CAR_JUMP:
		case INPUT_VEH_ROCKET_BOOST:
		case INPUT_VEH_FLY_BOOST:
		case INPUT_VEH_PARACHUTE:
		case INPUT_VEH_BIKE_WINGS:
		case INPUT_VEH_FLY_BOMB_BAY:
		case INPUT_VEH_FLY_COUNTER:
		case INPUT_VEH_TRANSFORM:
		case INPUT_QUAD_LOCO_REVERSE:
			return true;
		}
		return false;
	}

	static bool predicate_controller(const int input)
	{
		switch (input)
		{
		case INPUT_ENTER:
		case INPUT_PHONE:
		case INPUT_COVER:
		case INPUT_VEH_HEADLIGHT:
		case INPUT_VEH_NEXT_RADIO:
		case INPUT_VEH_PREV_RADIO:
		case INPUT_VEH_RADIO_WHEEL:
		case INPUT_VEH_CIN_CAM:
		case INPUT_SELECT_CHARACTER_MICHAEL:
		case INPUT_SELECT_CHARACTER_FRANKLIN:
		case INPUT_SELECT_CHARACTER_TREVOR:
		case INPUT_SELECT_CHARACTER_MULTIPLAYER:
		case INPUT_CHARACTER_WHEEL:
		case INPUT_MELEE_ATTACK1:
		case INPUT_MELEE_ATTACK2:
		case INPUT_MELEE_ATTACK_LIGHT:
		case INPUT_MELEE_ATTACK_ALTERNATE:
		case INPUT_MULTIPLAYER_INFO:
		case INPUT_MAP_POI:
			return true;
		}
		return false;
	}

	static bool predicate_controller_plus_tabs(const int input)
	{
		return input == INPUT_VEH_ACCELERATE || input == INPUT_VEH_BRAKE || predicate_controller(input);
	}

	static bool predicate_numpad(const int input)
	{
		return false;
	}
	
	class CommandDisableInputsPreset : public CommandListAction
	{
	public:
		explicit CommandDisableInputsPreset(CommandList* const parent)
			: CommandListAction(parent, LOC("PRSTS"), {}, NOLABEL, {
				{(long long)&predicate_mouse, LOC("JUSTMOUSE"), {}, LOC("DGMEINPT_H_MOUSE")},
				{(long long)&predicate_controller, LOC("CTRLLR"), {}, LOC("DGMEINPT_H_CTRLLR")},
				{(long long)&predicate_controller_plus_tabs, LOC("CTRLLR_N_TABS"), {}, LOC("DGMEINPT_H_CTRLLR_N_TABS")},
				{(long long)&predicate_numpad, LOC("KSCHM_1"), {}, LOC("DGMEINPT_H_NUMPAD")},
			})
		{
		}

		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(click, item);
			Click auto_click = click.deriveAuto();
			bool first = true;
			for (auto& child : parent->children)
			{
				if (child->type != COMMAND_TOGGLE)
				{
					continue;
				}
				if (first)
				{
					((CommandToggle*)child.get())->toggleCorrelation(auto_click, (item->value == (long long)&predicate_mouse), ToggleCorrelation::MENU_OPEN, false);
					first = false;
					continue;
				}
				auto* const cmd = (CommandDisableInput*)child.get();
				cmd->toggleCorrelation(auto_click, (((predicate_t)item->value)(cmd->input)), ToggleCorrelation::MENU_OPEN, false);
			}
		}
	};

	static const std::array top_inputs = {
		INPUT_ENTER_CHEAT_CODE,
		INPUT_REPLAY_START_STOP_RECORDING,
		INPUT_REPLAY_START_STOP_RECORDING_SECONDARY,
	};

	CommandDisableInputs::CommandDisableInputs(CommandList* const parent)
		: CommandList(parent, LOC("DGMEINPT"))
	{
		this->createChild<CommandDisableInputsPreset>();

		this->createChild<CommandDisableInputInvert>();

		for (const auto& i : top_inputs)
		{
			this->createChild<CommandDisableInput>(i);
		}

		for (int i = 0; i < _NUM_INPUT; ++i)
		{
			if (std::find(top_inputs.begin(), top_inputs.end(), i) == top_inputs.end())
			{
				this->createChild<CommandDisableInput>(i);
			}
		}
	}
}
