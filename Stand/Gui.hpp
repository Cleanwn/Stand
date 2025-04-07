#pragma once

#include <optional>
#include <unordered_set>

#include <Windows.h>

#include <soup/fwd.hpp>
#include <soup/Promise.hpp>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "AbstractPlayer.hpp"
#include "CommandList.hpp"
#include "ConfigHotkeys.hpp"
#include "ConfigMetaState.hpp"
#include "ConfigState.hpp"
#include "Direction.hpp"
#include "LangId.hpp"
#include "MenuKey.hpp"
#include "MouseMode.hpp"
#include "RecursiveScopedSpinlock.hpp"
#include "Spinlock.hpp"
#include "Vector2Plus.hpp"

#define GUI_DEBUG false
#define BAD_BOY_ENABLED true
#define FREE_PROTECTIONS false

namespace Stand
{
	enum RootState : uint8_t
	{
		GUI_NONE,
		GUI_MINIMAL,
		GUI_FREEONLINE,
		GUI_FREE,
		GUI_BASIC,
		GUI_REGULAR,
		GUI_ULTIMATE,
	};

	enum BulkStateOperation : uint8_t
	{
		BSO_NONE,
		BSO_DEFAULT,
		BSO_LOAD,
	};

	enum InputType : uint8_t
	{
		INPUTTYPE_INDIFFERENT,
		INPUTTYPE_MOUSE_MOVE,
		INPUTTYPE_MOUSE_SCROLLWHEEL,
		INPUTTYPE_MOUSE_CLICK,
	};

	enum class ButtonInstructionsMode : uint8_t
	{
		ALWAYS,
		SMART,
		NEVER,
	};

	struct WarningData
	{
		uint64_t hash;
		std::wstring text;
		time_t can_proceed_after;
		std::function<void(ThreadContext)> proceed_callback;
		std::function<void(ThreadContext)> cancel_callback;
	};

	enum class UnloadState : uint8_t
	{
		NONE,
		REQUESTED,
		SAVE_PENDING_CONFIGS,
		APPLY_DEFAULT_STATE,
		EMPTY_PLAYER_LIST,
		//CLEANUP_NON_SCRIPT,
		SWITCH_TO_GUI_NONE,
		HAND_OVER_TO_MAIN
	};

	class Gui
	{
	public:
		Gui();

		time_t init_time = 0;
		soup::Promise<> initial_load_done{};
		soup::Promise<> initial_auth_done{};
		[[nodiscard]] bool isFullyLoaded() const noexcept;

		std::unique_ptr<CommandList> pseudo_commands{};
		std::unique_ptr<CommandList> root_list{};
		uint8_t root_cursor = 0;
		[[nodiscard]] CommandList* getActivateTab() const;
		[[nodiscard]] CommandList* getGameTab() const;
		[[nodiscard]] CommandList* getStandTab() const;
		void removeTab(CommandList* tab);
		void resetCursor();

		[[nodiscard]] bool isListVisible() const;

		HANDLE memory_thread = INVALID_HANDLE_VALUE;
		bool memory_thread_running = false;
		HANDLE metrics_thread = INVALID_HANDLE_VALUE;
		bool metrics_thread_running = false;
		HANDLE worker_thread = INVALID_HANDLE_VALUE;
		bool worker_thread_running = false;
		//bool user_has_pressed_any_input = false;
		bool user_was_in_online = false;
		bool session_or_transition_was_active = false;
		bool was_freeroam = true;
		bool was_host = true;
		bool was_chatting = false;
		bool opened = false;

		RecursiveScopedSpinlock root_mtx{};
		bool minimal = false;
		RootState root_state = GUI_NONE;
		BulkStateOperation current_bulk_state_operation = BSO_NONE;
		bool root_check_in_progress = false;
		bool about_to_update_root_state = false;
		bool root_update_in_progress = false;
		lang_t root_lang = LANG_EN;
		bool players_discovered = false;
		bool reset_cursor_on_back = true;
		bool keep_cursor_reduced_for_huge_lists = true;
		void setApplicableNonFreeRootStateValue() noexcept;

