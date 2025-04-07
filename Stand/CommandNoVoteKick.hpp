#pragma once

#include "CommandListSelect.hpp"

#include "Gui.hpp"
#include "is_session.hpp"
#include "regular_event.hpp"
#include "ScriptGlobal.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandNoVoteKick : public CommandListSelect
	{
	public:
		explicit CommandNoVoteKick(CommandList* const parent)
			: CommandListSelect(parent, LOC("PTX_VK"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("votekicks")), NOLABEL, {
				{0, LOC("DOFF"), CMDNAMES("off")},
				{5, LOC("PTX_VK_SP"), CMDNAMES("sctv"), LOC("RCMD")},
				{2, LOC("PTX_VK_F2"), CMDNAMES("mission2")},
				{4, LOC("PTX_VK_F4"), CMDNAMES("mission4")},
				{1, LOC("PTX_VK_F1"), CMDNAMES("mission1")},
				{3, LOC("PTX_VK_F3"), CMDNAMES("mission3")},
			}, 0)
		{
		}

		static void onTickAny()
		{
			if (*ScriptGlobal(GLOBAL_VOTE_KICKED).at(g_player, 1).as<int*>() == 1)
			{
				Util::toast(LOC("PTX_VK_T"));
			}
			*ScriptGlobal(GLOBAL_VOTE_KICKED).at(g_player, 1).as<int*>() = 0;
		}

		static void onTick1()
		{
			if (is_session_started())
			{
				*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_BITSET1).as<int*>() |= (1 << GLOBAL_PLAYERREMOTE_BITSET1_NOVOTEKICK1);
				onTickAny();
			}
		}

		static void onTick2()
		{
			if (is_session_started())
			{
				*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_BITSET1).as<int*>() |= (1 << GLOBAL_PLAYERREMOTE_BITSET1_NOVOTEKICK2);
				onTickAny();
			}
		}

		static void onTick3()
		{
			if (is_session_started())
			{
				*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_FLAGS).as<int*>() |= (1 << GLOBAL_ORG_FLAGS_NOVOTEKICK3);
				onTickAny();
			}
		}

		static void onTick4()
		{
			if (is_session_started())
			{
				*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_FLAGS).as<int*>() |= (1 << GLOBAL_ORG_FLAGS_NOVOTEKICK4);
				onTickAny();
			}
		}

		static void onTick5()
		{
			if (is_session_started())
			{
				*ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(g_player, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_SPECTATING).as<BOOL*>() = TRUE;
				onTickAny();
			}
		}

		void onChange(Click& click, long long prev_value) final
		{
			switch (prev_value)
			{
			case 1:
				reScriptTickEvent::unregisterHandler(&onTick1);
				ensureScriptThread([]
				{
					*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_BITSET1).as<int*>() &= ~(1 << GLOBAL_PLAYERREMOTE_BITSET1_NOVOTEKICK1);
				});
				break;

			case 2:
				reScriptTickEvent::unregisterHandler(&onTick2);
				ensureScriptThread([]
				{
					*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_BITSET1).as<int*>() &= ~(1 << GLOBAL_PLAYERREMOTE_BITSET1_NOVOTEKICK2);
				});
				break;

			case 3:
				reScriptTickEvent::unregisterHandler(&onTick3);
				ensureScriptThread([]
				{
					*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_FLAGS).as<int*>() &= ~(1 << GLOBAL_ORG_FLAGS_NOVOTEKICK3);
				});
				break;

			case 4:
				reScriptTickEvent::unregisterHandler(&onTick4);
				ensureScriptThread([]
				{
					*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_FLAGS).as<int*>() &= ~(1 << GLOBAL_ORG_FLAGS_NOVOTEKICK4);
				});
				break;

			case 5:
				reScriptTickEvent::unregisterHandler(&onTick5);
				ensureScriptThread([]
				{
					*ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(g_player, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_SPECTATING).as<BOOL*>() = FALSE;
				});
				break;
			}
			switch (value)
			{
			case 1:
				reScriptTickEvent::registerHandler(&onTick1);
				break;

			case 2:
				reScriptTickEvent::registerHandler(&onTick2);
				break;

			case 3:
				reScriptTickEvent::registerHandler(&onTick3);
				break;

			case 4:
				reScriptTickEvent::registerHandler(&onTick4);
				break;

			case 5:
				reScriptTickEvent::registerHandler(&onTick5);
				break;
			}
		}
	};
}
