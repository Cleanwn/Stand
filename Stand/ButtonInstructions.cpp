#include "ButtonInstructions.hpp"

#include "Commandbox.hpp"
#include "ControllerInputConfig.hpp"
#include "Gui.hpp"
#include "hud_component.hpp"
#include "input.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "Scaleform.hpp"
#include "tbScreenshotMode.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	static Scaleform instructional_buttons_scaleform("instructional_buttons");

	void ButtonInstructions::setFreeMovementThisTick()
	{
		if (Input::isFreeSprintAvailable())
		{
			feature_buttons |= FREE_SPRINT;
		}
		else
		{
			feature_buttons |= CLOSEMENU4MOREMOVE;
		}
		if (Input::isAscDescAvailable())
		{
			feature_buttons |= ASC_DESC;
		}
		else
		{
			feature_buttons |= CLOSEMENU4MOREMOVE;
		}
	}

	void ButtonInstructions::onTick()
	{
		const Button_t buttons = (gui_buttons | feature_buttons);

		if (buttons != 0
			&& !g_commandbox.active
			&& !g_gui.isInteractionMenuOpen()
			&& !g_tb_screenshot_mode.isEnabled()
			&& (!NativeCallContext::canInvoke(0xA571D46727E2B718) || Input::keyboard_and_mouse == (bool)PAD::IS_USING_KEYBOARD_AND_MOUSE(0)) // Ensure that our KBM flag matches the game's otherwise the scaleform might show useless shit.
			)
		{
			if (instructional_buttons_scaleform.ensureLoaded())
			{
				const uhash_t scheme_hash = Input::getSchemeHash();
				if (prev_buttons != buttons || scheme_hash != prev_scheme_hash)
				{
					prev_buttons = buttons;
					prev_scheme_hash = scheme_hash;

					instructional_buttons_scaleform["SET_DATA_SLOT_EMPTY"]();
					instructional_buttons_scaleform["TOGGLE_MOUSE_BUTTONS"](false);

					int button_slot = 0;

					if (buttons & CLOSEMENU4MOREMOVE)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("BI_CLSMNU_MOV").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & YES_NO)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::scheme.key_menu_click, ControllerInputConfig::menu_click);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("YES").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::scheme.key_menu_back, ControllerInputConfig::menu_back);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("NO").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & ABORT_PROCEED)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::scheme.key_menu_back, ControllerInputConfig::menu_back);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("ABRT").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::scheme.key_menu_click, ControllerInputConfig::menu_click);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("PRCD").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & COMMANDBOX_CONTEXT)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(ControllerInputConfig::command_box);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("KB_CMDPRMPT").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(ControllerInputConfig::menu_context);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("B_CTX").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & LAND)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::keyboard_and_mouse ? INPUT_PARACHUTE_DEPLOY : INPUT_FRONTEND_Y);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("FLY_LAND").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & FREE_SPRINT)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::getFreeSprint());
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("SPRNT").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & ASC_DESC)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::getDescend());
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("DESC").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::getAscend());
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("ASC").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & OBJECT_ROTATE)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(INPUT_CONTEXT);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("ROT_R").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(INPUT_CONTEXT_SECONDARY);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("ROT_L").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & FOCUS_IN_MENU)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::isController() ? INPUT_FRONTEND_RB : INPUT_CONTEXT);
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("FCSIMNU").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					if (buttons & SUPER_FLIGHT)
					{
						GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(instructional_buttons_scaleform, "SET_DATA_SLOT");
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(button_slot++);
						Input::addToScaleform(Input::getContext());
						GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(LANG_GET("FLY").c_str());
						GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
					}

					instructional_buttons_scaleform["SET_MAX_WIDTH"](1.0f);
					instructional_buttons_scaleform["DRAW_INSTRUCTIONAL_BUTTONS"](0);
				}

				instructional_buttons_scaleform.drawFullscreen(200);

				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HUD_VEHICLE_NAME);
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HUD_AREA_NAME);
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HUD_VEHICLE_CLASS);
				HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HUD_STREET_NAME);

				ScriptGlobal(GLOBAL_TIMERS + GLOBAL_TIMERS_BI_OFFSET).set<int>(1);
			}
			else
			{
				prev_buttons = 0;
			}
		}
		else
		{
			if (instructional_buttons_scaleform.isLoaded())
			{
				instructional_buttons_scaleform["SET_DATA_SLOT_EMPTY"]();
				instructional_buttons_scaleform.unload();
			}
			prev_buttons = 0;
		}
	}
}
