#pragma once

#include "CommandListSelect.hpp"

#include "AbstractEntity.hpp"
#include "CommandVehInvisibility.hpp"
#include "is_session.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandInvisibility : public CommandListSelect
	{
	public:
		explicit CommandInvisibility(CommandList* const parent)
			: CommandListSelect(parent, LOC("INVIS"), CMDNAMES("invisibility"), LOC("INVIS_H"), {
				{0, LOC("DOFF"), CMDNAMES("off")},
				{1, LOC("LOCVIS"), CMDNAMES("remote")},
				{2, LOC("ON"), CMDNAMES("on")}
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			CommandVehInvisibility::player_ped_invisiblity = value;
			if (value == 1)
			{
				registerPostTickEventHandler([this]
				{
					const bool on = (value == 1);
					if (is_session_started())
					{
						ENTITY::SET_ENTITY_VISIBLE(g_player_ped, !on, FALSE);
						NETWORK::SET_ENTITY_LOCALLY_VISIBLE(g_player_ped);
					}
					return on;
				});
			}
			else if (value == 2)
			{
				registerPostTickEventHandler([this]
				{
					const bool on = (value == 2);
					ENTITY::SET_ENTITY_VISIBLE(g_player_ped, !on, FALSE);
					return on;
				});
			}
		}
	};
}
