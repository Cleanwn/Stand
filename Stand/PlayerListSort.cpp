#include "PlayerListSort.hpp"

#include <algorithm>

#include "AbstractEntity.hpp"
#include "CNetworkSession.hpp"
#include "CommandPlayer.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "netPeerId.hpp"
#include "pointers.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	bool PlayerListSort::discovery(const CommandPlayer* const a, const CommandPlayer* const b)
	{
		return a->discovery > b->discovery;
	}

	bool PlayerListSort::slot(const CommandPlayer* const a, const CommandPlayer* const b)
	{
		return a->getPlayer() < b->getPlayer();
	}

	bool PlayerListSort::rank(const CommandPlayer* const a, const CommandPlayer* const b)
	{
		return a->getPlayer().getRank() > b->getPlayer().getRank();
	}

	bool PlayerListSort::money(const CommandPlayer* const a, const CommandPlayer* const b)
	{
		return a->getPlayer().getMoney() > b->getPlayer().getMoney();
	}

	bool PlayerListSort::distance(const CommandPlayer* const a, const CommandPlayer* const b)
	{
		auto my_pos = g_player_ped.getPos();
		return a->getPlayer().getPos().distance(my_pos) < b->getPlayer().getPos().distance(my_pos);
	}

	bool PlayerListSort::kd(const CommandPlayer* const a, const CommandPlayer* const b)
	{
		return a->getPlayer().getKd() > b->getPlayer().getKd();
	}

	bool PlayerListSort::name(const CommandPlayer* const a, const CommandPlayer* const b)
	{
		std::string a_name = a->getPlayerName();
		std::string b_name = b->getPlayerName();
		StringUtils::simplify(a_name);
		StringUtils::simplify(b_name);
		return a_name < b_name;
	}

	bool PlayerListSort::host_queue_impl(AbstractPlayer a, AbstractPlayer b)
	{
		if (a.isHost())
		{
			return true;
		}
		if (b.isHost())
		{
			return false;
		}
		if (!a.canBecomeHost())
		{
			return false;
		}
		if (!b.canBecomeHost())
		{
			return true;
		}
		return AbstractPlayer::host_queue_sort_algos[(*pointers::network_session)->host_queue_sort_algo](a, b);
	}

	bool PlayerListSort::host_queue(const CommandPlayer* const a, const CommandPlayer* const b)
	{
		return host_queue_impl(a->getPlayer(), b->getPlayer());
	}

	bool PlayerListSort::operator()(const std::unique_ptr<Command>& _a, const std::unique_ptr<Command>& _b)
	{
		auto a = (const CommandListPlayer*)_a.get();
		auto b = (const CommandListPlayer*)_b.get();

		// Settings
		if (a->type == COMMAND_LIST)
		{
			return true;
		}
		if (b->type == COMMAND_LIST)
		{
			return false;
		}

		// All Players
		if (a->type == COMMAND_LIST_PLAYER && !a->pp->single)
		{
			return true;
		}
		if (b->type == COMMAND_LIST_PLAYER && !b->pp->single)
		{
			return false;
		}

		// Divider
		if (a->type == COMMAND_DIVIDER)
		{
			return true;
		}
		if (b->type == COMMAND_DIVIDER)
		{
			return false;
		}

		bool a_hoist = false;
		bool b_hoist = false;
		for (const auto& flag : hoisted_flags)
		{
			if (a->as<CommandPlayer>()->getFlag(flag))
			{
				a_hoist = true;
			}
			if (b->as<CommandPlayer>()->getFlag(flag))
			{
				b_hoist = true;
			}
		}
		if (a_hoist && !b_hoist)
		{
			return true;
		}
		if (b_hoist && !a_hoist)
		{
			return false;
		}

		bool res = algo((const CommandPlayer*)a, (const CommandPlayer*)b);
		if (desc)
		{
			res = !res;
		}
		return res;
	}

	bool PlayerListSort::isUpToDate()
	{
		bool ret = true;
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		auto i = g_gui.player_list->children.begin();
		//if (i != g_gui.m_player_list->children.end())
		{
			auto j = (i + 1);
			for (; j != g_gui.player_list->children.end(); ++i, ++j)
			{
				if (!PlayerListSort()(*i, *j))
				{
					ret = false;
					break;
				}
			}
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		return ret;
	}

	void PlayerListSort::update()
	{
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		std::sort(g_gui.player_list->children.begin(), g_gui.player_list->children.end(), PlayerListSort());
		flags_have_changed_since_last_sort = false;
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
		g_gui.player_list->processChildrenUpdate();
	}

	bool PlayerListSort::isDistanceBased() noexcept
	{
		return algo == &distance;
	}
}
