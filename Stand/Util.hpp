#pragma once

#include <array>
#include <functional>
#include <optional>
#include <unordered_map>

#include <Windows.h>

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"
#include "natives_decl.hpp"
#include "typedecl.hpp"

#include "outfit_data.hpp"

#define TOAST_ABOVE_MAP			((toast_t)0b1)
#define TOAST_CONSOLE			((toast_t)0b10)
#define TOAST_FILE				((toast_t)0b100)
#define TOAST_WEB				((toast_t)0b1000)
#define TOAST_CHAT				((toast_t)0b10000)
#define TOAST_CHAT_TEAM			((toast_t)0b100000)
#define TOAST_DEFAULT			(TOAST_ABOVE_MAP | TOAST_WEB)
#define TOAST_LOGGER			(TOAST_CONSOLE | TOAST_FILE)
#define TOAST_ALL				(TOAST_DEFAULT | TOAST_LOGGER)

namespace Stand
{
	class Util
	{
	public:
		inline static bool unit_of_choice_is_mps = false;

		static void bypass_string_limit(void f(const char*), const std::string& message);
		static void toast(const Label& message, toast_t flags = TOAST_DEFAULT);
		static void toast(std::string&& literal_message, toast_t flags = TOAST_DEFAULT);
		static void toast(const std::wstring& literal_message, toast_t flags = TOAST_DEFAULT);
		static void toast(const Label& message, ThreadContext) = delete;
		static void toast(std::string&& literal_message, ThreadContext) = delete;
		static void ensureToasterContext(Toaster* toaster, std::function<void()>&& callback);
		static void toast_web(const Label& message);
		static void toast_web(Label&& message);
		static void toastWebLiteral(std::string&& message);
		static void toastChat(const Label& message, bool team_chat, compactplayer_t recipient = MAX_PLAYERS);
		[[nodiscard]] static Label getWotString();
		static void toast_uwotm8();
		static void toast_web_uwotm8();
		static void onPreventedCrash(const Codename& type) noexcept;
		static void onPreventedCrash(const Codename& type, const std::string& extra_data) noexcept;
		static void onPreventedCrash(const Codename& type, AbstractPlayer likely_cause) noexcept;
		static void onPreventedCrash(const Codename& type, const std::string& extra_data, AbstractPlayer likely_cause) noexcept;
		static void onPreventedCrash(const Label& type) noexcept;
		static void onPreventedCrash(const Label& type, AbstractPlayer likely_cause) noexcept;
		static void onPreventedBufferOverrun(const Codename& type) noexcept;
		static void onPreventedBufferOverrun(const Label& type) noexcept;
		[[nodiscard]] static int get_closest_hud_colour(int target_r, int target_g, int target_b, int target_a = 255);
		static void show_corner_help_impl(const char* message, bool beep);
		static void show_corner_help_impl(const std::string& message, bool beep);
		static void show_corner_help(const char* message, bool beep);
		static void show_corner_help(const std::string& message, bool beep);
		[[nodiscard]] static bool is_help_message_being_displayed(const char* message);
		[[nodiscard]] static bool is_help_message_being_displayed(const std::string& message);
		static void replace_corner_help(const char* message, const char* replacement_message);
		[[nodiscard]] static std::pair<int, const char*> mps_to_unit_of_choice_pair(float metres_per_second);
		[[nodiscard]] static std::pair<int, const wchar_t*> mps_to_unit_of_choice_pair_w(float metres_per_second);
		[[nodiscard]] static std::string mps_to_unit_of_choice(float metres_per_second);
		[[nodiscard]] static std::wstring mps_to_unit_of_choice_w(float metres_per_second);
		static bool copy_to_clipboard(const std::string& text);
		static void copy_to_clipboard(Click& click, const StringCastable& text);
		static bool copy_to_clipboard_utf8(const std::string& text);
		static void copy_to_clipboard_utf8(Click& click, const std::string& text);
		static bool copy_to_clipboard_utf16(const std::wstring& text);
		static void copy_to_clipboard_utf16(Click& click, const std::wstring& text);
		[[nodiscard]] static std::string getClipboardTextASCII();
		[[nodiscard]] static std::string getClipboardTextUTF8();
		[[nodiscard]] static std::wstring getClipboardTextUTF16();
		[[nodiscard]] static AbstractEntity getVehicle(const bool include_last_driven = true);
		[[nodiscard]] static AbstractEntity getVehicle(Click& click, const bool include_last_driven = true);
		[[nodiscard]] static AbstractEntity getCurrentlyDrivingVehicle(Click& click);
		[[nodiscard]] static AbstractEntity createPed(const int pedType, const Hash model, const v3& pos, const float heading, const bool networked = true);
		[[nodiscard]] static AbstractEntity createVehicle(const Hash hash, const v3& pos, const float heading, const bool networked = true);
		[[nodiscard]] static AbstractEntity createObject(const Hash model, const v3& pos, const bool networked = true);
		[[nodiscard]] static int get_ped_seat(const Ped& ped, const Vehicle& veh);
		[[nodiscard]] static std::string ped_model_hash_to_string(AbstractModel model);
		[[nodiscard]] static std::wstring ped_model_hash_to_localised_string(AbstractModel model);
		[[nodiscard]] static Hash string_to_ped_model_hash(const std::string& str);
		static void transform(Click& click, const AbstractModel model, std::unordered_map<std::string, std::string> outfit = {});
		static void transformNoCheck(const AbstractModel model, std::unordered_map<std::string, std::string>&& outfit = {});
		[[nodiscard]] static std::array<std::int16_t, COMPACT_OUTFIT_SIZE> get_compact_outfit_from_commands_state();
		[[nodiscard]] static std::unordered_map<std::string, std::string> get_colons_outfit_from_commands_state();
		[[nodiscard]] static std::array<std::int16_t, COMPACT_OUTFIT_SIZE> get_compact_outfit_from_ped(AbstractEntity& ped);
		[[nodiscard]] static std::unordered_map<std::string, std::string> get_colons_outfit_from_ped(AbstractEntity& ped);
		[[nodiscard]] static std::unordered_map<std::string, std::string> compact_outfit_to_colons_outfit(const AbstractModel model, const std::array<std::int16_t, COMPACT_OUTFIT_SIZE>& compact_outfit, const bool include_model);
		static void loadOutfit(Command* const command_for_warning, Click& click, const std::unordered_map<std::string, std::string>& outfit);
		static bool convertOutfitKey(std::string& key);
		static void applyOutfit(AbstractEntity& ped, const std::unordered_map<std::string, std::string>& outfit);
		static void applyOutfitToPlayerPed(const AbstractModel model, const std::unordered_map<std::string, std::string>& outfit);

