#include "PlayerHistory.hpp"

#include <filesystem>
#include <memory>

#include <fmt/core.h>

#include <soup/aes.hpp>
#include <soup/filesystem.hpp>
#include <soup/ObfusString.hpp>
#include <soup/rand.hpp>

#include "Auth.hpp"
#include "BinaryStream.hpp"
#include "CommandDivider.hpp"
#include "CommandHistoricPlayer.hpp"
#include "CommandListHistoricPlayers.hpp"
#include "CommandPlayerHistory.hpp"
#include "Exceptional.hpp"
#include "FlowEventReaction.hpp"
#include "get_appdata_path.hpp"
#include "Gui.hpp"
#include "PlayerHistoryPacket.hpp"
#include "reversible_scramble.hpp"
#include "rlGamerId.hpp"
#include "Sanity.hpp"
#include "ScAccount.hpp"
#include "StringUtils.hpp"

#define LOG_UNUNIQUE_MAC_IDS false

#if LOG_UNUNIQUE_MAC_IDS
#include "FileLogger.hpp"
#include "Util.hpp"
#endif

#define PROFILE_HISTORY_LOOKUP false
#define PROFILE_HISTORY_LOAD false

#if PROFILE_HISTORY_LOOKUP || PROFILE_HISTORY_LOAD
#include "TimedCall.hpp"
#endif

namespace Stand
{
	HistoricPlayer* PlayerHistory::findAccountId(int64_t account_id)
	{
		for (const auto& hp : player_history)
		{
			if (hp->account_id == account_id)
			{
				return hp.get();
			}
		}
		return nullptr;
	}

	HistoricPlayer* PlayerHistory::findRID(int64_t rid)
	{
		for (const auto& hp : player_history)
		{
			if (hp->rid == rid)
			{
				return hp.get();
			}
		}
		return nullptr;
	}

	HistoricPlayer* PlayerHistory::findMacId(uint32_t mac_id)
	{
		for (const auto& hp : player_history)
		{
			if (hp->mac_id == mac_id)
			{
				return hp.get();
			}
		}
		return nullptr;
	}

	HistoricPlayer* PlayerHistory::findName(const std::string& name)
	{
		for (const auto& hp : player_history)
		{
			if (hp->name == name)
			{
				return hp.get();
			}
		}
		return nullptr;
	}

	HistoricPlayer* PlayerHistory::getRandomEntry()
	{
		const size_t size = player_history.size();
		if (size != 0)
		{
			return player_history.at(soup::rand(0, size - 1)).get();
		}
		return nullptr;
	}

	void PlayerHistory::manual_add_complete(const ScAccount& a)
	{
		auto hp = findRID(a.rid);
		if (hp != nullptr)
		{
			findCommand(hp)
				->open(TC_OTHER);
		}
		else
		{
			insert(a.rid, a.name);
		}
	}

	void PlayerHistory::insert(int64_t rid, const std::string& name)
	{
		insert(rid, 0, std::string(name), 0, 0);
	}

	HistoricPlayer* PlayerHistory::insert(int64_t rid, uint32_t mac_id, std::string&& name, time_t time, uint8_t flags)
	{
		HistoricPlayer* entry;
		EXCEPTIONAL_LOCK(mtx)
		entry = player_history.emplace_back(soup::make_unique<HistoricPlayer>(0, rid, mac_id, std::move(name), time, time, 0, std::string{}, 0)).get();
		EXCEPTIONAL_UNLOCK(mtx)
		entry->flags = flags;
		player_history_command->children.emplace(player_history_command->children.begin() + list_offset, std::make_unique<CommandHistoricPlayer>(entry));
		if (player_history_command->m_cursor > list_offset)
		{
			player_history_command->m_cursor++;
			if (player_history_command->isCurrentUiList())
			{
				player_history_command->fixCursorAndOffset();
			}
		}
		onUpdate();
		updateDivider();

		if (!g_auth.isStateConsistent())
		{
			Sanity::playerhistory_insert_in_auth_inconsistent_state = true;
		}

		return entry;
	}

