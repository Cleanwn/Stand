#include "Gui.hpp"

#include <shellapi.h>

#include <fmt/xchar.h>
#include <rapidjson/document.h>

#include <soup/netIntel.hpp>
#include <soup/ObfusString.hpp>
#include <soup/os.hpp>
#include <soup/rand.hpp>

#include "AbstractEntity.hpp"
#include "AntiAntiCheat.hpp"
#include "Auth.hpp"
#include "BattlEyeServer.hpp"
#include "BgScript.hpp"
#include "ButtonInstructions.hpp"
#include "Camgr.hpp"
#include "Chat.hpp"
#include "ChatCommandsCommon.hpp"
#include "ChatHistory.hpp"
#include "CleanupHelper.hpp"
#include "ColonsAndTabs.hpp"
#include "Commandbox.hpp"
#include "CommandFlyThroughWindscreen.hpp"
#include "CommandList.hpp"
#include "CommandPlayer.hpp"
#include "CommandPlayerHistory.hpp"
#include "CommandSnake.hpp"
#include "CommandTabPlayers.hpp"
#include "CommandTabStand.hpp"
#include "CommandToggle.hpp"
#include "conf.hpp"
#include "ContextMenu.hpp"
#include "ControllerInputConfig.hpp"
#include "CPlayerInfo.hpp"
#include "DisableDripfeed.hpp"
#include "drawDebugText.hpp"
#include "DrawUtil3d.hpp"
#include "evtPlayerJoinEvent.hpp"
#include "evtPlayerLeaveEvent.hpp"
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "extract_delimited_string_with_escaping.hpp"
#include "FiberPool.hpp"
#include "FileLogger.hpp"
#include "fmt_arg.hpp"
#include "GameToaster.hpp"
#include "get_current_time_millis.hpp"
#include "GlobalsWatcher.hpp"
#include "gta_input.hpp"
#include "gta_ped.hpp"
#include "Hooking.hpp"
#include "HttpRequestBuilder.hpp"
#include "input.hpp"
#include "is_session.hpp"
#include "LeaveNotifications.hpp"
#include "main.hpp"
#include "MapUtil.hpp"
#include "MenuGrid.hpp"
#include "Metrics.hpp"
#include "natives.hpp"
#include "NetInterface.hpp"
#include "PackageRepositoryFetchTask.hpp"
#include "PlayerHistory.hpp"
#include "PlayerListSort.hpp"
#include "PlayerProviderSingle.hpp"
#include "pointers.hpp"
#include "Pong.hpp"
#include "RelayCon.hpp"
#include "RemoteGamer.hpp"
#include "Renderer.hpp"
#include "RootMgr.hpp"
#include "RootNameMgr.hpp"
#include "ScAccount.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "StringUtils.hpp"
#include "TickMgr.hpp"
#include "TickQueues.hpp"
#include "TpUtil.hpp"
#include "TransitionHelper.hpp"
#include "TransitionState.hpp"
#include "Tunables.hpp"
#include "Tutorial.hpp"
#include "Util.hpp"
#include "Worker.hpp"

#ifdef STAND_DEBUG
#include "Snippets.hpp"
#endif

#define VEHICLE_UPDATE_HELPER false

#if VEHICLE_UPDATE_HELPER
#include "using_model.hpp"
#endif

#define PROFILE_STATE false

#if PROFILE_STATE
#include "TimedCall.hpp"
#endif

namespace Stand
{
	Gui::Gui() :
		root_list(std::make_unique<CommandList>(nullptr, NOLABEL)),
		next_keyboard_eval(get_current_time_millis())
	{
	}

	bool Gui::isFullyLoaded() const noexcept
	{
		return PlayerHistory::inited && inited_netIntel;
	}

	CommandList* Gui::getActivateTab() const
	{
		return root_list->resolveChildByMenuName(LOC("ACTVTE"))->as<CommandList>();
	}

	CommandList* Gui::getGameTab() const
	{
		return root_list->resolveChildByMenuName(LOC("GME"))->as<CommandList>();
	}

	CommandList* Gui::getStandTab() const
	{
		return root_list->resolveChildByMenuNameLiteral("Stand")->as<CommandList>();
	}

	void Gui::removeTab(CommandList* target)
	{
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		uint8_t i = 0;
		for (auto it = root_list->children.begin(); it != root_list->children.end(); ++i, ++it)
		{
			if (it->get() == target)
			{
				if (root_cursor > i)
				{
					--root_cursor;
				}
				root_list->removeChild(it);
				updateTabsIgnoreContextMenu();
				g_menu_grid.update();
				break;
			}
		}
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
	}

	void Gui::resetCursor()
	{
		if (g_renderer.areTabsEnabled())
		{
			root_list->children.at(0)->as<CommandList>()->open(TC_OTHER);
		}
		else
		{
			root_list->open(TC_OTHER);
		}
	}

	bool Gui::isListVisible() const
	{
		return !isWarningActive()
			&& !isAwaitingSetHotkeyInput()
			&& !isPromptActive()
			;
	}

	void Gui::setApplicableNonFreeRootStateValue() noexcept
	{
		if (g_auth.license_permissions == LICPERM_BASIC)
		{
			root_state = GUI_BASIC;
		}
		else if (g_auth.license_permissions == LICPERM_REGULAR)
		{
			root_state = GUI_REGULAR;
		}
		else if (g_auth.license_permissions == LICPERM_ULTIMATE)
		{
			root_state = GUI_ULTIMATE;
		}
	}

	void Gui::setProfilesTutorialDone()
	{
		Tutorial::setState(TUT_DONE);
		profiles_tutorial = 4;
		meta_state.save();
	}

	void Gui::doPromptUserForControllerDisables(ThreadContext thread_context)
	{
		if (doPromptUserForDisablePreset(thread_context, g_renderer.areTabsEnabled() ? ATSTRINGHASH("CTRLLR_N_TABS") : ATSTRINGHASH("CTRLLR")))
		{
			user_prompted_for_controller_disables = true;
		}
	}

	bool Gui::doPromptUserForDisablePreset(ThreadContext thread_context, hash_t preset)
	{
		auto* disable_game_inputs = getDisableGameInputs();
		if (disable_game_inputs == nullptr)
		{
			return false;
		}
		showPrompt(thread_context, LANG_FMT(
			"DGMEINPT_P",
			FMT_ARG("preset", Lang::get(preset)),
			FMT_ARG("disable_game_inputs_path", disable_game_inputs->getLocalisedAddress())
		), [preset](ThreadContext thread_context)
		{
			Click click(CLICK_AUTO, thread_context);
			g_gui.getDisableGameInputs()->recursivelyResolveChildByMenuNameHash(preset)->onClick(click);
			g_gui.saveActiveState();
			if (preset == ATSTRINGHASH("JUSTMOUSE"))
			{
				g_gui.setMouseMode(MouseMode::NAVIGATE);
			}
		}, [preset](ThreadContext thread_context)
		{
			if (preset == ATSTRINGHASH("CTRLLR_N_TABS") || preset == ATSTRINGHASH("CTRLLR"))
			{
				g_gui.active_profile.save();
			}
			else if (preset == ATSTRINGHASH("JUSTMOUSE"))
			{
				g_gui.setMouseMode(MouseMode::NAVIGATE);
			}
		});
		return true;
	}

	CommandList* Gui::getDisableGameInputs() const
	{
		return getGameTab()->recursivelyResolveChildByMenuName(LOC("DGMEINPT"))->as<CommandList>();
	}

	cursor_t Gui::getCursorPadding() const
	{
		return last_input_type == INPUTTYPE_INDIFFERENT ? cursor_padding : 0;
	}

	void Gui::lerpStart(CommandList* const list) noexcept
	{
		if (lerp != 0)
		{
			lerp_start = get_current_time_millis();
			lerp_from_cursor = list->m_cursor - list->m_offset;
			lerp_from_offset = list->m_offset;
		}
	}

	void Gui::lerpStop() noexcept
	{
		lerp_start = 0;
	}

	bool Gui::isWarningActive() const
	{
		return warnings.find(root_cursor) != warnings.end();
	}

	void Gui::setMouseMode(MouseMode mode)
	{
		mouse_mode = mode;
		updateCursorVisibility();
	}

	static soup::Bytepatch hide_cursor_jnz_patch{};

	void Gui::updateCursorVisibility()
	{
		if (opened && mouse_mode != MouseMode::NONE)
		{
			if (!hide_cursor_jnz_patch.isPatched() && pointers::hide_cursor_jnz != nullptr)
			{
				uint8_t patch[] = {
					soup::Bytepatch::JMP1 // jnz -> jmp
				};
				hide_cursor_jnz_patch.initPatch(pointers::hide_cursor_jnz, patch, COUNT(patch));
			}
			if (GetForegroundWindow() == pointers::hwnd)
			{
				g_renderer.ensureWndProcContext([]
				{
					while (ShowCursor(TRUE) < 0);
				});
			}
		}
		else
		{
			hide_cursor_jnz_patch.restore();
		}
	}

