#pragma once

#include "CommandPlayerActionAggressive.hpp"

namespace Stand
{
	class CommandPlayerInfiniteRing : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerInfiniteRing(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("INFRING"), CMDNAMES("ring"))
		{
		}

		void onClick(Click& click) final
		{
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
			{
				ensureScriptThread(thread_context, [this]
				{
					auto id = AUDIO::GET_SOUND_ID();
					for (const auto& p : PP_PTR->getPlayers())
					{
						GtaThread::fromHash(ATSTRINGHASH("main_persistent"))->spoofAs([&] // needed to keep it truly infinite
						{
							AUDIO::PLAY_SOUND_FROM_ENTITY(id, "Remote_Ring", p.getPed(), "Phone_SoundSet_Michael", TRUE, FALSE);
						});
					}
					Script::current()->yield();
					*pointers::stop_sound_dont_network = 1;
					AUDIO::STOP_SOUND(id);
					*pointers::stop_sound_dont_network = 0;
					AUDIO::RELEASE_SOUND_ID(id);
				});
			});
		}
	};
}
