#pragma once

#include "CommandToggle.hpp"

#include "FiberPool.hpp"
#include "Hooking.hpp"
#include "netStatus.hpp"
#include "Script.hpp"

namespace Stand
{
	class CommandPlayerMagnet : public CommandToggle
	{
	private:
		bool job_busy = false;

	public:
		explicit CommandPlayerMagnet(CommandList* const parent)
			: CommandToggle(parent, LOC("PLYMGNT"), CMDNAMES("playermagnet"), LOC("PLYMGNT_H"), false, CMDFLAGS_TOGGLE | CMDFLAG_FEATURELIST_REGULAR)
		{
		}

		void onEnable(Click& click) final
		{
			if (!click.isRegularEdition())
			{
				return onChangeImplUnavailable(click);
			}

			if (job_busy)
			{
				FiberPool::queueJob([this]
				{
					while (job_busy)
					{
						Script::current()->yield();
					}

					g_hooking.player_magnet = true;
				});
			}
			else
			{
				g_hooking.player_magnet = true;
			}
		}

		void onDisable(Click& click) final
		{
			g_hooking.player_magnet = false;

			if (!job_busy)
			{
				job_busy = true;
				FiberPool::queueJob([this]
				{
					cleanup();
					job_busy = false;
				});
			}
		}

		static void cleanup()
		{
			for (const auto& id : g_hooking.session_advertisements)
			{
				rage::netStatus status{};
				if (OG(rage_rlScMatchmaking_Unadvertise)(g_player.getGamerInfo()->local_index, &id, &status))
				{
					status.waitUntilDone();
				}
			}

			g_hooking.session_advertisements.clear();
		}
	};
}