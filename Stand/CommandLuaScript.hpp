#pragma once

#include "CommandListConcealer.hpp"

#include <fmt/format.h>

#include <lua.h>
#undef G
#undef isLua

#include "fwddecl.hpp"

#include "CommandListFolderItemData.hpp"
#include "Exceptional.hpp"
#include "LuaResourceMap.hpp"
#include "LuaScriptData.hpp"
#include "luaS_push.hpp"
#include "RateLimit.hpp"

#define LUA_MEMORY_DEBUG false
#define LUA_MEMORY_DEBUG_SIZE 0x1000

namespace Stand
{
#if LUA_MEMORY_DEBUG
	inline std::vector<std::pair<void*, size_t>> allocated_ud{};
#endif

#pragma pack(push, 1)
	class CommandLuaScript : public CommandListFolderItem<CommandListConcealer, CommandLuaScript>
	{
	private:
		bool viewed_with_repo_done = false;
	public:
		inline static std::vector<CommandLuaScript*> all_loaded = {};
		inline static bool hold_up_save = false;

		inline static uint8_t event_handlers_ref = 0;
		std::vector<int> chat_message_callbacks{};
		std::vector<int> pad_shake_callbacks{};
		std::vector<int> player_join_callbacks{};
		std::vector<int> player_leave_callbacks{};
		std::vector<int> flow_event_done_callbacks{};
		std::vector<int> transition_finished_callbacks{};
		static void registerEventHandlers();
		void unregisterEventHandlers();
		void clearEventHandlers();

		CommandListFolderItemData folder_item_data;

		hash_t last_code_hash = 0;
		uint8_t code_changes_made = 0;

		bool run_queued = false;
		Script* script = nullptr;
		bool running = false;
		bool invoke_no_fiber = false;
		lua_State* L = nullptr;
		bool initial_load = false;
		bool iterating_all_commands = false;
		std::vector<soup::WeakRef<Command>> all_commands = {};
		bool keep_running_implied;

		std::string warn_buf;

		uint8_t busy = 0;
		CommandDivider* busy_cmd;

		std::vector<int> stop_handlers{};

		uint8_t first_ticks;
		[[nodiscard]] bool isFirstTick() const noexcept
		{
			return first_ticks == 100;
		}

		LuaResourceMap<EphemeralBackgroundBlur> bgblurs = {};
		bool enabled_ar_spinner = false;
		bool enabled_graceland = false;
		std::vector<ToastConfig*> toast_configs{};

		bool block_send_message = false;
		RateLimit message_ratelimit{ 10000, 0 };
		[[nodiscard]] bool canSendChatMessage(int num_recipients) noexcept;

		LuaScriptData script_data;

		explicit CommandLuaScript(CommandList* const parent, const std::wstring& menu_name, CommandListFolderItemData&& folder_item_data);
		~CommandLuaScript();

		[[nodiscard]] bool isInstalledViaRepo() const;

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;

		[[nodiscard]] std::string getCode() const;

		std::string getState() const final;
		std::string getDefaultState() const final;
		void setState(Click& click, const std::string& state) final;
		void applyDefaultState() final;
		void shutdownFromState();
		[[nodiscard]] bool isEnabledInActiveProfile() const;

		[[nodiscard]] static bool isObfuscatedCode(const std::string& code, bool is_main_file);
		static void onObfuscatedCodeDetected();
		void run(bool notify, std::string&& code);
		void addHeadCommands();
		void stop(bool hot);
		void applyDefaultState(bool hot);
		void dispatchOnStop();
		void clearCommands();

		bool can_continue;
		bool is_silent_stop;
		bool is_instant_stop;
		bool off_thread = false;
		void requestStop(bool silent, bool instant);

		std::vector<CommandPhysical*> need_state_commands = {};
		void applyCommandStates();

		void preAttachOfOwnedCommand(Command* command);
		void preAttachOfOwnedCommand(CommandPhysical* command);
		void preReattachOfOwnedCommand(Command* command);
		void preReattachOfOwnedCommand(CommandPhysical* command);
		void preDetachOfOwnedCommand(Command* cmd);
		void deleteCommand(Command* command, bool keep_in_parent = false);
		size_t collectGarbage();

		[[nodiscard]] bool ownsCommand(Command* command) const noexcept;
		//[[nodiscard]] bool wasCreatedThisTick(CommandPhysical* command) const noexcept;

		void devNoticeNoCheck(std::string message) const;

		void disableArSpinner();
		void disableGracefulLanding();

		void enableBusy();
		void disableBusy();

		soup::TransientToken running_tt;
		[[nodiscard]] soup::WeakRef<CommandLuaScript> getRunningWeakref();

		std::unordered_set<hash_t> lang_labels{};
		[[nodiscard]] bool ownsLabel(hash_t key) const;

		bool showed_os_warning;

		bool compat_2take1 = false;
		bool compat_kiddions = false;

		bool restriction_force_silent_start = false;
		bool restriction_force_silent_stop = false;
		bool restriction_disable_internet_access = true;
		void populateStopped();
		void populateRestrictions();

		[[nodiscard]] static CommandLuaScript* fromFiber(Script* fiber);
	};
#pragma pack(pop)

