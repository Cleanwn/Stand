#include "PlayerProvider.hpp"

#include <unordered_set>

#include "AbstractPlayer.hpp"
#include "CommandPlayer.hpp"
#include "CommandToggleNoCorrelation.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "PlayerProviderSingle.hpp"
#include "Script.hpp"
#include "regular_event.hpp"

namespace Stand
{
	std::vector<AbstractPlayer> PlayerProvider::getPlayers() const
	{
		return getPlayers(single);
	}

	AbstractPlayer PlayerProvider::getPlayer() const
	{
		if (single)
		{
			return ((const PlayerProviderSingle*)this)->getPlayer();
		}
		return MAX_PLAYERS;
	}

	bool PlayerProvider::isUser() const
	{
		return single && ((const PlayerProviderSingle*)this)->p == g_player;
	}

	void PlayerProvider::toggleFiberBatch(CommandPhysical* const cmd, std::function<void(const std::vector<AbstractPlayer>&)>&& callback) const
	{
		toggleFiberBatch(cmd->getPathEnFromParent(COMMAND_LIST_PLAYER), std::move(callback));
	}

	static std::unordered_set<std::string> active_toggle_batches;

	void PlayerProvider::toggleFiberBatch(const std::string& english_command_path_from_player_command, std::function<void(const std::vector<AbstractPlayer>&)>&& callback) const
	{
		if (active_toggle_batches.find(english_command_path_from_player_command) != active_toggle_batches.end())
		{
			return;
		}
		active_toggle_batches.emplace(english_command_path_from_player_command);
		auto* const all_players_toggle = ((const CommandToggleNoCorrelation*)((const CommandList*)((const CommandList*)g_gui.root_list->resolveChildByMenuName(LOC("PLYLST")))->resolveChildByMenuName(LOC("ALLPLY")))->resolveCommandEnglish(english_command_path_from_player_command));
		do
		{
			std::unordered_set<Player> included_players;
			std::vector<AbstractPlayer> players;
			if (all_players_toggle->m_on)
			{
				players = excludes.getNonExcludedPlayers(true);
				for (const AbstractPlayer p : players)
				{
					included_players.emplace(p.p);
				}
			}
			EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
			// Note: No reference here because we could hit a race condition where command is set to nullptr after we checked that it's not a nullptr!
			for (const auto command : g_gui.player_commands)
			{
				if (command == nullptr)
				{
					continue;
				}
				AbstractPlayer ap(command->pp->getPlayers(true).at(0));
				if (included_players.find(ap) == included_players.end())
				{
					auto* const toggle = ((const CommandToggleNoCorrelation*)command->resolveCommandEnglish(english_command_path_from_player_command));
					if (toggle != nullptr && toggle->m_on)
					{
						players.emplace_back(ap);
					}
				}
			}
			EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
			if (!all_players_toggle->m_on && players.empty())
			{
				break;
			}
			__try
			{
				callback(players);
			}
			__EXCEPTIONAL()
			{
			}
			Script::current()->yield();
		} while (true);
		active_toggle_batches.erase(english_command_path_from_player_command);
	}

	void PlayerProvider::toggleBatch(const std::string& english_command_path_from_player_command, std::function<void(const std::vector<AbstractPlayer>&)>&& callback) const
	{
		if (active_toggle_batches.find(english_command_path_from_player_command) == active_toggle_batches.end())
		{
			FiberPool::queueJob([english_command_path_from_player_command, callback{std::move(callback)}]
			{
				active_toggle_batches.emplace(english_command_path_from_player_command);
				auto* const all_players_toggle = ((const CommandToggleNoCorrelation*)((const CommandList*)((const CommandList*)g_gui.root_list->resolveChildByMenuName(LOC("PLYLST")))->resolveChildByMenuName(LOC("ALLPLY")))->resolveCommandEnglish(english_command_path_from_player_command));
				reScriptTickEvent::registerHandlerInThreadContext([=]()
				{
					std::unordered_set<Player> included_players;
					std::vector<AbstractPlayer> players;
					if (all_players_toggle->m_on)
					{
						players = excludes.getNonExcludedPlayers(true);
						for (const AbstractPlayer p : players)
						{
							included_players.emplace(p.p);
						}
					}
					EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
					// Note: No reference here because we could hit a race condition where command is set to nullptr after we checked that it's not a nullptr!
					for (const auto command : g_gui.player_commands)
					{
						if (command == nullptr)
						{
							continue;
						}
						AbstractPlayer ap(command->pp->getPlayers(true).at(0));
						if (included_players.find(ap) == included_players.end())
						{
							auto* const toggle = ((const CommandToggleNoCorrelation*)command->resolveCommandEnglish(english_command_path_from_player_command));
							if (toggle != nullptr && toggle->m_on)
							{
								players.emplace_back(ap);
							}
						}
					}
					EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
					if (!all_players_toggle->m_on && players.empty())
					{
						active_toggle_batches.erase(english_command_path_from_player_command);
						return false;
					}
					callback(players);
					return true;
				}, std::move(std::string(english_command_path_from_player_command)));
			});
		}
	}
}