		template <typename T>
		static void vector_emplace_back_unique(std::vector<T>& vec, T val)
		{
			if (std::find(vec.begin(), vec.end(), val) == vec.end())
			{
				vec.emplace_back(std::move(val));
			}
		}

		static std::vector<hash_t> getOutfitDataOrderRaw();
		static const unsigned long menu_names_version;
		[[nodiscard]] static bool upgrade_menu_names_from_colons_and_tabs_file(std::unordered_map<std::string, std::string>& data, unsigned long version);
		[[nodiscard]] static bool upgrade_menu_names_from_colons_and_tabs_file(std::unordered_map<std::string, std::string>& data);
		[[nodiscard]] static std::string upgrade_menu_path(std::string&& path, unsigned long tree_version);
		static void upgrade_menu_path(std::string& path, unsigned long tree_version);
		static void BEGIN_TEXT_COMMAND_DISPLAY_TEXT(const std::string& message);
		static void _BEGIN_TEXT_COMMAND_LINE_COUNT(const std::string& message);
		static void BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(const std::string& message);
		static void BEGIN_TEXT_COMMAND_DISPLAY_HELP(const std::string& message);
		static void _BEGIN_TEXT_COMMAND_GET_WIDTH(const std::string& message);
		static void BEGIN_TEXT_COMMAND_THEFEED_POST(const std::string& message);
		[[nodiscard]] static int8_t get_char_slot_no_notify();
		[[nodiscard]] static int8_t getCharSlot(Click& click) noexcept;
		[[nodiscard]] static int8_t getCharSlot(const Click& click) noexcept;
		[[nodiscard]] static std::string GET_LABEL_TEXT_no_replacements(hash_t label);
		[[nodiscard]] static std::string GET_LABEL_TEXT(const char* label, const bool use_replacements = true);
		[[nodiscard]] static std::string GET_LABEL_TEXT(const std::string& label, const bool use_replacements = true);
		[[nodiscard]] static std::string getLabelTextOrKey(const std::string& label, const bool use_replacements = true);
		static void cleanup_label_text(std::string& text);
		[[nodiscard]] static bool isMeleeDamageModifierLegitimate(const float modifier);
		[[nodiscard]] static bool isWeaponDamageModifierLegitimate(const float modifier);
		[[nodiscard]] static bool isMaxHealthLegitimate(const int max_health);
		[[nodiscard]] static int getMaxHealthForRank(const int rank);
		[[nodiscard]] static int get_rp_required_for_rank(const int rank);
		[[nodiscard]] static int get_car_club_rep_required_for_level(int rank);
		[[nodiscard]] static float VDIST(const float x1, const float y1, const float x2, const float y2);
		static void STAT_SET_INT_IF_LESS(const Hash hash, const int value);
		static void setWaypoint(const Vector2Plus& coords);
		static void setWaypoint2d(const rage::Vector2& coords);
		static void setWaypoint2d(const v3& coords);
		static void setWaypoint3d(const v3& coords);
		[[nodiscard]] static playerbitset_t get_session_players_bitflag();
		[[nodiscard]] static playerbitset_t get_session_players_bitflag(const std::vector<AbstractPlayer>& players);