	extern int luaS_panic_handler(lua_State* L);
	extern void luaS_openMenu(lua_State* L);
	extern void luaS_openEntities(lua_State* L);
	extern void luaS_openUtil(lua_State* L);
	extern void luaS_openFilesystem(lua_State* L);
	extern void luaS_openSoupLuaBindings(lua_State* L);
	extern void luaS_warnf(void* ud, const char* msg, int tocont);
	[[nodiscard]] extern CommandLuaScript* luaS_thisptr(lua_State* L);
	[[nodiscard]] extern CommandLuaScript* luaS_getThisPtrAndImplyKeepRunning(lua_State* L);
	[[nodiscard]] extern LuaScriptData* luaS_scriptData(lua_State* L);
	extern void luaS_notice(lua_State* L, std::string message);
	extern void luaS_deprecated(lua_State* L, const char* func_name);
	extern void luaS_deprecatedCustom(lua_State* L, const std::string& message);
	extern float luaS_checkTableNumber(lua_State* L, int i, const char* key);
	extern DirectX::SimpleMath::Color luaS_checkColour(lua_State* L, int& i);
	extern DirectX::SimpleMath::Color luaS_checkColour(lua_State* L, int&& i);
	extern std::unordered_map<std::string, std::string> luaS_check_map_string_string(lua_State* L, int i);
	extern std::vector<std::string> luaS_checkStringArray(lua_State* L, int i);
	extern std::vector<std::pair<long long, std::string>> luaS_checkStringArrayWithIndecies(lua_State* L, int i);
	extern std::vector<std::pair<long long, Label>> luaS_checkLabelArrayWithIndecies(lua_State* L, int i);
	extern std::vector<CommandName> luaS_checkCommandNames(lua_State* L, int i);

	template <typename T, SOUP_RESTRICT(std::is_pointer_v<T>)>
	[[nodiscard]] inline T luaS_check(lua_State* L, int i)
	{
		return reinterpret_cast<T>(luaL_checkinteger(L, i));
	}

