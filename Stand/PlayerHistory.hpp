#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <soup/Promise.hpp>
#include <soup/UniquePtr.hpp>

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"
#include "typedecl.hpp"

#include "Command.hpp"
#include "HistoricPlayer.hpp"
#include "Spinlock.hpp"

namespace Stand
{
	class PlayerHistory
	{
	public:
		static inline CommandList* player_history_command = {};
		static inline std::unique_ptr<Command> retained_player_history_command = {};
		static inline Spinlock mtx{};
		static inline std::vector<soup::UniquePtr<HistoricPlayer>> player_history = {};
		static inline soup::Promise<> loaded_data{};
		static inline soup::Promise<> inited{};
		static inline CommandDivider* divider = nullptr;
		static inline cursor_t list_offset = 0;
		static inline bool needs_to_be_saved = false;
		static inline bool save_ongoing = false;
		static inline bool refuse_to_save = false;

		[[nodiscard]] static HistoricPlayer* findAccountId(int64_t account_id);
		[[nodiscard]] static HistoricPlayer* findRID(int64_t rid);
		[[nodiscard]] static HistoricPlayer* findMacId(uint32_t mac_id);
		[[nodiscard]] static HistoricPlayer* findName(const std::string& name);
		[[nodiscard]] static HistoricPlayer* getRandomEntry();
		static void manual_add_complete(const ScAccount& a);
		static void insert(int64_t rid, const std::string& name);
		static HistoricPlayer* insert(int64_t rid, uint32_t mac_id, std::string&& name, time_t time, uint8_t flags);
		[[nodiscard]] static CommandHistoricPlayer* findCommand(HistoricPlayer* hp);
		static void erase(HistoricPlayer* hp);
		static void eraseCommand(HistoricPlayer* hp);
		static void eraseAllExcept(bool keep_noted, bool keep_tracked, bool keep_whitelisted, bool keep_join_reactions);
		[[nodiscard]] static Label getDividerMenuName();
		static void updateDivider();
		static void save() noexcept;
		static void onUpdate();
		static void saveNow(); // internal use only
	private:
		static void loadData();
	public:
		static void load();

		[[nodiscard]] static bool isUniqueMacAddressHash(uint32_t hash) noexcept;
	};
}
