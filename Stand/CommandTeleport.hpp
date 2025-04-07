#pragma once

#include "CommandListPlaces.hpp"

#include "AbstractPlayer.hpp"
#include "TpUtil.hpp"

namespace Stand
{
	class CommandTeleport : public CommandListPlaces
	{
	public:
		explicit CommandTeleport(CommandList* const parent)
			: CommandListPlaces(parent, LOC("TPS"), { CMDNAME("tp"), CMDNAME("teleport") })
		{
			CommandPhysical::collapse_command_names.emplace(L"tp");
			CommandPhysical::collapse_command_names.emplace(L"teleport");
		}

		void populate()
		{
			CommandListPlaces::populate(true);
		}

		[[nodiscard]] bool supportsUsageByOthers() const noexcept final
		{
			return true;
		}

		[[nodiscard]] std::vector<CommandName> decorateCommandNames(const std::vector<CommandName>& command_names) const
		{
			return combineCommandNames(std::vector<CommandName>{ CMDNAME("tp"), CMDNAME("teleport") }, command_names);
		}

		void processPop(Click& click, const PointOfPresence& pop) const final
		{
			const auto p = click.getEffectiveIssuer();

			if (p == g_player)
			{
				if (pop.veh != INVALID_GUID)
				{
					TpUtil::teleport_to_veh(pop.veh);
				}
				else if (pop.blip != INVALID_BLIP_ID)
				{
					TpUtil::teleport_to_blip(pop.blip);
				}
				else
				{
					TpUtil::teleportWithRedirects(pop.pos);
				}
			}
			else
			{
				p.fiberTeleport(TpUtil::followRedirects(pop.pos, true));
			}
		}
	};
}
