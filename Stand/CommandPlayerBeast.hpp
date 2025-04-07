#pragma once

#include "CommandPlayerAction.hpp"

#include "ScriptLocal.hpp"
#include "SessionScriptMgr.hpp"

namespace Stand
{
	class CommandPlayerBeast : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerBeast(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("PLYBEAST"), { CMDNAME("beast") }, NOLABEL, COMMANDPERM_NEUTRAL, CMDFLAGS_ACTION | CMDFLAG_FEATURELIST_ULTIMATE)
		{
		}

		void onClick(Click& click) final
		{
			if (!SessionScriptMgr::startPrecheck(click))
			{
				return;
			}
			ensureYieldableScriptThread(click, [this]
			{
				const auto target = PP_PTR_SINGLE->getPlayer();
				auto thread = GtaThread::fromHash(ATSTRINGHASH("am_hunt_the_beast"));
				if (thread == nullptr)
				{
					SessionScriptMgr::start(ATSTRINGHASH("am_hunt_the_beast"));
					for (uint8_t i = 0; i != 255; ++i)
					{
						Script::current()->yield(100);
						thread = GtaThread::fromHash(ATSTRINGHASH("am_hunt_the_beast"));
						if (thread != nullptr
							&& thread->getNetComponent() != nullptr
							)
						{
							break;
						}
					}
					for (uint8_t i = 0; i != 255; ++i)
					{
						Script::current()->yield(100);
						thread = GtaThread::fromHash(ATSTRINGHASH("am_hunt_the_beast"));
						if (thread == nullptr)
						{
							break;
						}
						auto netcomp = thread->getNetComponent();
						if (netcomp == nullptr
							|| netcomp->IsPlayerAParticipant(target)
							)
						{
							break;
						}
					}
				}
				if (thread != nullptr)
				{
					*ScriptLocal(thread, LOCAL_HTB_SHDATA + 1).as<int*>() = 2; // state
					*ScriptLocal(thread, LOCAL_HTB_SHDATA + 1 + 2).as<int*>() = NETWORK::GET_NETWORK_TIME(); // duration timer
					//*ScriptLocal(thread, LOCAL_HTB_SHDATA + 1 + 2).as<int*>() = 0x7fffffff; // duration timer
					*ScriptLocal(thread, LOCAL_HTB_BEAST).as<int*>() = target;
					*ScriptLocal(thread, LOCAL_HTB_SHDATA + 1 + 7).as<int*>() = target;
				}
			});
		}
	};
}
