#include "CommandLuaScript.hpp"

#include <mutex> // lock_guard

#include <lualib.h>
#include <lauxlib.h>
#include <ldebug.h>
#include <lstate.h>
#undef G
#undef isLua

#include <soup/gmBox.hpp>
#include <soup/ffi.hpp>
#include <soup/JsonObject.hpp>
#include <soup/ObfusString.hpp>
#include <soup/Poly.hpp>
#include <soup/Quaternion.hpp>
#include <soup/rand.hpp>
#include <soup/rtti.hpp>
#include <soup/sha1.hpp>
#include <soup/StringRefReader.hpp>
#include <soup/unicode.hpp>
#include <soup/Uri.hpp>

#include <soup_lua_bindings.hpp>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "AbstractPlayer.hpp"
#include "Auth.hpp"
#include "BlipUtil.hpp"
#include "CGpsSlot.hpp"
#include "CHandlingData.hpp"
#include "Chat.hpp"
#include "ChatHistory.hpp"
#include "CMiniMapBlip.hpp"
#include "CMultiplayerChat.hpp"
#include "ColonsAndTabs.hpp"
#include "ColourUtil.hpp"
#include "Commandbox.hpp"
#include "CommandboxGrid.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandLink.hpp"
#include "CommandListFolder.hpp"
#include "CommandListPlayer.hpp"
#include "CommandListSelect.hpp"
#include "CommandLuaAction.hpp"
#include "CommandLuaTextslider.hpp"
#include "CommandLuaColour.hpp"
#include "CommandLuaCompatLayer.hpp"
#include "CommandLuaList.hpp"
#include "CommandLuaListAction.hpp"
#include "CommandLuaListSelect.hpp"
#include "CommandLuaPlayerListPlayersShortcut.hpp"
#include "CommandLuaSlider.hpp"
#include "CommandLuaSliderClick.hpp"
#include "CommandLuaStart.hpp"
#include "CommandLuaTextsliderStateful.hpp"
#include "CommandLuaTextInput.hpp"
#include "CommandLuaToggle.hpp"
#include "CommandPlayer.hpp"
#include "CommandPlayerClassifier.hpp"
#include "CommandPlayerClassifierDetection.hpp"
#include "CommandRainbow.hpp"
#include "CommandReadonlyLink.hpp"
#include "CommandReadonlyName.hpp"
#include "CommandReadonlyValueCopy.hpp"
#include "CommandSessionCode.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderFloat.hpp"
#include "CommandTogglePointerDuplex.hpp"
#include "ComponentSavedata.hpp"
#include "conf.hpp"
#include "CPlayerInfo.hpp"
#include "CStatsDataMgr.hpp"
#include "CVehicleDrawHandler.hpp"
#include "drawDebugText.hpp"
#include "DrawUtil3d.hpp"
#include "evtChatEvent.hpp"
#include "evtEvent.hpp"
#include "evtFlowEventDoneEvent.hpp"
#include "evtPadShakeEvent.hpp"
#include "evtPlayerJoinEvent.hpp"
#include "evtPlayerLeaveEvent.hpp"
#include "evtTransitionFinishedEvent.hpp"
#include "FiberPool.hpp"
#include "FileLogger.hpp"
#include "fwPool.hpp"
#include "get_appdata_path.hpp"
#include "gta_cam.hpp"
#include "get_current_time_millis.hpp"
#include "get_ground_z.hpp"
#include "get_seconds_since_unix_epoch.hpp"
#include "gta_ped.hpp"
#include "gta_vehicle.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "HttpRequestBuilder.hpp"
#include "huddecl.hpp"
#include "is_session.hpp"
#include "joaat_hash_db.hpp"
#include "joaatToString.hpp"
#include "lang.hpp"
#include "LangData.hpp"
#include "lua_commons.hpp"
#include "LuaConfig.hpp"
#include "main.hpp"
#include "MenuGrid.hpp"
#include "Metrics.hpp"
#include "natives.hpp"
#include "netPeerId.hpp"
#include "PackageMgr.hpp"
#include "PlayerHistory.hpp"
#include "player_tags.hpp"
#include "PlayerListSort.hpp"
#include "PlayerProvider.hpp"
#include "PlayerProviderAll.hpp"
#include "PointerNames.hpp"
#include "pointers.hpp"
#include "Renderer.hpp"
#include "ScUtil.hpp"
#include "Script.hpp"
#include "script_handler.hpp"
#include "script_thread.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptHostUtil.hpp"
#include "StringUtils.hpp"
#include "tbArSpinner.hpp"
#include "tbScreenshotMode.hpp"
#include "TpUtil.hpp"
#include "tse.hpp"
#include "Tunables.hpp"
#include "UI3DDrawManager.hpp"
#include "Util.hpp"
#include "validate_file_name.hpp"
#include "vehicle_items.hpp"
#include "VehicleType.hpp"
#include "weapons.hpp"

namespace Stand
{
	struct SoupLuaBindingsDataProvider : public soup::LuaBindings::DataProvider
	{
		soup::netIntel& getNetIntel(lua_State* L) final
		{
			SOUP_IF_UNLIKELY (!g_gui.inited_netIntel)
			{
				throw soup::Exception("netIntel is not inited yet. Use util.is_soup_netintel_inited before attempting to use it.");
			}
			return g_gui.getNetIntel();
		}
	};

	static void onChatEvent(evtChatEvent& e)
	{
		FiberPool::queueJob([e]
		{
			for (const auto& script : CommandLuaScript::all_loaded)
			{
				luaS_invoke_void(script->L, script->chat_message_callbacks, e.sender, e.sender, e.text, e.team_chat, e.networked, e.is_auto);
			}
		});
	}

	static void onPadShake(evtPadShakeEvent& e)
	{
		FiberPool::queueJob([e]
		{
			for (const auto& script : CommandLuaScript::all_loaded)
			{
				luaS_invoke_void(script->L, script->pad_shake_callbacks, e.light_duration, e.light_intensity, e.heavy_duration, e.heavy_intensity, e.delay_after_this_one);
			}
		});
	}

	static size_t pending_join_events = 0;

	static void onPlayerJoin(evtPlayerJoinEvent& e)
	{
		++pending_join_events;
		for (const auto& script : CommandLuaScript::all_loaded)
		{
			if (e.notify)
			{
				script->collectGarbage();
			}
			luaS_invoke_void(script->L, script->player_join_callbacks, e.player, e.notify);
		}
		--pending_join_events;
	}

	static void onPlayerLeave(evtPlayerLeaveEvent& e)
	{
		while (pending_join_events != 0)
		{
			Script::current()->yield();
		}
		for (const auto& script : CommandLuaScript::all_loaded)
		{
			script->collectGarbage();
			luaS_invoke_void(script->L, script->player_leave_callbacks, e.player, e.name, e.notify);
		}
	}

	static void onFlowEventDone(evtFlowEventDoneEvent& e)
	{
		if (g_tunables.getBool(TUNAHASH("hide stand user detection from scripts")) && !FlowEvent::can_be_visible_to_script(e.type))
		{
			return;
		}

		// If the game is paused, we don't want to spam the queue, as it would make unpausing take exponentially longer.
		Util::queueJobIfAllowed([e]
		{
			for (const auto& script : CommandLuaScript::all_loaded)
			{
				if (e.extra_data.empty())
				{
					luaS_invoke_void(script->L, script->flow_event_done_callbacks, e.player, e.name);
				}
				else
				{
					luaS_invoke_void(script->L, script->flow_event_done_callbacks, e.player, e.name, e.extra_data);
				}
			}
		});
	}

	static void onTransitionFinished(evtTransitionFinishedEvent& e)
	{
		FiberPool::queueJob([e]
		{
			for (const auto& script : CommandLuaScript::all_loaded)
			{
				luaS_invoke_void(script->L, script->transition_finished_callbacks);
			}
		});
	}

	void CommandLuaScript::registerEventHandlers()
	{
		if (++event_handlers_ref == 1)
		{
			evtChatEvent::registerHandler(&onChatEvent);
			evtPadShakeEvent::registerHandler(&onPadShake);
			evtPlayerJoinEvent::registerHandler(&onPlayerJoin);
			evtPlayerLeaveEvent::registerHandler(&onPlayerLeave);
			evtFlowEventDoneEvent::registerHandler(&onFlowEventDone);
			evtTransitionFinishedEvent::registerHandler(&onTransitionFinished);
		}
	}

	void CommandLuaScript::unregisterEventHandlers()
	{
		if (--event_handlers_ref == 0)
		{
			evtChatEvent::unregisterHandler(&onChatEvent);
			evtPadShakeEvent::unregisterHandler(&onPadShake);
			evtPlayerJoinEvent::unregisterHandler(&onPlayerJoin);
			evtPlayerLeaveEvent::unregisterHandler(&onPlayerLeave);
			evtFlowEventDoneEvent::unregisterHandler(&onFlowEventDone);
			evtTransitionFinishedEvent::unregisterHandler(&onTransitionFinished);
		}
		clearEventHandlers();
	}

	void CommandLuaScript::clearEventHandlers()
	{
		chat_message_callbacks.clear();
		pad_shake_callbacks.clear();
		player_join_callbacks.clear();
		player_leave_callbacks.clear();
		flow_event_done_callbacks.clear();
		transition_finished_callbacks.clear();
	}

	// When adding new events, also update lua_util_remove_handler

	static bool remove_handler_from_vec(lua_State* L, std::vector<int>& vec, const int ref)
	{
		luaS_releaseReference(L, ref);
		auto i = std::find(vec.begin(), vec.end(), ref);
		if (i != vec.end())
		{
			vec.erase(i);
			return true;
		}
		return false;
	}

#define REMOVE_HANDLER_FROM_VEC(vec) if(remove_handler_from_vec(L, vec, ref)) { lua_pushboolean(L, true); return 1; }

	bool CommandLuaScript::canSendChatMessage(int num_recipients) noexcept
	{
		if (block_send_message)
		{
			return false;
		}
		message_ratelimit.max_window_requests = (15 * (AbstractPlayer::getPlayerCount() - 1));
		for (int i = 0; i != num_recipients; ++i)
		{
			if (!message_ratelimit.canRequest())
			{
				block_send_message = true;
				return false;
			}
			message_ratelimit.addRequest();
		}
		return true;
	}

	CommandLuaScript::CommandLuaScript(CommandList* const parent, const std::wstring& menu_name, CommandListFolderItemData&& folder_item_data)
		: CommandListFolderItem(parent, LIT(menu_name), { CMDNAME("lua") }, NOLABEL, CMDFLAG_SUPPORTS_STATE_OPERATIONS | CMDFLAG_FEATURELIST_SKIP), folder_item_data(std::move(folder_item_data))
	{
#if COMPACT_COMMAND_NAMES
		command_names.at(0).append(StringUtils::utf16_to_utf8(menu_name));
#else
		command_names.at(0).append(menu_name);
#endif
		StringUtils::simplify(command_names.at(0));

		indicator_type = LISTINDICATOR_OFF;

		populateStopped();
	}

	CommandLuaScript::~CommandLuaScript()
	{
		SOUP_IF_UNLIKELY (L != nullptr)
		{
#ifdef STAND_DEBUG
			Exceptional::report("CommandLuaScript dtor without proper state defaulting!");
#endif
			script->stop();
			lua_close(L);
		}
	}

	bool CommandLuaScript::isInstalledViaRepo() const
	{
		return PackageMgr::findByFile(getFilePath()) != nullptr;
	}

	void CommandLuaScript::onTickInGameViewport()
	{
		if (!viewed_with_repo_done)
		{
			if (g_tunables.ready)
			{
				viewed_with_repo_done = true;

				if (isInstalledViaRepo())
				{
					setHelpText(LOC("REPO_P_H"));
				}
			}
		}
	}

	void CommandLuaScript::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}

	std::string CommandLuaScript::getCode() const
	{
		return soup::string::fromFile(getFilePath());
	}

	std::string CommandLuaScript::getState() const
	{
		std::string state{};
		if (L != nullptr)
		{
			StringUtils::list_append(state, "On");
		}

		if (compat_2take1)
		{
			StringUtils::list_append(state, "2Take1Menu Compatibility");
		}
		else if (compat_kiddions)
		{
			StringUtils::list_append(state, "Kiddion's Modest Menu Compatibility");
		}

		if (restriction_force_silent_start)
		{
			StringUtils::list_append(state, "Force Silent Start");
		}
		if (restriction_force_silent_stop)
		{
			StringUtils::list_append(state, "Force Silent Stop");
		}
		if (restriction_disable_internet_access)
		{
			StringUtils::list_append(state, "Disable Internet Access");
		}
		return state;
	}

	std::string CommandLuaScript::getDefaultState() const
	{
		return "Disable Internet Access";
	}

	void CommandLuaScript::setState(Click& click, const std::string& state)
	{
		std::vector<std::string> properties = soup::string::explode(state, ", ");

		const bool on = (std::find(properties.begin(), properties.end(), "On") != properties.end());

		if (!on)
		{
			shutdownFromState();
		}

		compat_2take1 = (std::find(properties.begin(), properties.end(), "2Take1Menu Compatibility") != properties.end());
		compat_kiddions = (std::find(properties.begin(), properties.end(), "Kiddion's Modest Menu Compatibility") != properties.end());

		restriction_force_silent_start = (std::find(properties.begin(), properties.end(), "Force Silent Start") != properties.end());
		restriction_force_silent_stop = (std::find(properties.begin(), properties.end(), "Force Silent Stop") != properties.end());
		restriction_disable_internet_access = (std::find(properties.begin(), properties.end(), "Disable Internet Access") != properties.end());

		if (on)
		{
			if (L == nullptr)
			{
				hold_up_save = true;
				run_queued = true;
				FiberPool::queueJob([wr{ getWeakRef() }]
				{
					if (auto self = wr.getPointer(); !self || !self->as<CommandLuaScript>()->run_queued)
					{
						return;
					}

					// If this script is being updated, make sure we don't load the code until we have the latest version.
					while (!PackageMgr::updating_completed)
					{
						Script::current()->yield();

						if (auto self = wr.getPointer(); !self || !self->as<CommandLuaScript>()->run_queued)
						{
							return;
						}
					}

					if (auto self = wr.getPointer())
					{
						self->as<CommandLuaScript>()->run(false, self->as<CommandLuaScript>()->getCode());
					}
				});
			}
		}
	}

	void CommandLuaScript::applyDefaultState()
	{
		shutdownFromState();

		restriction_force_silent_start = false;
		restriction_force_silent_stop = false;
		restriction_disable_internet_access = true;
	}

	void CommandLuaScript::shutdownFromState()
	{
		run_queued = false;

		if (L != nullptr)
		{
			requestStop(true, true);

			// Schedule in now to finish work
			script->nestedTick();
			SOUP_ASSERT(L == nullptr);
		}
	}

	bool CommandLuaScript::isEnabledInActiveProfile() const
	{
		if (auto e = g_gui.active_profile.data.find(getPathConfig()); e != g_gui.active_profile.data.end())
		{
			std::vector<std::string> properties = soup::string::explode(e->second, ", ");

			return std::find(properties.begin(), properties.end(), "On") != properties.end();
		}
		return false;
	}

	bool CommandLuaScript::isObfuscatedCode(const std::string& code, bool is_main_file)
	{
		// Mark for precompiled code ('<esc>Lua')
		if (code.substr(0, 4) == "\x1bLua")
		{
			// Since 104.2 we allow loading of compiled code as long as we've signed it.
			soup::StringRefReader sr(code);
			sr.seekEnd();
			auto size = sr.getPosition();
			sr.seek(size - 4);
			uint32_t t;
			sr.u32(t);
			if (t == size - 517)
			{
				sr.seek(t);
				uint8_t v;
				sr.u8(v);
				if (v == 0)
				{
					auto digest = soup::sha1::hash(code.substr(0, t));

					std::string sig;
					sr.str(512, sig);

					if (g_auth.public_key.verify<soup::sha1>(digest, soup::Bigint::fromBinary(sig)))
					{
						return false;
					}

					//Util::toast("Compiled code has invalid signature");
				}
				else
				{
					//Util::toast("Compiled code extra data is not v0");
				}
			}
			else
			{
				//Util::toast("Compiled code has no extra data");
			}
			return true;
		}

		// Luraph Obfuscator
		if (code.find(";end;end;end;else") != std::string::npos
			|| code.find("else return a/z;end") != std::string::npos
			)
		{
			return true;
		}

		// Misc
		if (code.find(R"(else print("WRONG PASSWORD!")end)") != std::string::npos
			|| code.find((const char*)u8R"(else print("密码错误")end)") != std::string::npos // "wrong password"
			|| code.find(R"(t(r, u), t(r, u), t(r, u), t(r, u),)") != std::string::npos
			|| code.find(R"(function(a,b,c,d,e,f,g,h,i)local j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z)") != std::string::npos
			|| code.find(R"(function decode(str) local function binary_to_string(bin))") != std::string::npos
			|| code.find(R"(local v0="";)") != std::string::npos
			|| code.find(R"(L0_1, L1_1, L2_1)") != std::string::npos
			)
		{
			return true;
		}

		// Generic detections since obfuscated luas can be highly randomised but they're mostly "all in one line"
		{
			const auto num_lines = std::max<size_t>(1, soup::string::explode(code, '\n').size());

			if (code.size() > 100000 // more than 100.000 characters?
				&& num_lines < 100 // less than 100 lines?
				)
			{
				return true;
			}

			if (is_main_file) // resources/jackz_actions/actions_data.lua is a 1-line, 54k-character data file
			{
				if ((code.size() / num_lines) > 10000) // more than 10.000 characters per average line?
				{
					return true;
				}
			}
		}

		// "\123\69\42"
		// table.concat({"\x12", "\x34", "\x56" })
		{
			// 0 - outside string, looking for " or '
			// 1 - inside string, looking for \
			// 2 - inside string, looking for number
			// 3 - inside string, looking for \ or number
			char del = '"';
			uint8_t search_state = 0;
			uint8_t modulo = 10;
			uint8_t escaped_val = 0;
			uint8_t bytes = 0;
			for (const auto& c : code)
			{
				if (search_state == 0)
				{
					if (c == '"' || c == '\'')
					{
						del = c;
						search_state = 1;
					}
				}
				else if (c == del)
				{
					if (search_state == 3)
					{
						if (escaped_val != '\t')
						{
							if (++bytes > 100)
							{
								return true;
							}
						}
					}
					search_state = 0;
					//bytes = 0;
				}
				else if (c == '\\')
				{
					if (search_state == 1)
					{
						search_state = 2;
						modulo = 10;
					}
					else if (search_state == 3)
					{
						search_state = 2;
						if (escaped_val != '\t')
						{
							if (++bytes > 100)
							{
								return true;
							}
						}
					}
					else
					{
						search_state = 1;
						//bytes = 0;
					}
				}
				else if (soup::string::isNumberChar(c))
				{
					if (search_state == 2)
					{
						escaped_val = (c - '0');
						search_state = 3;
					}
					else if (search_state == 3)
					{
						escaped_val *= modulo;
						escaped_val += (c - '0');
					}
					else
					{
						search_state = 1;
						//bytes = 0;
					}
				}
				else if (c >= 'a' && c <= 'f')
				{
					if (modulo == 16)
					{
						if (search_state == 2)
						{
							escaped_val = 0xA + (c - 'a');
							search_state = 3;
						}
						else
						{
							escaped_val *= modulo;
							escaped_val += 0xA + (c - 'a');
						}
					}
				}
				else if (c >= 'A' && c <= 'F')
				{
					if (modulo == 16)
					{
						if (search_state == 2)
						{
							escaped_val = 0xA + (c - 'A');
							search_state = 3;
						}
						else
						{
							escaped_val *= modulo;
							escaped_val += 0xA + (c - 'A');
						}
					}
				}
				else if (c == 'n')
				{
					if (search_state == 2)
					{
						search_state = 1;
					}
				}
				else if (c == 'x')
				{
					if (search_state == 2)
					{
						modulo = 16;
					}
				}
			}
		}

		// 123,69,42
		if (!g_tunables.getBool(TUNAHASH("ALLOW_OBF_COMMA_NUMBERS")))
		{
			// 0 - looking for number
			// 1 - looking for more numbers or ,
			uint8_t search_state = 0;
			uint8_t bytes = 0;
			for (const auto& c : code)
			{
				if (soup::string::isNumberChar(c))
				{
					search_state = 1;
				}
				else if (search_state == 1 && c == ',')
				{
					if (++bytes > 100)
					{
						return true;
					}
					search_state = 0;
				}
				else if (!soup::string::isSpace(c))
				{
					bytes = 0;
				}
			}
		}

		// 0x12,0x34,0x56
		if (!g_tunables.getBool(TUNAHASH("ALLOW_OBF_COMMA_HEX_NUMBERS")))
		{
			// 0 - looking for '0'
			// 1 - looking for 'x'
			// 2 - looking for hex digits or ,
			uint8_t search_state = 0;
			uint8_t bytes = 0;
			for (const auto& c : code)
			{
				if (search_state == 0 && c == '0')
				{
					search_state = 1;
				}
				else if (search_state == 1 && c == 'x')
				{
					search_state = 2;
				}
				else if (search_state == 2 && soup::string::isHexDigitChar(c))
				{
					// maintain state 2
				}
				else if (search_state == 2 && c == ',')
				{
					if (++bytes > 100)
					{
						return true;
					}
					search_state = 0;
				}
				else if (!soup::string::isSpace(c))
				{
					bytes = 0;
				}
			}
		}

#if false
		Util::toast("does not appear obfuscated, but blocking anyway");
		return true;
#endif

		return false;
	}

	void CommandLuaScript::onObfuscatedCodeDetected()
	{
#ifdef STAND_DEV
		Util::toast(soup::ObfusString("Refusing to run this obfuscated code"));
#endif
	}
}

extern bool luaS_moderateLoad(lua_State* L, const char* code)
{
	using namespace Stand;

	if (!luaS_thisptr(L)->initial_load)
	{
		// Runtime is calling "load" to load the contents, this is fine since it was already moderated.
		luaS_thisptr(L)->initial_load = true;
	}
	else
	{
		if (CommandLuaScript::isObfuscatedCode(code, false))
		{
			return false;
		}
	}
	return true;
}

extern bool luaS_moderateLoadfile(lua_State* L, const char* _filename)
{
	using namespace Stand;

	std::string filename = _filename;
	std::string filename_lower = filename;
	StringUtils::to_lower(filename_lower);
	if (filename_lower.find("baibaiscript") != std::string::npos
		|| filename_lower.find("baibailib") != std::string::npos
		|| filename_lower.find("daidailib") != std::string::npos
		|| filename_lower.find("heezyscript") != std::string::npos
		|| filename_lower.find("heezylib") != std::string::npos
		|| filename_lower.find("ymlib") != std::string::npos
		|| filename_lower.find("ymslib") != std::string::npos
		|| filename_lower.find("sakuralib") != std::string::npos
		)
	{
		Util::toast(LOC("LUA_SCAM"));
		return false;
	}
	if (CommandLuaScript::isObfuscatedCode(soup::string::fromFile(filename), false))
	{
		return false;
	}
	return true;
}

extern bool luaS_moderateCLib(lua_State* L, const char* path)
{
	using namespace Stand;

	std::lock_guard lock(g_tunables.mtx);
	if (!g_tunables.c_module_whitelist_enabled)
	{
		return true;
	}
	const auto hash = soup::string::bin2hexLower(soup::sha1::hash(soup::string::fromFile(path)));
	for (const auto& whitelisted_hash : g_tunables.c_module_whitelist)
	{
		if (hash == whitelisted_hash)
		{
			return true;
		}
	}
#ifdef STAND_DEBUG
	Util::toast(fmt::format("C module at {} not on whitelist", path), TOAST_ALL);
#endif
	return false;
}

extern bool luaS_moderateHttpRequest(lua_State* L, const char* url)
{
	if (Stand::luaS_thisptr(L)->restriction_disable_internet_access)
	{
		return false;
	}
	return true;
}

extern bool luaS_moderateReadFile(lua_State* L, const char* path)
{
	if (strstr(path, "Local Storage")
		|| strstr(path, "Activation Key.txt")
		)
	{
		return false;
	}
	return true;
}

extern bool luaS_moderateWriteFile(lua_State* L, const char* path)
{
	return luaS_moderateReadFile(L, path);
}

