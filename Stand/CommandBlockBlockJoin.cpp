#include "CommandBlockBlockJoin.hpp"

#include "AbstractPlayer.hpp"
#include "CNetworkSession.hpp"
#include "get_current_time_millis.hpp"
#include "netComplaintMsg.hpp"
#include "pointers.hpp"
#include "rlGamerInfo.hpp"
#include "ScriptGlobal.hpp"
#include "TransitionState.hpp"
#include "Util.hpp"

#ifdef STAND_DEBUG
#include "drawDebugText.hpp"
#endif

namespace Stand
{
	SessionPlayer::SessionPlayer(const rage::rlGamerInfo& info)
		: discovery(get_current_time_millis()), rid(info.getHandle().rockstar_id), name(info.name), peerId(info.peer.id)
	{
	}

	bool SessionPlayer::operator==(const SessionPlayer& b) const noexcept
	{
		return rid == b.rid;
	}

	CommandBlockBlockJoin::CommandBlockBlockJoin(CommandList* const parent)
		: CommandToggleRegular(parent, LOC("PTX_BJ"), CMDNAMES_OBF("blockjoinkarma", "blockblockjoins"), LOC("PTX_BJ_H"))
	{
	}

	static void processPlayers(uint8_t detection_id, std::vector<SessionPlayer>& players, bool(*is_established)(SessionPlayer&))
	{
		std::unordered_set<int64_t> blockers{};
		std::vector<SessionPlayer*> to_remove{};
		for (auto i = players.begin(); i != players.end(); )
		{
#ifdef STAND_DEBUG
			if (GET_MILLIS_SINCE(i->discovery) > 1000)
			{
				drawDebugText(fmt::format("[{}] Waiting for {} ({})", detection_id, i->name, GET_MILLIS_SINCE(i->discovery)));
			}
#endif
			if (is_established(*i))
			{
				//Util::toast(fmt::format("Connection to {} established in {} ms", i->name, GET_MILLIS_SINCE(i->discovery)), TC_SCRIPT_NOYIELD);
				i = players.erase(i);
				continue;
			}
			else if (GET_MILLIS_SINCE(i->discovery) > 10000)
			{
				blockers.emplace(i->rid);
				if (GET_MILLIS_SINCE(i->discovery) > 15000)
				{
					to_remove.emplace_back(&*i);
				}
			}
			++i;
		}
		if (blockers.size() < 3)
		{
			for (const auto& sp : to_remove)
			{
				std::string name = sp->name;
				name.append(" (").append(fmt::to_string(sp->rid)).push_back(')');
				std::string msg = LANG_FMT("PTX_BJ_T2", std::move(name));
				msg.append(" (");
				msg.append(fmt::to_string(detection_id));
				msg.push_back(')');
				Util::toast(std::move(msg), CommandBlockBlockJoin::toast_config.getToastFlags());
				if (!g_player.isHost())
				{
					rage::netComplaintMsg packet;
					packet.sender = g_player.getPeerId();
					packet.targets[0] = sp->peerId;
					packet.num_targets = 1;
					pointers::send_netComplaintMsg(*pointers::connection_mgr, AbstractPlayer::getHost().getEndpoint()->GetAddress(), (*pointers::network_session)->session->channel_id, &packet, 0);

#if false
					FiberPool::queueJob([]
					{
						Script::current()->yield(10000);
						if (ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() != TRANSITION_STATE_WAIT_JOIN_FM_SESSION)
						{
							return;
						}
						Util::toast("Still on WAIT_JOIN_FM_SESSION, forcing through.");

						*(uint8_t*)IDA_ADDR(00007FF7C1786D44) = 0xb0;
						*(uint8_t*)IDA_ADDR(00007FF7C1786D45) = 0x01;
						*(uint8_t*)IDA_ADDR(00007FF7C1786D46) = 0xc3;
						Script::current()->yield();
						*(uint8_t*)IDA_ADDR(00007FF7C1786D44) = 0x48;
						*(uint8_t*)IDA_ADDR(00007FF7C1786D45) = 0x89;
						*(uint8_t*)IDA_ADDR(00007FF7C1786D46) = 0x5c;

						Script::current()->yield(5000);

						if (!AbstractPlayer::getScriptHost().exists())
						{
							Util::toast("No SH, up for grabs then.");
							ScriptHostUtil::takeIt();
						}
					});
#endif
				}
				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					if (p.getRockstarId() == sp->rid)
					{
						if (g_player.isHost())
						{
							p.kickHostNoprekick(AbstractPlayer::DESYNC);
						}
						else
						{
#if HAVE_BREAKUP_KICK
							p.remoteDesync(rage::rlGamerHandle(sp->rid));
							p.desyncBreakup();
#else
#endif
						}
					}
#if HAVE_BREAKUP_KICK
					else if (p.isHost())
					{
						p.remoteDesync(rage::rlGamerHandle(sp->rid));
					}
#endif
				}
				players.erase(std::find(players.begin(), players.end(), *sp));
			}
		}
	}

	void CommandBlockBlockJoin::onChangeInner(Click& click)
	{
		if (m_on)
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);
				if (ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() == TRANSITION_STATE_WAIT_JOIN_FM_SESSION)
				{
					processPlayers(1, CommandBlockBlockJoin::session_players, [](SessionPlayer& p)
					{
						return AbstractPlayer::fromRockstarId(p.rid).isValid();
					});	
				}
				processPlayers(2, CommandBlockBlockJoin::pending_object_ids, [](SessionPlayer& sp)
				{
					return !AbstractPlayer::fromRockstarId(sp.rid).isValid();
				});
				processPlayers(3, CommandBlockBlockJoin::pending_script_join_acks, [](SessionPlayer& sp)
				{
					return !AbstractPlayer::fromRockstarId(sp.rid).isValid();
				});
				HANDLER_END;
			});
		}
	}
}