	void PlayerHistory::erase(HistoricPlayer* hp)
	{
		SOUP_ASSERT(!hp->inSession().isValid());
		eraseCommand(hp); // erase command first so we won't have a stale pointer and don't need to hold 2 locks at once
		EXCEPTIONAL_LOCK(mtx)
		hp->preRemove();
		for (auto i = player_history.begin(); i != player_history.end(); ++i)
		{
			if (i->get() == hp)
			{
				player_history.erase(i);
				save();
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
	}

	void PlayerHistory::eraseCommand(HistoricPlayer* hp)
	{
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		for (auto i = player_history_command->children.begin() + list_offset; i != player_history_command->children.end(); ++i)
		{
			if (((CommandHistoricPlayer*)i->get())->player == hp)
			{
				player_history_command->removeChild(i);
				break;
			}
		}
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
	}

	void PlayerHistory::eraseAllExcept(bool keep_noted, bool keep_tracked, bool keep_whitelisted, bool keep_join_reactions)
	{
		while (true)
		{
			if (mtx.tryLock())
			{
				if (g_gui.root_mtx.tryLockWrite())
				{
					break; // all locks acquired, proceed to main logic
				}
				mtx.unlock();
			}
		}
		__try
		{
			bool modified = false;
			auto hist_it = player_history.end();
			auto cmd_it = player_history_command->children.begin() + list_offset;
			for (; cmd_it != player_history_command->children.end(); )
			{
				--hist_it;
				HistoricPlayer* hp = *hist_it;
				if (!hp->inSession().isValid()
					&& (!keep_noted || hp->note.empty())
					&& (!keep_tracked || !hp->isTracked())
					&& (!keep_whitelisted || !(hp->flags & HP_WHITELIST))
					&& (!keep_join_reactions || !hp->join_reactions)
					)
				{
					hp->preRemove();
					hist_it = player_history.erase(hist_it);
					cmd_it = player_history_command->children.erase(cmd_it);
					modified = true;
				}
				else
				{
					++cmd_it;
				}
			}
			if (modified)
			{
				onUpdate();
				updateDivider();
			}
		}
		__EXCEPTIONAL()
		{
		}
		mtx.unlock();
		g_gui.root_mtx.unlockWrite();
	}

	CommandHistoricPlayer* PlayerHistory::findCommand(HistoricPlayer* hp)
	{
		for (auto i = player_history_command->children.begin() + list_offset; i != player_history_command->children.end(); ++i)
		{
			if (((CommandHistoricPlayer*)i->get())->player == hp)
			{
				return (CommandHistoricPlayer*)i->get();
			}
		}
		return nullptr;
	}

	Label PlayerHistory::getDividerMenuName()
	{
		std::string name{ LANG_GET("PLYHIST") };
		name.append(" (").append(fmt::to_string(player_history.size())).push_back(')');
		return LIT(std::move(name));
	}

	void PlayerHistory::updateDivider()
	{
		divider->setMenuName(getDividerMenuName());
	}

	void PlayerHistory::save() noexcept
	{
		needs_to_be_saved = true;
	}

	void PlayerHistory::onUpdate()
	{
		save();
		if (player_history_command != nullptr)
		{
			player_history_command->processChildrenUpdate();
		}
	}

#define FORMAT_VERSION 10

	void PlayerHistory::saveNow()
	{
		if (refuse_to_save)
		{
			needs_to_be_saved = false;
			return;
		}
		if (save_ongoing)
		{
			return;
		}
		save_ongoing = true;
		Exceptional::createManagedExceptionalThread(__FUNCTION__, []
		{
#ifdef STAND_DEBUG
			if (!needs_to_be_saved)
			{
				Exceptional::report("History save thread running but needs_to_be_saved was false");
			}
#endif
			needs_to_be_saved = false;

			std::string history_bin{};
			{
				PlayerHistoryPacket<HistoricPlayerPacketV10> pack{};
				EXCEPTIONAL_LOCK(mtx)
				for (auto& entry : player_history)
				{
					HistoricPlayerPacketV10 pp;
					pp.rid = entry->rid;
					pp.name = entry->name;
					pp.first_seen = entry->first_seen;
					pp.last_seen = entry->last_seen;
					pp.note = entry->note;
					pp.flags = entry->flags;
					pp.join_reactions = entry->join_reactions;
					pp.mac_id = entry->mac_id;
					pp.account_id = entry->account_id;
					pack.players.emplace_back(std::move(pp));
				}
				EXCEPTIONAL_UNLOCK(mtx)
				history_bin = pack.toBinaryString();
			}

			const auto bin = get_appdata_path(soup::ObfusString("Player History.bin"));
			const auto bin_tmp = get_appdata_path(soup::ObfusString("Player History.bin.tmp"));

			{
				BinaryOutStream file_out(bin_tmp);
				file_out.write<uint8_t>(FORMAT_VERSION);
				file_out.write(history_bin.data(), history_bin.size());
			}

			soup::filesystem::replace(bin, bin_tmp);

			save_ongoing = false;
		});
	}

	void PlayerHistory::loadData()
	{
		needs_to_be_saved = false;
		//player_history.clear();

		const auto bin = get_appdata_path(soup::ObfusString("Player History.bin"));

		// Handle the case where history was saved to tmp file only (bug in 24.8.1-24.8.4)
		{
			const auto bin_tmp = get_appdata_path(soup::ObfusString("Player History.bin.tmp"));
			if (!std::filesystem::exists(bin) && std::filesystem::exists(bin_tmp))
			{
				std::filesystem::rename(bin_tmp, bin);
			}
		}

		// Handle the case where there was an abrupt shutdown during the history write on a pre-24.8.1 version.
		{
			const auto bin_bak = get_appdata_path(soup::ObfusString("Player History.bin.bak"));
			if (soup::filesystem::filesize(bin_bak) > soup::filesystem::filesize(bin))
			{
				std::filesystem::rename(bin_bak, bin);
			}
		}

		std::string history_bin;
#if PROFILE_HISTORY_LOAD
		TimedCall::once("load to memory", [&]
#endif
		{
			history_bin = soup::string::fromFile(bin);
		}
#if PROFILE_HISTORY_LOAD
		);
#endif
		if (!history_bin.empty())
		{
			const auto format = (uint8_t)history_bin.at(0);
			if (format > FORMAT_VERSION)
			{
				refuse_to_save = true;
			}
			else
			{
				if (format >= 6)
				{
#if PROFILE_HISTORY_LOAD
					TimedCall::once("erase first byte", [&]
#endif
					{
						history_bin.erase(0, 1);
					}
#if PROFILE_HISTORY_LOAD
					);
#endif
					if (format >= 10)
					{
						PlayerHistoryPacket<HistoricPlayerPacketV10> pack;
						bool unpack_result = false;
#if PROFILE_HISTORY_LOAD
						TimedCall::once("pack.fromBinary", [&]
						{
#endif
							unpack_result = pack.fromBinary(history_bin);
#if PROFILE_HISTORY_LOAD
						});
#endif
						if (unpack_result)
						{
#if PROFILE_HISTORY_LOAD
							TimedCall::once("emplace to player_history", [&]
							{
#endif
								player_history.reserve(pack.players.size());
								for (auto& p : pack.players)
								{
									/*if (p.v7.v6.rid == 99453882)
									{
										p.v7.v6.scrambled_gamer_id = 0x97EE93E57168877Dull;
									}*/
									auto* hp = player_history.emplace_back(soup::make_unique<HistoricPlayer>(
										p.account_id,
										(int64_t)p.rid,
										p.mac_id,
										std::move(p.name),
										(unixtime_t)p.first_seen,
										(unixtime_t)p.last_seen,
										(floweventreaction_t)p.join_reactions,
										std::move(p.note),
										(uint8_t)p.flags
									)).get();
									hp->onImportedFromFormatVersion9OrBelow();
									hp->processFlags();
								}
#if PROFILE_HISTORY_LOAD
							});
#endif
						}
					}
					else if ((history_bin.size() % 16) == 0)
					{
#if PROFILE_HISTORY_LOAD
						TimedCall::once("decrypt", [&]
						{
#endif
							soup::aes::cbcDecrypt(
								(uint8_t*)history_bin.data(), history_bin.size(),
								(const uint8_t*)"\x27\x36\x72\x92\x48\x72\x40\x46\x82\x72\x48\x05\x18\x85\x47\x17", 16,
								(const uint8_t*)"\x71\x18\x90\x40\x74\x95\x37\x07\x81\x42\x92\x70\x16\x17\x86\x45"
							);
#if PROFILE_HISTORY_LOAD
						});
#endif

						if (format >= 9)
						{
							SOUP_ASSERT(soup::aes::pkcs7Unpad(history_bin));
						}

						if (format >= 8)
						{
							PlayerHistoryPacket<HistoricPlayerPacketV8> pack;
							bool unpack_result = false;
#if PROFILE_HISTORY_LOAD
							TimedCall::once("pack.fromBinary", [&]
							{
#endif
								unpack_result = pack.fromBinary(history_bin);
#if PROFILE_HISTORY_LOAD
							});
#endif
							if (unpack_result)
							{
#if PROFILE_HISTORY_LOAD
								TimedCall::once("emplace to player_history", [&]
								{
#endif
									player_history.reserve(pack.players.size());
									for (auto& p : pack.players)
									{
										/*if (p.v7.v6.rid == 99453882)
										{
											p.v7.v6.scrambled_gamer_id = 0x97EE93E57168877Dull;
										}*/
										auto* hp = player_history.emplace_back(soup::make_unique<HistoricPlayer>(
											//p.v7.v6.scrambled_gamer_id,
											p.account_id,
											(int64_t)p.v7.v6.rid,
											p.v7.mac_id,
											std::move(p.v7.v6.name),
											(unixtime_t)p.v7.v6.first_seen,
											(unixtime_t)p.v7.v6.last_seen,
											(floweventreaction_t)p.v7.v6.join_reactions,
											std::move(p.v7.v6.note),
											(uint8_t)p.v7.v6.flags
										)).get();
										hp->onImportedFromFormatVersion9OrBelow();
										hp->processFlags();
									}
#if PROFILE_HISTORY_LOAD
								});
#endif
							}
						}
						else if (format >= 7)
						{
							PlayerHistoryPacket<HistoricPlayerPacketV7> pack;
							if (pack.fromBinary(history_bin))
							{
								player_history.reserve(pack.players.size());
								for (auto& p : pack.players)
								{
									auto* hp = player_history.emplace_back(soup::make_unique<HistoricPlayer>(
										//p.v6.scrambled_gamer_id,
										0,
										(int64_t)p.v6.rid,
										p.mac_id,
										std::move(p.v6.name),
										(unixtime_t)p.v6.first_seen,
										(unixtime_t)p.v6.last_seen,
										(floweventreaction_t)p.v6.join_reactions,
										std::move(p.v6.note),
										(uint8_t)p.v6.flags
									)).get();
									hp->onImportedFromFormatVersion9OrBelow();
									hp->processFlags();
								}
							}
						}
						else
						{
							PlayerHistoryPacket<HistoricPlayerPacketV6> pack;
							if (pack.fromBinary(history_bin))
							{
								player_history.reserve(pack.players.size());
								std::unordered_set<uint64_t> all_rids{};
								for (auto& p : pack.players)
								{
									if (all_rids.contains(p.rid))
									{
										continue;
									}
									all_rids.emplace(p.rid);
									auto* hp = player_history.emplace_back(soup::make_unique<HistoricPlayer>(
										//p.scrambled_gamer_id,
										0,
										(int64_t)p.rid,
										0,
										std::move(p.name),
										(unixtime_t)p.first_seen,
										(unixtime_t)p.last_seen,
										(floweventreaction_t)p.join_reactions,
										std::move(p.note),
										(uint8_t)p.flags
									)).get();
									hp->onImportedFromFormatVersion9OrBelow();
									hp->processFlags();
								}
							}
						}
					}
				}
				needs_to_be_saved = (format != FORMAT_VERSION);
			}
		}
	}

	void PlayerHistory::load()
	{
		__try
		{
			loadData();
		}
		__EXCEPTIONAL()
		{
		}
		loaded_data.fulfil();

#if LOG_UNUNIQUE_MAC_IDS
		std::unordered_set<uint32_t> mac_ids{};
		std::unordered_set<uint32_t> ununique_mac_ids{};
		for (const auto& hp : player_history)
		{
			if (hp->mac_id != 0)
			{
				if (mac_ids.contains(hp->mac_id))
				{
					if (!ununique_mac_ids.contains(hp->mac_id))
					{
						ununique_mac_ids.emplace(hp->mac_id);
						//g_logger.log(Util::to_padded_hex_string_with_0x(hp->mac_id));
					}
				}
				else
				{
					mac_ids.emplace(hp->mac_id);
				}
			}
		}
		for (const auto& mac_id : ununique_mac_ids)
		{
			std::string names;
			for (const auto& hp : player_history)
			{
				if (hp->mac_id == mac_id)
				{
					StringUtils::list_append(names, hp->name);
				}
			}
			g_logger.log(fmt::format("{}: {}", Util::to_padded_hex_string_with_0x(mac_id), names));
		}
#endif

		std::vector<std::unique_ptr<Command>> replacement_children{};
#if PROFILE_HISTORY_LOAD
		TimedCall::once("make replacement_children", [&]
		{
#endif
			replacement_children.reserve(player_history.size() + 1);
			divider = replacement_children.emplace_back(player_history_command->makeChild<CommandDivider>(getDividerMenuName()))->as<CommandDivider>();
			list_offset = (player_history_command->children.size() + 1);
			player_history_command->children.reserve(player_history.size());
			for (auto i = player_history.crbegin(); i != player_history.crend(); i++)
			{
				replacement_children.emplace_back(std::make_unique<CommandHistoricPlayer>(i->get()));
			}
#if PROFILE_HISTORY_LOAD
		});
#endif
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		auto offset = 0;
		for (auto i = player_history_command->children.begin(); i != player_history_command->children.end(); ++i)
		{
			replacement_children.insert(replacement_children.begin() + (offset++), std::move((*i)));
		}
		player_history_command->children = std::move(replacement_children);
		player_history_command->as<CommandPlayerHistory>()->starred->populate();
		player_history_command->as<CommandPlayerHistory>()->tracked->populate();
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
		inited.fulfil();
	}

	bool PlayerHistory::isUniqueMacAddressHash(uint32_t hash) noexcept
	{
		switch (hash)
		{
		case 0x99E6C95F: // #1 least unique mac adddress: 00-1A-7D-DA-71-15
		case 0xBF3893F2: // #2 least unique mac addresss: 00-1A-7D-DA-71-13
		case 0xE2B45AE9: // #3 least unique mac addresss: 00-1A-7D-DA-71-11
		case 0x9544A313:
		case 0x5D24849A:
		case 0x4FF0B568:
		case 0x979DC927:
		case 0xAC3FEE01:
		case 0x837F82BC:
		case 0xA4694B8B:
		case 0x0E97DF03:
		case 0xD37B6611:
		case 0xF449FE14:
		case 0x71A878D7:
		case 0xD1D5D5ED:
		case 0x72231534:
		case 0xD0F7376F:
		case 0x43659C52:
		case 0x64949AAD:
		case 0xA6A8E2E3:
		case 0xF20D799F:
		case 0xCF4DB420:
		case 0x26E06348:
		case 0xE9FD772A:
		case 0x9CC425E0:
		case 0x8808AB46:
			return false;
		}
		return true;
	}
}