namespace Stand
{
	void CommandLuaScript::addHeadCommands()
	{
		createChild<CommandLambdaAction>(LOC("LUA_STOP"), std::vector<CommandName>(1, std::move(CommandName(CMDNAME("stoplua")).append(StringUtils::simplify(menu_name.getEnglishForCommandName())))), NOLABEL, [this](Click& click)
		{
			click.stopInputIfAllowed();
			if (L != nullptr)
			{
				requestStop(restriction_force_silent_stop, false);
			}
		}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
	}

#define FOR_EACH_CMDTYPE(f) \
f(list) \
f(action) \
f(toggle) \
f(slider) \
f(slider_float) \
f(click_slider) \
f(click_slider_float) \
f(list_select) \
f(list_action) \
f(text_input) \
f(colour) \
f(rainbow) \
f(divider) \
f(readonly) \
f(readonly_name) \
f(hyperlink) \
f(action_slider) \
f(textslider) \
f(slider_text) \
f(textslider_stateful) \
f(player_list_players_shortcut) \
f(link)

#define ENABLE_SECURED_CONTENT false

	void CommandLuaScript::run(bool notify, std::string&& code)
	{
		if (L != nullptr)
		{
			hold_up_save = false;
			return;
		}

		script = Script::current();

		if (isObfuscatedCode(code, true))
		{
			onObfuscatedCodeDetected();
			hold_up_save = false;
			return evacuateCursorStandalone();
		}

		if (g_tunables.getBool(TUNAHASH("DISABLE_LUAS")))
		{
			hold_up_save = false;
			return evacuateCursorStandalone();
		}

		if (code_changes_made != 5)
		{
			const hash_t code_hash = rage::atStringHash(code);
			if (code_hash != last_code_hash)
			{
				if (last_code_hash != 0)
				{
					if (++code_changes_made == 5)
					{
						// Ew, they're a developer!

						// Make sure they're not a complete retard and getting their work deleted by auto-updating...
						if (isInstalledViaRepo())
						{
							Util::toast(LOC("LUA_REPODEV"));
						}
						else
						{
							// Alright, make sure they know about developer mode.
							if (!LuaConfig::notify_bad_practice->m_on)
							{
								Util::toast(LOC("DEVHINT"));
							}
						}
					}
				}
				last_code_hash = code_hash;
			}
		}

		resetChildrenWithPreDelete();
		addHeadCommands();
		enableBusy();
		processChildrenUpdate();
		updateWebState();

		L = luaL_newstate();
		L->l_G->user_data = &script_data;

		lua_atpanic(L, &luaS_panic_handler);
		if (LuaConfig::enable_warnings->m_on)
		{
			lua_setwarnf(L, &luaS_warnf, this);
		}
		luaL_openlibs(L);

		can_continue = true;
		is_silent_stop = false;

#if ENABLE_SECURED_CONTENT
		size_t cont_name_off = std::string::npos;
		if (auto str = soup::ObfusString("secured/").str(); code.length() > str.length())
		{
			if (code.substr(0, str.length()) == str)
			{
				cont_name_off = 8;
			}
			else
			{
				str.insert(0, 1, '\n');
				cont_name_off = code.find(str);
				if (cont_name_off != std::string::npos)
				{
					cont_name_off += str.length();
				}
			}
		}
		std::shared_ptr<std::optional<std::string>> cont_req{};
		if (cont_name_off != std::string::npos)
		{
			if (!g_auth.hasApiCredentials())
			{
				if (notify)
				{
					Util::toast(LANG_GET("SECCONT_NCRED"));
				}
				goto _abort_start;
			}

			cont_req = std::make_shared<std::optional<std::string>>();

			soup::JsonObject obj{};
			obj.add("a", g_auth.activation_key_to_try);
			obj.add("c", code.substr(cont_name_off));

			HttpRequestBuilder hrb{ HttpRequestBuilder::POST, soup::ObfusString("stand.sh"), soup::ObfusString("/api/secured_content") };
			hrb.setPayload(obj.encode());
			hrb.setResponseCallback([cont_req](soup::HttpResponse&& resp)
			{
				*cont_req = std::move(resp.body);
			});
			hrb.setFailCallback([cont_req]()
			{
				*cont_req = std::string();
			});
			hrb.dispatch();
		}
#endif

		while (!g_gui.players_discovered
#if ENABLE_SECURED_CONTENT
			|| (cont_req && !cont_req->has_value())
#endif
			)
		{
			Script::current()->yield();
			if (!can_continue)
			{
#if ENABLE_SECURED_CONTENT
			_abort_start:
#endif
				disableBusy();
				stop(false);
				return;
			}
		}

#if ENABLE_SECURED_CONTENT
		if (cont_req)
		{
			if (cont_req->value().empty())
			{
				if (notify)
				{
					Util::toast(Label::combineWithSpace(LOC("SECCONT_NAVAIL"), LOC("CONHELP")));
				}
				goto _abort_start;
			}
			code = std::move(cont_req->value());
			cont_req.reset();
		}
#endif

		flags |= CMDFLAG_NOT_STATE_FOR_CHILDREN;
		setIndicatorType(LISTINDICATOR_ON);

		registerEventHandlers();
		all_loaded.emplace_back(this);

		// Constants
		lua_pushstring(L, StringUtils::utf16_to_utf8(getRelativeFilePath()).c_str());
		lua_setglobal(L, "SCRIPT_RELPATH");

		auto filename = std::filesystem::path(getFilePath()).filename().u8string();

		std::string ext = ".lua";
		if (filename.substr(filename.length() - 4) != u8".lua")
		{
			ext = ".pluto";
			L->l_G->setCompatibilityMode(false);
		}

		{
			lua_pushstring(L, reinterpret_cast<const char*>(filename.c_str()));
			lua_setglobal(L, "SCRIPT_FILENAME");

			lua_pushstring(L, reinterpret_cast<const char*>(filename.substr(0, filename.length() - ext.length()).c_str()));
			lua_setglobal(L, "SCRIPT_NAME");
		}

		lua_pushboolean(L, notify);
		lua_setglobal(L, "SCRIPT_MANUAL_START");

		lua_pushboolean(L, !notify || restriction_force_silent_start);
		lua_setglobal(L, "SCRIPT_SILENT_START");

		showed_os_warning = (code.find("i_really_need_manual_access_to_process_apis") != std::string::npos);
		lua_pushboolean(L, showed_os_warning);
		lua_setglobal(L, "SCRIPT_MAY_NEED_OS");

		{
			std::lock_guard lock(g_tunables.mtx);
			pluto_pushstring(L, g_tunables.latest_natives_version);
			lua_setglobal(L, "LATEST_NATIVES_VERSION");
		}

		if (compat_2take1)
		{
			L->l_G->setCompatibilityMode(true);

			lua_pushboolean(L, true);
			lua_setglobal(L, "SCRIPT_2TAKE1_COMPAT");
		}
		else if (compat_kiddions)
		{
			L->l_G->setCompatibilityMode(true);

			lua_pushboolean(L, true);
			lua_setglobal(L, "SCRIPT_KIDDIONS_COMPAT");
		}

		EXPOSE_INT_CONSTANT(INVALID_GUID);

		EXPOSE_INT_CONSTANT(TOAST_ABOVE_MAP);
		EXPOSE_INT_CONSTANT(TOAST_CONSOLE);
		EXPOSE_INT_CONSTANT(TOAST_FILE);
		EXPOSE_INT_CONSTANT(TOAST_WEB);
		EXPOSE_INT_CONSTANT(TOAST_CHAT);
		EXPOSE_INT_CONSTANT(TOAST_CHAT_TEAM);
		EXPOSE_INT_CONSTANT(TOAST_DEFAULT);
		EXPOSE_INT_CONSTANT(TOAST_LOGGER);
		EXPOSE_INT_CONSTANT(TOAST_ALL);

		EXPOSE_INT_CONSTANT(CLICK_MENU);
		EXPOSE_INT_CONSTANT(CLICK_COMMAND);
		EXPOSE_INT_CONSTANT(CLICK_HOTKEY);
		EXPOSE_INT_CONSTANT(CLICK_FLAG_AUTO);
		EXPOSE_INT_CONSTANT(CLICK_BULK);
		EXPOSE_INT_CONSTANT(CLICK_AUTO);
		EXPOSE_INT_CONSTANT(CLICK_SCRIPTED);
		EXPOSE_INT_CONSTANT(CLICK_FLAG_CHAT);
		EXPOSE_INT_CONSTANT(CLICK_CHAT_ALL);
		EXPOSE_INT_CONSTANT(CLICK_CHAT_TEAM);
		EXPOSE_INT_CONSTANT(CLICK_FLAG_WEB);
		EXPOSE_INT_CONSTANT(CLICK_WEB);
		EXPOSE_INT_CONSTANT(CLICK_WEB_COMMAND);

		EXPOSE_INT_CONSTANT(COMMANDPERM_FRIENDLY);
		EXPOSE_INT_CONSTANT(COMMANDPERM_NEUTRAL);
		EXPOSE_INT_CONSTANT(COMMANDPERM_SPAWN);
		EXPOSE_INT_CONSTANT(COMMANDPERM_RUDE);
		EXPOSE_INT_CONSTANT(COMMANDPERM_AGGRESSIVE);
		EXPOSE_INT_CONSTANT(COMMANDPERM_TOXIC);
		EXPOSE_INT_CONSTANT(COMMANDPERM_USERONLY);

		EXPOSE_INT_CONSTANT(ALIGN_TOP_LEFT);
		EXPOSE_INT_CONSTANT(ALIGN_TOP_CENTRE);
		EXPOSE_INT_CONSTANT(ALIGN_TOP_RIGHT);
		EXPOSE_INT_CONSTANT(ALIGN_CENTRE_LEFT);
		EXPOSE_INT_CONSTANT(ALIGN_CENTRE);
		EXPOSE_INT_CONSTANT(ALIGN_CENTRE_RIGHT);
		EXPOSE_INT_CONSTANT(ALIGN_BOTTOM_LEFT);
		EXPOSE_INT_CONSTANT(ALIGN_BOTTOM_CENTRE);
		EXPOSE_INT_CONSTANT(ALIGN_BOTTOM_RIGHT);

		EXPOSE_INT_CONSTANT(COMMAND_FULLTYPEFLAG);
		EXPOSE_INT_CONSTANT(COMMAND_FLAG_LIST);
		EXPOSE_INT_CONSTANT(COMMAND_FLAG_LIST_ACTION);
		EXPOSE_INT_CONSTANT(COMMAND_FLAG_TOGGLE);
		EXPOSE_INT_CONSTANT(COMMAND_FLAG_SLIDER);
		EXPOSE_INT_CONSTANT(COMMAND_LINK);
		EXPOSE_INT_CONSTANT(COMMAND_FIRST_PHYSICAL);
		EXPOSE_INT_CONSTANT(COMMAND_ACTION);
		EXPOSE_INT_CONSTANT(COMMAND_ACTION_ITEM);
		EXPOSE_INT_CONSTANT(COMMAND_INPUT);
		EXPOSE_INT_CONSTANT(COMMAND_TEXTSLIDER);
		EXPOSE_INT_CONSTANT(COMMAND_TEXTSLIDER_STATEFUL);
		EXPOSE_INT_CONSTANT(COMMAND_READONLY_NAME);
		EXPOSE_INT_CONSTANT(COMMAND_READONLY_VALUE);
		EXPOSE_INT_CONSTANT(COMMAND_READONLY_LINK);
		EXPOSE_INT_CONSTANT(COMMAND_DIVIDER);
		EXPOSE_INT_CONSTANT(COMMAND_LIST);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_CUSTOM_SPECIAL_MEANING);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_PLAYER);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_COLOUR);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_HISTORICPLAYER);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_READONLY);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_REFRESHABLE);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_CONCEALER);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_SEARCH);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_NAMESHARE);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_ACTION);
		EXPOSE_INT_CONSTANT(COMMAND_LIST_SELECT);
		EXPOSE_INT_CONSTANT(COMMAND_TOGGLE_NO_CORRELATION);
		EXPOSE_INT_CONSTANT(COMMAND_TOGGLE);
		EXPOSE_INT_CONSTANT(COMMAND_TOGGLE_CUSTOM);
		EXPOSE_INT_CONSTANT(COMMAND_SLIDER);
		EXPOSE_INT_CONSTANT(COMMAND_SLIDER_FLOAT);
		EXPOSE_INT_CONSTANT(COMMAND_SLIDER_RAINBOW);

		EXPOSE_INT_CONSTANT(LISTINDICATOR_ARROW);
		EXPOSE_INT_CONSTANT(LISTINDICATOR_ARROW_IF_CHILDREN);
		EXPOSE_INT_CONSTANT(LISTINDICATOR_OFF);
		EXPOSE_INT_CONSTANT(LISTINDICATOR_ON);

		EXPOSE_INT_CONSTANT(GLOBAL_TUNABLE_BASE);

		// deprecated since Stand 100
		{
			lua_pushinteger(L, COMMAND_INPUT);
			lua_setglobal(L, "COMMAND_ACTION_VALUE");

			lua_pushinteger(L, COMMAND_TEXTSLIDER);
			lua_setglobal(L, "COMMAND_ACTION_SLIDER");

			lua_pushinteger(L, COMMAND_TEXTSLIDER_STATEFUL);
			lua_setglobal(L, "COMMAND_SLIDER_TEXT");
		}

		std::string lua_scripts_folder = StringUtils::utf16_to_utf8(get_appdata_path(R"(Lua Scripts\)"));
		std::string script_in_folder = soup::string::fixType(std::filesystem::path(getFilePath()).parent_path().u8string());
		script_in_folder.push_back('\\');

		// package.path
		{
			lua_getglobal(L, "package");
			lua_getfield(L, -1, "path");
			std::string cur_path = lua_tostring(L, -1);
			cur_path.push_back(';');
			cur_path.append(lua_scripts_folder).append("lib\\?.lua;").append(lua_scripts_folder).append("?.lua");
			cur_path.push_back(';');
			cur_path.append(lua_scripts_folder).append("lib\\?.pluto;").append(lua_scripts_folder).append("?.pluto");
			cur_path.push_back(';');
			cur_path.append(lua_scripts_folder).append("lib\\?\\init.lua;").append(lua_scripts_folder).append("?\\init.lua");
			cur_path.push_back(';');
			cur_path.append(lua_scripts_folder).append("lib\\?\\init.pluto;").append(lua_scripts_folder).append("?\\init.pluto");
			if (lua_scripts_folder != script_in_folder)
			{
				cur_path.push_back(';');
				cur_path.append(script_in_folder).append("?.lua");
				cur_path.push_back(';');
				cur_path.append(script_in_folder).append("?.pluto");
			}
			lua_pop(L, 1);
			lua_pushstring(L, cur_path.c_str());
			lua_setfield(L, -2, "path");
			lua_pop(L, 1);
		}

		// package.cpath
		{
			lua_getglobal(L, "package");
			lua_getfield(L, -1, "cpath");
			std::string cur_cpath = lua_tostring(L, -1);
			cur_cpath.push_back(';');
			cur_cpath.append(lua_scripts_folder).append("lib\\?.dll;").append(lua_scripts_folder).append("?.dll");
			if (lua_scripts_folder != script_in_folder)
			{
				cur_cpath.push_back(';');
				cur_cpath.append(script_in_folder).append("lib\\?.dll;").append(script_in_folder).append("?.dll");
			}
			lua_pop(L, 1);
			lua_pushstring(L, cur_cpath.c_str());
			lua_setfield(L, -2, "cpath");
			lua_pop(L, 1);
		}

		luaS_openMenu(L);

		LIB(players,
			LIB_FUNC(players, on_join)
			LIB_FUNC(players, on_leave)
			LIB_FUNC(players, internal_get_join_callbacks)
			LIB_FUNC(players, exists)
			LIB_FUNC(players, user)
			LIB_FUNC(players, user_ped)
			LIB_FUNC(players, list)
			LIB_FUNC(players, list_only)
			LIB_FUNC(players, list_except)
			LIB_FUNC(players, list_all_with_excludes)
			LIB_FUNC(players, get_host)
			LIB_FUNC(players, get_script_host)
			LIB_FUNC(players, get_focused)
			LIB_FUNC(players, get_name)
			LIB_FUNC(players, get_rockstar_id)
			LIB_FUNC(players, get_rockstar_id_2)
			LIB_FUNC(players, get_ip)
			LIB_FUNC(players, get_ip_string)
			LIB_FUNC(players, get_port)
			LIB_FUNC(players, get_connect_ip)
			LIB_FUNC(players, get_connect_port)
			LIB_FUNC(players, get_lan_ip)
			LIB_FUNC(players, get_lan_port)
			LIB_FUNC(players, are_stats_ready)
			LIB_FUNC(players, get_rank)
			LIB_FUNC(players, get_rp)
			LIB_FUNC(players, get_money)
			LIB_FUNC(players, get_wallet)
			LIB_FUNC(players, get_bank)
			LIB_FUNC(players, get_kd)
			LIB_FUNC(players, get_kills)
			LIB_FUNC(players, get_deaths)
			LIB_FUNC(players, get_language)
			LIB_FUNC(players, is_using_controller)
			LIB_FUNC(players, get_name_with_tags)
			LIB_FUNC(players, get_tags_string)
			LIB_FUNC(players, is_godmode)
			LIB_FUNC(players, is_marked_as_modder)
			LIB_FUNC(players, is_marked_as_modder_or_admin)
			LIB_FUNC(players, is_marked_as_admin)
			LIB_FUNC(players, is_marked_as_attacker)
			LIB_FUNC(players, is_otr)
			LIB_FUNC(players, is_out_of_sight)
			LIB_FUNC(players, is_in_interior)
			LIB_FUNC(players, is_typing)
			LIB_FUNC(players, is_using_vpn)
			LIB_FUNC(players, is_visible)
			LIB_FUNC(players, get_host_token)
			LIB_FUNC(players, get_host_token_hex)
			LIB_FUNC(players, get_host_queue_position)
			LIB_FUNC(players, get_host_queue)
			LIB_FUNC(players, get_boss)
			LIB_FUNC(players, get_org_type)
			LIB_FUNC(players, get_org_colour)
			LIB_FUNC(players, clan_get_motto)
			LIB_FUNC_CUSTOM(get_pos, &lua_players_get_position)
			LIB_FUNC(players, get_position)
			LIB_FUNC(players, is_in_vehicle)
			LIB_FUNC(players, get_vehicle_model)
			LIB_FUNC(players, is_using_rc_vehicle)
			LIB_FUNC(players, get_bounty)
			LIB_FUNC(players, send_sms)
			LIB_FUNC(players, get_cam_pos)
			LIB_FUNC(players, get_cam_rot)
			LIB_FUNC(players, get_spectate_target)
			LIB_FUNC(players, get_waypoint)
			LIB_FUNC(players, get_net_player)
			LIB_FUNC(players, set_wanted_level)
			LIB_FUNC(players, give_pickup_reward)
			LIB_FUNC(players, get_weapon_damage_modifier)
			LIB_FUNC(players, get_melee_weapon_damage_modifier)
			LIB_FUNC(players, add_detection)
			LIB_FUNC(players, on_flow_event_done)
			LIB_FUNC(players, teleport_2d)
			LIB_FUNC(players, teleport_3d)
			LIB_FUNC(players, get_millis_since_discovery)
		)

		luaS_openEntities(L);

		LIB(chat,
			LIB_FUNC(chat, on_message)
			LIB_FUNC(chat, internal_try_lock)
			LIB_FUNC(chat, internal_unlock)
			LIB_FUNC(chat, internal_close_impl)
			LIB_FUNC(chat, internal_open_impl)
			LIB_FUNC(chat, internal_add_to_draft)
			LIB_FUNC(chat, send_message)
			LIB_FUNC(chat, send_targeted_message)
			LIB_FUNC(chat, get_state)
			LIB_FUNC(chat, get_draft)
			LIB_FUNC(chat, remove_from_draft)
			LIB_FUNC(chat, get_history)
		)

		LIB(directx,
			LIB_FUNC(directx, create_texture)
			LIB_FUNC(directx, has_texture_loaded)
			LIB_FUNC(directx, get_texture_dimensions)
			LIB_FUNC(directx, get_texture_character)
			LIB_FUNC(directx, draw_texture_client)
			LIB_FUNC(directx, draw_texture_by_points)
			LIB_FUNC(directx, create_font)
			LIB_FUNC(directx, draw_text)
			LIB_FUNC(directx, draw_text_client)
			LIB_FUNC(directx, draw_rect)
			LIB_FUNC(directx, draw_rect_client)
			LIB_FUNC(directx, draw_line)
			LIB_FUNC(directx, draw_line_client)
			LIB_FUNC(directx, draw_triangle)
			LIB_FUNC(directx, draw_triangle_client)
			LIB_FUNC(directx, draw_circle_square)
			LIB_FUNC(directx, get_client_size)
			LIB_FUNC(directx, get_text_size)
			LIB_FUNC(directx, get_text_size_client)
			LIB_FUNC(directx, pos_hud_to_client)
			LIB_FUNC(directx, size_hud_to_client)
			LIB_FUNC(directx, pos_client_to_hud)
			LIB_FUNC(directx, size_client_to_hud)
			LIB_FUNC(directx, square_to_client)
			LIB_FUNC(directx, client_to_square)
			LIB_FUNC(directx, blurrect_new)
			LIB_FUNC(directx, blurrect_free)
			LIB_FUNC(directx, blurrect_draw)
			LIB_FUNC(directx, blurrect_draw_client)
		)

		luaS_openUtil(L);

		// v3.*
		{
			lua_newtable(L);
			{
				std::string lib_name_dot = "v3.";
				LIB_FUNC(v3, new)
				LIB_FUNC(v3, free)
				LIB_FUNC(v3, get)
				LIB_FUNC(v3, getX)
				LIB_FUNC(v3, getY)
				LIB_FUNC(v3, getZ)
				LIB_FUNC(v3, getHeading)
				LIB_FUNC(v3, set)
				LIB_FUNC(v3, setX)
				LIB_FUNC(v3, setY)
				LIB_FUNC(v3, setZ)
				LIB_FUNC(v3, reset)
				LIB_FUNC(v3, add)
				LIB_FUNC(v3, sub)
				LIB_FUNC(v3, mul)
				LIB_FUNC(v3, div)
				LIB_FUNC(v3, addNew)
				LIB_FUNC(v3, subNew)
				LIB_FUNC(v3, mulNew)
				LIB_FUNC(v3, divNew)
				LIB_FUNC(v3, eq)
				LIB_FUNC(v3, magnitude)
				LIB_FUNC(v3, distance)
				LIB_FUNC(v3, abs)
				LIB_FUNC(v3, min)
				LIB_FUNC(v3, max)
				LIB_FUNC(v3, dot)
				LIB_FUNC(v3, normalise)
				LIB_FUNC(v3, crossProduct)
				LIB_FUNC(v3, toRot)
				LIB_FUNC(v3, lookAt)
				LIB_FUNC(v3, toDir)
				LIB_FUNC(v3, toString)
			}

			lua_newtable(L);
			luaS_push(L, "__call");
			lua_pushcfunction(L, &lua_v3_call);
			lua_settable(L, -3);
			lua_setmetatable(L, -2);

			lua_setglobal(L, "v3");
		}

		LIB(lang,
			LIB_FUNC(lang, get_current)
			LIB_FUNC(lang, is_code_valid)
			LIB_FUNC(lang, get_code_for_soup)
			LIB_FUNC(lang, is_automatically_translated)
			LIB_FUNC(lang, is_english)
			LIB_FUNC(lang, register)
			LIB_FUNC(lang, set_translate)
			LIB_FUNC(lang, translate)
			LIB_FUNC(lang, find)
			LIB_FUNC(lang, find_builtin)
			LIB_FUNC(lang, find_registered)
			LIB_FUNC(lang, get_string)
			LIB_FUNC(lang, is_mine)
			LIB_FUNC(lang, is_builtin)
		)

		luaS_openFilesystem(L);

		LIB(async_http,
			LIB_FUNC(async_http, have_access)
			LIB_FUNC(async_http, init)
			LIB_FUNC(async_http, dispatch)
			LIB_FUNC(async_http, set_post)
			LIB_FUNC(async_http, add_header)
			LIB_FUNC(async_http, set_method)
			LIB_FUNC(async_http, prefer_ipv6)
		)

		luaS_register_memory(L);
		luaS_register_profiling(L);
		luaS_register_native_invoker(L);

		luaS_openSoupLuaBindings(L);

		if (notify)
		{
			Util::toast(LANG_FMT("LUA_RNNG2", std::string((const char*)filename.data(), filename.size())));
			g_gui.on_command_state_change();
		}

		Lang::ensureStandKeys();

		disableBusy();

		int dummy = 0;
		const char* lua_runtime = (
#include "lua_runtime.pluto"
			);
		initial_load = false;
		luaL_loadbuffer(L, lua_runtime, strlen(lua_runtime), "Stand Lua Runtime");
		lua_pcall(L, 0, LUA_MULTRET, 0);

		luaS_push(L, code);
		running = true;
		first_ticks = 100;
		keep_running_implied = false;
		__try
		{
			lua_call(L, 1, 0);
		}
		__EXCEPTIONAL_LUA()
		{
		}
		//ExecCtx::get().tc = TC_SCRIPT_NOYIELD;
		running = false;

		flags &= ~CMDFLAG_NOT_STATE_FOR_CHILDREN;
		setIndicatorType(LISTINDICATOR_OFF);

		unregisterEventHandlers();
		all_loaded.erase(std::find(all_loaded.begin(), all_loaded.end(), this));

		running_tt.refresh();

		stop(true);
	}

	void CommandLuaScript::stop(bool hot)
	{
		hold_up_save = false;

		applyDefaultState(hot);

		dispatchOnStop();
		stop_handlers.clear();

		__try
		{
			lua_close(L);
		}
		__EXCEPTIONAL_LUA()
		{
		}
		L = nullptr;

		clearCommands();
		populateStopped();
		resetCursor();
		processChildrenVisualUpdate();

		{
			std::string leaks{};
			if (LuaConfig::notify_missed_cleanup_blurrect->m_on && !bgblurs.empty())
			{
				StringUtils::list_append(leaks, fmt::format("{} blurrect(s)", bgblurs.size()));
			}
			if (LuaConfig::notify_missed_cleanup_arspinner->m_on && enabled_ar_spinner)
			{
				StringUtils::list_append(leaks, "ar spinner");
			}
			if (LuaConfig::notify_missed_cleanup_graceland->m_on && enabled_graceland)
			{
				StringUtils::list_append(leaks, "graceful landing");
			}
			if (!leaks.empty())
			{
				leaks.insert(0, "Missed cleanup(s): ");
				devNoticeNoCheck(std::move(leaks));
			}
		}

		EXCEPTIONAL_LOCK(g_renderer.extras_mtx)
		bgblurs.freeAll();
		EXCEPTIONAL_UNLOCK(g_renderer.extras_mtx)
		disableArSpinner();
		disableGracefulLanding();

		if (!toast_configs.empty())
		{
			for (auto& tc : toast_configs)
			{
				delete tc;
			}
			toast_configs.clear();
		}

		if (!lang_labels.empty())
		{
			for (lang_t i = LANG_EN; i != LANG_REAL_END; ++i)
			{
				for (const hash_t& key : lang_labels)
				{
					Lang::id_to_map(i)->erase(key);
				}
			}
			lang_labels.clear();
		}

		if (g_gui.canUpdateThemeSetting())
		{
			g_gui.on_command_state_change();
		}

		//ExecCtx::get().tc = TC_SCRIPT_YIELDABLE;
	}

	void CommandLuaScript::applyDefaultState(bool hot)
	{
		__try
		{
			iterating_all_commands = true;
			for (auto& entry : all_commands)
			{
				if (auto cmd = entry.getPointer())
				{
					if (cmd->isPhysical()
						&& cmd->isAttached()
						)
					{
						cmd->as<CommandPhysical>()->applyDefaultState();
					}
				}
			}
			iterating_all_commands = false;
		}
		__EXCEPTIONAL()
		{
		}

		if (hot
			&& !script->isCurrent()
			)
		{
			script->nestedTick();
		}
	}

	void CommandLuaScript::dispatchOnStop()
	{
		luaS_invoke_void_direct(L, stop_handlers);
	}

	void CommandLuaScript::clearCommands()
	{
		// Delay deletion of owned commands because if we're in the middle of recursivelyApplyState, we might be invalidating its iterator.
		// menu.attach_before(menu.ref_by_path("Stand>Settings"), menu.list(menu.shadow_root(), "test", {}, ""))
		FiberPool::queueJob([all_commands{ std::move(this->all_commands) }]
		{
			EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
			for (auto& ref : all_commands)
			{
				if (auto cmd = ref.getPointer())
				{
					if (cmd->parent != nullptr)
					{
						cmd->beGoneRootWriteLocked();
					}
				}
			}
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
		});
		resetChildrenWithPreDelete();
		this->all_commands.clear();
		this->need_state_commands.clear();
	}

	void CommandLuaScript::requestStop(bool silent, bool instant)
	{
		can_continue = false;
		is_silent_stop = silent;
		is_instant_stop = instant;
	}

	void CommandLuaScript::applyCommandStates()
	{
		if (!need_state_commands.empty())
		{
			invoke_no_fiber = true;
			Click click(CLICK_BULK, TC_SCRIPT_NOYIELD);
			for (auto& need_state_command : need_state_commands)
			{
				__try
				{
					auto path = need_state_command->getPathConfig();
					{
						auto entry = g_gui.active_profile.data.find(path);
						if (entry != g_gui.active_profile.data.end())
						{
							need_state_command->setState(click, entry->second);
						}
					}
					{
						auto entry = g_gui.hotkeys.data.find(path);
						if (entry != g_gui.hotkeys.data.end())
						{
							need_state_command->hotkeys = entry->second;
							need_state_command->checkAddToCommandsWithHotkeys();
							need_state_command->processVisualUpdate(false);
							need_state_command->onHotkeysChanged(CLICK_BULK);
						}
					}
				}
				__EXCEPTIONAL()
				{
				}
			}
			invoke_no_fiber = false;
			need_state_commands.clear();
		}
	}

	void CommandLuaScript::preAttachOfOwnedCommand(Command* command)
	{
		if (command->isPhysical())
		{
			preAttachOfOwnedCommand(command->as<CommandPhysical>());
		}
	}

	void CommandLuaScript::preAttachOfOwnedCommand(CommandPhysical* command)
	{
		auto pc = command->resolveParent(COMMAND_LIST_PLAYER)->as<CommandListPlayer>();
		if (pc != nullptr)
		{
			pc->registerCommand(command);
		}
		if (pc == nullptr || !pc->isSingle())
		{
			if (first_ticks)
			{
				need_state_commands.emplace_back(command);
			}
		}
	}

	void CommandLuaScript::preReattachOfOwnedCommand(Command* command)
	{
		if (command->isPhysical())
		{
			preReattachOfOwnedCommand(command->as<CommandPhysical>());
		}
	}

	void CommandLuaScript::preReattachOfOwnedCommand(CommandPhysical* command)
	{
		command->onAttach();
		return preAttachOfOwnedCommand(command);
	}

	void CommandLuaScript::preDetachOfOwnedCommand(Command* cmd)
	{
		if (cmd->isList())
		{
			cmd->as<CommandList>()->evacuateCursor();

			for (const auto& child : cmd->as<CommandList>()->children)
			{
				if (child->isPhysical())
				{
					preDetachOfOwnedCommand(child->as<CommandPhysical>());
				}
			}
		}

		if (first_ticks && cmd->isPhysical())
		{
			auto it = std::find(need_state_commands.begin(), need_state_commands.end(), cmd->as<CommandPhysical>());
			if (it != need_state_commands.end())
			{
				need_state_commands.erase(it);
			}
		}
	}

	void CommandLuaScript::deleteCommand(Command* command, bool keep_in_parent)
	{
		preDetachOfOwnedCommand(command);
		//if (command->isPhysical())
		{
			/*if (!soft && command->parent->type == COMMAND_LIST_PLAYER)
			{
				compactplayer_t player = command->parent->as<CommandListPlayer>()->pp->getPlayers(true).at(0);
				auto& cmds = player_commands.at(player);
				for (auto i = cmds.begin(); i != cmds.end(); )
				{
					if (auto ptr = i->getPointer())
					{
						if (ptr == command)
						{
							cmds.erase(i);
							break;
						}
						++i;
					}
					else
					{
						i = cmds.erase(i);
					}
				}
			}*/
			if (command->isList())
			{
				for (auto& c : command->as<CommandList>()->children)
				{
					deleteCommand(c->as<CommandPhysical>(), true);
				}
				command->as<CommandList>()->children.clear();
			}
		}
		/*for (auto i = all_commands.begin(); i != all_commands.end(); ++i)
		{
			if (i->getPointer() == command)
			{
				all_commands.erase(i);
				break;
			}
		}*/
		if (!keep_in_parent)
		{
			command->beGone();
		}
	}

	static size_t collectGarbageInVector(std::vector<soup::WeakRef<Command>>& cmds)
	{
		size_t accum = 0;
		std::vector<soup::WeakRef<Command>> replacement;
		replacement.reserve(cmds.size());
		for (auto i = cmds.begin(); i != cmds.end(); ++i)
		{
			if (auto ptr = i->getPointer())
			{
				replacement.emplace_back(std::move(*i));
			}
			else
			{
				++accum;
			}
		}
		cmds = std::move(replacement);
		return accum;
	}

	size_t CommandLuaScript::collectGarbage()
	{
		size_t gcd = collectGarbageInVector(all_commands);
		//Util::toast(fmt::format("GC'd {} for {}", gcd, menu_name.getEnglishUtf8()));
		return gcd;
	}

	bool CommandLuaScript::ownsCommand(Command* command) const noexcept
	{
		SOUP_IF_LIKELY (command->hasLuaData())
		{
			return command->getLuaData()->script == this;
		}
		return false;
	}

	// This is faulty since CommandUniqPtr wouldn't be added to need_state_commands
	/*bool CommandLuaScript::wasCreatedThisTick(CommandPhysical* command) const noexcept
	{
		return std::find(need_state_commands.begin(), need_state_commands.end(), command) != need_state_commands.end();
	}*/

	void CommandLuaScript::devNoticeNoCheck(std::string message) const
	{
		const auto rel_path = StringUtils::utf16_to_utf8(getRelativeFilePath());
		if (message.find(rel_path) == std::string::npos)
		{
			message = fmt::format("{}: {}", rel_path, message);
		}
		Util::toast(std::move(message), TOAST_ALL);
	}

	void CommandLuaScript::disableArSpinner()
	{
		if (enabled_ar_spinner)
		{
			enabled_ar_spinner = false;
			g_tb_ar_spinner.disable();
		}
	}

	void CommandLuaScript::disableGracefulLanding()
	{
		if (enabled_graceland)
		{
			enabled_graceland = false;
			--g_hooking.graceful_landing;
		}
	}

	void CommandLuaScript::enableBusy()
	{
		if (busy++ == 0)
		{
			auto cmd = makeChild<CommandDivider>(LOC("GENWAIT"));
			busy_cmd = cmd.get();
			EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
			SOUP_ASSERT(!children.empty());
			children.emplace(children.begin() + 1, std::move(cmd));
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
			if (cmd) // We still own it?
			{
				busy_cmd = nullptr; // Reset soon-to-be-stale pointer to it
			}
			if (m_cursor != 0)
			{
				++m_cursor;
			}
			processChildrenUpdate();
		}
		SOUP_ASSERT(busy != 0); // We just incremented this value, it should not be 0.
	}

	void CommandLuaScript::disableBusy()
	{
		SOUP_ASSERT(busy != 0);
		if (--busy == 0)
		{
			if (busy_cmd)
			{
				busy_cmd->beGone();
			}
		}
	}

	soup::WeakRef<CommandLuaScript> CommandLuaScript::getRunningWeakref()
	{
		return soup::WeakRef<CommandLuaScript>(running_tt, this);
	}

	bool CommandLuaScript::ownsLabel(hash_t key) const
	{
		return lang_labels.contains(key);
	}

	void CommandLuaScript::populateStopped()
	{
		createChild<CommandLuaStart>(std::vector<CommandName>(1, std::move(CommandName(CMDNAME("startlua")).append(StringUtils::simplify(menu_name.getEnglishForCommandName())))));
		createChild<CommandLuaCompatLayer>();
		populateRestrictions();
	}

	void CommandLuaScript::populateRestrictions()
	{
		createChild<CommandDivider>(LOC("RESTRCS"));
		createChild<CommandTogglePointerDuplex>(&restriction_force_silent_start, LOC("FRCSILNTSTRT"), {}, NOLABEL, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
		createChild<CommandTogglePointerDuplex>(&restriction_force_silent_stop, LOC("FRCSILNTSTP"), {}, NOLABEL, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
		createChild<CommandTogglePointerDuplex>(&restriction_disable_internet_access, LOC("DISINACC"), {}, NOLABEL, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
	}

	CommandLuaScript* CommandLuaScript::fromFiber(Script* fiber)
	{
		for (auto& script : all_loaded)
		{
			if (script->script == fiber)
			{
				return script;
			}
		}
		return nullptr;
	}

	int luaS_panic_handler(lua_State* L)
	{
		// Lua would call abort next, so we'll take our error and leave...
		throw std::exception(lua_tostring(L, -1));
	}

	void luaS_openMenu(lua_State* L)
	{
#define ADD_MENU_FUNC(type) LIB_FUNC(menu, type)

		LIB(menu,
			LIB_FUNC(menu, my_root)
			LIB_FUNC(menu, player_root)
			LIB_FUNC(menu, shadow_root)
			LIB_FUNC(menu, ref_by_path)
			LIB_FUNC(menu, ref_by_rel_path)
			LIB_FUNC(menu, ref_by_command_name)
			FOR_EACH_CMDTYPE(ADD_MENU_FUNC)
			LIB_FUNC_CUSTOM(color, &lua_menu_colour)
			LIB_FUNC(menu, internal_create_rainbow_carqdmtrem)
			LIB_FUNC(menu, apply_command_states)
			LIB_FUNC(menu, delete)
			LIB_FUNC(menu, replace)
			LIB_FUNC(menu, detach)
			LIB_FUNC(menu, attach)
			LIB_FUNC(menu, attach_after)
			LIB_FUNC(menu, attach_before)
			LIB_FUNC(menu, is_ref_valid)
			LIB_FUNC(menu, focus)
			LIB_FUNC(menu, is_focused)
			LIB_FUNC(menu, get_applicable_players)
			LIB_FUNC(menu, get_parent)
			LIB_FUNC(menu, get_type)
			LIB_FUNC(menu, get_children)
			LIB_FUNC(menu, list_get_focus)
			LIB_FUNC(menu, list_get_focus_physical)
			LIB_FUNC(menu, collect_garbage)
			LIB_FUNC(menu, is_open)
			LIB_FUNC_CUSTOM(get_pos, &lua_menu_get_position)
			LIB_FUNC(menu, get_position)
			LIB_FUNC_CUSTOM(get_main_view_pos_and_size, &lua_menu_get_main_view_position_and_size)
			LIB_FUNC(menu, get_main_view_position_and_size)
			LIB_FUNC(menu, get_current_menu_list)
			LIB_FUNC(menu, get_current_ui_list)
			LIB_FUNC(menu, get_active_list_cursor_text)
			LIB_FUNC(menu, are_tabs_visible)
			LIB_FUNC(menu, show_command_box)
			LIB_FUNC(menu, show_command_box_click_based)
			LIB_FUNC(menu, trigger_commands)
			LIB_FUNC(menu, trigger_command)
			LIB_FUNC(menu, command_box_is_open)
			LIB_FUNC(menu, command_box_get_dimensions)
			LIB_FUNC(menu, is_in_screenshot_mode)
			LIB_FUNC(menu, on_tick_in_viewport)
			LIB_FUNC(menu, on_focus)
			LIB_FUNC(menu, on_blur)
			LIB_FUNC(menu, remove_handler)
			LIB_FUNC(menu, get_physical)
			LIB_FUNC(menu, get_menu_name)
			LIB_FUNC(menu, get_command_names)
			LIB_FUNC(menu, get_help_text)
			LIB_FUNC(menu, get_perm)
			LIB_FUNC(menu, get_name_for_config)
			LIB_FUNC(menu, get_visible)
			LIB_FUNC(menu, get_value)
			LIB_FUNC(menu, get_min_value)
			LIB_FUNC(menu, get_max_value)
			LIB_FUNC(menu, get_step_size)
			LIB_FUNC(menu, get_precision)
			LIB_FUNC(menu, get_indicator_type)
			LIB_FUNC(menu, get_target)
			LIB_FUNC(menu, get_state)
			LIB_FUNC(menu, get_default_state)
			LIB_FUNC(menu, set_state)
			LIB_FUNC(menu, apply_default_state)
			LIB_FUNC(menu, recursively_apply_default_state)
			LIB_FUNC(menu, save_state_in_memory)
			LIB_FUNC(menu, set_menu_name)
			LIB_FUNC(menu, set_command_names)
			LIB_FUNC(menu, set_help_text)
			LIB_FUNC(menu, set_name_for_config)
			LIB_FUNC(menu, set_visible)
			LIB_FUNC(menu, set_value)
			LIB_FUNC(menu, set_min_value)
			LIB_FUNC(menu, set_max_value)
			LIB_FUNC(menu, set_step_size)
			LIB_FUNC(menu, set_precision)
			LIB_FUNC(menu, set_indicator_type)
			LIB_FUNC(menu, set_target)
			LIB_FUNC(menu, set_list_action_options)
			LIB_FUNC(menu, set_action_slider_options)
			LIB_FUNC(menu, set_textslider_options)
			LIB_FUNC(menu, add_value_replacement)
			LIB_FUNC(menu, set_temporary)
			LIB_FUNC(menu, show_warning)
			LIB_FUNC(menu, get_activation_key_hash)
			LIB_FUNC(menu, get_edition)
			LIB_FUNC(menu, get_version)
		)
	}

	void luaS_openEntities(lua_State* L)
	{
		LIB(entities,
			LIB_FUNC(entities, create_ped)
			LIB_FUNC(entities, create_vehicle)
			LIB_FUNC(entities, create_object)
			LIB_FUNC(entities, get_user_vehicle_as_handle)
			LIB_FUNC(entities, get_user_vehicle_as_pointer)
			LIB_FUNC(entities, get_user_personal_vehicle_as_handle)
			LIB_FUNC(entities, handle_to_pointer)
			LIB_FUNC(entities, has_handle)
			LIB_FUNC(entities, pointer_to_handle)
			LIB_FUNC(entities, any_to_handle)
			LIB_FUNC(entities, get_all_vehicles_as_handles)
			LIB_FUNC(entities, get_all_vehicles_as_pointers)
			LIB_FUNC(entities, get_all_peds_as_handles)
			LIB_FUNC(entities, get_all_peds_as_pointers)
			LIB_FUNC(entities, get_all_objects_as_handles)
			LIB_FUNC(entities, get_all_objects_as_pointers)
			LIB_FUNC(entities, get_all_pickups_as_handles)
			LIB_FUNC(entities, get_all_pickups_as_pointers)
			LIB_FUNC(entities, delete)
			LIB_FUNC(entities, delete_by_handle)
			LIB_FUNC(entities, delete_by_pointer)
			LIB_FUNC(entities, get_model_hash)
			LIB_FUNC(entities, get_model_uhash)
			LIB_FUNC_CUSTOM(get_pos, &lua_entities_get_position)
			LIB_FUNC(entities, get_position)
			LIB_FUNC_CUSTOM(get_rot, &lua_entities_get_rotation)
			LIB_FUNC(entities, get_rotation)
			LIB_FUNC(entities, get_health)
			LIB_FUNC(entities, get_upgrade_value)
			LIB_FUNC(entities, get_upgrade_max_value)
			LIB_FUNC(entities, set_upgrade_value)
			LIB_FUNC(entities, get_current_gear)
			LIB_FUNC(entities, set_current_gear)
			LIB_FUNC(entities, get_next_gear)
			LIB_FUNC(entities, set_next_gear)
			LIB_FUNC(entities, get_rpm)
			LIB_FUNC(entities, set_rpm)
			LIB_FUNC(entities, get_gravity)
			LIB_FUNC(entities, set_gravity)
			LIB_FUNC(entities, set_gravity_multiplier)
			LIB_FUNC(entities, get_boost_charge)
			LIB_FUNC(entities, get_draw_handler)
			LIB_FUNC(entities, vehicle_draw_handler_get_pearlecent_colour)
			LIB_FUNC(entities, vehicle_draw_handler_get_wheel_colour)
			LIB_FUNC(entities, get_vehicle_has_been_owned_by_player)
			LIB_FUNC(entities, get_player_info)
			LIB_FUNC(entities, player_info_get_game_state)
			LIB_FUNC(entities, get_weapon_manager)
			LIB_FUNC(entities, get_head_blend_data)
			LIB_FUNC(entities, get_net_object)
			LIB_FUNC(entities, get_owner)
			LIB_FUNC(entities, set_can_migrate)
			LIB_FUNC(entities, get_can_migrate)
			LIB_FUNC(entities, give_control)
			LIB_FUNC(entities, give_control_by_handle)
			LIB_FUNC(entities, give_control_by_pointer)
			LIB_FUNC(entities, vehicle_get_handling)
			LIB_FUNC(entities, handling_get_subhandling)
			LIB_FUNC(entities, detach_wheel)
			LIB_FUNC(entities, get_bomb_bay_pos)
			LIB_FUNC_CUSTOM(get_bomb_bay_position, &lua_entities_get_bomb_bay_pos)
			LIB_FUNC(entities, is_player_ped)
			LIB_FUNC(entities, is_invulnerable)
		)
	}

	void luaS_openUtil(lua_State* L)
	{
		LIB(util,
			//LIB_FUNC(util, internal_prerun_rantpgscmu)
			LIB_FUNC(util, ensure_package_is_installed)
			LIB_FUNC(util, execute_in_os_thread)
			LIB_FUNC(util, internal_yield_zfvasowkyumq)
			LIB_FUNC(util, internal_is_keep_running_implied_uuqepkb32o)
			LIB_FUNC(util, internal_emit_bad_practice_w7uoni5ptt)
			LIB_FUNC(util, internal_emit_deprecated_hzpyiownhj)
			LIB_FUNC(util, request_stop)
			LIB_FUNC(util, is_os_thread)
			LIB_FUNC(util, can_continue)
			LIB_FUNC(util, set_busy)
			LIB_FUNC(util, on_stop)
			LIB_FUNC(util, dispatch_on_stop)
			LIB_FUNC(util, toast)
			LIB_FUNC(util, log)
			LIB_FUNC(util, draw_debug_text)
			LIB_FUNC(util, draw_centred_text)
			LIB_FUNC_CUSTOM(draw_centered_text, &lua_util_draw_centred_text)
			LIB_FUNC(util, show_corner_help)
			LIB_FUNC(util, replace_corner_help)
			LIB_FUNC(util, joaat)
			LIB_FUNC(util, ujoaat)
			LIB_FUNC(util, reverse_joaat)
			LIB_FUNC(util, is_this_model_a_blimp)
			LIB_FUNC(util, is_this_model_an_object)
			LIB_FUNC(util, is_this_model_a_submarine)
			LIB_FUNC(util, is_this_model_a_submarine_car)
			LIB_FUNC(util, is_this_model_a_trailer)
			LIB_FUNC(util, get_vehicles)
			LIB_FUNC(util, get_objects)
			LIB_FUNC(util, get_weapons)
			LIB_FUNC_CUSTOM(BEGIN_TEXT_COMMAND_DISPLAY_TEXT, &lua_util_begin_text_command_display_text)
			LIB_FUNC_CUSTOM(_BEGIN_TEXT_COMMAND_LINE_COUNT, &lua_util_begin_text_command_line_count)
			LIB_FUNC_CUSTOM(BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED, &lua_util_begin_text_command_is_this_help_message_being_displayed)
			LIB_FUNC_CUSTOM(BEGIN_TEXT_COMMAND_DISPLAY_HELP, &lua_util_begin_text_command_display_help)
			LIB_FUNC_CUSTOM(_BEGIN_TEXT_COMMAND_GET_WIDTH, &lua_util_begin_text_command_get_width)
			LIB_FUNC_CUSTOM(BEGIN_TEXT_COMMAND_THEFEED_POST, &lua_util_begin_text_command_thefeed_post)
			LIB_FUNC(util, get_rp_required_for_rank)
			LIB_FUNC(util, get_session_players_bitflag)
			LIB_FUNC(util, trigger_script_event)
			LIB_FUNC(util, current_time_millis)
			LIB_FUNC(util, current_unix_time_seconds)
			LIB_FUNC(util, remove_handler)
			LIB_FUNC(util, clear_commands_and_native_event_handlers)
			LIB_FUNC(util, is_session_started)
			LIB_FUNC(util, is_session_transition_active)
			LIB_FUNC(util, get_char_slot)
			LIB_FUNC(util, get_ground_z)
			LIB_FUNC(util, spoof_script)
			LIB_FUNC(util, spoof_script_thread)
			LIB_FUNC(util, remove_blip)
			LIB_FUNC(util, arspinner_is_enabled)
			LIB_FUNC(util, arspinner_enable)
			LIB_FUNC(util, arspinner_disable)
			LIB_FUNC(util, graceland_is_enabled)
			LIB_FUNC(util, graceland_enable)
			LIB_FUNC(util, graceland_disable)
			LIB_FUNC(util, is_bigmap_active)
			LIB_FUNC(util, copy_to_clipboard)
			LIB_FUNC(util, get_clipboard_text)
			LIB_FUNC(util, read_colons_and_tabs_file)
			LIB_FUNC(util, write_colons_file)
			LIB_FUNC(util, draw_ar_beacon)
			LIB_FUNC(util, draw_box)
			LIB_FUNC(util, draw_sphere)
			LIB_FUNC(util, request_script_host)
			LIB_FUNC(util, give_script_host)
			LIB_FUNC(util, register_file)
			LIB_FUNC(util, get_label_text)
			LIB_FUNC(util, register_label)
			LIB_FUNC(util, is_key_down)
			LIB_FUNC(util, call_foreign_function)
			LIB_FUNC(util, get_rtti_name)
			LIB_FUNC(util, get_rtti_hierarchy)
			LIB_FUNC(util, set_particle_fx_asset)
			LIB_FUNC(util, blip_handle_to_pointer)
			LIB_FUNC(util, get_blip_display)
			LIB_FUNC(util, teleport_2d)
			LIB_FUNC(util, is_interaction_menu_open)
			LIB_FUNC(util, on_transition_finished)
			LIB_FUNC_CUSTOM(get_closest_hud_color, &lua_util_get_closest_hud_colour)
			LIB_FUNC(util, get_closest_hud_colour)
			LIB_FUNC(util, is_soup_netintel_inited)
			LIB_FUNC(util, on_pad_shake)
			LIB_FUNC(util, open_folder)
			LIB_FUNC(util, set_nullptr_preference)
			LIB_FUNC(util, get_tps)
			LIB_FUNC(util, get_session_code)
			LIB_FUNC(util, stat_get_type)
			LIB_FUNC(util, stat_get_int64)
			LIB_FUNC(util, stat_set_int64)
			LIB_FUNC(util, new_toast_config)
			LIB_FUNC(util, toast_config_get_flags)
			LIB_FUNC(util, get_model_info)
			LIB_FUNC(util, is_valid_file_name)
			LIB_FUNC(util, rgb2hsv)
			LIB_FUNC(util, hsv2rgb)
			LIB_FUNC(util, calculate_luminance)
			LIB_FUNC(util, calculate_contrast)
			LIB_FUNC(util, is_contrast_sufficient)
			LIB_FUNC(util, set_waypoint)
			LIB_FUNC(util, set_cam_quaternion)
			LIB_FUNC(util, get_screen_coord_from_world_coord_no_adjustment)
			LIB_FUNC(util, utf8_to_utf16)
			LIB_FUNC(util, utf16_to_utf8)
			LIB_FUNC(util, get_gps_route)
			LIB_FUNC(util, sc_is_blocked)
			LIB_FUNC(util, sc_block)
			LIB_FUNC(util, sc_unblock)
			LIB_FUNC(util, get_tunable_int)
			LIB_FUNC(util, get_tunable_bool)
			LIB_FUNC_CUSTOM(ui3dscene_set_element_2d_pos, &lua_util_ui3dscene_set_element_2d_position)
			LIB_FUNC(util, ui3dscene_set_element_2d_position)
			LIB_FUNC(util, ui3dscene_set_element_2d_size)
			LIB_FUNC(util, ui3dscene_set_element_background_colour)
			LIB_FUNC_CUSTOM(ui3dscene_set_element_background_color, &lua_util_ui3dscene_set_element_background_colour)
			LIB_FUNC(util, ip_from_string)
			LIB_FUNC(util, ip_to_string)
			LIB_FUNC(util, ip_get_as)
			LIB_FUNC(util, ip_get_location)
		)
	}

	void luaS_openFilesystem(lua_State* L)
	{
		LIB(filesystem,
			LIB_FUNC(filesystem, appdata_dir)
			LIB_FUNC(filesystem, stand_dir)
			LIB_FUNC(filesystem, scripts_dir)
			LIB_FUNC(filesystem, resources_dir)
			LIB_FUNC(filesystem, store_dir)
		)
	}

	void luaS_openSoupLuaBindings(lua_State* L)
	{
		if (!soup::LuaBindings::data_provider)
		{
			soup::LuaBindings::data_provider = soup::make_unique<SoupLuaBindingsDataProvider>();
		}
		soup::LuaBindings::open(L);
	}

	void luaS_warnf(void* ud, const char* msg, int tocont)
	{
		std::string& warn_buf = reinterpret_cast<CommandLuaScript*>(ud)->warn_buf;
		warn_buf.append(msg);
		if (!tocont)
		{
			g_logger.log(std::move(warn_buf));
			warn_buf.clear();
		}
	}

	CommandLuaScript* luaS_thisptr(lua_State* L)
	{
		return reinterpret_cast<CommandLuaScript*>(reinterpret_cast<uintptr_t>(luaS_scriptData(L)) - offsetof(CommandLuaScript, script_data));
	}

	CommandLuaScript* luaS_getThisPtrAndImplyKeepRunning(lua_State* L)
	{
		auto* const thisptr = luaS_thisptr(L);
		thisptr->keep_running_implied = true;
		return thisptr;
	}

	LuaScriptData* luaS_scriptData(lua_State* L)
	{
		return reinterpret_cast<LuaScriptData*>(L->l_G->user_data);
	}

	void luaS_notice(lua_State* L, std::string message)
	{
		if (luaG_addsrcinfo(L, message.c_str()))
		{
			message = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		luaS_thisptr(L)->devNoticeNoCheck(std::move(message));
	}

	void luaS_deprecated(lua_State* L, const char* func_name)
	{
		if (LuaConfig::notify_deprecated->m_on)
		{
			std::string str(func_name);
			str.append(" is deprecated");
			luaS_notice(L, std::move(str));
		}
	}

	void luaS_deprecatedCustom(lua_State* L, const std::string& message)
	{
		if (LuaConfig::notify_deprecated->m_on)
		{
			luaS_notice(L, message);
		}
	}

	float luaS_checkTableNumber(lua_State* L, int i, const char* key)
	{
		lua_pushstring(L, key);
		lua_gettable(L, i);
		if (!lua_isnumber(L, -1))
		{
			luaL_error(L, std::string("Expected parameter ").append(fmt::to_string(i)).append("'s \"").append(key).append("\" value to be a number").c_str());
		}
		auto result = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		return result;
	}

	DirectX::SimpleMath::Color luaS_checkColour(lua_State* L, int& i)
	{
		if (lua_gettop(L) >= i)
		{
			if (lua_istable(L, i))
			{
				DirectX::SimpleMath::Color clr(luaS_checkTableNumber(L, i, "r"), luaS_checkTableNumber(L, i, "g"), luaS_checkTableNumber(L, i, "b"), luaS_checkTableNumber(L, i, "a"));
				SOUP_IF_UNLIKELY (clr.R() < 0.0f || clr.R() > 1.0f
					|| clr.G() < 0.0f || clr.G() > 1.0f
					|| clr.B() < 0.0f || clr.B() > 1.0f
					|| clr.A() < 0.0f || clr.A() > 1.0f
					)
				{
					luaL_argerror(L, i, "value out of range");
				}
				return clr;
			}
			if (lua_gettop(L) >= (i + 3))
			{
				auto r = (float)luaL_checknumber(L, i);
				SOUP_IF_UNLIKELY (r < 0.0f || r > 1.0f)
				{
					luaL_argerror(L, i, "value out of range");
				}
				auto g = (float)luaL_checknumber(L, ++i);
				SOUP_IF_UNLIKELY (g < 0.0f || g > 1.0f)
				{
					luaL_argerror(L, i, "value out of range");
				}
				auto b = (float)luaL_checknumber(L, ++i);
				SOUP_IF_UNLIKELY (b < 0.0f || b > 1.0f)
				{
					luaL_argerror(L, i, "value out of range");
				}
				auto a = (float)luaL_checknumber(L, ++i);
				SOUP_IF_UNLIKELY (a < 0.0f || a > 1.0f)
				{
					luaL_argerror(L, i, "value out of range");
				}
				return DirectX::SimpleMath::Color(r, g, b, a);
			}
		}
		luaL_error(L, std::string("Expected parameter ").append(fmt::to_string(i)).append(" to be a Colour (1 table or 4 numbers)").c_str());
	}

	DirectX::SimpleMath::Color luaS_checkColour(lua_State* L, int&& i)
	{
		return luaS_checkColour(L, i);
	}

	std::unordered_map<std::string, std::string> luaS_check_map_string_string(lua_State* L, int i)
	{
		luaL_checktype(L, i, LUA_TTABLE);
		std::unordered_map<std::string, std::string> map{};
		lua_pushvalue(L, i);
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			lua_pushvalue(L, -2);
			std::string key = luaS_checkStringCastable(L, -1);
			std::string value = luaS_checkStringCastable(L, -2);
			map.emplace(std::move(key), std::move(value));
			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		return map;
	}

	std::vector<std::string> luaS_checkStringArray(lua_State* L, int i)
	{
		luaL_checktype(L, i, LUA_TTABLE);
		std::vector<std::string> arr{};
		lua_pushvalue(L, i);
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			lua_pushvalue(L, -2);
			arr.emplace_back(luaS_checkStringCastable(L, -2));
			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		return arr;
	}

	std::vector<std::pair<long long, std::string>> luaS_checkStringArrayWithIndecies(lua_State* L, int i)
	{
		luaL_checktype(L, i, LUA_TTABLE);
		std::vector<std::pair<long long, std::string>> arr{};
		lua_pushvalue(L, i);
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			lua_pushvalue(L, -2);
			arr.emplace_back(luaL_checkinteger(L, -1), luaS_checkStringCastable(L, -2));
			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		return arr;
	}

	std::vector<std::pair<long long, Label>> luaS_checkLabelArrayWithIndecies(lua_State* L, int i)
	{
		luaL_checktype(L, i, LUA_TTABLE);
		std::vector<std::pair<long long, Label>> arr{};
		lua_pushvalue(L, i);
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			lua_pushvalue(L, -2);
			arr.emplace_back(luaL_checkinteger(L, -1), luaS_checkLabel(L, -2));
			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		return arr;
	}

	[[nodiscard]] static std::vector<CommandTextsliderData> luaS_checkSliderTextDataArray(lua_State* L, int i)
	{
		std::vector<CommandTextsliderData> options{};
		for (auto& opt : luaS_checkLabelArrayWithIndecies(L, i))
		{
			options.emplace_back(CommandTextsliderData{ opt.first, std::move(opt.second) });
		}
		return options;
	}

	std::vector<CommandName> luaS_checkCommandNames(lua_State* L, int i)
	{
		auto arr = luaS_checkStringArray(L, i);
		std::vector<CommandName> command_names{};
		for (std::string& command : arr)
		{
			StringUtils::simplifyCommandName(command);
			if (!command.empty())
			{
				command_names.emplace_back(utf8ToCmdName(command));
			}
		}
		return command_names;
	}

	[[nodiscard]] static CommandListActionItemData luaS_checkListActionItemData(lua_State* L, int i, long long index)
	{
		long long value = index;
		Label menu_name{};
		std::vector<CommandName> command_names{};
		Label help_text{};
		Label category{};

		if (lua_type(L, i) == LUA_TTABLE)
		{
			const auto table_size = lua_rawlen(L, i);
			if (table_size >= 4)
			{
				lua_rawgeti(L, i, 3);
				if (lua_type(L, -1) == LUA_TTABLE)
				{
					command_names = luaS_checkCommandNames(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 1);
					value = luaL_checkinteger(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 2);
					menu_name = luaS_checkLabel(L, -1);
					lua_pop(L, 1);

					/*lua_rawgeti(L, i, 3);
					command_names = luaS_checkCommandNames(L, -1);
					lua_pop(L, 1);*/

					lua_rawgeti(L, i, 4);
					help_text = luaS_checkLabel(L, -1);
					lua_pop(L, 1);

					if (table_size >= 5)
					{
						lua_rawgeti(L, i, 5);
						category = luaS_checkLabel(L, -1);
						lua_pop(L, 1);
					}
				}
				else
				{
					lua_pop(L, 1);

					//luaS_deprecatedCustom(L, "Old style list action item data used. Assuming value = index.");

					lua_rawgeti(L, i, 1);
					menu_name = luaS_checkLabel(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 2);
					command_names = luaS_checkCommandNames(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 3);
					help_text = luaS_checkLabel(L, -1);
					lua_pop(L, 1);
				}
			}
			else if (table_size >= 3)
			{
				lua_rawgeti(L, i, 3);
				if (lua_type(L, -1) == LUA_TTABLE)
				{
					// New: value, menu_name, command_names
					command_names = luaS_checkCommandNames(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 1);
					value = luaL_checkinteger(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 2);
					menu_name = luaS_checkLabel(L, -1);
					lua_pop(L, 1);
				}
				else
				{
					// Old: menu_name, command_names, help_text
					lua_pop(L, 1);

					//luaS_deprecatedCustom(L, "Old style list action item data used. Assuming value = index.");

					lua_rawgeti(L, i, 1);
					menu_name = luaS_checkLabel(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 2);
					command_names = luaS_checkCommandNames(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 3);
					help_text = luaS_checkLabel(L, -1);
					lua_pop(L, 1);
				}
			}
			else if (table_size >= 2)
			{
				lua_rawgeti(L, i, 2);
				if (lua_type(L, -1) != LUA_TTABLE)
				{
					// New: value, menu_name
					lua_pop(L, 1);

					lua_rawgeti(L, i, 1);
					value = luaL_checkinteger(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 2);
					menu_name = luaS_checkLabel(L, -1);
					lua_pop(L, 1);
				}
				else 
				{
					// Old: menu_name, command_names
					command_names = luaS_checkCommandNames(L, -1);
					lua_pop(L, 1);

					//luaS_deprecatedCustom(L, "Old style list action item data used. Assuming value = index.");

					lua_rawgeti(L, i, 1);
					menu_name = luaS_checkLabel(L, -1);
					lua_pop(L, 1);

					lua_rawgeti(L, i, 2);
					command_names = luaS_checkCommandNames(L, -1);
					lua_pop(L, 1);
				}
			}
			else if (table_size >= 1)
			{
				lua_rawgeti(L, i, 1);
				menu_name = luaS_checkLabel(L, -1);
				lua_pop(L, 1);

				//luaS_deprecatedCustom(L, "Old style list action item data used. Assuming value = index.");
			}
			else
			{
				luaL_error(L, "list action item data can't be empty");
			}
		}
		else if (lua_type(L, i) == LUA_TSTRING
			|| lua_type(L, i) == LUA_TNUMBER
			)
		{
			//luaS_deprecatedCustom(L, "Using a Label instead of list action item data is deprecated. Assuming value = index.");
			menu_name = luaS_checkLabel(L, i);
		}
		else
		{
			luaL_typeerror(L, i, "list action item data");
		}

		return CommandListActionItemData(value, std::move(menu_name), std::move(command_names), std::move(help_text), COMMANDPERM_USERONLY, 0, std::move(category));
	}

	struct LuaListActionItemData
	{
		long long index;
		CommandListActionItemData data;
	};

	[[nodiscard]] static std::vector<CommandListActionItemData> luaS_checkListActionItemDataArray(lua_State* L, int i)
	{
		luaL_checktype(L, i, LUA_TTABLE);
		std::vector<LuaListActionItemData> luaoptions{};
		lua_pushvalue(L, i);
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			lua_pushvalue(L, -2);
			const long long index = luaL_checkinteger(L, -1);
			luaoptions.emplace_back(LuaListActionItemData{ index,  luaS_checkListActionItemData(L, -2, index) });
			lua_pop(L, 2);
		}
		lua_pop(L, 1);

		// Sort options
		std::sort(luaoptions.begin(), luaoptions.end(), [](const LuaListActionItemData& a, const LuaListActionItemData& b)
		{
			return a.index < b.index;
		});

		// Convert to native list action item data
		std::vector<CommandListActionItemData> options;
		options.reserve(luaoptions.size());
		for (auto& option : luaoptions)
		{
			options.emplace_back(std::move(option.data));
		}
		return options;
	}

	void luaS_pushfunction(lua_State* L, int ref)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	}

	int luaS_checkFunction(lua_State* L, int i)
	{
		luaL_checktype(L, i, LUA_TFUNCTION);
		return luaS_refFunction(L, i);
	}

	int luaS_checkOptFunction(lua_State* L, int i)
	{
		if (lua_gettop(L) >= i && lua_isfunction(L, i))
		{
			return luaS_refFunction(L, i);
		}
		return LUA_NOREF;
	}

	int luaS_refFunction(lua_State* L, int i)
	{
		lua_pushvalue(L, i);
		auto ref = luaL_ref(L, LUA_REGISTRYINDEX);
		SOUP_ASSUME(ref != LUA_NOREF);
		return ref;
	}

	void luaS_releaseReference(lua_State* L, int ref)
	{
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
	}

	void luaS_releaseReferences(lua_State* L, const std::vector<int>& refs)
	{
		for (const auto& ref : refs)
		{
			luaS_releaseReference(L, ref);
		}
	}

	soup::WeakRef<Command>* luaS_checkCommandRef(lua_State* L, int i)
	{
		soup::LuaBindings::checkTypename(L, i, "CommandRef");
		return reinterpret_cast<soup::WeakRef<Command>*>(lua_touserdata(L, i));
	}

	std::unique_ptr<Command>* luaS_checkCommandUniqPtr(lua_State* L, int i)
	{
		soup::LuaBindings::checkTypename(L, i, "CommandUniqPtr");
		auto pCmd = reinterpret_cast<std::unique_ptr<Command>*>(lua_touserdata(L, i));
		if (!*pCmd)
		{
			luaL_error(L, "CommandUniqPtr does not hold a command");
		}
		return pCmd;
	}

	Command* luaS_checkCommand(lua_State* L, int i, bool can_be_detached)
	{
		Command* cmd;
		if (soup::LuaBindings::isTypename(L, i, "CommandUniqPtr"))
		{
			auto pCmd = reinterpret_cast<std::unique_ptr<Command>*>(lua_touserdata(L, i));
			if (!*pCmd)
			{
				luaL_error(L, "CommandUniqPtr does not hold a command");
			}
			cmd = pCmd->get();
		}
		else
		{
			auto ref = luaS_checkCommandRef(L, i);
			if (!ref->isValid())
			{
				luaL_error(L, "CommandRef is not valid");
			}
			cmd = ref->ptr;
			SOUP_IF_UNLIKELY (cmd == nullptr)
			{
				luaL_error(L, "shadow_root is not valid in this context");
			}
		}
		SOUP_IF_UNLIKELY (!cmd->isAttached())
		{
			if (!can_be_detached)
			{
				luaL_error(L, "Referenced command has been detached");
			}
		}
		return cmd;
	}

	CommandPhysical* luaS_checkCommandPhysical(lua_State* L, int i, bool can_be_detached)
	{
		auto cmd = luaS_checkCommand(L, i, can_be_detached);
		if (!cmd->isPhysical())
		{
			luaL_error(L, "Command is not physical");
		}
		return cmd->as<CommandPhysical>();
	}

#define luaS_checkCommandIssuable(...) static_cast<CommandIssuable*>(luaS_checkCommandPhysical(__VA_ARGS__))

	CommandList* luaS_checkCommandList(lua_State* L, int i, bool can_be_detached)
	{
		auto* command = luaS_checkCommandPhysical(L, i, can_be_detached);
		if (!command->isList())
		{
			luaL_error(L, "Expected referenced command to be a list");
		}
		return command->as<CommandList>();
	}

	CommandList* luaS_checkCommandParent(lua_State* L, CommandLuaScript* thisptr, int i)
	{
		auto ref = luaS_checkCommandRef(L, i);
		if (!ref->isValid())
		{
			luaL_error(L, "CommandRef is not valid");
		}
		if (ref->ptr == nullptr)
		{
			return nullptr;
		}
		auto* list = ref->ptr->as<CommandList>();
		SOUP_IF_UNLIKELY (!list->isAttached())
		{
			luaL_error(L, "Referenced command has been detached");
		}
		SOUP_IF_UNLIKELY (!list->isList())
		{
			luaL_error(L, "Expected referenced command to be a list");
		}
		luaS_checkCommandSuitableAsParent(L, thisptr, list);
		return list;
	}

	CommandList* luaS_checkCommandParentNoShadow(lua_State* L, CommandLuaScript* thisptr, int i)
	{
		auto* list = luaS_checkCommandParent(L, thisptr, i);
		if (list == nullptr)
		{
			luaS_commandNotApplicable(L);
		}
		return list;
	}

	void luaS_checkCommandSuitableAsParent(lua_State* L, CommandLuaScript* thisptr, CommandList* list)
	{
		if (list != thisptr) // not our own root?
		{
			if (list->type == thisptr->type) // same type as CommandLuaScript?
			{
				const auto luas_list = thisptr->parent;
				if (list->parent == luas_list) // a list inside of the "Lua Scripts" list?
				{
					// it's another script's root
					luaL_error(L, "Unsuitable parent");
				}
				//else if (list->hasLuaData())
				//{
				//	if (auto lua_data = list->getLuaData())
				//	{
				//		if (lua_data->script != thisptr) // list belongs to a different script?
				//		{
				//			// that's fine, I guess.
				//		}
				//	}
				//}
			}

			SOUP_IF_UNLIKELY (list->isRoot()
				|| list == g_gui.player_list // PlayerListSort would have UB + Gui has header size hard-coded
				|| list->isT<CommandPlayerClassifier>() // children are expected to be CommandPlayerClassifierDetection
				|| list == PlayerHistory::player_history_command // has strict expectations of what child is what and where
				|| list->type == COMMAND_LIST_REFRESHABLE // folder views like "Saved Places" have strict expectations, too
				)
			{
				luaL_error(L, "Unsuitable parent");
			}

			if (LuaConfig::notify_bad_practice->m_on)
			{
				if (list->isListAction())
				{
					luaS_notice(L, "Adding commands inside of a list_action or list_select is bad practice");
				}
			}
		}
	}

	[[noreturn]] void luaS_commandNotApplicable(lua_State* L)
	{
		luaL_error(L, "Provided command can't be used in this context");
	}

	static void luaS_checkMutable(lua_State* L, CommandLuaScript* thisptr, Command* command)
	{
		SOUP_IF_UNLIKELY (!thisptr->ownsCommand(command))
		{
			luaL_error(L, "Attempt to modify unowned command");
		}
	}

	Command* luaS_checkMutableCommand(lua_State* L, CommandLuaScript* thisptr, int i)
	{
		auto* const command = luaS_checkCommand(L, i);
		luaS_checkMutable(L, thisptr, command);
		return command;
	}

	CommandPhysical* luaS_checkMutableCommandPhysical(lua_State* L, CommandLuaScript* thisptr, int i)
	{
		auto* const command = luaS_checkCommandPhysical(L, i);
		luaS_checkMutable(L, thisptr, command);
		return command;
	}

	CommandPhysical* luaS_checkMutableOrDetachedCommand(lua_State* L, CommandLuaScript* thisptr, int i)
	{
		if (soup::LuaBindings::isTypename(L, i, "CommandUniqPtr"))
		{
			auto pCmd = reinterpret_cast<std::unique_ptr<Command>*>(lua_touserdata(L, i));
			SOUP_IF_UNLIKELY (!*pCmd)
			{
				luaL_error(L, "CommandUniqPtr does not hold a command");
			}
			return pCmd->get()->as<CommandPhysical>();
		}
		return luaS_checkMutableCommandPhysical(L, thisptr, i);
	}

	CommandLuaData* luaS_checkCommandLuaData(lua_State* L, int i)
	{
		auto cmd = luaS_checkCommand(L, i, true);
		SOUP_IF_LIKELY (cmd->hasLuaData())
		{
			SOUP_IF_LIKELY (auto data = cmd->getLuaData())
			{
				return data;
			}
		}
		luaS_commandNotApplicable(L);
	}

#define PROPERTY_INDEX_CASE(prop) case soup::joaat::hash(#prop): return lua_menu_get_ ## prop(L);

	static int lua_CommandAny_index(lua_State* L)
	{
		switch (soup::joaat::hash(luaL_checkstring(L, 2)))
		{
			FOR_EACH_PROPERTY(PROPERTY_INDEX_CASE)

		case soup::joaat::hash("getPhysical"):
			lua_pushcfunction(L, &lua_menu_get_physical);
			return 1;

		case soup::joaat::hash("equals"):
			lua_pushcfunction(L, [](lua_State* L) -> int
			{
				lua_pushboolean(L, luaS_checkCommand(L, 1, true) == luaS_checkCommand(L, 2, true));
				return 1;
			});
			return 1;
		}
		return 0;
	}

#define PROPERTY_NEWINDEX_CASE(prop) case soup::joaat::hash(#prop): return lua_menu_set_ ## prop(L, 3);

	static int lua_CommandAny_newindex(lua_State* L)
	{
		switch (soup::joaat::hash(luaL_checkstring(L, 2)))
		{
			FOR_EACH_PROPERTY(PROPERTY_NEWINDEX_CASE)
		}
		luaL_error(L, "Unknown property");
	}

#define CMDTYPE_INDEX_CASE(type) case soup::joaat::hash(#type): lua_pushcfunction(L, &lua_menu_ ## type); return 1;

	[[nodiscard]] static bool allowed_to_ref_command(Command* cmd)
	{
		if (g_tunables.getBool(TUNAHASH("hide stand user detection from scripts")) && cmd && cmd->parent && cmd->parent->isT<CommandPlayerClassifier>())
		{
			return FlowEvent::can_be_visible_to_script(cmd->as<CommandPlayerClassifierDetection>()->event_type);
		}
		return true;
	}

	void luaS_pushCommandRef(lua_State* L, Command* cmd)
	{
		if (!allowed_to_ref_command(cmd))
		{
			cmd = nullptr;
		}
		luaS_pushCommandRef(L, cmd->getWeakRef());
	}

	void luaS_pushCommandRef(lua_State* L, soup::WeakRef<Command> ref)
	{
		soup::LuaBindings::pushNewAndBeginMtImpl<soup::WeakRef<Command>>(L, "CommandRef", std::move(ref));

		luaS_push(L, "__index");
		lua_pushcfunction(L, [](lua_State* L) -> int
		{
			switch (soup::joaat::hash(luaL_checkstring(L, 2)))
			{
			case soup::joaat::hash("isValid"):
				lua_pushcfunction(L, &lua_menu_is_ref_valid);
				return 1;

			case soup::joaat::hash("refByRelPath"):
				lua_pushcfunction(L, &lua_menu_ref_by_rel_path);
				return 1;
			
			case soup::joaat::hash("delete"):
				lua_pushcfunction(L, &lua_menu_delete);
				return 1;

			case soup::joaat::hash("defaultAndDelete"):
				lua_getglobal(L, "internal_menu_default_and_delete_bqokz2yxr8");
				return 1;

			case soup::joaat::hash("detach"):
				lua_pushcfunction(L, &lua_menu_detach);
				return 1;

			case soup::joaat::hash("attach"):
				lua_pushcfunction(L, &lua_menu_attach);
				return 1;

			case soup::joaat::hash("attachAfter"):
				lua_pushcfunction(L, &lua_menu_attach_after);
				return 1;

			case soup::joaat::hash("attachBefore"):
				lua_pushcfunction(L, &lua_menu_attach_before);
				return 1;

			case soup::joaat::hash("focus"):
				lua_pushcfunction(L, &lua_menu_focus);
				return 1;

			case soup::joaat::hash("isFocused"):
				lua_pushcfunction(L, &lua_menu_is_focused);
				return 1;

			case soup::joaat::hash("getApplicablePlayers"):
				lua_pushcfunction(L, &lua_menu_get_applicable_players);
				return 1;

			case soup::joaat::hash("getParent"):
				lua_pushcfunction(L, &lua_menu_get_parent);
				return 1;

			case soup::joaat::hash("getType"):
				lua_pushcfunction(L, &lua_menu_get_type);
				return 1;

			case soup::joaat::hash("getChildren"):
				lua_pushcfunction(L, &lua_menu_get_children);
				return 1;

			case soup::joaat::hash("getFocus"):
				lua_pushcfunction(L, &lua_menu_list_get_focus);
				return 1;

			case soup::joaat::hash("getFocusPhysical"):
				lua_pushcfunction(L, &lua_menu_list_get_focus_physical);
				return 1;

			case soup::joaat::hash("trigger"):
				lua_pushcfunction(L, &lua_menu_trigger_command);
				return 1;

			case soup::joaat::hash("onTickInViewport"):
				lua_pushcfunction(L, &lua_menu_on_tick_in_viewport);
				return 1;

			case soup::joaat::hash("onFocus"):
				lua_pushcfunction(L, &lua_menu_on_focus);
				return 1;

			case soup::joaat::hash("onBlur"):
				lua_pushcfunction(L, &lua_menu_on_blur);
				return 1;

			case soup::joaat::hash("removeHandler"):
				lua_pushcfunction(L, &lua_menu_remove_handler);
				return 1;

			case soup::joaat::hash("getState"):
				lua_pushcfunction(L, &lua_menu_get_state);
				return 1;

			case soup::joaat::hash("getDefaultState"):
				lua_pushcfunction(L, &lua_menu_get_default_state);
				return 1;

			case soup::joaat::hash("setState"):
				lua_pushcfunction(L, &lua_menu_set_state);
				return 1;

			case soup::joaat::hash("applyDefaultState"):
				lua_pushcfunction(L, &lua_menu_apply_default_state);
				return 1;

			case soup::joaat::hash("recursivelyApplyDefaultState"):
				lua_pushcfunction(L, &lua_menu_recursively_apply_default_state);
				return 1;

			case soup::joaat::hash("saveStateInMemory"): // undocumented
				lua_pushcfunction(L, &lua_menu_save_state_in_memory);
				return 1;

			case soup::joaat::hash("setListActionOptions"):
				lua_pushcfunction(L, &lua_menu_set_list_action_options);
				return 1;

			case soup::joaat::hash("setActionSliderOptions"):
				lua_pushcfunction(L, &lua_menu_set_action_slider_options);
				return 1;

			case soup::joaat::hash("setTextsliderOptions"):
				lua_pushcfunction(L, &lua_menu_set_textslider_options);
				return 1;

			case soup::joaat::hash("addValueReplacement"):
				lua_pushcfunction(L, &lua_menu_add_value_replacement);
				return 1;

			case soup::joaat::hash("setTemporary"):
				lua_pushcfunction(L, &lua_menu_set_temporary);
				return 1;

				FOR_EACH_CMDTYPE(CMDTYPE_INDEX_CASE)

			case soup::joaat::hash("toggle_loop"):
				lua_getglobal(L, "internal_menu_toggle_loop_sgmayqvfwr");
				return 1;

			case soup::joaat::hash("inline_rainbow"):
				lua_getglobal(L, "internal_menu_inline_rainbow_rrzsmxvnda");
				return 1;

			case soup::joaat::hash("color"):
				lua_pushcfunction(L, &lua_menu_colour);
				return 1;
			}
			return lua_CommandAny_index(L);
		});
		lua_settable(L, -3);

		luaS_push(L, "__newindex");
		lua_pushcfunction(L, &lua_CommandAny_newindex);
		lua_settable(L, -3);

		lua_setmetatable(L, -2);
	}

	void luaS_pushCommandUniqPtr(lua_State* L, std::unique_ptr<Command>&& cmd)
	{
		soup::LuaBindings::pushNewAndBeginMtImpl<std::unique_ptr<Command>>(L, "CommandUniqPtr", std::move(cmd));

		luaS_push(L, "__index");
		lua_pushcfunction(L, [](lua_State* L) -> int
		{
			switch (soup::joaat::hash(luaL_checkstring(L, 2)))
			{
			case soup::joaat::hash("ref"):
				lua_pushcfunction(L, [](lua_State* L) -> int
				{
					luaS_pushCommandRef(L, luaS_checkCommand(L, 1, true));
					return 1;
				});
				return 1;
			}
			return lua_CommandAny_index(L);
		});
		lua_settable(L, -3);

		luaS_push(L, "__newindex");
		lua_pushcfunction(L, &lua_CommandAny_newindex);
		lua_settable(L, -3);

		lua_setmetatable(L, -2);
	}

	int luaS_returnnewcommand(lua_State* L, CommandLuaScript* thisptr, Command* command)
	{
		if (command->isPhysical())
		{
			command->as<CommandPhysical>()->flags |= CMDFLAG_FEATURELIST_SKIP;
		}
		SOUP_IF_UNLIKELY (thisptr->iterating_all_commands)
		{
			delete command;
			luaL_error(L, "Cannot create new commands in this context");
		}
		thisptr->all_commands.emplace_back(command->getWeakRef());
		thisptr->keep_running_implied = true;
		if (command->parent == nullptr)
		{
			luaS_pushCommandUniqPtr(L, std::unique_ptr<Command>(command));
		}
		else
		{
			thisptr->preAttachOfOwnedCommand(command);
			luaS_pushCommandRef(L, command);
			EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
			command->parent->emplaceVisible(std::unique_ptr<Command>(command));
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
			command->parent->processChildrenUpdate();
		}
		return 1;
	}

	bool luaS_checkboolean(lua_State* L, int i)
	{
		if (lua_gettop(L) < i)
		{
			luaL_error(L, std::string("Missing boolean parameter ").append(fmt::to_string(i)).c_str());
		}
		if (lua_type(L, i) == LUA_TSTRING) // The semantics here may be confusing since "false" is true
		{
			luaL_error(L, fmt::format("Cannot cast string parameter {} to boolean", i).c_str());
		}
		return lua_toboolean(L, i);
	}

	bool luaS_checkoptboolean(lua_State* L, int i, bool d)
	{
		if (lua_gettop(L) < i)
		{
			return d;
		}
		return luaS_checkboolean(L, i);
	}

	hash_t luaS_checkHash(lua_State* L, int i)
	{
		if (lua_type(L, i) == LUA_TSTRING)
		{
			return rage::atStringHash(lua_tostring(L, i));
		}
		return (hash_t)luaL_checkinteger(L, i);
	}

	std::string luaS_checkStringCastable(lua_State* L, int i)
	{
		if (lua_gettop(L) < i)
		{
			luaL_error(L, std::string("Missing string parameter ").append(fmt::to_string(i)).c_str());
		}
		switch (lua_type(L, i))
		{
		default:
			luaL_error(L, std::string("Expected parameter ").append(fmt::to_string(i)).append(" to be a string or string-castable.").c_str());

		case LUA_TSTRING:
			return pluto_checkstring(L, i);

		case LUA_TNUMBER:
			return lua_tostring(L, i);

		case LUA_TBOOLEAN:
			return lua_toboolean(L, i) ? "true" : "false";
		}
	}

	Label luaS_checkLabel(lua_State* L, int i)
	{
		if (lua_gettop(L) < i)
		{
			luaL_error(L, std::string(soup::ObfusString("Missing Label parameter ").str()).append(fmt::to_string(i)).c_str());
		}
		if (lua_type(L, i) == LUA_TNUMBER)
		{
			auto key = (hash_t)luaL_checkinteger(L, i);
			if (key == 0) // 0 means literal Label but we want to see "string not found" not an empty string
			{
				++key;
			}
			return LOC_RT(key);
		}
		return LIT(luaL_checkstring(L, i));
	}

	void* luaS_checkuserdata(lua_State* L, int i)
	{
		if (lua_type(L, i) != LUA_TUSERDATA)
		{
			luaL_error(L, std::string("Expected parameter ").append(fmt::to_string(i)).append(" to be userdata").c_str());
		}
		return lua_touserdata(L, i);
	}

	void* luaS_checkPointer(lua_State* L, int i)
	{
		if (lua_type(L, i) == LUA_TUSERDATA)
		{
			return lua_touserdata(L, i);
		}
		auto addr = (void*)luaL_checkinteger(L, i);
#if LUA_MEMORY_DEBUG
		for (auto i = allocated_ud.begin(); i != allocated_ud.end(); ++i)
		{
			if ((reinterpret_cast<uintptr_t>(addr) >= reinterpret_cast<uintptr_t>(i->first))
				&& (reinterpret_cast<uintptr_t>(addr) < (reinterpret_cast<uintptr_t>(i->first) + LUA_MEMORY_DEBUG_SIZE))
				)
			{
				auto offset = reinterpret_cast<uintptr_t>(addr) - reinterpret_cast<uintptr_t>(i->first);
				if (offset >= i->second)
				{
					luaL_error(L, "Out-of-bounds access to allocated memory");
				}
				break;
			}
		}
#endif
		return addr;
	}

	bool luaS_canContinue(lua_State* L)
	{
		return luaS_thisptr(L)->can_continue;
	}

	bool luaS_checkSilent(lua_State* L, const char* func)
	{
		if (LuaConfig::enforce_silent_start->value != 0)
		{
			if (luaS_thisptr(L)->isFirstTick())
			{
				lua_getglobal(L, "SCRIPT_SILENT_START");
				const bool silent_start = lua_toboolean(L, -1);
				lua_pop(L, 1);

				if (silent_start)
				{
					if (LuaConfig::enforce_silent_start->value == 2)
					{
						luaS_notice(L, fmt::format("Blocked attempt to call {} in first tick despite SCRIPT_SILENT_START", func).c_str());
					}
					return false;
				}
			}
		}

		if (LuaConfig::enforce_silent_stop->value != 0)
		{
			if (luaS_thisptr(L)->is_silent_stop)
			{
				if (LuaConfig::enforce_silent_stop->value == 2)
				{
					luaS_notice(L, fmt::format("Blocked attempt to call {} despite silent stop", func).c_str());
				}
				return false;
			}
		}
		
		return true;
	}

	void luaS_pushPlayer(lua_State* L, AbstractPlayer p)
	{
		if (p.isValid())
		{
			lua_pushinteger(L, p);
		}
		else
		{
			lua_pushinteger(L, -1);
		}
	}

	bool luaS_isV3Instance(lua_State* L, int i)
	{
		return soup::LuaBindings::isTypename(L, i, "Vector3");
	}

	void luaS_awaitThreadCompletion(lua_State* L, const soup::Thread& t)
	{
		luaS_thisptr(L)->off_thread = true;
		while (t.isRunning())
		{
			if (!luaS_canContinue(L))
			{
				g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("{} can't continue but is waiting for an OS thread, going to have to busyspin.").str()), StringUtils::utf16_to_utf8(luaS_thisptr(L)->getRelativeFilePath())));
				while (t.isRunning());
				break;
			}
			ExecCtx::get().yield();
		}
		luaS_thisptr(L)->off_thread = false;
	}

	void luaS_awaitScript(lua_State* L)
	{
		while (luaS_thisptr(L)->off_thread)
		{
			Script::current()->yield();
		}
	}

	static int lua_v3_index(lua_State* L)
	{
		switch (rage::atStringHash(luaL_checkstring(L, 2)))
		{
		case ATSTRINGHASH("x"):
			luaS_push(L, reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, 1))->x);
			return 1;

		case ATSTRINGHASH("y"):
			luaS_push(L, reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, 1))->y);
			return 1;

		case ATSTRINGHASH("z"):
			luaS_push(L, reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, 1))->z);
			return 1;

		case ATSTRINGHASH("get"): lua_pushcfunction(L, &lua_v3_get); return 1;
		case ATSTRINGHASH("getX"): lua_pushcfunction(L, &lua_v3_getX); return 1;
		case ATSTRINGHASH("getY"): lua_pushcfunction(L, &lua_v3_getY); return 1;
		case ATSTRINGHASH("getZ"): lua_pushcfunction(L, &lua_v3_getZ); return 1;
		case ATSTRINGHASH("getHeading"): lua_pushcfunction(L, &lua_v3_getHeading); return 1;
		case ATSTRINGHASH("set"): lua_pushcfunction(L, &lua_v3_set); return 1;
		case ATSTRINGHASH("setX"): lua_pushcfunction(L, &lua_v3_setX); return 1;
		case ATSTRINGHASH("setY"): lua_pushcfunction(L, &lua_v3_setY); return 1;
		case ATSTRINGHASH("setZ"): lua_pushcfunction(L, &lua_v3_setZ); return 1;
		case ATSTRINGHASH("reset"): lua_pushcfunction(L, &lua_v3_reset); return 1;
		case ATSTRINGHASH("add"): lua_pushcfunction(L, &lua_v3_add); return 1;
		case ATSTRINGHASH("sub"): lua_pushcfunction(L, &lua_v3_sub); return 1;
		case ATSTRINGHASH("mul"): lua_pushcfunction(L, &lua_v3_mul); return 1;
		case ATSTRINGHASH("div"): lua_pushcfunction(L, &lua_v3_div); return 1;
		case ATSTRINGHASH("addNew"): lua_pushcfunction(L, &lua_v3_addNew); return 1;
		case ATSTRINGHASH("subNew"): lua_pushcfunction(L, &lua_v3_subNew); return 1;
		case ATSTRINGHASH("mulNew"): lua_pushcfunction(L, &lua_v3_mulNew); return 1;
		case ATSTRINGHASH("divNew"): lua_pushcfunction(L, &lua_v3_divNew); return 1;
		case ATSTRINGHASH("eq"): lua_pushcfunction(L, &lua_v3_eq); return 1;
		case ATSTRINGHASH("magnitude"): lua_pushcfunction(L, &lua_v3_magnitude); return 1;
		case ATSTRINGHASH("distance"): lua_pushcfunction(L, &lua_v3_distance); return 1;
		case ATSTRINGHASH("abs"): lua_pushcfunction(L, &lua_v3_abs); return 1;
		case ATSTRINGHASH("min"): lua_pushcfunction(L, &lua_v3_min); return 1;
		case ATSTRINGHASH("max"): lua_pushcfunction(L, &lua_v3_max); return 1;
		case ATSTRINGHASH("dot"): lua_pushcfunction(L, &lua_v3_dot); return 1;
		case ATSTRINGHASH("normalise"): lua_pushcfunction(L, &lua_v3_normalise); return 1;
		case ATSTRINGHASH("crossProduct"): lua_pushcfunction(L, &lua_v3_crossProduct); return 1;
		case ATSTRINGHASH("toRot"): lua_pushcfunction(L, &lua_v3_toRot); return 1;
		case ATSTRINGHASH("lookAt"): lua_pushcfunction(L, &lua_v3_lookAt); return 1;
		case ATSTRINGHASH("toDir"): lua_pushcfunction(L, &lua_v3_toDir); return 1;
		case ATSTRINGHASH("toString"): lua_pushcfunction(L, &lua_v3_toString); return 1;
		}
		return 0;
	}

	static int lua_v3_newindex(lua_State* L)
	{
		switch (*luaL_checkstring(L, 2))
		{
		case 'x':
			reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, 1))->x = (float)luaL_checknumber(L, 3);
			break;

		case 'y':
			reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, 1))->y = (float)luaL_checknumber(L, 3);
			break;

		case 'z':
			reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, 1))->z = (float)luaL_checknumber(L, 3);
			break;
		}
		return 0;
	}

	static int lua_mem_add(lua_State* L)
	{
		luaS_push(L, reinterpret_cast<uintptr_t>(lua_touserdata(L, 1)) + luaL_checkinteger(L, 2));
		return 1;
	}

	static int lua_mem_sub(lua_State* L)
	{
		luaS_push(L, reinterpret_cast<uintptr_t>(lua_touserdata(L, 1)) - luaL_checkinteger(L, 2));
		return 1;
	}

	void luaS_push_mem_metafuncs(lua_State* L)
	{
		luaS_push(L, "__add");
		lua_pushcfunction(L, &lua_mem_add);
		lua_settable(L, -3);
		luaS_push(L, "__sub");
		lua_pushcfunction(L, &lua_mem_sub);
		lua_settable(L, -3);
	}

	rage::scrVector3* luaS_newV3(lua_State* L)
	{
		auto ret = soup::LuaBindings::pushNewAndBeginMtImpl<rage::scrVector3>(L, "Vector3");
		luaS_push(L, "__index");
		lua_pushcfunction(L, &lua_v3_index);
		lua_settable(L, -3);
		luaS_push(L, "__newindex");
		lua_pushcfunction(L, &lua_v3_newindex);
		lua_settable(L, -3);
		luaS_push(L, "__len");
		lua_pushcfunction(L, &lua_v3_magnitude);
		lua_settable(L, -3);
		luaS_push_mem_metafuncs(L);
		lua_setmetatable(L, -2);
		return ret;
	}

	int lua_menu_my_root(lua_State* L)
	{
		const auto thisptr = luaS_thisptr(L);
		luaS_pushCommandRef(L, thisptr);
		return 1;
	}

	int lua_menu_player_root(lua_State* L)
	{
		AbstractPlayer ap((Player)luaL_checkinteger(L, 1));
		luaS_pushCommandRef(L, reinterpret_cast<CommandPhysical*>(ap.getCommand())->getWeakRef());
		return 1;
	}

	int lua_menu_shadow_root(lua_State* L)
	{
		luaS_pushCommandRef(L, soup::WeakRef<Command>::makeValidNullptrRef());
		return 1;
	}

	int lua_menu_ref_by_path(lua_State* L)
	{
		std::string path = luaL_checkstring(L, 1);
		if (lua_gettop(L) >= 2)
		{
			std::string provided_path = path;
			Util::upgrade_menu_path(path, (unsigned long)luaL_checkinteger(L, 2));
			if (LuaConfig::notify_ref_by_path_upgrade->m_on && path != provided_path)
			{
				luaS_notice(L, "Path has changed");
			}
		}
		luaS_pushCommandRef(L, g_gui.root_list->resolveCommandConfig(path));
		return 1;
	}

	int lua_menu_ref_by_rel_path(lua_State* L)
	{
		luaS_pushCommandRef(L, luaS_checkCommandList(L, 1)->resolveCommandConfig(luaL_checkstring(L, 2)));
		return 1;
	}

	int lua_menu_ref_by_command_name(lua_State* L)
	{
		CommandName target = utf8ToCmdName(luaL_checkstring(L, 1));
		StringUtils::simplifyCommandName(target);

		std::vector<CommandPhysical*> commands{};
		g_gui.root_list->findCommandsWhereCommandNameStartsWith(commands, target, COMMANDPERM_ALL, 1);
		if (commands.empty())
		{
			luaS_pushCommandRef(L, nullptr);
		}
		else
		{
			luaS_pushCommandRef(L, commands[0]);

			if (LuaConfig::notify_bad_practice->m_on && luaS_thisptr(L)->first_ticks)
			{
				g_logger.log(fmt::format("menu.ref_by_command_name(\"{}\") -> menu.ref_by_path(\"{}\", {})", cmdNameToUtf8(target), commands[0]->getPathConfig(), Util::menu_names_version));
			}
		}
		return 1;
	}

	int lua_menu_list(lua_State* L)
	{
		const int numop = lua_gettop(L);
		auto thisptr = luaS_thisptr(L);
		std::vector<CommandName> command_names{};
		Label help_text{};
		if (numop >= 3)
		{
			command_names = luaS_checkCommandNames(L, 3);
			if (numop >= 4)
			{
				help_text = luaS_checkLabel(L, 4);
			}
		}
		return luaS_returnnewcommand(L, thisptr, new CommandLuaList(
			luaS_checkCommandParent(L, thisptr, 1),
			luaS_checkLabel(L, 2),
			std::move(command_names),
			std::move(help_text),
			thisptr,
			luaS_checkOptFunction(L, 5),
			luaS_checkOptFunction(L, 6),
			luaS_checkOptFunction(L, 7)
		));
		return 0;
	}

	int lua_menu_action(lua_State* L)
	{
		const int args = lua_gettop(L);
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		CommandList* const parent = luaS_checkCommandParent(L, thisptr, 1);
		std::wstring syntax{};
		CommandPerm perm = COMMANDPERM_USERONLY;
		if (args >= 7)
		{
			if (lua_isstring(L, 7))
			{
				const char* str = luaL_checkstring(L, 7);
				if (*str)
				{
					syntax = std::move(LANG_GET_W("CMD").append(L": ").append(StringUtils::utf8_to_utf16(str)));
				}
			}
			if (args >= 8)
			{
				perm = (CommandPerm)luaL_checkinteger(L, 8);
			}
		}
		return luaS_returnnewcommand(L, thisptr, new CommandLuaAction(
			parent,
			luaS_checkLabel(L, 2),
			luaS_checkCommandNames(L, 3),
			luaS_checkLabel(L, 4),
			thisptr,
			luaS_checkFunction(L, 5),
			luaS_checkOptFunction(L, 6),
			std::move(syntax),
			perm
		));
	}

	int lua_menu_toggle(lua_State* L)
	{
		const int args = lua_gettop(L);
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		CommandList* const parent = luaS_checkCommandParent(L, thisptr, 1);
		bool default_on = false;
		CommandPerm perm = COMMANDPERM_USERONLY;
		if (args >= 6)
		{
			default_on = luaS_checkboolean(L, 6);
			if (args >= 7)
			{
				perm = (CommandPerm)luaL_checkinteger(L, 7);
			}
		}
		return luaS_returnnewcommand(L, thisptr, new CommandLuaToggle(
			parent,
			luaS_checkLabel(L, 2),
			luaS_checkCommandNames(L, 3),
			luaS_checkLabel(L, 4),
			thisptr,
			luaS_checkFunction(L, 5),
			default_on,
			perm
		));
	}

	template <typename T>
	int lua_menu_slider_impl(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		CommandList* const parent = luaS_checkCommandParent(L, thisptr, 1);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaSlider<T>(
			parent,
			luaS_checkLabel(L, 2),
			luaS_checkCommandNames(L, 3),
			luaS_checkLabel(L, 4),
			(int)luaL_checkinteger(L, 5),
			(int)luaL_checkinteger(L, 6),
			(int)luaL_checkinteger(L, 7),
			(unsigned int)luaL_checkinteger(L, 8),
			thisptr,
			luaS_checkFunction(L, 9)
		));
	}

	int lua_menu_slider(lua_State* L)
	{
		return lua_menu_slider_impl<CommandSlider>(L);
	}

	int lua_menu_slider_float(lua_State* L)
	{
		return lua_menu_slider_impl<CommandSliderFloat>(L);
	}

	template <typename T>
	int lua_menu_click_slider_impl(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		CommandList* const parent = luaS_checkCommandParent(L, thisptr, 1);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaSliderClick<T>(
			parent,
			luaS_checkLabel(L, 2),
			luaS_checkCommandNames(L, 3),
			luaS_checkLabel(L, 4),
			(int)luaL_checkinteger(L, 5),
			(int)luaL_checkinteger(L, 6),
			(int)luaL_checkinteger(L, 7),
			(int)luaL_checkinteger(L, 8),
			thisptr,
			luaS_checkFunction(L, 9)
		));
	}

	int lua_menu_click_slider(lua_State* L)
	{
		return lua_menu_click_slider_impl<CommandSlider>(L);
	}

	int lua_menu_click_slider_float(lua_State* L)
	{
		return lua_menu_click_slider_impl<CommandSliderFloat>(L);
	}

	int lua_menu_list_select(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		CommandList* const parent = luaS_checkCommandParent(L, thisptr, 1);
		auto options = luaS_checkListActionItemDataArray(L, 5);
		auto default_value = luaL_checkinteger(L, 6);
		bool default_value_is_valid = false;
		for (const auto& option : options)
		{
			if (option.value == default_value)
			{
				default_value_is_valid = true;
				break;
			}
		}
		if (!default_value_is_valid)
		{
			luaL_error(L, "default_value given to list_select does not equal the value of any option");
		}
		return luaS_returnnewcommand(L, thisptr, new CommandLuaListSelect(
			parent,
			luaS_checkLabel(L, 2),
			luaS_checkCommandNames(L, 3),
			luaS_checkLabel(L, 4),
			thisptr,
			std::move(options),
			default_value,
			luaS_checkFunction(L, 7)
		));
	}

	int lua_menu_list_action(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		CommandList* const parent = luaS_checkCommandParent(L, thisptr, 1);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaListAction(
			parent,
			luaS_checkLabel(L, 2),
			luaS_checkCommandNames(L, 3),
			luaS_checkLabel(L, 4),
			thisptr,
			luaS_checkListActionItemDataArray(L, 5),
			luaS_checkFunction(L, 6)
		));
	}

	int lua_menu_text_input(lua_State* L)
	{
		auto command_names = luaS_checkCommandNames(L, 3);
		if (command_names.empty())
		{
			luaL_error(L, "Expected at least one command name");
		}
		std::string default_value{};
		if (lua_gettop(L) >= 6)
		{
			default_value = luaL_checkstring(L, 6);
		}
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		CommandList* const parent = luaS_checkCommandParent(L, thisptr, 1);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaTextInput(
			parent,
			luaS_checkLabel(L, 2),
			std::move(command_names),
			luaS_checkLabel(L, 4),
			thisptr,
			luaS_checkFunction(L, 5),
			std::move(default_value)
		));
	}

	int lua_menu_colour(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		CommandList* const parent = luaS_checkCommandParent(L, thisptr, 1);
		int i = 5;
		auto command_names = luaS_checkCommandNames(L, 3);
		auto colour = luaS_checkColour(L, i);
		auto transparency = luaS_checkboolean(L, ++i);
		auto on_change = luaS_checkFunction(L, ++i);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaColour(
			parent,
			luaS_checkLabel(L, 2),
			std::move(command_names),
			luaS_checkLabel(L, 4),
			std::move(colour),
			std::move(transparency),
			thisptr,
			std::move(on_change)
		));
	}

	int lua_menu_rainbow(lua_State* L)
	{
		auto thisptr = luaS_thisptr(L);
		auto colour = luaS_checkCommandPhysical(L, 1);
		if (colour->type != COMMAND_LIST_COLOUR)
		{
			luaS_commandNotApplicable(L);
		}
		return luaS_returnnewcommand(L, thisptr, new CommandRainbow(
			colour->parent,
			LIT(LANG_FMT("LGBTQ", colour->menu_name.getLocalisedUtf8())),
			colour->as<CommandColourCustom>()->getRainbowCommandNames(),
			colour->as<CommandColourCustom>()
		));
	}

	int lua_menu_internal_create_rainbow_carqdmtrem(lua_State* L)
	{
		auto thisptr = luaS_thisptr(L);
		auto colour = luaS_checkCommandPhysical(L, 1);
		if (colour->type != COMMAND_LIST_COLOUR)
		{
			luaS_commandNotApplicable(L);
		}
		return luaS_returnnewcommand(L, thisptr, new CommandRainbow(
			nullptr,
			LOC("LGBT"),
			colour->as<CommandColourCustom>()->getRainbowCommandNames(),
			colour->as<CommandColourCustom>()
		));
	}

	int lua_menu_divider(lua_State* L)
	{
		auto thisptr = luaS_thisptr(L);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaPhysical<CommandDivider>(luaS_checkCommandParent(L, thisptr, 1), luaS_checkLabel(L, 2), thisptr));
	}

	int lua_menu_readonly(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		std::string value = {};
		if (lua_gettop(L) >= 3)
		{
			value = luaL_checkstring(L, 3);
		}
		return luaS_returnnewcommand(L, thisptr, new CommandLuaPhysical<CommandReadonlyValueCopy>(luaS_checkCommandParent(L, thisptr, 1), luaS_checkLabel(L, 2), thisptr, std::move(value)));
	}

	int lua_menu_readonly_name(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaPhysical<CommandReadonlyName>(luaS_checkCommandParent(L, thisptr, 1), luaS_checkLabel(L, 2), thisptr));
	}

	int lua_menu_hyperlink(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		Label help_text = {};
		if (lua_gettop(L) >= 4)
		{
			help_text = luaS_checkLabel(L, 4);
		}
		soup::Uri uri(luaL_checkstring(L, 3));
		if (uri.scheme != "http" && uri.scheme != "https")
		{
			// file: URI + trigger_command would be bad since it could be used to trigger executables
			luaL_error(L, "menu.hyperlink only accepts http and https links");
		}
		return luaS_returnnewcommand(L, thisptr, new CommandLuaPhysical<CommandReadonlyLink>(luaS_checkCommandParent(L, thisptr, 1), luaS_checkLabel(L, 2), thisptr, uri.toString(), std::move(help_text)));
	}

	int lua_menu_action_slider(lua_State* L)
	{
		luaS_deprecated(L, "menu.action_slider");

		return lua_menu_textslider(L);
	}

	int lua_menu_textslider(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaTextslider(
			luaS_checkCommandParent(L, thisptr, 1),
			luaS_checkLabel(L, 2),
			luaS_checkCommandNames(L, 3),
			luaS_checkLabel(L, 4),
			thisptr,
			luaS_checkSliderTextDataArray(L, 5),
			luaS_checkFunction(L, 6)
		));
	}

	int lua_menu_slider_text(lua_State* L)
	{
		luaS_deprecated(L, "menu.slider_text");

		return lua_menu_textslider_stateful(L);
	}

	int lua_menu_textslider_stateful(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		return luaS_returnnewcommand(L, thisptr, new CommandLuaTextsliderStateful(
			luaS_checkCommandParent(L, thisptr, 1),
			luaS_checkLabel(L, 2),
			luaS_checkCommandNames(L, 3),
			luaS_checkLabel(L, 4),
			thisptr,
			luaS_checkSliderTextDataArray(L, 5),
			luaS_checkFunction(L, 6)
		));
	}

	int lua_menu_player_list_players_shortcut(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto parent = luaS_checkCommandParent(L, thisptr, 1);
		auto pc = parent->resolveParent(COMMAND_LIST_PLAYER)->as<CommandListPlayer>();
		if (pc == nullptr
			|| !pc->isSingle()
			)
		{
			luaL_error(L, "This command type must be within a player's root");
		}
		bool single_only = false;
		if (lua_gettop(L) >= 4)
		{
			single_only = luaS_checkboolean(L, 4);
		}
		return luaS_returnnewcommand(L, thisptr, new CommandLuaPlayerListPlayersShortcut(
			parent,
			thisptr,
			luaS_checkLabel(L, 2),
			luaL_checkstring(L, 3),
			single_only
		));
	}

	static void luaS_checkRecursion(lua_State* L, CommandList* parent, Command* cmd)
	{
		if (parent->hasParent(cmd->isList() ? cmd->as<CommandList>() : cmd->parent))
		{
			luaL_error(L, "Attempt to create recursive tree structure");
		}
	}

	int lua_menu_link(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto parent = luaS_checkCommandParent(L, thisptr, 1);
		auto target = luaS_checkCommandPhysical(L, 2, true);
		if (parent != nullptr) // avoid nullptr deref with shadow root
		{
			luaS_checkRecursion(L, parent, target);
		}
		return luaS_returnnewcommand(L, thisptr, new CommandLua<CommandLink>(
			parent,
			thisptr,
			target,
			lua_gettop(L) >= 3 ? lua_toboolean(L, 3) : false
		));
	}

	int lua_menu_apply_command_states(lua_State* L)
	{
		auto* thisptr = luaS_thisptr(L);
		if (!thisptr->first_ticks)
		{
			luaL_error(L, "menu.apply_command_states called after first 100 ticks");
		}
		thisptr->applyCommandStates();
		return 0;
	}

	int lua_menu_delete(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		const auto tbd = luaS_checkMutableCommand(L, thisptr, 1);
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		thisptr->deleteCommand(tbd);
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
		return 0;
	}

	static void luaS_setParent(lua_State* L, Command* cmd, CommandList* parent)
	{
		luaS_checkRecursion(L, parent, cmd);
		cmd->setParent(parent);
	}

	int lua_menu_replace(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto tbr = luaS_checkMutableCommand(L, thisptr, 1);
		auto pCmd = luaS_checkCommandUniqPtr(L, 2);
		auto cmd = pCmd->get();
		thisptr->preDetachOfOwnedCommand(tbr);
		luaS_setParent(L, cmd, tbr->parent);
		tbr->replace(std::move(*pCmd));
		luaS_pushCommandRef(L, cmd);
		return 1;
	}

	int lua_menu_detach(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto cmd = luaS_checkMutableCommand(L, thisptr, 1);
		thisptr->preDetachOfOwnedCommand(cmd);
		auto pCmd = cmd->detach();
		luaS_pushCommandUniqPtr(L, std::move(pCmd));
		return 1;
	}

	int lua_menu_attach(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto list = luaS_checkCommandParentNoShadow(L, thisptr, 1);
		auto pCmd = luaS_checkCommandUniqPtr(L, 2);
		auto cmd = pCmd->get();
		luaS_setParent(L, cmd, list);
		thisptr->preReattachOfOwnedCommand(cmd);
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		list->emplaceVisible(std::move(*pCmd));
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
		list->processChildrenUpdate();
		luaS_pushCommandRef(L, cmd);
		return 1;
	}

	static int lua_menu_attach_rel(lua_State* L, cursor_t offset)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto anchor = luaS_checkCommand(L, 1);
		if (anchor->isConcealed())
		{
			luaL_error(L, "Invisible command can not be used as an anchor");
		}
		auto list = anchor->parent;
		luaS_checkCommandSuitableAsParent(L, thisptr, list);
		auto pCmd = luaS_checkCommandUniqPtr(L, 2);
		auto cmd = pCmd->get();
		luaS_setParent(L, cmd, list);
		thisptr->preReattachOfOwnedCommand(cmd);
		auto prev_focus = list->getFocus();
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		list->children.emplace(list->children.begin() + anchor->getCursorPositionInParent() + offset, std::move(*pCmd));
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
		list->processChildrenUpdateWithPossibleCursorDisplacement(prev_focus, TC_SCRIPT_NOYIELD);
		luaS_pushCommandRef(L, cmd);
		return 1;
	}

	int lua_menu_attach_after(lua_State* L)
	{
		return lua_menu_attach_rel(L, 1);
	}

	int lua_menu_attach_before(lua_State* L)
	{
		return lua_menu_attach_rel(L, 0);
	}

	int lua_menu_is_ref_valid(lua_State* L)
	{
		lua_pushboolean(L, luaS_checkCommandRef(L, 1)->isValid());
		return 1;
	}

	int lua_menu_focus(lua_State* L)
	{
		luaS_checkCommand(L, 1)->focus(TC_SCRIPT_NOYIELD);
		return 0;
	}

	int lua_menu_is_focused(lua_State* L)
	{
		lua_pushboolean(L, luaS_checkCommandPhysical(L, 1)->isFocused());
		return 1;
	}

	static void pushPlayerList(lua_State* L, const std::vector<AbstractPlayer>& list)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		for (const auto& p : list)
		{
			lua_pushinteger(L, i++);
			lua_pushinteger(L, p);
			lua_settable(L, -3);
		}
	}

	int lua_menu_get_applicable_players(lua_State* L)
	{
		auto pc = luaS_checkCommandPhysical(L, 1)->resolveParent(COMMAND_LIST_PLAYER)->as<CommandListPlayer>();
		if (pc == nullptr)
		{
			luaL_error(L, "Not a player command");
		}
		if (lua_gettop(L) >= 2)
		{
			pushPlayerList(L, pc->pp->getPlayers(luaS_checkboolean(L, 2)));
		}
		else
		{
			pushPlayerList(L, pc->pp->getPlayers());
		}
		return 1;
	}

	int lua_menu_get_parent(lua_State* L)
	{
		luaS_pushCommandRef(L, luaS_checkCommand(L, 1, true)->parent);
		return 1;
	}
	
	int lua_menu_get_type(lua_State* L)
	{
		luaS_push(L, luaS_checkCommand(L, 1, true)->type);
		return 1;
	}

	int lua_menu_get_children(lua_State* L)
	{
		auto list = luaS_checkCommandList(L, 1, true);
		list->onPreScriptedAccess();

		lua_newtable(L);
		lua_Integer i = 1;
		if (g_tunables.getBool(TUNAHASH("hide stand user detection from scripts")) && list->isT<CommandPlayerClassifier>())
		{
			for (const auto& child : list->children)
			{
				if (FlowEvent::can_be_visible_to_script(child->as<CommandPlayerClassifierDetection>()->event_type))
				{
					lua_pushinteger(L, i++);
					luaS_pushCommandRef(L, child.get());
					lua_settable(L, -3);
				}
			}
		}
		else
		{
			for (const auto& child : list->children)
			{
				lua_pushinteger(L, i++);
				luaS_pushCommandRef(L, child.get());
				lua_settable(L, -3);
			}
		}
		return 1;
	}

	int lua_menu_list_get_focus(lua_State* L)
	{
		auto list = luaS_checkCommandList(L, 1, true);
		luaS_pushCommandRef(L, list->getFocus());
		return 1;
	}

	int lua_menu_list_get_focus_physical(lua_State* L)
	{
		auto list = luaS_checkCommandList(L, 1, true);
		luaS_pushCommandRef(L, list->getFocusPhysical());
		return 1;
	}

	int lua_menu_collect_garbage(lua_State* L)
	{
		luaS_push(L, luaS_thisptr(L)->collectGarbage());
		return 1;
	}

	int lua_menu_is_open(lua_State* L)
	{
		lua_pushboolean(L, g_gui.opened);
		return 1;
	}

	int lua_menu_get_position(lua_State* L)
	{
		lua_pushnumber(L, double(g_menu_grid.origin.x) / HUD_WIDTH);
		lua_pushnumber(L, double(g_menu_grid.origin.y) / HUD_HEIGHT);
		return 2;
	}

	int lua_menu_get_main_view_position_and_size(lua_State* L)
	{
		lua_pushnumber(L, double(g_menu_grid.main_view->x) / HUD_WIDTH);
		lua_pushnumber(L, double(g_menu_grid.main_view->y) / HUD_HEIGHT);
		lua_pushnumber(L, double(g_menu_grid.main_view->width) / HUD_WIDTH);
		lua_pushnumber(L, double(g_menu_grid.main_view->height) / HUD_HEIGHT);
		return 4;
	}

	int lua_menu_get_current_menu_list(lua_State* L)
	{
		luaS_pushCommandRef(L, g_gui.getCurrentMenuList());
		return 1;
	}

	int lua_menu_get_current_ui_list(lua_State* L)
	{
		luaS_pushCommandRef(L, g_gui.getCurrentUiList());
		return 1;
	}

	int lua_menu_get_active_list_cursor_text(lua_State* L)
	{
		bool even_when_disabled = lua_gettop(L) > 1 ? lua_toboolean(L, 1) : false;
		if (even_when_disabled || g_gui.show_cursor_pos)
		{
			bool even_when_inappropriate = lua_gettop(L) > 2 ? lua_toboolean(L, 2) : false;
			if (even_when_inappropriate || g_gui.isListVisible())
			{
				luaS_push(L, g_gui.getCurrentUiList()->getCursorText());
				return 1;
			}
		}
		lua_pushliteral(L, "");
		return 1;
	}

	int lua_menu_are_tabs_visible(lua_State* L)
	{
		lua_pushboolean(L, g_menu_grid.getItemByType(GRIDITEM_TABS) != nullptr);
		return 1;
	}

	int lua_menu_show_command_box(lua_State* L)
	{
		g_gui.showCommandBox(lua_gettop(L) >= 1 ? StringUtils::utf8_to_utf16(luaL_checkstring(L, 1)) : std::wstring{});
		return 0;
	}

	int lua_menu_show_command_box_click_based(lua_State* L)
	{
		Click click((ClickType)luaL_checkinteger(L, 1), TC_SCRIPT_YIELDABLE);
		g_gui.showCommandBox(lua_gettop(L) >= 2 ? StringUtils::utf8_to_utf16(luaL_checkstring(L, 2)) : std::wstring{}, click);
		return 0;
	}

	int lua_menu_trigger_commands(lua_State* L)
	{
		Click click(CLICK_SCRIPTED, TC_SCRIPT_NOYIELD);
		lua_pushboolean(L, g_gui.triggerCommands(luaL_checkstring(L, 1), click));
		return 1;
	}

	int lua_menu_trigger_command(lua_State* L)
	{
		Click click(CLICK_SCRIPTED, TC_SCRIPT_NOYIELD);
		auto args = lua_gettop(L) >= 2 ? StringUtils::utf8_to_utf16(luaL_checkstring(L, 2)) : std::wstring{};
		luaS_checkCommandPhysical(L, 1)->onCommand(click, args);
		return 0;
	}

	int lua_menu_command_box_is_open(lua_State* L)
	{
		luaS_push(L, g_commandbox.active);
		return 1;
	}

	int lua_menu_command_box_get_dimensions(lua_State* L)
	{
		int16_t x, y, width, height;
		g_commandbox_grid.getDimensions(x, y, width, height);
		lua_pushnumber(L, double(x) / HUD_WIDTH);
		lua_pushnumber(L, double(y) / HUD_HEIGHT);
		lua_pushnumber(L, double(width) / HUD_WIDTH);
		lua_pushnumber(L, double(height) / HUD_HEIGHT);
		return 4;
	}

	int lua_menu_is_in_screenshot_mode(lua_State* L)
	{
		luaS_push(L, g_tb_screenshot_mode.isEnabled());
		return 1;
	}

	int lua_menu_on_tick_in_viewport(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto* command = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		auto ref = luaS_checkFunction(L, 2);
		if (auto lua_data = command->getLuaData())
		{
			lua_data->on_tick_in_viewport_handlers.emplace_back(ref);
		}
		else
		{
			luaS_commandNotApplicable(L);
		}
		luaS_push(L, ref);
		return 1;
	}

	int lua_menu_on_focus(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto* command = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		auto ref = luaS_checkFunction(L, 2);
		if (auto lua_data = command->getLuaData())
		{
			lua_data->on_focus_handlers.emplace_back(ref);
		}
		else
		{
			luaS_commandNotApplicable(L);
		}
		luaS_push(L, ref);
		return 1;
	}

	int lua_menu_on_blur(lua_State* L)
	{
		CommandLuaScript* const thisptr = luaS_thisptr(L);
		auto* command = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		auto ref = luaS_checkFunction(L, 2);
		if (auto lua_data = command->getLuaData())
		{
			lua_data->on_blur_handlers.emplace_back(ref);
		}
		else
		{
			luaS_commandNotApplicable(L);
		}
		luaS_push(L, ref);
		return 1;
	}

	int lua_menu_remove_handler(lua_State* L)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* command = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		const auto ref = (int)luaL_checkinteger(L, 2);
		if (auto lua_data = command->getLuaData())
		{
			REMOVE_HANDLER_FROM_VEC(lua_data->on_tick_in_viewport_handlers);
			REMOVE_HANDLER_FROM_VEC(lua_data->on_focus_handlers);
			REMOVE_HANDLER_FROM_VEC(lua_data->on_blur_handlers);

			lua_pushboolean(L, false);
			return 1;
		}
		luaS_commandNotApplicable(L);
	}

	int lua_menu_get_physical(lua_State* L)
	{
		auto* cmd = luaS_checkCommand(L, 1, true);
		luaS_pushCommandRef(L, cmd->getPhysical());
		return 1;
	}

	int lua_menu_get_menu_name(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1, true);
		luaS_push(L, cmd->menu_name);
		return 1;
	}

	int lua_menu_get_command_names(lua_State* L)
	{
		auto* cmd = luaS_checkCommandIssuable(L, 1, true);
		luaS_push(L, cmd->command_names);
		return 1;
	}

	int lua_menu_get_help_text(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1, true);
		luaS_push(L, cmd->help_text);
		return 1;
	}

	int lua_menu_get_perm(lua_State* L)
	{
		auto* cmd = luaS_checkCommandIssuable(L, 1, true);
		luaS_push(L, cmd->perm);
		return 1;
	}

	int lua_menu_get_name_for_config(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		luaS_push(L, cmd->getNameForConfig());
		return 1;
	}

	int lua_menu_get_visible(lua_State* L)
	{
		auto* cmd = luaS_checkCommand(L, 1);
		luaS_push(L, !cmd->isConcealed());
		return 1;
	}

	int lua_menu_get_value(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			auto* cmd = luaS_checkCommandPhysical(L, 1);
			cmd->onPreScriptedAccess();
			if (cmd->isT<CommandToggleNoCorrelation>())
			{
				lua_pushboolean(L, cmd->as<CommandToggleNoCorrelation>()->m_on);
			}
			else if (cmd->isT<CommandSlider>())
			{
				lua_pushinteger(L, cmd->as<CommandSlider>()->value);
			}
			else if (cmd->isT<CommandListSelect>())
			{
				lua_pushinteger(L, cmd->as<CommandListSelect>()->value);
			}
			else if (cmd->isTextslider())
			{
				lua_pushinteger(L, cmd->as<CommandTextslider>()->value);
			}
			else if (cmd->isT<CommandInput>())
			{
				luaS_push(L, cmd->as<CommandInput>()->value);
			}
			else if (cmd->isT<CommandReadonlyValue>())
			{
				luaS_push(L, cmd->as<CommandReadonlyValue>()->value);
			}
			else
			{
				if (LuaConfig::notify_get_value_fail->m_on)
				{
					luaS_commandNotApplicable(L);
				}
				lua_pushinteger(L, 0);
			}
			return 1;
		});
	}

	int lua_menu_get_min_value(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		if (!cmd->isT<CommandSlider>())
		{
			luaS_commandNotApplicable(L);
		}
		luaS_push(L, cmd->as<CommandSlider>()->min_value);
		return 1;
	}

	int lua_menu_get_max_value(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		if (!cmd->isT<CommandSlider>())
		{
			luaS_commandNotApplicable(L);
		}
		luaS_push(L, cmd->as<CommandSlider>()->max_value);
		return 1;
	}

	int lua_menu_get_step_size(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		if (!cmd->isT<CommandSlider>())
		{
			luaS_commandNotApplicable(L);
		}
		luaS_push(L, cmd->as<CommandSlider>()->step_size);
		return 1;
	}

	int lua_menu_get_precision(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		if (!cmd->isT<CommandSliderFloat>())
		{
			luaS_commandNotApplicable(L);
		}
		luaS_push(L, cmd->as<CommandSliderFloat>()->precision);
		return 1;
	}

	int lua_menu_get_indicator_type(lua_State* L)
	{
		luaS_push(L, luaS_checkCommandList(L, 1)->indicator_type);
		return 1;
	}

	int lua_menu_get_target(lua_State* L)
	{
		auto* cmd = luaS_checkCommand(L, 1);
		if (!cmd->isT<CommandLink>())
		{
			luaS_commandNotApplicable(L);
		}
		luaS_pushCommandRef(L, cmd->as<CommandLink>()->target);
		return 1;
	}

	int lua_menu_get_state(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		luaS_push(L, cmd->getState());
		return 1;
	}

	int lua_menu_get_default_state(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		luaS_push(L, cmd->getDefaultState());
		return 1;
	}

	int lua_menu_set_state(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		Click click(CLICK_SCRIPTED, TC_SCRIPT_NOYIELD);
		cmd->setState(click, pluto_checkstring(L, 2));
		return 0;
	}

	int lua_menu_apply_default_state(lua_State* L)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		static_assert(TC_APPLYDEFAULTSTATE == TC_SCRIPT_NOYIELD);
		ExecCtx::get().tc = TC_SCRIPT_NOYIELD;
		cmd->applyDefaultState();
		ExecCtx::get().tc = TC_SCRIPT_YIELDABLE;
		return 0;
	}

	int lua_menu_recursively_apply_default_state(lua_State* L)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkCommandParentNoShadow(L, thisptr, 1);
		static_assert(TC_APPLYDEFAULTSTATE == TC_SCRIPT_NOYIELD);
		ExecCtx::get().tc = TC_SCRIPT_NOYIELD;
		cmd->recursivelyApplyDefaultState();
		ExecCtx::get().tc = TC_SCRIPT_YIELDABLE;
		return 0;
	}

	int lua_menu_save_state_in_memory(lua_State* L) // undocumented
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		cmd->saveStateInMemory();
		return 0;
	}

	int lua_menu_set_menu_name(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableOrDetachedCommand(L, thisptr, 1);
		cmd->setMenuName(luaS_checkLabel(L, i));
		return 0;
	}

	int lua_menu_set_command_names(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableOrDetachedCommand(L, thisptr, 1);
		if (!cmd->canBeResolved())
		{
			luaS_commandNotApplicable(L);
		}
		cmd->setCommandNames(luaS_checkCommandNames(L, i));
		return 0;
	}

	int lua_menu_set_help_text(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableOrDetachedCommand(L, thisptr, 1);
		if (!cmd->canBeResolved())
		{
			luaS_commandNotApplicable(L);
		}
		cmd->setHelpText(luaS_checkLabel(L, i));
		return 0;
	}

	int lua_menu_set_name_for_config(lua_State* L, int i)
	{
		auto data = luaS_checkCommandLuaData(L, 1);
		data->name_for_config = luaL_checkstring(L, i);
		return 0;
	}

	int lua_menu_set_visible(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommand(L, thisptr, 1);
		const bool desired = luaS_checkboolean(L, i);
		if (cmd->isConcealed() == desired)
		{
			if (!cmd->parent->isT<CommandListConcealer>())
			{
				luaS_commandNotApplicable(L);
			}
			cmd->parent->as<CommandListConcealer>()->setVisible(cmd, desired);
		}
		return 0;
	}

	int lua_menu_set_value(lua_State* L, int i)
	{
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		Click click(CLICK_SCRIPTED, TC_SCRIPT_NOYIELD);
		if (cmd->isT<CommandToggleNoCorrelation>())
		{
			cmd->as<CommandToggleNoCorrelation>()->setStateBool(click, luaS_checkboolean(L, i));
		}
		else if (cmd->isT<CommandSlider>())
		{
			cmd->as<CommandSlider>()->setValue(click, (int)luaL_checkinteger(L, i)); // triggers onChange
		}
		else if (cmd->isT<CommandListSelect>())
		{
			cmd->as<CommandListSelect>()->setValue(click, luaL_checkinteger(L, i)); // triggers onChange
		}
		else if (cmd->isT<CommandTextslider>())
		{
			luaS_checkMutable(L, luaS_thisptr(L), cmd); // no onChange handler called here, so should only be used on own commands
			cmd->as<CommandTextslider>()->value = luaL_checkinteger(L, i);
			cmd->as<CommandTextslider>()->processValueChange(click);
		}
		else if (cmd->isT<CommandTextsliderStateful>())
		{
			luaS_checkMutable(L, luaS_thisptr(L), cmd); // no onChange handler called here, so should only be used on own commands
			cmd->as<CommandTextsliderStateful>()->value = luaL_checkinteger(L, i);
			cmd->as<CommandTextsliderStateful>()->processValueChange(click);
		}
		else if (cmd->isT<CommandInput>())
		{
			luaS_checkMutable(L, luaS_thisptr(L), cmd); // no onChange handler called here, so should only be used on own commands
			cmd->as<CommandInput>()->setValue(luaL_checkstring(L, i));
		}
		else if (cmd->isT<CommandReadonlyValue>())
		{
			luaS_checkMutable(L, luaS_thisptr(L), cmd); // no onChange handler called here, so should only be used on own commands
			cmd->as<CommandReadonlyValue>()->setValue(luaL_checkstring(L, i));
		}
		else
		{
			luaS_commandNotApplicable(L);
		}
		return 0;
	}

	int lua_menu_set_min_value(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		if (!cmd->isT<CommandSlider>())
		{
			luaS_commandNotApplicable(L);
		}
		cmd->as<CommandSlider>()->setMinValue((int)luaL_checkinteger(L, i));
		return 0;
	}

	int lua_menu_set_max_value(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		if (!cmd->isT<CommandSlider>())
		{
			luaS_commandNotApplicable(L);
		}
		cmd->as<CommandSlider>()->setMaxValue((int)luaL_checkinteger(L, i));
		return 0;
	}

	int lua_menu_set_step_size(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		if (!cmd->isT<CommandSlider>())
		{
			luaS_commandNotApplicable(L);
		}
		cmd->as<CommandSlider>()->setStepSize((int)luaL_checkinteger(L, i));
		return 0;
	}

	int lua_menu_set_precision(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		if (!cmd->isT<CommandSliderFloat>())
		{
			luaS_commandNotApplicable(L);
		}
		auto precision = luaL_checkinteger(L, i);
		if (precision < 1)
		{
			precision = 1;
		}
		else if (precision > 30)
		{
			precision = 30;
		}
		cmd->as<CommandSliderFloat>()->precision = (uint8_t)precision;
		return 0;
	}

	int lua_menu_set_indicator_type(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommand(L, thisptr, 1);
		if (!cmd->isList())
		{
			luaL_error(L, "Expected referenced command to be a list");
		}
		auto indicator_type = (uint8_t)luaL_checkinteger(L, i);
		if (indicator_type >= _LISTINDICATOR_SIZE)
		{
			luaL_argerror(L, 2, "value out of range");
		}
		cmd->as<CommandList>()->setIndicatorType((ListIndicatorType)indicator_type);
		return 0;
	}

	int lua_menu_set_target(lua_State* L, int i)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommand(L, thisptr, 1);
		if (!cmd->isT<CommandLink>())
		{
			luaS_commandNotApplicable(L);
		}
		cmd->as<CommandLink>()->target = luaS_checkCommandPhysical(L, i, true)->getWeakRef();
		return 0;
	}

	int lua_menu_set_list_action_options(lua_State* L)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		if (!cmd->isListAction())
		{
			luaS_commandNotApplicable(L);
		}
		cmd->as<CommandListAction>()->setOptions(luaS_checkListActionItemDataArray(L, 2));
		cmd->as<CommandListAction>()->processVisualUpdate();
		cmd->as<CommandListAction>()->processChildrenUpdate();
		return 0;
	}

	int lua_menu_set_action_slider_options(lua_State* L)
	{
		luaS_deprecated(L, "menu.set_action_slider_options"); // since Stand 100

		return lua_menu_set_textslider_options(L);
	}

	int lua_menu_set_textslider_options(lua_State* L)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		if (!cmd->isTextslider())
		{
			luaS_commandNotApplicable(L);
		}
		cmd->as<CommandTextslider>()->setOptions(luaS_checkSliderTextDataArray(L, 2));
		cmd->as<CommandTextslider>()->processVisualUpdate();
		return 0;
	}

	int lua_menu_add_value_replacement(lua_State* L)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		if (!cmd->isSlider())
		{
			luaS_commandNotApplicable(L);
		}
		std::vector<std::pair<int, std::wstring>>* replacements;
		if (cmd->isT<CommandSliderFloat>())
		{
			replacements = &cmd->as<CommandLuaSliderBase<CommandSliderFloat>>()->replacements;
		}
		else
		{
			replacements = &cmd->as<CommandLuaSliderBase<CommandSlider>>()->replacements;
		}
		replacements->emplace_back((int)luaL_checkinteger(L, 2), StringUtils::utf8_to_utf16(pluto_checkstring(L, 3)));
		return 0;
	}

	int lua_menu_set_temporary(lua_State* L)
	{
		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkMutableCommandPhysical(L, thisptr, 1);
		/*if (!thisptr->wasCreatedThisTick(cmd))
		{
			luaL_error(L, "Command must be marked as temporary on the same tick as its created");
		}*/
		cmd->flags |= CMDFLAG_TEMPORARY;
		return 0;
	}

	int lua_menu_show_warning(lua_State* L)
	{
		if (!luaS_thisptr(L)->can_continue)
		{
			luaL_error(L, "Attempt to show warning while script is stopping");
		}

		auto* const thisptr = luaS_thisptr(L);
		auto* cmd = luaS_checkCommandPhysical(L, 1);
		Click click((ClickType)luaL_checkinteger(L, 2), TC_SCRIPT_YIELDABLE);
		const char* msg = luaL_checkstring(L, 3);
		int proceed_callback = luaS_checkFunction(L, 4);
		int cancel_callback = LUA_NOREF;
		warningflags_t flags = WARNING_COMMAND_BOUND;
		if (lua_gettop(L) >= 5)
		{
			cancel_callback = luaS_checkFunction(L, 5);
			if (lua_gettop(L) >= 6)
			{
				if (luaS_checkboolean(L, 6))
				{
					flags |= WARNING_SKIPPABLE;
				}
			}
		}
		auto weak_ref = thisptr->getRunningWeakref();
		cmd->showWarningA(click, msg, flags, [proceed_callback, cancel_callback, weak_ref](ThreadContext tc) mutable
		{
			Util::ensureYieldableScriptThread(tc, [proceed_callback, cancel_callback, weak_ref{ std::move(weak_ref) }]() mutable
			{
				__try
				{
					if (auto script = weak_ref.getPointer())
					{
						if (cancel_callback != LUA_NOREF)
						{
							luaS_releaseReference(script->L, cancel_callback);
						}
						luaS_invoke_void(script->L, proceed_callback);
						luaS_releaseReference(script->L, proceed_callback);
					}
				}
				__EXCEPTIONAL_LUA()
				{
				}
			});
		}, [proceed_callback, cancel_callback, weak_ref](ThreadContext tc) mutable
		{
			if (cancel_callback != LUA_NOREF)
			{
				Util::ensureYieldableScriptThread(tc, [proceed_callback, cancel_callback, weak_ref{ std::move(weak_ref) }]() mutable
				{
					__try
					{
						if (auto script = weak_ref.getPointer())
						{
							luaS_releaseReference(script->L, proceed_callback);
							luaS_invoke_void(script->L, cancel_callback);
							luaS_releaseReference(script->L, cancel_callback);
						}
					}
					__EXCEPTIONAL_LUA()
					{
					}
				});
			}
		});
		return 0;
	}

	static hash_t get_activation_key_hash_inner()
	{
		if (g_auth.activation_key_to_try.empty())
		{
			return 0;
		}
		return rage::atStringHash(g_auth.activation_key_to_try.substr(0, 6));
	}

	int lua_menu_get_activation_key_hash(lua_State* L)
	{
		luaS_push(L, get_activation_key_hash_inner());
		return 1;
	}

	static __forceinline int get_edition_inner()
	{
		if (g_gui.isRootStateBasicEdition())
		{
			return 1;
		}
		if (g_gui.isRootStateRegularEdition())
		{
			return 2;
		}
		if (g_gui.isRootStateUltimateEdition())
		{
			return 3;
		}
		return 0;
	}

	int lua_menu_get_edition(lua_State* L) // OBFUS!
	{
		luaS_push(L, get_edition_inner());
		return 1;
	}

	int lua_menu_get_version(lua_State* L)
	{
		lua_newtable(L);
		{
			luaS_push(L, STAND_VERSION);
			lua_setfield(L, -2, "version");
		}
		{
			luaS_push(L, STAND_VERSION_TARGET);
			lua_setfield(L, -2, "version_target");
		}
#ifdef STAND_FULL_BRANCH
		{
			luaS_push(L, STAND_FULL_BRANCH);
			lua_setfield(L, -2, "branch");
		}
#endif
		{
			luaS_push(L, soup::ObfusString(STAND_NAME).str());
			lua_setfield(L, -2, "brand");
		}
		{
			luaS_push(L, soup::ObfusString(STAND_NAMEVERSION).str());
			lua_setfield(L, -2, "full");
		}
		{
			luaS_push(L, GTA_VERSION_TARGET);
			lua_setfield(L, -2, "game");
		}
		return 1;
	}

	int lua_players_on_join(lua_State* L)
	{
		const auto thisptr = luaS_getThisPtrAndImplyKeepRunning(L);
		SOUP_IF_UNLIKELY (thisptr->player_join_callbacks.size() >= (size_t)g_tunables.getInt(TUNAHASH("lua player_join_callbacks limit"), 1000))
		{
			luaL_error(L, "This script has reached the limit for this resource");
		}
		const auto ref = luaS_checkFunction(L, 1);
		thisptr->player_join_callbacks.emplace_back(ref);
		lua_pushinteger(L, ref);
		return 1;
	}

	int lua_players_on_leave(lua_State* L)
	{
		const auto thisptr = luaS_getThisPtrAndImplyKeepRunning(L);
		SOUP_IF_UNLIKELY (thisptr->player_leave_callbacks.size() >= (size_t)g_tunables.getInt(TUNAHASH("lua player_leave_callbacks limit"), 1000))
		{
			luaL_error(L, "This script has reached the limit for this resource");
		}
		const auto ref = luaS_checkFunction(L, 1);
		thisptr->player_leave_callbacks.emplace_back(ref);
		lua_pushinteger(L, ref);
		return 1;
	}

	int lua_players_internal_get_join_callbacks(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		const auto thisptr = luaS_thisptr(L);
		for (const auto& cb : thisptr->player_join_callbacks)
		{
			lua_pushinteger(L, i++);
			luaS_pushfunction(L, cb);
			lua_settable(L, -3);
		}
		return 1;
	}

	int lua_players_exists(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).exists());
		return 1;
	}

	int lua_players_user(lua_State* L)
	{
		lua_pushinteger(L, g_player);
		return 1;
	}

	int lua_players_user_ped(lua_State* L)
	{
		lua_pushinteger(L, g_player_ped.getHandle());
		return 1;
	}

	// Common interface for players.list & players.get_host_queue
	[[nodiscard]] static std::vector<AbstractPlayer> getPlayerList(lua_State* L)
	{
		const int args = lua_gettop(L);
		bool include_self = (args >= 1 ? lua_toboolean(L, 1) : true);
		bool include_friends = (args >= 2 ? lua_toboolean(L, 2) : true);
		bool include_strangers = (args >= 3 ? lua_toboolean(L, 3) : true);
		std::vector<AbstractPlayer> list{};
		if (include_strangers)
		{
			list = AbstractPlayer::listExcept(!include_self, !include_friends);
		}
		else
		{
			list = AbstractPlayer::listOnly(include_self, include_friends);
		}
		return list;
	}

	int lua_players_list(lua_State* L)
	{
		pushPlayerList(L, getPlayerList(L));
		return 1;
	}

	int lua_players_list_only(lua_State* L)
	{
		const int args = lua_gettop(L);
		bool include_self = (args >= 1 ? lua_toboolean(L, 1) : false);
		bool include_friends = (args >= 2 ? lua_toboolean(L, 2) : false);
		bool include_crew_members = (args >= 3 ? lua_toboolean(L, 3) : false);
		bool include_org_members = (args >= 4 ? lua_toboolean(L, 4) : false);
		bool include_modders = luaS_checkoptboolean(L, 5, false);
		bool include_likely_modders = luaS_checkoptboolean(L, 6, false);
		pushPlayerList(L, AbstractPlayer::listOnly(include_self, include_friends, include_crew_members, false, include_org_members, include_modders, include_likely_modders));
		return 1;
	}

	int lua_players_list_except(lua_State* L)
	{
		const int args = lua_gettop(L);
		bool exclude_self = (args >= 1 ? lua_toboolean(L, 1) : false);
		bool exclude_friends = (args >= 2 ? lua_toboolean(L, 2) : false);
		bool exclude_crew_members = (args >= 3 ? lua_toboolean(L, 3) : false);
		bool exclude_org_members = (args >= 4 ? lua_toboolean(L, 4) : false);
		bool exclude_modders = luaS_checkoptboolean(L, 5, false);
		bool exclude_likely_modders = luaS_checkoptboolean(L, 6, false);
		pushPlayerList(L, AbstractPlayer::listExcept(exclude_self, exclude_friends, exclude_crew_members, false, exclude_org_members, exclude_modders, exclude_likely_modders));
		return 1;
	}

	int lua_players_list_all_with_excludes(lua_State* L)
	{
		PlayerProviderAll pp;
		bool include_self = false;
		if (lua_gettop(L) >= 1)
		{
			include_self = luaS_checkboolean(L, 1);
		}
		pushPlayerList(L, pp.getPlayers(include_self));
		return 1;
	}

	int lua_players_get_host(lua_State* L)
	{
		lua_pushinteger(L, (int32_t)AbstractPlayer::host);
		return 1;
	}

	int lua_players_get_script_host(lua_State* L)
	{
		lua_pushinteger(L, (int32_t)AbstractPlayer::script_host);
		return 1;
	}

	int lua_players_get_focused(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		for (AbstractPlayer p : g_gui.focused_players)
		{
			lua_pushinteger(L, i++);
			lua_pushinteger(L, p);
			lua_settable(L, -3);
		}
		return 1;
	}

	int lua_players_get_name(lua_State* L)
	{
		if (lua_istrue(L, 2))
		{
			luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getUnoverriddenName());
			return 1;
		}
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getName());
		return 1;
	}

	int lua_players_get_rockstar_id(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getRockstarId());
		return 1;
	}

	int lua_players_get_rockstar_id_2(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getRockstarId());
		return 1;
	}

	int lua_players_get_ip(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getAddress().ip.value);
		return 1;
	}

	int lua_players_get_ip_string(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getAddress().ip.toString<std::string>());
		return 1;
	}

	int lua_players_get_port(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getAddress().port);
		return 1;
	}

	int lua_players_get_connect_ip(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getConnectAddress().ip.value);
		return 1;
	}

	int lua_players_get_connect_port(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getConnectAddress().port);
		return 1;
	}

	int lua_players_get_lan_ip(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getLanAddress().ip.value);
		return 1;
	}

	int lua_players_get_lan_port(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getLanAddress().port);
		return 1;
	}

	int lua_players_are_stats_ready(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).areStatsReady());
		return 1;
	}

	int lua_players_get_rank(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getRank());
		return 1;
	}

	int lua_players_get_rp(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getRp());
		return 1;
	}

	int lua_players_get_money(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getMoney());
		return 1;
	}

	int lua_players_get_wallet(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getWallet());
		return 1;
	}

	int lua_players_get_bank(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getBank());
		return 1;
	}

	int lua_players_get_kd(lua_State* L)
	{
		lua_pushnumber(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getKd());
		return 1;
	}

	int lua_players_get_kills(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getKills());
		return 1;
	}

	int lua_players_get_deaths(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getDeaths());
		return 1;
	}

	int lua_players_get_language(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getLanguage());
		return 1;
	}

	int lua_players_is_using_controller(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).isUsingController());
		return 1;
	}

	int lua_players_get_name_with_tags(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getNameWithFlags());
		return 1;
	}

	int lua_players_get_tags_string(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getFlagsStrings());
		return 1;
	}

	int lua_players_is_godmode(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).hasFlag(FLAG_GOD));
		return 1;
	}

	int lua_players_is_marked_as_modder(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).hasFlag(FLAG_MOD));
		return 1;
	}

	int lua_players_is_marked_as_modder_or_admin(lua_State* L)
	{
#if HAS_MODAMN
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).hasFlag(FLAG_MODAMN));
#else
		lua_pushboolean(L, false);
