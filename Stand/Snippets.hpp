#pragma once

#include "CNetworkSession.hpp"
#include "netEvent.hpp"

namespace Stand
{
	struct Snippets
	{
		// Hooks

		static void onInit() // TC_SCRIPT_YIELDABLE
		{

		}

		static void onTick()
		{

		}

		// Snippets

		static void showBlacklist()
		{
			for (auto node = (*pointers::network_session)->blacklist.first; node; node = node->next)
			{
				drawDebugText(node->gamer_handle.rockstar_id);
			}
		}

		template <typename T>
		static void emulateMessageRecv(uint32_t channelId, const T& msg)
		{
			char buf[0x500];
			unsigned bufsize;

			msg.Export<T>(buf, sizeof(buf), &bufsize);

			rage::netEventFrameReceived evt;
			evt.channel_id = channelId;
			evt.unk_thisptr = &evt;
			evt.data = reinterpret_cast<rage::ReceivedMessageData*>(&buf);
			evt.size = bufsize;

			// b2808
			using CNetworkSession_OnConnectionEvent_t = void(__fastcall*)(CNetworkSession*, rage::netConnectionManager*, rage::netEvent*);
			((CNetworkSession_OnConnectionEvent_t)IDA_ADDR(00007FF62EB41DE0))(*pointers::network_session, (*pointers::connection_mgr), &evt);
		}

		static void showRateLimitUsage() // for onTick
		{
			for (const auto& p : AbstractPlayer::listAll())
			{
				if (auto cmd = p.getCommand())
				{
					if (cmd->create_automobile_ratelimit.isInWindow())
					{
						drawDebugText(fmt::format("{}: {}", p.getName(), cmd->create_automobile_ratelimit.window_reqs));
					}
				}
			}
		}

		static void simulateMessagesAgainstAdFilter() // for onInit
		{
			while (!g_player.getCommand())
			{
				Script::current()->yield();
			}
			std::ifstream f{ R"(C:\Users\Sainan\Desktop\messages.txt)" };
			std::string line;
			size_t count = 0;
			while (std::getline(f, line))
			{
				auto lvl = g_player.getMessageAdLevel(line);
				if (lvl != 0)
				{
					g_logger.log(fmt::format("{} <- {}", lvl, line));
				}
				++count;
			}
			g_logger.log(fmt::format("That was {} messages", count));
		}

		static void findGamerInSolo() // for onInit
		{
			size_t hp_i = PlayerHistory::player_history.size() - 1;

			RemoteGamer* pool[RemoteGamer::MAX_BULK_REQUESTS];
			for (auto& g : pool)
			{
				findGamerInSolo_fillSlot(hp_i, g);
			}

			while (hp_i != 0)
			{
				for (auto& g : pool)
				{
					drawDebugText(g->rid);

					if (g->isQueuedOrPending())
					{
						continue;
					}

					if (g->online_status == RemoteGamer::ONLINE_SOLO)
					{
						Util::toast(fmt::format("{} is in a solo", g->rid), TOAST_ALL);
						return;
					}

					findGamerInSolo_fillSlot(hp_i, g);
				}

				Script::current()->yield();
			}
		}

		static void findGamerInSolo_fillSlot(size_t& hp_i, RemoteGamer*& out_g)
		{
			RemoteGamer* g = nullptr;
			while (g == nullptr)
			{
				if (hp_i == 0)
				{
					return;
				}
				g = RemoteGamer::get(PlayerHistory::player_history.at(hp_i--)->rid, true, RGU_BULK);
			}
			out_g = g;
		}
	};
}