	void Gui::mapControlInputToMenuKeyThisTick(const int control_input, const MenuKey menu_key, const InputType input_type)
	{
		if (keyboard_pressing == menu_key)
		{
			if (PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, control_input))
			{
				inputStop();
			}
		}
		else if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, control_input))
		{
			if (isRootStateFull()
				&& user_understands_navigation
				&& !user_prompted_for_controller_disables
				)
			{
				doPromptUserForControllerDisables(TC_SCRIPT_NOYIELD);
			}
			else
			{
				onKeyPressStart(menu_key, input_type);
			}
		}
	}

	void Gui::onKeyPressStart(const MenuKey key, const InputType input_type)
	{
		keyboard_pressing = key;
		auto direction = menuKeyToDirection2d(key);

		if (opened)
		{
			last_input_type = input_type;
			keyboard_hold = 0;
			next_keyboard_eval = get_current_time_millis();

			if (direction != TELEPORT)
			{
				if (GET_MILLIS_SINCE(last_konami_input) > 1000)
				{
					konami = 0;
				}
				last_konami_input = get_current_time_millis();

				if (direction == UP)
				{
					if (konami != 0 && konami != 1)
					{
						konami = 0;
					}
					Command* focus = getCurrentUiFocusPhysical();
					if (focus != nullptr && !focus->handlesLeftRightInput())
					{
						++konami;
					}
				}
				else if (direction == DOWN)
				{
					if (konami == 2 || konami == 3)
					{
						++konami;
					}
					else
					{
						konami = 0;
					}
				}
				else if (direction == LEFT)
				{
					if (konami == 4 || konami == 6)
					{
						++konami;
						sfxLeftRight();
					}
					else
					{
						konami = 0;
					}
				}
				else if (direction == RIGHT)
				{
					if (konami == 7)
					{
						konami = 0;
						sfxLeftRight();
						Command* focus = getCurrentUiFocusPhysical();
						if (focus != nullptr && !focus->handlesLeftRightInput())
						{
							auto stand_tab = getStandTab()->as<CommandTabStand>();
							if (stand_tab != nullptr)
							{
								stand_tab->onKonamiCodeEntered();
							}
						}
					}
					else if (konami == 5)
					{
						++konami;
						sfxLeftRight();
					}
					else
					{
						konami = 0;
					}
				}
				else
				{
					konami = 0;
				}
			}
		}
		else
		{
			konami = 0;
		}
		// Update last_directional_input for Snake; not allowing move into opposite direction.
		switch (direction)
		{
		case UP:
			if (last_directional_input_processed == DOWN)
			{
				return;
			}
			break;

		case DOWN:
			if (last_directional_input_processed == UP)
			{
				return;
			}
			break;

		case LEFT:
			if (last_directional_input_processed == RIGHT)
			{
				return;
			}
			break;

		case RIGHT:
			if (last_directional_input_processed == LEFT)
			{
				return;
			}
			break;
		}
		last_directional_input = direction;
	}

	Direction Gui::menuKeyToDirection2d(const MenuKey key)
	{
		switch (key)
		{
		case MENUKEY_UP:
		case MENUKEY_ROOT_UP:
			return UP;

		case MENUKEY_DOWN:
		case MENUKEY_ROOT_DOWN:
		case MENUKEY_CLICK:
			return DOWN;

		case MENUKEY_RIGHT:
			return RIGHT;
			break;

		case MENUKEY_LEFT:
			return LEFT;
			break;
		}
		return TELEPORT;
	}

	bool Gui::isGodmodeActive()
	{
		return godmode;
	}

	void Gui::onGodmodeDisable()
	{
		if (!isGodmodeActive())
		{
			g_player_ped.godmodeDisable();
		}
	}

	void Gui::set_camera_distance_from_character_multiplier_all_things_considered() const
	{
		if (character_scale_affects_camera_command->m_on)
		{
			Camgr::set_distance_from_character_multiplier(user_defined_camera_distance_multiplier * abs(character_scale_multiplier));
		}
		else
		{
			Camgr::set_distance_from_character_multiplier(user_defined_camera_distance_multiplier);
		}
	}

	void Gui::updateRunSpeedIgnoreScale() const
	{
		g_player_ped.getPlayerInfo()->m_run_speed = user_defined_run_speed;
	}

	void Gui::updateSwimSpeedIgnoreScale() const
	{
		g_player_ped.getPlayerInfo()->m_swim_speed = user_defined_swim_speed;
	}

	bool Gui::isUsingAutosaveState() const noexcept
	{
		return auto_save_state == active_profile.getName();
	}

	void Gui::doGtaOnlineStatusRequest() noexcept
	{
		gtao_status_lang = Lang::getActiveLangForRockstar();
		HttpRequestBuilder b{ HttpRequestBuilder::GET, "support.rockstargames.com", {} };
		b.setPathWithRockstarLocalisation("/services/status.json");
		b.setResponseCallback([](soup::HttpResponse&& resp)
		{
			rapidjson::Document document;
			if (!document.Parse(resp.body.data(), resp.body.size()).HasParseError() && document.IsObject() && document.HasMember("statuses") && document["statuses"].IsArray())
			{
				for (const auto& service : document["statuses"].GetArray())
				{
					if (strcmp(service["tag"].GetString(), "gtao") != 0)
					{
						continue;
					}

					for (const auto& platform : service["services_platforms"].GetArray())
					{
						if (strcmp(platform["name"].GetString(), "PC") != 0)
						{
							continue;
						}

						const auto status = platform["service_status_id"].GetInt();
						// 1 = Up
						// 2 = Down
						// 3 = Limited
						if (status != 1) // Up
						{
							g_gui.status_text = StringUtils::utf8_to_utf16(LANG_FMT(
								"GTAOSTUS_O",
								((status == 2) ? LANG_GET("GTAOSTUS_O_D") : LANG_GET("GTAOSTUS_O_L"))
							));
						}
						else
						{
							g_gui.status_text.clear();
						}
					}

					std::string message{};
					if (service["alert_status"].GetInt() != 0) // not entirely sure if this controls message visibility, but looks good enough
					{
						message = service["message"].GetString();
					}
					if (!message.empty())
					{
						StringUtils::replace_all(message, "<p>", "");
						g_gui.status_message = StringUtils::utf8_to_utf16(LANG_FMT("GTAOSTUS_M", std::move(message)));
					}
					else
					{
						g_gui.status_message.clear();
					}
				}
			}
		});
		b.dispatch();
	}

	std::string Gui::getMenuOpenCloseButtonString()
	{
		return Input::keyboard_and_mouse ? Input::getPreferedHotkey(Input::scheme.key_menu_open_close).toBracketedString() : ControllerInputConfig::getOpenCloseStringForGame();
	}

	void Gui::processToggleCorrelation(ThreadContext thread_context, ToggleCorrelation_t correlation, bool value) const
	{
		Click click(CLICK_BULK, thread_context);
		for (const auto& command : commands_with_correlation)
		{
			if (command->correlation.type == correlation)
			{
				command->setStateBasedOnCorrelation(click, value);
			}
		}
	}

	void Gui::open(ThreadContext thread_context)
	{
		SOUP_IF_UNLIKELY (Tutorial::state == TUT_OPEN)
		{
			Tutorial::setState(TUT_OPEN + 1);
		}
		openCloseCommon(thread_context, true);
		auto* list = getCurrentUiList();
		if (list != nullptr)
		{
			list->onActiveListUpdate();
			auto* focus = list->getFocusPhysical();
			if(focus != nullptr)
			{
				focus->onFocus(thread_context, TELEPORT);
			}
		}
	}

	void Gui::close(ThreadContext thread_context)
	{
		inputStop();
		stupid_back_counter = 0;
		openCloseCommon(thread_context, false);
		auto* list = getCurrentUiList();
		if (list != nullptr)
		{
			list->onActiveListUpdate();
			auto* focus = list->getFocusPhysical();
			if (focus != nullptr)
			{
				focus->onBlur(thread_context, TELEPORT);
			}
		}
	}

	void Gui::openCloseCommon(ThreadContext thread_context, bool value)
	{
		opened = value;
		if (!isRootUpdatePendingOrInProgress())
		{
			processToggleCorrelation(thread_context, ToggleCorrelation::MENU_OPEN, value);
		}
		updateCursorVisibility();
	}

	void Gui::inputOpenCloseMenu(ThreadContext thread_context)
	{
		if (!opened)
		{
			open(thread_context);
		}
		else
		{
			close(thread_context);
		}
		sfxOpenClose(thread_context, opened);
	}

	void Gui::sfxOpenClose(ThreadContext thread_context, bool opened)
	{
		if (opened)
		{
			queueSoundJob(thread_context, []
			{
				AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
			});
		}
		else
		{
			sfxClose(thread_context);
		}
	}

	void Gui::sfxClose(ThreadContext thread_context)
	{
		queueSoundJob(thread_context, []
		{
			AUDIO::PLAY_SOUND_FRONTEND(-1, "BACK", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
		});
	}

	void Gui::openForcefully(ThreadContext thread_context)
	{
		if (!opened)
		{
			open(thread_context);
		}
		abortSetHotkey();
		ContextMenu::root.reset();
	}

	bool Gui::inputRootUp(ThreadContext thread_context, bool holding)
	{
		if (g_renderer.areTabsEnabled()
			&& isRootNavigationActive()
			&& Tutorial::state >= TUT_ROOT_UP
			)
		{
			auto prev_focus = getCurrentMenuFocus()->getWeakRef();
			if (root_cursor <= 0)
			{
				if (holding && wraparound_holding_pause)
				{
					return false;
				}
				onPreRootCursorUpdate(thread_context);
				root_cursor = ((int16_t)root_list->children.size()) - 1;
			}
			else
			{
				onPreRootCursorUpdate(thread_context);
				--root_cursor;
				SOUP_IF_UNLIKELY (Tutorial::state == TUT_ROOT_UP)
				{
					Tutorial::setState(TUT_ROOT_UP + 1);
				}
			}
			onRootCursorUpdate(thread_context);
			updateFocus(thread_context, TELEPORT, prev_focus);
		}
		return true;
	}

	bool Gui::inputRootDown(ThreadContext thread_context, bool holding)
	{
		if (g_renderer.areTabsEnabled()
			&& isRootNavigationActive()
			&& Tutorial::state >= TUT_ROOT_DOWN
			)
		{
			auto prev_focus = getCurrentMenuFocus()->getWeakRef();
			if (root_cursor + 1 >= root_list->children.size())
			{
				if (holding && wraparound_holding_pause)
				{
					return false;
				}
				onPreRootCursorUpdate(thread_context);
				root_cursor = 0;
			}
			else
			{
				onPreRootCursorUpdate(thread_context);
				++root_cursor;
				SOUP_IF_UNLIKELY (Tutorial::state == TUT_ROOT_DOWN)
				{
					Tutorial::setState(TUT_ROOT_DOWN + 1);
				}
			}
			onRootCursorUpdate(thread_context);
			updateFocus(thread_context, TELEPORT, prev_focus);
		}
		return true;
	}

	void Gui::onPreRootCursorUpdate(ThreadContext thread_context)
	{
		if (reset_cursor_on_root_navigation)
		{
			m_active_list.at(root_cursor)->onBack(thread_context);
		}
		m_active_list.at(root_cursor)->onActiveListUpdate();
	}

	void Gui::onRootCursorUpdate(ThreadContext thread_context)
	{
		queueSoundJob([]
		{
			AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
		});
		ContextMenu::root.reset();
		if (reset_cursor_on_root_navigation)
		{
			CommandList* prev_focus_list = m_active_list.at(root_cursor);
			if (auto prev_focus = prev_focus_list->getFocus())
			{
				prev_focus->getPhysical()->onBlur(thread_context, TELEPORT);
			}
			prev_focus_list->resetCursorIfApplicable();
			m_active_list[root_cursor] = root_list->children.at(root_cursor)->as<CommandList>();
			m_active_list.at(root_cursor)->resetCursor();
			prev_focus_list->onActiveListUpdate();
		}
		else
		{
			m_active_list.at(root_cursor)->fixCursorAndOffset();
		}
		m_active_list.at(root_cursor)->onActiveListUpdate();
		lerpStop();
	}

	bool Gui::isRootNavigationActive() const
	{
		return !isAwaitingSetHotkeyInput() && !isPromptActive() && !isInBadBoyTimeout();
	}

	bool Gui::isMenuNavigationActive() const
	{
		return !isWarningActive() && isRootNavigationActive();
	}

	void Gui::processPostNavigationTutorial(const CommandList* const active_list) const
	{
		if (Tutorial::state == TUT_DONE)
		{
			if (!user_understands_commands)
			{
				if (auto focus = active_list->getFocus())
				{
					if (!focus->as<CommandIssuable>()->command_names.empty())
					{
						Tutorial::setState(TUT_CMD);
						return;
					}
				}
			}
			if (!user_understands_sliders)
			{
				if (auto focus = active_list->getFocus())
				{
					if (focus->handlesLeftRightInput())
					{
						Tutorial::setState(TUT_SLIDER);
						return;
					}
				}
			}
			if (profiles_tutorial == 3)
			{
				Tutorial::setState(TUT_PROFILES);
				return;
			}
			if (!user_understands_context_menu)
			{
				if (auto focus = active_list->getFocusPhysical())
				{
					if (focus->flags & CMDFLAG_TUTORIAL_CTX_POINT)
					{
						Tutorial::setState(TUT_CTX);
						return;
					}
				}
			}
		}
		else if (Tutorial::state == TUT_CMD)
		{
			auto focus = active_list->getFocus();
			if (!focus || focus->as<CommandIssuable>()->command_names.empty())
			{
				Tutorial::setState(TUT_DONE);
				processPostNavigationTutorial(active_list);
			}
		}
		else if (Tutorial::state == TUT_SLIDER)
		{
			auto focus = active_list->getFocus();
			if (!focus || !focus->handlesLeftRightInput())
			{
				Tutorial::setState(TUT_DONE);
				processPostNavigationTutorial(active_list);
			}
		}
		else if (Tutorial::state == TUT_CTX)
		{
			auto focus = active_list->getFocusPhysical();
			if (!focus || !(focus->flags & CMDFLAG_TUTORIAL_CTX_POINT))
			{
				Tutorial::setState(TUT_DONE);
				processPostNavigationTutorial(active_list);
			}
		}
	}

	bool Gui::inputUp(ThreadContext thread_context, const bool holding)
	{
		if (isMenuNavigationActive()
			&& Tutorial::state >= TUT_UP
			)
		{
			auto* const active_list = getCurrentUiList();
			auto prev_focus = active_list->getFocus()->getWeakRef();
			if (active_list->m_cursor <= 0)
			{
				lerpStop();
				if ((holding && wraparound_holding_pause) || last_input_type != INPUTTYPE_INDIFFERENT)
				{
					return false;
				}
				const auto num_children = (cursor_t)active_list->countVisibleChildren();
				const auto on_screen_limit = g_gui.getCommandsOnScreenLimit();
				active_list->m_cursor = num_children - 1;
				active_list->m_offset = num_children > on_screen_limit ? num_children - on_screen_limit : 0;
			}
			else
			{
				lerpStart(active_list);
				active_list->m_cursor--;
				if (last_input_type == INPUTTYPE_MOUSE_SCROLLWHEEL)
				{
					active_list->m_offset--;
				}
				active_list->fixCursorAndOffset();
				SOUP_IF_UNLIKELY (Tutorial::state == TUT_UP)
				{
					Tutorial::setState(TUT_UP + 1);
				}
			}
			updateActiveFocus(thread_context, UP, prev_focus);
		}
		return true;
	}

	bool Gui::inputDown(ThreadContext thread_context, const bool holding)
	{
		if (isMenuNavigationActive()
			&& Tutorial::state >= TUT_DOWN
			)
		{
			auto* const active_list = getCurrentUiList();
			auto prev_focus = active_list->getFocus()->getWeakRef();
			if ((active_list->m_cursor + 1) >= (cursor_t)active_list->countVisibleChildren())
			{
				lerpStop();
				if ((holding && wraparound_holding_pause) || last_input_type != INPUTTYPE_INDIFFERENT)
				{
					return false;
				}
				active_list->m_cursor = 0;
				active_list->m_offset = 0;
			}
			else
			{
				lerpStart(active_list);
				++active_list->m_cursor;
				if (last_input_type == INPUTTYPE_MOUSE_SCROLLWHEEL)
				{
					active_list->m_offset++;
				}
				active_list->fixCursorAndOffset();
				SOUP_IF_UNLIKELY (Tutorial::state == TUT_DOWN)
				{
					Tutorial::setState(TUT_DOWN + 1);
				}
			}
			updateActiveFocus(thread_context, DOWN, prev_focus);
		}
		return true;
	}

	bool Gui::inputLeft(ThreadContext thread_context, const bool holding)
	{
		if (isMenuNavigationActive())
		{
			auto* focus = getCurrentUiFocusPhysical();
			if (focus != nullptr)
			{
				if (focus->handlesLeftRightInput())
				{
					SOUP_IF_UNLIKELY (Tutorial::state == TUT_SLIDER)
					{
						Tutorial::setState(TUT_DONE);
						user_understands_sliders = true;
						saveTutorialFlags();
					}

					Click click(CLICK_MENU, thread_context);
					return focus->onLeft(click, holding);
				}
			}
		}
		return false;
	}

	bool Gui::inputRight(ThreadContext thread_context, const bool holding)
	{
		if (isMenuNavigationActive())
		{
			auto* focus = getCurrentUiFocusPhysical();
			if (focus != nullptr)
			{
				if (focus->handlesLeftRightInput())
				{
					SOUP_IF_UNLIKELY (Tutorial::state == TUT_SLIDER)
					{
						Tutorial::setState(TUT_DONE);
						user_understands_sliders = true;
						saveTutorialFlags();
					}

					Click click(CLICK_MENU, thread_context);
					return focus->onRight(click, holding);
				}
			}
		}
		return false;
	}

	void Gui::sfxLeftRight() const
	{
		queueSoundJob([]
		{
			AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_LEFT_RIGHT", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
		});
	}

	void Gui::inputClick(ThreadContext thread_context)
	{
		if (isInBadBoyTimeout())
		{
			return;
		}
		if (true)
		{
			const auto warning_i = warnings.find(root_cursor);
			if (warning_i != warnings.end())
			{
				inputStop();
				if (IS_DEADLINE_REACHED(warning_i->second.can_proceed_after))
				{
					accepted_warnings.insert(warning_i->second.hash);
					auto f = std::move(warning_i->second.proceed_callback);
					warnings.erase(warning_i);
					getCurrentUiFocusPhysical()->onFocus(thread_context, BACK);
					g_menu_grid.update();
					queueSoundJob([]
					{
						AUDIO::PLAY_SOUND_FRONTEND(-1, "CONTINUE", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
					});
					f(thread_context);
				}
				else
				{
					if (isSoundEnabled())
					{
						doSound(SOUND_ERROR, thread_context);
					}
				}
			}
			else if (isPromptActive())
			{
				prompt_yes_callback(thread_context);
				clearPrompt(thread_context);
				queueSoundJob([]
				{
					AUDIO::PLAY_SOUND_FRONTEND(-1, "CONTINUE", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
				});
			}
			else if (!isAwaitingSetHotkeyInput())
			{
				auto* const list = getCurrentUiList();
				if (list != nullptr)
				{
					Command* const focus = list->getFocus();
					if (focus != nullptr)
					{
						auto* physical = focus->getPhysical();
						if (physical != nullptr)
						{
							Click click(CLICK_MENU, thread_context);
							if (physical->isT<CommandList>() && list != physical && !physical->isTab())
							{
								if (list != physical->as<CommandList>()->parent)
								{
									physical->as<CommandList>()->parent_for_back = list->getWeakRef();
								}
							}
							physical->onClick(click);
							if (click.isSoundAllowed())
							{
								queueSound(click.sound_feedback);
							}
							SOUP_IF_UNLIKELY (Tutorial::state == TUT_CLICK)
							{
								Tutorial::setState(TUT_CLICK + 1);
							}
						}
						else
						{
							if (sound)
							{
								queueSound(SOUND_ERROR);
							}
						}
					}
				}
			}
		}
	}

	void Gui::inputBack(ThreadContext thread_context, bool play_sound)
	{
		if (Tutorial::state >= TUT_BACK)
		{
			if (isAwaitingSetHotkeyInput())
			{
				abortSetHotkey();
				queueSoundJob(thread_context, play_sound, []
				{
					AUDIO::PLAY_SOUND_FRONTEND(-1, "CANCEL", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
				});
			}
			else if (isPromptActive())
			{
				prompt_no_callback(thread_context);
				clearPrompt(thread_context);
				queueSoundJob(thread_context, play_sound, []
				{
					AUDIO::PLAY_SOUND_FRONTEND(-1, "CANCEL", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
				});
			}
			else
			{
				const auto warning_i = warnings.find(root_cursor);
				if (warning_i != warnings.end())
				{
					inputStop();
					auto f = std::move(warning_i->second.cancel_callback);
					warnings.erase(warning_i);
					getCurrentUiFocusPhysical()->onFocus(thread_context, BACK);
					g_menu_grid.update();
					queueSoundJob(thread_context, play_sound, []
					{
						AUDIO::PLAY_SOUND_FRONTEND(-1, "CANCEL", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
					});
					f(thread_context);
				}
				else
				{
					auto* active_list = getCurrentUiList();
					if (active_list->parent != nullptr && (!active_list->isTab() || !g_renderer.areTabsEnabled()))
					{
						queueSoundJob(thread_context, play_sound, []
						{
							AUDIO::PLAY_SOUND_FRONTEND(-1, "BACK", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
						});
						CommandList* const prev_list = active_list;
						auto prev_focus = prev_list->getFocus()->getWeakRef();
						active_list->resetCursorIfApplicable();
						active_list->onBack(thread_context);
						auto* const prev_active_list = active_list;
						auto* const tab = getCurrentMenuTab();
						active_list = active_list->getParentForBack(tab);
						if (g_auth.isStateConsistentRaw())
						{
							if (ContextMenu::isOpen())
							{
								ContextMenu::view = active_list;
							}
							else
							{
								m_active_list[root_cursor] = active_list;
							}
						}
						active_list->onActiveListUpdate();
						g_gui.lerpStop();
						active_list->updateCursorGoingBack(prev_active_list);
						updateActiveFocus(thread_context, BACK, prev_focus);
						g_gui.stupid_back_counter = 0;

						// Fix onActiveListUpdate not being called on empty list
						if (!prev_focus.isValid())
						{
							prev_active_list->onActiveListUpdate();
						}

						SOUP_IF_UNLIKELY (Tutorial::state == TUT_BACK)
						{
							Tutorial::setState(TUT_BACK + 1);
						}
					}
					else if (ContextMenu::isOpen())
					{
						ContextMenu::close(thread_context);
						g_menu_grid.update();
						queueSoundJob(thread_context, play_sound, []
						{
							AUDIO::PLAY_SOUND_FRONTEND(-1, "CANCEL", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
						});
					}
					else if (back_resets_cursor && active_list->m_cursor != 0)
					{
						auto prev_focus = active_list->getFocus()->getWeakRef();
						active_list->resetCursor();
						updateActiveFocus(thread_context, UP, prev_focus);
					}
					else if (back_closes_menu)
					{
						close(thread_context);
						sfxClose(thread_context);
					}
					else if (user_understands_navigation)
					{
						queueSoundJob([]
						{
							AUDIO::PLAY_SOUND_FRONTEND(-1, "ERROR", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
						});
						if (++stupid_back_counter == 3)
						{
							stupid_back_counter = 0;
							if (menu_navigation_reminders)
							{
								Util::ensureScriptThreadIfAllowed(thread_context, []
								{
									Util::show_corner_help(LANG_FMT("TUT_HDE", getMenuOpenCloseButtonString()), true);
								});
							}
						}
					}
				}
			}
		}
	}

	void Gui::inputStop() noexcept
	{
		keyboard_pressing = MENUKEY_NONE;
	}

	void Gui::shellExecute(const char* uri)
	{
		g_renderer.stop_input();
		ShellExecuteA(nullptr, nullptr, uri, nullptr, nullptr, SW_SHOW);
	}

	void Gui::shellExecute(const wchar_t* uri)
	{
		g_renderer.stop_input();
		ShellExecuteW(nullptr, nullptr, uri, nullptr, nullptr, SW_SHOW);
	}

	void Gui::prepareForOverlay() noexcept
	{
		inputStop();
		g_renderer.gameplayState = OVERLAY;
	}

	void Gui::showCommandBox()
	{
		if (isInBadBoyTimeout())
		{
			return;
		}
		inputStop();
		g_commandbox.active = command_box_enabled;
		g_commandbox.onFocus();
		g_commandbox.opened_for_chaining = (GET_MILLIS_SINCE(g_commandbox.last_command_issuance) < 1000);
		SOUP_IF_UNLIKELY (Tutorial::state == TUT_CMD)
		{
			Tutorial::setState(TUT_DONE);
			user_understands_commands = true;
			saveTutorialFlags();
		}
	}

	void Gui::showCommandBox(std::wstring&& prefill)
	{
		if (isInBadBoyTimeout())
		{
			return;
		}
		showCommandBox();
		g_commandbox.setText(std::move(prefill));
	}

	void Gui::showCommandBox(const std::wstring& prefill, Click& click)
	{
		if (!click.issuedByUser())
		{
			click.setResponse(LIT(StringUtils::utf16_to_utf8(prefill)));
		}
		else if (click.isWeb())
		{
			g_relay.sendLineAsync(std::move(soup::ObfusString("show_command_box ").str().append(StringUtils::utf16_to_utf8(prefill))));
		}
		else if (click.type & CLICK_FLAG_CHAT)
		{
			Chat::ensureOpenWithEmptyDraft(click.type != CLICK_CHAT_ALL);
			auto chat_prefill = std::wstring(1, ChatCommandsCommon::getEffectiveChatPrefix());
			chat_prefill.append(prefill);
			Chat::addToDraft(std::move(chat_prefill));
		}
		else if (!g_commandbox.active)
		{
			showCommandBox(std::wstring(prefill));
		}
	}

	void Gui::triggerHotkey(std::vector<CommandPhysical*>&& matches) const
	{
		std::vector<Click> responses{};
		for (auto& command : matches)
		{
			Click click(CLICK_HOTKEY, TC_SCRIPT_YIELDABLE);
			command->onClick(click);
			click.ensureResponse();
			if (matches.size() > 1)
			{
				click.ensureActivationName(command->getActivationName().getLocalisedUtf8());
			}
			if (click.chain_state == ChainState::PASSED_THROUGH)
			{
				responses.emplace_back(std::move(click));
			}
			else
			{
				if (!g_gui.suppress_generic_hotkey_responses || click.response_type != RESPONSE_GENERIC)
				{
					click.respond();
				}
				if (click.isSoundAllowed())
				{
					playSound(click.sound_feedback);
				}
				if (click.chain_state == ChainState::EXECUTED)
				{
					for (auto& response : responses)
					{
						response.forgetResponse();
					}
					responses.clear();
					break;
				}
			}
		}
	}

	std::vector<Hotkey> Gui::triggerHotkeyDown(Hotkey hotkey) const
	{
		std::vector<CommandPhysical*> matches{};
		std::vector<Hotkey> hold_mode_matches{};
		EXCEPTIONAL_LOCK(g_gui.commands_with_hotkeys_mtx)
		for (const auto& command : commands_with_hotkeys)
		{
			auto overlap = hotkey.getOverlap(command->hotkeys);
			if (overlap != nullptr
				&& command->canBeTriggeredInCurrentState()
				)
			{
				matches.emplace_back(command);
				if (overlap->isHoldMode())
				{
					hold_mode_matches.emplace_back(*overlap);
				}
			}
		}
		EXCEPTIONAL_UNLOCK(g_gui.commands_with_hotkeys_mtx)
		triggerHotkey(std::move(matches));
		return hold_mode_matches;
	}

	void Gui::triggerHotkeyUp(Hotkey hotkey) const
	{
		hotkey.enableHoldMode();
		std::vector<CommandPhysical*> matches{};
		EXCEPTIONAL_LOCK(g_gui.commands_with_hotkeys_mtx)
		for (const auto& command : commands_with_hotkeys)
		{
			if (hotkey.overlapsWithIncludeHoldMode(command->hotkeys)
				&& command->canBeTriggeredInCurrentState())
			{
				matches.emplace_back(command);
			}
		}
		EXCEPTIONAL_UNLOCK(g_gui.commands_with_hotkeys_mtx)
		triggerHotkey(std::move(matches));
	}

	bool Gui::triggerCommands(const std::string& input, Click& click, CommandList* const root)
	{
		return triggerCommands(StringUtils::utf8_to_utf16(input), click, root);
	}

	// TC_SCRIPT_YIELDABLE
	bool Gui::triggerCommands(std::wstring&& input, Click& click, CommandList* const root)
	{
		auto commands = StringUtils::explode_with_delimiting(input, L';');
		bool multiple_commands = (commands.size() > 1);
		bool any_success = false;
		for (auto& command : commands)
		{
			processCommand(command, click, multiple_commands, any_success, root);
		}
		if (any_success)
		{
			input.clear();
		}
		else
		{
			click.sound_feedback = SOUND_ERROR;
			click.showCommandBoxIfPossible(std::move(input));
		}
		if (click.isSoundAllowed())
		{
			playSound(click.sound_feedback);
		}
		return any_success;
	}

	bool Gui::parseCommand(std::wstring& command, std::wstring& args)
	{
		while (!command.empty() && command.at(0) == L' ')
		{
			command.erase(0, 1);
		}
		if (!command.empty())
		{
			bool collapsed = false;
			while (true)
			{
				const size_t space_off = command.find(L' ');
				if (space_off != std::wstring::npos)
				{
					auto command_name = command.substr(0, space_off);
					if (!collapsed && CommandPhysical::collapse_command_names.find(StringUtils::to_lower(std::wstring(command_name))) != CommandPhysical::collapse_command_names.end())
					{
						command.erase(space_off, 1);
						collapsed = true;
						continue;
					}
					if (command.length() > space_off + 1L)
					{
						args = command.substr(space_off + 1L);
					}
					command = std::move(command_name);
				}
				break;
			}
			StringUtils::simplifyCommandName(command);
			if (!command.empty())
			{
				return true;
			}
		}
		return false;
	}

	std::vector<CommandIssuable*> Gui::findCommandsWhereCommandNameStartsWith(const CommandName& command_name_prefix, const CommandPerm perms) const
	{
		std::vector<CommandIssuable*> commands{};
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		if (!pseudo_commands->findCommandsWhereCommandNameStartsWith(commands, command_name_prefix, perms))
		{
			root_list->findCommandsWhereCommandNameStartsWith(commands, command_name_prefix, perms);
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		return commands;
	}

	template <typename T>
	[[nodiscard]] static std::vector<soup::WeakRef<T>> toWeakrefs(const std::vector<T*>& instances)
	{
		std::vector<soup::WeakRef<T>> weakrefs;
		weakrefs.reserve(instances.size());
		for (const auto& inst : instances)
		{
			weakrefs.emplace_back(inst->getWeakRef<T>());
		}
		return weakrefs;
	}

	std::vector<soup::WeakRef<CommandIssuable>> Gui::findCommandsWhereCommandNameStartsWithAsWeakrefs(const CommandName& command_name_prefix, const CommandPerm perms) const
	{
		std::vector<soup::WeakRef<CommandIssuable>> weakrefs;
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		std::vector<CommandIssuable*> commands{};
		if (!pseudo_commands->findCommandsWhereCommandNameStartsWith(commands, command_name_prefix, perms))
		{
			root_list->findCommandsWhereCommandNameStartsWith(commands, command_name_prefix, perms);
		}
		weakrefs = toWeakrefs<CommandIssuable>(commands); // capture weakrefs while we still have the lock
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		return weakrefs;
	}

	void Gui::processCommand(std::wstring& command, Click& click, bool& multiple_commands, bool& any_success, CommandList* const root) const
	{
		std::wstring args{};
		if (!parseCommand(command, args))
		{
			return;
		}
#if COMPACT_COMMAND_NAMES
		CommandName command_name = StringUtils::utf16_to_utf8(command);
#else
		CommandName& command_name = command;
#endif
		std::vector<CommandIssuable*> commands{};
		if (root == nullptr)
		{
			commands = findCommandsWhereCommandNameStartsWith(command_name, click.getEffectivePermissions());
		}
		else
		{
			root->findCommandsWhereCommandNameStartsWith(commands, command_name, click.getEffectivePermissions());
		}
		if (commands.size() != 1)
		{
			click.setResponseCommandUnknownOrAmbiguous(commands.empty(), command);
			click.respond();
			click.forgetResponse();
		}
		else
		{
			CommandIssuable* const command_ptr = commands.at(0);
			if (!command_ptr->isAuthorised(click))
			{
				click.setResponse(LIT(fmt::format(fmt::runtime(Lang::get(command_ptr->perm == COMMANDPERM_USERONLY ? ATSTRINGHASH("CMDISS") : ATSTRINGHASH("CMDPERM"))), command_ptr->getActivationNameLocalisedUtf8())));
				click.respond();
				click.forgetResponse();
			}
			else
			{
				command_ptr->onCommand(click, args);
				if (!args.empty())
				{
					multiple_commands = true;
				}
				if (!suppress_generic_command_box_responses || click.response_type != RESPONSE_GENERIC)
				{
					click.ensureResponse();
					if (multiple_commands)
					{
						click.ensureActivationName(command_ptr->getActivationNameLocalisedUtf8());
					}
					click.respond();
				}
				click.forgetResponse();
				if (!args.empty())
				{
					processCommand(args, click, multiple_commands, any_success, root);
				}
				any_success = true;
			}
		}
	}

	bool Gui::on_command_state_change()
	{
		if (user_understands_navigation
			&& user_understands_commands
			&& user_understands_sliders
			)
		{
			if (profiles_tutorial < 3)
			{
				++profiles_tutorial;
			}
		}
		if (auto_save_state.empty())
		{
			return false;
		}
		if (!command_state_change_save.has_value())
		{
			command_state_change_save = get_current_time_millis() + 10'000;
		}
		return true;
	}

	void Gui::saveActiveState()
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		if (isRootStateFull())
		{
			saveStateInMemory(active_profile.data);
			active_profile.save();
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void Gui::saveState(std::wstring&& name)
	{
		Exceptional::createManagedExceptionalThread(__FUNCTION__, [name{ std::move(name) }]() mutable
		{
			EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
			if (g_gui.isRootStateFull())
			{
				ConfigState state(std::move(name));
				g_gui.saveStateInMemory(state.data);
				if (state.getName() == g_gui.active_profile.getName())
				{
					g_gui.active_profile.data = state.data;
				}
				state.saveToFileNow();
			}
			EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
			if (g_gui.unload_state == UnloadState::SAVE_PENDING_CONFIGS)
			{
				g_gui.backToNormalProceedToApplyDefaultState();
			}
		});
	}

	void Gui::saveStateInMemory(std::unordered_map<std::string, std::string>& state)
	{
		state.clear();
		root_list->recursivelySaveStateInMemory(state);
	}

	void Gui::cancelPendingSave()
	{
		command_state_change_save = std::nullopt;
		active_profile.setSaveNoLongerNeeded();
	}

	void Gui::recursivelyApplyState(std::unordered_map<std::string, std::string>& state, CommandList* list, const std::string& prefix /*= {}*/)
	{
		Click click(CLICK_BULK, TC_SCRIPT_NOYIELD);
		for (auto& _command : list->children)
		{
			SOUP_IF_UNLIKELY (!_command->isPhysical())
			{
				continue;
			}
			auto* command = (CommandPhysical*)_command.get();
			SOUP_IF_UNLIKELY (command->flags & CMDFLAG_STATE_AND_CHECK_FINISHLIST)
			{
				return;
			}
			std::string name = std::string(prefix).append(command->getNameForConfig());
			__try
			{
				if (command->supportsStateOperations()
					&& command->supportsSavedState()
					)
				{
					const auto state_i = state.find(name);
					if (state_i != state.end())
					{
#if GUI_DEBUG
						persistent_debug_text = std::string(name).append(": ").append(state_i->second);
#endif
						command->setState(click, state_i->second);
					}
					else
					{
#if GUI_DEBUG
						persistent_debug_text = std::string(name).append(": [Default]");
#endif
						command->applyDefaultState();
					}
				}
				if (command->isListNonAction())
				{
					name.push_back('>');

#if PROFILE_STATE
					uint64_t ticks;
					QueryPerformanceCounter((LARGE_INTEGER*)&ticks);
#endif

					recursivelyApplyState(state, (CommandList*)command, name);

#if PROFILE_STATE
					uint64_t ticks2;
					QueryPerformanceCounter((LARGE_INTEGER*)&ticks2);

					ticks2 -= ticks;

					auto ms = (ticks2 / 10000);
					auto fraction = (ticks2 % 10000);

					g_logger.log(fmt::format("{}.{:0>5} ms for {}", ms, fraction, name));
#endif
				}
			}
			__EXCEPTIONAL()
			{
				Util::toast(std::move(std::string("Failed to set state for ").append(name)), TOAST_ALL);
			}
		}
#if GUI_DEBUG
		persistent_debug_text.clear();
#endif
	}

	void Gui::recursivelyApplyActiveState()
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		current_bulk_state_operation = BSO_LOAD;
		recursivelyApplyState(active_profile.data, root_list.get());
		current_bulk_state_operation = BSO_NONE;
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void Gui::recursivelyApplyDefaultState()
	{
		EXCEPTIONAL_LOCK_WRITE(root_mtx)
		current_bulk_state_operation = BSO_DEFAULT;
		root_list->recursivelyApplyDefaultState();
		current_bulk_state_operation = BSO_NONE;
		EXCEPTIONAL_UNLOCK_WRITE(root_mtx)
	}

	void Gui::performRootCheck(const CommandList* const list, std::unordered_map<CommandName, std::string>& command_names_map)
	{
		for (auto& _command : list->children)
		{
			SOUP_IF_UNLIKELY (!_command->isPhysical())
			{
				continue;
			}
			auto* command = (CommandPhysical*)_command.get();
			SOUP_IF_UNLIKELY (command->flags & CMDFLAG_STATE_AND_CHECK_FINISHLIST)
			{
				break;
			}
			std::string path = command->getPathConfig();
#if GUI_DEBUG
			persistent_debug_text = path;
#endif
			if (command->canBeResolved())
			{
				Command* const resolved = root_list->resolveCommandConfig(path);
				if (resolved == nullptr)
				{
					g_logger.log(std::move(std::string(path).append(" could not be resolved")));
				}
				else if (resolved != command)
				{
					g_logger.log(std::move(std::string(path).append(" resolved to an unexpected address")));
				}
			}
			for (const auto& command_name : command->command_names)
			{
				const auto map_i = command_names_map.find(command_name);
				if (map_i == command_names_map.end())
				{
					command_names_map.emplace(command_name, path);
				}
				else
				{
					g_logger.log(fmt::format("{} is shared between {} and {}", cmdNameToUtf8(command_name), map_i->second, path));
				}
			}
			if (command->isList())
			{
				performRootCheck((CommandList*)command, command_names_map);
			}
			if (isUnloadPending())
			{
				break;
			}
		}
#if GUI_DEBUG
		persistent_debug_text.clear();
#endif
	}

	void Gui::loadStateToMemory(ConfigState& state)
	{
		state.load();
	}

	bool Gui::loadTutorialFlags(const std::unordered_map<std::string, std::string>& state)
	{
		user_understands_navigation = MapUtil::hasKeyValue(state, "Finished Tutorial", "Yes");
		user_understands_commands = MapUtil::hasKeyValue(state, "Understands Commands", "Yes");
		user_understands_sliders = MapUtil::hasKeyValue(state, "Understands Sliders", "Yes");
		profiles_tutorial = MapUtil::hasKeyValue(state, "Understands Profiles", "Yes") ? 4 : 0;
		user_understands_context_menu = MapUtil::hasKeyValue(state, "Understands Context Menu", "Yes");
		user_understands_controller_open_close = MapUtil::hasKeyValue(state, "Understands Controller Open/Close", "Yes");
		user_prompted_for_controller_disables = MapUtil::hasKeyValue(state, "Prompted For Controller Disables", "Yes");
		user_understands_command_chaining = MapUtil::hasKeyValue(state, "Understands Command Chaining", "Yes");
		return user_understands_navigation;
	}

	void Gui::enableAutosave(const std::wstring& profile_name)
	{
		std::wstring name = L"Profiles\\";
		name.append(profile_name);
		auto_save_state = std::move(name);
	}

	void Gui::disableAutosave() noexcept
	{
		auto_save_state.clear();
		command_state_change_save = std::nullopt;
	}

	std::wstring Gui::getActiveStateNameUtf16() const
	{
		return active_profile.getName().substr(9);
	}

	std::string Gui::getActiveStateNameUtf8() const
	{
		return StringUtils::utf16_to_utf8(getActiveStateNameUtf16());
	}

	bool Gui::canUpdateThemeSetting() const noexcept
	{
		return !about_to_update_root_state && canUpdateThemeSettingWithDefaultDependency();
	}

	bool Gui::canUpdateThemeSettingWithDefaultDependency() const noexcept
	{
		return !isUnloadPending();
	}

	bool Gui::isRootStateFreeEdition() const noexcept
	{
		return root_state == GUI_FREEONLINE || root_state == GUI_FREE;
	}

	bool Gui::shouldRootStateShowLicensePrompt() const noexcept
	{
		return root_state >= GUI_FREEONLINE && root_state < GUI_BASIC;
	}

	bool Gui::isFastLoad() noexcept
	{
		//return !is_session_or_transition_active(true);
		return false;
	}

	bool Gui::isRootUpdatePendingOrInProgress() const noexcept
	{
		return about_to_update_root_state || root_update_in_progress;
	}

	void Gui::updateRootName()
	{
		root_list->setMenuName(LIT(RootNameMgr::getCurrentRootNameForState()));
	}

	void Gui::updateTabs(ThreadContext thread_context)
	{
		if (ContextMenu::isOpen())
		{
			ContextMenu::close(thread_context);
		}
		updateTabsIgnoreContextMenu();
	}

	void Gui::updateTabsIgnoreContextMenu()
	{
		m_active_list.clear();
		if (g_renderer.areTabsEnabled())
		{
			m_active_list.reserve(root_list->children.size());
			for (const auto& tab : root_list->children)
			{
				m_active_list.emplace_back(tab->as<CommandList>());
			}
		}
		else
		{
			m_active_list.emplace_back(root_list.get());
			root_cursor = 0;
		}
	}

	void Gui::onTabRenderModeUpdate()
	{
		for (const auto& tab : root_list->children)
		{
			tab->as<CommandTab>()->updateRenderName();
		}
	}

	bool Gui::isWebGuiActive() const
	{
		return web_focus != nullptr;
	}

	void Gui::sendRootListToWeb() const
	{
		EXCEPTIONAL_LOCK(g_relay.send_mtx)
		g_relay.sendRaw("tabs\n");
		for (const auto& command : root_list->children)
		{
			g_relay.sendLine(std::move(std::string("b ").append(command->getPhysical()->menu_name.getWebString())));
		}
		EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
	}

	void Gui::webGoBack(uint8_t steps)
	{
		for (; steps != 0; --steps)
		{
			g_relay.sendRaw("<\n");
			web_focus = web_focus->parent;
		}
		web_focus->updateWebStateImpl();
	}

	void Gui::addHotkeyToFocusedCommand()
	{
		hotkey_state = HS_ADD;
		g_menu_grid.update();
	}

	void Gui::changeHotkeyOnFocusedCommand()
	{
		hotkey_state = HS_CHANGE;
		g_menu_grid.update();
	}

	bool Gui::isAwaitingSetHotkeyInput() const noexcept
	{
		return hotkey_state != HS_NONE;
	}

	void Gui::onSetHotkeyInput(const Hotkey hotkey)
	{
		auto* const command = getCurrentMenuFocusPhysical();
		if (command != nullptr)
		{
			command->modifyHotkeys(hotkey_state == HS_ADD, hotkey, CLICK_MENU);
		}
		abortSetHotkey();
	}

	void Gui::abortSetHotkey() noexcept
	{
		hotkey_state = HS_NONE;
		g_menu_grid.update();
	}

	void Gui::recursivelyApplyHotkeys(CommandList* list, const std::string& prefix)
	{
		for (auto& _command : list->children)
		{
			if (!_command->isPhysical())
			{
				continue;
			}
			auto* command = (CommandPhysical*)_command.get();
			if (command->flags & CMDFLAG_HOTKEYS_FINISHLIST)
			{
				return;
			}
			std::string name = std::string(prefix).append(command->getNameForConfig());
			if (!(command->flags & CMDFLAG_TEMPORARY))
			{
				const auto hotkey_i = hotkeys.data.find(name);
				if (hotkey_i == hotkeys.data.end())
				{
					command->applyDefaultHotkeys();
				}
				else
				{
					command->hotkeys = hotkey_i->second;
				}
				command->checkAddToCommandsWithHotkeys();
				command->onHotkeysChanged(CLICK_BULK);
			}
			if (command->isList())
			{
				name.push_back('>');
				recursivelyApplyHotkeys((CommandList*)command, name);
			}
		}
	}

	bool Gui::isPromptActive() const noexcept
	{
		return !prompt_text.empty();
	}

	void Gui::showPrompt(ThreadContext thread_context, std::wstring&& prompt_text, std::function<void(ThreadContext)>&& yes_callback, std::function<void(ThreadContext)>&& no_callback)
	{
		openForcefully(thread_context);
		inputStop();
		g_gui.getCurrentUiFocusPhysical()->onBlur(thread_context, FORWARD);
		this->prompt_text = std::move(prompt_text);
		this->prompt_yes_callback = std::move(yes_callback);
		this->prompt_no_callback = std::move(no_callback);
		g_menu_grid.update();
	}

	void Gui::showPrompt(ThreadContext thread_context, const std::string& prompt_text, std::function<void(ThreadContext)>&& yes_callback, std::function<void(ThreadContext)>&& no_callback)
	{
		return showPrompt(thread_context, StringUtils::utf8_to_utf16(prompt_text), std::move(yes_callback), std::move(no_callback));
	}

	void Gui::clearPrompt(ThreadContext thread_context)
	{
		g_gui.getCurrentUiFocusPhysical()->onFocus(thread_context, BACK);
		prompt_text.clear();
		prompt_yes_callback = nullptr;
		prompt_no_callback = nullptr;
		g_menu_grid.update();
	}

	void Gui::backToNormal(Click& click)
	{
		if (isUnloadPending())
		{
			return;
		}

		if (g_auth.hasApiCredentials())
		{
			HttpRequestBuilder b(HttpRequestBuilder::POST, soup::ObfusString("stand.sh"), soup::ObfusString("/api/logout"));
			b.setPayload(g_auth.activation_key_to_try);
			b.dispatch();
		}

		unload_state = UnloadState::REQUESTED;
		if (!g_renderer.stream_proof)
		{
			g_toaster = &g_game_toaster;
			click.setResponse(LOC("UNLD_T"));
		}
		if (opened)
		{
			// we need to update the grid but also ensure active command loses focus
			// conveniently, resetCursor does both
			g_gui.mouse_mode_set_focus = false; // however, we want to avoid the mouse being moved
			resetCursor();
		}
		else
		{
			g_menu_grid.update();
		}
		CleanupHelper::execScript();
		if (command_state_change_save.has_value()) // Autosave pending?
		{
			command_state_change_save = std::nullopt;
			unload_state = UnloadState::SAVE_PENDING_CONFIGS;
			saveState(std::wstring(auto_save_state));
		}
		else
		{
			backToNormalProceedToApplyDefaultState();
		}
	}

	void Gui::backToNormalNonScript()
	{
		backToNormalCommonNonScript();
		FiberPool::queueJob([]
		{
			CleanupHelper::execScript();
			CleanupHelper::execScriptForce();
			g_gui.unload_state = UnloadState::HAND_OVER_TO_MAIN;
		});
	}

	void Gui::backToNormalCommonNonScript()
	{
		BgScript::shutdown();
		CleanupHelper::execNonScript();
	}

	void Gui::backToNormalProceedToApplyDefaultState()
	{
		unload_state = UnloadState::APPLY_DEFAULT_STATE;
		FiberPool::queueJob([]
		{
			g_gui.recursivelyApplyDefaultState();

			Script::current()->yield(); // CommandWorldDoors needs an extra tick for its fiber to disable tbDeceiveInteriorChecks
#ifdef STAND_DEBUG
			// Native tables should be clean now, but in debug build we'll want to have notifications in case that's false.
			CleanupHelper::execScriptForce();
#endif

			// Proceed to empty player list
			SOUP_IF_LIKELY (g_gui.player_list != nullptr)
			{
				g_gui.unload_state = UnloadState::EMPTY_PLAYER_LIST;
			}
			else
			{
				g_gui.backToNormalProceedToSwitchToGuiNone();
			}
		});
	}

	void Gui::backToNormalProceedToSwitchToGuiNone()
	{
		unload_state = UnloadState::SWITCH_TO_GUI_NONE;
		Worker::queueJob([]
		{
			backToNormalCommonNonScript();
			RootMgr::updateRootState();
			g_gui.unload_state = UnloadState::HAND_OVER_TO_MAIN;
			g_running = false;
		});
	}

	void Gui::removePlayer(const AbstractPlayer p, CommandPlayer* const player_command)
	{
		// Player list
		player_commands[p] = nullptr;
		player_command->queued_for_deletion = true;
		player_command->onLeave();
		FiberPool::queueJob([p, player_command]
		{
			if (player_command->isCurrentUiList())
			{
				player_command->getFocus()->getPhysical()->onBlur(TC_SCRIPT_YIELDABLE, BACK);
			}
			player_command->evacuateCursor();

			EXCEPTIONAL_LOCK_WRITE_YIELDING(g_gui.root_mtx)
			player_command->recursivelyApplyDefaultState();
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)

			// Need to give an extra tick because if Lua change handler needs queueJob, we might be deleting the command in the same tick,
			// but before the queued job.
			Script::current()->yield();
			Script::current()->yield();

			EXCEPTIONAL_LOCK_WRITE_YIELDING(g_gui.root_mtx)
			player_command->beGoneRootWriteLocked();
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)

			g_gui.updatePlayerCount();
		});
		// Streamer spoof
		if (AbstractPlayer::streamer_spoof != 1)
		{
			p.unspoofName();
		}
		// Tick Queues
		if (auto i = TickQueues::arm_queue.find(p); i != TickQueues::arm_queue.end())
		{
			TickQueues::arm_queue.erase(i);
		}
		for (auto i = TickQueues::disarm_queue.begin(); i != TickQueues::disarm_queue.end(); )
		{
			if ((i->first >> 32) == p)
			{
				i = TickQueues::disarm_queue.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	CommandList* Gui::getCurrentMenuTab() const
	{
		if (g_renderer.areTabsEnabled())
		{
			return root_list->children.at(root_cursor)->as<CommandList>();
		}
		return root_list.get();
	}

	CommandList* Gui::getCurrentUiList() const noexcept
	{
		if (ContextMenu::isOpen())
		{
			return ContextMenu::view;
		}
		return getCurrentMenuList();
	}

	Command* Gui::getCurrentUiFocus() const noexcept
	{
		auto* list = getCurrentUiList();
		if (list != nullptr)
		{
			return list->getFocus();
		}
		return nullptr;
	}

	CommandPhysical* Gui::getCurrentUiFocusPhysical() const noexcept
	{
		auto* focus = getCurrentUiFocus();
		if (focus != nullptr)
		{
			return focus->getPhysical();
		}
		return nullptr;
	}

	CommandList* Gui::getCurrentMenuList() const noexcept
	{
		return getCurrentMenuList(root_cursor);
	}

	CommandList* Gui::getCurrentMenuList(uint8_t tab_index) const noexcept
	{
		if (tab_index < m_active_list.size())
		{
			return m_active_list.at(tab_index);
		}
		return nullptr;
	}

	Command* Gui::getCurrentMenuFocus() const noexcept
	{
		auto* activeList = getCurrentMenuList();
		if (activeList != nullptr)
		{
			return activeList->getFocus();
		}
		return nullptr;
	}

	CommandPhysical* Gui::getCurrentMenuFocusPhysical() const noexcept
	{
		auto* focus = getCurrentMenuFocus();
		if (focus != nullptr)
		{
			return focus->getPhysical();
		}
		return nullptr;
	}

	void Gui::notifyPrevFocus(ThreadContext thread_context, Direction momentum, soup::WeakRef<Command> prev_focus, CommandList* const list)
	{
		if (auto prev_focus_ptr = prev_focus.getPointer())
		{
			if (!prev_focus_ptr->isPhysical())
			{
				// If we're leaving a search results page, we want to dispatch onBlur for the physical, even if the link will be deleted by onActiveListUpdate.
				prev_focus = prev_focus_ptr->getPhysical()->getWeakRef();
			}

			if (prev_focus_ptr->parent != list && prev_focus_ptr->parent != nullptr)
			{
				prev_focus_ptr->parent->onActiveListUpdate();
			}

			if (prev_focus_ptr = prev_focus.getPointer()) // Need to check again because onActiveListUpdate may have deleted the child
			{
				prev_focus_ptr->as<CommandPhysical>()->onBlur(thread_context, momentum);
			}
		}
	}

	void Gui::updateFocus(ThreadContext thread_context, Direction momentum, const soup::WeakRef<Command>& prev_focus) const
	{
		auto* list = getCurrentMenuList();
		notifyPrevFocus(thread_context, momentum, prev_focus, list);
		updateFocusedList(thread_context, momentum, list);
	}

	void Gui::updateFocus(ThreadContext thread_context, Direction momentum) const
	{
		auto* list = getCurrentMenuList();
		updateFocusedList(thread_context, momentum, list);
	}

	void Gui::updateActiveFocus(ThreadContext thread_context, Direction momentum, const soup::WeakRef<Command>& prev_focus) const
	{
		auto* list = getCurrentUiList();
		notifyPrevFocus(thread_context, momentum, prev_focus, list);
		updateFocusedList(thread_context, momentum, list);
	}

	void Gui::updateActiveFocus(ThreadContext thread_context, Direction momentum, Command* const prev_focus) const
	{
		auto* list = getCurrentUiList();
		notifyPrevFocus(thread_context, momentum, prev_focus, list);
		updateFocusedList(thread_context, momentum, list);
	}

	void Gui::updateFocusedList(ThreadContext thread_context, Direction momentum, CommandList* const list) const
	{
		if (list != nullptr)
		{
			list->processFocusUpdate(thread_context, momentum, true);
			if (user_understands_navigation)
			{
				processPostNavigationTutorial(list);
			}
		}
		g_menu_grid.update();
	}

	void Gui::evaluateKeyboardInput(ThreadContext thread_context)
	{
		if (opened
			&& keyboard_pressing != MENUKEY_NONE
			&& g_renderer.doesGameplayStateAllowMenuNavigateInput()
			&& !isRootUpdatePendingOrInProgress()
			&& next_keyboard_eval <= get_current_time_millis()
			)
		{
			if (keyboard_hold != 1 && keyboard_hold != -2)
			{
				if (keyboard_pressing == MENUKEY_ROOT_UP)
				{
					if (!inputRootUp(thread_context, keyboard_hold > 0))
					{
						keyboard_hold = -3;
					}
				}
				else if (keyboard_pressing == MENUKEY_ROOT_DOWN)
				{
					if (!inputRootDown(thread_context, keyboard_hold > 0))
					{
						keyboard_hold = -3;
					}
				}
				else if (keyboard_pressing == MENUKEY_UP)
				{
					if (inputUp(thread_context, keyboard_hold > 0))
					{
						queueSoundJob([]
						{
							AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
						});
					}
					else
					{
						keyboard_hold = -3;
					}
				}
				else if (keyboard_pressing == MENUKEY_DOWN)
				{
					if (inputDown(thread_context, keyboard_hold > 0))
					{
						queueSoundJob([]
						{
							AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
						});
					}
					else
					{
						keyboard_hold = -3;
					}
				}
				else if (keyboard_pressing == MENUKEY_LEFT)
				{
					if (inputLeft(thread_context, keyboard_hold > 0))
					{
						sfxLeftRight();
					}
					else
					{
						keyboard_hold = -3;
					}
				}
				else if (keyboard_pressing == MENUKEY_RIGHT)
				{
					if (inputRight(thread_context, keyboard_hold > 0))
					{
						sfxLeftRight();
					}
					else
					{
						keyboard_hold = -3;
					}
				}
				else if (keyboard_pressing == MENUKEY_CLICK)
				{
					inputClick(thread_context);
				}
				else if (keyboard_pressing == MENUKEY_BACK)
				{
					inputBack(thread_context, true);
				}
			}
			if (keyboard_hold < 20)
			{
				if (keyboard_hold == -1)
				{
					keyboard_hold = 2;
				}
				else
				{
					keyboard_hold++;
				}
			}
			next_keyboard_eval = get_current_time_millis() + (
				(keyboard_pressing == MENUKEY_ROOT_UP || keyboard_pressing == MENUKEY_ROOT_DOWN)
					? tab_input_speed
					: (keyboard_hold == 20 ? input_speed_holding : input_speed)
				)
			;
		}
	}

	bool Gui::isInMenuNavigationContext() const
	{
		return opened
			|| Pong::isActive()
			|| CommandSnake::playing;
	}

	bool Gui::isSoundEnabled() const
	{
		return sound;
	}

	void Gui::queueSoundJob(std::function<void()>&& fiber_func) const
	{
		queueSoundJob(TC_OTHER, std::move(fiber_func));
	}

	void Gui::queueSoundJob(ThreadContext thread_context, std::function<void()>&& func) const
	{
		if (isSoundEnabled())
		{
			Util::ensureScriptThreadIfAllowed(thread_context, std::move(func));
		}
	}

	void Gui::queueSoundJob(ThreadContext thread_context, bool play_sound, std::function<void()>&& func) const
	{
		if (play_sound)
		{
			queueSoundJob(thread_context, std::move(func));
		}
	}

	void Gui::playSound(Sound sound)
	{
		switch (sound)
		{
		case SOUND_SELECT:
			AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
			break;

		case SOUND_YES:
			AUDIO::PLAY_SOUND_FRONTEND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
			break;

		case SOUND_NO:
			AUDIO::PLAY_SOUND_FRONTEND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
			break;

		case SOUND_ERROR:
			AUDIO::PLAY_SOUND_FRONTEND(-1, "ERROR", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
			break;
		}
	}

	void Gui::queueSound(Sound sound) const
	{
		Util::queueJobIfAllowed([sound]
		{
			playSound(sound);
		});
	}

	void Gui::doSound(Sound sound, ThreadContext thread_context) const
	{
		if (thread_context_is_script(thread_context))
		{
			playSound(sound);
		}
		else
		{
			queueSound(sound);
		}
	}

	bool Gui::isInteractionMenuOpen()
	{
		return (*pointers::is_session_started ? *ScriptGlobal(GLOBAL_MP_PI_OPEN).as<int*>() >= 0 : *ScriptGlobal(GLOBAL_SP_PI_OPEN).as<BOOL*>());
	}

	bool Gui::isPhoneClosed()
	{
		return SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(ATSTRINGHASH("cellphone_flashhand")) == 0;
	}

	void Gui::initMinimal()
	{
		initNonScript();
		RootMgr::updateRootStateCore();
		recursivelyApplyState(active_profile.data, root_list.get());
	}

	void Gui::initNonScript()
	{
		BgScript::bootFromBuiltin();

		PlayerHistory::retained_player_history_command = std::make_unique<CommandPlayerHistory>();
		PlayerHistory::player_history_command = PlayerHistory::retained_player_history_command->as<CommandList>();

		Exceptional::createManagedThread([]
		{
			THREAD_NAME("Init Stand");
			__try
			{
				g_gui.hotkeys.load();
				g_gui.starred_commands.load();
			}
			__EXCEPTIONAL()
			{
			}
			g_gui.initial_load_done.fulfil();
			__try
			{
				PlayerHistory::load();
			}
			__EXCEPTIONAL()
			{
			}
		});

		Exceptional::createManagedThread([]
		{
			THREAD_NAME("Init Soup");
			__try
			{
				g_gui.getNetIntel().init(true, false);
			}
			__EXCEPTIONAL()
			{
			}
			g_gui.inited_netIntel = true;
		});

		g_tunables.download();

		if (auto e = g_gui.meta_state.data.find(soup::ObfusString("Packages Source")); e != g_gui.meta_state.data.end())
		{
			g_net_interface.add<InitRepoFromCustomSourceTask>(soup::Uri(e->second));
		}
	}

#if VEHICLE_UPDATE_HELPER
	static const char* veh_classes[] = {
		"compacts",
		"sedan",
		"suv",
		"coupe",
		"muscle",
		"sport_classic",
		"sport",
		"super",
		"motorcycle",
		"off_road",
		"industrial",
		"utility",
		"van",
		"cycle",
		"boat",
		"helicopter",
		"plane",
		"service",
		"emergency",
		"military",
		"commercial",
		"rail",
	};
#endif

	void Gui::initScript()
	{
		{
			int value;
			user_was_in_online = STATS::STAT_GET_INT(ATSTRINGHASH("MPPLY_GRIEFING"), &value, -1);
		}

		init_time = get_current_time_millis();

		if (root_state == GUI_NONE)
		{
			initNonScript();
		}

#if VEHICLE_UPDATE_HELPER
		FiberPool::queueJob([]
		{
			const char* new_vehs[] = {
"castigator",
"coquette5",
"dominator10",
"driftcypher",
"driftnebula",
"driftsentinel",
"driftvorschlag",
"envisage",
"eurosX32",
"niobe",
"paragon3",
"pipistrello",
"pizzaboy",
"poldominator10",
"poldorado",
"polgreenwood",
"policet3",
"polimpaler5",
"polimpaler6",
"vorschlaghammer",
"yosemite1500",
			};
			for (const auto& veh : new_vehs)
			{
				auto hash = rage::atStringHash(veh);
				using_model(hash, [=]
				{
					std::string make_name = VEHICLE::GET_MAKE_NAME_FROM_VEHICLE_MODEL(hash);
					StringUtils::to_lower(make_name);

					std::string str = "{";
					str.append(fmt::format(R"("{}", "{}", ATSTRINGHASH("{}"), "1.68 TODO: Update Name")", veh, make_name, veh_classes[VEHICLE::GET_VEHICLE_CLASS_FROM_NAME(hash)]));
					str.append("},");
					g_logger.log(std::move(str));
				});
			}
		});
#endif

		memory_thread_running = true;
		memory_thread = Exceptional::createThread([]
		{
			THREAD_NAME("Memory");
			__try
			{
				float last_rotation[12];
				do
				{
					bool did_nothing = true;
					if (g_gui.character_scale_multiplier != 1.0f)
					{
						did_nothing = false;
						if (memcmp(last_rotation, ((CPed*)g_player_ped)->matrix.rotation, sizeof(last_rotation)) != 0)
						{
							((CPed*)g_player_ped)->matrix.scale(g_gui.character_scale_multiplier);
							memcpy(last_rotation, ((CPed*)g_player_ped)->matrix.rotation, sizeof(last_rotation));
						}
					}
					if (did_nothing)
					{
						soup::os::sleep(100);
					}
				} while (!g_gui.isUnloadPending());
			}
			__EXCEPTIONAL()
			{
			}
			g_gui.memory_thread_running = false;
		});

		metrics_thread_running = true;
		metrics_thread = Exceptional::createThread([]
		{
			THREAD_NAME("Metrics");
			do
			{
				Metrics::draws_per_second = Metrics::draws_this_second;
				Metrics::draws_this_second = 0;
				Metrics::ticks_per_second = Metrics::ticks_this_second;
				Metrics::ticks_this_second = 0;

				SOUP_IF_UNLIKELY (Metrics::ticks_per_second < 10)
				{
					if (GetForegroundWindow() == pointers::hwnd)
					{
						// This should be pretty damn rare, but there's some fuzzer crashes that while not crashing the game, do cause significant lag.
						// If the user is stupid enough to disable the timeout reaction for crash events, this should help them stay in a usable state.
						for (const auto& p : AbstractPlayer::listAll())
						{
							if (p.hasFlag(FLAG_ATTACKED)
								&& !p.isInTimeout()
								)
							{
								Util::toast(LANG_FMT("LOWFPSTIMEOUT_T", p.getName()));
								p.putInTimeout(TC_OTHER);
							}
						}
					}
				}

				soup::os::sleep(1'000);
			} while (!g_gui.isUnloadPending());
			g_gui.metrics_thread_running = false;
		});
		SetThreadPriority(metrics_thread, THREAD_PRIORITY_TIME_CRITICAL);
		
		AbstractPlayer::onInit();

		ChatHistory::fromScaleform();
	}

	void Gui::onTickInit()
	{
		if (!isRootUpdatePendingOrInProgress()
			&& initial_load_done
			&& g_tunables.ready
			&& (
				(initial_auth_done || isFastLoad())
				|| GET_MILLIS_SINCE(init_time) > 5000
			))
		{
			about_to_update_root_state = true;
			FiberPool::queueJob([]
			{
				RootMgr::updateRootStateImpl();

				if (!g_gui.user_understands_navigation)
				{
					Tutorial::start();
				}

				g_gui.worker_thread_running = true;
				g_gui.worker_thread = Exceptional::createThread(&Worker::thread_func);
				SetThreadPriority(g_gui.worker_thread, THREAD_PRIORITY_ABOVE_NORMAL);

				Script::current()->yield();

				if (is_session_started())
				{
					AntiAntiCheat::onEnteredOnline();

					if (g_player.isHost())
					{
						if (!BattlEyeServer::isRunning())
						{
							BattlEyeServer::start();
						}
					}
				}

				if (!RemoteGamer::inject_tracked_info)
				{
					Util::toast(LOC("HI"), TOAST_ALL);
#ifdef STAND_DEBUG
					Snippets::onInit();
#endif
					return;
				}
				while (!PlayerHistory::loaded_data
					|| !g_gui.initial_auth_done
					)
				{
					Script::current()->yield();
				}
				if (!RemoteGamer::isAnyTracked())
				{
					Util::toast(LOC("HI"), TOAST_ALL);
#ifdef STAND_DEBUG
					Snippets::onInit();
#endif
					return;
				}
				const bool expect_huge_delay = (RemoteGamer::tracked.size() > 100);
				if (expect_huge_delay)
				{
					Util::toast(LOC("HI_TRCK"), TOAST_ALL);
				}
				while (true)
				{
					bool any_pending = false;
					for (const auto& g : RemoteGamer::tracked)
					{
						if (g->isQueuedOrPending())
						{
							any_pending = true;
							break;
						}
					}
					if (!any_pending)
					{
						break;
					}
					Script::current()->yield();
				}
				std::vector<std::string> online_player_names{};
				for (const auto& g : RemoteGamer::tracked)
				{
					if (g->hasSession())
					{
						online_player_names.emplace_back(ScAccount::fromRID(g->rid)->getNameLazy());
					}
				}
				std::string msg;
				if (!expect_huge_delay)
				{
					msg = LANG_GET("HI_TRCK");
					msg.push_back(' ');
				}
				if (online_player_names.empty())
				{
					msg.append(LANG_GET("HI_TRCK_0"));
				}
				else
				{
					if (online_player_names.size() == 1)
					{
						msg.append(LANG_FMT("HI_TRCK_1", online_player_names.at(0)));
					}
					else
					{
						msg.append(LANG_FMT("HI_TRCK_M", StringUtils::concatListUsingLang(std::move(online_player_names))));
					}
				}
				Util::toast(std::move(msg), TOAST_ALL);

#ifdef STAND_DEBUG
				Snippets::onInit();
#endif
			});
		}
	}

	void Gui::onTick()
	{
		//drawDebugText(g_gta_module.range.base.as<void*>());

		/*if (!user_has_pressed_any_input)
		{
			for (int i = 0; i < _NUM_INPUT; i++)
			{
				if (PAD::IS_CONTROL_JUST_PRESSED(0, i))
				{
					user_has_pressed_any_input = true;
					break;
				}
			}
		}*/

#if GUI_DEBUG
		if (!persistent_debug_text.empty())
		{
			drawDebugText(persistent_debug_text);
		}
#endif

		__try
		{
			TickMgr::onTick();

			BgScript::invokeOptionalHook("t");
		}
		__EXCEPTIONAL()
		{
		}
#ifdef STAND_DEBUG
		Snippets::onTick();
#endif

		if (root_state <= GUI_MINIMAL
			&& !isUnloadPending()
			)
		{
			onTickInit();
			return;
		}

		ButtonInstructions::gui_buttons = 0;

		{
			const bool keyboard_and_mouse = (!controller_support || !NativeCallContext::canInvoke(0xA571D46727E2B718) || PAD::IS_USING_KEYBOARD_AND_MOUSE(0));
			SOUP_IF_UNLIKELY (keyboard_and_mouse != Input::keyboard_and_mouse)
			{
				Input::keyboard_and_mouse = keyboard_and_mouse;
				inputStop();
				g_menu_grid.update(); // to show/hide hotkey indicators
				if (user_understands_navigation)
				{
					if (!keyboard_and_mouse
						&& !user_understands_controller_open_close
						)
					{
						Util::show_corner_help(LANG_FMT("HLP_CTRLLR", ControllerInputConfig::getOpenCloseStringForGame()), true);
					}
				}
				else
				{
					Tutorial::update();
				}
			}
		}

		EXCEPTIONAL_LOCK(g_renderer.extras_mtx)
		SOUP_IF_UNLIKELY (no_extra_clear)
		{
			for (auto&& entry : g_renderer.extras_buffer)
			{
				g_renderer.extras.push_back(std::move(entry));
			}
		}
		else
		{
			g_renderer.extras.swap(g_renderer.extras_buffer);
		}
		EXCEPTIONAL_UNLOCK(g_renderer.extras_mtx)
		g_renderer.extras_buffer.clear();

		if (character_scale_affects_speed_command != nullptr
			&& *pointers::CLoadingScreens_ms_Context == 0
			)
		{
			if (character_scale_multiplier != 1.0f && character_scale_affects_speed_command->m_on)
			{
				need_run_speed_cleanup = true;
				need_swim_speed_cleanup = true;
				g_player_ped.getPlayerInfo()->m_run_speed = user_defined_run_speed * abs(character_scale_multiplier);
				g_player_ped.getPlayerInfo()->m_swim_speed = user_defined_swim_speed * abs(character_scale_multiplier);
			}
			else
			{
				if (user_defined_run_speed == 1.0f)
				{
					if (need_run_speed_cleanup)
					{
						need_run_speed_cleanup = false;
						updateRunSpeedIgnoreScale();
					}
				}
				else
				{
					if (user_defined_run_speed != 1.0f)
					{
						need_run_speed_cleanup = true;
						updateRunSpeedIgnoreScale();
					}
				}
				if (user_defined_swim_speed == 1.0f)
				{
					if (need_swim_speed_cleanup)
					{
						need_swim_speed_cleanup = false;
						updateSwimSpeedIgnoreScale();
					}
				}
				else
				{
					if (user_defined_swim_speed != 1.0f)
					{
						need_swim_speed_cleanup = true;
						updateSwimSpeedIgnoreScale();
					}
				}
			}
		}

		// Shitty fix for rendering issues
		if (g_renderer.m_pDevice != nullptr && g_renderer.m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
		{
			GRAPHICS::SET_SCRIPT_GFX_DRAW_BEHIND_PAUSEMENU(TRUE);
			GRAPHICS::DRAW_RECT(-FLT_MAX, -FLT_MAX, 0.0f, 0.0f, 0, 0, 0, 0, FALSE);
			GRAPHICS::SET_SCRIPT_GFX_DRAW_BEHIND_PAUSEMENU(FALSE);
		}

		SOUP_IF_LIKELY (!isUnloadPending())
		{
			g_renderer.lastTickTime = get_current_time_millis();

			Camgr::on_tick();
			DisableDripfeed::onTick();
			__try
			{
				TickQueues::onTick();
			}
			__EXCEPTIONAL()
			{
			}
			TransitionHelper::onTick();

			{
				bool session_active = *pointers::is_session_started;
				bool transition_active = is_session_transition_active(false);

				if (session_active)
				{
					if (!user_was_in_online)
					{
						user_was_in_online = true;

						AntiAntiCheat::onEnteredOnline();
					}

					*ScriptGlobal(GLOBAL_DISABLE_DRIPFEED).as<BOOL*>() = DisableDripfeed::enabled;

					GlobalsWatcher::onTickSessionActive();

					SOUP_IF_UNLIKELY (GET_MILLIS_SINCE(g_last_successful_sig_check) > 20000)
					{
						if (root_state != GUI_FREEONLINE)
						{
							opened = false;
						}
					}
				}

				const bool session_or_transition_active = (session_active || transition_active);
				SOUP_IF_UNLIKELY (session_or_transition_active != session_or_transition_was_active)
				{
					session_or_transition_was_active = session_or_transition_active;

					if (session_or_transition_active)
					{
						if (gtao_status_lang != Lang::getActiveLangForRockstar())
						{
							doGtaOnlineStatusRequest();
						}

						if (root_state == GUI_FREE)
						{
							// -> GUI_FREEONLINE
							RootMgr::updateRootState();
						}
					}
					else
					{
						if (root_state == GUI_FREEONLINE)
						{
							// -> GUI_FREE
							RootMgr::updateRootState();
						}
					}
				}
			}

			if (is_session_freeroam() != was_freeroam)
			{
				was_freeroam ^= true;
				processToggleCorrelation(TC_SCRIPT_NOYIELD, ToggleCorrelation::FREEROAM, was_freeroam);
			}

			if (g_player.isHost() != was_host)
			{
				was_host ^= true;
				processToggleCorrelation(TC_SCRIPT_NOYIELD, ToggleCorrelation::SESSION_HOST, was_host);
			}

			const bool chatting = Chat::isOpen();
			if (was_chatting != chatting)
			{
				was_chatting = chatting;
				processToggleCorrelation(TC_SCRIPT_NOYIELD, ToggleCorrelation::CHATTING, was_chatting);
			}

			if ((NativeCallContext::canInvoke(0x0CF2B696BBF945AE) && MISC::UPDATE_ONSCREEN_KEYBOARD() == 0)
				|| PLAYER::IS_SYSTEM_UI_BEING_DISPLAYED()
				)
			{
				g_renderer.gameplayState = OVERLAY;
			}
			else if (g_commandbox.active)
			{
				g_commandbox.onTick();
				g_renderer.gameplayState = COMMANDBOX;
				if (!Input::keyboard_and_mouse)
				{
					if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_FRONTEND_ACCEPT))
					{
						g_commandbox.processChar(13);
						g_commandbox.enter_released = true;
					}
					else if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_FRONTEND_CANCEL))
					{
						g_commandbox.processChar(27);
						g_commandbox.escape_released = true;
					}
				}
			}
			else if (chatting)
			{
				g_renderer.gameplayState = CHATTING;
				if (kill_loader_while_chatting)
				{
					HUD::BUSYSPINNER_OFF();
				}
			}
			else if (HUD::IS_PAUSE_MENU_ACTIVE())
			{
				g_renderer.gameplayState = PAUSED;
			}
			else if (HUD::IS_WARNING_MESSAGE_ACTIVE()
				|| *pointers::CLoadingScreens_ms_Context != 0
				)
			{
				g_renderer.gameplayState = MENUING;
			}
			else
			{
				if (!Input::keyboard_and_mouse && PAD::IS_DISABLED_CONTROL_PRESSED(0, ControllerInputConfig::menu_open_close_1) && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControllerInputConfig::menu_open_close_2))
				{
					inputOpenCloseMenu(TC_SCRIPT_NOYIELD);
					if (!user_understands_controller_open_close)
					{
						user_understands_controller_open_close = true;
						saveTutorialFlags();
					}
					if (opened
						&& user_understands_navigation
						&& !user_prompted_for_controller_disables
						)
					{
						doPromptUserForControllerDisables(TC_SCRIPT_NOYIELD);
					}
				}

				/*if (*pointers::is_session_started && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, is_writing_in_team_chat ? INPUT_MP_TEXT_CHAT_ALL : INPUT_MP_TEXT_CHAT_TEAM))
				{
					is_writing_in_team_chat = !is_writing_in_team_chat;
				}*/

				if (isInteractionMenuOpen() || !(isPhoneClosed() || (phone_disabled && phone_stay_closed)))
				{
					g_renderer.gameplayState = MENUING;
				}
				else
				{
					g_renderer.gameplayState = PLAYING;
					if (!Input::keyboard_and_mouse)
					{
						{
							std::pair<int, MenuKey> controller_keyboard_map[] = {
								{ControllerInputConfig::menu_root_up, MENUKEY_ROOT_UP},
								{ControllerInputConfig::menu_root_down, MENUKEY_ROOT_DOWN},
								{ControllerInputConfig::menu_up, MENUKEY_UP},
								{ControllerInputConfig::menu_down, MENUKEY_DOWN},
								{ControllerInputConfig::menu_left, MENUKEY_LEFT},
								{ControllerInputConfig::menu_right, MENUKEY_RIGHT},
								{ControllerInputConfig::menu_click, MENUKEY_CLICK},
								{ControllerInputConfig::menu_back, MENUKEY_BACK},
							};
							for (const auto& pair : controller_keyboard_map)
							{
								if (pair.first != ControllerInputConfig::menu_open_close_2 || !PAD::IS_DISABLED_CONTROL_PRESSED(0, ControllerInputConfig::menu_open_close_1))
								{
									mapControlInputToMenuKeyThisTick(pair.first, pair.second, INPUTTYPE_INDIFFERENT);
								}
							}
						}
						if (opened)
						{
							if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControllerInputConfig::menu_context))
							{
								ContextMenu::toggleIfAvailable(TC_SCRIPT_NOYIELD);
							}
							else if (ControllerInputConfig::command_box != -1)
							{
								if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, ControllerInputConfig::command_box))
								{
									if (Tutorial::state >= TUT_DONE)
									{
										if (isSoundEnabled())
										{
											AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
										}
										showCommandBox();
									}
								}
								else if (!isPromptActive() && !isWarningActive())
								{
									if (!user_prompted_for_controller_disables)
									{
										PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
									}
									else
									{
										if (button_instructions_mode == ButtonInstructionsMode::ALWAYS)
										{
											ButtonInstructions::gui_buttons |= ButtonInstructions::COMMANDBOX_CONTEXT;
										}
									}
								}
							}
						}
					}
				}
			}

			// Focused player stuff
			if (const Command* const command = getCurrentMenuFocus())
			{
				auto* const focused_player_command = (CommandListPlayer*)command->resolveParent(COMMAND_LIST_PLAYER);
				if(focused_player_command != nullptr)
				{
					focused_players = focused_player_command->pp->getPlayers(focused_player_command->pp->single);
					if (command == focused_player_command ? focused_player_beacon_in_list_of_all_players : focused_player_beacon_in_list_belonging_to_player)
					{
						for (const AbstractPlayer p : focused_players)
						{
							if (!focused_player_beacon_exclude_me || (p.hasPed() &&  p.getPed() != g_cam_ped))
							{
								DrawUtil3d::draw_ar_beacon(p.getPos());
							}
						}
					}
				}
				else
				{
					focused_players.clear();
				}
			}
			else
			{
				focused_players.clear();
			}

			// Automatically save command state
			if (command_state_change_save.has_value() && command_state_change_save.value() < get_current_time_millis())
			{
				command_state_change_save = std::nullopt;
				saveState(std::wstring(auto_save_state));
			}

			// Track Waypoint
			const Blip waypoint_blip = HUD::GET_FIRST_BLIP_INFO_ID(8);
			if (waypoint_blip != 0 && HUD::GET_BLIP_COLOUR(waypoint_blip) == 84)
			{
				const v3 waypoint_blip_pos = HUD::GET_BLIP_COORDS(waypoint_blip);
				if (rage::Vector2(waypoint.x, waypoint.y).distance(rage::Vector2(waypoint_blip_pos.x, waypoint_blip_pos.y)) > 1.0f)
				{
					waypoint.reset();
				}
				if (waypoint.isNull())
				{
					waypoint = rage::Vector2(waypoint_blip_pos.x, waypoint_blip_pos.y);
				}
			}
			else
			{
				if (!waypoint.isNull())
				{
					waypoint.reset();
				}
			}

			if (opened && !about_to_update_root_state)
			{
				if (mouse_mode == MouseMode::NAVIGATE)
				{
					// Process mousewheel inputs (it's here and like this because I'm not getting any WM_MOUSEHWHEEL messages via wndproc)
					mapControlInputToMenuKeyThisTick(INPUT_CURSOR_SCROLL_UP, MENUKEY_UP, INPUTTYPE_MOUSE_SCROLLWHEEL);
					mapControlInputToMenuKeyThisTick(INPUT_CURSOR_SCROLL_DOWN, MENUKEY_DOWN, INPUTTYPE_MOUSE_SCROLLWHEEL);
				}

				if (mouse_mode == MouseMode::MOVE)
				{
					PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
				}
				if (isAwaitingSetHotkeyInput())
				{
					PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
				}
				else
				{
					if (isPromptActive())
					{
						if (button_instructions_mode != ButtonInstructionsMode::NEVER)
						{
							ButtonInstructions::gui_buttons |= ButtonInstructions::YES_NO;
						}
					}
					else if (isWarningActive())
					{
						if (button_instructions_mode != ButtonInstructionsMode::NEVER)
						{
							ButtonInstructions::gui_buttons |= ButtonInstructions::ABORT_PROCEED;
						}
					}
					else
					{
						EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
						auto* list = getCurrentUiList();
						if (list->canDispatchOnTickInViewportForChildren())
						{
							cursor_t draw_cursor = 0;
							cursor_t emul_cursor = list->m_offset;
							for (std::vector<std::unique_ptr<Command>>::const_iterator i; i = list->children.cbegin() + emul_cursor, emul_cursor < (cursor_t)list->children.size(); ++emul_cursor)
							{
								auto* cmd = i->get()->getPhysical();
								if (cmd != nullptr
									&& !cmd->isConcealed()
									)
								{
									cmd->onTickInGameViewport();
								}
								if (++draw_cursor == getCommandsOnScreenLimit())
								{
									break;
								}
								i++;
							}
						}
						EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
					}
				}
			}

			ButtonInstructions::onTick();
			ButtonInstructions::feature_buttons = 0;

			if (web_focus != nullptr
				&& web_focus->canDispatchOnTickInViewportForChildren()
				&& web_focus->canDispatchOnTickInWebViewportForChildren()
				)
			{
				EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
				for (const auto& command : web_focus->children)
				{
					auto* cmd = command->getPhysical();
					if (!cmd->isConcealed())
					{
						cmd->onTickInWebViewport();
					}
				}
				EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
			}
		}
	}

	void Gui::scriptFunc()
	{
		g_gui.initScript();
		do
		{
			__try
			{
				ExecCtx::get().tc = TC_SCRIPT_NOYIELD;
				g_gui.onTick();
#if REPORT_YIELD_IN_NOYIELD
				ExecCtx::get().tc = TC_SCRIPT_YIELDABLE;
#endif
			}
			__EXCEPTIONAL()
			{
			}
			Script::current()->yield();
		} while (true);
	}

	void Gui::onPostTick()
	{
		const bool no_ragdoll = g_player_veh.isValid() ? (seatglue && !CommandFlyThroughWindscreen::instance->m_on) : grace;
		if (no_ragdoll)
		{
			PED::SET_PED_CAN_RAGDOLL(g_player_ped, false);
			PLAYER::SET_PLAYER_FALL_DISTANCE_TO_TRIGGER_RAGDOLL_OVERRIDE(g_player, 1000.0f);
			if (g_player_veh.isValid())
			{
				PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(g_player_ped, 1);
				if (!*pointers::is_session_started)
				{
					PED::SET_PED_CONFIG_FLAG(g_player_ped, 32, false);
					PED::SET_PED_CAN_BE_DRAGGED_OUT(g_player_ped, false); // causes strange behaviour in GTA Online where players act like the vehicle doesn't exist when trying to enter
				}
			}
			ragdoll_reset = false;
		}
		else if (!ragdoll_reset)
		{
			PED::SET_PED_CAN_RAGDOLL(g_player_ped, true);
			PLAYER::SET_PLAYER_FALL_DISTANCE_TO_TRIGGER_RAGDOLL_OVERRIDE(g_player, -1.0f);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(g_player_ped, 0);
			if (!*pointers::is_session_started)
			{
				PED::SET_PED_CONFIG_FLAG(g_player_ped, 32, true);
				PED::SET_PED_CAN_BE_DRAGGED_OUT(g_player_ped, true);
			}
			((CPed*)g_player_ped)->m_nPhysicalFlags.bNotDamagedByAnything = false;
			ragdoll_reset = true;
		}
		if (isGodmodeActive())
		{
			g_player_ped.godmodeEnable(no_ragdoll);
			/*if (PED::IS_PED_DEAD_OR_DYING(g_player_ped, true))
			{
				PED::RESURRECT_PED(g_player_ped);
				g_player_ped.set_pos(g_player_ped.get_pos());
			}*/
		}

		TickMgr::onPostTick();

		SOUP_IF_UNLIKELY (g_commandbox.shouldBlockGameInputs())
		{
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			
			// Chat box opens when 't' is pressed while a loading or warning screen is active.
			HUD::CLOSE_MP_TEXT_CHAT();
		}
	}

	void Gui::managePlayerList(ThreadContext thread_context)
	{
		if (player_list != nullptr
			&& PlayerHistory::inited
			)
		{
			populatePlayerList(thread_context);
		}
	}

	void Gui::populatePlayerList(ThreadContext thread_context)
	{
		const bool forced_flag_update = CommandPlayer::force_flag_update;
		bool update_player_list_web_state = false;
		std::vector<AbstractPlayer> players{};
		if (!isUnloadPending() && !killswitched)
		{
			players = AbstractPlayer::listAll();
		}
		bool managed_to_lock = false;
		for (const auto& p : players)
		{
			{
				auto* const command = player_commands[p];
				if (command != nullptr)
				{
					if (!command->force_recreate)
					{
						if (command->rid == p.getRockstarId())
						{
							command->doWork(thread_context);
							continue;
						}
						if (!command->changed_info)
						{
							command->changed_info = true;
							continue;
						}
					}
					__try
					{
						evtPlayerLeaveEvent e(p, command->name, false);
						FiberPool::queueJob([e{ std::move(e) }]() mutable
						{
							evtPlayerLeaveEvent::trigger(e);
						});
					}
					__EXCEPTIONAL()
					{
					}
					removePlayer(p, command);
				}
			}
			if (p.getPlayerInfo() == nullptr
				|| (!managed_to_lock && !g_gui.root_mtx.tryLockWrite())
				)
			{
				continue;
			}
			managed_to_lock = true;
			Command* prev_focus = nullptr;
			bool notify_join = false;
			CommandPlayer* command = nullptr;
			__try
			{
				if (AbstractPlayer::streamer_spoof != 0 && g_player != p)
				{
					p.spoofName();
				}
				prev_focus = player_list->getFocus();
				notify_join = (p != g_player && players_discovered && !is_session_transition_active(true));
				command = player_list->createChild<CommandPlayer>((compactplayer_t)p, notify_join);
			}
			__EXCEPTIONAL()
			{
			}
			PlayerListSort::update();
			__try
			{
				player_list->processChildrenUpdateWithPossibleCursorDisplacement(prev_focus, thread_context);
			}
			__EXCEPTIONAL()
			{
			}
			__try
			{
				if (command != nullptr)
				{
					player_commands[p] = command;
					command->doWork(thread_context);
				}
				evtPlayerJoinEvent e(p, notify_join);
				FiberPool::queueJob([e{ std::move(e) }]() mutable
				{
					evtPlayerJoinEvent::trigger(e);
				});
				updatePlayerCount();
				update_player_list_web_state = true;
			}
			__EXCEPTIONAL()
			{
			}
		}
		if (managed_to_lock)
		{
			g_gui.root_mtx.unlockWrite();
			if (!players_discovered)
			{
				players_discovered = true;
			}
		}
		else
		{
			// if managed_to_lock is true, we can imply that the player list was sorted, so we can skip this
			if (PlayerListSort::isDistanceBased()
				&& !PlayerListSort::isUpToDate()
				)
			{
				PlayerListSort::update();
			}
			else if (!PlayerListSort::hoisted_flags.empty()
				&& PlayerListSort::flags_have_changed_since_last_sort
				)
			{
				PlayerListSort::update();
			}
		}
		for (const auto& command : player_commands)
		{
			if (command == nullptr)
			{
				continue;
			}
			const AbstractPlayer p = command->pp->getPlayers(true).at(0);
			if (!p.exists()
				|| isUnloadPending()
				)
			{
				evtPlayerLeaveEvent e(p, command->name, true);
				FiberPool::queueJob([e{ std::move(e) }]() mutable
				{
					if (auto tf = LeaveNotifications::toast_config.getToastFlags())
					{
						Util::toast(LANG_FMT("LOGLEAVE_T", e.name), tf);
					}

					evtPlayerLeaveEvent::trigger(e);
				});
				removePlayer(p, command);
				update_player_list_web_state = true;
			}
		}
		if (update_player_list_web_state)
		{
			player_list->updateWebState();
		}
		if (forced_flag_update)
		{
			CommandPlayer::force_flag_update = false;
		}
		SOUP_IF_UNLIKELY (g_gui.unload_state == UnloadState::EMPTY_PLAYER_LIST)
		{
			if (getPlayerCountFromCommands() == 0)
			{
				backToNormalProceedToSwitchToGuiNone();
			}
		}
	}

	size_t Gui::getPlayerCountFromCommands() const
	{
		return (player_list->children.size() - (g_gui.isRootStateFull() ? 3 : 2));
	}

	void Gui::updatePlayerCount() const
	{
		if (player_list == nullptr)
		{
			return;
		}

		std::string name = LANG_GET("PLYLST");
		name.append(" (").append(fmt::to_string(getPlayerCountFromCommands()));
		if (*pointers::CLoadingScreens_ms_Context == 0
			&& ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() == TRANSITION_STATE_WAIT_JOIN_FM_SESSION
			)
		{
			name.push_back('+');
		}
		name.push_back(')');
		player_list->as<CommandTabPlayers>()->divider->setMenuName(LIT(std::move(name)));
	}

#if BAD_BOY_ENABLED
	bool Gui::isInBadBoyTimeout() const noexcept
	{
		return bad_boy_timeout_until != 0;
	}

	void Gui::putInBadBoyTimeout()
	{
		bad_boy_timeout_until = get_current_time_millis() + 10000;
		opened = true;
		resetCursor(); // will do grid update so bad boy timeout will become visible to user

		FiberPool::queueJob([]
		{
			while (g_gui.bad_boy_timeout_until > get_current_time_millis())
			{
				Script::current()->yield();
			}
			g_gui.bad_boy_timeout_until = 0;
			g_menu_grid.update();
		});
	}
#endif

	static soup::netIntel s_net_intel{};

	soup::netIntel& Gui::getNetIntel() noexcept
	{
		return s_net_intel;
	}
}