		bool user_understands_navigation = false;
		bool user_understands_commands = false;
		bool user_understands_sliders = false;
		uint8_t profiles_tutorial = 0;
		bool user_understands_context_menu = false;
		bool user_understands_controller_open_close = false;
		bool user_prompted_for_controller_disables = false;
		void setProfilesTutorialDone();
		void doPromptUserForControllerDisables(ThreadContext thread_context);
		bool doPromptUserForDisablePreset(ThreadContext thread_context, hash_t preset);
	private:
		[[nodiscard]] CommandList* getDisableGameInputs() const;

	public:
		cursor_t cursor_padding = 2;
		cursor_t getCursorPadding() const;

		cursor_t command_rows = 11;
		int16_t command_columns = 1;
		[[nodiscard]] cursor_t getCommandsOnScreenLimit() const noexcept
		{
			return command_rows * command_columns;
		}

		bool reset_cursor_on_root_navigation = false;
		std::vector<CommandList*> m_active_list = {};
		std::unordered_map<uint64_t, std::string> active_list_store = {};

		cursor_t lerp = 70;
		time_t lerp_start = 0;
		cursor_t lerp_from_cursor = 0;
		cursor_t lerp_from_offset = 0;
		void lerpStart(CommandList* const list) noexcept;
		void lerpStop() noexcept;

		bool skip_accepted_warnings = true;
		std::unordered_set<uint64_t> accepted_warnings = {};
		std::unordered_map<int8_t, WarningData> warnings = {};
		bool force_warning_read = true;
		[[nodiscard]] bool isWarningActive() const;

		MouseMode mouse_mode = MouseMode::NONE;
		bool mouse_mode_set_focus = true;
		void setMouseMode(MouseMode mode);
		void updateCursorVisibility();

		bool controller_support = true;
		MenuKey keyboard_pressing = MENUKEY_NONE;
		InputType last_input_type = INPUTTYPE_INDIFFERENT;
		int8_t keyboard_hold = 0;
		time_t next_keyboard_eval;
		time_t input_speed = 100;
		time_t input_speed_holding = 60;
		time_t tab_input_speed = 120;
		bool wraparound_holding_pause = true;
		time_t last_konami_input = 0;
		uint8_t konami = 0;
	private:
		void mapControlInputToMenuKeyThisTick(const int control_input, const MenuKey menu_key, const InputType input_type);
	public:
		void onKeyPressStart(const MenuKey key, const InputType input_type);
		Direction last_directional_input = RIGHT;
		Direction last_directional_input_processed = RIGHT;
		[[nodiscard]] static Direction menuKeyToDirection2d(const MenuKey key);
		bool back_resets_cursor = false;
		bool back_closes_menu = false;
		bool show_cursor_pos = false;
		bool cursor_pos_includes_dividers = false;
		bool show_help_text = true;
		bool show_syntax = true;
		bool show_slider_behaviour = true;
		bool show_nonuseronly = true;
		bool show_cmd_owner = true;
		ButtonInstructionsMode button_instructions_mode = ButtonInstructionsMode::ALWAYS;
		UnloadState unload_state = UnloadState::NONE;
		bool killswitched = false;

		bool menu_navigation_reminders = true;
		uint8_t stupid_back_counter = 0;

		bool godmode = false;
		uint8_t grace = 0;
		bool seatglue = false;
		[[nodiscard]] bool isGodmodeActive();
		void onGodmodeDisable();

		bool kill_loader_while_chatting = true;

		float character_scale_multiplier = 1.0f;
		CommandToggleNoCorrelation* character_scale_affects_camera_command = nullptr;
		float user_defined_camera_distance_multiplier = 1.0f;
		CommandToggleNoCorrelation* character_scale_affects_speed_command = nullptr;
		float user_defined_run_speed = 1.0f;
		float user_defined_swim_speed = 1.0f;
		bool need_run_speed_cleanup = false;
		bool need_swim_speed_cleanup = false;
		void set_camera_distance_from_character_multiplier_all_things_considered() const;
		void updateRunSpeedIgnoreScale() const;
		void updateSwimSpeedIgnoreScale() const;

	private:
		bool ragdoll_reset = true;
	public:
		bool phone_disabled = false;
		time_t phone_open_deadline;
		bool phone_stay_closed = false;

		bool second_person_cam = false;

		CommandList* player_list = nullptr;
		CommandPlayer* player_commands[MAX_PLAYERS]{};
		std::vector<AbstractPlayer> focused_players = {};
		bool focused_player_beacon_in_list_of_all_players = false;
		bool focused_player_beacon_in_list_belonging_to_player = false;
		bool focused_player_beacon_exclude_me = true;