#endif
		return 1;
	}

	int lua_players_is_marked_as_admin(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).hasFlag(FLAG_AMN));
		return 1;
	}

	int lua_players_is_marked_as_attacker(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).hasFlag(FLAG_ATTACKED));
		return 1;
	}

	int lua_players_is_otr(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).isOtr());
		return 1;
	}

	int lua_players_is_out_of_sight(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).isOutOfSight());
		return 1;
	}

	int lua_players_is_in_interior(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).isInInterior());
		return 1;
	}

	int lua_players_is_typing(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).isTyping());
		return 1;
	}

	int lua_players_is_using_vpn(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).hasFlag(FLAG_VPN));
		return 1;
	}

	int lua_players_is_visible(lua_State* L)
	{
		lua_pushboolean(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).isVisible());
		return 1;
	}

	int lua_players_get_host_token(lua_State* L)
	{
		luaS_push(L, fmt::to_string(AbstractPlayer((Player)luaL_checkinteger(L, 1)).getPeerId().data));
		return 1;
	}

	int lua_players_get_host_token_hex(lua_State* L)
	{
		luaS_push(L, Util::to_padded_hex_string(AbstractPlayer((Player)luaL_checkinteger(L, 1)).getPeerId()));
		return 1;
	}

	int lua_players_get_host_queue_position(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getHostQueuePosition());
		return 1;
	}

	int lua_players_get_host_queue(lua_State* L)
	{
		auto list = getPlayerList(L);
		std::sort(list.begin(), list.end(), &PlayerListSort::host_queue_impl);
		pushPlayerList(L, list);
		return 1;
	}

	int lua_players_get_boss(lua_State* L)
	{
		luaS_pushPlayer(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getBoss());
		return 1;
	}

	int lua_players_get_org_type(lua_State* L)
	{
		lua_pushinteger(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getOrgType());
		return 1;
	}

	int lua_players_get_org_colour(lua_State* L)
	{
		int colour_slot = AbstractPlayer((Player)luaL_checkinteger(L, 1)).getColourSlot();
		if (colour_slot != -1)
		{
			lua_pushinteger(L, ScriptGlobal(GLOBAL_CEO_COLOUR_SLOTS).at(colour_slot, GLOBAL_CEO_COLOUR_SLOTS_SIZE).at(GLOBAL_CEO_COLOUR_SLOT_COLOUR).get<int>());
		}
		else
		{
			lua_pushinteger(L, -1);
		}
		return 1;
	}

	int lua_players_clan_get_motto(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getActiveClanMembership()->clan.motto);
		return 1;
	}

	int lua_players_get_position(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getPos());
		return 1;
	}

	int lua_players_is_in_vehicle(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).isInVehicle());
		return 1;
	}

	int lua_players_get_vehicle_model(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getCurrentVehicleModel());
		return 1;
	}

	int lua_players_is_using_rc_vehicle(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).isUsingRcVehicle());
		return 1;
	}

	int lua_players_get_bounty(lua_State* L)
	{
		auto bounty = AbstractPlayer((Player)luaL_checkinteger(L, 1)).getBounty();
		if (bounty.has_value())
		{
			luaS_push(L, bounty.value());
			return 1;
		}
		return 0;
	}

	int lua_players_send_sms(lua_State* L)
	{
		AbstractPlayer((Player)luaL_checkinteger(L, 1)).sendSms(luaL_checkstring(L, 2));
		return 0;
	}

	int lua_players_get_cam_pos(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getCamPos());
		return 1;
	}

	int lua_players_get_cam_rot(lua_State* L)
	{
		luaS_push(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getCamRot());
		return 1;
	}

	int lua_players_get_spectate_target(lua_State* L)
	{
		luaS_pushPlayer(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getSpectateTarget());
		return 1;
	}

	int lua_players_get_waypoint(lua_State* L)
	{
		Vector2Plus pos{};
		AbstractPlayer((Player)luaL_checkinteger(L, 1)).getWaypoint().consume(pos);
		luaS_push(L, pos.x);
		luaS_push(L, pos.y);
		luaS_push(L, pos.z);
		luaS_push(L, pos.z_is_guessed);
		return 4;
	}

	int lua_players_get_net_player(lua_State* L)
	{
		luaS_push(L, (lua_Integer)AbstractPlayer((Player)luaL_checkinteger(L, 1)).getCNetGamePlayer());
		return 1;
	}

	int lua_players_set_wanted_level(lua_State* L)
	{
		AbstractPlayer((Player)luaL_checkinteger(L, 1)).setWantedLevel((int32_t)luaL_checkinteger(L, 2));
		return 0;
	}

	int lua_players_give_pickup_reward(lua_State* L)
	{
		AbstractPlayer((Player)luaL_checkinteger(L, 1)).givePickupReward(luaS_checkHash(L, 2));
		return 0;
	}

	int lua_players_get_weapon_damage_modifier(lua_State* L)
	{
		lua_pushnumber(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getWeaponDamageModifier());
		return 1;
	}

	int lua_players_get_melee_weapon_damage_modifier(lua_State* L)
	{
		lua_pushnumber(L, AbstractPlayer((Player)luaL_checkinteger(L, 1)).getMeleeWeaponDamageModifier());
		return 1;
	}

	int lua_players_add_detection(lua_State* L)
	{
		CommandPlayerClassifierDetection* det = nullptr;

		auto p = AbstractPlayer((Player)luaL_checkinteger(L, 1));
		if (auto cmd = p.getCommand())
		{
			toast_t toast_flags = TOAST_DEFAULT;
			uint8_t severity = 100;
			if (lua_gettop(L) >= 3)
			{
				toast_flags = (toast_t)luaL_checkinteger(L, 3);
				if (lua_gettop(L) >= 4)
				{
					severity = (uint8_t)luaL_checkinteger(L, 4);
					if (severity > 100)
					{
						luaL_argerror(L, 4, "value out of range");
					}
				}
			}

			if (auto fe = cmd->getAvailableCustomFlowEvent(); fe != FlowEvent::_CUSTOM_END)
			{
				FlowEvent::custom_event_names[fe - FlowEvent::_CUSTOM_BEGIN] = luaS_checkLabel(L, 2);
				g_hooking.flow_event_reactions[fe].setReactions(toast_flags_to_flow_event_reactions(toast_flags));

				cmd->getAndApplyReactions(p, fe, severity);

				det = cmd->getDetectionsList()->getDetection(fe);
				det->setHelpText(LIT(LANG_FMT("MDDRDT_H_LUA", luaS_thisptr(L)->menu_name.getLiteralUtf8())));
			}
		}

		luaS_pushCommandRef(L, static_cast<Command*>(det));
		return 1;
	}

	int lua_players_on_flow_event_done(lua_State* L)
	{
		const auto thisptr = luaS_getThisPtrAndImplyKeepRunning(L);
		SOUP_IF_UNLIKELY (thisptr->flow_event_done_callbacks.size() >= (size_t)g_tunables.getInt(TUNAHASH("lua flow_event_done_callbacks limit"), 1000))
		{
			luaL_error(L, "This script has reached the limit for this resource");
		}
		const auto ref = luaS_checkFunction(L, 1);
		thisptr->flow_event_done_callbacks.emplace_back(ref);
		lua_pushinteger(L, ref);
		return 1;
	}

	static void teleport_player(AbstractPlayer p, Vector2Plus coords)
	{
		if (p == g_player)
		{
			TpUtil::teleport(coords);
		}
		else
		{
			FiberPool::queueJob([p{ std::move(p) }, coords{ std::move(coords) }]
			{
				p.fiberTeleport(coords);
			});
		}
	}

	int lua_players_teleport_2d(lua_State* L)
	{
		teleport_player((Player)luaL_checkinteger(L, 1), { (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3) });
		return 0;
	}

	int lua_players_teleport_3d(lua_State* L)
	{
		teleport_player((Player)luaL_checkinteger(L, 1), { (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4) });
		return 0;
	}

	int lua_players_get_millis_since_discovery(lua_State* L)
	{
		if (auto cmd = AbstractPlayer((Player)luaL_checkinteger(L, 1)).getCommand())
		{
			luaS_push(L, GET_MILLIS_SINCE(cmd->discovery));
			return 1;
		}
		return 0;
	}

	int lua_entities_create_ped(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, Util::createPed((int)luaL_checkinteger(L, 1), (Hash)luaL_checkinteger(L, 2), luaS_checkV3<v3>(L, 3), (float)luaL_checknumber(L, 4)).getHandle());
			return 1;
		});
	}

	int lua_entities_create_vehicle(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, Util::createVehicle((Hash)luaL_checkinteger(L, 1), luaS_checkV3<v3>(L, 2), (float)luaL_checknumber(L, 3)).getHandle());
			return 1;
		});
	}

	int lua_entities_create_object(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, Util::createObject((Hash)luaL_checkinteger(L, 1), luaS_checkV3<v3>(L, 2)).getHandle());
			return 1;
		});
	}

	int lua_entities_get_user_vehicle_as_handle(lua_State* L)
	{
		bool include_last_driven = true;
		if (lua_gettop(L) >= 1)
		{
			include_last_driven = luaS_checkboolean(L, 1);
		}
		luaS_push(L, Util::getVehicle(include_last_driven).getHandle());
		return 1;
	}

	int lua_entities_get_user_vehicle_as_pointer(lua_State* L)
	{
		bool include_last_driven = true;
		if (lua_gettop(L) >= 1)
		{
			include_last_driven = luaS_checkboolean(L, 1);
		}
		return luaS_returnPtrNilable(L, Util::getVehicle(include_last_driven).getPointer());
	}

	int lua_entities_get_user_personal_vehicle_as_handle(lua_State* L)
	{
		luaS_push(L, AbstractEntity::getPersonalVehicle().getHandle());
		return 1;
	}

	int lua_entities_handle_to_pointer(lua_State* L)
	{
		return luaS_returnPtrNilable(L, AbstractEntity::get((Entity)luaL_checkinteger(L, 1)).getPointer());
	}

	int lua_entities_has_handle(lua_State* L)
	{
		luaS_push(L, AbstractEntity::get((CPhysical*)luaL_checkinteger(L, 1)).hasHandle());
		return 1;
	}

	int lua_entities_pointer_to_handle(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::get((CPhysical*)luaL_checkinteger(L, 1)).getHandle());
			return 1;
		});
	}

	int lua_entities_any_to_handle(lua_State* L) // undocumented
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getHandle());
			return 1;
		});
	}

	int lua_entities_get_all_vehicles_as_handles(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		AbstractEntity::getAllVehicles([&](AbstractEntity&& ent)
		{
			luaS_push(L, i++);
			luaS_push(L, ent.getHandle());
			lua_settable(L, -3);
			CONSUMER_CONTINUE;
		});
		return 1;
	}

	int lua_entities_get_all_vehicles_as_pointers(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		AbstractEntity::getAllVehicles([&](AbstractEntity&& ent)
		{
			luaS_push(L, i++);
			luaS_push(L, ent.getPointer());
			lua_settable(L, -3);
			CONSUMER_CONTINUE;
		});
		return 1;
	}

	int lua_entities_get_all_peds_as_handles(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		AbstractEntity::getAllPeds([&](AbstractEntity&& ent)
		{
			luaS_push(L, i++);
			luaS_push(L, ent.getHandle());
			lua_settable(L, -3);
			CONSUMER_CONTINUE;
		});
		return 1;
	}

	int lua_entities_get_all_peds_as_pointers(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		AbstractEntity::getAllPeds([&](AbstractEntity&& ent)
		{
			luaS_push(L, i++);
			luaS_push(L, ent.getPointer());
			lua_settable(L, -3);
			CONSUMER_CONTINUE;
		});
		return 1;
	}

	int lua_entities_get_all_objects_as_handles(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		AbstractEntity::getAllObjects([&](AbstractEntity&& ent)
		{
			luaS_push(L, i++);
			luaS_push(L, ent.getHandle());
			lua_settable(L, -3);
			CONSUMER_CONTINUE;
		});
		return 1;
	}

	int lua_entities_get_all_objects_as_pointers(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		AbstractEntity::getAllObjects([&](AbstractEntity&& ent)
		{
			luaS_push(L, i++);
			luaS_push(L, ent.getPointer());
			lua_settable(L, -3);
			CONSUMER_CONTINUE;
		});
		return 1;
	}

	int lua_entities_get_all_pickups_as_handles(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		AbstractEntity::getAllPickups([&](AbstractEntity&& ent)
		{
			luaS_push(L, i++);
			luaS_push(L, ent.getHandle());
			lua_settable(L, -3);
			CONSUMER_CONTINUE;
		});
		return 1;
	}

	int lua_entities_get_all_pickups_as_pointers(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		AbstractEntity::getAllPickups([&](AbstractEntity&& ent)
		{
			luaS_push(L, i++);
			luaS_push(L, ent.getPointer());
			lua_settable(L, -3);
			CONSUMER_CONTINUE;
		});
		return 1;
	}

	int lua_entities_delete(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::getSmart(luaL_checkinteger(L, 1)).removeFromPlaneOfExistence();
			return 0;
		});
	}

	int lua_entities_delete_by_handle(lua_State* L) // undocumented as of 105.2
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::get((Entity)luaL_checkinteger(L, 1)).removeFromPlaneOfExistence();
			return 0;
		});
	}

	int lua_entities_delete_by_pointer(lua_State* L) // undocumented as of 105.2
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::get((CPhysical*)luaL_checkinteger(L, 1)).removeFromPlaneOfExistence();
			return 0;
		});
	}

	int lua_entities_get_model_hash(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getModelHash());
			return 1;
		});
	}

	int lua_entities_get_model_uhash(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, (uhash_t)AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getModelHash());
			return 1;
		});
	}

	int lua_entities_get_position(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CPhysical*)luaL_checkinteger(L, 1))->matrix.getTranslate());
			return 1;
		});
	}

	int lua_entities_get_rotation(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CPhysical*)luaL_checkinteger(L, 1))->matrix.getRotationXYZ());
			return 1;
		});
	}

	int lua_entities_get_health(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getHealth());
			return 1;
		});
	}

	int lua_entities_get_upgrade_value(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getUpgradeValue((int)luaL_checkinteger(L, 2)));
			return 1;
		});
	}

	int lua_entities_get_upgrade_max_value(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getUpgradeMaxValue((int)luaL_checkinteger(L, 2)));
			return 1;
		});
	}

	int lua_entities_set_upgrade_value(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::getSmart(luaL_checkinteger(L, 1)).applyUpgrade((int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3));
			return 0;
		});
	}

	int lua_entities_get_current_gear(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CVehicle*)luaL_checkinteger(L, 1))->m_Transmission.m_nPrevGear);
			return 1;
		});
	}

	int lua_entities_set_current_gear(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			((CVehicle*)luaL_checkinteger(L, 1))->m_Transmission.m_nPrevGear = (uint8_t)luaL_checkinteger(L, 2);
			return 0;
		});
	}

	int lua_entities_get_next_gear(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CVehicle*)luaL_checkinteger(L, 1))->m_Transmission.m_nGear);
			return 1;
		});
	}

	int lua_entities_set_next_gear(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			((CVehicle*)luaL_checkinteger(L, 1))->m_Transmission.m_nGear = (uint8_t)luaL_checkinteger(L, 2);
			return 0;
		});
	}

	int lua_entities_get_rpm(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CVehicle*)luaL_checkinteger(L, 1))->m_Transmission.rpm);
			return 1;
		});
	}

	int lua_entities_set_rpm(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			((CVehicle*)luaL_checkinteger(L, 1))->m_Transmission.rpm = (float)luaL_checknumber(L, 2);
			return 0;
		});
	}

	int lua_entities_get_gravity(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CVehicle*)luaL_checkinteger(L, 1))->m_fGravityForWheelIntegrator);
			return 1;
		});
	}

	int lua_entities_set_gravity(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			((CVehicle*)luaL_checkinteger(L, 1))->m_fGravityForWheelIntegrator = (float)luaL_checknumber(L, 2);
			return 0;
		});
	}

	int lua_entities_set_gravity_multiplier(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			pointers::CVehicle_SetGravityForWheellIntegrator(((CVehicle*)luaL_checkinteger(L, 1)), (float)luaL_checknumber(L, 2));
			return 0;
		});
	}

	int lua_entities_get_boost_charge(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CVehicle*)luaL_checkinteger(L, 1))->boost_charge);
			return 1;
		});
	}

	int lua_entities_get_draw_handler(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((rage::fwEntity*)luaL_checkinteger(L, 1))->draw_handler);
			return 1;
		});
	}

	int lua_entities_vehicle_draw_handler_get_pearlecent_colour(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CVehicleDrawHandler*)luaL_checkinteger(L, 1))->m_pearlescent);
			return 1;
		});
	}

	int lua_entities_vehicle_draw_handler_get_wheel_colour(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CVehicleDrawHandler*)luaL_checkinteger(L, 1))->m_wheel_color);
			return 1;
		});
	}

	int lua_entities_get_vehicle_has_been_owned_by_player(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushboolean(L, ((CVehicle*)luaL_checkinteger(L, 1))->m_nVehicleFlags.bHasBeenOwnedByPlayer);
			return 1;
		});
	}

	int lua_entities_get_player_info(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CPed*)luaL_checkinteger(L, 1))->player_info);
			return 1;
		});
	}

	int lua_entities_player_info_get_game_state(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CPlayerInfo*)luaL_checkinteger(L, 1))->game_state);
			return 1;
		});
	}

	int lua_entities_get_weapon_manager(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ((CPed*)luaL_checkinteger(L, 1))->weapon_manager);
			return 1;
		});
	}

	int lua_entities_get_head_blend_data(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getHeadBlendData());
			return 1;
		});
	}

	int lua_entities_get_net_object(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_pushPtrNilable(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getNetObject());
			return 1;
		});
	}

	int lua_entities_get_owner(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			auto b = AbstractEntity::getSmart(luaL_checkinteger(L, 1));
			luaS_push(L, b.getOwner());
			return 1;
		});
	}

	int lua_entities_set_can_migrate(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::getSmart(luaL_checkinteger(L, 1)).setCanMigrate(luaS_checkboolean(L, 2));
			return 0;
		});
	}

	int lua_entities_get_can_migrate(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getCanMigrate());
			return 1;
		});
	}

	int lua_entities_give_control(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::getSmart(luaL_checkinteger(L, 1)).giveControl((Player)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	int lua_entities_give_control_by_handle(lua_State* L) // undocumented as of 105.2
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::get((Entity)luaL_checkinteger(L, 1)).giveControl((Player)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	int lua_entities_give_control_by_pointer(lua_State* L) // undocumented as of 105.2
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::get((CPhysical*)luaL_checkinteger(L, 1)).giveControl((Player)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	int lua_entities_vehicle_get_handling(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_pushPtr<CHandlingData*>(L, luaS_check<CVehicle*>(L, 1)->handling_data);
			return 1;
		});
	}

	int lua_entities_handling_get_subhandling(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			return luaS_returnPtrNilable<CBaseSubHandlingData*>(L, luaS_check<CHandlingData*>(L, 1)->getSubhandlingData((eHandlingType)luaL_checkinteger(L, 2)));
		});
	}

	int lua_entities_detach_wheel(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			AbstractEntity::getSmart(luaL_checkinteger(L, 1)).detachWheel((int)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	int lua_entities_get_bomb_bay_pos(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).getBombBayPos().first);
			return 1;
		});
	}

	int lua_entities_is_player_ped(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushboolean(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).isAPlayer());
			return 1;
		});
	}

	int lua_entities_is_invulnerable(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushboolean(L, AbstractEntity::getSmart(luaL_checkinteger(L, 1)).isInvulnerable());
			return 1;
		});
	}

	int lua_chat_internal_try_lock(lua_State* L)
	{
		lua_pushboolean(L, Chat::mtx.tryLock());
		return 1;
	}

	int lua_chat_internal_unlock(lua_State* L)
	{
		Chat::mtx.unlock();
		return 0;
	}

	int lua_chat_internal_close_impl(lua_State* L)
	{
		HUD::CLOSE_MP_TEXT_CHAT();
		return 0;
	}

	int lua_chat_internal_open_impl(lua_State* L)
	{
		Chat::openImpl(luaS_checkboolean(L, 1));
		return 0;
	}

	int lua_chat_internal_add_to_draft(lua_State* L)
	{
		Chat::addToDraft(StringUtils::utf8_to_utf16(luaL_checkstring(L, 1)));
		return 0;
	}

	int lua_chat_send_message(lua_State* L)
	{
		std::string msg = luaL_checkstring(L, 1);
		const bool team_chat = luaS_checkboolean(L, 2);
		const bool networked = luaS_checkboolean(L, 4);
		if (!team_chat && networked)
		{
			if (!luaS_thisptr(L)->canSendChatMessage(Chat::getNumRecipientsExcludingSelf(team_chat)))
			{
				return 0;
			}
		}
		Chat::sendMessage(std::move(msg), team_chat, luaS_checkboolean(L, 3), networked);
		return 0;
	}

	int lua_chat_send_targeted_message(lua_State* L)
	{
		auto receiver = (Player)luaL_checkinteger(L, 1);
		if (receiver != g_player)
		{
			if (!luaS_thisptr(L)->canSendChatMessage(1))
			{
				return 0;
			}
		}
		Chat::sendTargetedMessage(receiver, (Player)luaL_checkinteger(L, 2), luaL_checkstring(L, 3), luaS_checkboolean(L, 4));
		return 0;
	}

	int lua_chat_on_message(lua_State* L)
	{
		const auto thisptr = luaS_getThisPtrAndImplyKeepRunning(L);
		SOUP_IF_UNLIKELY (thisptr->chat_message_callbacks.size() >= (size_t)g_tunables.getInt(TUNAHASH("lua chat_message_callbacks limit"), 1000))
		{
			luaL_error(L, "This script has reached the limit for this resource");
		}
		const auto ref = luaS_checkFunction(L, 1);
		thisptr->chat_message_callbacks.emplace_back(ref);
		lua_pushinteger(L, ref);
		return 1;
	}

	int lua_chat_get_state(lua_State* L)
	{
		lua_pushinteger(L, is_session_or_transition_active(false) ? (*pointers::chat_box)->getState() : CMultiplayerChat::CLOSED);
		return 1;
	}

	int lua_chat_get_draft(lua_State* L)
	{
		lua_pushstring(L, Chat::isOpen() ? StringUtils::utf16_to_utf8((*pointers::chat_box)->message).c_str() : "");
		return 1;
	}

	int lua_chat_remove_from_draft(lua_State* L)
	{
		const auto i = (uint8_t)luaL_checkinteger(L, 1);
		if (Chat::isOpen())
		{
			Chat::removeFromDraft(i);
		}
		return 0;
	}

	int lua_chat_get_history(lua_State* L)
	{
		lua_newtable(L);
		lua_Integer i = 1;
		for (const auto& entry : ChatHistory::history)
		{
			lua_pushinteger(L, i++);
			{
				lua_newtable(L);

				luaS_push(L, "sender_name");
				luaS_push(L, entry.sender_name);
				lua_settable(L, -3);

				luaS_push(L, "contents");
				luaS_push(L, entry.contents);
				lua_settable(L, -3);

				luaS_push(L, "team_chat");
				luaS_push(L, entry.team_chat);
				lua_settable(L, -3);

				luaS_push(L, "is_auto");
				luaS_push(L, entry.is_auto);
				lua_settable(L, -3);

				luaS_push(L, "time");
				luaS_push(L, entry.time);
				lua_settable(L, -3);
			}
			lua_settable(L, -3);
		}
		return 1;
	}

	int lua_directx_create_texture(lua_State* L)
	{
		const char* file_name = luaL_checkstring(L, 1);

		if (LuaConfig::enforce_silent_start->value != 0)
		{
			lua_getglobal(L, "SCRIPT_SILENT_START");
			const bool silent_start = lua_toboolean(L, -1);
			lua_pop(L, 1);

			if (silent_start
				&& luaS_thisptr(L)->isFirstTick()
				&& (strstr(file_name, "_logo.png") != nullptr
					|| strstr(file_name, "HC Banner.png") != nullptr
					|| strstr(file_name, "Heist Control Banner.png") != nullptr
					)
				)
			{
				if (LuaConfig::enforce_silent_start->value == 2)
				{
					luaS_notice(L, "Blocked attempt to load logo despite SCRIPT_SILENT_START");
				}

				lua_pushinteger(L, -1);
				return 1;
			}
		}

		lua_pushinteger(L, g_renderer.createTexture(luaL_checkstring(L, 1)));
		return 1;
	}

	int lua_directx_has_texture_loaded(lua_State* L)
	{
		const auto id = (int)luaL_checkinteger(L, 1);
		if (g_renderer.isTextureStillInCreateQueue(id))
		{
			lua_pushnil(L);
		}
		else
		{
			luaS_push(L, g_renderer.getScriptTexture(id) != nullptr);
		}
		return 1;
	}

	int lua_directx_get_texture_dimensions(lua_State* L)
	{
		const auto tex = g_renderer.getScriptTexture((int)luaL_checkinteger(L, 1));
		SOUP_IF_UNLIKELY (!tex)
		{
			luaL_error(L, "Texture with given id is not loaded");
		}
		luaS_push(L, tex->width);
		luaS_push(L, tex->height);
		return 2;
	}

	int lua_directx_get_texture_character(lua_State* L)
	{
		luaS_push(L, soup::unicode::utf32_to_utf8(0x100000 + (int)luaL_checkinteger(L, 1)));
		return 1;
	}

	int lua_directx_draw_texture_client(lua_State* L)
	{
		switch (lua_gettop(L))
		{
		default:
			luaL_error(L, "Unexpected number of arguments");
			break;

		case 9:
		case 12:
		{
			DirectX::SimpleMath::Color colour = luaS_checkColour(L, 9);
			g_renderer.drawTextureThisTickCP(
				(int)luaL_checkinteger(L, 1),
				(float)luaL_checknumber(L, 2),
				(float)luaL_checknumber(L, 3),
				(float)luaL_checknumber(L, 4),
				(float)luaL_checknumber(L, 5),
				(float)luaL_checknumber(L, 6),
				(float)luaL_checknumber(L, 7),
				(float)luaL_checknumber(L, 8),
				colour.x,
				colour.y,
				colour.z,
				colour.w
			);
		}
		break;

		case 13:
		case 16:
		{
			DirectX::SimpleMath::Color colour = luaS_checkColour(L, 13);
			g_renderer.drawTextureTimedCP(
				(int)luaL_checkinteger(L, 1),
				(int)luaL_checkinteger(L, 4),
				(float)luaL_checknumber(L, 5),
				(float)luaL_checknumber(L, 6),
				(float)luaL_checknumber(L, 7),
				(float)luaL_checknumber(L, 8),
				(float)luaL_checknumber(L, 9),
				(float)luaL_checknumber(L, 10),
				(float)luaL_checknumber(L, 11),
				colour.x,
				colour.y,
				colour.z,
				colour.w
			);
		}
		break;
		}
		return 0;
	}

	int lua_directx_draw_texture_by_points(lua_State* L)
	{
		g_renderer.drawSpriteByPointsThisTickHP(
			(int)luaL_checkinteger(L, 1),
			(float)luaL_checknumber(L, 2),
			(float)luaL_checknumber(L, 3),
			(float)luaL_checknumber(L, 4),
			(float)luaL_checknumber(L, 5),
			luaS_checkColour(L, 6)
		);
		return 0;
	}

	int lua_directx_create_font(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			auto path = StringUtils::utf8_to_utf16(luaL_checkstring(L, 1));
			auto font = new SpriteFont(g_renderer.m_pDevice, path.c_str());
			g_renderer.setupFont(*font);
			soup::LuaBindings::pushNew<soup::SharedPtr<SpriteFont>>(L, font);
			lua_newtable(L);
			soup::LuaBindings::addDtorToMt<soup::SharedPtr<SpriteFont>>(L);
			lua_setmetatable(L, -2);
			return 1;
		});
	}

	int lua_directx_draw_text(lua_State* L)
	{
		int i = 6;
		auto colour = luaS_checkColour(L, i);
		soup::SharedPtr<SpriteFont> font = g_renderer.m_font_user;
		bool force_in_bounds = false;
		v3 draw_origin_3d{};
		if (lua_gettop(L) > i)
		{
			force_in_bounds = luaS_checkboolean(L, ++i);
			if (lua_gettop(L) > i)
			{
				if (lua_type(L, ++i) != LUA_TNIL)
				{
					font = *reinterpret_cast<soup::SharedPtr<SpriteFont>*>(luaS_checkuserdata(L, i));
				}
				if (lua_gettop(L) > i)
				{
					draw_origin_3d = luaS_checkV3<v3>(L, ++i);
				}
			}
		}
		g_renderer.drawTextThisTickHP(
			float(luaL_checknumber(L, 1)),
			float(luaL_checknumber(L, 2)),
			StringUtils::utf8_to_utf16(luaL_checkstring(L, 3)),
			(Alignment)luaL_checkinteger(L, 4),
			(float)luaL_checknumber(L, 5),
			std::move(colour),
			font,
			force_in_bounds,
			std::move(draw_origin_3d)
		);
		return 0;
	}

	int lua_directx_draw_text_client(lua_State* L)
	{
		int i = 6;
		auto colour = luaS_checkColour(L, i);
		soup::SharedPtr<SpriteFont> font = g_renderer.m_font_user;
		bool force_in_bounds = false;
		v3 draw_origin_3d{};
		if (lua_gettop(L) > i)
		{
			force_in_bounds = luaS_checkboolean(L, ++i);
			if (lua_gettop(L) > i)
			{
				if (lua_type(L, ++i) != LUA_TNIL)
				{
					font = *reinterpret_cast<soup::SharedPtr<SpriteFont>*>(luaS_checkuserdata(L, i));
				}
				if (lua_gettop(L) > i)
				{
					draw_origin_3d = luaS_checkV3<v3>(L, ++i);
				}
			}
		}
		g_renderer.drawTextThisTickCP(
			float(luaL_checknumber(L, 1)),
			float(luaL_checknumber(L, 2)),
			StringUtils::utf8_to_utf16(luaL_checkstring(L, 3)),
			(Alignment)luaL_checkinteger(L, 4),
			(float)luaL_checknumber(L, 5),
			std::move(colour),
			font,
			force_in_bounds,
			std::move(draw_origin_3d)
		);
		return 0;
	}

	int lua_directx_draw_rect(lua_State* L)
	{
		g_renderer.drawRectThisTickHP(
			luaL_checknumber(L, 1),
			luaL_checknumber(L, 2),
			luaL_checknumber(L, 3),
			luaL_checknumber(L, 4),
			luaS_checkColour(L, 5)
		);
		return 0;
	}

	int lua_directx_draw_rect_client(lua_State* L)
	{
		g_renderer.drawRectThisTickCP(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2),
			(float)luaL_checknumber(L, 3),
			(float)luaL_checknumber(L, 4),
			luaS_checkColour(L, 5)
		);
		return 0;
	}

	int lua_directx_draw_line(lua_State* L)
	{
		int i = 5;
		const DirectX::SimpleMath::Color colour1 = luaS_checkColour(L, i);
		DirectX::SimpleMath::Color colour2;
		if (lua_gettop(L) > i)
		{
			colour2 = luaS_checkColour(L, ++i);
		}
		else
		{
			colour2 = colour1;
		}
		g_renderer.drawLineThisTickHP(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2),
			(float)luaL_checknumber(L, 3),
			(float)luaL_checknumber(L, 4),
			colour1,
			colour2
		);
		return 0;
	}

	int lua_directx_draw_line_client(lua_State* L)
	{
		int i = 5;
		const DirectX::SimpleMath::Color colour1 = luaS_checkColour(L, i);
		DirectX::SimpleMath::Color colour2;
		if (lua_gettop(L) > i)
		{
			colour2 = luaS_checkColour(L, ++i);
		}
		else
		{
			colour2 = colour1;
		}
		g_renderer.drawLineThisTickCP(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2),
			(float)luaL_checknumber(L, 3),
			(float)luaL_checknumber(L, 4),
			colour1,
			colour2
		);
		return 0;
	}

	int lua_directx_draw_triangle(lua_State* L)
	{
		g_renderer.drawTriangleThisTickHP(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2),
			(float)luaL_checknumber(L, 3),
			(float)luaL_checknumber(L, 4),
			(float)luaL_checknumber(L, 5),
			(float)luaL_checknumber(L, 6),
			luaS_checkColour(L, 7)
		);
		return 0;
	}

	int lua_directx_draw_triangle_client(lua_State* L)
	{
		g_renderer.drawTriangleThisTickCP(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2),
			(float)luaL_checknumber(L, 3),
			(float)luaL_checknumber(L, 4),
			(float)luaL_checknumber(L, 5),
			(float)luaL_checknumber(L, 6),
			luaS_checkColour(L, 7)
		);
		return 0;
	}

	int lua_directx_draw_circle_square(lua_State* L) // undocumented
	{
		auto x = (float)luaL_checknumber(L, 1);
		auto y = (float)luaL_checknumber(L, 2);
		auto radius = (float)luaL_checknumber(L, 3);
		int i = 4;
		auto colour = luaS_checkColour(L, i);
		auto part = (int)luaL_optinteger(L, ++i, -1);
		g_renderer.drawCircleThisTickS(x, y, radius, part, colour);
		return 0;
	}

	int lua_directx_get_client_size(lua_State* L)
	{
		luaS_push(L, g_renderer.client_size.x);
		luaS_push(L, g_renderer.client_size.y);
		return 2;
	}

	[[nodiscard]] static DirectX::SimpleMath::Vector2 to_percentage(const DirectX::SimpleMath::Vector2& v, bool client)
	{
		if (client)
		{
			return g_renderer.C2CP(v);
		}
		else
		{
			return g_renderer.H2HP(v);
		}
	}

	static int lua_directx_get_text_size_impl(lua_State* L, bool client)
	{
		std::wstring text = StringUtils::utf8_to_utf16(luaL_checkstring(L, 1));
		soup::SharedPtr<SpriteFont> font = g_renderer.m_font_user;
		float scale = 1.0f;
		if (lua_gettop(L) >= 2)
		{
			scale = (float)luaL_checknumber(L, 2);
			if (lua_gettop(L) >= 3)
			{
				if (lua_type(L, 3) != LUA_TNIL)
				{
					font = *reinterpret_cast<soup::SharedPtr<SpriteFont>*>(luaS_checkuserdata(L, 3));
				}
			}
		}
		if (!client)
		{
			scale *= g_renderer.resolution_text_scale;
		}
		auto size = g_renderer.getTextSize(std::move(font), text);
		size *= scale;
		size = to_percentage(size, client);
		luaS_push(L, size.x);
		luaS_push(L, size.y);
		return 2;
	}

	int lua_directx_get_text_size(lua_State* L)
	{
		return lua_directx_get_text_size_impl(L, false);
	}

	int lua_directx_get_text_size_client(lua_State* L)
	{
		return lua_directx_get_text_size_impl(L, true);
	}

	int lua_directx_pos_hud_to_client(lua_State* L)
	{
		auto v = g_renderer.C2CP(g_renderer.posH2C(g_renderer.HP2H(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2)
		)));
		luaS_push(L, v.x);
		luaS_push(L, v.y);
		return 2;
	}

	int lua_directx_size_hud_to_client(lua_State* L)
	{
		auto v = g_renderer.C2CP(g_renderer.sizeH2C(g_renderer.HP2H(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2)
		)));
		luaS_push(L, v.x);
		luaS_push(L, v.y);
		return 2;
	}

	int lua_directx_pos_client_to_hud(lua_State* L)
	{
		auto v = g_renderer.H2HP(g_renderer.posC2H(g_renderer.CP2C(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2)
		)));
		luaS_push(L, v.x);
		luaS_push(L, v.y);
		return 2;
	}

	int lua_directx_size_client_to_hud(lua_State* L)
	{
		auto v = g_renderer.H2HP(g_renderer.sizeC2H(g_renderer.CP2C(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2)
		)));
		luaS_push(L, v.x);
		luaS_push(L, v.y);
		return 2;
	}

	int lua_directx_square_to_client(lua_State* L) // undocumented
	{
		auto v = g_renderer.C2CP(g_renderer.S2C(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2)
		));
		luaS_push(L, v.x);
		luaS_push(L, v.y);
		return 2;
	}

	int lua_directx_client_to_square(lua_State* L) // undocumented
	{
		auto v = g_renderer.C2S(g_renderer.CP2C(
			(float)luaL_checknumber(L, 1),
			(float)luaL_checknumber(L, 2)
		));
		luaS_push(L, v.x);
		luaS_push(L, v.y);
		return 2;
	}

	int lua_directx_blurrect_new(lua_State* L)
	{
		const auto thisptr = luaS_thisptr(L);
		SOUP_IF_UNLIKELY (thisptr->bgblurs.size() >= (size_t)g_tunables.getInt(TUNAHASH("lua blurrect limit"), 1000))
		{
			luaL_error(L, "This script has reached the limit for this resource");
		}
		return thisptr->bgblurs.construct(L, new EphemeralBackgroundBlur());
	}

	int lua_directx_blurrect_free(lua_State* L)
	{
		EXCEPTIONAL_LOCK(g_renderer.extras_mtx)
		luaS_thisptr(L)->bgblurs.free(L);
		EXCEPTIONAL_UNLOCK(g_renderer.extras_mtx)
		return 0;
	}

	int lua_directx_blurrect_draw(lua_State* L)
	{
		auto inst = reinterpret_cast<EphemeralBackgroundBlur*>(luaL_checkinteger(L, 1));
		auto pos = g_renderer.posH2C(g_renderer.HP2H((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3)));
		auto size = g_renderer.sizeH2C(g_renderer.HP2H((float)luaL_checknumber(L, 4), (float)luaL_checknumber(L, 5)));
		auto strength = (uint8_t)luaL_checkinteger(L, 6);
		g_renderer.drawBgBlurThisTickC(inst, pos.x, pos.y, size.x, size.y, strength);
		return 0;
	}

	int lua_directx_blurrect_draw_client(lua_State* L)
	{
		auto inst = reinterpret_cast<EphemeralBackgroundBlur*>(luaL_checkinteger(L, 1));
		auto pos = g_renderer.CP2C((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
		auto size = g_renderer.CP2C((float)luaL_checknumber(L, 4), (float)luaL_checknumber(L, 5));
		auto strength = (uint8_t)luaL_checkinteger(L, 6);
		g_renderer.drawBgBlurThisTickC(inst, pos.x, pos.y, size.x, size.y, strength);
		return 0;
	}

	/*int lua_util_internal_prerun_rantpgscmu(lua_State* L)
	{
		if (CommandLuaScript::isObfuscatedCode(pluto_checkstring(L, 1)))
		{
			CommandLuaScript::onObfuscatedCodeDetected();
			lua_pushboolean(L, true);
		}
		else
		{
			lua_pushboolean(L, false);
		}
		return 1;
	}*/

	int lua_util_ensure_package_is_installed(lua_State* L) // undocumented
	{
		auto pkgname = luaL_checkstring(L, 1);
		Package* pkg = PackageMgr::find(pkgname);
		SOUP_IF_UNLIKELY (!pkg)
		{
			luaL_error(L, "Package %s not found", pkgname);
		}
		luaS_thisptr(L)->enableBusy();
		while (pkg->isBusy())
		{
			ExecCtx::get().yield();
		}
		if (!pkg->isInstalled())
		{
			pkg->setBusyForInstall();
			if (luaS_thisptr(L)->isInstalledViaRepo())
			{
				// Not doing any checks for this because the dev is unlikely to see this message. Relying on users to report it.
				luaS_notice(L, "Dependency declared via util.ensure_package_is_installed on a script installed from repo was not already installed. This indicates faulty metadata.");
			}
			soup::Thread t([](soup::Capture&& cap)
			{
				auto pkg = cap.get<Package*>();
				__try
				{
					pkg->userInstall();
				}
				__EXCEPTIONAL()
				{
				}
			}, pkg);
			luaS_awaitThreadCompletion(L, t);
		}
		luaS_thisptr(L)->disableBusy();
		return 0;
	}

	int lua_util_execute_in_os_thread(lua_State* L)
	{
		luaL_checktype(L, 1, LUA_TFUNCTION);
		soup::Thread t([](soup::Capture&& cap)
		{
			auto L = cap.get<lua_State*>();
			__try
			{
				lua_getglobal(L, "internal_xpcall_ioiqocsdvv");
				lua_pushvalue(L, 1);
				lua_call(L, 1, 0);
			}
			__EXCEPTIONAL_LUA()
			{
			}
		}, L);
		luaS_thisptr(L)->enableBusy();
		luaS_awaitThreadCompletion(L, t);
		luaS_thisptr(L)->disableBusy();
		return 0;
	}

	static HttpRequestBuilder* async_http_builder = nullptr;

	int lua_util_internal_yield_zfvasowkyumq(lua_State* L)
	{
		// Check if the dev missed something
		SOUP_IF_UNLIKELY (async_http_builder != nullptr)
		{
			delete async_http_builder;
			async_http_builder = nullptr;

			luaS_notice(L, "async_http.init was called, expected async_http.dispatch to be called in the same tick");
		}

		// Yield this fiber (and keep yielding if the game_state is unsuitable)
		do
		{
			Script::current()->yield();
		} while (*pointers::CLoadingScreens_ms_Context != 0 && luaS_canContinue(L));

		// Apply command state & manage related variables
		const auto thisptr = luaS_thisptr(L);
		if (thisptr->first_ticks)
		{
			thisptr->applyCommandStates();
			if (--thisptr->first_ticks == 0)
			{
				CommandLuaScript::hold_up_save = false;
			}
		}
#ifdef STAND_DEBUG
		else
		{
			SOUP_ASSERT(thisptr->need_state_commands.empty());
		}
#endif

		// Return to lua runtime
		return 0;
	}

	int lua_util_internal_is_keep_running_implied_uuqepkb32o(lua_State* L)
	{
		lua_pushboolean(L, luaS_thisptr(L)->keep_running_implied);
		return 1;
	}

	int lua_util_internal_emit_bad_practice_w7uoni5ptt(lua_State* L)
	{
		if (LuaConfig::notify_bad_practice->m_on)
		{
			luaS_notice(L, luaL_checkstring(L, 1));
		}
		return 0;
	}

	int lua_util_internal_emit_deprecated_hzpyiownhj(lua_State* L)
	{
		luaS_deprecated(L, luaL_checkstring(L, 1));
		return 0;
	}

	int lua_util_request_stop(lua_State* L) // undocumented
	{
		luaS_thisptr(L)->requestStop(false, false);
		return 0;
	}

	[[nodiscard]] static bool luaS_isOsThread()
	{
		return ExecCtx::get().tc == TC_OTHER;
	}

	int lua_util_is_os_thread(lua_State* L) // undocumented
	{
		luaS_push(L, luaS_isOsThread());
		return 1;
	}

	int lua_util_can_continue(lua_State* L)
	{
		luaS_push(L, luaS_thisptr(L)->can_continue);
		luaS_push(L, luaS_thisptr(L)->is_silent_stop);
		luaS_push(L, luaS_thisptr(L)->is_instant_stop); // undocumented
		return 3;
	}

	int lua_util_set_busy(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			if (luaS_checkboolean(L, 1))
			{
				luaS_thisptr(L)->enableBusy();
			}
			else
			{
				luaS_thisptr(L)->disableBusy();
			}
			return 0;
		});
	}

	int lua_util_on_stop(lua_State* L)
	{
		luaS_thisptr(L)->stop_handlers.emplace_back(luaS_checkFunction(L, 1));
		return 0;
	}

	int lua_util_dispatch_on_stop(lua_State* L) // undocumented
	{
		luaS_thisptr(L)->dispatchOnStop();
		return 0;
	}

	int lua_util_toast(lua_State* L)
	{
		if (luaS_checkSilent(L, "util.toast"))
		{
			toast_t flags = (lua_gettop(L) >= 2 ? (toast_t)luaL_checkinteger(L, 2) : TOAST_DEFAULT);
			if (flags & TOAST_CHAT)
			{
				if (!luaS_thisptr(L)->canSendChatMessage(AbstractPlayer::getPlayerCount() - 1))
				{
					flags &= ~TOAST_CHAT;
				}
			}
			Util::toast(luaS_checkStringCastable(L, 1), flags);
		}
		return 0;
	}

	int lua_util_log(lua_State* L)
	{
		Util::toast(luaS_checkStringCastable(L, 1), TOAST_LOGGER);
		return 0;
	}

	int lua_util_draw_debug_text(lua_State* L)
	{
		drawDebugText(luaS_checkStringCastable(L, 1));
		return 0;
	}

	int lua_util_draw_centred_text(lua_State* L)
	{
		g_renderer.drawCentredTextThisTick(luaS_checkStringCastable(L, 1));
		return 0;
	}

	int lua_util_show_corner_help(lua_State* L)
	{
		if (luaS_checkSilent(L, "util.show_corner_help"))
		{
			Util::show_corner_help(luaL_checkstring(L, 1), true);
		}
		return 0;
	}

	int lua_util_replace_corner_help(lua_State* L)
	{
		Util::replace_corner_help(luaL_checkstring(L, 1), luaL_checkstring(L, 2));
		return 0;
	}

	int lua_util_joaat(lua_State* L)
	{
		lua_pushinteger(L, (hash_t)rage::atStringHash(luaL_checkstring(L, 1)));
		return 1;
	}

	int lua_util_ujoaat(lua_State* L)
	{
		lua_pushinteger(L, (uhash_t)rage::atStringHash(luaL_checkstring(L, 1)));
		return 1;
	}

	int lua_util_reverse_joaat(lua_State* L)
	{
		if (auto str = joaatToStringRaw((hash_t)luaL_checkinteger(L, 1)))
		{
			lua_pushstring(L, str);
		}
		else
		{
			luaS_push(L, std::string{});
		}
		return 1;
	}

	int lua_util_is_this_model_a_blimp(lua_State* L)
	{
		luaS_push(L, AbstractModel(luaS_checkHash(L, 1)).getVehicleType() == VEHICLE_TYPE_BLIMP);
		return 1;
	}

	int lua_util_is_this_model_an_object(lua_State* L)
	{
		luaS_push(L, AbstractModel(luaS_checkHash(L, 1)).isObject());
		return 1;
	}

	int lua_util_is_this_model_a_submarine(lua_State* L)
	{
		luaS_push(L, AbstractModel(luaS_checkHash(L, 1)).isSubmarine());
		return 1;
	}

	int lua_util_is_this_model_a_submarine_car(lua_State* L)
	{
		luaS_push(L, AbstractModel(luaS_checkHash(L, 1)).isSubmarineCar());
		return 1;
	}

	int lua_util_is_this_model_a_trailer(lua_State* L)
	{
		luaS_push(L, AbstractModel(luaS_checkHash(L, 1)).isTrailer());
		return 1;
	}

	int lua_util_get_vehicles(lua_State* L)
	{
		lua_newtable(L);
		size_t i = 1;
		for (const auto& entry : vehicle_items)
		{
			luaS_push(L, i++);
			lua_newtable(L);

			lua_pushstring(L, "name");
			lua_pushstring(L, entry.name);
			lua_settable(L, -3);

			lua_pushstring(L, "manufacturer");
			lua_pushstring(L, entry.manufacturer);
			lua_settable(L, -3);

			lua_pushstring(L, "class");
			lua_pushinteger(L, (hash_t)entry.clazz);
			lua_settable(L, -3);

			lua_settable(L, -3);
		}
		return 1;
	}

	int lua_util_get_objects(lua_State* L)
	{
		lua_newtable(L);
		size_t i = 1;
		for (const auto& obj : g_objects)
		{
			luaS_push(L, i++);
			lua_pushstring(L, obj);
			lua_settable(L, -3);
		}
		for (const auto& obj : g_objects_downtown)
		{
			luaS_push(L, i++);
			lua_pushstring(L, obj);
			lua_settable(L, -3);
		}
		return 1;
	}

	int lua_util_get_weapons(lua_State* L)
	{
		lua_newtable(L);
		size_t i = 1;
		for (const auto& entry : Weapon::weapons)
		{
			luaS_push(L, i++);
			lua_newtable(L);

			lua_pushstring(L, "hash");
			luaS_push(L, entry.hash);
			lua_settable(L, -3);

			lua_pushstring(L, "label_key");
			luaS_push(L, entry.label_hash_name);
			lua_settable(L, -3);

			lua_pushstring(L, "category");
			luaS_push(L, entry.getCategoryName());
			lua_settable(L, -3);

			lua_pushstring(L, "category_id");
			luaS_push(L, entry.category);
			lua_settable(L, -3);

			lua_settable(L, -3);
		}
		return 1;
	}

	int lua_util_begin_text_command_display_text(lua_State* L)
	{
		Util::BEGIN_TEXT_COMMAND_DISPLAY_TEXT(luaL_checkstring(L, 1));
		return 0;
	}

	int lua_util_begin_text_command_line_count(lua_State* L)
	{
		Util::_BEGIN_TEXT_COMMAND_LINE_COUNT(luaL_checkstring(L, 1));
		return 0;
	}

	int lua_util_begin_text_command_is_this_help_message_being_displayed(lua_State* L)
	{
		Util::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(luaL_checkstring(L, 1));
		return 0;
	}

	int lua_util_begin_text_command_display_help(lua_State* L)
	{
		Util::BEGIN_TEXT_COMMAND_DISPLAY_HELP(luaL_checkstring(L, 1));
		return 0;
	}

	int lua_util_begin_text_command_get_width(lua_State* L)
	{
		Util::_BEGIN_TEXT_COMMAND_GET_WIDTH(luaL_checkstring(L, 1));
		return 0;
	}

	int lua_util_begin_text_command_thefeed_post(lua_State* L)
	{
		Util::BEGIN_TEXT_COMMAND_THEFEED_POST(luaL_checkstring(L, 1));
		return 0;
	}

	int lua_util_get_rp_required_for_rank(lua_State* L)
	{
		lua_pushinteger(L, Util::get_rp_required_for_rank((int)luaL_checkinteger(L, 1)));
		return 1;
	}

	int lua_util_get_session_players_bitflag(lua_State* L)
	{
		lua_pushinteger(L, Util::get_session_players_bitflag());
		return 1;
	}

	[[nodiscard]] std::vector<int64_t> luaS_checkTseArray(lua_State* L, int i, bool reinterpret_floats)
	{
		if (lua_gettop(L) < i)
		{
			luaL_error(L, std::string("Missing int array (table) parameter ").append(fmt::to_string(i)).c_str());
		}
		if (!lua_istable(L, i))
		{
			luaL_error(L, std::string("Expected parameter ").append(fmt::to_string(i)).append(" to be a int array (table)").c_str());
		}
		std::vector<int64_t> arr = {};
		lua_pushvalue(L, i);
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			lua_pushvalue(L, -2);
			SOUP_IF_UNLIKELY (lua_type(L, -2) == LUA_TNUMBER
				&& !lua_isinteger(L, i)
				&& reinterpret_floats
				)
			{
				*reinterpret_cast<float*>(&arr.emplace_back(0)) = (float)lua_tonumber(L, -2);
			}
			else
			{
				arr.emplace_back(luaL_checkinteger(L, -2));
			}
			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		return arr;
	}

	int lua_util_trigger_script_event(lua_State* L)
	{
		const lua_Integer session_player_bitflags = luaL_checkinteger(L, 1);
		bool reinterpret_floats = false;
		if (lua_gettop(L) >= 3)
		{
			reinterpret_floats = luaS_checkboolean(L, 3);
		}
		std::vector<int64_t> data = luaS_checkTseArray(L, 2, reinterpret_floats);
		SOUP_IF_LIKELY (!data.empty())
		{
			tse_impl(&data[0], (int)data.size(), (int)session_player_bitflags);
		}
		return 0;
	}

	int lua_util_current_time_millis(lua_State* L)
	{
		lua_pushinteger(L, get_current_time_millis());
		return 1;
	}

	int lua_util_current_unix_time_seconds(lua_State* L)
	{
		lua_pushinteger(L, get_seconds_since_unix_epoch());
		return 1;
	}

	int lua_util_remove_handler(lua_State* L)
	{
		const auto ref = (int)luaL_checkinteger(L, 1);
		auto* const thisptr = luaS_thisptr(L);

		REMOVE_HANDLER_FROM_VEC(thisptr->chat_message_callbacks);
		REMOVE_HANDLER_FROM_VEC(thisptr->pad_shake_callbacks);
		REMOVE_HANDLER_FROM_VEC(thisptr->player_join_callbacks);
		REMOVE_HANDLER_FROM_VEC(thisptr->player_leave_callbacks);
		REMOVE_HANDLER_FROM_VEC(thisptr->flow_event_done_callbacks);
		REMOVE_HANDLER_FROM_VEC(thisptr->transition_finished_callbacks);

		lua_pushboolean(L, false);
		return 1;
	}

	int lua_util_clear_commands_and_native_event_handlers(lua_State* L) // undocumented
	{
		FiberPool::queueJob([wr{ luaS_thisptr(L)->getWeakRef() }]
		{
			if (auto* const thisptr = wr.getPointer()->as<CommandLuaScript>())
			{
				if (thisptr->running)
				{
					thisptr->clearEventHandlers();
					thisptr->applyDefaultState(true);
					thisptr->dispatchOnStop();
					thisptr->stop_handlers.clear();
					thisptr->clearCommands();
					thisptr->addHeadCommands();

					// Since this is only used with restart_script, we want to go back to "early load" state, so apply_command_states will work again.
					thisptr->first_ticks = 100;
				}
			}
		});
		return 0;
	}

	int lua_util_is_session_started(lua_State* L)
	{
		lua_pushboolean(L, *pointers::is_session_started);
		return 1;
	}

	int lua_util_is_session_transition_active(lua_State* L)
	{
		lua_pushboolean(L, is_session_transition_active(true));
		return 1;
	}

	int lua_util_get_char_slot(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			return 1;
		});
	}

	int lua_util_get_ground_z(lua_State* L)
	{
		auto x = (float)luaL_checknumber(L, 1);
		auto y = (float)luaL_checknumber(L, 2);
		auto z_hint = lua_gettop(L) >= 3 ? (float)luaL_checknumber(L, 3) : 1000.0f;
		if (z_hint == 1000.0f)
		{
			STREAMING::REQUEST_COLLISION_AT_COORD(x, y, PATHFIND::GET_APPROX_HEIGHT_FOR_POINT(x, y));
		}
		else
		{
			STREAMING::REQUEST_COLLISION_AT_COORD(x, y, z_hint);
		}
		float ground_z = get_ground_z(x, y, z_hint);
		lua_pushboolean(L, ground_z != -200.0f);
		lua_pushnumber(L, ground_z);
		return 2;
	}

	static int restSpoofScript(lua_State* L, GtaThread* thread)
	{
		if (luaS_isOsThread())
		{
			luaL_error(L, "This function is not available in OS threads");
		}
		if (thread == nullptr)
		{
			lua_pushboolean(L, false);
		}
		else
		{
			const auto ref = luaS_checkFunction(L, 2);
			thread->spoofAs([L, ref]
			{
				luaS_invoke_void_direct(L, ref);
				luaS_releaseReference(L, ref);
			});
			lua_pushboolean(L, true);
		}
		return 1;
	}

	int lua_util_spoof_script(lua_State* L)
	{
		return restSpoofScript(L, GtaThread::fromHash(luaS_checkHash(L, 1)));
	}

	int lua_util_spoof_script_thread(lua_State* L)
	{
		return restSpoofScript(L, GtaThread::fromId((uint32_t)luaL_checkinteger(L, 1)));
	}

	int lua_util_remove_blip(lua_State* L)
	{
		BlipUtil::remove((Blip)luaL_checkinteger(L, 1));
		return 0;
	}

	int lua_util_arspinner_is_enabled(lua_State* L)
	{
		luaS_push(L, g_tb_ar_spinner.isEnabled());
		return 1;
	}

	int lua_util_arspinner_enable(lua_State* L)
	{
		auto thisptr = luaS_thisptr(L);
		if (!thisptr->enabled_ar_spinner)
		{
			thisptr->enabled_ar_spinner = true;
			g_tb_ar_spinner.enable();
		}
		return 0;
	}

	int lua_util_arspinner_disable(lua_State* L)
	{
		luaS_thisptr(L)->disableArSpinner();
		return 0;
	}

	int lua_util_graceland_is_enabled(lua_State* L)
	{
		luaS_push(L, g_hooking.graceful_landing != 0);
		return 1;
	}

	int lua_util_graceland_enable(lua_State* L)
	{
		auto thisptr = luaS_thisptr(L);
		if (!thisptr->enabled_graceland)
		{
			thisptr->enabled_graceland = true;
			++g_hooking.graceful_landing;
		}
		return 0;
	}

	int lua_util_graceland_disable(lua_State* L)
	{
		luaS_thisptr(L)->disableGracefulLanding();
		return 0;
	}

	int lua_util_is_bigmap_active(lua_State* L)
	{
		luaS_push(L, Util::is_bigmap_active());
		return 1;
	}

	int lua_util_copy_to_clipboard(lua_State* L)
	{
		bool notify = (lua_gettop(L) < 2 || luaS_checkboolean(L, 2));
		Click click(notify ? CLICK_AUTO : CLICK_BULK, TC_SCRIPT_YIELDABLE);
		Util::copy_to_clipboard_utf8(click, pluto_checkstring(L, 1));
		return 0;
	}

	int lua_util_get_clipboard_text(lua_State* L)
	{
		luaS_push(L, Util::getClipboardTextUTF8());
		return 1;
	}

	int lua_util_read_colons_and_tabs_file(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			std::ifstream in(std::filesystem::path(reinterpret_cast<const char8_t*>(luaL_checkstring(L, 1))));
			std::unordered_map<std::string, std::string> data{};
			ColonsAndTabs::read(in, data);
			luaS_push(L, data);
			return 1;
		});
	}

	int lua_util_write_colons_file(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			std::filesystem::path file = reinterpret_cast<const char8_t*>(luaL_checkstring(L, 1));
			auto data = luaS_check_map_string_string(L, 2);
			std::ofstream out(file);
			ColonsAndTabs::write(out, data);
			return 0;
		});
	}

	int lua_util_draw_ar_beacon(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			DrawUtil3d::draw_ar_beacon(luaS_checkV3<v3>(L, 1));
			return 0;
		});
	}

	int lua_util_draw_box(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			soup::gmBox bx{
				luaS_checkV3<soup::Vector3>(L, 1),
				luaS_checkV3<soup::Vector3>(L, 2),
				luaS_checkV3<soup::Vector3>(L, 3),
			};
			auto r = (int)luaL_checkinteger(L, 4);
			auto g = (int)luaL_checkinteger(L, 5);
			auto b = (int)luaL_checkinteger(L, 6);
			auto a = (int)luaL_optinteger(L, 7, 255);
			for (const auto& p : bx.toPolys())
			{
				DrawUtil3d::draw_poly(p.a, p.b, p.c, r, g, b, a);
			}
			return 0;
		});
	}

	int lua_util_draw_sphere(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			auto pos = luaS_checkV3<v3>(L, 1);
			auto radius = (float)luaL_checknumber(L, 2);
			auto r = (int)luaL_checkinteger(L, 3);
			auto g = (int)luaL_checkinteger(L, 4);
			auto b = (int)luaL_checkinteger(L, 5);
			auto a = (int)luaL_optinteger(L, 6, 255);
			auto quality = (int)luaL_optinteger(L, 7, 40);
			DrawUtil3d::draw_sphere(pos, radius, r, g, b, a, quality);
			return 0;
		});
	}

	int lua_util_request_script_host(lua_State* L)
	{
		const auto scriptHash = luaS_checkHash(L, 1);
		GtaThread* thread = nullptr;
		if (scriptHash != ATSTRINGHASH("freemode")
			|| ScriptHostUtil::canSafelyTakeIt()
			)
		{
			thread = GtaThread::fromHash(scriptHash);
		}
		if (thread != nullptr)
		{
			if (auto netcomp = thread->getNetComponent())
			{
				netcomp->takeHost();
				lua_pushboolean(L, true);
			}
			else
			{
				lua_pushboolean(L, false);
			}
		}
		else
		{
			lua_pushboolean(L, false);
		}
		return 1;
	}

	int lua_util_give_script_host(lua_State* L)
	{
		CNetGamePlayer* p = AbstractPlayer((Player)luaL_checkinteger(L, 2)).getCNetGamePlayer();
		if (p == nullptr)
		{
			luaL_error(L, soup::ObfusString("Invalid target player").c_str());
		}
		if (auto thread = GtaThread::fromHash(luaS_checkHash(L, 1)))
		{
			if (auto netcomp = thread->getNetComponent())
			{
				if (!netcomp->isHost(p))
				{
					if (!netcomp->amHost())
					{
						luaL_error(L, soup::ObfusString("You need to be script host in order to transfer it").c_str());
					}
					netcomp->giveHostRaw(p);
				}
				lua_pushboolean(L, true);
			}
			else
			{
				lua_pushboolean(L, false);
			}
		}
		else
		{
			lua_pushboolean(L, false);
		}
		return 1;
	}

	static bool registerFile(const std::filesystem::path& p)
	{
		if (std::filesystem::is_regular_file(p)
			&& pointers::register_file != nullptr
			)
		{
			uint32_t texId;
			pointers::register_file(&texId, p.string().c_str(), true, p.filename().string().c_str(), false, false);
			return texId != -1;
		}
		return false;
	}

	int lua_util_register_file(lua_State* L)
	{
		luaS_push(L, registerFile(std::filesystem::path((const char8_t*)luaL_checkstring(L, 1))));
		return 1;
	}

	int lua_util_get_label_text(lua_State* L)
	{
		luaS_push(L, Util::GET_LABEL_TEXT_no_replacements(luaS_checkHash(L, 1)));
		return 1;
	}

	int lua_util_register_label(lua_State* L)
	{
		std::string key;
		uint32_t key_hash;
		do
		{
			key = soup::rand.str<std::string>(6);
			key_hash = rage::atStringHash(key);
		} while (g_hooking.doesTextLabelExist(key_hash));

		g_hooking.label_replacements.emplace(key_hash, luaL_checkstring(L, 1));

		luaS_push(L, key);
		return 1;
	}

	int lua_util_is_key_down(lua_State* L)
	{
		if (GetForegroundWindow() != pointers::hwnd)
		{
			luaS_push(L, false);
		}
		else
		{
			int vk = 0;
			if (lua_type(L, 1) == LUA_TSTRING)
			{
				vk = (int)*luaL_checkstring(L, 1);
			}
			if ((vk < 'A' || vk > 'Z')
				&& (vk < '0' || vk > '9')
				&& vk != ' '
				)
			{
				vk = (int)luaL_checkinteger(L, 1);
			}
			luaS_push(L, bool(GetAsyncKeyState(vk) & 0x8000));
		}
		return 1;
	}

	[[nodiscard]] static uintptr_t luaS_checkRegister(lua_State* L, int i)
	{
		if (lua_type(L, i) == LUA_TSTRING)
		{
			return reinterpret_cast<uintptr_t>(luaL_checkstring(L, i));
		}
		if (lua_type(L, i) == LUA_TUSERDATA)
		{
			return (uintptr_t)lua_touserdata(L, i);
		}
		if (lua_type(L, i) == LUA_TNUMBER
			&& !lua_isinteger(L, i)
			)
		{
			float fval = static_cast<float>(luaL_checknumber(L, i));
			return static_cast<uintptr_t>(*reinterpret_cast<uint32_t*>(&fval));
		}
		return (uintptr_t)luaL_checkinteger(L, i);
	}

	int lua_util_call_foreign_function(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			std::vector<uintptr_t> args{};
			const int num_args = (lua_gettop(L) + 1);
			if (num_args >= 2)
			{
				for (int i = 2; i != num_args; ++i)
				{
					args.emplace_back(luaS_checkRegister(L, i));
				}
			}
			const auto func = (void*)luaL_checkinteger(L, 1);
			if (!soup::ffi::isSafeToCall(func))
			{
				luaL_error(L, "Attempt to call an unsafe function");
			}
			bool warning_required = g_gta_module.range.contains(func)
									? g_tunables.isOnAddressList(TUNAHASH("ffi.gta5.special"), func)
										? g_tunables.getBool(TUNAHASH("ffi_warning_required.gta5.special"))
										: g_tunables.getBool(TUNAHASH("ffi_warning_required.gta5"))
									: g_tunables.getBool(TUNAHASH("ffi_warning_required.other"));
			if (warning_required && !luaS_thisptr(L)->showed_os_warning)
			{
				luaL_error(L, "FFI call to this function is not allowed without first calling util.i_really_need_manual_access_to_process_apis");
			}
			luaS_push(L, soup::ffi::call(func, args));
			return 1;
		});
	}

	int lua_util_get_rtti_name(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, soup::RttiObject::fromInstance((void*)luaL_checkinteger(L, 1))->getTypeInfo()->getName());
			return 1;
		});
	}

	int lua_util_get_rtti_hierarchy(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, soup::RttiObject::fromInstance((void*)luaL_checkinteger(L, 1))->getHierarchyString());
			return 1;
		});
	}

	int lua_util_set_particle_fx_asset(lua_State* L)
	{
		*pointers::particle_fx_asset = luaS_checkHash(L, 1);
		return 0;
	}

	int lua_util_blip_handle_to_pointer(lua_State* L)
	{
		return luaS_returnPtrNilable(L, CMiniMapBlip::fromHandle((int)luaL_checkinteger(L, 1)));
	}

	int lua_util_get_blip_display(lua_State* L)
	{
		if (auto blip = CMiniMapBlip::fromHandle((int)luaL_checkinteger(L, 1)); blip && blip->m_active)
		{
			lua_pushinteger(L, blip->m_display_id);
		}
		else
		{
			lua_pushinteger(L, -1);
		}
		return 1;
	}

	int lua_util_teleport_2d(lua_State* L)
	{
		SOUP_IF_UNLIKELY (lua_gettop(L) >= 3
			&& LuaConfig::notify_bad_practice->m_on
			)
		{
			luaS_notice(L, "Too many arguments passed to util.teleport_2d");
		}
		TpUtil::teleport({ (float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2) });
		return 0;
	}

	int lua_util_is_interaction_menu_open(lua_State* L)
	{
		lua_pushboolean(L, Gui::isInteractionMenuOpen());
		return 1;
	}

	int lua_util_on_transition_finished(lua_State* L)
	{
		const auto thisptr = luaS_getThisPtrAndImplyKeepRunning(L);
		SOUP_IF_UNLIKELY (thisptr->transition_finished_callbacks.size() >= (size_t)g_tunables.getInt(TUNAHASH("lua transition_finished_callbacks limit"), 1000))
		{
			luaL_error(L, "This script has reached the limit for this resource");
		}
		const auto ref = luaS_checkFunction(L, 1);
		thisptr->transition_finished_callbacks.emplace_back(ref);
		lua_pushinteger(L, ref);
		return 1;
	}

	int lua_util_get_closest_hud_colour(lua_State* L)
	{
		int target_a = 255;
		if (lua_gettop(L) >= 4)
		{
			target_a = (int)luaL_checkinteger(L, 4);
		}
		lua_pushinteger(L, Util::get_closest_hud_colour((int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3), target_a));
		return 1;
	}

	int lua_util_is_soup_netintel_inited(lua_State* L)
	{
		lua_pushboolean(L, g_gui.inited_netIntel);
		return 1;
	}

	int lua_util_on_pad_shake(lua_State* L)
	{
		const auto thisptr = luaS_getThisPtrAndImplyKeepRunning(L);
		SOUP_IF_UNLIKELY (thisptr->pad_shake_callbacks.size() >= (size_t)g_tunables.getInt(TUNAHASH("lua pad_shake_callbacks limit"), 1000))
		{
			luaL_error(L, "This script has reached the limit for this resource");
		}
		const auto ref = luaS_checkFunction(L, 1);
		thisptr->pad_shake_callbacks.emplace_back(ref);
		lua_pushinteger(L, ref);
		return 1;
	}

	int lua_util_open_folder(lua_State* L)
	{
		std::filesystem::path path = (char8_t*)luaL_checkstring(L, 1);
		if (!std::filesystem::is_directory(path))
		{
			luaL_error(L, "util.open_folder may only be used on a folder");
		}
		auto wpath = path.wstring();
		g_gui.shellExecute(wpath.c_str());
		return 0;
	}

	int lua_util_set_nullptr_preference(lua_State* L)
	{
		const auto thisptr = luaS_scriptData(L);
		const bool prev = thisptr->isNilptrOptedIn();
		thisptr->nilptr = luaS_checkoptboolean(L, 1, true);
		luaS_push(L, prev);
		return 1;
	}

	int lua_util_get_tps(lua_State* L)
	{
		lua_pushinteger(L, Metrics::ticks_per_second);
		return 1;
	}

	int lua_util_get_session_code(lua_State* L)
	{
		bool applicable;
		auto code = CommandSessionCode::getSessionCodeOrStartRequest(applicable);
		luaS_push(L, applicable);
		if (code)
		{
			luaS_push(L, *code);
			return 2;
		}
		return 1;
	}

	int lua_util_stat_get_type(lua_State* L)
	{
		if (auto stat = g_comp_savedata.findStat(luaS_checkHash(L, 1)))
		{
			luaS_push(L, stat->GetType());
			return 1;
		}
		return 0;
	}

	int lua_util_stat_get_int64(lua_State* L)
	{
		if (auto stat = g_comp_savedata.findStat(luaS_checkHash(L, 1)))
		{
			luaS_push(L, stat->GetInt64());
			return 1;
		}
		return 0;
	}

	int lua_util_stat_set_int64(lua_State* L)
	{
		if (auto stat = g_comp_savedata.findStat(luaS_checkHash(L, 1)))
		{
			stat->SetInt64(luaL_checkinteger(L, 2));
		}
		return 0;
	}

	int lua_util_new_toast_config(lua_State* L)
	{
		auto thisptr = luaS_thisptr(L);
		if (thisptr->toast_configs.size() == 1000)
		{
			luaL_error(L, "This script has already allocated 1000 toast configs. This can't be intended behaviour.");
		}
		auto list = luaS_checkCommandParent(L, thisptr, 1);
		auto tc = new ToastConfig{};
		std::vector<CommandName> command_names_prefix{};
		if (lua_gettop(L) >= 2)
		{
			tc->fromToastFlags((toast_t)luaL_checkinteger(L, 2));
			if (lua_gettop(L) >= 3)
			{
				command_names_prefix = luaS_checkCommandNames(L, 3);
			}
		}
		tc->populateList(list, command_names_prefix);
		thisptr->toast_configs.emplace_back(tc);
		luaS_push(L, tc);
		return 1;
	}

	int lua_util_toast_config_get_flags(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, reinterpret_cast<ToastConfig*>(luaS_checkPointer(L, 1))->getToastFlags());
			return 1;
		});
	}

	int lua_util_get_model_info(lua_State* L)
	{
		luaS_pushPtr(L, g_hooking.getModelInfo(luaS_checkHash(L, 1)));
		return 1;
	}

	int lua_util_is_valid_file_name(lua_State* L)
	{
		lua_pushboolean(L, validate_file_name(StringUtils::utf8_to_utf16(pluto_checkstring(L, 1)), luaS_checkboolean(L, 2)));
		return 1;
	}

	int lua_util_rgb2hsv(lua_State* L)
	{
		DirectX::SimpleMath::Vector3 c((float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
		c = ColourUtil::rgb_to_hsv(c);
		luaS_push(L, c.x);
		luaS_push(L, c.y);
		luaS_push(L, c.z);
		return 3;
	}

	int lua_util_hsv2rgb(lua_State* L)
	{
		DirectX::SimpleMath::Vector3 c((float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
		c = ColourUtil::hsv_to_rgb(c);
		luaS_push(L, c.x);
		luaS_push(L, c.y);
		luaS_push(L, c.z);
		return 3;
	}

	int lua_util_calculate_luminance(lua_State* L)
	{
		DirectX::SimpleMath::Vector3 c((float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
		luaS_push(L, ColourUtil::luminance(c));
		return 1;
	}

	int lua_util_calculate_contrast(lua_State* L)
	{
		DirectX::SimpleMath::Vector3 a((float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
		DirectX::SimpleMath::Vector3 b((float)luaL_checknumber(L, 4), (float)luaL_checknumber(L, 5), (float)luaL_checknumber(L, 6));
		luaS_push(L, ColourUtil::contrast(a, b));
		return 1;
	}

	int lua_util_is_contrast_sufficient(lua_State* L)
	{
		DirectX::SimpleMath::Vector3 a((float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
		DirectX::SimpleMath::Vector3 b((float)luaL_checknumber(L, 4), (float)luaL_checknumber(L, 5), (float)luaL_checknumber(L, 6));
		luaS_push(L, ColourUtil::isContrastSufficient(a, b));
		return 1;
	}

	int lua_util_set_waypoint(lua_State* L)
	{
		Util::setWaypoint3d(luaS_checkV3<v3>(L, 1));
		return 1;
	}

	int lua_util_set_cam_quaternion(lua_State* L)
	{
		if (auto cam = (*pointers::cam_pool)->GetAt((int32_t)luaL_checkinteger(L, 1)))
		{
			soup::Quaternion quat((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4), (float)luaL_checknumber(L, 5));
			quat.toMatrix(cam->frame.view);
			cam->update_flags |= 0x80;
		}
		return 0;
	}

	int lua_util_get_screen_coord_from_world_coord_no_adjustment(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushboolean(L, Util::GET_SCREEN_COORD_FROM_WORLD_COORD_NO_ADJUSTMENT(
				(float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3),
				(float*)luaS_checkPointer(L, 4), (float*)luaS_checkPointer(L, 5)
			));
			return 1;
		});
	}

	int lua_util_utf8_to_utf16(lua_State* L)
	{
		size_t size;
		const char* data = luaL_checklstring(L, 1, &size);
		std::string utf8((const char*)data, size / sizeof(char)); // byte length -> character length
		auto utf16 = StringUtils::utf8_to_utf16(utf8);
		lua_pushlstring(L, (const char*)utf16.data(), utf16.size() * sizeof(wchar_t)); // character length -> byte length
		return 1;
	}

	int lua_util_utf16_to_utf8(lua_State* L)
	{
		size_t size;
		const char* data = luaL_checklstring(L, 1, &size);
		std::wstring utf16((const wchar_t*)data, size / sizeof(wchar_t)); // byte length -> character length
		auto utf8 = StringUtils::utf16_to_utf8(utf16);
		lua_pushlstring(L, (const char*)utf8.data(), utf8.size() * sizeof(char)); // character length -> byte length
		return 1;
	}

	int lua_util_get_gps_route(lua_State* L)
	{
		const auto slot = luaL_checkinteger(L, 1);
		SOUP_IF_UNLIKELY (slot < 0 || slot >= 3)
		{
			luaL_argerror(L, 1, "value out of range");
		}
		lua_newtable(L);
		for (int i = 0; i != pointers::gps_slots[slot].m_NumNodes; ++i)
		{
			lua_pushinteger(L, i + 1);
			lua_newtable(L);
			{
				lua_pushstring(L, "x");
				luaS_push(L, pointers::gps_slots[slot].m_NodeCoordinates[i].x);
				lua_settable(L, -3);
				lua_pushstring(L, "y");
				luaS_push(L, pointers::gps_slots[slot].m_NodeCoordinates[i].y);
				lua_settable(L, -3);
				lua_pushstring(L, "z");
				luaS_push(L, pointers::gps_slots[slot].m_NodeCoordinates[i].z);
				lua_settable(L, -3);
				lua_pushstring(L, "junction");
				luaS_push(L, *reinterpret_cast<int*>(&pointers::gps_slots[slot].m_NodeCoordinates[i].w) == GNI_IGNORE_FOR_NAV);
				lua_settable(L, -3);
			}
			lua_settable(L, -3);
		}
		luaS_push(L, pointers::gps_slots[slot].isPartialRoute());
		return 2;
	}

	int lua_util_sc_is_blocked(lua_State* L)
	{
		luaS_push(L, ScUtil::isBlocked(luaL_checkinteger(L, 1)));
		return 1;
	}

	int lua_util_sc_block(lua_State* L)
	{
		ScUtil::block(luaL_checkinteger(L, 1));
		return 0;
	}

	int lua_util_sc_unblock(lua_State* L)
	{
		ScUtil::unblock(luaL_checkinteger(L, 1));
		return 0;
	}

	int lua_util_get_tunable_int(lua_State* L) // undocumented
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			if (auto opt = g_tunables.getOptionalInt((uint32_t)luaL_checkinteger(L, 1)))
			{
				luaS_push(L, opt.value());
				return 1;
			}
			return 0;
		});
	}

	int lua_util_get_tunable_bool(lua_State* L) // undocumented
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			if (auto opt = g_tunables.getOptionalBool((uint32_t)luaL_checkinteger(L, 1)))
			{
				luaS_push(L, opt.value());
				return 1;
			}
			return 0;
		});
	}

	int lua_util_ui3dscene_set_element_2d_position(lua_State* L)
	{
		const auto slot = (int)luaL_checkinteger(L, 1);
		const auto x = (float)luaL_checknumber(L, 2);
		const auto y = (float)luaL_checknumber(L, 3);

		if (pointers::ui3d_draw_mgr
			&& (*pointers::ui3d_draw_mgr)->current_preset
			&& slot >= 0 && slot < UI_MAX_SCENE_PRESET_ELEMENTS
			)
		{
			(*pointers::ui3d_draw_mgr)->current_preset->elements[slot].screen_x = x;
			(*pointers::ui3d_draw_mgr)->current_preset->elements[slot].screen_y = y;
		}

		return 0;
	}

	int lua_util_ui3dscene_set_element_2d_size(lua_State* L)
	{
		const auto slot = (int)luaL_checkinteger(L, 1);
		const auto width = (float)luaL_checknumber(L, 2);
		const auto height = (float)luaL_checknumber(L, 3);

		if (pointers::ui3d_draw_mgr
			&& (*pointers::ui3d_draw_mgr)->current_preset
			&& slot >= 0 && slot < UI_MAX_SCENE_PRESET_ELEMENTS
			)
		{
			(*pointers::ui3d_draw_mgr)->current_preset->elements[slot].screen_width = width;
			(*pointers::ui3d_draw_mgr)->current_preset->elements[slot].screen_height = height;
		}

		return 0;
	}

	int lua_util_ui3dscene_set_element_background_colour(lua_State* L)
	{
		const auto slot = (int)luaL_checkinteger(L, 1);
		const auto colour = luaS_checkColour(L, 2);

		if (pointers::ui3d_draw_mgr
			&& (*pointers::ui3d_draw_mgr)->current_preset
			&& slot >= 0 && slot < UI_MAX_SCENE_PRESET_ELEMENTS
			)
		{
			(*pointers::ui3d_draw_mgr)->current_preset->elements[slot].bg_colour.r = static_cast<uint8_t>(colour.R() * 255.0f);
			(*pointers::ui3d_draw_mgr)->current_preset->elements[slot].bg_colour.g = static_cast<uint8_t>(colour.G() * 255.0f);
			(*pointers::ui3d_draw_mgr)->current_preset->elements[slot].bg_colour.b = static_cast<uint8_t>(colour.B() * 255.0f);
			(*pointers::ui3d_draw_mgr)->current_preset->elements[slot].bg_colour.a = static_cast<uint8_t>(colour.A() * 255.0f);
		}

		return 0;
	}

	int lua_util_ip_from_string(lua_State* L)
	{
		soup::IpAddr addr;
		SOUP_IF_LIKELY (addr.fromString(luaL_checkstring(L, 1)))
		{
			luaS_push(L, addr.getV4NativeEndian());
			return 1;
		}
		return 0;
	}

	int lua_util_ip_to_string(lua_State* L)
	{
		luaS_push(L, soup::IpAddr(soup::native_u32_t(static_cast<uint32_t>(luaL_checkinteger(L, 1)))).toString4());
		return 1;
	}

	int lua_util_ip_get_as(lua_State* L)
	{
		if (g_gui.inited_netIntel)
		{
			if (auto as = g_gui.getNetIntel().getAsByIpv4(soup::native_u32_t(static_cast<uint32_t>(luaL_checkinteger(L, 1)))))
			{
				lua_newtable(L);
				{
					luaS_push(L, as->number);
					lua_setfield(L, -2, "number");
				}
				{
					lua_pushstring(L, as->handle);
					lua_setfield(L, -2, "handle");
				}
				{
					lua_pushstring(L, as->name);
					lua_setfield(L, -2, "name");
				}
				return 1;
			}
		}
		return 0;
	}

	int lua_util_ip_get_location(lua_State* L)
	{
		if (g_gui.inited_netIntel)
		{
			if (auto loc = g_gui.getNetIntel().getLocationByIpv4(soup::native_u32_t(static_cast<uint32_t>(luaL_checkinteger(L, 1)))))
			{
				lua_newtable(L);
				{
					lua_pushstring(L, loc->country_code.c_str());
					lua_setfield(L, -2, "country_code");
				}
				{
					lua_pushstring(L, loc->state);
					lua_setfield(L, -2, "state");
				}
				{
					lua_pushstring(L, loc->city);
					lua_setfield(L, -2, "city");
				}
				return 1;
			}
		}
		return 0;
	}

	int lua_v3_call(lua_State* L)
	{
		lua_remove(L, 1);
		return lua_v3_new(L);
	}

	int lua_v3_new(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			const auto args = lua_gettop(L);
			auto v = luaS_newV3(L);
			if (args == 0)
			{
				v->x = 0.0f;
				v->y = 0.0f;
				v->z = 0.0f;
			}
			else if (args == 1)
			{
				*v = luaS_checkV3<rage::scrVector3>(L, 1);
			}
			else
			{
				v->x = (float)luaL_checknumber(L, 1);
				v->y = (float)luaL_checknumber(L, 2);
				v->z = (float)luaL_checknumber(L, 3);
			}
			return 1;
		});
	}

	int lua_v3_free(lua_State* L)
	{
		luaS_deprecated(L, "v3.free"); // since 0.83
		return 0;
	}

	int lua_v3_get(lua_State* L)
	{
		auto inst = reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1));
		lua_pushnumber(L, inst->x);
		lua_pushnumber(L, inst->y);
		lua_pushnumber(L, inst->z);
		return 3;
	}

	int lua_v3_getX(lua_State* L)
	{
		lua_pushnumber(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->x);
		return 1;
	}

	int lua_v3_getY(lua_State* L)
	{
		lua_pushnumber(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->y);
		return 1;
	}

	int lua_v3_getZ(lua_State* L)
	{
		lua_pushnumber(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->z);
		return 1;
	}

	int lua_v3_getHeading(lua_State* L)
	{
		lua_pushnumber(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->heading());
		return 1;
	}

	int lua_v3_set(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			auto inst = reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1));
			if (lua_gettop(L) == 2)
			{
				*inst = luaS_checkV3<rage::scrVector3>(L, 2);
			}
			else
			{
				inst->x = (float)luaL_checknumber(L, 2);
				inst->y = (float)luaL_checknumber(L, 3);
				inst->z = (float)luaL_checknumber(L, 4);
			}
			lua_pushvalue(L, 1);
			return 1;
		});
	}

	int lua_v3_setX(lua_State* L)
	{
		reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->x = (float)luaL_checknumber(L, 2);
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_setY(lua_State* L)
	{
		reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->y = (float)luaL_checknumber(L, 2);
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_setZ(lua_State* L)
	{
		reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->z = (float)luaL_checknumber(L, 2);
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_reset(lua_State* L)
	{
		reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->reset();
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_add(lua_State* L)
	{
		*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) += *reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 2));
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_sub(lua_State* L)
	{
		*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) -= *reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 2));
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_mul(lua_State* L)
	{
		*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) *= (float)luaL_checknumber(L, 2);
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_div(lua_State* L)
	{
		*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) /= (float)luaL_checknumber(L, 2);
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_addNew(lua_State* L)
	{
		*luaS_newV3(L) = (*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) + *reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 2)));
		return 1;
	}

	int lua_v3_subNew(lua_State* L)
	{
		*luaS_newV3(L) = (*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) - *reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 2)));
		return 1;
	}

	int lua_v3_mulNew(lua_State* L)
	{
		*luaS_newV3(L) = (*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) * (float)luaL_checknumber(L, 2));
		return 1;
	}

	int lua_v3_divNew(lua_State* L)
	{
		*luaS_newV3(L) = (*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) / (float)luaL_checknumber(L, 2));
		return 1;
	}

	int lua_v3_eq(lua_State* L)
	{
		luaS_push(L, *reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)) == *reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1)));
		return 1;
	}

	int lua_v3_magnitude(lua_State* L)
	{
		luaS_push(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->magnitude());
		return 1;
	}

	int lua_v3_distance(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->distance(*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 2))));
			return 1;
		});
	}

	int lua_v3_abs(lua_State* L)
	{
		auto inst = reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1));
		*inst = inst->abs();
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_min(lua_State* L)
	{
		luaS_push(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->min());
		return 1;
	}

	int lua_v3_max(lua_State* L)
	{
		luaS_push(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->max());
		return 1;
	}

	int lua_v3_dot(lua_State* L)
	{
		luaS_push(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->dot(*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 2))));
		return 1;
	}

	int lua_v3_normalise(lua_State* L)
	{
		reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->normalise();
		lua_pushvalue(L, 1);
		return 1;
	}

	int lua_v3_crossProduct(lua_State* L)
	{
		*luaS_newV3(L) = reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->crossProduct(*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 2)));
		return 1;
	}

	int lua_v3_toRot(lua_State* L)
	{
		*luaS_newV3(L) = reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->toRot();
		return 1;
	}

	int lua_v3_lookAt(lua_State* L)
	{
		*luaS_newV3(L) = reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->lookAt(*reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 2)));
		return 1;
	}

	int lua_v3_toDir(lua_State* L)
	{
		*luaS_newV3(L) = reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->toDir();
		return 1;
	}

	int lua_v3_toString(lua_State* L)
	{
		luaS_push(L, reinterpret_cast<rage::scrVector3*>(luaS_checkPointer(L, 1))->operator std::string());
		return 1;
	}

	int lua_lang_get_current(lua_State* L)
	{
		lua_pushstring(L, Lang::idToApiCode(Lang::active_id));
		return 1;
	}

	int lua_lang_is_code_valid(lua_State* L)
	{
		lua_pushboolean(L, Lang::apiCodeToId(luaL_checkstring(L, 1)) != LANG_SIZE);
		return 1;
	}

	int lua_lang_get_code_for_soup(lua_State* L)
	{
		lua_pushstring(L, Lang::idToCodeForSoup(Lang::apiCodeToId(luaL_checkstring(L, 1))));
		return 1;
	}

	int lua_lang_is_automatically_translated(lua_State* L)
	{
		lua_pushboolean(L, Lang::apiCodeToId(luaL_checkstring(L, 1)) >= LANG_REAL_END);
		return 1;
	}

	int lua_lang_is_english(lua_State* L)
	{
		const auto lang_id = Lang::apiCodeToId(luaL_checkstring(L, 1));
		lua_pushboolean(L, lang_id == LANG_EN || lang_id >= LANG_SEX);
		return 1;
	}

	int lua_lang_register(lua_State* L)
	{
		const auto value = StringUtils::utf8_to_utf16(luaL_checkstring(L, 1));

		uint32_t key;
		do
		{
			key = soup::rand.t<uint32_t>(1, -1);
		} while (Lang::doesLabelWithKeyExist(key));

		luaS_thisptr(L)->lang_labels.emplace(key);
		for (lang_t i = LANG_EN; i != LANG_REAL_END; ++i)
		{
			Lang::id_to_map(i)->emplace(key, value);
		}

		luaS_push(L, key);
		return 1;
	}

	static std::unordered_map<hash_t, std::wstring>* current_translate = nullptr;

	[[nodiscard]] static std::unordered_map<hash_t, std::wstring>* luaS_checkLangMap(lua_State* L, int i)
	{
		auto lang_id = Lang::apiCodeToId(luaL_checkstring(L, i));
		if (lang_id >= LANG_REAL_END)
		{
			if (lang_id == LANG_SIZE)
			{
				luaL_error(L, "Invalid language code");
			}
			luaL_error(L, "Provided language has no data");
		}
		return Lang::id_to_map(lang_id);
	}

	int lua_lang_set_translate(lua_State* L)
	{
		current_translate = luaS_checkLangMap(L, 1);
		return 0;
	}

	int lua_lang_translate(lua_State* L)
	{
		if (current_translate == nullptr)
		{
			luaL_error(L, "lang.translate called before lang.set_translate");
		}
		const auto key = (hash_t)luaL_checkinteger(L, 1);
		if (key == 0)
		{
			luaL_error(L, "lang.translate called with label = 0.");
		}
		auto e = current_translate->find(key);
		if (e == current_translate->end()
			|| !luaS_thisptr(L)->lang_labels.contains(key)
			)
		{
			luaL_error(L, "lang.translate received a label that your script has not registered. Make sure you're using the return value of lang.register or lang.find_registered.");
		}
		e->second = StringUtils::utf8_to_utf16(luaL_checkstring(L, 2));
		return 0;
	}

	int lua_lang_find(lua_State* L)
	{
		const auto value = StringUtils::utf8_to_utf16(luaL_checkstring(L, 1));

		std::unordered_map<hash_t, std::wstring>* map = Lang::id_to_map(LANG_EN);
		if (lua_gettop(L) >= 2)
		{
			map = luaS_checkLangMap(L, 2);
		}

		hash_t key = 0;
		for (const auto& entry : *map)
		{
			if (entry.second == value)
			{
				key = entry.first;
				break;
			}
		}
		luaS_push(L, key);
		return 1;
	}

	int lua_lang_find_builtin(lua_State* L)
	{
		const auto value = StringUtils::utf8_to_utf16(luaL_checkstring(L, 1));

		std::unordered_map<hash_t, std::wstring>* map = Lang::id_to_map(LANG_EN);
		if (lua_gettop(L) >= 2)
		{
			map = luaS_checkLangMap(L, 2);
		}

		hash_t key = 0;
		for (const auto& entry : *map)
		{
			if (entry.second == value
				&& Lang::getStandKeys().contains(entry.first)
				)
			{
				key = entry.first;
				break;
			}
		}
		luaS_push(L, key);
		return 1;
	}

	int lua_lang_find_registered(lua_State* L)
	{
		const auto value = StringUtils::utf8_to_utf16(luaL_checkstring(L, 1));

		std::unordered_map<hash_t, std::wstring>* map = Lang::id_to_map(LANG_EN);
		if (lua_gettop(L) >= 2)
		{
			map = luaS_checkLangMap(L, 2);
		}

		hash_t key = 0;
		for (const auto& k : luaS_thisptr(L)->lang_labels)
		{
			if (map->at(k) == value)
			{
				key = k;
				break;
			}
		}
		luaS_push(L, key);
		return 1;
	}

	int lua_lang_get_string(lua_State* L)
	{
		Label label = luaS_checkLabel(L, 1);
		if (label.isLiteral())
		{
			luaS_push(L, label.literal_str);
		}
		else
		{
			lang_t lang_id = LANG_EN;
			if (lua_gettop(L) >= 2)
			{
				lang_id = Lang::apiCodeToId(luaL_checkstring(L, 2));
			}
			auto data = Lang::id_to_data(lang_id);
			luaS_push(L, data->get(label.getLocalisationHash()));
		}
		return 1;
	}

	int lua_lang_is_mine(lua_State* L)
	{
		lua_pushboolean(L, luaS_thisptr(L)->ownsLabel((hash_t)luaL_checkinteger(L, 1)));
		return 1;
	}

	int lua_lang_is_builtin(lua_State* L)
	{
		lua_pushboolean(L, Lang::getStandKeys().contains((hash_t)luaL_checkinteger(L, 1)));
		return 1;
	}

	int lua_filesystem_appdata_dir(lua_State* L)
	{
		luaS_push(L, get_roaming_path());
		return 1;
	}

	int lua_filesystem_stand_dir(lua_State* L)
	{
		luaS_push(L, get_appdata_path());
		return 1;
	}

	int lua_filesystem_scripts_dir(lua_State* L)
	{
		luaS_push(L, get_appdata_path("Lua Scripts\\"));
		return 1;
	}

	int lua_filesystem_resources_dir(lua_State* L)
	{
		luaS_push(L, get_appdata_path("Lua Scripts\\resources\\"));
		return 1;
	}

	int lua_filesystem_store_dir(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			auto path = get_appdata_path("Lua Scripts\\store\\");
			if (!std::filesystem::exists(path))
			{
				std::filesystem::create_directory(path);
			}
			luaS_push(L, path);
			return 1;
		});
	}

	int lua_async_http_have_access(lua_State* L)
	{
		luaS_push(L, !luaS_thisptr(L)->restriction_disable_internet_access);
		return 1;
	}

	int lua_async_http_init(lua_State* L)
	{
		if (async_http_builder != nullptr)
		{
			delete async_http_builder;
			async_http_builder = nullptr;
		}
		// Try to process host as a full Uri
		std::string host = luaL_checkstring(L, 1);
		if (host.find("//") == std::string::npos)
		{
			host.insert(0, "//");
		}
		soup::Uri uri(host);
		if (uri.getRequestPath().empty()) // No path?
		{
			if (lua_gettop(L) >= 2 && lua_type(L, 2) != LUA_TNIL) // Path parameter given?
			{
				// Add path parameter to Uri
				std::string path = luaL_checkstring(L, 2);
				if (path.c_str()[0] != '/')
				{
					path.insert(0, 1, '/');
				}
				host.append(path);
				uri = soup::Uri(std::move(host));
			}
		}
		else if (LuaConfig::notify_bad_practice->m_on
			&& lua_type(L, 2) == LUA_TSTRING
			&& lua_tostring(L, 2)[0] != '\0'
			)
		{
			luaS_notice(L, "async_http.init provided with a path, but ignored in favour of host parameter which looks like a URI");
		}
		async_http_builder = new HttpRequestBuilder(uri);
		if (lua_gettop(L) >= 3)
		{
			auto succ_callback = luaS_checkFunction(L, 3);
			int fail_callback = LUA_NOREF;
			if (lua_gettop(L) >= 4)
			{
				fail_callback = luaS_checkFunction(L, 4);
			}
			auto weak_ref = luaS_getThisPtrAndImplyKeepRunning(L)->getRunningWeakref();
			async_http_builder->setResponseCallback([weak_ref, succ_callback, fail_callback](soup::HttpResponse&& resp) mutable
			{
				FiberPool::queueJob([weak_ref{ std::move(weak_ref) }, succ_callback, fail_callback, resp{ std::move(resp) }]() mutable
				{
					__try
					{
						if (auto script = weak_ref.getPointer())
						{
							if (fail_callback != LUA_NOREF)
							{
								luaS_releaseReference(script->L, fail_callback);
							}
							luaS_invoke_void(script->L, succ_callback, resp.body, resp.header_fields, resp.status_code);
							luaS_releaseReference(script->L, succ_callback);
						}
					}
					__EXCEPTIONAL_LUA()
					{
					}
				});
			});
			if (fail_callback != LUA_NOREF)
			{
				async_http_builder->setFailCallback([weak_ref, succ_callback, fail_callback](std::string&& reason) mutable
				{
					FiberPool::queueJob([weak_ref{ std::move(weak_ref) }, succ_callback, fail_callback, reason{ std::move(reason) }]() mutable
					{
						__try
						{
							if (auto script = weak_ref.getPointer())
							{
								luaS_releaseReference(script->L, succ_callback);
								luaS_invoke_void(script->L, fail_callback, reason);
								luaS_releaseReference(script->L, fail_callback);
							}
						}
						__EXCEPTIONAL_LUA()
						{
						}
					});
				});
			}
		}
		return 0;
	}

	int lua_async_http_dispatch(lua_State* L)
	{
		if (async_http_builder == nullptr)
		{
			luaL_error(L, "async_http.init needs to be called first");
		}

		if (luaS_thisptr(L)->restriction_disable_internet_access)
		{
			async_http_builder->fail_callback("Internet Access Disabled");
		}
		else
		{
			async_http_builder->dispatch();
		}

		delete async_http_builder;
		async_http_builder = nullptr;

		return 0;
	}

	int lua_async_http_set_post(lua_State* L)
	{
		if (async_http_builder == nullptr)
		{
			luaL_error(L, "async_http.init needs to be called first");
		}

		async_http_builder->addHeader("Content-Type", luaL_checkstring(L, 1));
		async_http_builder->setPayload(luaL_checkstring(L, 2));

		return 0;
	}

	int lua_async_http_add_header(lua_State* L)
	{
		if (async_http_builder == nullptr)
		{
			luaL_error(L, "async_http.init needs to be called first");
		}

		async_http_builder->addHeader(luaL_checkstring(L, 1), luaL_checkstring(L, 2));

		return 0;
	}

	int lua_async_http_set_method(lua_State* L)
	{
		if (async_http_builder == nullptr)
		{
			luaL_error(L, "async_http.init needs to be called first");
		}

		async_http_builder->method = luaL_checkstring(L, 1);
		soup::string::upper(async_http_builder->method);

		return 0;
	}

	int lua_async_http_prefer_ipv6(lua_State* L)
	{
		if (async_http_builder == nullptr)
		{
			luaL_error(L, "async_http.init needs to be called first");
		}

		async_http_builder->prefer_ipv6 = true;

		return 0;
	}
}