		[[nodiscard]] static inline constexpr uint64_t i32_to_u64_trickery(int32_t i)
		{
			if (i < 0)
			{
				return uint64_t(i) & 0b0000000000000000000000000000000011111111111111111111111111111111;
			}
			else
			{
				return uint64_t(i) | 0b1111111111111111111111111111111100000000000000000000000000000000;
			}
		}

		[[nodiscard]] static std::string to_hex_string(uint16_t dec);
		[[nodiscard]] static std::string to_hex_string(uint32_t dec);
		[[nodiscard]] static std::string to_hex_string_with_0x(uint32_t dec);
		[[nodiscard]] static std::string to_padded_hex_string(int32_t dec);
		[[nodiscard]] static std::string to_padded_hex_string(uint32_t dec);
		[[nodiscard]] static std::string to_padded_hex_string(int64_t dec);
		[[nodiscard]] static std::string to_padded_hex_string(uint64_t dec);
		[[nodiscard]] static std::string to_padded_hex_string_with_0x(int32_t dec);
		[[nodiscard]] static std::string to_padded_hex_string_with_0x(uint32_t dec);
		[[nodiscard]] static std::string to_padded_hex_string_with_0x(int64_t dec);
		[[nodiscard]] static std::string to_padded_hex_string_with_0x(uint64_t dec);
		[[nodiscard]] static std::string get_module_file_name(HMODULE hmod);
		static bool bin_file(const std::wstring& path, const char* const data, const size_t data_size);
		static void bin_file_write(const std::wstring& path, const char* const data, const size_t data_size);
		static void bin_file_write(const std::wstring& path, const std::string& data);
		static player_t generate_invalid_player_id(); // generate a player id out of valid range
		static player_t get_invalid_player_id(); // gets a player id within valid range of a player not in session
		static player_t get_invalid_player_id(player_t fallback); // gets a player id within valid range of a player not in session, otherwise uses fallback
		static void ensureScriptThreadIfAllowed(ThreadContext thread_context, std::function<void()>&& func);
		static void queueJobIfAllowed(std::function<void()>&& func);
		static void ensureYieldableScriptThread(ThreadContext thread_context, std::function<void()>&& func);
		[[nodiscard]] static float angles_dist(float a1, float a2) noexcept;
		[[nodiscard]] static bool is_bigmap_active();
		[[nodiscard]] static std::string getRandomLicensePlate(int seed);
		static bool GET_SCREEN_COORD_FROM_WORLD_COORD_NO_ADJUSTMENT(float worldX, float worldY, float worldZ, float* screenX, float* screenY);
		[[nodiscard]] static bool is_legit_ambient_pickup(hash_t type, int value, hash_t model);

		template <typename T>
		static void predicate_remove(std::vector<T>& vec, std::function<bool(const T&)>&& pred)
		{
			vec.erase(
				std::remove_if(vec.begin(), vec.end(), std::move(pred)),
				vec.end()
			);
		}
	};
}
