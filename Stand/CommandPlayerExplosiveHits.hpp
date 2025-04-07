#pragma once

#include "CommandListSelect.hpp"
#include "CommandOnPlayer.hpp"

#include "eExplosionTag.hpp"

namespace Stand
{
	class CommandPlayerExplosiveHits : public CommandOnPlayer<CommandListSelect>
	{
	public:
		explicit CommandPlayerExplosiveHits(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("EXPAMMO"), CMDNAMES_OBF("explosiveammo"), LOC("PLYEXPAMMO_H"), eExplosionTag_getListActionItems(true), EXP_TAG_DONTCARE, CMDFLAGS_LIST_SELECT, CMDFLAG_FEATURELIST_FINISHLIST)
		{
			PC_PTR->registerCommand(this);
		}

		void onChange(Click& click, long long prev_value) final
		{
			const auto value = this->value;
			if (value != EXP_TAG_DONTCARE)
			{
				registerScriptTickEventHandler([this, value]
				{
					HANDLER_CHECK(value == this->value);
					for (const auto& p : PP_PTR->getPlayers())
					{
						if (auto ped = p.getPed(); ped.isValid())
						{
							Vector3 pos;
							if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(ped, &pos))
							{
								FIRE::ADD_OWNED_EXPLOSION(ped, pos.x, pos.y, pos.z, (int)value, 1, true, false, 0);
							}
						}
					}
					HANDLER_END;
				});
			}
		}
	};
}