	public:
		std::wstring auto_save_state;
		std::optional<time_t> command_state_change_save = std::nullopt;
		[[nodiscard]] bool isUsingAutosaveState() const noexcept;

#if GUI_DEBUG
		std::string persistent_debug_text = {};
#endif
		bool no_collision_with_other_vehicles = false;
		bool no_extra_clear = false;
		Vector2Plus waypoint = { 0.0f, 0.0f, 0.0f };

	public:
		lang_t gtao_status_lang = -1;
		std::wstring status_text = {};
		std::wstring status_message = {};
		bool show_status_message = false;
		void doGtaOnlineStatusRequest() noexcept;

	public:
		[[nodiscard]] static std::string getMenuOpenCloseButtonString();

	public:
		std::vector<CommandToggle*> commands_with_correlation{};
		void processToggleCorrelation(ThreadContext thread_context, ToggleCorrelation_t correlation, bool value) const;

		void open(ThreadContext thread_context);
		void close(ThreadContext thread_context);
	private:
		void openCloseCommon(ThreadContext thread_context, bool value);
	public:
		void inputOpenCloseMenu(ThreadContext thread_context);
		void sfxOpenClose(ThreadContext thread_context, bool opened);
		void sfxClose(ThreadContext thread_context);
		void openForcefully(ThreadContext thread_context);
		bool inputRootUp(ThreadContext thread_context, bool holding = false);
		bool inputRootDown(ThreadContext thread_context, bool holding = false);
		void onPreRootCursorUpdate(ThreadContext thread_context);
		void onRootCursorUpdate(ThreadContext thread_context);
		bool isRootNavigationActive() const;
		bool isMenuNavigationActive() const;
		void processPostNavigationTutorial(const CommandList* const active_list) const;
		bool inputUp(ThreadContext thread_context, const bool holding = false);
		bool inputDown(ThreadContext thread_context, const bool holding = false);
		bool inputLeft(ThreadContext thread_context, const bool holding);
		bool inputRight(ThreadContext thread_context, const bool holding);
		void sfxLeftRight() const;
		void inputClick(ThreadContext thread_context);
		void inputBack(ThreadContext thread_context, bool play_sound);
		void inputStop() noexcept;
		static void shellExecute(const char* uri);
		static void shellExecute(const wchar_t* uri);
		void prepareForOverlay() noexcept;

		bool user_understands_command_chaining = false;
		void showCommandBox();
		void showCommandBox(std::wstring&& prefill);
		void showCommandBox(const std::wstring& prefill, Click& click);

	public:
		Spinlock commands_with_hotkeys_mtx{};
		std::vector<CommandPhysical*> commands_with_hotkeys{};
		bool hotkeys_disabled = false;
		bool suppress_generic_hotkey_responses = false;
	private:
		void triggerHotkey(std::vector<CommandPhysical*>&& matches) const;
	public:
		[[nodiscard]] std::vector<Hotkey> triggerHotkeyDown(Hotkey hotkey) const; // script thread
		void triggerHotkeyUp(Hotkey hotkey) const; // script thread

	public:
		bool command_box_enabled = true;
		bool suppress_generic_command_box_responses = false;
		bool triggerCommands(const std::string& input, Click& click, CommandList* const root = nullptr);
		bool triggerCommands(std::wstring&& input, Click& click, CommandList* const root = nullptr);
		[[nodiscard]] static bool parseCommand(std::wstring& command, std::wstring& args);
		[[nodiscard]] std::vector<CommandIssuable*> findCommandsWhereCommandNameStartsWith(const CommandName& command_name_prefix, const CommandPerm perms = COMMANDPERM_ALL) const;
		[[nodiscard]] std::vector<soup::WeakRef<CommandIssuable>> findCommandsWhereCommandNameStartsWithAsWeakrefs(const CommandName& command_name_prefix, const CommandPerm perms = COMMANDPERM_ALL) const;
	private:
		void processCommand(std::wstring& command, Click& click, bool& multiple_commands, bool& any_success, CommandList* const root = nullptr) const;