	template <typename T>
	[[nodiscard]] inline T luaS_checkV3(lua_State* L, int i)
	{
		if (lua_gettop(L) >= i)
		{
			switch (lua_type(L, i))
			{
			case LUA_TTABLE:
				return T(luaS_checkTableNumber(L, i, "x"), luaS_checkTableNumber(L, i, "y"), luaS_checkTableNumber(L, i, "z"));

			case LUA_TNUMBER:
				if (lua_isinteger(L, i))
				{
					auto inst = reinterpret_cast<rage::scrVector3*>(lua_tointeger(L, i));
					return T(inst->x, inst->y, inst->z);
				}
				break;

			case LUA_TUSERDATA:
				{
					auto inst = reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, i));
					return T(inst->x, inst->y, inst->z);
				}
				break;
			}
		}
		luaL_error(L, std::string("Expected parameter ").append(fmt::to_string(i)).append(" to be a Vector3 (table or v3 instance)").c_str());
	}

	extern void luaS_pushfunction(lua_State* L, int ref);
	[[nodiscard]] extern int luaS_checkFunction(lua_State* L, int i);
	[[nodiscard]] extern int luaS_checkOptFunction(lua_State* L, int i);
	[[nodiscard]] extern int luaS_refFunction(lua_State* L, int i);
	extern void luaS_releaseReference(lua_State* L, int ref);
	extern void luaS_releaseReferences(lua_State* L, const std::vector<int>& refs);

	[[nodiscard]] extern soup::WeakRef<Command>* luaS_checkCommandRef(lua_State* L, int i);
	[[nodiscard]] extern std::unique_ptr<Command>* luaS_checkCommandUniqPtr(lua_State* L, int i);
	[[nodiscard]] extern Command* luaS_checkCommand(lua_State* L, int i, bool can_be_detached = false);
	[[nodiscard]] extern CommandPhysical* luaS_checkCommandPhysical(lua_State* L, int i, bool can_be_detached = false);
	[[nodiscard]] extern CommandList* luaS_checkCommandList(lua_State* L, int i, bool can_be_detached = false);
	[[nodiscard]] extern CommandList* luaS_checkCommandParent(lua_State* L, CommandLuaScript* thisptr, int i);
	[[nodiscard]] extern CommandList* luaS_checkCommandParentNoShadow(lua_State* L, CommandLuaScript* thisptr, int i);
	extern void luaS_checkCommandSuitableAsParent(lua_State* L, CommandLuaScript* thisptr, CommandList* list);
	[[noreturn]] extern void luaS_commandNotApplicable(lua_State* L);
	[[nodiscard]] extern Command* luaS_checkMutableCommand(lua_State* L, CommandLuaScript* thisptr, int i);
	[[nodiscard]] extern CommandPhysical* luaS_checkMutableCommandPhysical(lua_State* L, CommandLuaScript* thisptr, int i);
	[[nodiscard]] extern CommandPhysical* luaS_checkMutableOrDetachedCommand(lua_State* L, CommandLuaScript* thisptr, int i);
	[[nodiscard]] extern CommandLuaData* luaS_checkCommandLuaData(lua_State* L, int i);
	extern void luaS_pushCommandRef(lua_State* L, Command* cmd);
	extern void luaS_pushCommandRef(lua_State* L, soup::WeakRef<Command> ref);
	extern void luaS_pushCommandUniqPtr(lua_State* L, std::unique_ptr<Command>&& cmd);
	[[nodiscard]] extern int luaS_returnnewcommand(lua_State* L, CommandLuaScript* thisptr, Command* command);
	[[nodiscard]] extern bool luaS_checkboolean(lua_State* L, int i);
	[[nodiscard]] extern bool luaS_checkoptboolean(lua_State* L, int i, bool d);
	[[nodiscard]] extern hash_t luaS_checkHash(lua_State* L, int i);
	[[nodiscard]] extern std::string luaS_checkStringCastable(lua_State* L, int i);
	[[nodiscard]] extern Label luaS_checkLabel(lua_State* L, int i);
	[[nodiscard]] extern void* luaS_checkuserdata(lua_State* L, int i);
	[[nodiscard]] extern void* luaS_checkPointer(lua_State* L, int i);
	[[nodiscard]] extern bool luaS_canContinue(lua_State* L);
	[[nodiscard]] extern bool luaS_checkSilent(lua_State* L, const char* func);
	extern void luaS_pushPlayer(lua_State* L, AbstractPlayer p);
	[[nodiscard]] extern bool luaS_isV3Instance(lua_State* L, int i);
	extern void luaS_awaitThreadCompletion(lua_State* L, const soup::Thread& t);
	extern void luaS_awaitScript(lua_State* L);

	extern void luaS_push_mem_metafuncs(lua_State* L);
	extern void luaS_set_mem_metatable(lua_State* L);
	extern rage::scrVector3* luaS_newV3(lua_State* L);

	inline static std::string exception_name{};

	inline long onLowLevelException(ExceptionData* data) noexcept
	{
		exception_name = Exceptional::getExceptionName(data);
		if (data->exp->ExceptionRecord->ExceptionCode != 0xE06D7363)
		{
			Exceptional::report(exception_name, {}, data);
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}

	inline int luaS_tryCatch(lua_State* L, int f(lua_State* L))
	{
		__try
		{
			return f(L);
		}
		__EXCEPTIONAL_CUSTOM_LUA(&onLowLevelException)
		{
		}
		luaL_error(L, exception_name.c_str());
	}

	template <typename T>
	inline T luaS_tryCatch(lua_State* L, std::function<T()>&& f)
	{
		__try
		{
			return f();
		}
		__EXCEPTIONAL_CUSTOM_LUA(&onLowLevelException)
		{
		}
		luaL_error(L, exception_name.c_str());
	}

	template <typename...Args>
	inline void luaS_invoke_void(lua_State* L, int ref, const Args&...args)
	{
		__try
		{
			luaS_awaitScript(L);
			lua_getglobal(L, "internal_coroutine_create_hsdkbejsis");
			luaS_pushfunction(L, ref);
			(luaS_push(L, args), ...);
			lua_call(L, sizeof...(args) + 1, 0);
		}
		__EXCEPTIONAL_LUA()
		{
		}
	}

	template <typename...Args>
	inline void luaS_invoke_void(lua_State* L, const std::vector<int>& refs, const Args&...args)
	{
		for (const auto& ref : refs)
		{
			luaS_invoke_void(L, ref, args...);
		}
	}

	template <typename...Args>
	inline void luaS_invoke_void_direct(lua_State* L, int ref, const Args&...args)
	{
		__try
		{
			lua_getglobal(L, "internal_xpcall_ioiqocsdvv");
			luaS_pushfunction(L, ref);
			(luaS_push(L, args), ...);
			lua_call(L, sizeof...(args) + 1, 0);
		}
		__EXCEPTIONAL_LUA()
		{
		}
	}

	template <typename...Args>
	inline void luaS_invoke_void_direct(lua_State* L, const std::vector<int>& refs, const Args&...args)
	{
		for (const auto& ref : refs)
		{
			luaS_invoke_void_direct(L, ref, args...);
		}
	}

	template <typename...Args>
	inline void luaS_invoke_void_variable(lua_State* L, bool direct, int ref, const Args&...args)
	{
		if (direct)
		{
			luaS_invoke_void_direct(L, ref, args...);
		}
		else
		{
			luaS_invoke_void(L, ref, args...);
		}
	}

	template <typename...Args>
	inline void luaS_invoke_void_variable(lua_State* L, bool direct, const std::vector<int>& refs, const Args&...args)
	{
		if (direct)
		{
			luaS_invoke_void_direct(L, refs, args...);
		}
		else
		{
			luaS_invoke_void(L, refs, args...);
		}
	}

#define FOR_EACH_PROPERTY(f) \
f(menu_name) \
f(command_names) \
f(help_text) \
f(name_for_config) \
f(visible) \
f(value) \
f(min_value) \
f(max_value) \
f(step_size) \
f(precision) \
f(indicator_type) \
f(target)

	extern int lua_menu_my_root(lua_State* L);
	extern int lua_menu_player_root(lua_State* L);
	extern int lua_menu_shadow_root(lua_State* L);
	extern int lua_menu_ref_by_path(lua_State* L);
	extern int lua_menu_ref_by_rel_path(lua_State* L);
	extern int lua_menu_ref_by_command_name(lua_State* L);
	extern int lua_menu_list(lua_State* L);
	extern int lua_menu_action(lua_State* L);
	extern int lua_menu_toggle(lua_State* L);
	extern int lua_menu_slider(lua_State* L);
	extern int lua_menu_slider_float(lua_State* L);
	extern int lua_menu_click_slider(lua_State* L);
	extern int lua_menu_click_slider_float(lua_State* L);
	extern int lua_menu_list_select(lua_State* L);
	extern int lua_menu_list_action(lua_State* L);
	extern int lua_menu_text_input(lua_State* L);
	extern int lua_menu_colour(lua_State* L);
	extern int lua_menu_rainbow(lua_State* L);
	extern int lua_menu_internal_create_rainbow_carqdmtrem(lua_State* L);
	extern int lua_menu_divider(lua_State* L);
	extern int lua_menu_readonly(lua_State* L);
	extern int lua_menu_readonly_name(lua_State* L);
	extern int lua_menu_hyperlink(lua_State* L);
	extern int lua_menu_action_slider(lua_State* L);
	extern int lua_menu_textslider(lua_State* L);
	extern int lua_menu_slider_text(lua_State* L);
	extern int lua_menu_textslider_stateful(lua_State* L);
	extern int lua_menu_player_list_players_shortcut(lua_State* L);
	extern int lua_menu_link(lua_State* L);
	extern int lua_menu_apply_command_states(lua_State* L);
	extern int lua_menu_delete(lua_State* L);
	extern int lua_menu_replace(lua_State* L);
	extern int lua_menu_detach(lua_State* L);
	extern int lua_menu_attach(lua_State* L);
	extern int lua_menu_attach_after(lua_State* L);
	extern int lua_menu_attach_before(lua_State* L);
	extern int lua_menu_is_ref_valid(lua_State* L);
	extern int lua_menu_focus(lua_State* L);
	extern int lua_menu_is_focused(lua_State* L);
	extern int lua_menu_get_applicable_players(lua_State* L);
	extern int lua_menu_get_parent(lua_State* L);
	extern int lua_menu_get_type(lua_State* L);
	extern int lua_menu_get_children(lua_State* L);
	extern int lua_menu_list_get_focus(lua_State* L);
	extern int lua_menu_list_get_focus_physical(lua_State* L);
	extern int lua_menu_collect_garbage(lua_State* L);
	extern int lua_menu_is_open(lua_State* L);
	extern int lua_menu_get_position(lua_State* L);
	extern int lua_menu_get_main_view_position_and_size(lua_State* L);
	extern int lua_menu_get_current_menu_list(lua_State* L);
	extern int lua_menu_get_current_ui_list(lua_State* L);
	extern int lua_menu_get_active_list_cursor_text(lua_State* L);
	extern int lua_menu_are_tabs_visible(lua_State* L);
	extern int lua_menu_show_command_box(lua_State* L);
	extern int lua_menu_show_command_box_click_based(lua_State* L);
	extern int lua_menu_trigger_commands(lua_State* L);
	extern int lua_menu_trigger_command(lua_State* L);
	extern int lua_menu_command_box_is_open(lua_State* L);
	extern int lua_menu_command_box_get_dimensions(lua_State* L);
	extern int lua_menu_is_in_screenshot_mode(lua_State* L);
	extern int lua_menu_on_tick_in_viewport(lua_State* L);
	extern int lua_menu_on_focus(lua_State* L);
	extern int lua_menu_on_blur(lua_State* L);
	extern int lua_menu_remove_handler(lua_State* L);
	extern int lua_menu_get_physical(lua_State* L);
	extern int lua_menu_get_menu_name(lua_State* L);
	extern int lua_menu_get_command_names(lua_State* L);
	extern int lua_menu_get_help_text(lua_State* L);
	extern int lua_menu_get_perm(lua_State* L);
	extern int lua_menu_get_name_for_config(lua_State* L);
	extern int lua_menu_get_visible(lua_State* L);
	extern int lua_menu_get_value(lua_State* L);
	extern int lua_menu_get_min_value(lua_State* L);
	extern int lua_menu_get_max_value(lua_State* L);
	extern int lua_menu_get_step_size(lua_State* L);
	extern int lua_menu_get_precision(lua_State* L);
	extern int lua_menu_get_indicator_type(lua_State* L);
	extern int lua_menu_get_target(lua_State* L);
	extern int lua_menu_get_state(lua_State* L);
	extern int lua_menu_get_default_state(lua_State* L);
	extern int lua_menu_set_state(lua_State* L);
	extern int lua_menu_apply_default_state(lua_State* L);
	extern int lua_menu_recursively_apply_default_state(lua_State* L);
	extern int lua_menu_save_state_in_memory(lua_State* L);
#define DECL_PROPERTY_SETTER(prop) extern int lua_menu_set_ ## prop(lua_State* L, int i); inline int lua_menu_set_ ## prop(lua_State* L) { return lua_menu_set_ ## prop(L, 2); }
	FOR_EACH_PROPERTY(DECL_PROPERTY_SETTER)
	extern int lua_menu_set_list_action_options(lua_State* L);
	extern int lua_menu_set_action_slider_options(lua_State* L);
	extern int lua_menu_set_textslider_options(lua_State* L);
	extern int lua_menu_add_value_replacement(lua_State* L);
	extern int lua_menu_set_temporary(lua_State* L);
	extern int lua_menu_show_warning(lua_State* L);
	extern int lua_menu_get_activation_key_hash(lua_State* L);
	extern int lua_menu_get_edition(lua_State* L);
	extern int lua_menu_get_version(lua_State* L);

	extern int lua_players_on_join(lua_State* L);
	extern int lua_players_on_leave(lua_State* L);
	extern int lua_players_internal_get_join_callbacks(lua_State* L);
	extern int lua_players_exists(lua_State* L);
	extern int lua_players_user(lua_State* L);
	extern int lua_players_user_ped(lua_State* L);
	extern int lua_players_list(lua_State* L);
	extern int lua_players_list_only(lua_State* L);
	extern int lua_players_list_except(lua_State* L);
	extern int lua_players_list_all_with_excludes(lua_State* L);
	extern int lua_players_get_host(lua_State* L);
	extern int lua_players_get_script_host(lua_State* L);
	extern int lua_players_get_focused(lua_State* L);
	extern int lua_players_get_name(lua_State* L);
	extern int lua_players_get_rockstar_id(lua_State* L);
	extern int lua_players_get_rockstar_id_2(lua_State* L);
	extern int lua_players_get_ip(lua_State* L);
	extern int lua_players_get_ip_string(lua_State* L);
	extern int lua_players_get_port(lua_State* L);
	extern int lua_players_get_connect_ip(lua_State* L);
	extern int lua_players_get_connect_port(lua_State* L);
	extern int lua_players_get_lan_ip(lua_State* L);
	extern int lua_players_get_lan_port(lua_State* L);
	extern int lua_players_are_stats_ready(lua_State* L);
	extern int lua_players_get_rank(lua_State* L);
	extern int lua_players_get_rp(lua_State* L);
	extern int lua_players_get_money(lua_State* L);
	extern int lua_players_get_wallet(lua_State* L);
	extern int lua_players_get_bank(lua_State* L);
	extern int lua_players_get_kd(lua_State* L);
	extern int lua_players_get_kills(lua_State* L);
	extern int lua_players_get_deaths(lua_State* L);
	extern int lua_players_get_language(lua_State* L);
	extern int lua_players_is_using_controller(lua_State* L);
	extern int lua_players_get_name_with_tags(lua_State* L);
	extern int lua_players_get_tags_string(lua_State* L);
	extern int lua_players_is_godmode(lua_State* L);
	extern int lua_players_is_marked_as_modder(lua_State* L);
	extern int lua_players_is_marked_as_modder_or_admin(lua_State* L);
	extern int lua_players_is_marked_as_admin(lua_State* L);
	extern int lua_players_is_marked_as_attacker(lua_State* L);
	extern int lua_players_is_otr(lua_State* L);
	extern int lua_players_is_out_of_sight(lua_State* L);
	extern int lua_players_is_in_interior(lua_State* L);
	extern int lua_players_is_typing(lua_State* L);
	extern int lua_players_is_using_vpn(lua_State* L);
	extern int lua_players_is_visible(lua_State* L);
	extern int lua_players_get_host_token(lua_State* L);
	extern int lua_players_get_host_token_hex(lua_State* L);
	extern int lua_players_get_host_queue_position(lua_State* L);
	extern int lua_players_get_host_queue(lua_State* L);
	extern int lua_players_get_boss(lua_State* L);
	extern int lua_players_get_org_type(lua_State* L);
	extern int lua_players_get_org_colour(lua_State* L);
	extern int lua_players_clan_get_motto(lua_State* L);
	extern int lua_players_get_position(lua_State* L);
	extern int lua_players_is_in_vehicle(lua_State* L);
	extern int lua_players_get_vehicle_model(lua_State* L);
	extern int lua_players_is_using_rc_vehicle(lua_State* L);
	extern int lua_players_get_bounty(lua_State* L);
	extern int lua_players_send_sms(lua_State* L);
	extern int lua_players_get_cam_pos(lua_State* L);
	extern int lua_players_get_cam_rot(lua_State* L);
	extern int lua_players_get_spectate_target(lua_State* L);
	extern int lua_players_get_waypoint(lua_State* L);
	extern int lua_players_get_net_player(lua_State* L);
	extern int lua_players_set_wanted_level(lua_State* L);
	extern int lua_players_give_pickup_reward(lua_State* L);
	extern int lua_players_get_weapon_damage_modifier(lua_State* L);
	extern int lua_players_get_melee_weapon_damage_modifier(lua_State* L);
	extern int lua_players_add_detection(lua_State* L);
	extern int lua_players_on_flow_event_done(lua_State* L);
	extern int lua_players_teleport_2d(lua_State* L);
	extern int lua_players_teleport_3d(lua_State* L);
	extern int lua_players_get_millis_since_discovery(lua_State* L);

	extern int lua_entities_create_ped(lua_State* L);
	extern int lua_entities_create_vehicle(lua_State* L);
	extern int lua_entities_create_object(lua_State* L);
	extern int lua_entities_get_user_vehicle_as_handle(lua_State* L);
	extern int lua_entities_get_user_vehicle_as_pointer(lua_State* L);
	extern int lua_entities_get_user_personal_vehicle_as_handle(lua_State* L);
	extern int lua_entities_handle_to_pointer(lua_State* L);
	extern int lua_entities_has_handle(lua_State* L);
	extern int lua_entities_pointer_to_handle(lua_State* L);
	extern int lua_entities_any_to_handle(lua_State* L);
	extern int lua_entities_get_all_vehicles_as_handles(lua_State* L);
	extern int lua_entities_get_all_vehicles_as_pointers(lua_State* L);
	extern int lua_entities_get_all_peds_as_handles(lua_State* L);
	extern int lua_entities_get_all_peds_as_pointers(lua_State* L);
	extern int lua_entities_get_all_objects_as_handles(lua_State* L);
	extern int lua_entities_get_all_objects_as_pointers(lua_State* L);
	extern int lua_entities_get_all_pickups_as_handles(lua_State* L);
	extern int lua_entities_get_all_pickups_as_pointers(lua_State* L);
	extern int lua_entities_delete(lua_State* L);
	extern int lua_entities_delete_by_handle(lua_State* L);
	extern int lua_entities_delete_by_pointer(lua_State* L);
	extern int lua_entities_get_model_hash(lua_State* L);
	extern int lua_entities_get_model_uhash(lua_State* L);
	extern int lua_entities_get_position(lua_State* L);
	extern int lua_entities_get_rotation(lua_State* L);
	extern int lua_entities_get_health(lua_State* L);
	extern int lua_entities_get_upgrade_value(lua_State* L);
	extern int lua_entities_get_upgrade_max_value(lua_State* L);
	extern int lua_entities_set_upgrade_value(lua_State* L);
	extern int lua_entities_get_current_gear(lua_State* L);
	extern int lua_entities_set_current_gear(lua_State* L);
	extern int lua_entities_get_next_gear(lua_State* L);
	extern int lua_entities_set_next_gear(lua_State* L);
	extern int lua_entities_get_rpm(lua_State* L);
	extern int lua_entities_set_rpm(lua_State* L);
	extern int lua_entities_get_gravity(lua_State* L);
	extern int lua_entities_set_gravity(lua_State* L);
	extern int lua_entities_set_gravity_multiplier(lua_State* L);
	extern int lua_entities_get_boost_charge(lua_State* L);
	extern int lua_entities_get_draw_handler(lua_State* L);
	extern int lua_entities_vehicle_draw_handler_get_pearlecent_colour(lua_State* L);
	extern int lua_entities_vehicle_draw_handler_get_wheel_colour(lua_State* L);
	extern int lua_entities_get_vehicle_has_been_owned_by_player(lua_State* L);
	extern int lua_entities_get_player_info(lua_State* L);
	extern int lua_entities_player_info_get_game_state(lua_State* L);
	extern int lua_entities_get_weapon_manager(lua_State* L);
	extern int lua_entities_get_head_blend_data(lua_State* L);
	extern int lua_entities_get_net_object(lua_State* L);
	extern int lua_entities_get_owner(lua_State* L);
	extern int lua_entities_set_can_migrate(lua_State* L);
	extern int lua_entities_get_can_migrate(lua_State* L);
	extern int lua_entities_give_control(lua_State* L);
	extern int lua_entities_give_control_by_handle(lua_State* L);
	extern int lua_entities_give_control_by_pointer(lua_State* L);
	extern int lua_entities_vehicle_get_handling(lua_State* L);
	extern int lua_entities_handling_get_subhandling(lua_State* L);
	extern int lua_entities_detach_wheel(lua_State* L);
	extern int lua_entities_get_bomb_bay_pos(lua_State* L);
	extern int lua_entities_is_player_ped(lua_State* L);
	extern int lua_entities_is_invulnerable(lua_State* L);

	extern int lua_chat_internal_try_lock(lua_State* L);
	extern int lua_chat_internal_unlock(lua_State* L);
	extern int lua_chat_internal_close_impl(lua_State* L);
	extern int lua_chat_internal_open_impl(lua_State* L);
	extern int lua_chat_internal_add_to_draft(lua_State* L);
	extern int lua_chat_send_message(lua_State* L);
	extern int lua_chat_send_targeted_message(lua_State* L);
	extern int lua_chat_on_message(lua_State* L);
	extern int lua_chat_get_state(lua_State* L);
	extern int lua_chat_get_draft(lua_State* L);
	extern int lua_chat_remove_from_draft(lua_State* L);
	extern int lua_chat_get_history(lua_State* L);

	extern int lua_directx_create_texture(lua_State* L);
	extern int lua_directx_has_texture_loaded(lua_State* L);
	extern int lua_directx_get_texture_dimensions(lua_State* L);
	extern int lua_directx_get_texture_character(lua_State* L);
	extern int lua_directx_draw_texture_client(lua_State* L);
	extern int lua_directx_draw_texture_by_points(lua_State* L);
	extern int lua_directx_create_font(lua_State* L);
	extern int lua_directx_draw_text(lua_State* L);
	extern int lua_directx_draw_text_client(lua_State* L);
	extern int lua_directx_draw_rect(lua_State* L);
	extern int lua_directx_draw_rect_client(lua_State* L);
	extern int lua_directx_draw_line(lua_State* L);
	extern int lua_directx_draw_line_client(lua_State* L);
	extern int lua_directx_draw_triangle(lua_State* L);
	extern int lua_directx_draw_triangle_client(lua_State* L);
	extern int lua_directx_draw_circle_square(lua_State* L);
	extern int lua_directx_get_client_size(lua_State* L);
	extern int lua_directx_get_text_size(lua_State* L);
	extern int lua_directx_get_text_size_client(lua_State* L);
	extern int lua_directx_pos_hud_to_client(lua_State* L);
	extern int lua_directx_size_hud_to_client(lua_State* L);
	extern int lua_directx_pos_client_to_hud(lua_State* L);
	extern int lua_directx_size_client_to_hud(lua_State* L);
	extern int lua_directx_square_to_client(lua_State* L);
	extern int lua_directx_client_to_square(lua_State* L);
	extern int lua_directx_blurrect_new(lua_State* L);
	extern int lua_directx_blurrect_free(lua_State* L);
	extern int lua_directx_blurrect_draw(lua_State* L);
	extern int lua_directx_blurrect_draw_client(lua_State* L);

	//extern int lua_util_internal_prerun_rantpgscmu(lua_State* L);
	extern int lua_util_ensure_package_is_installed(lua_State* L);
	extern int lua_util_execute_in_os_thread(lua_State* L);
	extern int lua_util_internal_yield_zfvasowkyumq(lua_State* L);
	extern int lua_util_internal_is_keep_running_implied_uuqepkb32o(lua_State* L);
	extern int lua_util_internal_emit_bad_practice_w7uoni5ptt(lua_State* L);
	extern int lua_util_internal_emit_deprecated_hzpyiownhj(lua_State* L);
	extern int lua_util_request_stop(lua_State* L);
	extern int lua_util_is_os_thread(lua_State* L);
	extern int lua_util_can_continue(lua_State* L);
	extern int lua_util_set_busy(lua_State* L);
	extern int lua_util_on_stop(lua_State* L);
	extern int lua_util_dispatch_on_stop(lua_State* L);
	extern int lua_util_toast(lua_State* L);
	extern int lua_util_log(lua_State* L);
	extern int lua_util_draw_debug_text(lua_State* L);
	extern int lua_util_draw_centred_text(lua_State* L);
	extern int lua_util_show_corner_help(lua_State* L);
	extern int lua_util_replace_corner_help(lua_State* L);
	extern int lua_util_joaat(lua_State* L);
	extern int lua_util_ujoaat(lua_State* L);
	extern int lua_util_reverse_joaat(lua_State* L);
	extern int lua_util_is_this_model_a_blimp(lua_State* L);
	extern int lua_util_is_this_model_an_object(lua_State* L);
	extern int lua_util_is_this_model_a_submarine(lua_State* L);
	extern int lua_util_is_this_model_a_submarine_car(lua_State* L);
	extern int lua_util_is_this_model_a_trailer(lua_State* L);
	extern int lua_util_get_vehicles(lua_State* L);
	extern int lua_util_get_objects(lua_State* L);
	extern int lua_util_get_weapons(lua_State* L);
	extern int lua_util_begin_text_command_display_text(lua_State* L);
	extern int lua_util_begin_text_command_line_count(lua_State* L);
	extern int lua_util_begin_text_command_is_this_help_message_being_displayed(lua_State* L);
	extern int lua_util_begin_text_command_display_help(lua_State* L);
	extern int lua_util_begin_text_command_get_width(lua_State* L);
	extern int lua_util_begin_text_command_thefeed_post(lua_State* L);
	extern int lua_util_get_rp_required_for_rank(lua_State* L);
	extern int lua_util_get_session_players_bitflag(lua_State* L);
	extern int lua_util_trigger_script_event(lua_State* L);
	extern int lua_util_current_time_millis(lua_State* L);
	extern int lua_util_current_unix_time_seconds(lua_State* L);
	extern int lua_util_remove_handler(lua_State* L);
	extern int lua_util_clear_commands_and_native_event_handlers(lua_State* L);
	extern int lua_util_is_session_started(lua_State* L);
	extern int lua_util_is_session_transition_active(lua_State* L);
	extern int lua_util_get_char_slot(lua_State* L);
	extern int lua_util_get_ground_z(lua_State* L);
	extern int lua_util_spoof_script(lua_State* L);
	extern int lua_util_spoof_script_thread(lua_State* L);
	extern int lua_util_remove_blip(lua_State* L);
	extern int lua_util_arspinner_is_enabled(lua_State* L);
	extern int lua_util_arspinner_enable(lua_State* L);
	extern int lua_util_arspinner_disable(lua_State* L);
	extern int lua_util_graceland_is_enabled(lua_State* L);
	extern int lua_util_graceland_enable(lua_State* L);
	extern int lua_util_graceland_disable(lua_State* L);
	extern int lua_util_is_bigmap_active(lua_State* L);
	extern int lua_util_copy_to_clipboard(lua_State* L);
	extern int lua_util_get_clipboard_text(lua_State* L);
	extern int lua_util_read_colons_and_tabs_file(lua_State* L);
	extern int lua_util_write_colons_file(lua_State* L);
	extern int lua_util_draw_ar_beacon(lua_State* L);
	extern int lua_util_draw_box(lua_State* L);
	extern int lua_util_draw_sphere(lua_State* L);
	extern int lua_util_request_script_host(lua_State* L);
	extern int lua_util_give_script_host(lua_State* L);
	extern int lua_util_register_file(lua_State* L);
	extern int lua_util_get_label_text(lua_State* L);
	extern int lua_util_register_label(lua_State* L);
	extern int lua_util_is_key_down(lua_State* L);
	extern int lua_util_call_foreign_function(lua_State* L);
	extern int lua_util_get_rtti_name(lua_State* L);
	extern int lua_util_get_rtti_hierarchy(lua_State* L);
	extern int lua_util_set_particle_fx_asset(lua_State* L);
	extern int lua_util_blip_handle_to_pointer(lua_State* L);
	extern int lua_util_get_blip_display(lua_State* L);
	extern int lua_util_teleport_2d(lua_State* L);
	extern int lua_util_is_interaction_menu_open(lua_State* L);
	extern int lua_util_on_transition_finished(lua_State* L);
	extern int lua_util_get_closest_hud_colour(lua_State* L);
	extern int lua_util_is_soup_netintel_inited(lua_State* L);
	extern int lua_util_on_pad_shake(lua_State* L);
	extern int lua_util_open_folder(lua_State* L);
	extern int lua_util_set_nullptr_preference(lua_State* L);
	extern int lua_util_get_tps(lua_State* L);
	extern int lua_util_get_session_code(lua_State* L);
	extern int lua_util_stat_get_type(lua_State* L);
	extern int lua_util_stat_get_int64(lua_State* L);
	extern int lua_util_stat_set_int64(lua_State* L);
	extern int lua_util_new_toast_config(lua_State* L);
	extern int lua_util_toast_config_get_flags(lua_State* L);
	extern int lua_util_get_model_info(lua_State* L);
	extern int lua_util_is_valid_file_name(lua_State* L);
	extern int lua_util_rgb2hsv(lua_State* L);
	extern int lua_util_hsv2rgb(lua_State* L);
	extern int lua_util_calculate_luminance(lua_State* L);
	extern int lua_util_calculate_contrast(lua_State* L);
	extern int lua_util_is_contrast_sufficient(lua_State* L);
	extern int lua_util_set_waypoint(lua_State* L);
	extern int lua_util_set_cam_quaternion(lua_State* L);
	extern int lua_util_get_screen_coord_from_world_coord_no_adjustment(lua_State* L);
	extern int lua_util_utf8_to_utf16(lua_State* L);
	extern int lua_util_utf16_to_utf8(lua_State* L);
	extern int lua_util_get_gps_route(lua_State* L);
	extern int lua_util_sc_is_blocked(lua_State* L);
	extern int lua_util_sc_block(lua_State* L);
	extern int lua_util_sc_unblock(lua_State* L);
	extern int lua_util_get_tunable_int(lua_State* L);
	extern int lua_util_get_tunable_bool(lua_State* L);
	extern int lua_util_ui3dscene_set_element_2d_position(lua_State* L);
	extern int lua_util_ui3dscene_set_element_2d_size(lua_State* L);
	extern int lua_util_ui3dscene_set_element_background_colour(lua_State* L);
	extern int lua_util_ip_from_string(lua_State* L);
	extern int lua_util_ip_to_string(lua_State* L);
	extern int lua_util_ip_get_as(lua_State* L);
	extern int lua_util_ip_get_location(lua_State* L);

	extern int lua_v3_call(lua_State* L);
	extern int lua_v3_new(lua_State* L);
	extern int lua_v3_free(lua_State* L);
	extern int lua_v3_get(lua_State* L);
	extern int lua_v3_getX(lua_State* L);
	extern int lua_v3_getY(lua_State* L);
	extern int lua_v3_getZ(lua_State* L);
	extern int lua_v3_getHeading(lua_State* L);
	extern int lua_v3_set(lua_State* L);
	extern int lua_v3_setX(lua_State* L);
	extern int lua_v3_setY(lua_State* L);
	extern int lua_v3_setZ(lua_State* L);
	extern int lua_v3_reset(lua_State* L);
	extern int lua_v3_add(lua_State* L);
	extern int lua_v3_sub(lua_State* L);
	extern int lua_v3_mul(lua_State* L);
	extern int lua_v3_div(lua_State* L);
	extern int lua_v3_addNew(lua_State* L);
	extern int lua_v3_subNew(lua_State* L);
	extern int lua_v3_mulNew(lua_State* L);
	extern int lua_v3_divNew(lua_State* L);
	extern int lua_v3_eq(lua_State* L);
	extern int lua_v3_magnitude(lua_State* L);
	extern int lua_v3_distance(lua_State* L);
	extern int lua_v3_abs(lua_State* L);
	extern int lua_v3_min(lua_State* L);
	extern int lua_v3_max(lua_State* L);
	extern int lua_v3_dot(lua_State* L);
	extern int lua_v3_normalise(lua_State* L);
	extern int lua_v3_crossProduct(lua_State* L);
	extern int lua_v3_toRot(lua_State* L);
	extern int lua_v3_lookAt(lua_State* L);
	extern int lua_v3_toDir(lua_State* L);
	extern int lua_v3_toString(lua_State* L);

	extern int lua_lang_get_current(lua_State* L);
	extern int lua_lang_is_code_valid(lua_State* L);
	extern int lua_lang_get_code_for_soup(lua_State* L);
	extern int lua_lang_is_automatically_translated(lua_State* L);
	extern int lua_lang_is_english(lua_State* L);
	extern int lua_lang_register(lua_State* L);
	extern int lua_lang_set_translate(lua_State* L);
	extern int lua_lang_translate(lua_State* L);
	extern int lua_lang_find(lua_State* L);
	extern int lua_lang_find_builtin(lua_State* L);
	extern int lua_lang_find_registered(lua_State* L);
	extern int lua_lang_get_string(lua_State* L);
	extern int lua_lang_is_mine(lua_State* L);
	extern int lua_lang_is_builtin(lua_State* L);

	extern int lua_filesystem_appdata_dir(lua_State* L);
	extern int lua_filesystem_stand_dir(lua_State* L);
	extern int lua_filesystem_scripts_dir(lua_State* L);
	extern int lua_filesystem_resources_dir(lua_State* L);
	extern int lua_filesystem_store_dir(lua_State* L);
	extern int lua_filesystem_mkdirs(lua_State* L);

	extern int lua_async_http_have_access(lua_State* L);
	extern int lua_async_http_init(lua_State* L);
	extern int lua_async_http_dispatch(lua_State* L);
	extern int lua_async_http_set_post(lua_State* L);
	extern int lua_async_http_add_header(lua_State* L);
	extern int lua_async_http_set_method(lua_State* L);
	extern int lua_async_http_prefer_ipv6(lua_State* L);

	extern void luaS_register_memory(lua_State* L);
	extern void luaS_register_profiling(lua_State* L);
	extern void luaS_register_native_invoker(lua_State* L);
}