	public:
		ConfigMetaState meta_state = ConfigMetaState();
		ConfigState active_profile = ConfigState();
		uint8_t unlock_recover_state = 0;
		std::optional<std::unordered_map<std::string, std::string>> recover_state{};
		bool on_command_state_change();
		void saveActiveState();
		static void saveState(std::wstring&& name);
		void saveStateInMemory(std::unordered_map<std::string, std::string>& state);
		void cancelPendingSave();
		void recursivelyApplyState(std::unordered_map<std::string, std::string>& state, CommandList* list, const std::string& prefix = {});
		void recursivelyApplyActiveState();
		void recursivelyApplyDefaultState();
		void performRootCheck(const CommandList* const list, std::unordered_map<CommandName, std::string>& command_names_map);
		void loadStateToMemory(ConfigState& state);
		bool loadTutorialFlags(const std::unordered_map<std::string, std::string>& state);
		void saveTutorialFlags() { meta_state.save(); }
		void enableAutosave(const std::wstring& profile_name);
		void disableAutosave() noexcept;
		[[nodiscard]] std::wstring getActiveStateNameUtf16() const;
		[[nodiscard]] std::string getActiveStateNameUtf8() const;

		[[nodiscard]] bool canUpdateThemeSetting() const noexcept;
		[[nodiscard]] bool canUpdateThemeSettingWithDefaultDependency() const noexcept;
		[[nodiscard]] SOUP_FORCEINLINE bool isUnloadPending() const noexcept { return unload_state != UnloadState::NONE; }

		[[nodiscard]] bool isRootStateFreeEdition() const noexcept;
		[[nodiscard]] SOUP_FORCEINLINE bool isRootStateBasicEdition() const noexcept { return root_state == GUI_BASIC; }
		[[nodiscard]] SOUP_FORCEINLINE bool isRootStateRegularEdition() const noexcept { return root_state == GUI_REGULAR; }
		[[nodiscard]] SOUP_FORCEINLINE bool isRootStateUltimateEdition() const noexcept { return root_state == GUI_ULTIMATE; }
		[[nodiscard]] SOUP_FORCEINLINE bool isRootStateNonFree() const noexcept { return root_state >= GUI_BASIC; }
		[[nodiscard]] SOUP_FORCEINLINE bool isRootStateFull() const noexcept { return root_state > GUI_FREEONLINE; }
		[[nodiscard]] bool shouldRootStateShowLicensePrompt() const noexcept;

#if FREE_PROTECTIONS
		[[nodiscard]] SOUP_FORCEINLINE bool doesRootStateAllowPlayerList() const noexcept { return root_state >= GUI_FREEONLINE; }
#else
		[[nodiscard]] SOUP_FORCEINLINE bool doesRootStateAllowPlayerList() const noexcept { return root_state > GUI_FREEONLINE; }
#endif

#if FREE_PROTECTIONS
		[[nodiscard]] SOUP_FORCEINLINE bool doesRootStateAllowProtections() const noexcept { return root_state == GUI_FREEONLINE || isRootStateNonFree(); }
#else
		[[nodiscard]] SOUP_FORCEINLINE bool doesRootStateAllowProtections() const noexcept { return isRootStateNonFree(); }
#endif

#if FREE_PROTECTIONS
		[[nodiscard]] SOUP_FORCEINLINE bool doesRootStateAllowCrashPatches() const noexcept { return root_state >= GUI_FREEONLINE; }
#else
		[[nodiscard]] SOUP_FORCEINLINE bool doesRootStateAllowCrashPatches() const noexcept { return isRootStateFull(); }
#endif

		[[nodiscard]] static bool isFastLoad() noexcept;
		[[nodiscard]] bool isRootUpdatePendingOrInProgress() const noexcept;
		void updateRootName();
		void updateTabs(ThreadContext thread_context);
		void updateTabsIgnoreContextMenu();
		void onTabRenderModeUpdate();

		CommandList* web_focus = nullptr;
		[[nodiscard]] bool isWebGuiActive() const;
		void sendRootListToWeb() const;
		void webGoBack(uint8_t steps = 1);

	public:
		enum HotkeyState : uint8_t
		{
			HS_NONE,
			HS_ADD,
			HS_CHANGE,
		};

		ConfigHotkeys hotkeys = ConfigHotkeys();
		HotkeyState hotkey_state = HS_NONE;
		void addHotkeyToFocusedCommand();
		void changeHotkeyOnFocusedCommand();
		[[nodiscard]] bool isAwaitingSetHotkeyInput() const noexcept;
		void onSetHotkeyInput(const Hotkey hotkey);
		void abortSetHotkey() noexcept;
		void recursivelyApplyHotkeys(CommandList* list, const std::string& prefix = {});

	public:
		std::wstring prompt_text = {};
		std::function<void(ThreadContext)> prompt_yes_callback = {};
		std::function<void(ThreadContext)> prompt_no_callback = {};
		[[nodiscard]] bool isPromptActive() const noexcept;
		void showPrompt(ThreadContext thread_context, std::wstring&& prompt_text, std::function<void(ThreadContext)>&& yes_callback, std::function<void(ThreadContext)>&& no_callback);
		void showPrompt(ThreadContext thread_context, const std::string& prompt_text, std::function<void(ThreadContext)>&& yes_callback, std::function<void(ThreadContext)>&& no_callback);
		void clearPrompt(ThreadContext thread_context);

	public:
		void backToNormal(Click& click);
		static void backToNormalNonScript();
		static void backToNormalCommonNonScript();
		void backToNormalProceedToApplyDefaultState();
		void backToNormalProceedToSwitchToGuiNone();

	private:
		void removePlayer(const AbstractPlayer p, CommandPlayer* const player_command);
	public:
		[[nodiscard]] CommandList* getCurrentMenuTab() const;
		[[nodiscard]] CommandList* getCurrentUiList() const noexcept;
		[[nodiscard]] Command* getCurrentUiFocus() const noexcept;
		[[nodiscard]] CommandPhysical* getCurrentUiFocusPhysical() const noexcept;
		[[nodiscard]] CommandList* getCurrentMenuList() const noexcept;
		[[nodiscard]] CommandList* getCurrentMenuList(uint8_t tab_index) const noexcept;
		[[nodiscard]] Command* getCurrentMenuFocus() const noexcept;
		[[nodiscard]] CommandPhysical* getCurrentMenuFocusPhysical() const noexcept;
	private:
		static void notifyPrevFocus(ThreadContext thread_context, Direction momentum, soup::WeakRef<Command> prev_focus, CommandList* const list);
	public:
		void updateFocus(ThreadContext thread_context, Direction momentum, const soup::WeakRef<Command>& prev_focus) const;
		void updateFocus(ThreadContext thread_context, Direction momentum) const;
		void updateActiveFocus(ThreadContext thread_context, Direction momentum, const soup::WeakRef<Command>& prev_focus) const;
		void updateActiveFocus(ThreadContext thread_context, Direction momentum, Command* const prev_focus) const;
		void updateFocusedList(ThreadContext thread_context, Direction momentum, CommandList* const list) const;
		void evaluateKeyboardInput(ThreadContext thread_context);
		[[nodiscard]] bool isInMenuNavigationContext() const;

	public:
		bool sound = true;
		[[nodiscard]] bool isSoundEnabled() const;
		void queueSoundJob(std::function<void()>&& fiber_func) const;
		void queueSoundJob(ThreadContext thread_context, std::function<void()>&& func) const;
		void queueSoundJob(ThreadContext thread_context, bool play_sound, std::function<void()>&& func) const;
		static void playSound(Sound sound);
		void queueSound(Sound sound) const;
		void doSound(Sound sound, ThreadContext thread_context) const;

	public:
		ConfigTreeSimple starred_commands = ConfigTreeSimple(L"Saved Commands");

		[[nodiscard]] static bool isInteractionMenuOpen();
		[[nodiscard]] static bool isPhoneClosed();

		bool inited_netIntel = false;
	private:
		void onTickTutorial();
	public:
		void initMinimal();
	private:
		void initNonScript();
		void initScript();
		void onTickInit();
		void onTick();
	public:
		static void scriptFunc();

		void onPostTick();

		void managePlayerList(ThreadContext thread_context);
		void populatePlayerList(ThreadContext thread_context);
		[[nodiscard]] size_t getPlayerCountFromCommands() const;
		void updatePlayerCount() const;

#if BAD_BOY_ENABLED
		time_t bad_boy_timeout_until = 0;
		[[nodiscard]] bool isInBadBoyTimeout() const noexcept;
		void putInBadBoyTimeout();
#else
		[[nodiscard]] static constexpr bool isInBadBoyTimeout() noexcept
		{
			return false;
		}
#endif

		[[nodiscard]] soup::netIntel& getNetIntel() noexcept;
	};

	inline Gui g_gui{};
}
